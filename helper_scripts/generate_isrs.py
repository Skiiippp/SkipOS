NUM_ISRS = 256

# Index of isrs that return error codes
error_isrs = [8,10,11,12,13,14,17,30]

def gen_isrs(num_isrs):
    print("section .text")
    for i in range(num_isrs):
        print(f"global isr_{i}")
        print(f"isr_{i}:")
        print(f"    push rdi")  # Push current 1st arg to stack
        print(f"    mov rdi, {i}")  # Put isr index as new 1st arg
        if i in error_isrs:
            print(f"    jmp isr_error")
        else:
            print(f"    jmp isr_nonerror")
        print("")
    
def gen_isr_table(num_isrs):
    print("section .data")
    print("global isr_table")
    print("isr_table:")
    for i in range(num_isrs):
        print(f"    dq isr_{i}")

if __name__ == "__main__":
    print("extern isr_error")
    print("extern isr_nonerror")
    print("")

    gen_isrs(NUM_ISRS)
    print("")
    gen_isr_table(NUM_ISRS)

    
