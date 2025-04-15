KERNEL := build/kernel.bin
IMG := build/foo.img

LINKER_SCRIPT := core/boot/linker.ld
GRUB_CFG := core/boot/grub.cfg
ASM_SRC := $(wildcard core/boot/*.asm)
ASM_OBJ := $(patsubst core/boot/%.asm, build/%.o, $(ASM_SRC))
C_SRC := $(wildcard core/src/*.c)
C_OBJ := $(patsubst core/src/%.c, build/%.o, $(C_SRC))
UNIT_TEST_SRC := $(wildcard unit_test/src/*.c)
UNIT_TEST_OBJ := $(patsubst unit_test/src/%.c, build/%.o, $(UNIT_TEST_SRC))
SYSTEM_TEST_SRC := $(wildcard system_test/src/*.c)
SYSTEM_TEST_OBJ := $(patsubst system_test/src/%.c, build/%.o, $(SYSTEM_TEST_SRC))


LOOP_SCRIPT := helper_scripts/get_loopback.py
FST_LOOP := 0
SEC_LOOP := 0

.PHONY: all clean run img

all: $(KERNEL)

clean:
	rm -rf build

run: $(IMG)
	qemu-system-x86_64 -s -drive format=raw,file=$(IMG) -serial stdio

debug: $(IMG)
	gdb -x helper_scripts/gdb_commands

img: $(IMG)

$(IMG) : $(KERNEL) $(grub_cfg)
	$(eval FST_LOOP := $(shell python3 $(LOOP_SCRIPT)))
	$(eval SEC_LOOP := $(shell echo $$(($(FST_LOOP) + 1))))
	@dd if=/dev/zero of=$(IMG) bs=512 count=32768
	@parted $(IMG) mklabel msdos
	@parted $(IMG) mkpart primary ext2 2048s 30720s
	@parted $(IMG) set 1 boot on
	@sudo losetup /dev/loop$(FST_LOOP) $(IMG)
	@sudo losetup /dev/loop$(SEC_LOOP) $(IMG) -o 1048576
	@sudo mkfs.ext2 /dev/loop$(SEC_LOOP)
	@sudo mkdir -p /mnt/osfiles
	@sudo mount /dev/loop$(SEC_LOOP) /mnt/osfiles
	@sudo grub-install --root-directory=/mnt/osfiles --target=i386-pc --no-floppy --modules="normal part_msdos ext2 multiboot" /dev/loop$(FST_LOOP)
	@mkdir -p build/imgfiles/boot/grub
	@cp $(KERNEL) build/imgfiles/boot/kernel.bin
	@cp $(GRUB_CFG) build/imgfiles/boot/grub
	@sudo cp -r build/imgfiles/* /mnt/osfiles
	@sudo umount /mnt/osfiles
	@sudo losetup -d /dev/loop$(FST_LOOP)
	@sudo losetup -d /dev/loop$(SEC_LOOP)

$(KERNEL): $(LINKER_SCRIPT) $(ASM_OBJ) $(C_OBJ) $(UNIT_TEST_OBJ) $(SYSTEM_TEST_OBJ)
	ld -n -T $(LINKER_SCRIPT) -o $(KERNEL)  $(ASM_OBJ) $(C_OBJ) $(UNIT_TEST_OBJ) $(SYSTEM_TEST_OBJ)

build/%.o: core/boot/%.asm
	@mkdir -p $(shell dirname $@)
	nasm -felf64 $< -o $@

build/%.o: core/src/%.c
	@mkdir -p $(shell dirname $@)
	x86_64-elf-gcc -c $< -o $@ -g -masm=intel -mno-red-zone -Wall -Werror -Wextra

build/%.o: unit_test/src/%.c
	@mkdir -p $(shell dirname $@)
	x86_64-elf-gcc -c $< -o $@ -g -masm=intel -mno-red-zone -Wall -Werror -Wextra
	
build/%.o: system_test/src/%.c
	@mkdir -p $(shell dirname $@)
	x86_64-elf-gcc -c $< -o $@ -g -masm=intel -mno-red-zone -Wall -Werror -Wextra