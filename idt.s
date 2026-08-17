.intel_syntax noprefix

exception_handler:
	cli
	hlt

.macro isr_err_stub no
isr_stub_\no:
    call exception_handler
    iret 
.endm

.macro isr_no_err_stub no
isr_stub_\no:
    call exception_handler
    iret 
.endm

.macro dd_stub no
	.long isr_stub_\no
.endm


/* Interrupts setup */
isr_no_err_stub 0
isr_no_err_stub 1
isr_no_err_stub 2
isr_no_err_stub 3
isr_no_err_stub 4
isr_no_err_stub 5
isr_no_err_stub 6
isr_no_err_stub 7
isr_err_stub    8
isr_no_err_stub 9
isr_err_stub    10
isr_err_stub    11
isr_err_stub    12
isr_err_stub    13
isr_err_stub    14
isr_no_err_stub 15
isr_no_err_stub 16
isr_err_stub    17
isr_no_err_stub 18
isr_no_err_stub 19
isr_no_err_stub 20
isr_no_err_stub 21
isr_no_err_stub 22
isr_no_err_stub 23
isr_no_err_stub 24
isr_no_err_stub 25
isr_no_err_stub 26
isr_no_err_stub 27
isr_no_err_stub 28
isr_no_err_stub 29
isr_err_stub    30
isr_no_err_stub 31

.altmacro
.global isr_stub_table
isr_stub_table:
	.set i, 0
.rep 32
	dd_stub %i
	.set i, i+1
.endr
.noaltmacro

.global ioport_in
.global ioport_out
.global keyboard_handler

.extern keyboard_isr

ioport_in:
    mov edx, [esp+4] // port to read from
    in al, dx
    ret

ioport_out:
    mov edx, [esp+4] // port to write to 
    mov eax, [esp+8] // 8bit value to write
    out dx, al
    ret

keyboard_handler:
    pushad
    cld
    call kb_isr
    popad
    iret