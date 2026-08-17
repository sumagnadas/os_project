#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "drivers/interrupts.h"
// #include<

#include "drivers/tty.h"

/* Check if the compiler thinks you are targeting the wrong operating system. */
#if defined(__linux__)
#error \
    "You are not using a cross-compiler, you will most certainly run into trouble"
#endif

/* This tutorial will only work for the 32-bit ix86 targets. */
#if !defined(__i386__)
#error "This tutorial needs to be compiled with a ix86-elf compiler"
#endif

void kernel_main(void)
{
  /* Initialize terminal interface */
  terminal_initialize();

  /* Terminal test */
  terminal_setcolor(VGA_COLOR_GREEN);
  terminal_writestring("Hello, kernel World!\nMeowChika\nMeow");
  terminal_setcolor(VGA_COLOR_RED);
  terminal_writestring("\nMeow\nMeow\nMeow\nMeow\nMeow\nMeow\nMeow\nMeow");
  terminal_setcolor(vga_entry(VGA_COLOR_BLUE, VGA_COLOR_GREEN));
  terminal_writestring("\nMeow\nMeow\nMeow\nMeow\nMeow\nMeow\n");
  terminal_setcolor(VGA_COLOR_BLUE);
  terminal_writestring("Meow\nMeow\nMeow\nMeow\nMeow\nMeow\nMeow\nMeow\nMeow");
}
