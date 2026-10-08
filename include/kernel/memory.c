#include "memory.h"
#include "userspace.h"
#include "interrupts.h"

#define USER_STACK_SIZE 4096 // one page user stack

extern void set_paging(uint32_t page_dir);

__attribute__((aligned(4096))) uint8_t user_stack[USER_STACK_SIZE];

#define USER_CODE_PAGE ((uint32_t)user_code & ~0xFFF)
#define USER_STACK_PAGE ((uint32_t)user_stack & ~0xFFF)

struct pte_t
{
    uint8_t attribs;   // P, R/W, U/S, PWT, PCD, A, D, PAT
    uint8_t flags;     // G, AVL(0 - 2), address (12 - 15)
    uint16_t addr_mid; // address (16 - 31)
} __attribute__((packed));

struct pde_t
{
    uint8_t attribs;    // P, R/W, U/S, PWT, PCD, A, D, PS (0 = go to PT, 1 = 4 MB page)
    uint8_t flags;      // AVL(0 - 3), address (12 - 15)
    uint16_t addr_high; // address (16 - 31)
} __attribute__((packed));

uint32_t pg_dir[1024] __attribute__((aligned(4096)));
uint32_t pg_table[1024] __attribute__((aligned(4096)));

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
    // pg_dir[0].attribs = 0b00000011;
    // pg_dir[0].flags = ((uint32_t)pg_table >> 8) & 0xF0;
    // pg_dir[0].addr_high = (uint32_t)pg_table >> 16;
}