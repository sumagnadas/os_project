#include "stdint.h"
#include "kernel/misc/multiboot.h"
#include "kernel/memory/alloc.h"

extern uint32_t kernel_start, kernel_end, boot_start, boot_end;

struct bitmap pfa;

void init_pfa(uint32_t flags, uint32_t mmap_addr, uint32_t mmap_length, uint32_t mem_upper, uint32_t mem_lower)
{

    for (int i = 0; i < 131072; i++)
        pfa.free[i] = 0xFF;

    // map reserved areas
    if (flags & (1 << 6))
    {
        for (multiboot_memory_map_t *mmap = (multiboot_memory_map_t *)mmap_addr; (unsigned long)mmap < mmap_addr + mmap_length; mmap = (multiboot_memory_map_t *)((unsigned long)mmap + mmap->size + sizeof(mmap->size)))
        {
            if (mmap->type == MULTIBOOT_MEMORY_AVAILABLE)
            {
                // align up/down to page boundary and
                // get page number for start and end region
                uint64_t start_pg = ALIGN(mmap->addr + 0xFFF) / 0x1000;
                uint64_t end_pg = ALIGN(mmap->addr + mmap->len) / 0x1000;

                // set free
                for (int pg_no = start_pg; pg_no < end_pg; pg_no++)
                    BITMAP_SET_FREE(pg_no);
            }
        }
    }
    else if (flags & (1 << 0))
    {
        /* failsafe for memory map unavailablity */

        // Calculate page counts based on KB values
        uint32_t low_pages = mem_lower / 4;
        uint32_t high_pages = mem_upper / 4;

        // Free conventional low memory (Skip page 0 for safety)
        for (uint32_t pg = 1; pg < low_pages; pg++)
        {
            BITMAP_SET_FREE(pg);
        }

        // Free continuous high memory starting at 1MB (Page 256)
        uint32_t start_page = 0x100000 / 0x1000; // Page 256
        uint32_t end_page = start_page + high_pages;

        for (uint32_t pg = start_page; pg < end_page; pg++)
            if (pg < 131072)
                BITMAP_SET_FREE(pg);
    }
    else
    {
        while (1)
            ;
    }

    // mark kernel area as reserved
    uint32_t kernel_start_pg = (uintptr_t)(&kernel_start) / 0x1000;
    uint32_t kernel_end_pg = (uintptr_t)(&kernel_end) / 0x1000;
    for (int i = kernel_start_pg; i < kernel_end_pg; i++)
    {
        BITMAP_SET_ALLOCATED(i);
    }

    // mark boot area as reserved
    uint32_t boot_start_pg = (uintptr_t)(&boot_start) / 0x1000;
    uint32_t boot_end_pg = (uintptr_t)(&boot_end) / 0x1000;
    for (int i = boot_start_pg; i < boot_end_pg; i++)
    {
        BITMAP_SET_ALLOCATED(i);
    }
}
int pfa_alloc()
{
    for (int i = 0; i < 131072; i++)
    {
        for (int j = 0; j < 8; i++)
        {
            if (pfa.free[i] & (1 << j) == 0)
            {
                // mark physicall page as allocated
                pfa.free[i] |= (1 << j);
                return i * 8 + j; // return page number;
            }
        }
    }
    return 0;
}
int pfa_unalloc(int page)
{
    int arr_index = page / 8, bit_ind = page % 8;
    if (pfa.free[arr_index] & (1 << bit_ind) == 0)
    {
        return 0;
    }
    pfa.free[arr_index] &= ~(1 << bit_ind);
    return 1;
}