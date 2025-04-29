#pragma once

#include "../inc/common.h"

/**
 * NOTE: On interrupt numbering:
 * Interrupts should be viewed with respect to their numberings in the IDT. Interrupts on the PICs will be mapped from 0x20-0x2F. 
 */

// Disable maskable interrupts
#define CLI asm volatile ("cli")

// Enable maskable interrupts
#define STI asm volatile ("sti")

// Safe for nested - DANGER: BE CAREFUL WITH SCOPE (i.e, must wrap this macro in braces after an if)!
#define NESTED_SAFE_CLI                     \
    bool enable_ints = false;               \
    do                                      \
    {                                       \
        if (IRQ_are_interrupts_enabled())   \
        {                                   \
            enable_ints = true;             \
            CLI;                            \
        }                                   \
    }                                       \
    while (0)                               \

#define NESTED_SAFE_STI     \
    do                      \
    {                       \
        if (enable_ints)    \
        {                   \
            STI;            \
        }                   \
    }                       \
    while (0)               \


extern void IRQ_init(); // Run before anything else that registers IRQs
extern void IRQ_start();    // Run after anything else that registers IRQs
extern bool IRQ_are_interrupts_enabled();
extern void IRQ_enable_index(u8 irq_index);
extern void IRQ_disable_index(u8 irq_index);
extern bool IRQ_is_enabled(u8 irq_index);
extern void IRQ_end_of_interrupt(u8 irq_index);

typedef void (*irq_handler_t)(u8, u32, void *);
extern void IRQ_set_handler(u8 irq_index, irq_handler_t handler, void *arg);
