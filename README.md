# SkipOS: CPE454 OS Project

### Completed Milestones:
- M01: boot
- M02: printk
- M03: keyboard
- M04: interrupts
- M05: serial
- M06: page frame allocator
- M07: virtual page allocator
- MO8: heap allocator (kmalloc)

### Tech Debt
- Optimize scan code search with a LUT
- Figure out why getting eroneous keyboard IRQ right after enabling IRQs
- One kmalloc() is implemented, remove unfortunate global vars
- Virtual page allocator currently doesn't keep track of freed virtual addresses on heap (huge address space)