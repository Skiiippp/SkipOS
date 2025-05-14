#pragma once

#include "../inc/common.h"

#define PAGE_SIZE 0x1000

void MMU_init(const u8 *mb_tags_ptr);
void *MMU_pf_alloc();
void MMU_pf_free(void *pf);