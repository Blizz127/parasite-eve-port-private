#!/usr/bin/env python3
"""Unit tests for guest_lower.py (E21 guest-memory lowering).

Run: python3 tools/analysis/test_guest_lower.py

Each runtime case lowers a small retail-typed leaf, compiles it on the host
against a mock guest RAM (little-endian byte array at 0x80000000, a mock
PE_GuestCall that records the retail jalr, a guest-frame arena), runs it, and
checks guest memory / the return value against what the MIPS O32 build of the
same C does: 4-byte pointers, natural alignment, unsigned plain `char`
(gcc-2.7.2-psx), modular 32-bit arithmetic, pointer scaling by guest sizes.
"""
from __future__ import annotations

import shutil
import subprocess
import sys
import tempfile
import unittest
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import guest_lower as gl  # noqa: E402

MOCK_H = r"""
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
typedef uint32_t pe_addr_t;
static uint8_t ram[0x200000];
static uint8_t *at(pe_addr_t a) { if ((a & 0x1FFFFFFFu) >= sizeof ram) { printf("BAD %08X\n", a); exit(3); } return ram + (a & 0x1FFFFFu); }
static uint8_t  PE_LoadU8(pe_addr_t a)  { return *at(a); }
static uint16_t PE_LoadU16(pe_addr_t a) { uint16_t v; memcpy(&v, at(a), 2); return v; }
static uint32_t PE_LoadU32(pe_addr_t a) { uint32_t v; memcpy(&v, at(a), 4); return v; }
static void PE_StoreU8(pe_addr_t a, uint8_t v)   { *at(a) = v; }
static void PE_StoreU16(pe_addr_t a, uint16_t v) { memcpy(at(a), &v, 2); }
static void PE_StoreU32(pe_addr_t a, uint32_t v) { memcpy(at(a), &v, 4); }
static void PE_Fill(pe_addr_t a, uint32_t n, uint8_t v) { memset(at(a), v, n); }
static void *PE_Translate(pe_addr_t a, size_t n) { (void)n; return at(a); }
static pe_addr_t PE_HostToGuest(const volatile void *p) { return p ? 0x80000000u + (pe_addr_t)((const uint8_t *)p - ram) : 0u; }
static pe_addr_t frame_sp = 0x801FEC00u;
static pe_addr_t PE_GuestFrame_Enter(uint32_t n) { frame_sp -= (n + 7u) & ~7u; return frame_sp; }
static void PE_GuestFrame_Leave(pe_addr_t b, uint32_t n) { if (b != frame_sp) exit(4); frame_sp += (n + 7u) & ~7u; }
static unsigned calls; static pe_addr_t call_fn; static uintptr_t call_a0, call_a1;
static int PE_GuestCall(const char *site, pe_addr_t fn, unsigned n, uintptr_t a0, uintptr_t a1, uintptr_t a2, uintptr_t a3)
{ (void)site; (void)n; (void)a2; (void)a3; calls++; call_fn = fn; call_a0 = a0; call_a1 = a1; return 7; }
static unsigned boundaries;
#define PE_D_COMP_BOUNDARY0(s, v) (boundaries++, 0)
#define PE_D_COMP_BOUNDARY1(s, v, ...) (boundaries++, 0)
#define PE_D_COMP_BOUNDARY2(s, v, ...) (boundaries++, 0)
"""


def lower(src: str, name: str = "func_80190000", known=(), sigs=None) -> dict:
    return gl.lower_leaf(name, src, set(known), sigs or {})


def tu(src: str, name: str = "func_80190000", known=(), sigs=None, extra: str = "") -> str:
    res = lower(src, name, known, sigs)
    head = "\n".join(res["helpers"])
    if res["sig"] is None:
        body = res["body"] + "\n}\n"
    else:
        body = f"{res['sig']}\n{{\n{res['body']}\n}}\n"
    return MOCK_H + extra + head + "\n" + body


class CRunner(unittest.TestCase):
    def run_c(self, code: str, main: str) -> str:
        cc = shutil.which("cc") or shutil.which("gcc")
        with tempfile.TemporaryDirectory() as d:
            c = Path(d) / "t.c"
            c.write_text(code + "\nint main(void)\n{\n" + main + "\n    return 0;\n}\n")
            exe = Path(d) / "t"
            r = subprocess.run([cc, "-O2", "-std=gnu11", "-Werror=implicit-function-declaration",
                                "-Wno-unused-function", "-o", str(exe), str(c)],
                               capture_output=True, text=True)
            self.assertEqual(r.returncode, 0, r.stderr + code)
            r = subprocess.run([str(exe)], capture_output=True, text=True)
            self.assertEqual(r.returncode, 0, r.stdout + r.stderr)
            return r.stdout


@unittest.skipUnless(shutil.which("cc") or shutil.which("gcc"), "no host C compiler")
class Runtime(CRunner):
    def test_pointer_field_chain_and_code_pointer_call(self):
        # func_80190240's shape: o->ctx->fE through a guest pointer in RAM,
        # then the retail jalr through o->fn.
        src = """
        typedef struct Ctx { char pad[0xE]; unsigned char fE; } Ctx;
        typedef struct Obj { int pad0, pad4; Ctx *ctx; int (*fn)(struct Obj *); } Obj;
        int func_80190000(Obj *o) {
            if (o->ctx->fE < 2) return 1;
            o->fn(o);
            return 0;
        }"""
        out = self.run_c(tu(src), """
            PE_StoreU32(0x80100008u, 0x80100100u);          /* o->ctx */
            PE_StoreU32(0x8010000Cu, 0x80190370u);          /* o->fn  */
            PE_StoreU8(0x8010010Eu, 1);
            int r = func_80190000(0x80100000u);
            printf("%d %u\\n", r, calls);
            PE_StoreU8(0x8010010Eu, 2);
            r = func_80190000(0x80100000u);
            printf("%d %u %08X %08X\\n", r, calls, call_fn, (unsigned)call_a0);""")
        self.assertEqual(out.split("\n")[:2], ["1 0", "0 1 80190370 80100000"])

    def test_o32_layout_pointer_members(self):
        src = """
        typedef struct S { char a; int *p; short s; char c[3]; struct S *next; } S;
        int func_80190000(S *x, int *q) {
            x->a = 1; x->p = q; x->s = -2; x->c[2] = 9; x->next = x + 1;
            return sizeof(S);
        }"""
        out = self.run_c(tu(src), """
            int n = func_80190000(0x80100000u, 0x80123450u);
            printf("%d %02X %08X %04X %02X %08X\\n", n, PE_LoadU8(0x80100000u),
                   PE_LoadU32(0x80100004u), PE_LoadU16(0x80100008u),
                   PE_LoadU8(0x8010000Cu), PE_LoadU32(0x80100010u));""")
        self.assertEqual(out.strip(), "20 01 80123450 FFFE 09 80100014")

    def test_plain_char_is_unsigned(self):
        src = """
        int func_80190000(char *p, signed char *q) { return (*p == 0xFF) * 100 + *q; }"""
        out = self.run_c(tu(src), """
            PE_StoreU8(0x80100000u, 0xFF);
            printf("%d\\n", func_80190000(0x80100000u, 0x80100000u));""")
        self.assertEqual(out.strip(), "99")

    def test_pointer_scaling_and_difference(self):
        src = """
        typedef struct V { short x, y, z, pad; } V;
        int func_80190000(V *v, short *s) {
            V *w = v + 3;
            short *t = s + 5;
            w->y = 4;
            w[-1].z = 5;
            return (w - v) * 100 + (t - s) + (int)(&w->z - s);
        }"""
        out = self.run_c(tu(src), """
            int r = func_80190000(0x80100000u, 0x80100000u);
            printf("%d %04X %04X\\n", r, PE_LoadU16(0x8010001Au), PE_LoadU16(0x80100014u));""")
        # w = +24, w[-1].z = +20, &w->z = +28 -> 14 shorts
        self.assertEqual(out.strip(), "319 0004 0005")

    def test_guest_frame_local_passed_by_address(self):
        src = """
        typedef struct M { short m[3][3]; short pad; int t[3]; } M;
        extern void func_80001000(M *out);
        int func_80190000(int k) {
            M m;
            func_80001000(&m);
            return m.t[2] + m.m[1][1] + k;
        }"""
        extra = """
        static pe_addr_t seen;
        void func_80001000(pe_addr_t out) { seen = out; PE_StoreU32(out + 28u, 40u); PE_StoreU16(out + 8u, 2u); }
        """
        sigs = {"func_80001000": {"ret": "void", "params": ["pe_addr_t out"]}}
        out = self.run_c(tu(src, known={"func_80001000"}, sigs=sigs, extra=extra), """
            int r = func_80190000(1);
            printf("%d %08X %08X\\n", r, seen, frame_sp);""")
        self.assertEqual(out.strip(), "43 801FEBE0 801FEC00")

    def test_host_pointer_parameter_translated(self):
        src = """
        typedef struct V { short x, y, z, pad; } V;
        extern int func_80001000(V *v);
        int func_80190000(V *v) { return func_80001000(v + 1); }"""
        extra = """
        typedef struct HV { short x, y, z, pad; } HV;
        int func_80001000(HV *v) { return v->y; }
        """
        sigs = {"func_80001000": {"ret": "int", "params": ["HV *v"]}}
        out = self.run_c(tu(src, known={"func_80001000"}, sigs=sigs, extra=extra), """
            PE_StoreU16(0x8010000Au, 77);
            printf("%d\\n", func_80190000(0x80100000u));""")
        self.assertEqual(out.strip(), "77")

    def test_compound_assign_incdec_and_wrap(self):
        src = """
        typedef struct O { int n; unsigned char k; short h; } O;
        int func_80190000(O *o) {
            o->n += 5;
            o->h -= 3;
            o->n = o->n * 0x10000;
            return o->k++ + (++o->k);
        }"""
        out = self.run_c(tu(src), """
            PE_StoreU32(0x80100000u, 0x7FFF);
            PE_StoreU8(0x80100004u, 0xFF);
            int r = func_80190000(0x80100000u);
            printf("%d %08X %02X %04X\\n", r, PE_LoadU32(0x80100000u),
                   PE_LoadU8(0x80100004u), PE_LoadU16(0x80100006u));""")
        # n = 0x8004 * 0x10000 wraps to 0x80040000; k: 255 + 1 -> 0, ++ -> 1
        self.assertEqual(out.strip(), "256 80040000 01 FFFD")

    def test_runtime_division_r3000_semantics_and_trap(self):
        # func_80192680's shape: a runtime divisor read from guest memory.
        # Zero is retail's break-7 divide check (a guest trap, never a host
        # SIGFPE); INT_MIN / -1 wraps like the R3000; constants stay plain.
        src = """
        typedef struct O { int a; int b; unsigned u; unsigned v; int q, r; unsigned uq, ur; int c; } O;
        void func_80190000(O *o) {
            o->q = o->a / o->b;
            o->r = o->a % o->b;
            o->uq = o->u / o->v;
            o->ur = o->u % o->v;
            o->c = o->a / 24;
        }"""
        code = tu(src, extra="static unsigned traps;\n"
                  "static void PE_Decomp_GuestTrapDivZero(const char *fn) { (void)fn; traps++; }\n")
        # helper definition + the one runtime `/` (o->a / 24 stays plain)
        self.assertEqual(code.count("pe_low_divs("), 2)
        out = self.run_c(code, """
            PE_StoreU32(0x80100000u, 100u); PE_StoreU32(0x80100004u, 0u);
            PE_StoreU32(0x80100008u, 7u);   PE_StoreU32(0x8010000Cu, 0u);
            func_80190000(0x80100000u);
            printf("%d %d %08X %u %d %u\\n", (int32_t)PE_LoadU32(0x80100010u),
                   (int32_t)PE_LoadU32(0x80100014u), PE_LoadU32(0x80100018u),
                   PE_LoadU32(0x8010001Cu), (int32_t)PE_LoadU32(0x80100020u), traps);
            PE_StoreU32(0x80100000u, 0x80000000u); PE_StoreU32(0x80100004u, 0xFFFFFFFFu);
            PE_StoreU32(0x80100008u, 7u);   PE_StoreU32(0x8010000Cu, 2u);
            func_80190000(0x80100000u);
            printf("%08X %d %u %u %u\\n", PE_LoadU32(0x80100010u),
                   (int32_t)PE_LoadU32(0x80100014u), PE_LoadU32(0x80100018u),
                   PE_LoadU32(0x8010001Cu), traps);""")
        lines = out.split()
        # 100/0 -> -1, 100%0 -> 100, 7u/0 -> FFFFFFFF, 7u%0 -> 7, 100/24 -> 4; 4 traps
        self.assertEqual(lines[:6], ["-1", "100", "FFFFFFFF", "7", "4", "4"])
        # INT_MIN / -1 -> INT_MIN, % -> 0; 7u/2 = 3, 7u%2 = 1; no new trap
        self.assertEqual(lines[6:], ["80000000", "0", "3", "1", "4"])
        # constant non-zero divisor stays a plain host division
        self.assertTrue(gl.safe_divisor("24") and gl.safe_divisor("(0x18)"))
        self.assertFalse(gl.safe_divisor("0") or gl.safe_divisor("-1") or gl.safe_divisor("v_n"))

    def test_packed_struct_and_init_list(self):
        src = """
        typedef struct P { char a; int b; } __attribute__((packed)) P;
        extern void func_80001000(int *v);
        int func_80190000(P *p) {
            int v[4] = { 3, 4 };
            p->b = 0x11223344;
            func_80001000(v);
            return sizeof(P) * 100 + v[1] + v[3];
        }"""
        extra = "void func_80001000(pe_addr_t v) { (void)v; }\n"
        sigs = {"func_80001000": {"ret": "void", "params": ["pe_addr_t v"]}}
        out = self.run_c(tu(src, known={"func_80001000"}, sigs=sigs, extra=extra), """
            PE_StoreU32(0x801FEBF0u + 12u, 0xDEAD);  /* stale frame byte must be zeroed */
            int r = func_80190000(0x80100000u);
            printf("%d %08X\\n", r, PE_LoadU32(0x80100001u));""")
        self.assertEqual(out.strip(), "504 11223344")

    def test_unknown_callee_is_a_loud_boundary(self):
        src = "int func_80190000(int a) { return func_80012340(a) + 1; }"
        res = lower(src)
        self.assertIn('PE_D_COMP_BOUNDARY1("func_80012340", 0x80012340u, a_a)', res["body"])


GTE_MOCK = r"""
static uint32_t gd[32], gc[32]; static unsigned gcmds;
static uint32_t PE_GTE_ReadData(uint32_t r) { return gd[r]; }
static void PE_GTE_WriteData(uint32_t r, uint32_t v)
{ gd[r] = (r == 1u || (r >= 9u && r <= 11u)) ? (uint32_t)(int32_t)(int16_t)v : v; }
static uint32_t PE_GTE_ReadCtrl(uint32_t r) { return gc[r]; }
static void PE_GTE_WriteCtrl(uint32_t r, uint32_t v) { gc[r] = v; }
static void PE_GTE_Command(uint32_t cmd)
{   /* independent reference: MVMVA sf=1 mx=RT v=V0 cv=none, lm=0 */
    int16_t m[9], v[3]; int i;
    gcmds++;
    if (cmd != 0x486012u) { printf("CMD %07X\n", cmd); exit(5); }
    for (i = 0; i < 4; i++) { m[2 * i] = (int16_t)gc[i]; m[2 * i + 1] = (int16_t)(gc[i] >> 16); }
    m[8] = (int16_t)gc[4];
    v[0] = (int16_t)gd[0]; v[1] = (int16_t)(gd[0] >> 16); v[2] = (int16_t)gd[1];
    for (i = 0; i < 3; i++) {
        int64_t mac = ((int64_t)m[3 * i] * v[0] + (int64_t)m[3 * i + 1] * v[1] + (int64_t)m[3 * i + 2] * v[2]) >> 12;
        gd[25 + i] = (uint32_t)(int32_t)mac;
        gd[9 + i] = (uint32_t)(int32_t)(mac < -32768 ? -32768 : mac > 32767 ? 32767 : mac);
    }
}
"""

GTE_SRC = r"""
typedef struct MATRIX { short m[3][3]; int t[3]; } MATRIX;
typedef struct SVECTOR { short vx, vy, vz, pad; } SVECTOR;
typedef struct VECTOR { int vx, vy, vz, pad; } VECTOR;
extern int g_count asm("D_80180010");
int func_80190000(MATRIX *mat, SVECTOR *v, VECTOR *out, short *ir)
{
    int a, b, c;
    __asm__ volatile("lw $12,0(%0)\n\tlw $13,4(%0)\n\tctc2 $12,$0\n\tctc2 $13,$1\n\t"
                     "lw $12,8(%0)\n\tlw $13,12(%0)\n\tlw $14,16(%0)\n\tctc2 $12,$2\n\t"
                     "ctc2 $13,$3\n\tctc2 $14,$4" : : "r"(mat) : "$12", "$13", "$14");
    __asm__ volatile("lhu $13,2(%0)\n\tlhu $12,0(%0)\n\tsll $13,$13,16\n\tor $12,$12,$13\n\t"
                     "mtc2 $12,$0\n\tlwc2 $1,4(%0)" : : "r"(v) : "$12", "$13");
    __asm__ volatile("nop\n\tnop\n\t.word 0x4A486012");
    __asm__ volatile("swc2 $25,0(%0)\n\tswc2 $26,4(%0)\n\tswc2 $27,8(%0)" : : "r"(out) : "memory");
    __asm__ volatile("mfc2 %0,$9\n\tmfc2 %1,$10\n\tmfc2 %2,$11" : "=r"(a), "=r"(b), "=r"(c) : "2"(c));
    ir[0] = a; ir[1] = b; ir[2] = c;
    g_count++;
    return a + b + c;
}
"""


@unittest.skipUnless(shutil.which("cc") or shutil.which("gcc"), "no host C compiler")
class GteAsm(CRunner):
    """Inline PSY-Q GTE macros lowered to register-level native GTE calls."""

    def test_rotmatrix_ldv0_rtv0_stlvnl_mfc2(self):
        code = tu(GTE_SRC, extra=GTE_MOCK)
        self.assertIn("PE_GTE_Command(0x0486012u)", code)
        out = self.run_c(code, r"""
            static const short R[9] = { 0x1000, 0, 0, 0, 0, -0x1000, 0, 0x1000, 0 };
            int i;
            for (i = 0; i < 9; i++) PE_StoreU16(0x80100000u + 2u * i, (uint16_t)R[i]);
            PE_StoreU16(0x80100100u, 100); PE_StoreU16(0x80100102u, (uint16_t)-200);
            PE_StoreU16(0x80100104u, 40000u & 0xFFFF);
            int r = func_80190000(0x80100000u, 0x80100100u, 0x80100200u, 0x80100300u);
            printf("%d %d %d %d | %d %d %d | %u %u\n", r,
                   (int)PE_LoadU32(0x80100200u), (int)PE_LoadU32(0x80100204u), (int)PE_LoadU32(0x80100208u),
                   (short)PE_LoadU16(0x80100300u), (short)PE_LoadU16(0x80100302u), (short)PE_LoadU16(0x80100304u),
                   gcmds, PE_LoadU32(0x80180010u));""")
        # x = 100; y = -vz = -(40000 as s16 = -25536) = 25536; z = vy = -200
        self.assertEqual(out.strip(), "25436 100 25536 -200 | 100 25536 -200 | 1 1")

    def test_set_alias_renames_function(self):
        src = r"""
        asm(".set RoomLib_Helper, func_80190100");
        extern int RoomLib_Helper(int);
        int func_80190000(int a) { return RoomLib_Helper(a) + 1; }"""
        res = lower(src)
        self.assertIn('PE_D_COMP_BOUNDARY1("func_80190100"', res["body"])


class Refusals(unittest.TestCase):
    def assertRefused(self, src, why):
        with self.assertRaises(gl.LowerError) as cm:
            lower(src)
        self.assertIn(why, str(cm.exception))

    def test_float(self):
        self.assertRefused("int func_80190000(float *f) { return *f > 1; }", "float")

    def test_bitfield(self):
        self.assertRefused("typedef struct B { int a : 3; } B;\n"
                           "int func_80190000(B *b) { return b->a; }", "bitfield")

    def test_static_local(self):
        self.assertRefused("int func_80190000(void) { static int n; return n++; }", "static local")

    def test_real_asm(self):
        self.assertRefused('int func_80190000(int a) { __asm__("syscall"); return a; }', "asm")

    def test_unsupported_cop2_command(self):
        # RTPS (0x4A180001) has no register-level native implementation yet.
        self.assertRefused('int func_80190000(int a) { __asm__ volatile(".word 0x4A180001"); '
                           'return a; }', "cop2 command")

    def test_memory_constraint(self):
        self.assertRefused('int func_80190000(int *p) { __asm__ volatile("" : : "m"(*p)); '
                           'return 0; }', "input constraint m")

    def test_output_read_before_written(self):
        self.assertRefused('int func_80190000(void) { int x; __asm__ volatile('
                           '"mtc2 %0,$9" : "=r"(x)); return x; }', "read before written")

    def test_indirect_call_arity(self):
        self.assertRefused("int func_80190000(void (*f)(int,int,int,int,int)) "
                           "{ f(1,2,3,4,5); return 0; }", "more than 4")


if __name__ == "__main__":
    unittest.main()
