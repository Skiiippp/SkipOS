kernel := build/kernel.bin
img := build/foo.img

linker_script := src/linker.ld
grub_cfg := src/grub.cfg
assembly_source_files := $(wildcard src/*.asm)
assembly_object_files := $(patsubst src/%.asm, \
	build/%.o, $(assembly_source_files))

get_loopback_script := helper_scripts/get_loopback.py
first_loopback := 0
second_loopback := 0

.PHONY: all clean run img

all: $(kernel)

clean:
	@rm -rf build

run: $(img)
	qemu-system-x86_64 -s -drive format=raw,file=$(img) -serial stdio

img: $(img)

$(img) : $(kernel) $(grub_cfg)
	$(eval first_loopback := $(shell python3 $(get_loopback_script)))
	$(eval second_loopback := $(shell echo $$(( $(first_loopback) + 1 ))))
	@dd if=/dev/zero of=$(img) bs=512 count=32768
	@parted $(img) mklabel msdos
	@parted $(img) mkpart primary ext2 2048s 30720s
	@parted $(img) set 1 boot on
	@sudo losetup /dev/loop$(first_loopback) $(img)
	@sudo losetup /dev/loop$(second_loopback) $(img) -o 1048576
	@sudo mkfs.ext2 /dev/loop$(second_loopback)
	@sudo mkdir -p /mnt/osfiles
	@sudo mount /dev/loop$(second_loopback) /mnt/osfiles
	@sudo grub-install --root-directory=/mnt/osfiles --target=i386-pc --no-floppy --modules="normal part_msdos ext2 multiboot" /dev/loop$(first_loopback)
	@mkdir -p build/imgfiles/boot/grub
	@cp $(kernel) build/imgfiles/boot/kernel.bin
	@cp $(grub_cfg) build/imgfiles/boot/grub
	@sudo cp -r build/imgfiles/* /mnt/osfiles
	@sudo umount /mnt/osfiles
	@sudo losetup -d /dev/loop$(first_loopback)
	@sudo losetup -d /dev/loop$(second_loopback)

$(kernel): $(assembly_object_files) $(linker_script)
	@ld -n -T $(linker_script) -o $(kernel) $(assembly_object_files)

build/%.o: src/%.asm
	@mkdir -p $(shell dirname $@)
	@nasm -felf64 $< -o $@