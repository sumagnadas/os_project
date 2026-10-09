#include <stdbool.h>
#include <stdint.h>
#include "drivers/tty.h"
#include "kernel/interrupts.h"

/* Check if the compiler thinks you are targeting the wrong operating system. */
#if defined(__linux__)
#error \
    "You are not using a cross-compiler, you will most certainly run into trouble"
#endif

/* This tutorial will only work for the 32-bit ix86 targets. */
#if !defined(__i386__)
#error "This tutorial needs to be compiled with a ix86-elf compiler"
#endif

extern void jump_user_code();

void syscall_handler(int code, uint32_t arg1, uint32_t arg2, uint32_t arg3, uint32_t arg4, uint32_t arg5, uint32_t arg6)
{
  switch (code)
  {
  case 1:
    terminal_writestring((char *)arg1);
    break;

  default:
    break;
  }
}

void kernel_main(void)
{
  /* Initialize terminal interface */
  terminal_initialize();

  /* Terminal test */
  terminal_setcolor(VGA_COLOR_GREEN);
  terminal_writestring("Hello, kernel World!\nKernel testing");
  terminal_setcolor(VGA_COLOR_RED);
  terminal_writestring("\nColor Testing");
  terminal_setcolor(vga_entry(VGA_COLOR_BLUE, VGA_COLOR_GREEN));
  terminal_writestring("\nColor mixing");
  terminal_setcolor(VGA_COLOR_LIGHT_GREEN);
  terminal_writestring("\nColor brightening\n");
  terminal_writestring("Hello, This is from kernel before jumping!\n");

  /* Number display testing*/
  terminal_writestring("This is a number, ");
  print_dec(189);
  terminal_writestring(" and this is a hex, ");
  print_hex(0x40);
  terminal_putchar('\n');

  // Jump to userspace after all set up.
  jump_user_code();
}