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

// Bellow funcs are for kernel heap management, but not kmalloc itself
void *MMU_alloc_page();
void *MMU_alloc_pages(size_t num);
void MMU_free_page(void *vp);
void MMU_free_pages(void *vp, size_t num);
/**
 * END Virtual page allocator stuff
 */
