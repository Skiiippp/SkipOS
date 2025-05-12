#include "../inc/gdt.h"

#include "../inc/common.h"
#include "../inc/printk.h"
#include "../inc/assert.h"
#include "../inc/string.h"

/**
 * BEGIN PRIVATE
 */

#define EXT_INT_STACK_SIZE 4096
static u8 gp_stack[EXT_INT_STACK_SIZE];
static u8 df_stack[EXT_INT_STACK_SIZE];
static u8 pf_stack[EXT_INT_STACK_SIZE];
static u8 *gp_stack_base = &gp_stack[EXT_INT_STACK_SIZE];
static u8 *df_stack_base = &df_stack[EXT_INT_STACK_SIZE];
static u8 *pf_stack_base = &pf_stack[EXT_INT_STACK_SIZE];

#define GDT_NUM_ENTIES 4    // 1st zero entry, 2nd kern code desc, 3rd and 4th tss desc

#define KCODE_DESC_ACCESS 0b10011111
#define KCODE_DESC_FLAGS 0b0010

#define TSS_DESC_ACCESS 0b10001001
#define TSS_DESC_FLAGS 0b0010

typedef struct
{
    u16 limit_low;
    u16 base_low;   
    u8 base_mid;
    u8 access_byte;
    u8 limit_high: 4;
    u8 flags: 4;
    u8 base_high;
} __attribute__((packed)) gdt_usr_desc_t;   // kern code and data desc

typedef struct
{
    u16 limit_low;
    u16 base_low;
    u8 base_mid;
    u8 access_byte;
    u8 limit_high: 4;
    u8 flags: 4;
    u8 base_high;
    u32 base_super_high;
    u32 reserved;
} __attribute__((packed)) gdt_sys_desc_t;   // tss desc

typedef struct
{
    u16 size;
    u64 offset;
} __attribute__((packed)) gdt_ptr_t;

typedef struct
{
    u32 res0;

    u64 rsp0;
    u64 rsp1;
    u64 rsp2;

    u64 res1;

    u64 ist1;
    u64 ist2;
    u64 ist3;
    u64 ist4;
    u64 ist5;
    u64 ist6;
    u64 ist7;

    u64 res2;
    u16 res3;

    u16 iobase;
} __attribute__((packed)) tss_t;

static gdt_usr_desc_t gdt[GDT_NUM_ENTIES];

static gdt_ptr_t gdt_ptr;

static tss_t tss;

// In 64 bit mode, don't care about limit and base (for kernel code and data)
static void set_gdt_usr_desc(gdt_usr_desc_t *gdt_usr_desc_ptr, u8 access_byte, u8 flags);

static void set_gdt_sys_desc(gdt_sys_desc_t *gdt_sys_desc_ptr, u8 access_byte, u8 flags,  u64 base, u32 limit);

/**
 * END PRIVATE
 */

void GDT_init(void)
{
    assert(sizeof(gdt_usr_desc_t) == 8);
    assert(sizeof(gdt) == GDT_NUM_ENTIES * sizeof(gdt_usr_desc_t));
    assert(sizeof(tss_t) == 104);

    const u16 tss_offset = (uintptr_t)&gdt[2] - (uintptr_t)&gdt[0];

    gdt_ptr.size = sizeof(gdt) - 1;
    gdt_ptr.offset = (u64)gdt; 

    set_gdt_usr_desc(&gdt[1], KCODE_DESC_ACCESS, KCODE_DESC_FLAGS);
    set_gdt_sys_desc((gdt_sys_desc_t *)&gdt[2], TSS_DESC_ACCESS, TSS_DESC_FLAGS, (u64)&tss, sizeof(tss));

    memset(&tss, 0, sizeof(tss));
    tss.ist1 = (u64)gp_stack_base;
    tss.ist2 = (u64)df_stack_base;
    tss.ist3 = (u64)pf_stack_base;

    asm volatile ("lgdt %0" :: "m"(gdt_ptr));
    asm volatile ("ltr %0" :: "r"(tss_offset));
}

void set_gdt_usr_desc(gdt_usr_desc_t *gdt_usr_desc_ptr, u8 access_byte, u8 flags)
{
    assert(gdt_usr_desc_ptr);

    memset(gdt_usr_desc_ptr, 0, sizeof(*gdt_usr_desc_ptr));

    gdt_usr_desc_ptr->access_byte = access_byte;
    gdt_usr_desc_ptr->flags = flags & 0xF;
}

void set_gdt_sys_desc(gdt_sys_desc_t *gdt_sys_desc_ptr, u8 access_byte, u8 flags, u64 base, u32 limit)
{
    assert(gdt_sys_desc_ptr);

    memset(gdt_sys_desc_ptr, 0, sizeof(*gdt_sys_desc_ptr));

    gdt_sys_desc_ptr->access_byte = access_byte;
    gdt_sys_desc_ptr->flags = flags & 0xF;

    gdt_sys_desc_ptr->limit_low = limit & 0xFFFF;
    gdt_sys_desc_ptr->limit_high = (limit >> 16) & 0xF;

    gdt_sys_desc_ptr->base_low = base & 0xFFFF;
    gdt_sys_desc_ptr->base_mid = (base >> 16) & 0xFF;
    gdt_sys_desc_ptr->base_high = (base >> 24) & 0xFF;
    gdt_sys_desc_ptr->base_super_high = (base >> 32) & 0xFFFFFFFF;
}


