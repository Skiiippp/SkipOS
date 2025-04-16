global isr_error
global isr_nonerror

isr_error:
    cli
    hlt
    iretq

isr_nonerror:
    iretq
