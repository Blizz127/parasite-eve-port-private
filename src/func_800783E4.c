/* vendor-derived: khasinski/parasite-eve-decomp@3067919e7b82f0334b873117eef07e7157f4ed69 src/main/psyq/libgte/average.c — attribution khasinski (Chris Hasinski) */
/* psyq libgte: Psy-Q SDK library code — matching build only; never pull into pc_port. */
/* inline GTE/asm: this function uses the vendor's inline PSY-Q GTE (COP2) asm macros — count separately from plain C. */
/* Imported under the owner's explicit authorization (2026-09-28, via Devbox Coordinator, vendor lane).
 * Upstream tree kept verbatim in vendor/khasinski-parasite-eve-decomp/ (see its PROVENANCE.md).
 * This file is that vendor TU run through the era cpp with the vendor CPP flags
 * (-undef -P, -I include, -I tools/m2c), `LoadAverageShort12` renamed to `func_800783E4`, and vendor symbol
 * names mapped (post-cpp, token-wise) to this repo's address names from the vendor sym tables. */
/* Single-function extract: every other function definition of the vendor TU is reduced to a
 * prototype so this file compiles only `func_800783E4` (the TU spans several of this repo's yaml functions). */
int func_80077DC4(int angle);
int func_80077CF4(int angle);
void func_80078344(void *first, void *second, int first_scale, int second_scale,
                   void * volatile output);
void func_80078394(void *first, void *second, int first_scale, int second_scale,
                  void * volatile output);
void func_800783E4(void *first, void *second, int first_scale,
                        int second_scale, void * volatile output) {
    int lzcr;
    register int *out asm("$13");
    register int gte_x asm("$8"); register int gte_y asm("$9") ; register int gte_z asm("$10") ;
    gte_x = ((int *)first)[0];
    gte_z = ((volatile int *)first)[1];
    gte_y = gte_x >> 16;
    gte_x &= 0xFFFF;
    gte_z &= 0xFFFF;
    asm volatile("mtc2 %0,$8\n\t" "mtc2 $8,$9\n\t" "mtc2 $9,$10\n\t" "mtc2 $10,$11" : : "r"( first_scale ), "r"(gte_x), "r"(gte_y), "r"(gte_z)) ;
    asm volatile("nop\n\t" ".word 0x4B98003D") ;
    gte_x = ((int *)second)[0];
    gte_z = ((volatile int *)second)[1];
    gte_y = gte_x >> 16;
    gte_x &= 0xFFFF;
    gte_z &= 0xFFFF;
    asm volatile("mfc2 %0,$31" : "=r"( lzcr )) ;
    asm volatile("mtc2 %0,$8\n\t" "mtc2 $8,$9\n\t" "mtc2 $9,$10\n\t" "mtc2 $10,$11" : : "r"( second_scale ), "r"(gte_x), "r"(gte_y), "r"(gte_z)) ;
    asm volatile("nop\n\t" ".word 0x4BA8003E") ;
    asm volatile("mfc2 %0,$9" : "=r"( gte_x )) ;
    asm volatile("mfc2 %0,$10" : "=r"( gte_y )) ;
    gte_x &= 0xFFFF;
    gte_y = (unsigned int)gte_y << 16;
    gte_x |= gte_y;
    out = output;
    asm volatile("mfc2 %0,$11" : "=r"( gte_z )) ;
    ((volatile int *)out)[0] = gte_x;
    ((volatile int *)out)[1] = gte_z;
}
void func_8007846C(void *first, void *second, int first_scale,
                       int second_scale, void * volatile output);
void func_800784F4(void *first, void *second, int first_scale,
                     int second_scale, void * volatile output);
void func_80078554(void *first, void *second, int first_scale,
                    int second_scale, void * volatile output);
