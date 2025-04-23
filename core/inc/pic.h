#pragma once

#include "../inc/common.h"

#define LINES_PER_PIC 8

extern void PIC_disable_all_pic_irqs();

// PIC interrupts will be remapped to (base) to (base + 15)
extern void PIC_remap(u8 base);

extern void PIC_set_pic1_mask(u8 mask);

extern void PIC_set_pic2_mask(u8 mask);

extern u8 PIC_get_pic1_mask();

extern u8 PIC_get_pic2_mask();

extern void PIC_send_pic1_eoi();

extern void PIC_send_pic2_eoi();