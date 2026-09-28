/* vendor-derived: khasinski/parasite-eve-decomp@3067919e7b82f0334b873117eef07e7157f4ed69 src/main/psyq/libgte/reg10.c — attribution khasinski (Chris Hasinski) */
/* psyq libgte: Psy-Q SDK library code — matching build only; never pull into pc_port. */
/* inline GTE/asm: this function uses the vendor's inline PSY-Q GTE (COP2) asm macros — count separately from plain C. */
/* Imported under the owner's explicit authorization (2026-09-28, via Devbox Coordinator, vendor lane).
 * Upstream tree kept verbatim in vendor/khasinski-parasite-eve-decomp/ (see its PROVENANCE.md).
 * This file is that vendor TU run through the era cpp with the vendor CPP flags
 * (-undef -P, -I include, -I tools/m2c), `SetBackColor` renamed to `func_80078FC4`, and vendor symbol
 * names mapped (post-cpp, token-wise) to this repo's address names from the vendor sym tables. */
void func_80078FC4(int r, int g, int b) {
    r <<= 4;
    g <<= 4;
    b <<= 4;
    asm volatile("ctc2 %0,$13" : : "r"(r));
    asm volatile("ctc2 %0,$14" : : "r"(g));
    asm volatile("ctc2 %0,$15" : : "r"(b));
}
