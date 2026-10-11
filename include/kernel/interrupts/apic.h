#include "kernel/interrupts/interrupts.h"
/* APIC */
extern uint64_t volatile lapic_base;

#define IA32_APIC_BASE_MSR 0x1B
#define LAPIC_REG(offset) (*(volatile uint32_t *)((uint8_t *)lapic_base + (offset)))

#define LAPIC_EOI 0xB0
#define LAPIC_SPURIOUS 0xF0
#define LAPIC_TIMER_LVT 0x320
#define LAPIC_TIMER_INIT_COUNT 0x380
#define LAPIC_TIMER_CUR_COUNT 0x390
#define LAPIC_TIMER_DIV 0x3E0

#define IOAPIC_BASE 0xFEC00000 // shortcut for now
#define IOAPIC_REGSEL (*(volatile uint32_t *)(IOAPIC_BASE))
#define IOAPIC_WIN (*(volatile uint32_t *)(IOAPIC_BASE + 0x10))
