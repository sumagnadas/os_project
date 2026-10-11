#include <stdint.h>
#include <stdbool.h>
#include "kernel/config.h"

#define IDT_MAX_DESCRIPTORS 256
#define PIC1_COMMAND_PORT 0X20
#define PIC1_DATA_PORT 0X21
#define PIC2_COMMAND_PORT 0XA0
#define PIC2_DATA_PORT 0XA1
#define IDT_INTERRUPT_GATE_32BIT 0x8E
#define IDT_INTERRUPT_GATE_USER 0xEE

/* Keyboard */
#define KEYBOARD_DATA_PORT 0x60
#define KEYBOARD_STATUS_PORT 0x64