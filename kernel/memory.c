#include "kernel/memory.h"
#include "kernel/userspace.h"
#include "kernel/interrupts.h"

#define USER_STACK_SIZE 4096 // one page user stack
#define USER_CODE_PAGE ((uint32_t)user_code & ~0xFFF)

#define USER_STACK_PAGE ((uint32_t)(user_stack) & ~0xFFF)

struct gdt_entry
{
    uint16_t limit_low;
    uint16_t base_low;
    uint8_t base_middle;
    uint8_t access;
    uint8_t granularity;
    uint8_t base_high;
} __attribute__((packed));

struct gdt_ptr
{
    uint16_t limit;
    uint32_t base;
} __attribute__((packed));

struct tss
{
    uint32_t prev_tss;
    uint32_t esp0;
    uint32_t ss0;
    // everything else unused because sw switching gonna be used
    uint32_t esp1, ss1, esp2, ss2;
    uint32_t cr3, eip, eflags;
    uint32_t eax, ecx, edx, ebx, esp, ebp, esi, edi;
    uint32_t es, cs, ss, ds, fs, gs;
    uint32_t ldt;
    uint16_t trap, iomap_base;
} __attribute__((packed));

// functions from assembly for setting up memory stuff
extern void gdt_flush(uint32_t);
extern void set_tss_esp0(uint32_t);
extern void set_paging(uint32_t);

// GDT table definition
struct gdt_entry gdt[6];
struct gdt_ptr gp;
struct tss TSS = {0};

// user stack definition
__attribute__((aligned(4096))) uint8_t user_stack[USER_STACK_SIZE];

// paging table and directory
uint32_t pg_dir[1024] __attribute__((aligned(4096)));
uint32_t pg_table[1024] __attribute__((aligned(4096)));

void gdt_set_gate(int num, uint32_t base, uint32_t limit,
                  uint8_t access, uint8_t gran)
{
    gdt[num].base_low = base & 0xFFFF;
    gdt[num].base_middle = (base >> 16) & 0xFF;
    gdt[num].base_high = (base >> 24) & 0xFF;

    gdt[num].limit_low = limit & 0xFFFF;
    gdt[num].granularity = (limit >> 16) & 0x0F;

    gdt[num].granularity |= gran & 0xF0;
    gdt[num].access = access;
}

void gdt_install(void)
{
    gp.limit = (sizeof(struct gdt_entry) * 6) - 1;
    gp.base = (uint32_t)&gdt;

    gdt_set_gate(0, 0, 0, 0, 0);             // null descriptor, required
    gdt_set_gate(1, 0, 0xFFFFF, 0x9A, 0xCF); // kernel code
    gdt_set_gate(2, 0, 0xFFFFF, 0x92, 0xCF); // kernel data
    gdt_set_gate(3, 0, 0xFFFFF, 0xFA, 0xCF); // user code
    gdt_set_gate(4, 0, 0xFFFFF, 0xF2, 0xCF); // user data

    // set up tss
    TSS.ss0 = 0x10;
    set_tss_esp0((uint32_t)&TSS);
    gdt_set_gate(5, (uint32_t)&TSS, sizeof(TSS) - 1, 0x89, 0x0);

    gdt_flush((uint32_t)&gp);
}

void init_paging(void)
{
    // default page setup
    for (int i = 0; i < 1024; i++)
    {
        pg_table[i] = (i * 0x1000) | 0x3; // identity map 0-4MB: P | R/W | U/S
        pg_dir[i] = 0x2;                  // not present
    }
    pg_dir[0] = ((uint32_t)pg_table) | 0x7;

    // user page setup
    int code_idx = USER_CODE_PAGE / 0x1000;
    int stack_idx = USER_STACK_PAGE / 0x1000;
    pg_table[stack_idx] = USER_STACK_PAGE | 0x7;
    pg_table[code_idx] = USER_CODE_PAGE | 0x7;

    set_paging((uint32_t)pg_dir);
}