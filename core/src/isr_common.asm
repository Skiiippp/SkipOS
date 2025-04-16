global isr_error
global isr_nonerror

extern isr_helper

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

; at start, has old rdi, rsi, rax on the stack, in that order
; rdi (1st arg) is isr index, rsi (2nd arg) is error code if applicable
isr_general:
    push rcx
    push rdx
    push r8
    push r9
    push r10
    push r11

    call isr_helper

    pop r11
    pop r10
    pop r9
    pop r8
    pop rdx
    pop rcx
    pop rax
    pop rsi
    pop rdi

    iretq
