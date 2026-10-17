/*
 * Phase 6E-A — libgte geometry-engine control state.
 *
 * func_80077F7C = PsyQ InitGeom (asm/disc1/68478.s, handwritten asm).
 * Classification: 2/3 (SDK host implementation with deterministic constants).
 *
 * Collapsed retail effects (hardware-only, no guest-RAM state):
 *   - func_8007A134: GTE kernel patch — EnterCriticalSection, BIOS B(56h),
 *     compare/patch 6 kernel words, FlushCache, ExitCriticalSection.
 *     Host kernel needs no GTE workaround patch: no-op.
 *   - SR |= 0x40000000 (cop2 enable): host has no coprocessor SR: no-op.
 *
 * Reproduced retail effects: the seven ctc2 control-register writes.
 * cop2 registers have no guest-RAM address, so they live in g_pe_gte.
 */
#include "pe_sdk.h"
#include "pe_gte_commands.h"
#include "game_port.h"

PeGteState g_pe_gte;

void func_80077F7C(void)
{
    g_pe_gte.zsf3 = 0x155;      /* ctc2 $29 */
    g_pe_gte.zsf4 = 0x100;      /* ctc2 $30 */
    g_pe_gte.h    = 0x3E8;      /* ctc2 $26 */
    g_pe_gte.dqa  = -0x1062;    /* ctc2 $27 (0xFFFFEF9E) */
    g_pe_gte.dqb  = 0x1400000;  /* ctc2 $28 */
    g_pe_gte.ofx  = 0;          /* ctc2 $24 */
    g_pe_gte.ofy  = 0;          /* ctc2 $25 */
}

/* SetGeomOffset (asm leaf): OFX = a << 16, OFY = b << 16 */
void func_80079004(int a, int b)
{
    g_pe_gte.ofx = a << 16;
    g_pe_gte.ofy = b << 16;
}

/* SetGeomScreen (asm leaf): H = a */
void func_80079024(int a)
{
    g_pe_gte.h = a;
}

/* Phase 6E-B4 — exact 32-bit GTE LZCS/LZCR arithmetic (leading-sign-bit
 * count), factored out of the func_8003EAC8 translation.  No
 * __builtin_clz(0) undefined behavior or floating point. This arithmetic
 * helper is pure; SetLZCS below also retains the register side effects. */
uint32_t PE_GTE_LZCR(uint32_t v)
{
    uint32_t sign = v & 0x80000000u;
    uint32_t n = 0;
    while (n < 32u && (v & 0x80000000u) == sign) {
        n++;
        v <<= 1;
    }
    return n;
}

void PE_GTE_SetLZCS(uint32_t v)
{
    g_pe_gte.lzcs = v;
    g_pe_gte.lzcr = PE_GTE_LZCR(v);
}

static void pe_gte_load_m3(int16_t m[3][3], pe_addr_t matrix)
{
    m[0][0] = (int16_t)PE_LoadU16(matrix + 0u);
    m[0][1] = (int16_t)PE_LoadU16(matrix + 2u);
    m[0][2] = (int16_t)PE_LoadU16(matrix + 4u);
    m[1][0] = (int16_t)PE_LoadU16(matrix + 6u);
    m[1][1] = (int16_t)PE_LoadU16(matrix + 8u);
    m[1][2] = (int16_t)PE_LoadU16(matrix + 10u);
    m[2][0] = (int16_t)PE_LoadU16(matrix + 12u);
    m[2][1] = (int16_t)PE_LoadU16(matrix + 14u);
    m[2][2] = (int16_t)PE_LoadU16(matrix + 16u);
}

void PE_GTE_LoadRT(pe_addr_t matrix)
{
    pe_gte_load_m3(g_pe_gte.rt, matrix);
    g_pe_gte.tr[0] = (int32_t)PE_LoadU32(matrix + 20u);
    g_pe_gte.tr[1] = (int32_t)PE_LoadU32(matrix + 24u);
    g_pe_gte.tr[2] = (int32_t)PE_LoadU32(matrix + 28u);
}

void PE_GTE_LoadRT33(pe_addr_t matrix)
{
    pe_gte_load_m3(g_pe_gte.rt, matrix);
}

void PE_GTE_LoadLCM(pe_addr_t matrix)
{
    pe_gte_load_m3(g_pe_gte.lcm, matrix);
}

/* Original transition-init SDK leaves: 78E34/78E64 ctc2 matrix words,
 * 78FC4/78FE4 background/far colors, and 77E64 SetFogNearFar.
 * Far-color storage alone does not implement missing depth-cue commands. */
void func_80078E34(pe_addr_t matrix) { pe_gte_load_m3(g_pe_gte.llm, matrix); }
void func_80078E64(pe_addr_t matrix) { pe_gte_load_m3(g_pe_gte.lcm, matrix); }

void func_80078FC4(uint32_t r, uint32_t g, uint32_t b)
{
    g_pe_gte.bk[0]=(int32_t)(r<<4);g_pe_gte.bk[1]=(int32_t)(g<<4);g_pe_gte.bk[2]=(int32_t)(b<<4);
}

void func_80078FE4(uint32_t r, uint32_t g, uint32_t b)
{
    g_pe_gte.fc[0]=(int32_t)(r<<4);g_pe_gte.fc[1]=(int32_t)(g<<4);g_pe_gte.fc[2]=(int32_t)(b<<4);
}

void func_80077E64(uint32_t near_z, uint32_t far_z, int32_t h)
{
    int32_t span=(int32_t)(far_z-near_z);
    if (span<100) return;
    /* Each original shift/product uses only the low 32 bits. */
    int32_t first=(int32_t)((0u-near_z)*far_z)/span;
    int32_t offset=(int32_t)(far_z<<12)/span;
    int32_t numerator=(int32_t)((uint32_t)first<<8);
    if (!h || (numerator==INT32_MIN && h==-1)) {
        /* Original BREAK 7 / BREAK 6, before either coefficient write. */
        PE_Port_RequestStop(PE_PORT_STOP_UNRESOLVED_BOUNDARY);
        return;
    }
    int32_t slope=numerator/h;
    if (slope<-32768) slope=-32768;
    if (slope>32767) slope=32767;
    g_pe_gte.dqa=slope; /* original 78FAC: ctc2 $27 */
    g_pe_gte.dqb=(int32_t)((uint32_t)offset<<12); /* 78FB8: ctc2 $28 */
}

void PE_GTE_LoadLLM_halfs(const int16_t *halfs)
{
    g_pe_gte.llm[0][0] = halfs[0];
    g_pe_gte.llm[0][1] = halfs[1];
    g_pe_gte.llm[0][2] = halfs[2];
    g_pe_gte.llm[1][0] = halfs[3];
    g_pe_gte.llm[1][1] = halfs[4];
    g_pe_gte.llm[1][2] = halfs[5];
    g_pe_gte.llm[2][0] = halfs[6];
    g_pe_gte.llm[2][1] = halfs[7];
    g_pe_gte.llm[2][2] = halfs[8];
}

void PE_GTE_SetIR(int16_t ir1, int16_t ir2, int16_t ir3)
{
    g_pe_gte.ir[0] = ir1;
    g_pe_gte.ir[1] = ir2;
    g_pe_gte.ir[2] = ir3;
}

void PE_GTE_SetV0(int16_t vx, int16_t vy, int16_t vz)
{
    g_pe_gte.v0[0] = vx;
    g_pe_gte.v0[1] = vy;
    g_pe_gte.v0[2] = vz;
}

void PE_GTE_SetV1(int16_t vx, int16_t vy, int16_t vz)
{
    g_pe_gte.v1[0] = vx;
    g_pe_gte.v1[1] = vy;
    g_pe_gte.v1[2] = vz;
}

void PE_GTE_SetV2(int16_t vx, int16_t vy, int16_t vz)
{
    g_pe_gte.v2[0] = vx;
    g_pe_gte.v2[1] = vy;
    g_pe_gte.v2[2] = vz;
}

void PE_GTE_SetRGBC(uint32_t rgbc)
{
    g_pe_gte.rgbc = rgbc;
}

void PE_GTE_SetBK(int32_t rbk, int32_t gbk, int32_t bbk)
{
    g_pe_gte.bk[0] = rbk;
    g_pe_gte.bk[1] = gbk;
    g_pe_gte.bk[2] = bbk;
}

/* Every helper uses the same compiled native arithmetic as command calls.
 * MVMVA short forms omit the opcode; retain their selector bits and force 12h.
 * These helpers no longer skip selectors or retain another command's FLAG. */
void PE_GTE_MVMVA(uint32_t cmd)
{
    PE_GTE_Command((cmd & ~0x3Fu) | 0x12u);
}

void PE_GTE_NCCT(void) { PE_GTE_Command(0x118043Fu); }
void PE_GTE_GPF(int sf, int lm)
{
    PE_GTE_Command(0x3Du | ((uint32_t)(sf != 0) << 19) |
                   ((uint32_t)(lm != 0) << 10));
}
void PE_GTE_GPL(int sf, int lm)
{
    PE_GTE_Command(0x3Eu | ((uint32_t)(sf != 0) << 19) |
                   ((uint32_t)(lm != 0) << 10));
}
void PE_GTE_OP(int sf, int lm)
{
    PE_GTE_Command(0x0Cu | ((uint32_t)(sf != 0) << 19) |
                   ((uint32_t)(lm != 0) << 10));
}

static void pe_gte_mac0_flags(int64_t value, uint32_t *flags)
{
    if (value > INT32_MAX) *flags |= 1u << 16;
    if (value < INT32_MIN) *flags |= 1u << 15;
}

void PE_GTE_RTPS_coordinates(uint32_t *xy, uint32_t *z)
{
    PE_GTE_Command(0x180001u);
    *xy = g_pe_gte.sxy[2];
    *z = g_pe_gte.sz[3];
}

void PE_GTE_RTPT_coordinates(uint32_t xy[3], uint32_t z[3])
{
    PE_GTE_Command(0x280030u);
    for (unsigned i = 0; i < 3u; ++i) {
        xy[i] = g_pe_gte.sxy[i];
        z[i] = g_pe_gte.sz[i + 1u];
    }
}

/* AVSZ starts a new FLAG result: preceding RTPT projection/IR0 flags do
 * not survive. Retail AVSZ3/4 and the retained full-callee oracle use the
 * wide product for MAC0 overflow and OTZ saturation, before MAC0 truncates.
 * Same rule as the preserved native PE_GTE_AvszFlag implementation. */
static uint32_t pe_gte_avsz_flags(int64_t value)
{
    uint32_t flags = 0;
    int64_t depth = value >> 12;
    pe_gte_mac0_flags(value, &flags);
    if (depth < 0 || depth > 65535) flags |= 1u << 18;
    return flags | ((flags & 0x7F87E000u) ? 0x80000000u : 0u);
}

void PE_GTE_AVSZ3(const uint32_t z[3])
{
    int64_t value=(int64_t)(int16_t)g_pe_gte.zsf3*(z[0]+z[1]+z[2]);
    g_pe_gte.projection_flags=pe_gte_avsz_flags(value);
    g_pe_gte.mac0=(int32_t)value;
    value>>=12;
    g_pe_gte.otz=value<0?0u:value>65535?65535u:(uint32_t)value;
}

void PE_GTE_AVSZ4(const uint32_t z[4])
{
    int64_t value=(int64_t)(int16_t)g_pe_gte.zsf4*(z[0]+z[1]+z[2]+z[3]);
    g_pe_gte.projection_flags=pe_gte_avsz_flags(value);
    g_pe_gte.mac0=(int32_t)value;
    value>>=12;
    g_pe_gte.otz=value<0?0u:value>65535?65535u:(uint32_t)value;
}

int32_t PE_GTE_NCLIP(void)
{
    PE_GTE_Command(0x1400006u);
    return g_pe_gte.mac0;
}

/* ── register-level COP2 access (E21b: lowered inline GTE asm) ─────────
 * psx-spx "GTE Registers": data registers $0-$31 (mtc2/mfc2/lwc2/swc2) and
 * control registers $0-$31 (ctc2/cfc2) over g_pe_gte.  Matrices are packed
 * two 16-bit elements per register, the ninth element alone (sign-extended
 * on read).  H reads back sign-extended (hardware quirk). */
#include <stdio.h>
#include <stdlib.h>

static uint32_t g_pe_gte_res1;

static uint32_t pe_gte_pack(int16_t lo, int16_t hi)
{
    return (uint32_t)(uint16_t)lo | ((uint32_t)(uint16_t)hi << 16);
}

static uint32_t pe_gte_sx16(int32_t v)
{
    return (uint32_t)(int32_t)(int16_t)v;
}

static uint32_t pe_gte_orgb(void)
{
    uint32_t out = 0;
    int i;
    for (i = 0; i < 3; i++) {
        int32_t c = g_pe_gte.ir[i] >> 7;
        if (c < 0) c = 0;
        if (c > 0x1F) c = 0x1F;
        out |= (uint32_t)c << (5 * i);
    }
    return out;
}

uint32_t PE_GTE_ReadData(uint32_t reg)
{
    reg &= 31u;
    switch (reg) {
    case 0: return pe_gte_pack(g_pe_gte.v0[0], g_pe_gte.v0[1]);
    case 1: return pe_gte_sx16(g_pe_gte.v0[2]);
    case 2: return pe_gte_pack(g_pe_gte.v1[0], g_pe_gte.v1[1]);
    case 3: return pe_gte_sx16(g_pe_gte.v1[2]);
    case 4: return pe_gte_pack(g_pe_gte.v2[0], g_pe_gte.v2[1]);
    case 5: return pe_gte_sx16(g_pe_gte.v2[2]);
    case 6: return g_pe_gte.rgbc;
    case 7: return g_pe_gte.otz & 0xFFFFu;
    case 8: return pe_gte_sx16(g_pe_gte.ir0);
    case 9: case 10: case 11: return pe_gte_sx16(g_pe_gte.ir[reg - 9u]);
    case 12: case 13: case 14: return g_pe_gte.sxy[reg - 12u];
    case 15: return g_pe_gte.sxy[2];
    case 16: case 17: case 18: case 19: return g_pe_gte.sz[reg - 16u] & 0xFFFFu;
    case 20: case 21: case 22: return g_pe_gte.rgb_fifo[reg - 20u];
    case 23: return g_pe_gte_res1;
    case 24: return (uint32_t)g_pe_gte.mac0;
    case 25: case 26: case 27: return (uint32_t)g_pe_gte.mac[reg - 25u];
    case 28: case 29: return pe_gte_orgb();
    case 30: return g_pe_gte.lzcs;
    default: return g_pe_gte.lzcr;
    }
}

void PE_GTE_WriteData(uint32_t reg, uint32_t v)
{
    reg &= 31u;
    switch (reg) {
    case 0: g_pe_gte.v0[0] = (int16_t)v; g_pe_gte.v0[1] = (int16_t)(v >> 16); break;
    case 1: g_pe_gte.v0[2] = (int16_t)v; break;
    case 2: g_pe_gte.v1[0] = (int16_t)v; g_pe_gte.v1[1] = (int16_t)(v >> 16); break;
    case 3: g_pe_gte.v1[2] = (int16_t)v; break;
    case 4: g_pe_gte.v2[0] = (int16_t)v; g_pe_gte.v2[1] = (int16_t)(v >> 16); break;
    case 5: g_pe_gte.v2[2] = (int16_t)v; break;
    case 6: g_pe_gte.rgbc = v; break;
    case 7: g_pe_gte.otz = v & 0xFFFFu; break;
    case 8: g_pe_gte.ir0 = (int16_t)v; break;
    case 9: case 10: case 11: g_pe_gte.ir[reg - 9u] = (int16_t)v; break;
    case 12: case 13: case 14: g_pe_gte.sxy[reg - 12u] = v; break;
    case 15:
        g_pe_gte.sxy[0] = g_pe_gte.sxy[1];
        g_pe_gte.sxy[1] = g_pe_gte.sxy[2];
        g_pe_gte.sxy[2] = v;
        break;
    case 16: case 17: case 18: case 19: g_pe_gte.sz[reg - 16u] = v & 0xFFFFu; break;
    case 20: case 21: case 22: g_pe_gte.rgb_fifo[reg - 20u] = v; break;
    case 23: g_pe_gte_res1 = v; break;
    case 24: g_pe_gte.mac0 = (int32_t)v; break;
    case 25: case 26: case 27: g_pe_gte.mac[reg - 25u] = (int32_t)v; break;
    case 28:
        g_pe_gte.ir[0] = (int32_t)((v & 0x1Fu) << 7);
        g_pe_gte.ir[1] = (int32_t)(((v >> 5) & 0x1Fu) << 7);
        g_pe_gte.ir[2] = (int32_t)(((v >> 10) & 0x1Fu) << 7);
        break;
    case 30: PE_GTE_SetLZCS(v); break;
    default: break;                       /* ORGB / LZCR are read-only */
    }
}

static int16_t *pe_gte_matrix(uint32_t base)
{
    return base == 0u ? &g_pe_gte.rt[0][0] : base == 8u ? &g_pe_gte.llm[0][0]
                                                         : &g_pe_gte.lcm[0][0];
}

static int32_t *pe_gte_vec(uint32_t base)
{
    return base == 5u ? g_pe_gte.tr : base == 13u ? g_pe_gte.bk : g_pe_gte.fc;
}

uint32_t PE_GTE_ReadCtrl(uint32_t reg)
{
    reg &= 31u;
    if (reg < 24u) {
        uint32_t grp = reg / 8u * 8u, i = reg - grp;
        if (i < 5u) {
            int16_t *m = pe_gte_matrix(grp);
            return i < 4u ? pe_gte_pack(m[2u * i], m[2u * i + 1u]) : pe_gte_sx16(m[8]);
        }
        return (uint32_t)pe_gte_vec(grp + 5u)[i - 5u];
    }
    switch (reg) {
    case 24: return (uint32_t)g_pe_gte.ofx;
    case 25: return (uint32_t)g_pe_gte.ofy;
    case 26: return pe_gte_sx16(g_pe_gte.h);
    case 27: return pe_gte_sx16(g_pe_gte.dqa);
    case 28: return (uint32_t)g_pe_gte.dqb;
    case 29: return pe_gte_sx16(g_pe_gte.zsf3);
    case 30: return pe_gte_sx16(g_pe_gte.zsf4);
    default: return g_pe_gte.projection_flags;
    }
}

void PE_GTE_WriteCtrl(uint32_t reg, uint32_t v)
{
    reg &= 31u;
    if (reg < 24u) {
        uint32_t grp = reg / 8u * 8u, i = reg - grp;
        if (i < 5u) {
            int16_t *m = pe_gte_matrix(grp);
            m[2u * i] = (int16_t)v;
            if (i < 4u)
                m[2u * i + 1u] = (int16_t)(v >> 16);
        } else {
            pe_gte_vec(grp + 5u)[i - 5u] = (int32_t)v;
        }
        return;
    }
    switch (reg) {
    case 24: g_pe_gte.ofx = (int32_t)v; break;
    case 25: g_pe_gte.ofy = (int32_t)v; break;
    case 26: g_pe_gte.h = (int32_t)(v & 0xFFFFu); break;
    case 27: g_pe_gte.dqa = (int16_t)v; break;
    case 28: g_pe_gte.dqb = (int32_t)v; break;
    case 29: g_pe_gte.zsf3 = (int16_t)v; break;
    case 30: g_pe_gte.zsf4 = (int16_t)v; break;
    default: {
        uint32_t f = v & 0x7FFFF000u;
        g_pe_gte.projection_flags = f | ((f & 0x7F87E000u) ? 0x80000000u : 0u);
        break;
    }
    }
}

void PE_GTE_Command(uint32_t cmd)
{
    if (PE_GTE_ExecuteNative(cmd) == 0)
        return;
    fprintf(stderr, "FATAL: PE_GTE_Command: unsupported cop2 command 0x%07X (native only)\n", cmd);
    fflush(stderr);
    abort();
}
