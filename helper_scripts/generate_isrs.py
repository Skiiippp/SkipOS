NUM_ISRS = 256


def gen_isrs(num_isrs):
    print("section .text")
    for i in range(num_isrs):
        print(f"global isr_{i}")
        print(f"isr_{i}:")
        print(f"    push {i}")
        print("    jmp isr_error")
        print("")
    
def gen_isr_table(num_isrs):
    print("section .data")
    print("global isr_table")
    print("isr_table:")
    for i in range(num_isrs):
        print(f"    dq isr_{i}")

if __name__ == "__main__":
    print("extern isr_error")
    print("extern isr_noerror")
    print("")

    gen_isrs(NUM_ISRS)
    print("")
    gen_isr_table(NUM_ISRS)

    
