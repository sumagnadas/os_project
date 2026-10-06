#include <stdbool.h>
#include <stdint.h>
#include "drivers/tty.h"
#include "kernel/interrupts.h"
// #include<

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

void syscall_handler(int val)
{
  terminal_writestring("Back in ring 0 via syscall!\n");
  print_dec(val);
  terminal_writestring("\n");
}

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
  terminal_writestring("Hello, This is from kernel before jumping!\n");

  jump_user_code();
}

void user_code()
{
  terminal_writestring("Hello, This is from userspace!\n");
  asm volatile("mov eax, 30; int 0x80");
  terminal_writestring("Hello, This is from userspace again!\n");
  for (;;)
  {
  }
}