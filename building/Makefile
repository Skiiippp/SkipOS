.DEFAULT_GOAL := all

RISCV_PREFIX=riscv64-unknown-linux-gnu
CC=$(RISCV_PREFIX)-gcc
LINKER_SCRIPT = riscv64-virt.ld 
CRT0 = crt0.s
KERNEL = ../src/kernel/kmain.c
TO_BE_BUILT = $(LINKER_SCRIPT) $(CRT0) $(KERNEL)

all: build run

build: $(TO_BE_BUILT)
	$(CC) -g -ffreestanding -O0 -Wl,--gc-sections \
    -nostartfiles -nostdlib -nodefaultlibs -Wl,-T,$(TO_BE_BUILT) 

run: build
	qemu-system-riscv64 -machine virt -m 128M -gdb tcp::1234 -S -kernel a.out -bios none

debug: 
	$(RISCV_PREFIX)-gdb --tui a.out

clean:
	rm -f a.out