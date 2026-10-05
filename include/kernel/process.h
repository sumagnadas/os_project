#include <stdint.h>
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