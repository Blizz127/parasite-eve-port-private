/*
 * Hand adapters — memory-card command tag setters and their dispatchers
 * (0x800835C0 / 0x8008389C / 0x80083E50..0x80083EC4).  Bodies follow the
 * matched leaves src/func_XXXXXXXX.c; the card record is a guest address.
 */
#include "pe_guest_decomp.h"
#include "hand_hi_protos.h"

extern void func_80083E70(pe_addr_t pe_arg0);   /* generated TU */
extern void func_80083EE4(pe_addr_t pe_arg0);   /* generated TU */

/* Shared body: command byte +0x36 = tag, argument pointer +0x2C -> the
 * one-byte argument slot +0x24 = a1, argument count +0x35 = 1. */
static void card_tag(pe_addr_t a0, uint8_t tag, unsigned char a1)
{
    PE_StoreU8(a0 + 0x36u, tag);
    PE_StoreU32(a0 + 0x2Cu, a0 + 0x24u);
    PE_StoreU8(a0 + 0x24u, a1);
    PE_StoreU8(a0 + 0x35u, 1u);
}

/* src/func_80083E50.c / 80083E84 / 80083EA4 / 80083EC4: tags 'C' / 'L' /
 * 'F' / 'G'. */
void func_80083E50(pe_addr_t a0, unsigned char a1) { card_tag(a0, 0x43u, a1); }
void func_80083E84(pe_addr_t a0, unsigned char a1) { card_tag(a0, 0x4Cu, a1); }
void func_80083EA4(pe_addr_t a0, unsigned char a1) { card_tag(a0, 0x46u, a1); }
void func_80083EC4(pe_addr_t a0, unsigned char a1) { card_tag(a0, 0x47u, a1); }

/* src/func_800835C0.c: by state a0[0x46]: 2 -> func_80083E70 (declared
 * without arguments; retail keeps a0 live in $a0 for the call), 3 -> 'L'
 * with a0[0xE4], 4 -> 'G' with a0[0x47].
 * pe_dis.sh 0x800835C0 0x84: $a0 is never written; 0x80083604 is a bare
 * `jal 0x80083e70` with a0 still the entry argument. */
void func_800835C0(pe_addr_t a0)
{
    switch (PE_LoadU8(a0 + 0x46u)) {
    case 2:
        func_80083E70(a0);
        break;
    case 3:
        func_80083E84(a0, PE_LoadU8(a0 + 0xE4u));
        break;
    case 4:
        func_80083EC4(a0, PE_LoadU8(a0 + 0x47u));
        break;
    }
}

/* src/func_8008389C.c: by state a0[0x46]: 2 -> 'L', 3 -> 'F', 4 -> 'G'
 * when a0[0x48] == 0 else func_80083EE4 (bare call, a0 live in $a0); the
 * argument is a0[0x47].
 * pe_dis.sh 0x8008389C 0xA8: $a0 is never written; 0x8008392C is a bare
 * `jal 0x80083ee4` with a0 still the entry argument. */
void func_8008389C(pe_addr_t a0)
{
    switch (PE_LoadU8(a0 + 0x46u)) {
    case 2:
        func_80083E84(a0, PE_LoadU8(a0 + 0x47u));
        break;
    case 3:
        func_80083EA4(a0, PE_LoadU8(a0 + 0x47u));
        break;
    case 4:
        if (PE_LoadU8(a0 + 0x48u) == 0)
            func_80083EC4(a0, PE_LoadU8(a0 + 0x47u));
        else
            func_80083EE4(a0);
        break;
    }
}
