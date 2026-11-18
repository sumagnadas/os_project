.intel_syntax noprefix

/* Declare constants for the multiboot header. */
.set ALIGN,    1<<0             /* align loaded modules on page boundaries */
.set MEMINFO,  1<<1             /* provide memory map */
.set FLAGS,    ALIGN | MEMINFO  /* this is the Multiboot 'flag' field */
.set MAGIC,    0x1BADB002       /* 'magic number' lets bootloader find the header */
.set CHECKSUM, -(MAGIC + FLAGS) /* checksum of above, to prove we are multiboot */

.section .multiboot, "a"
.align 4
.long MAGIC
.long FLAGS
.long CHECKSUM

/* Initial bootstrapping for basic page table setup before switching to higher level kernel */
.section .text.boot
.global _start
_start:
    lea esp, boot_stack_top

	mov [mbi_magic], eax
	mov [mbi_addr], ebx

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
mbi_addr: .skip 4
mbi_magic: .skip 4

/* 16KiB Stack for kernel */
.section .bss
.align 16
stack_bottom:
.skip 16384 # 16 KiB
stack_top:

.section .text
.include "kernel/idt.s"

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
.extern mbmain
higher_half_start:
	/* Initialize stack before jumping into C code as a stack is reqd. */
	lea esp, stack_top

	/* Set up memory mapping for PFA */
	push mbi_magic
	push mbi_addr
	call mbmain

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


