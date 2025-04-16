global isr_error
global isr_nonerror

extern general_handler

isr_nonerror:
    push rsi    ; stack has rsi
    push rax    ; stack has rax
    jmp isr_general

isr_error:
    push rax    ; rax on stack
    push rsi    ; rsi on stack
    mov rsi, [rsp + 16]  ; put error code in rsi
    pop rax  ; pop old value of rsi in rax
    mov [rsp + 8], rax  ; put old rsi where error code was 
    jmp isr_general


; has old rdi, rsi, rax on the stack, in that order
; rdi (1st arg) is isr index, rsi (2nd arg) is error code if applicable
isr_general:
    call general_handler

    iretq
