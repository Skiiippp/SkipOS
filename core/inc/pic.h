#pragma once

#include "../inc/common.h"

#define LINES_PER_PIC 8

void PIC_disable_all_pic_irqs();

// PIC interrupts will be remapped to (base) to (base + 15)
void PIC_remap(u8 base);

void PIC_set_pic1_mask(u8 mask);

void PIC_set_pic2_mask(u8 mask);

u8 PIC_get_pic1_mask();

u8 PIC_get_pic2_mask();

void PIC_send_pic1_eoi();

void PIC_send_pic2_eoi();