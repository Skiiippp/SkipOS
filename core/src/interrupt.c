#include "../inc/interrupt.h"

#include "../inc/common.h"
#include "../inc/assert.h"
#include "../inc/pic.h"
#include "../inc/printk.h"

#include <stddef.h>

/**
 * BEGIN PRIVATE
 */

#define IDT_ENTRY_CNT 256
#define ISR_CNT IDT_ENTRY_CNT
#define IRQ_CNT ISR_CNT

#define EXTERNAL_INT_BASE 0x20
#define EXTERNAL_INT_CNT 16 // Includes the line on first PIC used for cascading
#define EXTERNAL_INT_MAX (EXTERNAL_INT_BASE + EXTERNAL_INT_CNT - 1)

// rflags 
#define RFLAGS_INTR_ENABLED_MSK (1 << 9)

// Offset into GDT for selctor - this is 8, see boot.asm file
#define GDT_OFFSET_KERNEL_CODE 0x08

// Thanks, OSDevWiki!
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

static void idt_set_descriptor(u8 idt_index, void *isr, u8 attributes);
 
static void idt_populate();

static void idt_load_idtr();

static void irqh_populate_table();

static void def_irq_handler(u8 isq_index, u32 error, void *arg);

static u64 get_rflags();

/**
 * END PRIVATE
 */

void IRQ_init()
{
    idt_populate();

    idt_load_idtr();

    irqh_populate_table();

    PIC_remap(EXTERNAL_INT_BASE);

    PIC_disable_all_pic_irqs();
}

void IRQ_start()
{
    STI;
}

bool IRQ_are_interrupts_enabled()
{
    return (get_rflags() & RFLAGS_INTR_ENABLED_MSK);
}

void IRQ_set_handler(u8 irq_num, irq_handler_t handler, void *arg)
{
    irq_table[irq_num].handler = handler;
    irq_table[irq_num].arg = arg;
}

void IRQ_enable_index(u8 irq_index)
{
    assert(irq_index >= EXTERNAL_INT_BASE && irq_index <= EXTERNAL_INT_MAX);    // Ensure valid index

    u8 curr_mask, new_irq_mask;
    u8 pic_line_index;
    u8 (* fp_PIC_get_mask)();
    void (* fp_PIC_set_mask)(u8);
    
    if (irq_index - EXTERNAL_INT_BASE < LINES_PER_PIC)
    {
        fp_PIC_get_mask = PIC_get_pic1_mask;
        fp_PIC_set_mask = PIC_set_pic1_mask;
        pic_line_index = irq_index - EXTERNAL_INT_BASE;
        assert(pic_line_index != 2);   // Ensure not trying to mess with cascade
    }
    else
    {
        fp_PIC_get_mask = PIC_get_pic2_mask;
        fp_PIC_set_mask = PIC_set_pic2_mask;
        pic_line_index = irq_index - EXTERNAL_INT_BASE - LINES_PER_PIC;
    }

    curr_mask = fp_PIC_get_mask();
    new_irq_mask = ~(1 << pic_line_index);
    if (curr_mask & ~new_irq_mask)  // Low means enabled!
    {
        curr_mask &= new_irq_mask;
        fp_PIC_set_mask(curr_mask);
    }
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

    CLI;
    asm volatile ("hlt");
}

u64 get_rflags()
{
    u64 rflags;

    asm volatile ("pushfq" ::: "memory");
    asm volatile ("pop %0" : "=r"(rflags) :: "memory");

    return rflags;
}

// Called in isr_common.asm
void isr_helper(u8 irq_num, u32 error)
{
    irq_table[irq_num].handler(irq_num, error, irq_table[irq_num].arg);
}