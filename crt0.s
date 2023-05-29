.global _start
_start:
    .cfi_startproc
    la sp, __stack_top
    jal zero, main
    .cfi_endproc
    .end