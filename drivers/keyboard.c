#include <stddef.h>
#include "drivers/keyboard.h"
#include "drivers/keyboard_map.h"
#include "kernel/config.h"
#include "drivers/tty.h"

// Handle keyboard input via interrupts
// Just print it to the terminal without any storage
void kb_isr()
{
    ioport_out(PIC1_COMMAND_PORT, 0X20);
    uint8_t status = ioport_in(KEYBOARD_STATUS_PORT);
    if (status & 0x1)
    {
        uint8_t keycode = ioport_in(KEYBOARD_DATA_PORT);
        if (keycode < 0 || keycode >= 128)
            return;
        terminal_putchar(keyboard_map[keycode]);
    }
}

void kb_init()
{
    // unmask interrupt
    // 0xFD = 1111 1101
    ioport_out(PIC1_DATA_PORT, 0xFD);
}