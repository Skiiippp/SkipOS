# Build
CC=riscv64-unknown-elf-gcc
CFLAGS=-ffreestanding -nostartfiles -nostdlib -nodefaultlibs
CFLAGS+=-g -Wl,--gc-sections -mcmodel=medany
RUNTIME=../src/init/crt0.s
LINKER_SCRIPT=../src/lds/riscv64-virt.ld
KERNEL_IMAGE=kmain.bin

# QEMU
QEMU=qemu-system-riscv64
MACH=virt
MEM=128M
RUN=$(QEMU) -nographic -machine $(MACH) -m $(MEM)	
# nographic equivelent to -serial stdio and no qemu window
RUN+=-bios none -kernel $(KERNEL_IMAGE)

# QEMU (debug)
GDB_PORT=1234

all: kmain.bin

kmain.bin: uart.o syscon.o common.o panic.o print.o kmain.o
	$(CC) *.o $(RUNTIME) $(CFLAGS) -T $(LINKER_SCRIPT) -o $(KERNEL_IMAGE)

uart.o: ../src/drivers/uart/uart.c 
	$(CC) -c ../src/drivers/uart/uart.c $(CFLAGS) -o uart.o

syscon.o: ../src/drivers/syscon/syscon.c
	$(CC) -c ../src/drivers/syscon/syscon.c $(CFLAGS) -o syscon.o

common.o: ../src/kernel/common.c
	$(CC) -c ../src/kernel/common.c $(CFLAGS) -o common.o

panic.o: ../src/kernel/panic.c
	$(CC) -c ../src/kernel/panic.c $(CFLAGS) -o panic.o

print.o: ../src/kernel/print.c
	$(CC) -c ../src/kernel/print.c $(CFLAGS) -o print.o

kmain.o: ../src/init/kmain.c
	$(CC) -c ../src/init/kmain.c $(CFLAGS) -o kmain.o



run: all
	$(RUN)

debug: all
	$(RUN) -gdb tcp::$(GDB_PORT) -S

clean:
	rm -vf *.o
	rm -vf $(KERNEL_IMAGE)


