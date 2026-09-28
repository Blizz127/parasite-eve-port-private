/* vendor-derived: khasinski/parasite-eve-decomp@3067919e7b82f0334b873117eef07e7157f4ed69 src/main/psyq/libgte/pipeline.c — attribution khasinski (Chris Hasinski) */
/* psyq libgte: Psy-Q SDK library code — matching build only; never pull into pc_port. */
/* inline GTE/asm: this function uses the vendor's inline PSY-Q GTE (COP2) asm macros — count separately from plain C. */
/* Imported under the owner's explicit authorization (2026-09-28, via Devbox Coordinator, vendor lane).
 * Upstream tree kept verbatim in vendor/khasinski-parasite-eve-decomp/ (see its PROVENANCE.md).
 * This file is that vendor TU run through the era cpp with the vendor CPP flags
 * (-undef -P, -I include, -I tools/m2c), `OuterProduct0` renamed to `func_800791D0`, and vendor symbol
 * names mapped (post-cpp, token-wise) to this repo's address names from the vendor sym tables. */
/* Single-function extract: every other function definition of the vendor TU is reduced to a
 * prototype so this file compiles only `func_800791D0` (the TU spans several of this repo's yaml functions). */
int func_80077DC4(int angle);
int func_80077CF4(int angle);
void func_80079034(void *input, void *output);
void func_8007905C(void *input, void *color, int depth, void *output);
void func_80079084(void *rgb0, void *rgb1, void *rgb2, int interpolation,
               void * volatile out0, void * volatile out1,
               void * volatile out2);
void func_800790C0(void *input, int depth, void *output);
void *func_800790E4(void *input, void *output);
void *func_8007910C(void *input, void *output);
int func_80079134(int z1, int z2, int z3);
int func_80079154(int z0, int z1, int z2, int z3);
void func_80079178(void *matrix_column, void *vector, void *output);
void func_800791D0(void *matrix_column, void *vector, void *output) {
    asm volatile("cfc2 $13,$0\n\t" "cfc2 $14,$2\n\t" "cfc2 $15,$4" : : : "$13", "$14", "$15") ;
    asm volatile("lw $8,0(%0)\n\t" "lw $9,4(%0)\n\t" "lw $10,8(%0)\n\t" "ctc2 $8,$0\n\t" "ctc2 $9,$2\n\t" "ctc2 $10,$4" : : "r"( matrix_column ) : "$8", "$9", "$10", "memory") ;
    asm volatile("lwc2 $11,8(%0)\n\t" "lwc2 $9,0(%0)\n\t" "lwc2 $10,4(%0)" : : "r"( vector ) : "memory") ;
    asm volatile("nop\n\t" ".word 0x4B70000C") ;
    asm volatile("swc2 $25,0(%0)\n\t" "swc2 $26,4(%0)\n\t" "swc2 $27,8(%0)" : : "r"( output ) : "memory") ;
    asm volatile("ctc2 $13,$0\n\t" "ctc2 $14,$2\n\t" "ctc2 $15,$4" : : : "$13", "$14", "$15") ;
}
int func_80079228(int value);
