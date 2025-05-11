#include "../inc/interrupt.h"

#include "../inc/common.h"
#include "../inc/assert.h"
#include "../inc/pic.h"
#include "../inc/printk.h"
#include "../inc/gdt.h"

#include <stddef.h>

/**
 * BEGIN PRIVATE
 */

#define IRQ_CNT 256
#define IDT_ENTRY_CNT IRQ_CNT
#define ISR_CNT IRQ_CNT

#define EXTERNAL_INT_BASE 0x20
#define EXTERNAL_INT_CNT 16 // Includes the line on first PIC used for cascading
#define EXTERNAL_INT_MAX (EXTERNAL_INT_BASE + EXTERNAL_INT_CNT - 1)

#define DE_INT_NUM 0x0
#define DF_INT_NUM 0x8
#define GP_INT_NUM 0xD
#define PF_INT_NUM 0xE

#define PIC1_CASCADE_LINE 2

// rflags 
#define RFLAGS_INTR_ENABLED_MSK (1 << 9)

// Offset into GDT for selctor - this is 8, see boot.asm file
#define GDT_OFFSET_KERNEL_CODE 0x08

// From OSDev Wiki
typedef struct {
	u16 isr_low;
	u16 kernel_cs;
	u8 ist;
	u8 attributes;
	u16 isr_mid;
	u32 isr_high;
	u32 reserved;
} __attribute__((packed)) idt_entry_t;

typedef struct {
    u16 limit;
    u64 base;
} __attribute__((packed)) idtr_t;

typedef struct {
    void *arg;
    irq_handler_t handler;
} irq_info_t;

static idt_entry_t idt[IDT_ENTRY_CNT];

static idtr_t idtr;

extern void *isr_table[];

static irq_info_t irq_table[IRQ_CNT];

static u8 get_irq_mask(u8 irq_index);

static u8 get_pic_line_index(u8 irq_index);

static void set_irq_line_state(u8 irq_index, bool enabled);

static void enable_cascade();

static void idt_set_descriptor(u8 idt_index, void *isr, u8 attributes);
 
static void idt_populate();

static void idt_load_idtr();

static void irqh_populate_table();

static u64 get_rflags();

static void def_irq_handler(u8 irq_index, u32 error, void *arg);

static void div_zero_handler(u8 irq_index, u32 error, void *arg);

static void double_fault_handler(u8 irq_index, u32 error, void *arg);

static void page_fault_handler(u8 irq_index, u32 error, void *arg);

static void gen_proc_handler(u8 irq_index, u32 error, void *arg);

/**
 * END PRIVATE
 */

void IRQ_init()
{
    idt_populate();

    idt_load_idtr();

    irqh_populate_table();

    IRQ_set_handler(DF_INT_NUM, double_fault_handler, NULL);
    IRQ_set_handler(DE_INT_NUM, div_zero_handler, NULL);
    IRQ_set_handler(PF_INT_NUM, page_fault_handler, NULL);
    IRQ_set_handler(GP_INT_NUM, gen_proc_handler, NULL);

    PIC_remap(EXTERNAL_INT_BASE);

    PIC_disable_all_pic_irqs();

    enable_cascade();
}

void IRQ_start()
{
    STI;
}

bool IRQ_are_interrupts_enabled()
{
    u8 rflags = get_rflags();
    return (rflags & RFLAGS_INTR_ENABLED_MSK);
}

void IRQ_set_handler(u8 irq_num, irq_handler_t handler, void *arg)
{
    irq_table[irq_num].handler = handler;
    irq_table[irq_num].arg = arg;
}

void IRQ_enable_index(u8 irq_index)
{
    set_irq_line_state(irq_index, true);
}

void IRQ_disable_index(u8 irq_index)
{
    set_irq_line_state(irq_index, false);
}

bool IRQ_is_enabled(u8 irq_index)
{
    u8 curr_mask = get_irq_mask(irq_index);
    u8 pic_line_index = get_pic_line_index(irq_index);

    return !(curr_mask & (1 << pic_line_index));    // Low is enabled
}

void IRQ_end_of_interrupt(u8 irq_index)
{
    // Check if a PIC based interrupt
    if(irq_index < EXTERNAL_INT_BASE || irq_index > EXTERNAL_INT_MAX)
    {
        return;
    }

    if (irq_index >= LINES_PER_PIC)
    {
        PIC_send_pic2_eoi();
    }

    PIC_send_pic1_eoi();
}

u8 get_irq_mask(u8 irq_index)
{
    assert(irq_index >= EXTERNAL_INT_BASE && irq_index <= EXTERNAL_INT_MAX);

    u8 (* fp_PIC_get_mask)();
    
    if (irq_index - EXTERNAL_INT_BASE < LINES_PER_PIC)
    {
        fp_PIC_get_mask = PIC_get_pic1_mask;
    }
    else
    {
        fp_PIC_get_mask = PIC_get_pic2_mask;
    }

    return fp_PIC_get_mask();
}

u8 get_pic_line_index(u8 irq_index)
{
    assert(irq_index >= EXTERNAL_INT_BASE && irq_index <= EXTERNAL_INT_MAX);

    u8 pic_line_index;

    if (irq_index - EXTERNAL_INT_BASE < LINES_PER_PIC)
    {
        pic_line_index = irq_index - EXTERNAL_INT_BASE;
        assert(pic_line_index != 2);   // Ensure not trying to mess with cascade
    }
    else
    {
        pic_line_index = irq_index - EXTERNAL_INT_BASE - LINES_PER_PIC;
    }

    return pic_line_index;
}

void set_irq_line_state(u8 irq_index, bool enable)
{
    u8 curr_mask = 0;
    u8 pic_line_index = 0;
    void (* fp_PIC_set_mask)(u8);
    
    assert(irq_index >= EXTERNAL_INT_BASE && irq_index <= EXTERNAL_INT_MAX);    // Ensure valid index

    if (irq_index - EXTERNAL_INT_BASE < LINES_PER_PIC)
    {
        fp_PIC_set_mask = PIC_set_pic1_mask;
        assert(pic_line_index != 2);   // Ensure not trying to mess with cascade
    }
    else
    {
        fp_PIC_set_mask = PIC_set_pic2_mask;
    }

    curr_mask = get_irq_mask(irq_index);
    pic_line_index = get_pic_line_index(irq_index);
    curr_mask &= ~(1 << pic_line_index);    // Clear necesarry bit
    if (!enable)   // If disable, set bit (high = disabled). Otherwise no need
    {
        curr_mask |= (1 << pic_line_index);
    }

    fp_PIC_set_mask(curr_mask);
}

void enable_cascade()
{
    u8 curr_mask = PIC_get_pic1_mask();
    curr_mask &= ~(1 << PIC1_CASCADE_LINE);
    PIC_set_pic1_mask(curr_mask);
}

// From OSDev Wiki
void idt_set_descriptor(u8 idt_index, void *isr, u8 attributes)
{
    idt_entry_t *entry_ptr;
        
    entry_ptr = &idt[idt_index];
    entry_ptr->isr_low = (uintptr_t)isr & 0xFFFF;
    entry_ptr->kernel_cs = GDT_OFFSET_KERNEL_CODE;
    entry_ptr->ist = 0;
    entry_ptr->attributes = attributes;
    entry_ptr->isr_mid = ((uintptr_t)isr >> 16) & 0xFFFF;
    entry_ptr->isr_high = ((uintptr_t)isr >> 32) & 0xFFFFFFFF;
    entry_ptr->reserved = 0;
}

void idt_populate()
{
    for(int i = 0; i < IDT_ENTRY_CNT; i++)
    {
        idt_set_descriptor(i, isr_table[i], 0x8E);
    }

    idt[GP_INT_NUM].ist = 1;
    idt[DF_INT_NUM].ist = 2;
    idt[PF_INT_NUM].ist = 3;

}

void idt_load_idtr()
{
    idtr.base = (uintptr_t)&idt[0];
    idtr.limit = (uint16_t)sizeof(idt_entry_t) * IDT_ENTRY_CNT - 1;
    asm volatile ("lidt %0" : : "m"(idtr));
}

void irqh_populate_table()
{
    for (int i = 0; i < IRQ_CNT; i++)
    {
        IRQ_set_handler((u8)i, def_irq_handler, NULL);
    }
}

void def_irq_handler(u8 irq_num, u32 error, void *arg)
{
    (void)error;
    (void)arg;

    printk("Warning: Unknown IRQ 0x%x triggered.\n", irq_num);

    HLT;
}

u64 get_rflags()
{
    u64 rflags;

    asm volatile (
        "pushfq\n\t"
        "popq %0"
        : "=r"(rflags)
    );

    return rflags;
}

// Called in isr_common.asm
void isr_helper(u8 irq_num, u32 error)
{
    irq_table[irq_num].handler(irq_num, error, irq_table[irq_num].arg);
}

void double_fault_handler(u8 irq_index, u32 error, void *arg)
{
    (void)error;
    (void)arg;

    assert(irq_index == DF_INT_NUM);

    printk("Double fault occured.\n");

    HLT;
}

void div_zero_handler(u8 irq_index, u32 error, void *arg)
{
    (void)error;
    (void)arg;

    assert(irq_index == DE_INT_NUM);

    printk("Divide error occured.\n");

    HLT;
}

void page_fault_handler(u8 irq_index, u32 error, void *arg)
{
    (void)error;
    (void)arg;

    assert(irq_index = PF_INT_NUM);

    asm volatile ("int 14");

    printk("Page fault occured.\n");

    HLT;
}

void gen_proc_handler(u8 irq_index, u32 error, void *arg)
{
    (void)error;
    (void)arg;

    assert(irq_index == GP_INT_NUM);

    printk("General protection fault occured.\n");

    HLT;
}
