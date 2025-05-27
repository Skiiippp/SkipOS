#include "../inc/memory.h"

#include "../inc/common.h"

/**
 * BEGIN PRIVATE
 */


static u64 PML4T[1];

static u64 PDPT[512];

/**
 * END PRIVATE
 */


void MMU_init_vp()
{
    const u64 pml4t_base_addr = (u64)PML4T;
    const u64 pdpt_base_addr = (u64)PDPT;

    PML4T[0] = pdpt_base_addr; // Set base addr for next in chain
    PML4T[0] |= 0x1;    // Set present
    PML4T[0] |= (0x1 << 1); // Set writeable

    for (int i = 0; i < 512; i++)
    {
        PDPT[i] |= (0x1 << 7);  // Set 1GB pages
        PDPT[i] |= 0x1; // Set present
        PDPT[i] |= (0x1 << 1);  // Set writeable
    }

    asm volatile ("mov %0, %%cr3" :: "r" (pml4t_base_addr));
}