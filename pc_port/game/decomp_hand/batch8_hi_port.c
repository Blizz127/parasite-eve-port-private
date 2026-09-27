/*
 * Hand adapters — batch 8 (>= 0x80060000).  Bodies follow the matched leaves
 * src/func_XXXXXXXX.c; pointers are guest addresses.  Retail stack buffers
 * that are handed to guest-address callees live at PE_HAND_HI_STACK.
 */
#include "pe_guest_decomp.h"
#include "pe_sdk.h"
#include "hand_hi_protos.h"

extern int func_800C6CE0(pe_addr_t slot);

/* src/func_80061B80.c: reset the edge cursor D_8009D148 to D_800A22B0 and
 * append (y, x) pairs — x = p[0] + D_8009D124, y = p[1] + D_8009D128 — for
 * each byte pair until p[0] >= 0xFF; a full buffer (12 pairs) calls
 * func_800527C0(4) instead (the empty-bodied leaf; its argument is unused).
 * Then func_80061878(p + 1, 0). */
void func_80061B80(int a0, int a1, pe_addr_t p)
{
    (void)a0; (void)a1;
    PE_StoreU32(0x8009D148u, 0x800A22B0u);
    while (PE_LoadU8(p) < 0xFFu) {
        int x = PE_LoadU8(p) + (int)PE_LoadU32(0x8009D124u);
        int y = (int)PE_LoadU32(0x8009D128u) + PE_LoadU8(p + 1u);
        pe_addr_t q = PE_LoadU32(0x8009D148u);

        if (q < 0x800A22B0u + 0x18u * 2u) {
            PE_StoreU16(q + 2u, (uint16_t)x);
            PE_StoreU32(0x8009D148u, q + 4u);
            PE_StoreU16(q, (uint16_t)y);
        } else {
            func_800527C0();
        }
        p += 2u;
    }
    func_80061878(p + 1u, 0u);
}

/* src/func_800C8064.c (+ twin 800C91A8): copy the 8-byte vector template to
 * a stack buffer, transform it by the matrix at B->f238 + 0x260
 * (func_80078C34 into a 4-halfword stack `out`), then a2[4]/[5] = base
 * x/y + out[0]/[1], a2[2] = 0x7F, a2[3] = 0, a2[6] = base z + out[2]. */
static void c8064_fill(pe_addr_t a2, pe_addr_t tmpl, pe_addr_t slot,
                       pe_addr_t gx, pe_addr_t gy, pe_addr_t gz)
{
    const pe_addr_t buf = PE_HAND_HI_STACK + 0x00u;
    const pe_addr_t out = PE_HAND_HI_STACK + 0x08u;
    int v1, u;

    PE_StoreU32(buf + 0u, PE_LoadU32(tmpl + 0u));
    PE_StoreU32(buf + 4u, PE_LoadU32(tmpl + 4u));
    (void)func_80078C34(PE_LoadU32(PE_LoadU32(slot) + 0x238u) + 0x260u, buf, out);
    PE_StoreU16(a2 + 8u, (uint16_t)(PE_LoadU16(gx) + PE_LoadU16(out + 0u)));
    PE_StoreU16(a2 + 10u, (uint16_t)(PE_LoadU16(gy) + PE_LoadU16(out + 2u)));
    v1 = PE_LoadU16(gz);
    u = PE_LoadU16(out + 4u);
    PE_StoreU16(a2 + 4u, 0x7Fu);
    PE_StoreU16(a2 + 6u, 0u);
    PE_StoreU16(a2 + 12u, (uint16_t)(v1 + u));
}
void func_800C8064(int a0, int a1, pe_addr_t a2)
{
    (void)a0; (void)a1;
    c8064_fill(a2, 0x800C214Cu, 0x800E279Cu, 0x800E2348u, 0x800E234Au, 0x800E234Cu);
}
void func_800C91A8(int a0, int a1, pe_addr_t a2)
{
    (void)a0; (void)a1;
    c8064_fill(a2, 0x800C217Cu, 0x800E27A0u, 0x800E2350u, 0x800E2352u, 0x800E2354u);
}

/* src/func_800C7AE0.c: vertex (a2 & 0xFFFF) of the model at a0 — header
 * offset (a0[0] + a0[1]) * 12 + 0x10 + (a0[2] + a0[3]) * 16, truncated to
 * u16 — transformed by matrix a1 into a3, then translated by a1->+0x14/18/1C. */
void func_800C7AE0(pe_addr_t a0, pe_addr_t a1, int a2, pe_addr_t a3)
{
    unsigned short off = (unsigned short)(
        (PE_LoadU16(a0 + 0u) + PE_LoadU16(a0 + 2u)) * 12 + 0x10 +
        (PE_LoadU16(a0 + 4u) + PE_LoadU16(a0 + 6u)) * 16);

    (void)func_80078C34(a1, a0 + off + (uint32_t)(a2 & 0xFFFF) * 8u, a3);
    PE_StoreU16(a3 + 0u, (uint16_t)(PE_LoadU16(a3 + 0u) + PE_LoadU32(a1 + 0x14u)));
    PE_StoreU16(a3 + 2u, (uint16_t)(PE_LoadU16(a3 + 2u) + PE_LoadU32(a1 + 0x18u)));
    PE_StoreU16(a3 + 4u, (uint16_t)(PE_LoadU16(a3 + 4u) + PE_LoadU32(a1 + 0x1Cu)));
}

/* src/func_80083C3C.c: for each group k < a0[0xE9], count the 6 slot bytes
 * at *(a0 + 0x20) equal to k, read the group limit (*(a0 + 4))[k*5 + 2]
 * (0 -> 1), and rewrite a0[0x5D + j] for each slot j of group k: 0xFF (and
 * cnt--) while cnt < lim, else k.  Then a0[0x46] = 0xFE; return 0. */
int func_80083C3C(pe_addr_t a0)
{
    int k = 0;

    if (PE_LoadU8(a0 + 0xE9u) != 0) {
        do {
            pe_addr_t p = PE_LoadU32(a0 + 0x20u);
            int cnt = 0, lim, j;

            for (j = 5; j >= 0; j--)
                if ((int)PE_LoadU8(p++) == k)
                    cnt++;
            lim = PE_LoadU8(PE_LoadU32(a0 + 4u) + (uint32_t)(k * 5 + 2));
            p = PE_LoadU32(a0 + 0x20u);
            if (lim == 0)
                lim = 1;
            for (j = 0; j < 6; j++) {
                if ((int)PE_LoadU8(p++) == k) {
                    if (cnt < lim) {
                        PE_StoreU8(a0 + 0x5Du + (uint32_t)j, 0xFFu);
                        cnt--;
                    } else {
                        PE_StoreU8(a0 + 0x5Du + (uint32_t)j, (uint8_t)k);
                    }
                }
            }
        } while (++k < (int)PE_LoadU8(a0 + 0xE9u));
    }
    PE_StoreU8(a0 + 0x46u, 0xFEu);
    return 0;
}

/* src/func_800C6C18.c: r = 1, or for slot state 3 whether the byte at
 * (**(a0 + 8))->+0x18 -> +0 is 2; when r and the state (re-queried) is 3,
 * set bit 0x4000 of (*D_8009D254)->+0x4C and bit 31 of **(a0 + 8). */
int func_800C6C18(pe_addr_t a0)
{
    int t = func_800C6CE0(a0);
    int r = 1;

    if (t == 3)
        r = PE_LoadU8(PE_LoadU32(PE_LoadU32(PE_LoadU32(a0 + 8u)) + 0x18u)) == 2;
    if (r != 0 && func_800C6CE0(a0) == 3) {
        pe_addr_t p = PE_LoadU32(PE_LoadU32(0x8009D254u));
        pe_addr_t q = PE_LoadU32(PE_LoadU32(a0 + 8u));

        PE_StoreU32(p + 0x4Cu, PE_LoadU32(p + 0x4Cu) | 0x4000u);
        PE_StoreU32(q, PE_LoadU32(q) | 0x80000000u);
    }
    return r;
}

/* src/func_800CC7AC.c: draw state (page 3, 0x10 colours, 16x16, blend 0);
 * D_800E2818[0x14] = (short)(a2[3] << 8) >> 7; then for i < (short)a2->+4
 * (unsigned compare) emit the point (+8, +0x28, +0x48 halfword columns)
 * through func_800C3B04(D_800E2818). */
void func_800CC7AC(int a0, int a1, pe_addr_t a2)
{
    unsigned int i;
    unsigned char c;

    (void)a0; (void)a1;
    func_800C2EAC(3);
    func_800C3098(0x10);
    func_800C2FF0(0x10, 0x10);
    func_800C3238(0);
    c = PE_LoadU8(a2 + 3u);
    PE_StoreU16(0x800E2818u + 0x28u, (uint16_t)((int16_t)(c << 8) >> 7));
    for (i = 0; i < (unsigned int)(int)(int16_t)PE_LoadU16(a2 + 4u); i++) {
        PE_StoreU16(0x800E2818u + 0u, PE_LoadU16(a2 + i * 2u + 8u));
        PE_StoreU16(0x800E2818u + 2u, PE_LoadU16(a2 + i * 2u + 0x28u));
        PE_StoreU16(0x800E2818u + 4u, PE_LoadU16(a2 + i * 2u + 0x48u));
        func_800C3B04(0x800E2818u);
    }
}

/* src/func_800CC55C.c: draw state (page 3, 0x10 colours, 32x32, blend 2);
 * three words at D_800E2260 + 0x10/0x14/0x18 = (short)a2->+6, halfword
 * [0x14] = a2[1]; for i < (s8)a2[2] (unsigned compare) emit the point at
 * a2 + 0x26 + i*8 through func_800C3B04(D_800E2260). */
void func_800CC55C(int a0, int a1, pe_addr_t a2)
{
    unsigned int i;
    uint32_t s6 = (uint32_t)(int32_t)(int16_t)PE_LoadU16(a2 + 6u);

    (void)a0; (void)a1;
    func_800C2EAC(3);
    func_800C3098(0x10);
    func_800C2FF0(0x20, 0x20);
    func_800C3238(2);
    PE_StoreU32(0x800E2260u + 0x10u, s6);
    PE_StoreU32(0x800E2260u + 0x14u, (uint32_t)(int32_t)(int16_t)PE_LoadU16(a2 + 6u));
    PE_StoreU32(0x800E2260u + 0x18u, (uint32_t)(int32_t)(int16_t)PE_LoadU16(a2 + 6u));
    PE_StoreU16(0x800E2260u + 0x28u, PE_LoadU8(a2 + 1u));
    for (i = 0; i < (unsigned int)(int)(signed char)PE_LoadU8(a2 + 2u); i++) {
        PE_StoreU16(0x800E2260u + 0u, PE_LoadU16(a2 + i * 8u + 0x26u));
        PE_StoreU16(0x800E2260u + 2u, PE_LoadU16(a2 + i * 8u + 0x28u));
        PE_StoreU16(0x800E2260u + 4u, PE_LoadU16(a2 + i * 8u + 0x2Au));
        func_800C3B04(0x800E2260u);
    }
}
