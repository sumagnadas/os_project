.PHONY: all

BUILD_DIR := build
SUBFOLDERS := lib kernel drivers
OS_NAME := small_os
CFLAGS := -ffreestanding -O2 -masm=intel -Iinclude

all: build_dir iso

build_dir:
	mkdir -p $(BUILD_DIR)
	for i in $(SUBFOLDERS); do mkdir -p $(BUILD_DIR)/$$i; done

clean: 
	rm -rf $(BUILD_DIR)

compiledb:
	compiledb make

iso: build_dir $(OS_NAME)
	mkdir -p $(BUILD_DIR)/isodir/boot/grub
	cp $(BUILD_DIR)/$(OS_NAME) $(BUILD_DIR)/isodir/boot/$(OS_NAME)
	cp grub.cfg $(BUILD_DIR)/isodir/boot/grub/grub.cfg
	grub-mkrescue -o $(BUILD_DIR)/$(OS_NAME).iso $(BUILD_DIR)/isodir

$(OS_NAME): build_dir base boot.o
	i686-elf-gcc -T linker.ld -o $(BUILD_DIR)/$(OS_NAME) $(CFLAGS) -nostdlib $(BUILD_DIR)/lib/boot.o $(BUILD_DIR)/base.knl -lgcc

boot.o: build_dir boot.s
	i686-elf-as boot.s -o $(BUILD_DIR)/lib/boot.o

kernel.o: kernel/memory.c kernel/interrupts.c kernel/process.c kernel/userspace.c
	i686-elf-gcc -c kernel/interrupts.c -o $(BUILD_DIR)/kernel/interrupts.o $(CFLAGS) -std=gnu99 -Wall -Wextra
	i686-elf-gcc -c kernel/memory.c -o $(BUILD_DIR)/kernel/memory.o $(CFLAGS) -std=gnu99 -Wall -Wextra
	i686-elf-gcc -c kernel/process.c -o $(BUILD_DIR)/kernel/process.o $(CFLAGS) -std=gnu99 -Wall -Wextra
	i686-elf-gcc -c kernel/userspace.c -o $(BUILD_DIR)/kernel/userspace.o $(CFLAGS) -std=gnu99 -Wall -Wextra
	i686-elf-ld -r $(BUILD_DIR)/kernel/memory.o $(BUILD_DIR)/kernel/process.o $(BUILD_DIR)/kernel/userspace.o $(BUILD_DIR)/kernel/interrupts.o -o $(BUILD_DIR)/lib/kernel.o

drivers.o: drivers/keyboard.c drivers/tty.c
	i686-elf-gcc -c drivers/keyboard.c -o $(BUILD_DIR)/drivers/keyboard.o $(CFLAGS) -std=gnu99 -Wall -Wextra
	i686-elf-gcc -c drivers/tty.c -o $(BUILD_DIR)/drivers/tty.o $(CFLAGS) -std=gnu99 -Wall -Wextra
	i686-elf-ld -r $(BUILD_DIR)/drivers/tty.o $(BUILD_DIR)/drivers/keyboard.o -o $(BUILD_DIR)/lib/drivers.o

base: kernel.o drivers.o
	i686-elf-gcc -c main.c -o $(BUILD_DIR)/lib/main.o $(CFLAGS) -std=gnu99 -Wall -Wextra
	i686-elf-ld -r \
		$(BUILD_DIR)/lib/drivers.o \
		$(BUILD_DIR)/lib/main.o \
		$(BUILD_DIR)/lib/kernel.o \
	-o $(BUILD_DIR)/base.knl

launch: build_dir iso
	qemu-system-i386 -cdrom $(BUILD_DIR)/$(OS_NAME).iso