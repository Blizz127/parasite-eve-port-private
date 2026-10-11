#!/usr/bin/env python3
"""E21 differential: lowered room TUs vs the retail MIPS code they came from.

For every generated E21 room TU (tools/analysis/guest_lower.py) whose only
calls are loud boundaries or retail `jalr`s (both are mocked identically on
the two sides), this

  1. loads the extracted retail room blob at its VMA into a 2 MiB guest RAM
     image pre-filled from a seed (every word a 4-aligned guest address, so
     pointer chains stay inside RAM; integer fields just see big values),
  2. runs the retail leaf in a small MIPS-I interpreter (delay slots, the
     era's lwl/lwr/swl/swr, mult/div), mocking every jal/jalr target with a
     recorded call returning a seed-derived value,
  3. runs the lowered native C on the same image with the same mocks,
  4. compares guest RAM (outside both stacks), the return value and the
     ordered call log.

Inline GTE asm (E21b) is covered too: the interpreter carries an independent
Python COP2 model (psx-spx register semantics, MVMVA mx=RT, NCLIP) and the
native side links the real pc_port/platform/pe_gte.c.  The 1 KiB scratchpad
(0x1F800000) is mapped and compared on both sides.

Cases where the retail code itself faults (address error, `break`, an
unsupported cop2 op) are skipped, not counted.  Retail blobs come from build/extracted (never
committed).  Usage: e21_diff.py [--seeds N] [--limit K] [--jobs J] [--verbose]
"""
from __future__ import annotations

import argparse
import concurrent.futures
import os
import random
import re
import shutil
import struct
import subprocess
import sys
import tempfile
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import gen_overlay_ports as o  # noqa: E402

RAM_BASE, RAM_SIZE = 0x80000000, 0x200000
SPAD = 0x400                             # scratchpad, stored after RAM
SENTINEL = 0x8000DEAC
SP0 = 0x801FFF00
EXCLUDE = (0x801FE000, 0x80200000)      # native frame arena + retail stack
MAX_STEPS = 200000
COVER: set | None = None                # retail pcs executed (--coverage)


class Fault(Exception):
    pass


def s32(v):
    v &= 0xFFFFFFFF
    return v - 0x100000000 if v & 0x80000000 else v


def mock_ret(target: int, seed: int) -> int:
    h = (target * 2654435761 + seed * 40503) & 0xFFFFFFFF
    if ((seed * 2654435761) & 0xFFFFFFFF) >> 16 & 1:   # half the seeds (hashed, so it does not
                                        # couple with the directed combinations): a small signed value (both signs of
        return ((h & 0x1FFF) - 0x1000) & 0xFFFFFFFF   # every division bias get exercised)
    return 0x80100000 + (h & 0xDFFFC)   # stays below both stack windows


def sx16(v):
    v &= 0xFFFF
    return (v - 0x10000 if v & 0x8000 else v) & 0xFFFFFFFF


def s16(v):
    v &= 0xFFFF
    return v - 0x10000 if v & 0x8000 else v


class Gte:
    """Independent COP2 model (psx-spx 'GTE Registers' / MVMVA / NCLIP)."""

    def __init__(self):
        self.d = [0] * 32
        self.c = [0] * 32

    def wd(self, r, v):
        v &= 0xFFFFFFFF
        if r in (1, 3, 5, 8, 9, 10, 11):
            self.d[r] = sx16(v)
        elif r in (7, 16, 17, 18, 19):
            self.d[r] = v & 0xFFFF
        elif r == 15:
            self.d[12], self.d[13], self.d[14] = self.d[13], self.d[14], v
        elif r == 28:
            for i in range(3):
                self.d[9 + i] = ((v >> (5 * i)) & 0x1F) * 0x80
        elif r == 30:
            self.d[30] = v
            sign = v >> 31
            n = 0
            while n < 32 and ((v >> (31 - n)) & 1) == sign:
                n += 1
            self.d[31] = n
        elif r in (29, 31):
            pass
        else:
            self.d[r] = v

    def rd(self, r):
        if r == 15:
            return self.d[14]
        if r in (28, 29):
            out = 0
            for i in range(3):
                c = s32(self.d[9 + i]) >> 7
                out |= min(max(c, 0), 0x1F) << (5 * i)
            return out
        return self.d[r]

    def wc(self, r, v):
        v &= 0xFFFFFFFF
        if r in (4, 12, 20, 26, 27, 29, 30):
            self.c[r] = sx16(v)
        elif r == 31:
            f = v & 0x7FFFF000
            self.c[31] = f | (0x80000000 if f & 0x7F87E000 else 0)
        else:
            self.c[r] = v

    def rc(self, r):
        return self.c[r]

    def command(self, cmd):
        op = cmd & 0x3F
        if op == 0x06:
            sx = [s16(self.d[12 + i]) for i in range(3)]
            sy = [s16(self.d[12 + i] >> 16) for i in range(3)]
            m = sx[0] * sy[1] + sx[1] * sy[2] + sx[2] * sy[0] - sx[0] * sy[2] - sx[1] * sy[0] - sx[2] * sy[1]
            self.d[24] = m & 0xFFFFFFFF
            return
        if op != 0x12:
            raise Fault(f"cop2 command {cmd:07X}")
        sf, mx, vs, cv, lm = (cmd >> 19) & 1, (cmd >> 17) & 3, (cmd >> 15) & 3, (cmd >> 13) & 3, (cmd >> 10) & 1
        if mx != 0 or vs not in (0, 3) or cv not in (0, 3):
            raise Fault(f"cop2 mvmva {cmd:07X}")
        m = []
        for i in range(4):
            m += [s16(self.c[i]), s16(self.c[i] >> 16)]
        m.append(s16(self.c[4]))
        if vs == 0:
            v = [s16(self.d[0]), s16(self.d[0] >> 16), s16(self.d[1])]
        else:
            v = [s16(self.d[9]), s16(self.d[10]), s16(self.d[11])]
        t = [s32(self.c[5 + i]) for i in range(3)] if cv == 0 else [0, 0, 0]
        for i in range(3):
            mac = (t[i] << 12) + m[3 * i] * v[0] + m[3 * i + 1] * v[1] + m[3 * i + 2] * v[2]
            mac >>= 12 * sf
            mac32 = s32(mac)
            self.d[25 + i] = mac32 & 0xFFFFFFFF
            lo = 0 if lm else -0x8000
            self.d[9 + i] = min(max(mac, lo), 0x7FFF) & 0xFFFFFFFF


OUT_LEN_SHIFT = 40                      # out=FN:K:LEN keeps LEN above the FN<<4|K key


def out_key(v: int) -> int:
    return v & ((1 << OUT_LEN_SHIFT) - 1)


def out_len(v: int) -> int:
    return (v >> OUT_LEN_SHIFT) or 8


def out_bytes(fn: int, k: int, seed: int, n: int = 8) -> bytes:
    """The bytes (8 by default) a mocked callee writes through an `out=` stack argument (both sides)."""
    x, b = (fn * 2654435761 + seed * 40503 + k * 0x9E37) & 0xFFFFFFFF, bytearray()
    for _ in range(n):
        x = (x * 1103515245 + 12345) & 0xFFFFFFFF
        b.append((x >> 16) & 0xFF)
    return bytes(b)


def run_mips(ram: bytearray, entry: int, args: list[int], seed: int, outs: frozenset = frozenset()):
    r = [0] * 32
    r[4:8] = args
    r[29] = SP0
    r[31] = SENTINEL
    r[28] = 0x8009CD70
    hi = lo = 0
    pc, npc = entry, entry + 4
    calls = []
    gte = Gte()

    def addr(a, n):
        if a & (n - 1):
            raise Fault("unaligned")
        if (a & 0x1FFFFC00) == 0x1F800000:
            return RAM_SIZE + (a & 0x3FF)
        off = (a & 0x1FFFFFFF)
        if off + n > RAM_SIZE:
            raise Fault(f"bad address {a:08X}")
        return off

    def ld(a, n, signed):
        off = addr(a, n)
        v = int.from_bytes(ram[off:off + n], "little")
        if signed and v & (1 << (8 * n - 1)):
            v -= 1 << (8 * n)
        return v & 0xFFFFFFFF

    written: set[int] = set()

    def st(a, n, v, mark=True):
        off = addr(a, n)
        ram[off:off + n] = (v & ((1 << (8 * n)) - 1)).to_bytes(n, "little")
        if mark and 0x1FF000 <= off < RAM_SIZE:   # register spills (sw s0-s7/fp/ra, x(sp)) are not data
            written.update(range(off, off + n))

    steps = 0
    while True:
        steps += 1
        if steps > MAX_STEPS:
            raise Fault("step limit")
        if pc == SENTINEL:
            return r[2], calls
        if COVER is not None:
            COVER.add(pc)
        w = ld(pc, 4, False)
        op, rs, rt, rd = w >> 26, (w >> 21) & 31, (w >> 16) & 31, (w >> 11) & 31
        sh, fn, imm = (w >> 6) & 31, w & 63, w & 0xFFFF
        simm = imm - 0x10000 if imm & 0x8000 else imm
        cur, pc, npc = pc, npc, npc + 4
        A, B = r[rs], r[rt]
        wr = None
        if op == 0:
            if fn == 0x00: wr = (rd, B << sh)
            elif fn == 0x02: wr = (rd, B >> sh)
            elif fn == 0x03: wr = (rd, s32(B) >> sh)
            elif fn == 0x04: wr = (rd, B << (A & 31))
            elif fn == 0x06: wr = (rd, B >> (A & 31))
            elif fn == 0x07: wr = (rd, s32(B) >> (A & 31))
            elif fn == 0x08:                                   # jr
                npc = A
            elif fn == 0x09:                                   # jalr
                tgt = A
                # mocked callee: the delay slot runs first (it may set an
                # argument register), then the call is recorded and "returns"
                pend_ret = mock_ret(tgt, seed)
                run_slot(ram, r, pc, ld, st)
                calls.append((tgt, tuple(r[4:8])))
                r[2] = pend_ret
                pc, npc = cur + 8, cur + 12
                r[rd] = cur + 8 if rd else 0
                r[0] = 0
                continue
            elif fn == 0x0C: raise Fault("syscall")
            elif fn == 0x0D: raise Fault("break")
            elif fn == 0x10: wr = (rd, hi)
            elif fn == 0x11: hi = A
            elif fn == 0x12: wr = (rd, lo)
            elif fn == 0x13: lo = A
            elif fn == 0x18:
                p = s32(A) * s32(B); lo, hi = p & 0xFFFFFFFF, (p >> 32) & 0xFFFFFFFF
            elif fn == 0x19:
                p = A * B; lo, hi = p & 0xFFFFFFFF, (p >> 32) & 0xFFFFFFFF
            elif fn == 0x1A:
                if B == 0: raise Fault("div0")
                a, b = s32(A), s32(B)
                q = abs(a) // abs(b) * (1 if (a >= 0) == (b >= 0) else -1)
                lo, hi = q & 0xFFFFFFFF, (a - q * b) & 0xFFFFFFFF
            elif fn == 0x1B:
                if B == 0: raise Fault("div0")
                lo, hi = A // B, A % B
            elif fn in (0x20, 0x21): wr = (rd, A + B)
            elif fn in (0x22, 0x23): wr = (rd, A - B)
            elif fn == 0x24: wr = (rd, A & B)
            elif fn == 0x25: wr = (rd, A | B)
            elif fn == 0x26: wr = (rd, A ^ B)
            elif fn == 0x27: wr = (rd, ~(A | B))
            elif fn == 0x2A: wr = (rd, int(s32(A) < s32(B)))
            elif fn == 0x2B: wr = (rd, int(A < B))
            else: raise Fault(f"special {fn:#x}")
        elif op == 1:
            if rt in (0x10, 0x11):
                raise Fault("bltzal/bgezal")
            take = (s32(A) < 0) if rt == 0 else (s32(A) >= 0) if rt == 1 else None
            if take is None: raise Fault("regimm")
            if take: npc = cur + 4 + (simm << 2)
        elif op == 2:
            npc = (cur & 0xF0000000) | ((w & 0x3FFFFFF) << 2)
        elif op == 3:
            tgt = (cur & 0xF0000000) | ((w & 0x3FFFFFF) << 2)
            run_slot(ram, r, pc, ld, st)
            sp = (r[29] + 16) & 0x1FFFFFFF
            av = tuple(r[4:8]) + struct.unpack_from("<8I", ram, sp)
            # stack-local arguments: frames differ between the sides, so the
            # pointed-to bytes (at call time) are compared instead of the address
            calls.append((tgt, tuple(("S", bytes(ram[(v & 0x1FFFFFFF):(v & 0x1FFFFFFF) + 16]),
                                      [((v & 0x1FFFFFFF) + j) in written for j in range(16)], v)
                                     if 0x801FF000 <= v < SP0 + 0x100 else v for v in av)))
            for k in range(len(av)):
                n_ = next((out_len(v) for v in outs if out_key(v) == (tgt << 4 | k)), 0)
                if n_ and 0x801FF000 <= av[k] < SP0 + 0x100:
                    o_ = av[k] & 0x1FFFFFFF
                    ram[o_:o_ + n_] = out_bytes(tgt, k, seed, n_)
                    written.update(range(o_, o_ + n_))
            r[2] = mock_ret(tgt, seed)
            r[31] = cur + 8
            pc, npc = cur + 8, cur + 12
            continue
        elif op == 4:
            if A == B: npc = cur + 4 + (simm << 2)
        elif op == 5:
            if A != B: npc = cur + 4 + (simm << 2)
        elif op == 6:
            if s32(A) <= 0: npc = cur + 4 + (simm << 2)
        elif op == 7:
            if s32(A) > 0: npc = cur + 4 + (simm << 2)
        elif op in (8, 9): wr = (rt, A + simm)
        elif op == 10: wr = (rt, int(s32(A) < simm))
        elif op == 11: wr = (rt, int(A < (simm & 0xFFFFFFFF)))
        elif op == 12: wr = (rt, A & imm)
        elif op == 13: wr = (rt, A | imm)
        elif op == 14: wr = (rt, A ^ imm)
        elif op == 15: wr = (rt, imm << 16)
        elif op == 0x12:                                        # COP2
            if w & (1 << 25):
                gte.command(w & 0x1FFFFFF)
            elif rs == 0: wr = (rt, gte.rd(rd))                 # mfc2
            elif rs == 2: wr = (rt, gte.rc(rd))                 # cfc2
            elif rs == 4: gte.wd(rd, B)                         # mtc2
            elif rs == 6: gte.wc(rd, B)                         # ctc2
            else: raise Fault("cop2 form")
        elif op == 0x32: gte.wd(rt, ld(A + simm, 4, False))     # lwc2
        elif op == 0x3A: st(A + simm, 4, gte.rd(rt))            # swc2
        elif op == 0x20: wr = (rt, ld(A + simm, 1, True))
        elif op == 0x21: wr = (rt, ld(A + simm, 2, True))
        elif op == 0x23: wr = (rt, ld(A + simm, 4, False))
        elif op == 0x24: wr = (rt, ld(A + simm, 1, False))
        elif op == 0x25: wr = (rt, ld(A + simm, 2, False))
        elif op in (0x22, 0x26):                                # lwl / lwr
            a = (A + simm) & 0xFFFFFFFF
            al = ld(a & ~3, 4, False)
            k = a & 3
            if op == 0x22:
                sh_ = (3 - k) * 8
                v = (B & (0xFFFFFFFF >> (32 - sh_) if sh_ else 0)) | ((al << sh_) & 0xFFFFFFFF)
            else:
                sh_ = k * 8
                v = (B & ~(0xFFFFFFFF >> sh_) & 0xFFFFFFFF) | (al >> sh_)
            wr = (rt, v)
        elif op == 0x28: st(A + simm, 1, B)
        elif op == 0x29: st(A + simm, 2, B)
        elif op == 0x2B: st(A + simm, 4, B, not (rs == 29 and (16 <= rt <= 23 or rt >= 30)))
        elif op in (0x2A, 0x2E):                                # swl / swr
            a = (A + simm) & 0xFFFFFFFF
            al = ld(a & ~3, 4, False)
            k = a & 3
            if op == 0x2A:
                sh_ = (3 - k) * 8
                mask = 0xFFFFFFFF >> sh_
                v = (al & ~mask & 0xFFFFFFFF) | (B >> sh_)
            else:
                sh_ = k * 8
                mask = (0xFFFFFFFF << sh_) & 0xFFFFFFFF
                v = (al & ~mask & 0xFFFFFFFF) | ((B << sh_) & 0xFFFFFFFF)
            st(a & ~3, 4, v)
        else:
            raise Fault(f"opcode {op:#x}")
        if wr is not None and wr[0]:
            r[wr[0]] = wr[1] & 0xFFFFFFFF
        r[0] = 0
        if npc != pc + 4 and cur + 4 == pc:
            pass


def run_slot(ram, r, pc, ld, st):
    """Execute the single delay-slot instruction of a jal/jalr (ALU/load/store)."""
    w = ld(pc, 4, False)
    op, rs, rt, rd = w >> 26, (w >> 21) & 31, (w >> 16) & 31, (w >> 11) & 31
    sh, fn, imm = (w >> 6) & 31, w & 63, w & 0xFFFF
    simm = imm - 0x10000 if imm & 0x8000 else imm
    A, B = r[rs], r[rt]
    v = None
    if w == 0:
        return
    if op == 0 and fn == 0x21: v = (rd, A + B)
    elif op == 0 and fn == 0x25: v = (rd, A | B)
    elif op == 0 and fn == 0x23: v = (rd, A - B)
    elif op == 0 and fn == 0x00: v = (rd, B << sh)
    elif op == 0 and fn == 0x02: v = (rd, B >> sh)
    elif op == 0 and fn == 0x03: v = (rd, s32(B) >> sh)
    elif op == 0 and fn == 0x24: v = (rd, A & B)
    elif op == 0 and fn == 0x2A: v = (rd, int(s32(A) < s32(B)))
    elif op == 0 and fn == 0x2B: v = (rd, int(A < B))
    elif op in (8, 9): v = (rt, A + simm)
    elif op == 10: v = (rt, int(s32(A) < simm))
    elif op == 12: v = (rt, A & imm)
    elif op == 13: v = (rt, A | imm)
    elif op == 15: v = (rt, imm << 16)
    elif op == 0x20: v = (rt, ld(A + simm, 1, True))
    elif op == 0x21: v = (rt, ld(A + simm, 2, True))
    elif op == 0x23: v = (rt, ld(A + simm, 4, False))
    elif op == 0x24: v = (rt, ld(A + simm, 1, False))
    elif op == 0x25: v = (rt, ld(A + simm, 2, False))
    elif op == 0x28: st(A + simm, 1, B)
    elif op == 0x29: st(A + simm, 2, B)
    elif op == 0x2B: st(A + simm, 4, B, not (rs == 29 and (16 <= rt <= 23 or rt >= 30)))
    else:
        raise Fault(f"delay-slot op {w:08X}")
    if v is not None and v[0]:
        r[v[0]] = v[1] & 0xFFFFFFFF
    r[0] = 0


MOCK_H = r"""
#pragma once
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
typedef uint32_t pe_addr_t;
extern uint8_t ram[0x200400];
static inline uint8_t *at(pe_addr_t a) { if ((a & 0x1FFFFC00u) == 0x1F800000u) return ram + 0x200000u + (a & 0x3FFu); if ((a & 0x1FFFFFFFu) + 4u > 0x200000u) { printf("FAULT %08X\n", a); exit(3); } return ram + (a & 0x1FFFFFFFu); }
uint32_t PE_GTE_ReadData(uint32_t reg);
void PE_GTE_WriteData(uint32_t reg, uint32_t value);
uint32_t PE_GTE_ReadCtrl(uint32_t reg);
void PE_GTE_WriteCtrl(uint32_t reg, uint32_t value);
void PE_GTE_Command(uint32_t cmd);
static inline void PE_Decomp_GuestTrapDivZero(const char *fn) { (void)fn; }
static inline uint8_t  PE_LoadU8(pe_addr_t a)  { return *at(a); }
static inline uint16_t PE_LoadU16(pe_addr_t a) { uint16_t v; memcpy(&v, at(a), 2); return v; }
static inline uint32_t PE_LoadU32(pe_addr_t a) { uint32_t v; memcpy(&v, at(a), 4); return v; }
static inline void PE_StoreU8(pe_addr_t a, uint8_t v)   { *at(a) = v; }
static inline void PE_StoreU16(pe_addr_t a, uint16_t v) { memcpy(at(a), &v, 2); }
static inline void PE_StoreU32(pe_addr_t a, uint32_t v) { memcpy(at(a), &v, 4); }
static inline void PE_Fill(pe_addr_t a, uint32_t n, uint8_t v) { memset(at(a), v, n); }
static inline void *PE_Translate(pe_addr_t a, size_t n) { (void)n; return at(a); }
pe_addr_t PE_GuestFrame_Enter(uint32_t n);
void PE_GuestFrame_Leave(pe_addr_t b, uint32_t n);
uint32_t mock_call(uint32_t fn, unsigned n, uint32_t a0, uint32_t a1, uint32_t a2, uint32_t a3);
static inline int PE_GuestCall(const char *s, pe_addr_t fn, unsigned n, uintptr_t a0, uintptr_t a1, uintptr_t a2, uintptr_t a3)
{ (void)s; return (int)mock_call(fn, n, (uint32_t)a0, (uint32_t)a1, (uint32_t)a2, (uint32_t)a3); }
#define PE_D_COMP_A(_1, _2, _3, _4, ...) (uint32_t)(uintptr_t)(_1), (uint32_t)(uintptr_t)(_2), (uint32_t)(uintptr_t)(_3), (uint32_t)(uintptr_t)(_4)
static inline void PE_Decomp_GuestTrapDivZero(const char *fn) { (void)fn; printf("GUEST_TRAP\n"); exit(4); }
#define PE_D_COMP_BOUNDARY0(s, v) (int)mock_call(v, 0u, 0, 0, 0, 0)
#define PE_D_COMP_BOUNDARY1(s, v, ...) (int)mock_call(v, 1u, PE_D_COMP_A(__VA_ARGS__, 0, 0, 0, 0))
#define PE_D_COMP_BOUNDARY2(s, v, ...) (int)mock_call(v, 2u, PE_D_COMP_A(__VA_ARGS__, 0, 0, 0, 0))
#define PE_D_COMP_BOUNDARY3(s, v, ...) (int)mock_call(v, 3u, PE_D_COMP_A(__VA_ARGS__, 0, 0, 0, 0))
#define PE_D_COMP_BOUNDARY4(s, v, ...) (int)mock_call(v, 4u, PE_D_COMP_A(__VA_ARGS__, 0, 0, 0, 0))
"""

GTE_EXT = r"""
#include <stdint.h>
extern uint8_t ram[0x200400];
static uint8_t *ext_at(uint32_t a) { return (a & 0x1FFFFC00u) == 0x1F800000u ? ram + 0x200000u + (a & 0x3FFu) : ram + (a & 0x1FFFFFu); }
uint16_t PE_LoadU16(uint32_t a) { uint8_t *p = ext_at(a); return (uint16_t)(p[0] | p[1] << 8); }
uint32_t PE_LoadU32(uint32_t a) { uint8_t *p = ext_at(a); return p[0] | p[1] << 8 | p[2] << 16 | (uint32_t)p[3] << 24; }
void PE_Port_RequestStop(const char *r) { (void)r; }
"""

HARNESS = r"""
uint8_t ram[0x200400];
static pe_addr_t fsp = 0x801FEC00u;
pe_addr_t PE_GuestFrame_Enter(uint32_t n) { fsp -= (n + 7u) & ~7u; return fsp; }
void PE_GuestFrame_Leave(pe_addr_t b, uint32_t n) { (void)b; fsp += (n + 7u) & ~7u; }
static unsigned seed_;
static FILE *logf_;
uint32_t mock_call(uint32_t fn, unsigned n, uint32_t a0, uint32_t a1, uint32_t a2, uint32_t a3)
{
    uint32_t a[4] = {a0, a1, a2, a3}, h = fn * 2654435761u + seed_ * 40503u;
    unsigned i;
    fprintf(logf_, "C %08X", fn);
    for (i = 0; i < n && i < 4; i++) fprintf(logf_, " %08X", a[i]);
    fprintf(logf_, "\n");
    if (((seed_ * 2654435761u) >> 16) & 1u) return (uint32_t)((int32_t)(h & 0x1FFFu) - 0x1000);
    return 0x80100000u + (h & 0xDFFFCu);
}
uint32_t mock_args(uint32_t fn, unsigned n, const uint32_t *a)
{
    uint32_t h = fn * 2654435761u + seed_ * 40503u;
    unsigned i;
    fprintf(logf_, "C %08X", fn);
    for (i = 0; i < n && i < 12; i++) {
        if (a[i] >= 0x801FE000u && a[i] < 0x801FF000u) {
            unsigned j;
            fprintf(logf_, " S:%08X:", a[i]);
            for (j = 0; j < 16; j++) fprintf(logf_, "%02X", ram[(a[i] & 0x1FFFFFu) + j]);
        } else
            fprintf(logf_, " %08X", a[i]);
    }
    fprintf(logf_, "\n");
    for (i = 0; i < n && i < 12; i++) {
        unsigned j, q;
        uint32_t x;
        for (q = 0; q < sizeof outs_ / sizeof outs_[0]; q++)
            if (outs_[q] == (fn << 4 | i) && a[i] >= 0x801FE000u && a[i] < 0x801FF000u) {
                x = fn * 2654435761u + seed_ * 40503u + i * 0x9E37u;
                for (j = 0; j < outl_[q]; j++) {
                    x = x * 1103515245u + 12345u;
                    ram[(a[i] & 0x1FFFFFu) + j] = (uint8_t)(x >> 16);
                }
            }
    }
    if (((seed_ * 2654435761u) >> 16) & 1u) return (uint32_t)((int32_t)(h & 0x1FFFu) - 0x1000);
    return 0x80100000u + (h & 0xDFFFCu);
}
int main(int argc, char **argv)
{
    FILE *f = fopen(argv[1], "rb");
    uint32_t a0 = strtoul(argv[3], 0, 16), a1 = strtoul(argv[4], 0, 16),
             a2 = strtoul(argv[5], 0, 16), a3 = strtoul(argv[6], 0, 16);
    seed_ = (unsigned)strtoul(argv[7], 0, 10);
    if (!f || fread(ram, 1, sizeof ram, f) != sizeof ram) return 2;
    fclose(f);
    logf_ = fopen(argv[8], "w");
    (void)a0; (void)a1; (void)a2; (void)a3;
    CALL_LINE
    fclose(logf_);
    f = fopen(argv[2], "wb"); fwrite(ram, 1, sizeof ram, f); fclose(f);
    return 0;
}
"""


def candidates() -> list[tuple[Path, str, str]]:
    out = []
    for p in sorted((o.REPO_ROOT / o.ROOMS_DIR).glob("*_port.c")):
        t = p.read_text()
        if "E21 guest-memory" not in t:
            continue
        name = p.name.split("__")[0]
        body = re.sub(r'"[^"]*"', '""', t.split("*/", 1)[1])
        calls = set(re.findall(r"\b(func_[0-9A-F]{8})\s*\(", body)) - {name}
        m0 = re.search(r"decomp-source-overlay: (room_\w+)/", t)
        nonmatching = bool(m0) and name in o.nonmatching_leaves().get(m0.group(1), {})
        if "PE_HostToGuest" in t or ((calls or "host prototypes" in t) and not nonmatching):
            continue
        m = re.search(r"decomp-source-overlay: (room_\w+)/", t)
        out.append((p, name, m.group(1)))
    return out


PROTO_RE = re.compile(r"^(\w[\w \*]*?)\s*\b(func_[0-9A-F]{8})\((.*?)\);", re.M)
_REPL: dict[str, str] | None = None


def host_protos(t: str) -> list[tuple[str, str, str]]:
    """(ret, func_X, params) of a TU's host-prototyped callees.  A host
    replacement (gen_overlay_ports.host_replacements: PE_RotMatrixYXZ_Guest for
    func_80079754, ...) is reported under its retail func_X so it is mocked and
    logged like the retail jal; its stub keeps the host name."""
    global _REPL
    if "host prototypes" not in t:
        return []
    if _REPL is None:
        _REPL = {v["host"]: k for k, v in o.host_replacements().items()}
    block = t.split("host prototypes", 1)[1].split("\n\n", 1)[0]
    out = list(PROTO_RE.findall(block))
    for m in re.finditer(r"^(\w[\w \*]*?)\s*\b(PE_\w+)\((.*?)\);", block, re.M):
        if m.group(2) in _REPL:
            out.append((m.group(1), m.group(2), m.group(3)))
    return out


def proto_vma(fn: str) -> int:
    return int((_REPL.get(fn, fn) if fn.startswith("PE_") else fn)[5:], 16)


def host_masks(t: str) -> dict[int, list[int]]:
    """{callee vma: per-argument compare mask} for host-prototyped callees."""
    out = {}
    for _ret, fn, prm in host_protos(t):
        ps = [] if prm.strip() == "void" else [x.strip() for x in prm.split(",")]
        out[proto_vma(fn)] = [0xFFFF if "short" in ty else 0xFF if "char" in ty else 0xFFFFFFFF
                                for ty in ps]
    return out


def host_stubs(t: str) -> str:
    """Mock definitions for a TU's host-prototyped callees (NON_MATCHING leaves):
    every argument is logged (stack args too, as retail passes them at sp+16+4k),
    narrow parameters masked to their width so they compare with the retail
    register/stack word; the return value is the same seed-derived mock as the
    retail interpreter's jal."""
    if "host prototypes" not in t:
        return ""
    out = []
    for ret, fn, prm in host_protos(t):
        ps = [] if prm.strip() == "void" else [x.strip() for x in prm.split(",")]
        names, logs = [], []
        for k, ty in enumerate(ps):
            nm = f"p{k}"
            names.append(re.sub(r"\w+$", nm, ty) if re.search(r"\s\w+$", ty) else f"{ty} {nm}")
            w = "0xFFFFu" if "short" in ty else "0xFFu" if "char" in ty else "0xFFFFFFFFu"
            logs.append(f"(uint32_t)(uintptr_t){nm} & {w}")
        body = (f"uint32_t a_[12] = {{{', '.join(logs) or '0'}}}; "
                f"uint32_t v_ = mock_args(0x{proto_vma(fn):08X}u, {len(ps)}u, a_);")
        retst = "" if ret.strip() == "void" else f" return ({ret.strip()})v_;"
        out.append(f"{ret} {fn}({', '.join(names) or 'void'}) {{ {body}{retst} }}")
    # Host replacements (pe_port_compat.h `host-replacement: func_X`): the lowered
    # TU calls PE_* directly; mock it as the retail jal to func_X.
    for fx, r in o.host_replacements().items():
        if not re.search(r"\b" + r["host"] + r"\s*\(", t):
            continue
        ps = [x.strip() for x in r["params"] if x.strip() not in ("", "void")]
        nm = [f"p{k}" for k in range(len(ps))]
        decl = ", ".join(f"pe_addr_t {n}" if "pe_addr_t" in ty else f"{re.sub(r'\s*\w+$', '', ty) or ty} {n}"
                         for ty, n in zip(ps, nm))
        logs = ", ".join(f"(uint32_t)(uintptr_t){n}" for n in nm) or "0"
        retst = "" if r["ret"] == "void" else f" return ({r['ret']})v_;"
        out.append(f"{r['ret']} {r['host']}({decl or 'void'}) {{ uint32_t a_[12] = {{{logs}}}; "
                   f"uint32_t v_ = mock_args(0x{fx[5:]}u, {len(ps)}u, a_);{retst} }}")
    return "\n" + "\n".join(out) + "\n"


def directed_specs() -> dict[tuple[str, str], list[tuple[str, int, list[int]]]]:
    """Directed inputs for NON_MATCHING leaves, from configs/USA/overlays/nonmatching.yaml:
    `inputs: [ovl, name, "a0=1,2; h(a1+0x10)=0..5"]`.  Each seed takes the
    (seed mod product)-th combination: `aN=` sets argument N, `h(aN+OFF)=` /
    `w(aN+OFF)=` store a halfword / word there (value lists or LO..HI ranges)."""
    out: dict = {}
    if not o.NONMATCH_MANIFEST.exists():
        return out
    for m in re.finditer(r'^\s*-\s*\[\s*(\w+)\s*,\s*(\w+)\s*,\s*"([^"]*)"\s*\]',
                         o.NONMATCH_MANIFEST.read_text().split("inputs:", 1)[-1], re.M):
        terms = []
        for t in m.group(3).split(";"):
            lhs, rhs = (x.strip() for x in t.split("="))
            vals: list[int] = []
            if lhs == "out":                   # out=FN:K[:LEN],...: callee FN writes LEN (8) bytes through stack arg K
                terms.append((lhs, [int(x.split(":")[0], 16) << 4 | int(x.split(":")[1])
                                    | (int(x.split(":")[2], 0) << OUT_LEN_SHIFT if x.count(":") == 2 else 0)
                                    for x in rhs.split(",")]))
                continue
            for v in rhs.split(","):
                if ".." in v:
                    lo_, hi_ = (int(x, 0) for x in v.split(".."))
                    vals += list(range(lo_, hi_ + 1))
                else:
                    vals.append(int(v, 0))
            terms.append((lhs, vals))
        out[(m.group(1), m.group(2))] = terms
    return out


def inputs(name, ov, data, seeds, recorded, spec):
    """(seed, ram, args): recorded game snapshots if given, else seeded random
    RAM (pointer-heavy) with the directed terms applied on top."""
    if recorded:
        for k, f in enumerate(sorted(Path(recorded).glob("rec_*.bin"))):
            b = f.read_bytes()
            if len(b) != RAM_SIZE + SPAD + 16:
                continue
            lf = ov["leaves"][name]
            at_ = lf["vram"] & 0x1FFFFFFF
            if b[at_:at_ + lf["file_size"]] != data[lf["file_offset"]:lf["file_offset"] + lf["file_size"]]:
                continue                       # another overlay's code was resident: not this leaf
            yield k, bytearray(b[:RAM_SIZE + SPAD]), list(struct.unpack_from("<4I", b, RAM_SIZE + SPAD))
        return
    for seed in range(seeds):
        rng = random.Random(f"{name}:{seed}")
        ram = bytearray(RAM_SIZE + SPAD)
        words = [0x80100000 + (rng.getrandbits(18) << 2) if rng.random() < 0.7
                 else rng.choice((0, 1, 2, 3, 0xFF, 0xFFFF, rng.getrandbits(32)))
                 for _ in range((RAM_SIZE + SPAD) // 4)]
        ram[:] = struct.pack(f"<{len(words)}I", *words)
        base = ov["vram"] & 0x1FFFFFFF
        ram[base:base + len(data)] = data
        args = [0x80100000 + (rng.getrandbits(18) << 2) for _ in range(4)]
        if spec:
            k = seed
            for lhs, vals in spec:
                if lhs == "out":
                    continue
                v = vals[k % len(vals)]
                k //= len(vals)
                mm = re.fullmatch(r"a(\d)", lhs)
                if mm:
                    args[int(mm.group(1))] = v & 0xFFFFFFFF
                    continue
                mm = re.fullmatch(r"([hw])\((?:a(\d)\+)?(0x[0-9A-Fa-f]+|\d+)\)", lhs)
                if not mm:
                    raise SystemExit(f"e21_diff: bad directed term {lhs!r}")
                base_ = args[int(mm.group(2))] if mm.group(2) else 0
                at = (base_ + int(mm.group(3), 0)) & 0x1FFFFFFF
                struct.pack_into("<H" if mm.group(1) == "h" else "<I", ram, at,
                                 v & (0xFFFF if mm.group(1) == "h" else 0xFFFFFFFF))
        yield seed, ram, args


def one(p: Path, name: str, ovl: str, seeds: int, cc: str, cover: bool = False,
        recorded: str = "", directed: dict | None = None) -> tuple[str, str]:
    directed = directed or {}
    global COVER
    COVER = set() if cover else None
    ov = o.parse_overlay(ovl)
    blob = o.blob_path(ovl)
    if blob is None:
        return "noblob", ""
    leaf = ov["leaves"][name]
    data = blob.read_bytes()
    t = p.read_text()
    sig = re.search(r"^(\w+) " + name + r"\((.*?)\)\s*\{", t, re.M)
    nparams = 0 if sig.group(2).strip() == "void" else len(sig.group(2).split(","))
    regs = ["a0", "a1", "a2", "a3"] + [f"PE_LoadU32(0x{SP0 + 16 + 4 * k:08X}u)" for k in range(8)]
    argl = ", ".join(regs[:nparams])
    if sig.group(1) == "void":
        call = f"{name}({argl}); fprintf(logf_, \"R -\\n\");"
    else:
        call = (f"uint32_t rv = (uint32_t)(int32_t){name}({argl}); "
                f"fprintf(logf_, \"R %08X\\n\", rv);")
    stubs = host_stubs(t)
    masks = host_masks(t)
    with tempfile.TemporaryDirectory() as d:
        d = Path(d)
        (d / "pe_guest_decomp.h").write_text(MOCK_H)
        outs = frozenset(v for lhs, vs in directed.get((ovl, name), []) if lhs == "out" for v in vs)
        harness = HARNESS.replace("static unsigned seed_;", "static unsigned seed_;\nstatic const uint32_t outs_[] = {"
                                  + ", ".join(f"0x{out_key(v) & 0xFFFFFFFF:X}u" for v in sorted(outs) or [0xFFFFFFFF]) + "};\n"
                                  + "static const unsigned outl_[] = {"
                                  + ", ".join(f"{out_len(v)}u" for v in sorted(outs) or [0]) + "};")
        src = t + harness.replace("CALL_LINE", call) + stubs
        (d / "t.c").write_text(src)
        (d / "gte_ext.c").write_text(GTE_EXT)
        pc = o.REPO_ROOT / "pc_port"
        r = subprocess.run([cc, "-O1", "-std=gnu11", "-funsigned-char", "-w", "-c",
                            "-I", str(pc / "include"), "-I", str(pc / "platform"), "-I", str(pc),
                            "-I", str(pc / "bootstrap"), "-o", str(d / "pe_gte.o"),
                            str(pc / "platform" / "pe_gte.c")], capture_output=True, text=True)
        if r.returncode:
            return "nocompile", "pe_gte.c: " + r.stderr[:300]
        r = subprocess.run([cc, "-O1", "-std=gnu11", "-funsigned-char", "-w", "-I", str(d),
                            "-o", str(d / "t"), str(d / "t.c"), str(d / "gte_ext.c"),
                            str(d / "pe_gte.o")], capture_output=True, text=True)
        if r.returncode:
            return "nocompile", r.stderr[:300]
        ran = 0
        for seed, ram, args in inputs(name, ov, data, seeds, recorded, directed.get((ovl, name))):
            r_ram = bytearray(ram)
            try:
                v0, calls = run_mips(r_ram, leaf["vram"], list(args), seed, outs)
            except Fault:
                continue
            (d / "in.bin").write_bytes(ram)
            r = subprocess.run([str(d / "t"), str(d / "in.bin"), str(d / "out.bin"),
                                *[f"{a:08X}" for a in args], str(seed), str(d / "log")],
                               capture_output=True, text=True, timeout=30)
            if r.returncode:
                return "MISMATCH", f"seed {seed}: native exit {r.returncode} {r.stdout[:80]}"
            n_ram = (d / "out.bin").read_bytes()
            lo, hi = EXCLUDE[0] & 0x1FFFFFFF, EXCLUDE[1] & 0x1FFFFFFF
            if r_ram[:lo] != n_ram[:lo]:
                i = next(i for i in range(lo) if r_ram[i] != n_ram[i])
                return "MISMATCH", (f"seed {seed}: RAM 0x{0x80000000 + i:08X} retail "
                                    f"{r_ram[i]:02X} native {n_ram[i]:02X}")
            if r_ram[RAM_SIZE:] != n_ram[RAM_SIZE:]:
                i = next(i for i in range(SPAD) if r_ram[RAM_SIZE + i] != n_ram[RAM_SIZE + i])
                return "MISMATCH", (f"seed {seed}: scratchpad 0x{0x1F800000 + i:08X} retail "
                                    f"{r_ram[RAM_SIZE + i]:02X} native {n_ram[RAM_SIZE + i]:02X}")
            log = (d / "log").read_text().split("\n")
            ncalls = [l for l in log if l.startswith("C ")]
            if len(ncalls) != len(calls):
                return "MISMATCH", f"seed {seed}: {len(calls)} retail calls vs {len(ncalls)} native"
            for (tgt, regs), nl in zip(calls, ncalls):
                parts = nl.split()[1:]
                if int(parts[0], 16) != tgt:
                    return "MISMATCH", f"seed {seed}: call {tgt:08X} vs {parts[0]}"
                mk = masks.get(tgt, [])
                for k, pv in enumerate(parts[1:]):
                    m = mk[k] if k < len(mk) else 0xFFFFFFFF
                    rv_ = regs[k]
                    if isinstance(rv_, tuple) and not pv.startswith("S:"):
                        rv_ = rv_[3]           # a guest pointer that merely lies in the stack window
                    if pv.startswith("S:") and not isinstance(rv_, tuple):
                        pv = pv.split(":")[1]  # likewise on the native side
                    if isinstance(rv_, tuple) or pv.startswith("S:"):
                        nb = bytes.fromhex(pv.split(":")[2]) if pv.startswith("S:") else b""
                        if not (isinstance(rv_, tuple) and len(nb) == 16 and
                                all(nb[j] == rv_[1][j] for j in range(16) if rv_[2][j])):
                            return "MISMATCH", (f"seed {seed}: call {tgt:08X} a{k} stack arg "
                                                f"{rv_[1].hex() if isinstance(rv_, tuple) else hex(rv_)} vs {pv} "
                                                f"(retail-written {''.join('w' if x else '.' for x in rv_[2]) if isinstance(rv_, tuple) else '-'})")
                        continue
                    if int(pv, 16) & m != rv_ & m:
                        return "MISMATCH", f"seed {seed}: call {tgt:08X} a{k} {rv_:08X} vs {pv}"
            rl = [l for l in log if l.startswith("R ")][0].split()[1]
            if rl != "-" and int(rl, 16) != v0 and not sig.group(1).startswith(("uint8", "int8", "uint16", "int16")):
                return "MISMATCH", f"seed {seed}: v0 {v0:08X} vs {rl}"
            ran += 1
        cov = ""
        if COVER is not None:
            lo_, hi_ = leaf["vram"], leaf["vram"] + leaf["file_size"]
            hit = sum(1 for a in COVER if lo_ <= a < hi_)
            cov = f", retail coverage {hit}/{leaf['file_size'] // 4} words"
        return ("ok" if ran else "skipped"), f"{ran} seeds{cov}"


def main() -> int:
    ap = argparse.ArgumentParser()
    ap.add_argument("--seeds", type=int, default=6)
    ap.add_argument("--limit", type=int, default=0)
    ap.add_argument("--verbose", action="store_true")
    ap.add_argument("--jobs", type=int, default=4)
    ap.add_argument("--recorded", default="", help="replay PE_RECORD_GUESTCALL snapshots from DIR")
    ap.add_argument("--coverage", action="store_true", help="report retail instruction coverage")
    ap.add_argument("--only", default="", help="substring of the TU file name or source overlay")
    args = ap.parse_args()
    cc = shutil.which("cc") or shutil.which("gcc")
    cands = candidates()
    if args.only:
        cands = [c for c in cands if args.only in c[0].name or args.only == c[2]]
    if args.limit:
        cands = cands[: args.limit]
    res: dict[str, list] = {}
    with concurrent.futures.ProcessPoolExecutor(max_workers=max(1, args.jobs)) as ex:
        futs = {ex.submit(one, p, n, ov, args.seeds, cc, args.coverage, args.recorded, directed_specs()): (p.name, ov) for p, n, ov in cands}
        for f in concurrent.futures.as_completed(futs):
            st, why = f.result()
            res.setdefault(st, []).append((futs[f][0], why))
    for st in sorted(res):
        print(f"e21_diff: {st}: {len(res[st])}")
        if st in ("MISMATCH", "nocompile") or args.verbose:
            for nm, why in sorted(res[st]):
                print(f"  {st} {nm}: {why}")
    return 1 if res.get("MISMATCH") or res.get("nocompile") else 0


if __name__ == "__main__":
    raise SystemExit(main())
