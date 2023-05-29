OUTPUT_FORMAT("elf64-littleriscv", "elf64-littleriscv", "elf64-littleriscv")
OUTPUT_ARCH(riscv)
ENTRY(_start)

MEMORY
{
    RAM (rwx)   : ORIGIN = 0x80000000, LENGTH = 128M
}


SECTIONS 
{
    PROVIDE(__stack_top = ORIGIN(RAM) + LENGTH(RAM));  
}