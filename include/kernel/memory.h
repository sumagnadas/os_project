#include <stdint.h>
// Paging structures
struct pte_t;
struct pde_t;

void init_paging(void);
void map_lapic(uint32_t lapic_base);