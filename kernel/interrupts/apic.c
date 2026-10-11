
#include "kernel/interrupts/interrupts.h"
#include "kernel/interrupts/apic.h"
#include "kernel/helpers.h"
#include "drivers/tty.h"
#include "kernel/memory.h"

uint64_t volatile lapic_base;
/* APIC */
void init_lapic_timer(void);
void ioapic_set_irq(uint8_t, uint8_t);
void enable_apic(void)
{
    uint64_t base = read_msr(IA32_APIC_BASE_MSR);
    base |= (1 << 11); // enable bit
    write_msr(IA32_APIC_BASE_MSR, base);

    uint64_t a = read_msr(0x1b);

    lapic_base = (a & 0xFFFFF000);
    map_lapic(lapic_base, IOAPIC_BASE);
    ioapic_set_irq(1, 0x21);

    init_lapic_timer();
}

// Black magic at hardware level.
void init_lapic_timer(void)
{
    LAPIC_REG(0xF0) = 0x1FF;
    LAPIC_REG(0x3E0) = 0x3;
    LAPIC_REG(0x380) = 0x1000000;
    LAPIC_REG(0x320) = 0x20 | 0x20000; // vector 0x20, bit17=periodic
}

int val = 0;
void timer_interrupt(void)
{
    val++;
    terminal_writestring("Timer int ");
    print_dec(val);
    terminal_putchar('\n');
    LAPIC_REG(0xB0) = 0; // EOI — any write, not a port_out like the PIC
}

void ioapic_write(uint8_t reg, uint32_t val)
{
    IOAPIC_REGSEL = reg;
    IOAPIC_WIN = val;
}

void ioapic_set_irq(uint8_t irq, uint8_t vector)
{
    uint32_t low_reg = 0x10 + irq * 2;
    ioapic_write(low_reg, vector); // vector, delivery mode=fixed, active-high, edge-triggered (all default 0 bits = fine for ISA keyboard)
    ioapic_write(low_reg + 1, 0);  // destination: LAPIC ID 0 (this core)
}
