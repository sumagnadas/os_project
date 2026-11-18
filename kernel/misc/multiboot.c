#include "stdint.h"
#include "kernel/misc/multiboot.h"
#include "kernel/memory/alloc.h"
#include "kernel/video/tty.h"
extern struct bitmap pfa;

/* Check if the bit BIT in FLAGS is set. */
#define CHECK_FLAG(flags, bit) ((flags) & (1 << (bit)))

// Multiboot handler
void mbmain(uint32_t addr, uint32_t magic)
{
    multiboot_info_t *mbi;

    /* Am I booted by a Multiboot-compliant boot loader? */
    if (magic != MULTIBOOT_BOOTLOADER_MAGIC)
    {
        terminal_writestring("Invalid magic number: ");
        terminal_writestring("\n");
        return;
    }

    /* Set MBI to the address of the Multiboot information structure. */
    mbi = (multiboot_info_t *)(addr);
    uint32_t mmap_addr = 0, mmap_length = 0, mem_upper = 0, mem_lower = 0;
    /* Get and process actual memory map and basic info for
       memory limits (failsafe, in case memory map is not available) */
    if (CHECK_FLAG(mbi->flags, 0))
    {
        mem_upper = mbi->mem_upper;
        mem_lower = mbi->mem_lower;
    }
    if (CHECK_FLAG(mbi->flags, 6))
    {
        mmap_addr = mbi->mmap_addr;
        mmap_length = mbi->mmap_length;
    }
    init_pfa(mbi->flags, mmap_addr, mmap_length, mem_upper, mem_lower);

    /* Draw diagonal blue line. (just kept as a reminder to set up video asap) */
    if (CHECK_FLAG(mbi->flags, 12))
    {
        multiboot_uint32_t color;
        unsigned i;
        void *fb = (void *)(unsigned long)mbi->framebuffer_addr;

        switch (mbi->framebuffer_type)
        {
        case MULTIBOOT_FRAMEBUFFER_TYPE_INDEXED:
        {
            unsigned best_distance, distance;
            struct multiboot_color *palette;

            palette = (struct multiboot_color *)mbi->framebuffer_palette_addr;

            color = 0;
            best_distance = 4 * 256 * 256;

            for (i = 0; i < mbi->framebuffer_palette_num_colors; i++)
            {
                distance = (0xff - palette[i].blue) * (0xff - palette[i].blue) + palette[i].red * palette[i].red + palette[i].green * palette[i].green;
                if (distance < best_distance)
                {
                    color = i;
                    best_distance = distance;
                }
            }
        }
        break;

        case MULTIBOOT_FRAMEBUFFER_TYPE_RGB:
            color = ((1 << mbi->framebuffer_blue_mask_size) - 1)
                    << mbi->framebuffer_blue_field_position;
            break;

        case MULTIBOOT_FRAMEBUFFER_TYPE_EGA_TEXT:
            color = '\\' | 0x0100;
            break;

        default:
            color = 0xffffffff;
            break;
        }
        for (i = 0; i < mbi->framebuffer_width && i < mbi->framebuffer_height; i++)
        {
            switch (mbi->framebuffer_bpp)
            {
            case 8:
            {
                multiboot_uint8_t *pixel = fb + mbi->framebuffer_pitch * i + i;
                *pixel = color;
            }
            break;
            case 15:
            case 16:
            {
                multiboot_uint16_t *pixel = fb + mbi->framebuffer_pitch * i + 2 * i;
                *pixel = color;
            }
            break;
            case 24:
            {
                multiboot_uint32_t *pixel = fb + mbi->framebuffer_pitch * i + 3 * i;
                *pixel = (color & 0xffffff) | (*pixel & 0xff000000);
            }
            break;

            case 32:
            {
                multiboot_uint32_t *pixel = fb + mbi->framebuffer_pitch * i + 4 * i;
                *pixel = color;
            }
            break;
            }
        }
    }
}