#include "kernel/interrupts/interrupts.h"
#include "drivers/tty.h"

/*
Basic exception handler for any exception
Shows the exception vector and the error code along with it
Then goes in an infinite loop.
MIGHT print extra info for certain vectors.
*/
void exception_handler(uint32_t vector, uint32_t error_code)
{
    uint32_t cr2;
    asm volatile("mov %%cr2, %0" : "=r"(cr2));

    // General exception description
    terminal_writestring("EXCEPTION vec=");
    print_hex(vector);
    terminal_writestring(" err=");
    print_hex(error_code);

    // Vector specific info
    switch (vector)
    {
    case 14:
        terminal_writestring(" cr2=");
        print_hex(cr2);
        break;

    default:
        break;
    }
    terminal_writestring("\n");

    // infinite loop
    for (;;)
        asm volatile("cli; hlt");
}