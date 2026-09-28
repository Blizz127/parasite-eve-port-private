/* vendor-derived: khasinski/parasite-eve-decomp@3067919e7b82f0334b873117eef07e7157f4ed69 src/main/psyq/libgte/average.c — attribution khasinski (Chris Hasinski) */
/* psyq libgte: Psy-Q SDK library code — matching build only; never pull into pc_port. */
/* inline GTE/asm: this function uses the vendor's inline PSY-Q GTE (COP2) asm macros — count separately from plain C. */
/* Imported under the owner's explicit authorization (2026-09-28, via Devbox Coordinator, vendor lane).
 * Upstream tree kept verbatim in vendor/khasinski-parasite-eve-decomp/ (see its PROVENANCE.md).
 * This file is that vendor TU run through the era cpp with the vendor CPP flags
 * (-undef -P, -I include, -I tools/m2c), `LoadAverage12` renamed to `func_80078344`, and vendor symbol
 * names mapped (post-cpp, token-wise) to this repo's address names from the vendor sym tables. */
/* Single-function extract: every other function definition of the vendor TU is reduced to a
 * prototype so this file compiles only `func_80078344` (the TU spans several of this repo's yaml functions). */
int func_80077DC4(int angle);
int func_80077CF4(int angle);
void func_80078344(void *first, void *second, int first_scale, int second_scale,
                   void * volatile output) {
    int lzcr;
    asm volatile("mtc2 %0,$8" : : "r"( first_scale )) ;
    asm volatile("lwc2 $9,0(%0)\n\t" "lwc2 $10,4(%0)\n\t" "lwc2 $11,8(%0)" : : "r"( first ) : "memory") ;
    asm volatile("nop\n\t" ".word 0x4B98003D") ;
    asm volatile("mfc2 %0,$31" : "=r"( lzcr )) ;
    asm volatile("mtc2 %0,$8" : : "r"( second_scale )) ;
    asm volatile("lwc2 $9,0(%0)\n\t" "lwc2 $10,4(%0)\n\t" "lwc2 $11,8(%0)" : : "r"( second ) : "memory") ;
    asm volatile("nop\n\t" ".word 0x4BA8003E") ;
    asm volatile("swc2 $9,0(%0)\n\t" "swc2 $10,4(%0)\n\t" "swc2 $11,8(%0)" : : "r"( output ) : "$3", "memory") ;
}
void func_80078394(void *first, void *second, int first_scale, int second_scale,
                  void * volatile output);
void func_800783E4(void *first, void *second, int first_scale,
                        int second_scale, void * volatile output);
void func_8007846C(void *first, void *second, int first_scale,
                       int second_scale, void * volatile output);
void func_800784F4(void *first, void *second, int first_scale,
                     int second_scale, void * volatile output);
void func_80078554(void *first, void *second, int first_scale,
                    int second_scale, void * volatile output);
