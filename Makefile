.DEFAULT_GOAL := all

RISCV_PREFIX=riscv64-unknown-linux-gnu
CC=$(RISCV_PREFIX)-gcc
LINKER_SCRIPT = LinkerScript.ld 
CRT0 = crt0.s
MAIN = main.c
TO_BE_BUILT = $(LINKER_SCRIPT) $(CRT0) $(MAIN)

all: build run

build: $(TO_BE_BUILT)
	$(CC) -g -ffreestanding -O0 -Wl,--gc-sections \
    -nostartfiles -nostdlib -nodefaultlibs -Wl,-T,$(TO_BE_BUILT) 

run:
	qemu-system-riscv64 -machine virt -m 128M -gdb tcp::1234 -S -kernel a.out -bios none

debug:
	$(RISCV_PREFIX)-gdb --tui a.out

clean:
	rm -f a.out