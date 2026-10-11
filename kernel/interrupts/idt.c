#include "kernel/interrupts/interrupts.h"
#include "kernel/interrupts/idt.h"
#include "kernel/memory.h"

typedef struct
{
    uint16_t isr_low;   // The lower 16 bits of the ISR's address
    uint16_t kernel_cs; // The GDT segment selector that the CPU will load into CS before calling the ISR
    uint8_t reserved;   // Set to zero
    uint8_t attributes; // Type and attributes; see the IDT page
    uint16_t isr_high;  // The higher 16 bits of the ISR's address
} __attribute__((packed)) idt_entry_t;

typedef struct
{
    uint16_t limit;
    uint32_t base;
} __attribute__((packed)) idtr_t;

// IDT table
__attribute__((aligned(0x10))) static idt_entry_t idt[256]; // Create an array of IDT entries; aligned for performance
static idtr_t idtr;
static bool vectors[IDT_MAX_DESCRIPTORS];

// assigns the the idt entry from vector, handler and flags.
void idt_set_descriptor(uint8_t vector, void *isr, uint8_t flags)
{
    idt_entry_t *descriptor = &idt[vector];

    descriptor->isr_low = (uint32_t)isr & 0xFFFF;
    descriptor->kernel_cs = 0x08;
    descriptor->attributes = flags;
    descriptor->isr_high = (uint32_t)isr >> 16;
    descriptor->reserved = 0;
}

void idt_init()
{
    idtr.base = (uintptr_t)&idt[0];
    idtr.limit = (uint16_t)sizeof(idt_entry_t) * IDT_MAX_DESCRIPTORS - 1;

    // Assign exception handlers for exception vectors.
    for (uint8_t vector = 0; vector < 32; vector++)
    {
        idt_set_descriptor(vector, isr_stub_table[vector], IDT_INTERRUPT_GATE_32BIT);
        vectors[vector] = true;
    }

    // keyboard handler
    idt_set_descriptor(0x21, (void *)keyboard_handler, IDT_INTERRUPT_GATE_32BIT);
    vectors[0x21] = true;

    // syscall setup
    idt_set_descriptor(0x80, (void *)syscall_stub, IDT_INTERRUPT_GATE_USER);
    vectors[0x80] = true;

    // timer handler
    idt_set_descriptor(0x20, (void *)timer_handler, IDT_INTERRUPT_GATE_32BIT);
    vectors[0x20] = true;

    /* Disable PIC */
    // ICW1
    ioport_out(PIC1_COMMAND_PORT, 0x11);
    ioport_out(PIC2_COMMAND_PORT, 0x11);

    // ICW2
    ioport_out(PIC1_DATA_PORT, 0x20);
    ioport_out(PIC2_DATA_PORT, 0x28);

    // ICW3
    ioport_out(PIC1_DATA_PORT, 0x04);
    ioport_out(PIC2_DATA_PORT, 0x02);

    // ICW4
    ioport_out(PIC1_DATA_PORT, 0x1);
    ioport_out(PIC2_DATA_PORT, 0x1);

    // Mask interrupts
    ioport_out(PIC1_DATA_PORT, 0xff);
    ioport_out(PIC2_DATA_PORT, 0xff);

    // kb_init();
    // LOAD IDT
    __asm__ volatile("lidt %0" : : "m"(idtr)); // load the new IDT
    __asm__ volatile("sti");                   // set the interrupt flag
}
