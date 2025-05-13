#include "../inc/memory.h"
#include "../inc/printk.h"
#include "../inc/common.h"
#include "../inc/assert.h"

#include <stddef.h>

/**
 * BEGIN PRIVATE
 */

#define PAGE_SIZE 0x1000

#define MMAP_TAG_TYPE 6
#define END_TAG_TYPE 0
#define ELF_TAG_TYPE 9

#define VALID_MEM_INFO_ENT_TYPE 1

#define FIXED_HEADER_SIZE 8

#define MMAP_HEADER_OFFSET_TO_INFO_ARR 16

#define METADATA_ADDR 0x0

typedef struct
{
    u32 type;
    u32 size;
} __attribute__((packed)) basic_tag_t;

typedef struct
{
    basic_tag_t basic_tag_info;
    u32 mem_info_entry_size;    // Should be 24 (sizeof(mem_info_entry_t))
    u32 mem_info_entry_version; // Should be 0
} __attribute__((packed)) mmap_tag_t;

typedef struct
{
    u64 base_addr;
    u64 length;
    u32 type;
    u32 res;
} __attribute__((packed)) mem_info_entry_t;

typedef struct
{
    mmap_tag_t mmap_tag;
    mem_info_entry_t *mem_info_entry_arr;
    size_t num_mem_info_entries;
} mmap_info_t;

typedef struct
{
    basic_tag_t basic_tag_info;
    u32 num_sec_head_ents;
    u32 sec_head_ent_size; // Should be 64 (sizeof(sec_head_ent_t))
    u32 string_tbl_index;
} __attribute__((packed)) elf_tag_t;

typedef struct
{
    u32 sec_name_index;
    u32 type;
    u64 flags;
    u64 addr;
    u64 offset_in_file;
    u64 size;
    u32 tbl_index_link;
    u32 extra_info;
    u64 addr_align;
    u64 fixed_entry_size;
} __attribute__((packed)) sec_head_ent_t;

typedef struct
{
    elf_tag_t elf_tag;
    sec_head_ent_t *sec_head_ent_arr;
} elf_info_t;

typedef struct
{
    mem_info_entry_t *lower, *upper;
} usable_mem_info_ptrs_t;

/**
 * Based on my observations (printing shit), there are two type 1 mem sections. 
 * The first one is sort of small and starts at 0x0
 * The second one is much larger and starts at a higher offset
 * The ELF stuff puts all the object files one aft another in second sec.
 * 
 * Strat: Reserve the first page after 0x0 for my own metadata
 * Add all pages in the small first part to the page pool so we dont have to worry about that
 * If pool empty (head == tail == 0x0), start adding pages at a predetermined rate from the second, higher mem sec.
 */


// BEGIN LINKED LIST STUFF

typedef struct ll_elem_t ll_elem_t;

typedef struct ll_elem_t
{
    ll_elem_t *next, *prev;
} __attribute__((packed)) ll_elem_t;

typedef struct
{
    ll_elem_t head_elem;
    u64 base_addr; // For mem not yet in pool
    u64 size;   // For mem not yet in pool
} __attribute__((packed)) ll_head_info_t;

// NOTE: base_addr and size are not initialized!
static ll_elem_t *init_ll_head(u64 base);

//static bool is_empty(ll_elem *ll_head);

static void add_elem(ll_elem_t *head_ptr, ll_elem_t *new_ptr);

// END LINKED LIST STUFF

// Pointer should point to the start of the mmap tag structure in the tags 
static mmap_info_t get_mmap_info(const u8 *mmap_ptr);

// Pointer should be at start of elf tag structure
static elf_info_t get_elf_info(const u8 *elf_ptr);

// Return addr of tag, 0x0 if not found.
static const u8 *find_tag(const u8 *mb_tags_ptr, u32 tag_type);

// Return ptr to head info (should be 0x0)
static ll_head_info_t *init_pool(const mmap_info_t mmap_info, const elf_info_t elf_info);

static usable_mem_info_ptrs_t get_usable_mem_info_ptrs(const mmap_info_t mmap_info);

// Add the mem from the lower open region to the pool
static void add_lower_pages(ll_elem_t *const head_ptr, const u64 lower_base, const size_t lower_length);

static ll_head_info_t *init_ll_head_info(ll_elem_t *head_ptr, const elf_info_t *const elf_info_ptr, const mem_info_entry_t *const upper);

static u64 round_up_nearest_mult(u64 val, u64 base);

static u64 round_down_nearest_mult(u64 val, u64 base);

static void __attribute__((unused)) print_mem_infos(mem_info_entry_t *mem_info_entry_arr, size_t num_entries);

static void __attribute__((unused)) print_sec_head_ents(sec_head_ent_t *sec_head_ent_arr, size_t num_ents);

/**
 * END PRIVATE
 */

void MMU_init(const u8 *mb_tags_ptr)
{
    const u8 *mmap_ptr = find_tag(mb_tags_ptr, MMAP_TAG_TYPE);
    assert(mmap_ptr);

    mmap_info_t mmap_info = get_mmap_info(mmap_ptr);
    printk("Num mem info entries: %lu\n", mmap_info.num_mem_info_entries);
    print_mem_infos(mmap_info.mem_info_entry_arr, mmap_info.num_mem_info_entries);
    printk("\n");

    const u8 *elf_ptr = find_tag(mb_tags_ptr, ELF_TAG_TYPE);
    assert(elf_ptr);

    elf_info_t elf_info = get_elf_info(elf_ptr);
    printk("Num sec head entries; %u\n", elf_info.elf_tag.num_sec_head_ents);
    print_sec_head_ents(elf_info.sec_head_ent_arr, elf_info.elf_tag.num_sec_head_ents);

    init_pool(mmap_info, elf_info);
}

mmap_info_t get_mmap_info(const u8 *mmap_ptr)
{
    assert(mmap_ptr);

    mmap_info_t mmap_info = {.mmap_tag = *(mmap_tag_t *)mmap_ptr};
    assert(mmap_info.mmap_tag.basic_tag_info.type == MMAP_TAG_TYPE);
    assert(mmap_info.mmap_tag.mem_info_entry_size == sizeof(mem_info_entry_t) && sizeof(mem_info_entry_t) == 24);
    assert(mmap_info.mmap_tag.mem_info_entry_version == 0);

    mmap_info.mem_info_entry_arr = (mem_info_entry_t *)(mmap_ptr + sizeof(mmap_tag_t));
    
    const size_t arr_size_bytes = mmap_info.mmap_tag.basic_tag_info.size - sizeof(mmap_tag_t);
    assert(arr_size_bytes % sizeof(mem_info_entry_t) == 0);
    mmap_info.num_mem_info_entries = arr_size_bytes / sizeof(mem_info_entry_t);

    return mmap_info;
}

elf_info_t get_elf_info(const u8 *elf_ptr)
{
    assert(elf_ptr);

    elf_info_t elf_info = {.elf_tag = *(elf_tag_t *)elf_ptr};
    assert(elf_info.elf_tag.basic_tag_info.type == ELF_TAG_TYPE);
    assert(elf_info.elf_tag.sec_head_ent_size == sizeof(sec_head_ent_t) && sizeof(sec_head_ent_t) == 64);

    elf_info.sec_head_ent_arr = (sec_head_ent_t *)(elf_ptr + sizeof(elf_tag_t));

    assert(elf_info.elf_tag.num_sec_head_ents == ((elf_info.elf_tag.basic_tag_info.size - sizeof(elf_info.elf_tag))/sizeof(sec_head_ent_t)));

    return elf_info;
}

const u8 *find_tag(const u8 *mb_tags_ptr, u32 tag_type)
{
    u8 *curr_tags_ptr = (u8 *)mb_tags_ptr;
    basic_tag_t basic_tag;

    curr_tags_ptr += FIXED_HEADER_SIZE;

    while(*(u32 *)curr_tags_ptr != tag_type)
    {
        basic_tag = *(basic_tag_t *)curr_tags_ptr;
        curr_tags_ptr += round_up_nearest_mult(basic_tag.size, 8);
        if (basic_tag.type == END_TAG_TYPE)
        {
            return NULL;
        }
    }

    return curr_tags_ptr;
}

ll_head_info_t *init_pool(const mmap_info_t mmap_info, const elf_info_t elf_info)
{
    usable_mem_info_ptrs_t usable_mem_info_ptrs = get_usable_mem_info_ptrs(mmap_info);

    const size_t lower_length = round_down_nearest_mult(usable_mem_info_ptrs.lower->length, PAGE_SIZE);
    u64 lower_base = round_up_nearest_mult(usable_mem_info_ptrs.lower->base_addr, PAGE_SIZE);
    assert(lower_base == 0x0);

    //ll_head_info_t *head_info_ptr = init_ll_head(lower_base);
    ll_elem_t *head_ptr = init_ll_head(lower_base);

    add_lower_pages(head_ptr, lower_base, lower_length);

    ll_head_info_t *head_info_ptr = init_ll_head_info(head_ptr, &elf_info, usable_mem_info_ptrs.upper);

    printk("\t\t BASE: %lx\n", head_info_ptr->base_addr);
    printk("\t\t BASE + LEN: %lx\n", head_info_ptr->base_addr + head_info_ptr->size);

    return head_info_ptr;
}

usable_mem_info_ptrs_t get_usable_mem_info_ptrs(const mmap_info_t mmap_info)
{
    usable_mem_info_ptrs_t ret;
    mem_info_entry_t *curr_ent_ptr;
    size_t i = 0;
    size_t cnt = 0;

    while(i < mmap_info.num_mem_info_entries)
    {
        curr_ent_ptr = &mmap_info.mem_info_entry_arr[i];
        if (curr_ent_ptr->type == VALID_MEM_INFO_ENT_TYPE)
        {
            cnt += 1;
            if (curr_ent_ptr->base_addr == 0x0)
            {
                // Usable mem at 0x0
                ret.lower = curr_ent_ptr;
            }
            else
            {
                // Usable mem higher up
                ret.upper = curr_ent_ptr;
            }
        }
        i++;
    }
    assert(cnt == 2);

    return ret;
}

void add_lower_pages(ll_elem_t *const head_ptr, const u64 lower_base, const size_t lower_length)
{
    u64 curr_addr = lower_base + PAGE_SIZE;
    size_t cnt = 0;
    while(curr_addr < lower_length)
    {
        add_elem(head_ptr, (ll_elem_t *)curr_addr);
        curr_addr += PAGE_SIZE;
        cnt += 1;
    }
    assert(curr_addr == lower_length);
    assert(cnt == (lower_length / PAGE_SIZE) - 1);   // -1 for 1st page
}

ll_head_info_t *init_ll_head_info(ll_elem_t *head_ptr, const elf_info_t *const elf_info_ptr, const mem_info_entry_t *const upper)
{
    assert(head_ptr == 0x0);

    ll_head_info_t *head_info_ptr = (ll_head_info_t *)head_ptr;

     // Calculate the base for the new mem base on elf stuff
    // Go to the last section header, add size to addr to get new base

    assert(upper->base_addr == elf_info_ptr->sec_head_ent_arr[1].addr);  // Sec header 0 is at 0x0 with len 0, 1 is first upper

    const u32 final_sec_head_index = elf_info_ptr->elf_tag.num_sec_head_ents - 1;
    u64 upper_base = elf_info_ptr->sec_head_ent_arr[final_sec_head_index].addr + elf_info_ptr->sec_head_ent_arr[final_sec_head_index].size;

    upper_base = round_up_nearest_mult(upper_base, PAGE_SIZE);
    size_t upper_length = round_down_nearest_mult((upper->length + upper->base_addr), PAGE_SIZE) - upper_base;

    head_info_ptr->base_addr = upper_base;
    head_info_ptr->size = upper_length;

    return head_info_ptr;
}   


ll_elem_t *init_ll_head(u64 base)
{
    ll_elem_t *head_ptr = (ll_elem_t *)base;

    head_ptr->next = head_ptr;
    head_ptr->prev = head_ptr;

    return head_ptr;
}



void add_elem(ll_elem_t *head_ptr, ll_elem_t *new_ptr)
{
    new_ptr->next = head_ptr;
    new_ptr->prev = head_ptr->prev;
    head_ptr->prev->next = new_ptr;
    head_ptr->prev = new_ptr;
}


u64 round_up_nearest_mult(u64 val, u64 mult)
{
    if (val % mult != 0)
    {
        val += mult - (val % mult);
    }

    return val;
}

u64 round_down_nearest_mult(u64 val, u64 mult)
{
    return val - (val % mult);
}

void print_mem_infos(mem_info_entry_t *mem_info_entry_arr, size_t num_entries)
{
    for (size_t i = 0; i < num_entries; i++)
    {
        printk("\nBase Address: %lx\n", mem_info_entry_arr[i].base_addr);
        printk("Length: %lx\n", mem_info_entry_arr[i].length);
        printk("Base + Len: %lx\n", mem_info_entry_arr[i].base_addr + mem_info_entry_arr[i].length);
        printk("Type: %x\n", mem_info_entry_arr[i].type);
        if (mem_info_entry_arr[i].type == 1)
        {
            printk("\t\tNum pages: %lx\n", mem_info_entry_arr[i].length/PAGE_SIZE);
            printk("\t\tLength mod page size: %lx\n", mem_info_entry_arr[i].length % PAGE_SIZE);
        }
    }
}

void print_sec_head_ents(sec_head_ent_t *sec_head_ent_arr, size_t num_ents)
{
    for (size_t i = 0; i < num_ents; i++)
    {
        printk("\nBase Address: %lx\n", sec_head_ent_arr[i].addr);
        printk("Length: %lx\n", sec_head_ent_arr[i].size);
        printk("Addr of next: %lx\n", sec_head_ent_arr[i].addr + sec_head_ent_arr[i].size);
    }
}
