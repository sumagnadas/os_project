#include "kernel/interrupts/keyboard.h"

// common functions for exception handling
extern void *isr_stub_table[];

// functions defined in assembly
extern void syscall_stub();
extern void timer_handler();
