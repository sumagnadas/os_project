#include <stdint.h>

extern void keyboard_handler();
extern char ioport_in(uint16_t);
extern void ioport_out(uint16_t, uint8_t);

void kb_isr();
void kb_init();