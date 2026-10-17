/*
 * Hand adapters — port_absent `cd` group (2026-09-23): the last `absent`
 * matched leaves in 0x8007D614..0x80084EB0 (libspu DMA IRQ tail and the
 * libcard / memory-card controller).  Each function is written from the
 * verified matched leaf src/func_XXXXXXXX.c named above it; conventions as
 * in absent_lo_small_port.c / hand_hi_protos.h:
 *   - retail pointers are 32-bit guest addresses (pe_addr_t) accessed with
 *     PE_LoadXX / PE_StoreXX; a pointer global is its stored word (PE_LoadU32);
 *   - code-pointer values are the callee's retail VMA;
 *   - callees pc_port has not implemented are loud boundaries
 *     (PE_D_COMP_BOUNDARYn, exactly what a generated TU would emit).
 *
 * Guest function-pointer calls (the libcard slot table D_8009B724..748,
 * D_8009B6D0, D_8009B7A8[], D_8009B434, the local `f` of func_8008214C) go
 * through cd_icall(): it loads nothing itself — the caller passes the 32-bit
 * guest code address it loaded from the slot, exactly as retail's `lw` +
 * `jalr` — and dispatches to the host implementation of that exact VMA when
 * pc_port has one.  Any other address (including 0 in an unguarded call) is a
 * loud boundary that records the slot name, the guest code address and the
 * argument vector.  Nothing is bound by assumption: the dispatch key is the
 * value retail would jump to.
 *
 * func_80084FC4 / func_80084FE4 (root counter 2 timeout) are real calls
 * into rcnt2_port.c over the pe_rcnt2.h register model.
 */
#include "pe_guest_decomp.h"
#include "pe_sdk.h"
#include "pe_spu_dma.h"
#include "hand_absent_cd_protos.h"

/* decomp-derived / hand callees without a shared header prototype */
void func_8007DCAC(void);                               /* generated TU */
void func_80081268(void);                               /* generated TU */
int  func_80080AE4(pe_addr_t pe_buffer, int value);     /* generated TU */
void func_80081DF8(void);                               /* generated TU */
void func_80083578(void);                               /* generated TU */
int  func_800834E8(void);                               /* generated TU */
void func_800847A0(pe_addr_t pe_arg0);                  /* generated TU */
int  func_80084AE8(pe_addr_t pe_arg0);                  /* generated TU */

/* Callees with no pc_port implementation: loud boundaries. */
#define CD_B_80073A34(a, b) PE_D_COMP_BOUNDARY2("func_80073A34", 0x80073A34u, a, b)
/* func_80081D74 is ported in absent_r3_port.c. */
#define CD_B_80081D74(a, b) func_80081D74((int)(a), (int)(b))
#define CD_B_80072A54(a, b) func_80072A54(a, b)
#define CD_B_80071A04(a, b, c) \
    func_80071A04(a, b, c)
#define CD_B_8007E1F4(a, b) PE_D_COMP_BOUNDARY2("func_8007E1F4", 0x8007E1F4u, a, b)
#define CD_B_8007E1E4(a, b) PE_D_COMP_BOUNDARY2("func_8007E1E4", 0x8007E1E4u, a, b)
/* func_800830DC / func_800832B4: SIO0 byte exchange (sio_port.c). */
#define CD_B_800830DC(a, b) func_800830DC((pe_addr_t)(a), (int)(b))
#define CD_B_800832B4(a, b) func_800832B4((pe_addr_t)(a), (int)(b))
/* func_80084FC4 is the root-counter-2 timeout start (rcnt2_port.c). */
#define CD_B_80084FC4(a)    (func_80084FC4((unsigned int)(a)), 0)

/* libcard state words (all `int` / pointer globals in the matched leaves) */
#define GA_B724 0x8009B724u   /* void (*)(int)               */
#define GA_B728 0x8009B728u   /* void (*)(unsigned char *)   */
#define GA_B72C 0x8009B72Cu   /* int  (*)(unsigned char *, int) */
#define GA_B730 0x8009B730u   /* void (*)(int)               */
#define GA_B738 0x8009B738u   /* record getter                */
#define GA_B73C 0x8009B73Cu
#define GA_B740 0x8009B740u
#define GA_B744 0x8009B744u
#define GA_B748 0x8009B748u
#define GA_B74C 0x8009B74Cu
#define GA_B758 0x8009B758u   /* unsigned char * record base  */
#define GA_B75C 0x8009B75Cu
#define GA_B764 0x8009B764u
#define GA_B768 0x8009B768u
#define GA_B770 0x8009B770u
#define GA_B774 0x8009B774u
#define GA_B778 0x8009B778u
#define GA_B77C 0x8009B77Cu   /* int[] */
#define GA_B784 0x8009B784u   /* int *            */
#define GA_B788 0x8009B788u   /* SIO block pointer */
#define GA_B78C 0x8009B78Cu
#define GA_B79C 0x8009B79Cu
#define GA_B7A0 0x8009B7A0u
#define GA_B7A4 0x8009B7A4u
#define GA_B7A8 0x8009B7A8u   /* int (*[])()      */
#define GA_5AC0 0x800A5AC0u
#define GA_5AC4 0x800A5AC4u

static int32_t ld32(pe_addr_t a) { return (int32_t)PE_LoadU32(a); }

/* Dispatch one guest indirect call on the loaded code address `fn`.
 * `arity` is the number of argument registers the matched call site sets;
 * a host implementation needing more arguments than that is not called
 * (its extra registers are not proven) and the edge is a boundary. */
static uint32_t cd_icall(const char *slot, pe_addr_t fn, unsigned arity,
                         uint32_t a0, uint32_t a1, uint32_t a2)
{
    switch (fn) {
    case 0x80084644u:                    /* batch5_hi_port.c */
        if (arity >= 1u) { func_80084644(a0); return 0u; }
        break;
    case 0x800847A0u:                    /* generated TU */
        if (arity >= 1u) { func_800847A0(a0); return 0u; }
        break;
    case 0x80084AE8u:                    /* generated TU */
        if (arity >= 1u) return (uint32_t)func_80084AE8(a0);
        break;
    case 0x80084B20u:                    /* pe_controller_sdk_port.c */
        if (arity >= 1u) return func_80084B20(a0);
        break;
    case 0x80084F8Cu:                    /* pe_controller_sdk_port.c */
        if (arity >= 1u) return (uint32_t)func_80084F8C(a0);
        break;
    case PE_SPU_DMA_CALLBACK:            /* 0x80085098, void(void) */
        func_80085098();
        return 0u;
    default:
        break;
    }
    /* Any other target: the retail jalr through the guest-code registry
     * (native thunk, else the same loud boundary). */
    return (uint32_t)PE_GuestCall(slot, fn, arity, a0, a1, a2, 0u);
}

/* ── src/func_8007D614.c ───────────────────────────────────────────────
 * libspu DMA-completion IRQ tail: kick the busy-wait (func_8007DCAC) when
 * D_8009B44C == 0; clear bits 4-5 of the transfer-control halfword at
 * D_8009B3FC[0xD5] (+0x1AA) and poll them clear for at most 0xF00 retries;
 * then call D_8009B434() or, with no callback, func_80073A34(0xF0000009,
 * 0x20).  D_8009B3FC holds the SPU register base; the register accesses go
 * through the pc_port SPU register model exactly as pe_spu_dma.c's IRQ path
 * does (a RAM value is guest RAM).  The callback's live registers
 * (pe_dis.sh 0x8007D614 0x98: $a0 = 0xF0000000 from the `lui a0,0xf000` in
 * the 0x8007D690 delay slot) are not an argument of the C call and are not
 * forwarded. */
static uint16_t spu_ctl_load(pe_addr_t regs)
{
    if ((regs & 0x1FFFFFFFu) == 0x1F801C00u)
        return PE_SpuRegister_LoadU16(0x1AAu);
    return PE_LoadU16(regs + 0x1AAu);
}

static void spu_ctl_store(pe_addr_t regs, uint16_t v)
{
    if ((regs & 0x1FFFFFFFu) == 0x1F801C00u)
        PE_SpuRegister_StoreU16(0x1AAu, v);
    else
        PE_StoreU16(regs + 0x1AAu, v);
}

void func_8007D614(void)
{
    pe_addr_t p;
    unsigned int i;

    if (ld32(0x8009B44Cu) == 0)
        func_8007DCAC();
    p = PE_LoadU32(0x8009B3FCu);
    spu_ctl_store(p, (uint16_t)(spu_ctl_load(p) & 0xFFCFu));
    for (i = 0; spu_ctl_load(p) & 0x30u;) {
        if (++i >= 0xF01u)
            break;
    }
    if (PE_LoadU32(0x8009B434u) != 0u)
        (void)cd_icall("D_8009B434 (SPU DMA callback)", PE_LoadU32(0x8009B434u),
                       0u, 0u, 0u, 0u);
    else
        (void)CD_B_80073A34(0xF0000009u, 0x20);
}

/* ── src/func_80080F64.c: if ((a0 & 0xFF) == 2)
 *    func_80081D74(func_80080F98, -1).  Code pointer = retail VMA. */
/* func_80080F64: ported from the matching decomp -- generated TU pc_port/game/decomp/func_80080F64_port.c (src/func_80080F64.c); hand port retired after differential verification (docs/evidence/portverify/REPORT.md, round 6). */

/* ── src/func_80081110.c: card poll tick over D_8009B6AC[7]
 * (p = &D_8009B6AC[1]): p[0] += p[-1] << 2; p[1]--; a VSync(-1) timeout
 * past p[5] + 0x4B0 forces p[1] = -1; while p[1] != 0 and not timed out
 * return; otherwise func_80081268() and D_8009B6D0(p[1] < 0 ? 5 : 2, p[3]). */
void func_80081110(void)
{
    const pe_addr_t p = 0x8009B6B0u;
    int a, b, c, t, arg;

    a = ld32(p - 4u);
    b = ld32(p);
    c = ld32(p + 4u);
    PE_StoreU32(p, (uint32_t)(b + (a << 2)));
    PE_StoreU32(p + 4u, (uint32_t)(c - 1));
    t = (int)func_80073A44(-1);
    if (ld32(p + 0x14u) + 0x4B0 < t)
        PE_StoreU32(p + 4u, 0xFFFFFFFFu);
    if (ld32(p + 4u) != 0) {
        t = (int)func_80073A44(-1);
        if (ld32(p + 0x14u) + 0x4B0 >= t)
            return;
    }
    func_80081268();
    if (PE_LoadU32(0x8009B6D0u) != 0u) {
        arg = 2;
        if (ld32(p + 4u) < 0)
            arg = 5;
        (void)cd_icall("D_8009B6D0 (card poll callback)", PE_LoadU32(0x8009B6D0u),
                       2u, (uint32_t)arg, (uint32_t)ld32(p + 0xCu), 0u);
    }
}

/* ── src/func_800819D8.c: search the 0x2C-stride table D_800A3CB4 (0x80
 * entries; a zero key ends it) for key a0 whose name at D_800A3CBC + i*0x2C
 * compares equal (func_80072A54(a1, name) == 0); returns i + 1, else -1. */
/* func_800819D8: ported from the matching decomp -- generated TU pc_port/game/decomp/func_800819D8_port.c (src/func_800819D8.c); hand port retired after differential verification (docs/evidence/portverify/REPORT.md, round 6). */

/* ── src/func_8008214C.c: over D_8009B6EC[9] (p = &D_8009B6EC[7]): return
 * unless p[0] and p[1]; a0 == 2 -> func_80082204() unless func_8007F778();
 * else func_800824C8(p[-2]), func_800824DC(p[-1]), p[1] = 0, and the
 * callback p[-5] (read before the clear) gets (a0, a1, 0) when non-null. */
/* func_8008214C: ported from the matching decomp -- generated TU pc_port/game/decomp/func_8008214C_port.c (src/func_8008214C.c); hand port retired after differential verification (docs/evidence/portverify/REPORT.md, round 6). */

/* ── src/func_80082444.c: a0 == 1 -> read 2 bytes into the stack buf
 * (func_80080AE4(buf, 2)) and compare &buf[1] with D_8001205C over 5 bytes
 * (func_80071A04): equal -> 4, else 2; other a0 -> 2.  D_800B28F8 = v;
 * func_80081DF8().  buf lives at PE_HAND_CD_STACK. */
void func_80082444(unsigned char a0)
{
    const pe_addr_t buf = PE_HAND_CD_STACK;
    int v;

    if (a0 == 1) {
        (void)func_80080AE4(buf, 2);
        v = CD_B_80071A04(buf + 1u, 0x8001205Cu, 5);
        v = (v == 0) ? 4 : 2;
    } else {
        v = 2;
    }
    PE_StoreU32(0x800B28F8u, (uint32_t)v);
    func_80081DF8();
}

/* ── src/func_80082778.c: record = D_8009B738() (pe_dis.sh 0x80082778 0xD4:
 * the `jalr` at 0x80082794 runs with the caller's $a0/$a1/$a2 untouched, so
 * the retail getter receives (a0, a1, a2)).  a1 < 0 -> record[0xE9]; a1 >=
 * record[0xE9] -> 0; else byte (a2 - 1) of the 5-byte row a1 of
 * *(record + 4), 0 outside 1..5. */
int func_80082778(int a0, int a1, int a2)
{
    pe_addr_t p = cd_icall("D_8009B738 (card record getter)", PE_LoadU32(GA_B738),
                           3u, (uint32_t)a0, (uint32_t)a1, (uint32_t)a2);
    pe_addr_t row;
    unsigned int idx;

    if (a1 < 0)
        return PE_LoadU8(p + 0xE9u);
    if (a1 >= (int)PE_LoadU8(p + 0xE9u))
        return 0;
    row = PE_LoadU32(p + 4u) + (uint32_t)(a1 * 5);
    idx = (unsigned int)(a2 - 1);
    if (idx < 5u)
        return PE_LoadU8(row + idx);
    return 0;
}

/* ── src/func_8008284C.c: b = D_8009B738(a0); a1 < 0 -> b[0xEA]; a1 >=
 * b[0xEA] -> 0; row p = *(b + 8) + a1*8; a2 < 0 -> p[0]; a2 >= p[0] -> 0;
 * else (*(p + 4))[a2]. */
int func_8008284C(int a0, int a1, int a2)
{
    pe_addr_t b = cd_icall("D_8009B738 (card record getter)", PE_LoadU32(GA_B738),
                           1u, (uint32_t)a0, 0u, 0u);
    pe_addr_t p;

    if (a1 < 0)
        return PE_LoadU8(b + 0xEAu);
    if (a1 >= (int)PE_LoadU8(b + 0xEAu))
        return 0;
    p = PE_LoadU32(b + 8u) + (uint32_t)(a1 * 8);
    if (a2 < 0)
        return PE_LoadU8(p);
    if (a2 >= (int)PE_LoadU8(p))
        return 0;
    return PE_LoadU8(PE_LoadU32(p + 4u) + (uint32_t)a2);
}

/* ── src/func_800829C4.c: port-enable mask exchange.  cur = (D_8009B778 << 1)
 * | (D_8009B774 == 0); unchanged -> return cur.  Otherwise, with D_8009B75C
 * dropped to 0: bit 0 enables port 0 (D_8009B774 = 0; a D_800A5AC0 idle
 * count >= 0x96 resets record 0 through D_8009B728; count = 0), else
 * D_8009B774 = 1; bit 1 likewise port 1 (D_8009B778, D_800A5AC4, record
 * D_8009B758 + 0xF0).  D_8009B75C = 1; return cur. */
/* func_800829C4: ported from the matching decomp -- generated TU pc_port/game/decomp/func_800829C4_port.c (src/func_800829C4.c); hand port retired after differential verification (docs/evidence/portverify/REPORT.md, round 6). */

/* ── src/func_80082B08.c: p = D_8009B784; return 0 unless p[1] & 1 and
 * p[0] & 1; call D_8009B74C() when non-null; return 1.  (pe_dis.sh
 * 0x80082B08 0x68: the `jalr` at 0x80082B54 leaves the caller's $a0 live;
 * the C passes nothing, so no argument is forwarded.) */
/* func_80082B08: ported from the matching decomp -- generated TU pc_port/game/decomp/func_80082B08_port.c (src/func_80082B08.c); hand port retired after differential verification (docs/evidence/portverify/REPORT.md, round 6). */

/* ── src/func_80082B70.c: VBlank card handler.  D_8009B78C = 1; the idle
 * counters D_800A5AC0/AC4 of disabled ports (D_8009B774 != 0 /
 * D_8009B778 == 0) saturate at 0x96; when D_8009B75C and D_8009B774 <=
 * D_8009B778: D_8009B768 = 0, D_8009B764 = D_8009B774, start record
 * D_8009B774 (func_80082E00 == 0 -> D_8009B724(0xFFFF)), D_8009B76C = 0,
 * step func_80083014 on record D_8009B764 until it passes D_8009B778, then
 * write 0x88 to the SIO halfword D_8009B788 + 0xE.  Returns 0. */
int func_80082B70(void)
{
    int v;

    PE_StoreU32(GA_B78C, 1u);
    if (ld32(GA_B774) != 0) {
        v = ld32(GA_5AC0);
        if (v < 0x96)
            PE_StoreU32(GA_5AC0, (uint32_t)(v + 1));
    }
    if (ld32(GA_B778) == 0) {
        v = ld32(GA_5AC4);
        if (v < 0x96)
            PE_StoreU32(GA_5AC4, (uint32_t)(v + 1));
    }
    if (ld32(GA_B75C) != 0 && ld32(GA_B774) <= ld32(GA_B778)) {
        PE_StoreU32(0x8009B768u, 0u);
        PE_StoreU32(GA_B764, (uint32_t)ld32(GA_B774));
        if (func_80082E00(PE_LoadU32(GA_B758) + (uint32_t)(ld32(GA_B774) * 0xF0)) == 0)
            (void)cd_icall("D_8009B724 (card error hook)", PE_LoadU32(GA_B724),
                           1u, 0xFFFFu, 0u, 0u);
        PE_StoreU32(0x8009B76Cu, 0u);
        while (ld32(GA_B764) <= ld32(GA_B778))
            func_80083014(PE_LoadU32(GA_B758) + (uint32_t)(ld32(GA_B764) * 0xF0));
        PE_StoreU16(PE_LoadU32(GA_B788) + 0xEu, 0x88u);
    }
    return 0;
}

/* ── src/func_80082CF0.c: card controller reset.  D_8009B75C = 0;
 * EnterCriticalSection; func_8007E1F4/func_8007E1E4(2, &D_800A5AB0);
 * D_8009B784[0] = -2, D_8009B784[1] |= 1; func_80073C84(3, 0);
 * ExitCriticalSection; reset both records through D_8009B728
 * (D_8009B758, D_8009B758 + 0xF0); zero D_800A5AC4 then D_800A5AC0;
 * D_8009B75C = 1. */
void func_80082CF0(void)
{
    pe_addr_t p;

    PE_StoreU32(GA_B75C, 0u);
    (void)func_80072714();
    (void)CD_B_8007E1F4(2, 0x800A5AB0u);
    (void)CD_B_8007E1E4(2, 0x800A5AB0u);
    p = PE_LoadU32(GA_B784);
    PE_StoreU32(p, 0xFFFFFFFEu);
    PE_StoreU32(p + 4u, PE_LoadU32(p + 4u) | 1u);
    (void)func_80073C84(3u, 0u);
    func_80072724();
    (void)cd_icall("D_8009B728 (card record reset)", PE_LoadU32(GA_B728), 1u,
                   PE_LoadU32(GA_B758), 0u, 0u);
    (void)cd_icall("D_8009B728 (card record reset)", PE_LoadU32(GA_B728), 1u,
                   PE_LoadU32(GA_B758) + 0xF0u, 0u, 0u);
    PE_StoreU32(GA_5AC0 + 4u, 0u);
    PE_StoreU32(GA_5AC0, 0u);
    PE_StoreU32(GA_B75C, 1u);
}

/* ── src/func_80082E00.c: start a transfer on record a0.  SIO (the block at
 * D_8009B788): +0xA = 0x40 then 0, +8 = 0xD, +0xE = 0x88; func_80084FC4(
 * a0[0xE8] == 8 ? 0x50 : 0x91); +0xA = D_8009B764 ? 0x3003 : 0x1003.  Then
 * the per-port pending count D_8009B77C[D_8009B764]: while > 0, decrement
 * it and flush sector n of the record buffer (D_8009B744(*(a0 + 0xC) +
 * n*240)); when it reaches 0, mark it -1 and call D_8009B744(a0),
 * D_8009B748(a0).  If SIO +4 bit 9 is set: +0xA |= 0x10; still set ->
 * *SIO = 1, func_80083578(), a volatile byte read of *D_8009B788, return 0;
 * cleared -> *D_8009B784 = -0x81.  Return 0 when a0[0x50] && a0[0x36];
 * else *(*(a0 + 0x3C)) = 0 and return 1. */
int func_80082E00(pe_addr_t a0)
{
    pe_addr_t sio;
    pe_addr_t q;
    pe_addr_t f;
    int n;
    int idx;

    sio = PE_LoadU32(GA_B788);
    PE_StoreU16(sio + 0xAu, 0x40u);
    PE_StoreU16(sio + 0xAu, 0u);
    PE_StoreU16(sio + 8u, 0xDu);
    PE_StoreU16(sio + 0xEu, 0x88u);
    (void)CD_B_80084FC4(PE_LoadU8(a0 + 0xE8u) == 8u ? 0x50 : 0x91);
    idx = ld32(GA_B764);
    sio = PE_LoadU32(GA_B788);
    PE_StoreU16(sio + 0xAu, idx != 0 ? 0x3003u : 0x1003u);
    q = GA_B77C + (uint32_t)idx * 4u;
    n = ld32(q);
    if (n >= 0) {
        if (n > 0) {
            for (;;) {
                pe_addr_t lq = GA_B77C + (uint32_t)ld32(GA_B764) * 4u;
                n = ld32(lq) - 1;
                PE_StoreU32(lq, (uint32_t)n);
                (void)cd_icall("D_8009B744 (card sector flush)", PE_LoadU32(GA_B744),
                               1u, PE_LoadU32(a0 + 0xCu) + (uint32_t)(n * 240), 0u, 0u);
                if (ld32(GA_B77C + (uint32_t)ld32(GA_B764) * 4u) <= 0)
                    break;
            }
        }
        q = GA_B77C + (uint32_t)ld32(GA_B764) * 4u;
        if (ld32(q) == 0) {
            f = PE_LoadU32(GA_B744);
            PE_StoreU32(q, 0xFFFFFFFFu);
            (void)cd_icall("D_8009B744 (card sector flush)", f, 1u, a0, 0u, 0u);
            (void)cd_icall("D_8009B748 (card record done)", PE_LoadU32(GA_B748),
                           1u, a0, 0u, 0u);
        }
    }
    sio = PE_LoadU32(GA_B788);
    if (PE_LoadU16(sio + 4u) & 0x200u) {
        PE_StoreU16(sio + 0xAu, (uint16_t)(PE_LoadU16(sio + 0xAu) | 0x10u));
        if (PE_LoadU16(sio + 4u) & 0x200u) {
            PE_StoreU8(sio, 1u);
            func_80083578();
            (void)PE_LoadU8(PE_LoadU32(GA_B788));   /* `D_8009B788[0];` */
            return 0;
        }
        PE_StoreU32(PE_LoadU32(GA_B784), 0xFFFFFF7Fu);
    }
    if (PE_LoadU8(a0 + 0x50u) != 0u) {
        if (PE_LoadU8(a0 + 0x36u) != 0u)
            return 0;
    }
    PE_StoreU8(PE_LoadU32(a0 + 0x3Cu), 0u);
    return 1;
}

/* ── src/func_80083014.c: step the transfer state machine.  i = D_8009B768;
 * post-increment it and call the step D_8009B7A8[i] (with the caller's live
 * $a0, see hand_absent_cd_protos.h); r < 0 -> D_8009B724(r).  Otherwise,
 * when D_8009B768 != 0, func_80084FC4(0x3C) and, if func_800834E8() == 0,
 * D_8009B724(-3); finally a D_8009B768 >= 5 steps back by one. */
void func_80083014(pe_addr_t a0)
{
    int i = ld32(GA_B768);
    pe_addr_t f = PE_LoadU32(GA_B7A8 + (uint32_t)i * 4u);
    int r, t;

    PE_StoreU32(GA_B768, (uint32_t)(i + 1));
    r = (int)cd_icall("D_8009B7A8[] (card transfer step)", f, 1u, a0, 0u, 0u);
    if (r < 0) {
        (void)cd_icall("D_8009B724 (card error hook)", PE_LoadU32(GA_B724), 1u,
                       (uint32_t)r, 0u, 0u);
        return;
    }
    if (ld32(GA_B768) != 0) {
        (void)CD_B_80084FC4(0x3C);
        if (func_800834E8() == 0)
            (void)cd_icall("D_8009B724 (card error hook)", PE_LoadU32(GA_B724),
                           1u, 0xFFFFFFFDu, 0u, 0u);
    }
    t = ld32(GA_B768);
    if (t >= 5)
        PE_StoreU32(GA_B768, (uint32_t)(t - 1));
}

/* ── src/func_800837C8.c: begin a read of a1 bytes on record a0.  Returns 0
 * for a1 == 0, a busy record (+4 != 0) or a non-zero D_8009B740 (called
 * with the live a0/a1: pe_dis.sh 0x800837C8 0xD4, `jalr` at 0x80083800
 * with $a0/$a1 unchanged since entry).  Else sets +0x49 = 4, +0x46 = 1,
 * +0x14 = func_8008389C, +0x18 = func_80083944, +0x47 = 0 and the three
 * cumulative sizes: +0 = words(a1)*4, +4 += ((e3+1)>>1)*4,
 * +8 = +4 + ((e9*5 + 3) & 0xFFC); returns 1. */
int func_800837C8(pe_addr_t a0, int a1)
{
    int n, sz, e3, e9;

    if (a1 == 0)
        return 0;
    if (PE_LoadU32(a0 + 4u) != 0u)
        return 0;
    if (cd_icall("D_8009B740 (card busy query)", PE_LoadU32(GA_B740), 2u,
                 a0, (uint32_t)a1, 0u) != 0u)
        return 0;
    n = (a1 + 3) >> 2;
    PE_StoreU8(a0 + 0x49u, 4u);
    PE_StoreU8(a0 + 0x46u, 1u);
    PE_StoreU32(a0 + 0x14u, 0x8008389Cu);
    e3 = PE_LoadU8(a0 + 0xE3u);
    PE_StoreU32(a0 + 0x18u, 0x80083944u);
    e9 = PE_LoadU8(a0 + 0xE9u);
    sz = n << 2;
    PE_StoreU32(a0, (uint32_t)sz);
    PE_StoreU8(a0 + 0x47u, 0u);
    sz = sz + (((e3 + 1) >> 1) << 2);
    PE_StoreU32(a0 + 4u, (uint32_t)sz);
    PE_StoreU32(a0 + 8u, (uint32_t)(sz + ((e9 * 5 + 3) & 0xFFC)));
    return 1;
}

/* ── src/func_80083DF0.c: a0[0x53] set -> return 1 when a0[0x46] == 2,
 * else a0[0x46] = 0xFE; clear -> D_8009B728(a0).  Returns 0. */
/* func_80083DF0: ported from the matching decomp -- generated TU pc_port/game/decomp/func_80083DF0_port.c (src/func_80083DF0.c); hand port retired after differential verification (docs/evidence/portverify/REPORT.md, round 6). */

/* ── src/func_80083F04.c: D_8009B7A4 = D_8009B73C(a0);
 * func_800830DC(a0, -2). */
/* func_80083F04: ported from the matching decomp -- generated TU pc_port/game/decomp/func_80083F04_port.c (src/func_80083F04.c); hand port retired after differential verification (docs/evidence/portverify/REPORT.md, round 6). */

/* ── src/func_8008401C.c: with D_8009B7A4 set, D_8009B73C on buffer
 * sectors +0x1E0 and +0x2D0; r = func_800832B4(a0, a0[0x36] ? 0 :
 * D_8009B770); r < 0 -> r; no high nibble -> -9; else D_8009B79C =
 * (r & 0xF) << 1, or 0x20 when that is 0; return 0. */
/* func_8008401C: ported from the matching decomp -- generated TU pc_port/game/decomp/func_8008401C_port.c (src/func_8008401C.c); hand port retired after differential verification (docs/evidence/portverify/REPORT.md, round 6). */

/* ── src/func_800840DC.c: flag = (high nibble of **(a0 + 0x3C) == 8) &&
 * a0[0x36] == 0; r = func_800832B4(a0, D_8009B72C(a0, flag) & 0xFF);
 * 0x5A, 0 or negative -> r, else -9. */
int func_800840DC(pe_addr_t a0)
{
    int flag = 0;
    int r;

    if ((PE_LoadU8(PE_LoadU32(a0 + 0x3Cu)) >> 4) == 8)
        flag = (PE_LoadU8(a0 + 0x36u) == 0u);
    r = (int)cd_icall("D_8009B72C (card frame exchange)", PE_LoadU32(GA_B72C), 2u,
                      a0, (uint32_t)flag, 0u);
    r = CD_B_800832B4(a0, r & 0xFF);
    if (r == 0x5A || r == 0 || r < 0)
        return r;
    return -9;
}

/* ── src/func_80084168.c: the multi-sector transfer loop (register pins in
 * the leaf are era codegen hints; the control flow below is the leaf's
 * goto graph label for label).  `q` keeps its value across loop2 passes as
 * the leaf's $s3 does; it is only read on paths that assigned it (r1 > 0
 * before `is3`) unless a callback rewrote *p, in which case the value is
 * the previous pass's (initially 0 here: retail's caller $s3 is not a
 * property of the leaf). */
int func_80084168(pe_addr_t a0)
{
    const pe_addr_t s1 = a0;
    int flag, i, off, v, r, r1, r2, z;
    pe_addr_t p, q = 0u;

    (void)cd_icall("D_8009B730 (card sector prepare)", PE_LoadU32(GA_B730), 1u,
                   a0, 0u, 0u);
    flag = 0;
    if (ld32(GA_B770) != 0) {
        v = PE_LoadU8(PE_LoadU32(s1 + 0x3Cu));
        if ((v >> 4) == 8)
            flag = (PE_LoadU8(s1 + 0x36u) == 0u);
    }
    if (flag == 0)
        goto after;
    i = -1;
    off = -0xF0;
loop1:
    v = ld32(GA_B79C) - 1;
    PE_StoreU32(GA_B79C, (uint32_t)v);
    if (v <= 0)
        goto after;
    if (i >= 0)
        (void)cd_icall("D_8009B730 (card sector prepare)", PE_LoadU32(GA_B730), 1u,
                       PE_LoadU32(s1 + 0xCu) + (uint32_t)off, 0u, 0u);
    r = (int)cd_icall("D_8009B72C (card frame exchange)", PE_LoadU32(GA_B72C), 2u,
                      s1, 1u, 0u);
    r = CD_B_800832B4(s1, r & 0xFF);
    if (r < 0)
        goto out;
    (void)CD_B_80084FC4(0x3C);
    if (func_800834E8() == 0)
        goto minus3;
    i++;
    off += 0xF0;
    if (i < 4)
        goto loop1;
after:
    z = (ld32(GA_B764) == 0);
    if (ld32(GA_B79C) < 2)
        goto tail;
    p = GA_B77C + (uint32_t)z * 4u;
    off = z * 0xF0;
loop2:
    r1 = ld32(p);
    if (r1 < 0)
        goto tail;
    if (r1 > 0) {
        q = PE_LoadU32(PE_LoadU32(GA_B758) + (uint32_t)off + 0xCu) +
            (uint32_t)(r1 * 240) - 0xF0u;
        (void)cd_icall("D_8009B744 (card sector flush)", PE_LoadU32(GA_B744), 1u,
                       q, 0u, 0u);
    }
    r2 = ld32(p);
    if (r2 == 3)
        goto is3;
    if (r2 < 4) {
        if (r2 >= 2)
            goto call;
        if (r2 < 0)
            goto call;
        goto is01;
    }
    if (r2 != 4)
        goto call;
    PE_StoreU32(p, 3u);
    goto call;
is3:
    (void)cd_icall("D_8009B744 (card sector flush)", PE_LoadU32(GA_B744), 1u,
                   q - 0xF0u, 0u, 0u);
    v = 1;
    goto store;
is01:
    q = PE_LoadU32(GA_B758) + (uint32_t)off;
    (void)cd_icall("D_8009B744 (card sector flush)", PE_LoadU32(GA_B744), 1u,
                   q, 0u, 0u);
    (void)cd_icall("D_8009B748 (card record done)", PE_LoadU32(GA_B748), 1u,
                   q, 0u, 0u);
    v = -1;
store:
    PE_StoreU32(p, (uint32_t)v);
call:
    r = (int)cd_icall("D_8009B72C (card frame exchange)", PE_LoadU32(GA_B72C), 2u,
                      s1, (uint32_t)flag, 0u);
    r = CD_B_800830DC(s1, r & 0xFF);
    if (r < 0)
        goto out;
    (void)CD_B_80084FC4(0x3C);
    if (func_800834E8() != 0)
        goto cont;
minus3:
    r = -3;
    goto out;
cont:
    v = ld32(GA_B79C) - 1;
    PE_StoreU32(GA_B79C, (uint32_t)v);
    if (v >= 2)
        goto loop2;
    goto tail;
loop3:
    r = (int)cd_icall("D_8009B72C (card frame exchange)", PE_LoadU32(GA_B72C), 2u,
                      s1, (uint32_t)flag, 0u);
    r = CD_B_800830DC(s1, r & 0xFF);
    if (r < 0)
        goto out;
    (void)CD_B_80084FC4(0x3C);
    if (func_800834E8() == 0)
        goto minus3;
tail:
    v = ld32(GA_B79C) - 1;
    PE_StoreU32(GA_B79C, (uint32_t)v);
    if (v > 0)
        goto loop3;
    func_80083578();
    r2 = PE_LoadU8(s1 + 0x44u);
    PE_StoreU8(s1 + 0x44u, (uint8_t)(r2 + 1));
    r = PE_LoadU8(PE_LoadU32(GA_B7A0));
    PE_StoreU8(PE_LoadU32(s1 + 0x3Cu) + (uint32_t)r2, (uint8_t)r);
    (void)cd_icall("D_8009B724 (card error hook)", PE_LoadU32(GA_B724), 1u,
                   0u, 0u, 0u);
    r = 0;
out:
    return r;
}

/* ── src/func_80084B44.c: install the three card hooks. */
void func_80084B44(void)
{
    PE_StoreU32(GA_B73C, 0x80084B78u);
    PE_StoreU32(GA_B740, 0x80084F8Cu);
    PE_StoreU32(GA_B744, 0x80084C4Cu);
}

/* ── src/func_80084EB0.c: per-frame record tick.  +0x4C++; state +0x46:
 * 0 -> tail; 1 -> retry counter +0x4A below 2 bumps, else +0x49 = 2 and
 * +0x46 = 0xFF, return; other -> +0x4A below 4 bumps, else (+0x49 != 0 ->
 * D_8009B728(a0)) and tail.  Tail: unless **(a0 + 0x3C) == 0xF3, write
 * {0xFF, 0} at *(a0 + 0x30) and clear +0xE8. */
void func_80084EB0(pe_addr_t a0)
{
    unsigned int st, c;

    PE_StoreU32(a0 + 0x4Cu, PE_LoadU32(a0 + 0x4Cu) + 1u);
    st = PE_LoadU8(a0 + 0x46u);
    if (st == 0u)
        goto tail;
    if (st == 1u) {
        c = PE_LoadU8(a0 + 0x4Au);
        if (c < 2u)
            goto bump;
        PE_StoreU8(a0 + 0x49u, 2u);
        PE_StoreU8(a0 + 0x46u, 0xFFu);
        return;
    }
    c = PE_LoadU8(a0 + 0x4Au);
    if (c < 4u) {
bump:
        PE_StoreU8(a0 + 0x4Au, (uint8_t)(c + 1u));
        return;
    }
    if (PE_LoadU8(a0 + 0x49u) == 0u)
        goto tail;
    (void)cd_icall("D_8009B728 (card record reset)", PE_LoadU32(GA_B728), 1u,
                   a0, 0u, 0u);
tail:
    if (PE_LoadU8(PE_LoadU32(a0 + 0x3Cu)) != 0xF3u) {
        PE_StoreU8(PE_LoadU32(a0 + 0x30u), 0xFFu);
        PE_StoreU8(PE_LoadU32(a0 + 0x30u) + 1u, 0u);
        PE_StoreU8(a0 + 0xE8u, 0u);
    }
}
