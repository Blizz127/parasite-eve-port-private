/*
 * Hand adapters — SPU voice / control register leaves.
 *
 * Each body is the matched leaf src/func_XXXXXXXX.c with its absolute SPU
 * MMIO accesses (0x1F801C00 + offset) routed through the port's SPU register
 * file, PE_SpuRegister_{Load,Store}U16(offset) — the same host mapping the
 * translated stream code uses for D_8009B3FC == 0x1F801C00.  Register pins and
 * asm barriers in the leaves are retail codegen hints only; the arithmetic,
 * masks and access widths are reproduced exactly.
 */
#include "pe_guest_decomp.h"
#include "pe_spu_dma.h"
#include "hand_hi_protos.h"

#define SPU_BASE 0x1F801C00u

/* src/func_80087728.c: 0x1F800000 as a u16 array, [0xEC6]/[0xEC7] =
 * 0x1F801D8C/0x1F801D8E (SPU offsets 0x18C/0x18E): low then high half. */
void func_80087728(unsigned int a0)
{
    PE_SpuRegister_StoreU16(0x1F801D8Cu - SPU_BASE, (unsigned short)a0);
    PE_SpuRegister_StoreU16(0x1F801D8Eu - SPU_BASE, (unsigned short)(a0 >> 16));
}

/* src/func_80087798.c: p = 0x1F801C00 + (voice << 4); p[0] = a1 & 0x7FFF;
 * p[1] = a2 & 0x7FFF. */
void func_80087798(int voice, int a1, int a2)
{
    uint32_t off = (uint32_t)(voice << 4);

    PE_SpuRegister_StoreU16(off + 0u, (unsigned short)(a1 & 0x7FFF));
    PE_SpuRegister_StoreU16(off + 2u, (unsigned short)(a2 & 0x7FFF));
}

/* src/func_800877BC.c: *(u16 *)(0x1F801C04 + (a0 << 4)) = a1. */
void func_800877BC(int a0, unsigned short a1)
{
    PE_SpuRegister_StoreU16(4u + (uint32_t)(a0 << 4), a1);
}

/* src/func_8008780C.c: p = 0x1F801C08 + (voice << 4); the LOW BYTE of the
 * halfword is read (lbu), then the halfword is rewritten as
 * low | ((a2 >> 2) << 15) | (a1 << 8), truncated to 16 bits. */
void func_8008780C(int voice, int a1, unsigned int a2)
{
    uint32_t off = 8u + (uint32_t)(voice << 4);
    int hi = (int)((a2 >> 2) << 15);
    int mid = a1 << 8;
    unsigned char low = (unsigned char)(PE_SpuRegister_LoadU16(off) & 0xFFu);

    PE_SpuRegister_StoreU16(off, (unsigned short)(low | (hi | mid)));
}

/* src/func_8008783C.c: halfword at 0x1F801C08 + (a0 << 4):
 * v = (v & 0xFF0F) | (a1 << 4). */
void func_8008783C(int a0, int a1)
{
    uint32_t off = 8u + (uint32_t)(a0 << 4);
    unsigned short v = PE_SpuRegister_LoadU16(off);

    v = (unsigned short)((v & 0xFF0F) | (a1 << 4));
    PE_SpuRegister_StoreU16(off, v);
}

/* src/func_80087864.c: *p = (*p & 0xFFF0) | a1 at 0x1F801C08 + (a0 << 4). */
void func_80087864(unsigned int a0, unsigned int a1)
{
    uint32_t off = 8u + (a0 << 4);

    PE_SpuRegister_StoreU16(off,
        (unsigned short)((PE_SpuRegister_LoadU16(off) & 0xFFF0u) | a1));
}

/* src/func_8008788C.c: halfword at 0x1F801C0A + (voice << 4):
 * (*reg & 0x3F) | ((mode >> 1) << 14) | (field << 6). */
void func_8008788C(int voice, unsigned int field, unsigned int mode)
{
    uint32_t off = 0xAu + (uint32_t)(voice << 4);

    PE_SpuRegister_StoreU16(off,
        (unsigned short)((PE_SpuRegister_LoadU16(off) & 0x003Fu) |
                         (((mode >> 1) << 14) | (field << 6))));
}

/* src/func_8008770C.c / 80087744 / 80087760 / 8008777C: the 0x1F800000
 * u16 array at [0xEC4]/[0xECC]/[0xECA]/[0xEC8] = SPU 0x1D88/0x1D98/0x1D94/
 * 0x1D90 — low half, then high half at the next halfword. */
static void spu_store_pair(uint32_t off, unsigned int a0)
{
    PE_SpuRegister_StoreU16(off, (unsigned short)a0);
    PE_SpuRegister_StoreU16(off + 2u, (unsigned short)(a0 >> 16));
}
void func_8008770C(unsigned int a0) { spu_store_pair(0x1F801D88u - SPU_BASE, a0); }
void func_80087744(unsigned int a0) { spu_store_pair(0x1F801D98u - SPU_BASE, a0); }
void func_80087760(unsigned int a0) { spu_store_pair(0x1F801D94u - SPU_BASE, a0); }
void func_8008777C(unsigned int a0) { spu_store_pair(0x1F801D90u - SPU_BASE, a0); }

/* src/func_800877D4.c: *(u16 *)(0x1F801C06 + (a0 << 4)) = a1 >> 3. */
void func_800877D4(int a0, unsigned int a1)
{
    PE_SpuRegister_StoreU16(6u + (uint32_t)(a0 << 4), (unsigned short)(a1 >> 3));
}

/* src/func_800877F0.c: *(u16 *)(0x1F801C0E + (a0 << 4)) = a1 >> 3. */
void func_800877F0(int a0, unsigned int a1)
{
    PE_SpuRegister_StoreU16(0xEu + (uint32_t)(a0 << 4), (unsigned short)(a1 >> 3));
}

/* src/func_80089F08.c: *a1 = *(u16 *)(D_8009B3FC + (a0 << 4) + 0xC).  The
 * D_8009B3FC base is the SPU window (0x1F801C00) at runtime, or a guest RAM
 * mirror; routed the same way as pe_stream.c's stream_register_read. */
void func_80089F08(unsigned int a0, pe_addr_t a1)
{
    pe_addr_t base = PE_LoadU32(0x8009B3FCu);
    uint32_t off = (a0 << 4) + 0xCu;
    uint16_t v;

    if ((base & 0x1FFFFFFFu) == SPU_BASE)
        v = PE_SpuRegister_LoadU16(off);
    else
        v = PE_LoadU16(base + off);
    PE_StoreU16(a1, v);
}
