struct bitmap
{
    uint8_t free[131072]; // each bit in each byte represents one page
};

#define ALIGN(x) ((x) & ~0xFFF)
#define BITMAP_SET_ALLOCATED(pg) (pfa.free[(pg) / 8] |= (1 << ((pg) % 8)))
#define BITMAP_SET_FREE(pg) (pfa.free[(pg) / 8] &= ~(1 << ((pg) % 8)))

int pfa_alloc();
int pfa_unalloc(int);
void init_pfa(uint32_t, uint32_t, uint32_t, uint32_t, uint32_t);