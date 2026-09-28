/*
 * Hand adapters — port_absent lane (2026-09-23), SDK range.  Nineteen
 * matched leaves the generator rejects (guest code pointers, pointer
 * returns, kernel/hardware addresses, E18 layouts).  Each body is the
 * matched leaf src/func_XXXXXXXX.c; retail pointers are 32-bit guest
 * addresses (pe_addr_t) and struct fields are read at the leaf's offsets.
 *
 * Two small host services are shared by the adapters below:
 *
 *   pa_call   — a retail indirect call through a guest code pointer.  The
 *               32-bit identity is loaded from guest RAM exactly where the
 *               leaf loads it; an identity with a pc_port implementation
 *               runs it, anything else is a loud boundary
 *               (PE_Decomp_Boundary: symbol, the loaded target, argument
 *               vector) that returns 0.  Identities are listed with the
 *               static-data / installer evidence that proves them.
 *   pa_mmio_* — a store/load through a guest *pointer global* whose retail
 *               value is a hardware register.  Only the registers whose
 *               slots are proven by the static exe data below are routed,
 *               each to its existing single platform authority; guest RAM
 *               addresses stay plain guest RAM; anything else aborts
 *               loudly in PE_Translate (unrepresented hardware).
 *
 * Retail static data (tools/analysis/pe_dis.sh 0x8009564C 0x40,
 * 0x800956A0 0x30, 0x80095704 0x40, 0x80095740 0x10):
 *   D_8009566C = 0x8009564C (libetc jump table): +0x10 = 0x80074218,
 *     +0x18 = 0x800742B8 (+0x04/+0x14 are written by func_80073E28 only);
 *   D_80095670 = 0x1F801070 (I_STAT), D_80095674 = 0x1F801074 (I_MASK),
 *   D_80095678 = 0x1F8010F0 (DPCR), D_800956B0 = 0x1F801114 (timer 1
 *   mode), D_800956BC = 0x1F8010F4 (DICR);
 *   D_80095744 = 0x80095704 (libgpu jtb): [2] 0x80076C34, [3] 0x80076434,
 *     [13] 0x80077144, [14] 0x8007633C;  D_80095748 = 0x80071A74 (printf).
 */
#include "pe_guest_decomp.h"
#include "pe_sdk.h"
#include "pe_irq.h"
#include "pe_gpu.h"
#include "pe_timer1.h"
#include "pe_callback.h"
#include "pe_irq_delivery.h"
#include "game_port.h"

extern int func_80077144(int mode);          /* generated TU (jtb[13]) */
extern int func_8007633C(void);              /* generated TU (jtb[14]) */
extern void func_8009019C(pe_addr_t pe_a0, unsigned int a1); /* generated */
extern void func_800902AC(pe_addr_t pe_a0, unsigned int a1); /* generated */
extern unsigned int func_80076170(short x, short y);         /* generated */
extern unsigned int func_80076208(short x, short y);         /* generated */

/* ── shared host services ─────────────────────────────────────────── */

static int pa_call(const char *site, pe_addr_t target, unsigned arity,
                   uint32_t a0, uint32_t a1, uint32_t a2, uint32_t a3)
{
    (void)a1; (void)a2; (void)a3;
    switch (target) {
    case 0x80074218u:   /* libetc jtb +0x10 (static data) */
        return (int)func_80074218();
    case 0x800742B8u:   /* libetc jtb +0x18 (static data) */
        return (int)func_800742B8();
    case 0x80077144u:   /* libgpu jtb[13] (static data) */
        return func_80077144((int)a0);
    case 0x8007633Cu:   /* libgpu jtb[14] (static data) */
        return func_8007633C();
    case 0x8007440Cu:   /* CPU source 0: func_800743B4 -> func_80073CC4(0, .) */
        func_8007440C();
        return 0;
    case 0x80074520u:   /* CPU source 3: func_800744D4 -> func_80073CC4(3, .) */
        func_80074520();
        return 0;
    case 0x8007C13Cu:   /* CPU source bound by pe_irq_delivery.c DispatchCpuCallback */
        func_8007C13C();
        return 0;
    default:
        return PE_Decomp_Boundary(site, target, arity, a0, a1, a2, a3);
    }
}

/* Hardware registers reached through the SDK pointer globals go through
 * the central MMIO table (pc_port/platform/pe_mmio.h: I_STAT/I_MASK,
 * DPCR/DICR, RCNT1 0x107 program, RCNT2); any other I/O address is a loud
 * abort there, and RAM addresses are plain guest RAM. */
static void pa_mmio_store16(pe_addr_t a, uint16_t v) { PE_StoreU16(a, v); }
static uint16_t pa_mmio_load16(pe_addr_t a) { return PE_LoadU16(a); }
static void pa_mmio_store32(pe_addr_t a, uint32_t v) { PE_StoreU32(a, v); }

/* ── 0x80038954 ─────────────────────────────────────────────────────
 * src/func_80038954.c: fatal-error report.  printf(D_80010EB0,
 * D_80091A2C[(u8)a3], a0); printf(D_80010EC8, a1, a2); when (u8)a3 == 1 the
 * retail leaf spins forever (`for (;;) {}`, pe_dis.sh 0x80038954 0x60).  The
 * host cannot spin, so that halt is a run-control stop request: execution
 * never continues past it on retail either. */
void func_80038954(pe_addr_t a0, pe_addr_t a1, pe_addr_t a2, int a3)
{
    unsigned char i = (unsigned char)a3;

    (void)PE_D_COMP_BOUNDARY3("func_80071A74", 0x80071A74u, 0x80010EB0u,
                              PE_LoadU32(0x80091A2Cu + (uint32_t)i * 4u), a0);
    (void)PE_D_COMP_BOUNDARY3("func_80071A74", 0x80071A74u, 0x80010EC8u,
                              a1, a2);
    if (i == 1)
        PE_Port_RequestStop(PE_PORT_STOP_EXPLICIT);
}

/* ── 0x8005CAEC ─────────────────────────────────────────────────────
 * src/func_8005CAEC.c: point the selection list at D_800C0E48, count =
 * func_80052F70(), bitmap D_8009D058 = D_8009D05C (2 words, cleared), then
 * for every slot i < D_8009D040 whose id (D_800A1D9C[i], signed short)
 * indexes the list: resolve the record (0x100..0x17F -> D_800BEEAC +
 * v*32; 1..0xFF -> func_8005DB44(v-1); 0x200..0x208 -> D_8009DE64 + v*32;
 * else NULL) and set bit i when rec[0x14] < rec[1].  Returns the count of
 * set bits.  An out-of-range id or unresolved record reads through NULL
 * (retail reads kernel RAM 0x14/0x01 through the KUSEG mirror, which
 * PE_Translate folds onto KSEG0 — the same bytes). */
int func_8005CAEC(void)
{
    pe_addr_t p;
    int n = 0;
    int i;
    int k;

    PE_StoreU32(0x8009D048u, 0x800C0E48u);
    PE_StoreU32(0x8009D050u, (uint32_t)func_80052F70());
    PE_StoreU32(0x8009D058u, 0x8009D05Cu);
    PE_StoreU32(0x8009D064u, 2u);
    func_800542A0(0x3FEu);
    for (k = 0; k < (int)PE_LoadU32(0x8009D064u); k++)
        PE_StoreU32(PE_LoadU32(0x8009D058u) + (uint32_t)k * 4u, 0u);
    for (i = 0; i < (int)PE_LoadU32(0x8009D040u); i++) {
        int idx;

        if (i >= 0 && i < (int)PE_LoadU32(0x8009D040u))
            idx = (short)PE_LoadU16(0x800A1D9Cu + (uint32_t)i * 2u);
        else
            idx = 0;
        if (idx >= 0 && idx < (int)PE_LoadU32(0x8009D050u)) {
            int v = (short)PE_LoadU16(PE_LoadU32(0x8009D048u) +
                                      (uint32_t)idx * 2u);

            if ((unsigned int)(v - 0x100) < 0x80u)
                p = ((uint32_t)v << 5) + 0x800BEEACu;
            else if ((unsigned int)(v - 1) < 0xFFu)
                p = func_8005DB44((unsigned int)(v - 1));
            else if ((unsigned int)(v - 0x200) < 9u)
                p = ((uint32_t)v << 5) + 0x8009DE64u;
            else
                p = 0u;
        } else {
            p = 0u;
        }
        if (PE_LoadU8(p + 0x14u) < PE_LoadU8(p + 1u)) {
            pe_addr_t w = PE_LoadU32(0x8009D058u) + (uint32_t)(i >> 5) * 4u;

            n++;
            PE_StoreU32(w, PE_LoadU32(w) | (1u << (i & 0x1F)));
        }
    }
    return n;
}

/* ── 0x800727B4 / 0x80072950: libapi device hook ─────────────────────
 * The kernel device table pointer/byte size live at KUSEG 0x150/0x154
 * (`*(Dev **)0x150`, `*(unsigned int *)0x154` in the leaves).  KUSEG
 * 0x00000000..0x001FFFFF is the same physical main RAM as KSEG0 0x80000000
 * (psx-spx memory map), so they are read at 0x80000150/0x80000154.  Entries
 * are 0x50 bytes: +0 name pointer, +0x34 hook.  func_80072A54 is BIOS
 * A(17h) strcmp (pe_dis.sh 0x80072A54 0x10; pc_port/platform/
 * pe_bios_string.c); func_80072A64 (BIOS B(42h)) has no pc_port
 * implementation: loud boundary (0). */
#define PA_DEV_TABLE 0x80000150u
#define PA_DEV_BYTES 0x80000154u

int func_800727B4(pe_addr_t name, int arg)
{
    pe_addr_t s = name;
    pe_addr_t d = 0x800A32D8u;
    pe_addr_t e;
    pe_addr_t base;
    unsigned int n;
    int found = 0;

    while ((signed char)PE_LoadU8(s) >= 0x3B) {
        PE_StoreU8(d++, PE_LoadU8(s++));
    }
    PE_StoreU8(d, 0u);
    n = PE_LoadU32(PA_DEV_BYTES) / 0x50u;
    base = PE_LoadU32(PA_DEV_TABLE);
    for (e = base; e < base + n * 0x50u; e += 0x50u) {
        if (PE_LoadU32(e) != 0u &&
            func_80072A54(PE_LoadU32(e), 0x800A32D8u) == 0) {
            PE_StoreU32(0x800A32D0u, PE_LoadU32(e + 0x34u));
            found = 1;
            break;
        }
    }
    if (!found)
        return 0;
    n = PE_LoadU32(PA_DEV_BYTES) / 0x50u;
    base = PE_LoadU32(PA_DEV_TABLE);
    for (e = base; e < base + n * 0x50u; e += 0x50u) {
        if (PE_LoadU32(e) != 0u &&
            func_80072A54(PE_LoadU32(e), 0x800A32D8u) == 0) {
            PE_StoreU32(e + 0x34u, 0x80072950u);   /* e->hook = func_80072950 */
            break;
        }
    }
    return PE_D_COMP_BOUNDARY2("func_80072A64", 0x80072A64u, name, arg);
}

/* src/func_80072950.c (the hook installed above): force *a0 = 1 when 0,
 * restore the saved hook D_800A32D0 into the matching device entry, then
 * tail-call the saved hook with (a0, a1, a2) — a kernel device function:
 * loud boundary with the loaded identity. */
int func_80072950(pe_addr_t a0, int a1, int a2)
{
    unsigned int n;
    pe_addr_t p;
    pe_addr_t end;
    uint32_t h;

    if (PE_LoadU32(a0) == 0u)
        PE_StoreU32(a0, 1u);
    n = PE_LoadU32(PA_DEV_BYTES) / 80u;
    p = PE_LoadU32(PA_DEV_TABLE);
    h = PE_LoadU32(0x800A32D0u);
    end = p + n * 80u;
    for (; p < end; p += 0x50u) {
        if (PE_LoadU32(p) != 0u &&
            func_80072A54(PE_LoadU32(p), 0x800A32D8u) == 0) {
            PE_StoreU32(p + 0x34u, h);
            break;
        }
    }
    return pa_call("func_80072950_saved_device_hook", PE_LoadU32(0x800A32D0u),
                   3u, a0, (uint32_t)a1, (uint32_t)a2, 0u);
}

/* ── 0x80073D88 / 0x80073DB8 ────────────────────────────────────────
 * src: call the libetc jump-table slot D_8009566C + 0x10 / + 0x18 with no
 * arguments (static data: 0x80074218 / 0x800742B8). */
void func_80073D88(void)
{
    (void)pa_call("func_80073D88_jtb_0x10",
                  PE_LoadU32(PE_LoadU32(0x8009566Cu) + 0x10u), 0u, 0, 0, 0, 0);
}

void func_80073DB8(void)
{
    (void)pa_call("func_80073DB8_jtb_0x18",
                  PE_LoadU32(PE_LoadU32(0x8009566Cu) + 0x18u), 0u, 0, 0, 0, 0);
}

/* ── 0x80073E28: ResetCallback body ─────────────────────────────────
 * src/func_80073E28.c.  Guard D_800945E4 set -> 0.  I_MASK = 0,
 * I_STAT = (readback of I_MASK), DPCR = 0x33333333; clear 0x41A words at
 * D_800945E4; func_80074354 (BIOS A(13h) setjmp, pe_dis.sh 0x80074354
 * 0x10) on the +0x38 jmp_buf — its nonzero (longjmp) return runs the IRQ
 * dispatcher; that exception re-entry is a boundary (returns 0 here).
 * Then D_80094620 = D_80094620 + 0xFDC, func_800743A4 (BIOS B(19h)
 * HookEntryInt) on D_80094620 - 4, guard = 1, jtb[5] = func_800743B4(),
 * jtb[1] = func_800744D4(), func_8007436C (BIOS A(72h)), ExitCritical.
 * Returns the state block D_800945E4. */
pe_addr_t func_80073E28(void)
{
    const pe_addr_t p = 0x800945E4u;
    const pe_addr_t q = 0x80094620u;
    pe_addr_t a, b;
    uint32_t r;

    if (PE_LoadU16(p) != 0u)
        return 0u;
    b = PE_LoadU32(0x80095670u);
    a = PE_LoadU32(0x80095674u);
    pa_mmio_store16(a, 0u);
    pa_mmio_store16(b, pa_mmio_load16(a));
    pa_mmio_store32(PE_LoadU32(0x80095678u), 0x33333333u);
    func_80074330(p, 0x41A);
    if (PE_D_COMP_BOUNDARY1("func_80074354", 0x80074354u, p + 0x38u) != 0)
        func_80073F00();
    PE_StoreU32(q, q + 0xFDCu);
    (void)PE_D_COMP_BOUNDARY1("func_800743A4", 0x800743A4u, q - 4u);
    PE_StoreU16(q - 0x3Cu, 1u);
    r = func_800743B4();
    PE_StoreU32(PE_LoadU32(0x8009566Cu) + 5u * 4u, r);
    r = func_800744D4();
    PE_StoreU32(PE_LoadU32(0x8009566Cu) + 1u * 4u, r);
    (void)PE_D_COMP_BOUNDARY0("func_8007436C", 0x8007436Cu);
    func_80072724();
    return q - 0x3Cu;
}

/* ── 0x80073F00: CPU interrupt dispatcher ───────────────────────────
 * src/func_80073F00.c.  State block b = D_800945E4: +0 guard, +2 active,
 * +4 tbl[11] (guest code pointers), +0x30 registered mask.  Unset guard
 * -> printf("...", I_STAT) + func_80074384 (BIOS B(17h)
 * ReturnFromException, boundary).  active = 1; while (I_MASK & reg &
 * I_STAT): for sources 0..10 of the snapshot: I_STAT = ~(1 << i) (W0C),
 * then call tbl[i] when non-zero.  Then if (I_STAT & I_MASK) still pending,
 * a watchdog D_8009567C counts calls; past 0x800 it prints and clears
 * both the watchdog and I_STAT; otherwise the watchdog is cleared.
 * active = 0; func_80074384.  I_STAT/I_MASK go through their pointer
 * globals D_80095670/D_80095674 (routed to the PE_IRQ authority). */
void func_80073F00(void)
{
    const pe_addr_t b = 0x800945E4u;
    uint32_t m;
    uint32_t v;
    pe_addr_t pa, pb;

    if (PE_LoadU16(b) == 0u) {
        (void)PE_D_COMP_BOUNDARY2("func_80071A74", 0x80071A74u, 0x80011740u,
                                  pa_mmio_load16(PE_LoadU32(0x80095670u)));
        (void)PE_D_COMP_BOUNDARY0("func_80074384", 0x80074384u);
    }
    pa = PE_LoadU32(0x80095670u);
    PE_StoreU16(b + 2u, 1u);
    pb = PE_LoadU32(0x80095674u);
    v = pa_mmio_load16(pb) & (PE_LoadU16(b + 0x30u) & pa_mmio_load16(pa));
    m = v;
    while (v != 0u) {
        int i = 0;
        pe_addr_t q = b + 4u;

        while (m != 0u) {
            if (i >= 0xB)
                break;
            if ((m & 1u) != 0u) {
                pe_addr_t h;

                pa_mmio_store16(PE_LoadU32(0x80095670u),
                                (uint16_t)~(1u << i));
                h = PE_LoadU32(q);
                if (h != 0u)
                    (void)pa_call("func_80073F00_cpu_source", h, 0u, 0, 0, 0, 0);
            }
            q += 4u;
            m >>= 1;
            i++;
            if ((m & 0xFFFFu) == 0u)
                break;
        }
        pa = PE_LoadU32(0x80095670u);
        pb = PE_LoadU32(0x80095674u);
        v = pa_mmio_load16(pb) & (PE_LoadU16(0x80094614u) & pa_mmio_load16(pa));
        m = v;
    }
    pa = PE_LoadU32(0x80095670u);
    pb = PE_LoadU32(0x80095674u);
    if ((pa_mmio_load16(pa) & pa_mmio_load16(pb)) != 0u) {
        int c = (int)PE_LoadU32(0x8009567Cu);

        PE_StoreU32(0x8009567Cu, (uint32_t)(c + 1));
        if (c >= 0x801) {
            (void)PE_D_COMP_BOUNDARY3("func_80071A74", 0x80071A74u,
                                      0x8001175Cu, pa_mmio_load16(pa),
                                      pa_mmio_load16(pb));
            PE_StoreU32(0x8009567Cu, 0u);
            pa_mmio_store16(PE_LoadU32(0x80095670u), 0u);
        }
    } else {
        PE_StoreU32(0x8009567Cu, 0u);
    }
    PE_StoreU16(0x800945E6u, 0u);
    (void)PE_D_COMP_BOUNDARY0("func_80074384", 0x80074384u);
}

/* ── 0x800743B4 / 0x8007440C / 0x800744D4 ───────────────────────────
 * src/func_800743B4.c: timer-1 mode (*D_800956B0) = 0x107, dispatch count
 * D_800956AC = 0, clear the 8 VBlank slots D_8009568C, register
 * func_8007440C on CPU source 0, return the slot setter func_80074478. */
pe_addr_t func_800743B4(void)
{
    pa_mmio_store32(PE_LoadU32(0x800956B0u), 0x107u);
    PE_StoreU32(0x800956ACu, 0u);
    func_800744A4(0x8009568Cu, 8);
    (void)func_80073CC4(0u, 0x8007440Cu);
    return 0x80074478u;
}

/* src/func_8007440C.c: D_800956AC++, then call each non-zero slot of
 * D_8009568C[0..7] in order with no arguments.  pe_callback.c's
 * PE_Callback_Dispatch is the platform authority for exactly this body
 * (guest identity -> host binding; an unbound identity is a loud stop). */
void func_8007440C(void)
{
    PE_Callback_Dispatch();
}

/* src/func_800744D4.c: clear the 8 DMA callback slots D_800956C0,
 * *D_800956BC (DICR) = 0, register func_80074520 on CPU source 3, return
 * the DMA slot setter func_800746A0. */
pe_addr_t func_800744D4(void)
{
    func_8007474C(0x800956C0u, 8);
    pa_mmio_store32(PE_LoadU32(0x800956BCu), 0u);
    (void)func_80073CC4(3u, 0x80074520u);
    return 0x800746A0u;
}

/* ── 0x80074C14 / 0x80074CC8: libgpu setters ────────────────────────
 * src/func_80074C14.c: old = D_8009574D; debug level D_8009574E >= 2 ->
 * D_80095748(&D_80011840, a0); when a0 differs from D_8009574D (re-read)
 * call jtb[13](1), store a0, then func_80073CF4(2, 0).  Returns old. */
int func_80074C14(int a0)
{
    const pe_addr_t p = 0x8009574Du;
    int old = PE_LoadU8(p);

    if (PE_LoadU8(p + 1u) >= 2u)
        (void)pa_call("func_80074C14_debug_print", PE_LoadU32(0x80095748u),
                      2u, 0x80011840u, (uint32_t)a0, 0, 0);
    if (a0 != (int)PE_LoadU8(p)) {
        (void)pa_call("func_80074C14_jtb13",
                      PE_LoadU32(PE_LoadU32(0x80095744u) + 13u * 4u),
                      1u, 1u, 0, 0, 0);
        PE_StoreU8(p, (uint8_t)a0);
        (void)func_80073CF4(2u, 0u);
    }
    return old;
}

/* src/func_80074CC8.c: debug print as above (&D_80011854), then exchange
 * the word at D_8009574E + 0xA (D_80095758) with a0. */
/* func_80074CC8: ported from the matching decomp -- generated TU pc_port/game/decomp/func_80074CC8_port.c (src/func_80074CC8.c); hand port retired after differential verification (docs/evidence/portverify/REPORT.md, round 6). */

/* ── 0x80074FD4: ClearImage ─────────────────────────────────────────
 * src/func_80074FD4.c: validate/log the RECT at guest a0 through
 * func_80074E28(&D_800118C8, a0), then jtb[2](jtb[3], a0, 8,
 * 0x80000000 | b3<<16 | b2<<8 | b1).  The host func_80074E28 takes the
 * RECT by host pointer, so its four halfwords are read from guest a0. */
void func_80074FD4(int a0, unsigned char a1, unsigned char a2,
                   unsigned char a3)
{
    RECT r;
    pe_addr_t v1;
    pe_addr_t target;
    uint32_t word;

    r.x = (int16_t)PE_LoadU16((pe_addr_t)a0 + 0u);
    r.y = (int16_t)PE_LoadU16((pe_addr_t)a0 + 2u);
    r.w = (int16_t)PE_LoadU16((pe_addr_t)a0 + 4u);
    r.h = (int16_t)PE_LoadU16((pe_addr_t)a0 + 6u);
    func_80074E28(0x800118C8u, &r);
    v1 = PE_LoadU32(0x80095744u);
    word = ((uint32_t)a3 << 16) | 0x80000000u | ((uint32_t)a2 << 8) | a1;
    target = PE_LoadU32(v1 + 8u);
    if (target == 0x80076C34u)
        (void)func_80076C34(PE_LoadU32(v1 + 12u), (pe_addr_t)a0, 8, word);
    else
        (void)pa_call("func_80074FD4_jtb2", target, 4u, PE_LoadU32(v1 + 12u),
                      (uint32_t)a0, 8u, word);
}

/* ── 0x800751E4: ClearOTag-style link walk ──────────────────────────
 * src/func_800751E4.c: optional debug print (&D_800118F8, a0, a1); link
 * a1-1 consecutive words: byte +3 = 0, low 24 bits = next word's address
 * (high byte kept); seed D_8009580C = (&D_800957F8 & 0xFFFFFF) | 0x4000000
 * and terminate the last word with &D_8009580C & 0xFFFFFF.  Returns the
 * last word's address. */
pe_addr_t func_800751E4(pe_addr_t a0, int a1)
{
    pe_addr_t s0 = a0;
    int n;

    if (PE_LoadU8(0x8009574Eu) >= 2u)
        (void)pa_call("func_800751E4_debug_print", PE_LoadU32(0x80095748u),
                      3u, 0x800118F8u, s0, (uint32_t)a1, 0);
    n = a1 - 1;
    while (n != 0) {
        pe_addr_t next = s0 + 4u;

        n--;
        PE_StoreU8(s0 + 3u, 0u);
        PE_StoreU32(s0, (PE_LoadU32(s0) & 0xFF000000u) | (next & 0xFFFFFFu));
        s0 = next;
    }
    PE_StoreU32(0x8009580Cu, (0x800957F8u & 0xFFFFFFu) | 0x4000000u);
    PE_StoreU32(s0, 0x8009580Cu & 0xFFFFFFu);
    return s0;
}

/* ── 0x80075B1C ─────────────────────────────────────────────────────
 * src/func_80075B1C.c: jtb slot +0x38 (jtb[14]) called with no arguments;
 * return its result >> 31 (unsigned). */
unsigned int func_80075B1C(void)
{
    return (unsigned int)pa_call("func_80075B1C_jtb14",
                                 PE_LoadU32(PE_LoadU32(0x80095744u) + 0x38u),
                                 0u, 0, 0, 0, 0) >> 31;
}

/* ── 0x80075CE8: SetDrawEnv packet ──────────────────────────────────
 * src/func_80075CE8.c.  The draw-mode packers func_800762A0 /
 * func_80076150 / func_800762BC are `static` inside pe_libgpu.c, so their
 * matched C (src/func_800762A0.c, src/func_80076150.c,
 * src/func_800762BC.c) is transcribed here as local helpers.  ENV offsets:
 * +0 x, +2 y, +4 w, +6 h, +8 ox, +0xA oy, +0xC tw RECT, +0x14 tpage,
 * +0x16 dtd, +0x17 dfe, +0x18 isbg, +0x19..+0x1B r,g,b. */
static uint32_t pa_800762A0(int a0, int a1)
{
    int y = (a1 & 0x7FF) << 11;
    int x = (a0 & 0x7FF) | (int)0xE5000000u;
    return (uint32_t)(y | x);
}

static uint32_t pa_80076150(int arg0, int arg1, unsigned int arg2)
{
    return 0xE1000000u | (arg1 ? 0x200u : 0u) | (arg0 ? 0x400u : 0u) |
           (arg2 & 0x9FFu);
}

static uint32_t pa_800762BC(pe_addr_t tw)
{
    int code[4];

    if (tw == 0u)
        return 0u;
    code[0] = ((short)PE_LoadU16(tw + 0u) & 0xFF) >> 3;
    code[2] = ((-(short)PE_LoadU16(tw + 4u)) & 0xFF) >> 3;
    code[1] = ((short)PE_LoadU16(tw + 2u) & 0xFF) >> 3;
    code[3] = ((-(short)PE_LoadU16(tw + 6u)) & 0xFF) >> 3;
    return 0xE2000000u | ((uint32_t)code[1] << 15) | ((uint32_t)code[0] << 10) |
           ((uint32_t)code[3] << 5) | (uint32_t)code[2];
}

void func_80075CE8(pe_addr_t dd, pe_addr_t env)
{
    short ex = (short)PE_LoadU16(env + 0u);
    short ey = (short)PE_LoadU16(env + 2u);
    unsigned short ew = PE_LoadU16(env + 4u);
    unsigned short eh = PE_LoadU16(env + 6u);
    short ox = (short)PE_LoadU16(env + 8u);
    short oy = (short)PE_LoadU16(env + 0xAu);
    unsigned int tw, tx, ty, th;
    int aa, bb;
    int t;

    PE_StoreU32(dd + 4u, func_80076170(ex, ey));
    tw = ew;
    tx = (unsigned short)ex;
    ty = (unsigned short)ey;
    aa = (short)(tw + tx - 1u);
    th = eh;
    bb = (short)(ty + th - 1u);
    PE_StoreU32(dd + 8u, func_80076208((short)aa, (short)bb));
    PE_StoreU32(dd + 12u, pa_800762A0(ox, oy));
    PE_StoreU32(dd + 16u, pa_80076150(PE_LoadU8(env + 0x17u),
                                      PE_LoadU8(env + 0x16u),
                                      PE_LoadU16(env + 0x14u)));
    PE_StoreU32(dd + 20u, pa_800762BC(env + 0xCu));
    PE_StoreU32(dd + 24u, 0xE6000000u);
    t = 7;
    if (PE_LoadU8(env + 0x18u) != 0u) {
        short rx = ex, ry = ey;
        unsigned short rw = ew, rh = eh;
        unsigned short v;

        if ((short)rw < 0)
            v = 0;
        else if ((short)PE_LoadU16(0x80095750u) - 1 < (short)rw)
            v = (unsigned short)(PE_LoadU16(0x80095750u) - 1u);
        else
            v = rw;
        rw = v;
        if ((short)rh >= 0) {
            if ((short)PE_LoadU16(0x80095752u) - 1 < (short)rh)
                v = (unsigned short)(PE_LoadU16(0x80095752u) - 1u);
            else
                v = rh;
        } else {
            v = 0;
        }
        rh = v;
        rx = (short)(rx - ox);
        ry = (short)(ry - oy);
        PE_StoreU32(dd + (uint32_t)t * 4u,
                    0x60000000u | ((uint32_t)PE_LoadU8(env + 0x1Bu) << 16) |
                    ((uint32_t)PE_LoadU8(env + 0x1Au) << 8) |
                    PE_LoadU8(env + 0x19u));
        t++;
        PE_StoreU32(dd + (uint32_t)t * 4u,
                    (uint32_t)(unsigned short)rx | ((uint32_t)(unsigned short)ry << 16));
        t++;
        PE_StoreU32(dd + (uint32_t)t * 4u, (uint32_t)rw | ((uint32_t)rh << 16));
        t++;
    }
    PE_StoreU8(dd + 3u, (uint8_t)(t - 1));
}

/* ── 0x80090AEC / 0x80090B5C: sequence cursor opcodes ───────────────
 * src: byte = *cursor++; halfword +0xBA / +0xBC = byte ? byte + 1 : 0x101;
 * then func_8009019C / func_800902AC(a0).  Those callees take (a0, a1)
 * and the matched leaves declare them with one argument: the retail call
 * forwards the caller's live $a1, which these leaves never write
 * (pe_dis.sh 0x80090AEC 0x38 / 0x80090B5C 0x40: only v0/v1/sp/ra are
 * written before the jal), so a1 is spelled as a trailing parameter. */
void func_80090AEC(pe_addr_t a0, unsigned int a1)
{
    pe_addr_t p = PE_LoadU32(a0);
    int v;

    PE_StoreU32(a0, p + 1u);
    v = PE_LoadU8(p);
    PE_StoreU16(a0 + 0xBAu, (uint16_t)(v ? v + 1 : 0x101));
    func_8009019C(a0, a1);
}

void func_80090B5C(pe_addr_t a0, unsigned int a1)
{
    pe_addr_t p = PE_LoadU32(a0);
    int v;

    PE_StoreU32(a0, p + 1u);
    v = PE_LoadU8(p);
    PE_StoreU16(a0 + 0xBCu, (uint16_t)(v ? v + 1 : 0x101));
    func_800902AC(a0, a1);
}
