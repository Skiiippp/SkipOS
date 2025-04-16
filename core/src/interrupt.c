#include "../inc/interrupt.h"

#include "../inc/common.h"
#include "../inc/assert.h"
#include "../inc/port_io.h"

/**
 * BEGIN PRIVATE
 */

#define PIC1_CMD_PORT 0x20
#define PIC1_DATA_PORT 0x21
#define PIC2_CMD_PORT 0xA0
#define PIC2_DATA_PORT 0xA1

#define IDT_ENTRY_CNT 256
#define ISR_CNT IDT_ENTRY_CNT

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

static idt_entry_t idt[IDT_ENTRY_CNT];

typedef struct {
    u16 limit;
    u64 base;
} __attribute__((packed)) idtr_t;

static idtr_t idtr;

extern void *isr_table[];

void idt_set_descriptor(u8 idt_index, void *isr, u8 attributes);
 
void idt_populate();

/**
 * END PRIVATE
 */

void IRQ_init()
{
    CLI;

    idt_populate();

    // Set up IDT and stuff
    idtr.base = (uintptr_t)&idt[0];
    idtr.limit = (uint16_t)sizeof(idt_entry_t) * IDT_ENTRY_CNT - 1;
    asm volatile ("lidt %0" : : "m"(idtr));

    // Mask PIC (for now)
    outb(0x21, 0xFF);
    outb(0xA0, 0xFF);

    STI;
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