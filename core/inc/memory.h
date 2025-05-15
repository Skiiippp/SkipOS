#pragma once

#include "../inc/common.h"

#include <stddef.h>

#define PAGE_SIZE 0x1000

/**
 * BEGIN Page frame allocator stuff
 */
// Returns system's memory size
size_t MMU_init_pf(const u8 *mb_tags_ptr);
void *MMU_pf_alloc();
void MMU_pf_free(void *pf);
/**
 * END Page frame allocator stuff
 */

/**
 * BEGIN Virtual page allocator stuff
 */
void MMU_init_vp();
/**
 * END Virtual page allocator stuff
 */