#include "../inc/memory.h"

#include "../inc/common.h"
#include "../inc/assert.h"
#include "../inc/string.h"
#include "../inc/interrupt.h"

#include <stddef.h>

/**
 * BEGIN PRIVATE
 */

#define KERN_HEAP_BASE 0x10000000000

#define ENTIRES_PER_TABLE 512

#define PF_INT_NUM 0xE

#define PRESENT_MASK 0x1
#define WRITEABLE_MASK (0x1 << 1)
#define DEMAND_MASK (0x1 << 9)

#define PAGE_SIZE_1GB_MSK (0x1 << 7)

typedef enum
{
    PHYSICAL = 0,
    L1 = 1,
    L2 = 2,
    L3 = 3,
    L4 = 4
} pt_level_t;

typedef struct
{
    u64 *l4_table;
} __attribute__((packed)) page_table_t;

static page_table_t page_table;

static uintptr_t kern_heap_top;

static void setup_identity_paging(page_table_t *page_table_ptr);

static uintptr_t __attribute__((unused)) get_physical_addr(page_table_t *page_table_ptr, uintptr_t virtual_addr);

/**
 * @brief If alloc_on_demand==true, don't actually allocate the page, but set bit 9 (which is avaliable). Then page fault handler will actuall alloc page when used.
 */
static void alloc_virtual_page(page_table_t *page_table_ptr, uintptr_t virtual_addr, bool alloc_on_demand);

/**
 * @brief Gets the lowest present page in the page table for the virtual address. Returns ptr to relevent entry. pt_level_ptr gives pt level of returned entry ptr.
 */
static u64 *get_lowest_present_page(page_table_t *page_table_ptr, uintptr_t virtual_addr, pt_level_t *pt_level_ptr);

static bool is_next_page_present(u64 entry);

// Page fault ISR
static void page_fault_handler(u8 irq_index, u32 error, void *arg);

static void __attribute__((unused)) virt_test();

/**
 * END PRIVATE
 */

void MMU_init_vp()
{
    page_table.l4_table = (u64 *)MMU_pf_alloc();
    memset(page_table.l4_table, 0, PAGE_SIZE);

    setup_identity_paging(&page_table);
    asm volatile ("mov %%cr3, %0" :: "r" ((u64)page_table.l4_table));

    IRQ_set_handler(PF_INT_NUM, page_fault_handler, NULL);

    kern_heap_top = KERN_HEAP_BASE;
}

void *MMU_alloc_page()
{
    return MMU_alloc_pages(1);
}

void *MMU_alloc_pages(size_t num)
{
    void *ret = (void *)kern_heap_top;

    for(size_t i = 0; i < num; i++)
    {
        alloc_virtual_page(&page_table, kern_heap_top, true);
        kern_heap_top += PAGE_SIZE;
    }

    return ret;
}

void MMU_free_page(void *vp)
{
    MMU_free_pages(vp, 1);
}

void MMU_free_pages(void *vp, size_t num)
{
    assert((uintptr_t)vp % PAGE_SIZE == 0);
    assert(num > 0);

    uintptr_t next_to_free = (uintptr_t)vp;
    u64 *lowest_entry_ptr;
    pt_level_t level;

    for(size_t i = 0; i < num; i++)
    {
        lowest_entry_ptr = get_lowest_present_page(&page_table, next_to_free, &level);
        assert(level == PHYSICAL || level == L1);
        assert(level == PHYSICAL);

        if(level == PHYSICAL)
        {
            *lowest_entry_ptr &= ~(PRESENT_MASK | WRITEABLE_MASK);
            printk("FREE PHYSICAL: %p\n", (void *)lowest_entry_ptr);
            MMU_pf_free((void *)lowest_entry_ptr);
        }
        else if(level == L1)
        {
            assert(*lowest_entry_ptr & DEMAND_MASK);
            assert(!(*lowest_entry_ptr & (PRESENT_MASK | DEMAND_MASK)));
            *lowest_entry_ptr &= ~DEMAND_MASK;
        }
        else
        {
            assert(false);
        }

        next_to_free += PAGE_SIZE;
    }
}

void virt_test()
{
    u8 *test;

    test = (u8 *)MMU_alloc_pages(2);

    *test = 0xAA;

    MMU_free_pages(test, 2);
}

void setup_identity_paging(page_table_t *page_table_ptr)
{
    u64 *l3_identity_table = MMU_pf_alloc();
    memset(l3_identity_table, 0, PAGE_SIZE);

    page_table_ptr->l4_table[0] = (u64)l3_identity_table; // Set base addr for next in chain
    page_table_ptr->l4_table[0] |= (PRESENT_MASK | WRITEABLE_MASK);    // Set present, writeable

    for (int i = 0; i < ENTIRES_PER_TABLE; i++)
    {
        l3_identity_table[i] = (PRESENT_MASK | WRITEABLE_MASK | PAGE_SIZE_1GB_MSK); // 1GB pages, present, writeable
    }
}

uintptr_t get_physical_addr(page_table_t *page_table_ptr, uintptr_t virtual_addr)
{
    assert(page_table_ptr);
    assert(virtual_addr % PAGE_SIZE == 0);

    pt_level_t pt_level;
    uintptr_t physical_addr;

    physical_addr = (uintptr_t)get_lowest_present_page(page_table_ptr, virtual_addr, &pt_level);
    if(pt_level != PHYSICAL)
    {
        return 0x0;
    }

    return (uintptr_t)physical_addr;
}

void alloc_virtual_page(page_table_t *page_table_ptr, uintptr_t virtual_addr, bool alloc_on_demand)
{
    assert(page_table_ptr);
    assert(virtual_addr % PAGE_SIZE == 0);

    u64 *lowest_entry_ptr;
    pt_level_t pt_level;
    pt_level_t lowest;

    lowest_entry_ptr = get_lowest_present_page(page_table_ptr, virtual_addr, &pt_level);
    assert(pt_level != PHYSICAL);

    if (alloc_on_demand)
    {
        lowest = L1;
    }
    else
    {
        lowest = PHYSICAL;
    }

    while(pt_level != lowest)
    {
        // Could def be slightly optimized, but this is simple
        u64 *new_page = (u64 *)MMU_pf_alloc();
        memset(new_page, 0, PAGE_SIZE);
        *lowest_entry_ptr = (u64)new_page;
        *lowest_entry_ptr |= WRITEABLE_MASK | PRESENT_MASK;
        lowest_entry_ptr = get_lowest_present_page(page_table_ptr, virtual_addr, &pt_level);
    }

    if (alloc_on_demand)
    {
        assert(pt_level == L1);
        assert(!(*lowest_entry_ptr & DEMAND_MASK));

        *lowest_entry_ptr |= DEMAND_MASK;
    }
}

u64 *get_lowest_present_page(page_table_t *page_table_ptr, uintptr_t virtual_addr, pt_level_t *pt_level_ptr)
{
    assert(virtual_addr % PAGE_SIZE == 0);
    assert(pt_level_ptr);
    assert(page_table_ptr);

    u16 l4_index, l3_index, l2_index, l1_index;

    l4_index = (virtual_addr >> 39) & 0x1FF;
    l3_index = (virtual_addr >> 30) & 0x1FF;
    l2_index = (virtual_addr >> 21) & 0x1FF;
    l1_index = (virtual_addr >> 12) & 0x1FF;

    // Check if physical - 1GB pages
    if (l4_index == 0)
    {
        *pt_level_ptr = PHYSICAL;
        return (u64 *)virtual_addr;
    }

    u64 *l4_table = page_table_ptr->l4_table;
    u64 l4_entry = l4_table[l4_index];
    if (!is_next_page_present(l4_entry))
    {
        *pt_level_ptr = L4;
        return &l4_table[l4_index];
    }

    u64 *l3_table = (u64 *)(l4_entry & ~(0xFFF));
    u64 l3_entry = l3_table[l3_index];
    if (!is_next_page_present(l3_entry))
    {
        *pt_level_ptr = L3;
        return &l3_table[l3_index];
    }

    u64 *l2_table = (u64 *)(l3_entry & ~(0xFFF));
    u64 l2_entry = l2_table[l2_index];
    if(!is_next_page_present(l2_entry))
    {
        *pt_level_ptr = L2;
        return &l2_table[l2_index];
    }

    u64 *l1_table = (u64 *)(l2_entry & ~(0xFFF));
    u64 l1_entry = l1_table[l1_index];
    if (!is_next_page_present(l1_entry))
    {
        *pt_level_ptr = L1;
        return &l1_table[l1_index];
    }

    *pt_level_ptr = PHYSICAL;
    uintptr_t physical_frame_addr = (uintptr_t)(l1_entry & ~(0xFFF));
    return (u64 *)physical_frame_addr;
}

bool is_next_page_present(u64 entry)
{
    return (entry & PRESENT_MASK);
}

void page_fault_handler(u8 irq_index, u32 error, void *arg)
{
    (void)arg;

    assert(irq_index == PF_INT_NUM);

    uintptr_t fault_addr;
    page_table_t fault_page_table;
    pt_level_t level;
    u64 *lowest_entry_ptr, *new_page;

    asm volatile ("mov %0, %%cr3" : "=r"(fault_page_table.l4_table));
    asm volatile ("mov %0, %%cr2" : "=r"(fault_addr));    

    fault_addr -= fault_addr % PAGE_SIZE;

    lowest_entry_ptr = get_lowest_present_page(&fault_page_table, fault_addr, &level);
    assert(lowest_entry_ptr);

    if (!(*lowest_entry_ptr & DEMAND_MASK) || level != L1)
    {
        printk("Unhandled page fault.\nFault address: %p\nPage table location: %p\nError code: %u\n", (void *)fault_addr, fault_page_table.l4_table, error);
        HLT;
    }
    
    new_page = (u64 *)MMU_pf_alloc();
    printk("NEW PAGE PHYSICAL: %p\n", new_page);
    *lowest_entry_ptr = (u64)new_page;
    *lowest_entry_ptr |= WRITEABLE_MASK | PRESENT_MASK;
}
