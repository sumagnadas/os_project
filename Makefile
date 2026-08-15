.PHONY: all

BUILD_DIR := build
OS_NAME := small_os

all: build_dir iso

build_dir:
	mkdir -p $(BUILD_DIR)

iso: build_dir $(OS_NAME)
	mkdir -p $(BUILD_DIR)/isodir/boot/grub
	cp $(BUILD_DIR)/$(OS_NAME) $(BUILD_DIR)/isodir/boot/$(OS_NAME)
	cp grub.cfg $(BUILD_DIR)/isodir/boot/grub/grub.cfg
	grub-mkrescue -o $(BUILD_DIR)/$(OS_NAME).iso $(BUILD_DIR)/isodir

$(OS_NAME): build_dir kernel.o boot.o
	i686-elf-gcc -T linker.ld -o $(BUILD_DIR)/$(OS_NAME) -ffreestanding -O2 -nostdlib $(BUILD_DIR)/boot.o $(BUILD_DIR)/kernel.o -lgcc

boot.o: build_dir boot.s
	i686-elf-as boot.s -o $(BUILD_DIR)/boot.o	

kernel.o: build_dir kernel.c
	i686-elf-gcc -c kernel.c -o $(BUILD_DIR)/kernel.o -std=gnu99 -ffreestanding -O2 -Wall -Wextra

launch: build_dir iso
	qemu-system-i386 -cdrom $(BUILD_DIR)/small_os.iso