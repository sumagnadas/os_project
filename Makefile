.PHONY: all

BUILD_DIR := build
OS_NAME := small_os
CFLAGS := -ffreestanding -O2 -masm=intel 

all: build_dir iso

build_dir:
	mkdir -p $(BUILD_DIR)

clean: 
	rm -rf $(BUILD_DIR)

compiledb:
	compiledb make

iso: build_dir $(OS_NAME)
	mkdir -p $(BUILD_DIR)/isodir/boot/grub
	cp $(BUILD_DIR)/$(OS_NAME) $(BUILD_DIR)/isodir/boot/$(OS_NAME)
	cp grub.cfg $(BUILD_DIR)/isodir/boot/grub/grub.cfg
	grub-mkrescue -o $(BUILD_DIR)/$(OS_NAME).iso $(BUILD_DIR)/isodir

$(OS_NAME): build_dir kernel boot.o
	i686-elf-gcc -T linker.ld -o $(BUILD_DIR)/$(OS_NAME) $(CFLAGS) -nostdlib $(BUILD_DIR)/boot.o $(BUILD_DIR)/small_os.knl -lgcc

boot.o: build_dir boot.s
	i686-elf-as boot.s -o $(BUILD_DIR)/boot.o	

kernel: build_dir kernel.c drivers/tty.h drivers/config.h drivers/interrupts.c
	i686-elf-gcc -c kernel.c -o $(BUILD_DIR)/kernel.o $(CFLAGS) -std=gnu99 -Wall -Wextra
	i686-elf-gcc -c drivers/keyboard.c -o $(BUILD_DIR)/keyboard.o $(CFLAGS) -std=gnu99 -Wall -Wextra
	i686-elf-gcc -c drivers/tty.c -o $(BUILD_DIR)/tty.o $(CFLAGS) -std=gnu99 -Wall -Wextra
	i686-elf-gcc -c drivers/interrupts.c -o $(BUILD_DIR)/interrupts.o $(CFLAGS) -std=gnu99 -Wall -Wextra
	i686-elf-ld -r $(BUILD_DIR)/tty.o $(BUILD_DIR)/keyboard.o $(BUILD_DIR)/kernel.o $(BUILD_DIR)/interrupts.o -o $(BUILD_DIR)/small_os.knl

launch: build_dir iso
	qemu-system-i386 -cdrom $(BUILD_DIR)/small_os.iso