#include "drivers/tty.h"

static size_t terminal_row;
static size_t terminal_column;
static uint8_t terminal_color;
static uint16_t *terminal_buffer = (uint16_t *)VGA_MEMORY;
static uint16_t terminal_mem_buf[25 * 80];

size_t strlen(const char *str)
{
    size_t len = 0;
    while (str[len])
        len++;
    return len;
}

/* Fundamental functions for writing to terminal */
// Initialize terminal
void terminal_initialize(void)
{
    terminal_row = 0;
    terminal_column = 0;
    terminal_color = vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);

    for (size_t y = 0; y < VGA_HEIGHT; y++)
    {
        for (size_t x = 0; x < VGA_WIDTH; x++)
        {
            const size_t index = y * VGA_WIDTH + x;
            terminal_buffer[index] = vga_entry(' ', terminal_color);
            terminal_mem_buf[index] = vga_entry(' ', terminal_color);
        }
    }
}

// Set color (fg, bg) for terminal
void terminal_setcolor(uint8_t color)
{
    terminal_color = color;
}

// Set character at location (x,y) in the terminal
void terminal_putentryat(char c, uint8_t color, size_t x, size_t y)
{
    const size_t index = y * VGA_WIDTH + x;
    terminal_buffer[index] = vga_entry(c, color);
}

// Move the terminal by one line up
void terminal_refresh()
{
    for (size_t y = 1; y < VGA_HEIGHT; y++)
    {
        for (size_t x = 0; x < VGA_WIDTH; x++)
        {
            const size_t index = y * VGA_WIDTH + x;
            const size_t index_nrow = (y - 1) * VGA_WIDTH + x;
            terminal_buffer[index_nrow] = terminal_buffer[index];
        }
    }
    for (size_t x = 0; x < VGA_WIDTH; x++)
        terminal_buffer[(VGA_HEIGHT - 1) * VGA_WIDTH + x] = vga_entry(' ', terminal_color);
}

// Write character to current cursor position
void terminal_putchar(char c)
{
    switch (c)
    {
    case '\n':
    {
        ++terminal_row;
        terminal_column = 0;
        break;
    }
    default:
    {
        terminal_putentryat(c, terminal_color, terminal_column, terminal_row);
        ++terminal_column;
        break;
    }
    }
    if (terminal_column == VGA_WIDTH)
    {
        terminal_column = 0;
        ++terminal_row;
    }
    if (terminal_row == VGA_HEIGHT)
    {
        terminal_refresh();
        --terminal_row;
    }
}

/* String printing */
void terminal_write(const char *data, size_t size)
{
    for (size_t i = 0; i < size; i++)
        terminal_putchar(data[i]);
}
void terminal_writestring(const char *data)
{
    terminal_write(data, strlen(data));
}

/* Printing numbers */
void print_hex(uint32_t n)
{
    char hex_chars[] = "0123456789ABCDEF";
    terminal_writestring("0x");
    for (int i = 28; i >= 0; i -= 4)
    {
        terminal_putchar(hex_chars[(n >> i) & 0xF]);
    }
}
void print_dec(uint32_t n)
{
    char dec_chars[] = "0123456789";
    uint32_t nr = 0;

    // reverse number for printing;
    while (n > 0)
    {
        nr *= 10;
        nr += n % 10;
        n /= 10;
    }
    while (nr > 0)
    {
        terminal_putchar((nr % 10) + 0x30);
        nr /= 10;
    }
}