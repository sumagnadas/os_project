.intel_syntax noprefix

/* Declare constants for the multiboot header. */
.set ALIGN,    1<<0             /* align loaded modules on page boundaries */
.set MEMINFO,  1<<1             /* provide memory map */
.set FLAGS,    ALIGN | MEMINFO  /* this is the Multiboot 'flag' field */
.set MAGIC,    0x1BADB002       /* 'magic number' lets bootloader find the header */
.set CHECKSUM, -(MAGIC + FLAGS) /* checksum of above, to prove we are multiboot */

/* 
Declare a multiboot header that marks the program as a kernel. These are magic
values that are documented in the multiboot standard. The bootloader will
search for this signature in the first 8 KiB of the kernel file, aligned at a
32-bit boundary. The signature is in its own section so the header can be
forced to be within the first 8 KiB of the kernel file.
*/
.section .multiboot, "a"
.align 4
.long MAGIC
.long FLAGS
.long CHECKSUM

.include "kernel/idt.s"

.section .text.boot
.global _start
_start:
    lea esp, boot_stack_top

    mov ecx, offset boot_page_table
	mov edx, 0
.fill:
    mov eax, edx
    shl eax, 12
    or  eax, 0x3
    mov [ecx + edx*4], eax
    inc edx
    cmp edx, 1024
    jl .fill

    mov eax, offset boot_page_table
    or  eax, 0x3
    mov [boot_page_dir], eax
    mov [boot_page_dir + 768*4], eax

    mov eax, offset boot_page_dir
    mov cr3, eax
    mov eax, cr0
    or  eax, 0x80000001
    mov cr0, eax

    lea ecx, [higher_half_start]
    jmp ecx

.size _start, . - _start

.section .bss.boot
.align 4096
boot_page_dir:   .skip 4096
boot_page_table: .skip 4096
.align 16
boot_stack_bottom: .skip 4096
boot_stack_top:

/* 16KiB Stack for kernel */
.section .bss
.align 16
stack_bottom:
.skip 16384 # 16 KiB
stack_top:

/*
The linker script specifies _start as the entry point to the kernel and the
bootloader will jump to this position once the kernel has been loaded. It
doesn't make sense to return from this function as the bootloader is gone.
*/
.section .text
.global _start
.type _start, @function
.global gdt_flush
.global set_tss_esp0

flush_tss:
	/* Load TSS to task register */
	mov ax, 0x28 // fifth 8-byte selector, symbolically OR-ed with 0 to set the RPL (requested privilege level).
	ltr ax
	ret

set_tss_esp0:
	/* set TSS.esp0 to the kernel stack */
	push ebx
	mov eax, [esp+8]
	lea ebx, stack_top
	mov [eax+4], ebx
	pop ebx
	ret

/* Flush and load GDT */
gdt_flush:
    mov eax, [esp+4]   
    lgdt [eax]

	/* Change data segment */
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

	/* Change code segment */
    jmp 0x08:.flush
.flush:
    ret

.global set_paging
set_paging:
	/* Set paging directory */
	mov eax, [esp+4]
	mov cr3, eax
	
	/* Set bits for CR0.PG and another bit */
	mov eax, cr0
	or eax, 0x80000001
	mov cr0, eax
	ret

.extern init_paging
higher_half_start:
	/* Initialize stack before jumping into C code as a stack is reqd. */
	lea esp, stack_top

	/* Initialize paging, GDT, IDT and TSS */
	call gdt_install
	call init_paging
	call flush_tss
	call enable_apic
	call idt_init
	
	/* Enter the high-level kernel. */
	call kernel_main

	sti
1:	hlt
	jmp 1b

// setup for syscall handler
.global syscall_stub
.extern syscall_handler
syscall_stub:
    pushad
	
	// push all the arguments
	push ebp
	push edi
	push esi
	push edx
	push ecx
	push ebx
	push eax

    call syscall_handler
	// pop them
	pop eax
	pop ebx
	pop ecx
	pop edx
	pop esi
	pop edi
	pop ebp
    popad
    iret


