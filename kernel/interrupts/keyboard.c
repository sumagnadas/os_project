#include "kernel/interrupts/interrupts.h"
#include "kernel/interrupts/apic.h"
#include "drivers/keyboard_map.h"
#include "kernel/interrupts/keyboard.h"
#include "drivers/tty.h"

// Handle keyboard input via interrupts
// Just print it to the terminal without any storage
void kb_isr()
{
    LAPIC_REG(0xB0) = 0; // EOI — any write, not a port_out like the PIC
    uint8_t status = ioport_in(KEYBOARD_STATUS_PORT);
    if (status & 0x1)
    {
        uint8_t keycode = ioport_in(KEYBOARD_DATA_PORT);
        if (keycode >= 128)
            return;
        terminal_putchar(keyboard_map[keycode]);
    }
}
