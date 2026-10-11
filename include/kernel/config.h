/* VGA configuration */
#define VGA_WIDTH 80
#define VGA_HEIGHT 25
#define VGA_MEMORY 0xB8000

/* IDT */
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

/* APIC */
#define IA32_APIC_BASE_MSR 0x1B
#define LAPIC_REG(offset) (*(volatile uint32_t *)((uint8_t *)lapic_base + (offset)))

#define LAPIC_EOI 0xB0
#define LAPIC_SPURIOUS 0xF0
#define LAPIC_TIMER_LVT 0x320
#define LAPIC_TIMER_INIT_COUNT 0x380
#define LAPIC_TIMER_CUR_COUNT 0x390
#define LAPIC_TIMER_DIV 0x3E0