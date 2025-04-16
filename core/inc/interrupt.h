#pragma once

#include "../inc/common.h"

// Disable maskable interrupts
#define CLI asm volatile ("cli")

// Enable maskable interrupts
#define STI asm volatile ("sti")

extern void IRQ_init();
//extern void IRQ_set_mask(int irq);
//extern void IRQ_clear_mask(int irq);
//extern int IRQ_get_mask(int IRQline);
//extern void IRQ_end_of_interrupt(int irq);

typedef void (*irq_handler_t)(u8, u32, void *);
extern void IRQ_set_handler(u8 irq_index, irq_handler_t handler, void *arg);
