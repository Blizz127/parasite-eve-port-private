/* vendor-derived: khasinski/parasite-eve-decomp@3067919e7b82f0334b873117eef07e7157f4ed69 src/main/psyq/libgte/mtx_11.c — attribution khasinski (Chris Hasinski) */
/* psyq libgte: Psy-Q SDK library code — matching build only; never pull into pc_port. */
/* inline GTE/asm: this function uses the vendor's inline PSY-Q GTE (COP2) asm macros — count separately from plain C. */
/* Imported under the owner's explicit authorization (2026-09-28, via Devbox Coordinator, vendor lane).
 * Upstream tree kept verbatim in vendor/khasinski-parasite-eve-decomp/ (see its PROVENANCE.md).
 * This file is that vendor TU run through the era cpp with the vendor CPP flags
 * (-undef -P, -I include, -I tools/m2c), `SetColorMatrix` renamed to `func_80078E64`, and vendor symbol
 * names mapped (post-cpp, token-wise) to this repo's address names from the vendor sym tables. */
int func_80077DC4(int angle);
int func_80077CF4(int angle);
void func_80078E64(int *m) {
    register int r0 asm("$8") = m[0];
    register int r1 asm("$9") = m[1];
    register int r2 asm("$10") = m[2];
    register int r3 asm("$11") = m[3];
    register int r4 asm("$12") = m[4];
    asm volatile("ctc2 %0,$16" : : "r"( r0 )) ;
    asm volatile("ctc2 %0,$17" : : "r"( r1 )) ;
    asm volatile("ctc2 %0,$18" : : "r"( r2 )) ;
    asm volatile("ctc2 %0,$19" : : "r"( r3 )) ;
    asm volatile("ctc2 %0,$20" : : "r"( r4 )) ;
}
