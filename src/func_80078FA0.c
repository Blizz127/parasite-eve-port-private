/* vendor-derived: khasinski/parasite-eve-decomp@3067919e7b82f0334b873117eef07e7157f4ed69 src/main/psyq/libgte/reg03.c — attribution khasinski (Chris Hasinski) */
/* psyq libgte: Psy-Q SDK library code — matching build only; never pull into pc_port. */
/* inline GTE/asm: this function uses the vendor's inline PSY-Q GTE (COP2) asm macros — count separately from plain C. */
/* Imported under the owner's explicit authorization (2026-09-28, via Devbox Coordinator, vendor lane).
 * Upstream tree kept verbatim in vendor/khasinski-parasite-eve-decomp/ (see its PROVENANCE.md).
 * This file is that vendor TU run through the era cpp with the vendor CPP flags
 * (-undef -P, -I include, -I tools/m2c), `SetData32` renamed to `func_80078FA0`, and vendor symbol
 * names mapped (post-cpp, token-wise) to this repo's address names from the vendor sym tables. */
/* Single-function extract: every other function definition of the vendor TU is reduced to a
 * prototype so this file compiles only `func_80078FA0` (the TU spans several of this repo's yaml functions). */
int func_80077DC4(int angle);
int func_80077CF4(int angle);
void func_80078EB4(void *v);
void func_80078EC4(void *v);
void func_80078ED4(void *v);
void func_80078EE4(void *v0, void *v1, void *v2);
void func_80078F04(void *c0, void *c1, void *c2);
void func_80078F18(int ir1, int ir2, int ir3);
void func_80078F2C(int ir0);
void func_80078F38(int sz1, int sz2, int sz3);
void func_80078F4C(int sz0, int sz1, int sz2, int sz3);
void func_80078F64(int sxy0, int sxy1, int sxy2);
void func_80078F78(int r11r12, int r22r23, int r33);
void func_80078F8C(int mac1, int mac2, int mac3);
void func_80078FA0(int data) {
    asm volatile("mtc2 %0,$30" : : "r"( data )) ;
}
void func_80078FAC(int dqa);
void func_80078FB8(int dqb);
