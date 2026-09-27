/*
 * BIOS A-function string/memory routines reached through the EXE's
 * `li t2,0xA0; jr t2; li t1,N` trampolines, adapted to guest memory.
 *
 * The PS1 BIOS is not in this tree, so the semantics come from the psx-spx
 * kernel/BIOS reference, sections "BIOS String Functions" and "BIOS Memory
 * Fill/Copy/Compare" (https://psx-spx.consoledev.net/kernelbios/, source
 * psx-spx.github.io docs/kernelbios.md), quoted at each definition.  All
 * pointers are 32-bit guest addresses; every byte goes through PE_Load/Store
 * (so the KUSEG/KSEG1 mirrors and bounds checks apply).
 *
 *   trampoline     BIOS     routine
 *   func_80072A54  A(17h)   strcmp(str1, str2)
 *   func_80071A04  A(18h)   strncmp(str1, str2, maxlen)
 *   func_80071A14  A(19h)   strcpy(dst, src)
 *   func_80072314  A(1Bh)   strlen(src)
 *   func_80071A34  A(2Ah)   memcpy(dst, src, len)
 *   func_80071A44  A(2Bh)   memset(dst, fillbyte, len)
 *   (A(28h) bzero is func_80071A24_port.c.)
 */
#ifndef PE_BIOS_STRING_H
#define PE_BIOS_STRING_H
#include <stdint.h>
#include "pe_guest_ram.h"

int32_t func_80072A54(pe_addr_t str1, pe_addr_t str2);
int32_t func_80071A04(pe_addr_t str1, pe_addr_t str2, int32_t maxlen);
pe_addr_t func_80071A14(pe_addr_t dst, pe_addr_t src);
int32_t func_80072314(pe_addr_t src);
pe_addr_t func_80071A34(pe_addr_t dst, pe_addr_t src, int32_t len);
pe_addr_t func_80071A44(pe_addr_t dst, int32_t fillbyte, int32_t len);

#endif
