#ifndef PAGING_H
#define PAGING_H

#include "boot.h"

void get_page_tables_size (boot_info_t *bi, uint64_t p_vaddr,
                           uint64_t krn_vaddr, uint64_t krn_vaddr_end);

void setup_page_tables (boot_info_t *bi, uintptr_t bi_addr,
                        uintptr_t krn_paddr);

#endif