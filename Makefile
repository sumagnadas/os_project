.PHONY: all build_dir

BUILD_DIR := build
FOLDERS := lib kernel
OS_NAME := small_os
CFLAGS := -ffreestanding -O2 -masm=intel -Iinclude

SUBFOLDERS += $(shell find $(FOLDERS) -maxdepth 1 -mindepth 1 -type d 2> /dev/null)
SUBFOLDERS += $(FOLDERS)

all: build_dir iso

build_dir:
	mkdir -p $(BUILD_DIR)
	for i in $(SUBFOLDERS); do \
		mkdir -p $(BUILD_DIR)/$$i; \
	done

clean: 
	rm -rf $(BUILD_DIR)

compiledb:
	compiledb make

iso: build_dir $(BUILD_DIR)/$(OS_NAME)
	mkdir -p $(BUILD_DIR)/isodir/boot/grub
	cp $(BUILD_DIR)/$(OS_NAME) $(BUILD_DIR)/isodir/boot/$(OS_NAME)
	cp grub.cfg $(BUILD_DIR)/isodir/boot/grub/grub.cfg
	grub-mkrescue -o $(BUILD_DIR)/$(OS_NAME).iso $(BUILD_DIR)/isodir

$(BUILD_DIR)/$(OS_NAME): build_dir $(BUILD_DIR)/base.knl $(BUILD_DIR)/lib/boot.o
	i686-elf-gcc -T linker.ld -o $(BUILD_DIR)/$(OS_NAME) $(CFLAGS) -nostdlib $(BUILD_DIR)/lib/boot.o $(BUILD_DIR)/base.knl -lgcc

$(BUILD_DIR)/lib/boot.o: build_dir kernel/boot.s
	i686-elf-as kernel/boot.s -o $(BUILD_DIR)/lib/boot.o

$(BUILD_DIR)/lib/kernel.o: kernel/interrupts/*.c kernel/video/*.c kernel/memory.c kernel/process.c kernel/userspace.c
	for f in kernel/interrupts/*.c; do \
		name=$$(basename $$f .c); \
		i686-elf-gcc -c $$f -o $(BUILD_DIR)/kernel/interrupts/$$name.o $(CFLAGS) -std=gnu99 -Wall -Wextra; \
	done
	i686-elf-ld -r $(BUILD_DIR)/kernel/interrupts/*.o -o $(BUILD_DIR)/kernel/interrupts.o

	for f in kernel/video/*.c; do \
		name=$$(basename $$f .c); \
		i686-elf-gcc -c $$f -o $(BUILD_DIR)/kernel/video/$$name.o $(CFLAGS) -std=gnu99 -Wall -Wextra; \
	done
	i686-elf-ld -r $(BUILD_DIR)/kernel/video/*.o -o $(BUILD_DIR)/kernel/video.o
	
	i686-elf-gcc -c kernel/memory.c -o $(BUILD_DIR)/kernel/memory.o $(CFLAGS) -std=gnu99 -Wall -Wextra
	i686-elf-gcc -c kernel/process.c -o $(BUILD_DIR)/kernel/process.o $(CFLAGS) -std=gnu99 -Wall -Wextra
	i686-elf-gcc -c kernel/userspace.c -o $(BUILD_DIR)/kernel/userspace.o $(CFLAGS) -std=gnu99 -Wall -Wextra
	i686-elf-ld -r \
		$(BUILD_DIR)/kernel/memory.o \
		$(BUILD_DIR)/kernel/process.o \
		$(BUILD_DIR)/kernel/userspace.o \
		$(BUILD_DIR)/kernel/interrupts.o \
		$(BUILD_DIR)/kernel/video.o \
	-o $(BUILD_DIR)/lib/kernel.o

$(BUILD_DIR)/base.knl: $(BUILD_DIR)/lib/kernel.o
	i686-elf-gcc -c main.c -o $(BUILD_DIR)/lib/main.o $(CFLAGS) -std=gnu99 -Wall -Wextra
	i686-elf-ld -r \
		$(BUILD_DIR)/lib/main.o \
		$(BUILD_DIR)/lib/kernel.o \
	-o $(BUILD_DIR)/base.knl

launch: build_dir iso
	qemu-system-i386 -cdrom $(BUILD_DIR)/$(OS_NAME).iso