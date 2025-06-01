#include "../inc/kmalloc.h"

#include "../inc/memory.h"
#include "../inc/assert.h"
#include "../inc/printk.h"

/**
 * BEGIN PRIVATE
 */

#define MAX_CHUNK_SIZE 2048

typedef struct __attribute__((packed)) FreeList
{
    struct FreeList *next;
} freelist_t; 

typedef struct
{
    const size_t max_size;
    size_t avail;
    freelist_t *head;
} kmalloc_pool_t;

typedef struct
{
    kmalloc_pool_t *pool;
    size_t size;
} __attribute__((packed)) kmalloc_header_t;

static kmalloc_pool_t pool_arr[] = {
    {.max_size = 32},
    {.max_size = 64},
    {.max_size = 128},
    {.max_size = 256},
    {.max_size = 512},
    {.max_size = 1024},
    {.max_size = 2048}
};

static kmalloc_pool_t *get_best_pool(size_t size);

static void add_page(kmalloc_pool_t *pool_ptr);

/**
 * END PRIVATE
 */

void kmalloc_init()
{
    const size_t pool_arr_size = sizeof(pool_arr)/sizeof(pool_arr[0]);

    for (size_t i = 0; i < pool_arr_size; i++)
    {
        pool_arr[i].avail = 0;
        add_page(&pool_arr[i]);
    }
}

void *kmalloc(size_t size)
{
    assert(size > 0);

    const size_t bytes_needed = size + sizeof(kmalloc_header_t);
    kmalloc_pool_t *pool_ptr = get_best_pool(bytes_needed);

    // Check if need to alloc big virt mem segment outside of blocks
    if (pool_ptr == NULL)
    {
        size_t new_page_cnt = (bytes_needed-1)/PAGE_SIZE + 1;
        kmalloc_header_t *new_page = (kmalloc_header_t *)MMU_alloc_pages(new_page_cnt);
        new_page->pool = NULL;
        new_page->size = new_page_cnt * PAGE_SIZE;
        return (void *)((uintptr_t)new_page + sizeof(kmalloc_header_t));
    }

    if (pool_ptr->avail == 0)
    {
        add_page(pool_ptr);
    }

    kmalloc_header_t *new_mem = (kmalloc_header_t *)pool_ptr->head;
    pool_ptr->head = pool_ptr->head->next;
    pool_ptr->avail--;

    new_mem->pool = pool_ptr;
    new_mem->size = bytes_needed;
    return (void *)((uintptr_t)new_mem + sizeof(kmalloc_header_t));
}

void kfree(void *ptr)
{
    assert(ptr);

    kmalloc_header_t *const old_mem = (kmalloc_header_t *)((uintptr_t)ptr - sizeof(kmalloc_header_t));
    const size_t size = old_mem->size;
    kmalloc_pool_t *const pool_ptr = old_mem->pool;

    printk("Pool ptr: %p\n", pool_ptr);

    if (pool_ptr == NULL)
    {
        MMU_free_pages((void *)old_mem, size/PAGE_SIZE);
        return;
    }

    // temp
    assert(false);

    freelist_t *const new_head = (freelist_t *)old_mem;
    new_head->next = pool_ptr->head;
    pool_ptr->head = new_head;
}

kmalloc_pool_t *get_best_pool(size_t size)
{
    const size_t pool_arr_size = sizeof(pool_arr)/sizeof(pool_arr[0]);

    for (size_t i = 0; i < pool_arr_size; i++)
    {
        if (size <= pool_arr[i].max_size)
        {
            return &pool_arr[i];
        }
    }

    return NULL;
}

void add_page(kmalloc_pool_t *pool_ptr)
{
    assert(pool_ptr);

    void *new_page = MMU_alloc_page();
    const size_t num_chunks = PAGE_SIZE/pool_ptr->max_size;
    freelist_t *old_head = pool_ptr->head;
    freelist_t *chunk = (freelist_t *)new_page;

    pool_ptr->head = chunk;

    for(size_t i = 0; i < num_chunks-1; i++)
    {
        chunk->next = (freelist_t *)((uintptr_t)chunk + pool_ptr->max_size);
        chunk = chunk->next;
    }

    chunk->next = old_head;
    pool_ptr->avail += num_chunks;
}


