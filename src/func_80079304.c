/* vendor-derived: khasinski/parasite-eve-decomp@3067919e7b82f0334b873117eef07e7157f4ed69 src/main/psyq/libgte/RotTransPers4.c — attribution khasinski (Chris Hasinski) */
/* psyq libgte: Psy-Q SDK library code — matching build only; never pull into pc_port. */
/* inline GTE/asm: this function uses the vendor's inline PSY-Q GTE (COP2) asm macros — count separately from plain C. */
/* Imported under the owner's explicit authorization (2026-09-28, via Devbox Coordinator, vendor lane).
 * Upstream tree kept verbatim in vendor/khasinski-parasite-eve-decomp/ (see its PROVENANCE.md).
 * This file is that vendor TU run through the era cpp with the vendor CPP flags
 * (-undef -P, -I include, -I tools/m2c), `RotTransPers4` renamed to `func_80079304`, and vendor symbol
 * names mapped (post-cpp, token-wise) to this repo's address names from the vendor sym tables. */
typedef signed char s8;
typedef unsigned char u8;
typedef signed short s16;
typedef unsigned short u16;
typedef signed int s32;
typedef unsigned int u32;
typedef signed long long s64;
typedef unsigned long long u64;
typedef float f32;
int func_80077DC4(int angle);
int func_80077CF4(int angle);
long func_80079304(void *v0, void *v1, void *v2, void *v3,
                   void * volatile sxy0, void * volatile sxy1,
                   void * volatile sxy2, void * volatile sxy3,
                   void * volatile p, void * volatile flag) {
    register int  first_flags  asm("$3"); register int   flags  asm("$8"); register int   result  asm("$2") ;
    register void *gte_out0 asm("$8"); register void *gte_out1 asm("$9"); register void *gte_out2 asm("$10") ;
    asm volatile("lwc2 $0,0(%0)"  "\n\t" "lwc2 $1,4(%0)"  : : "r"( v0 ) : "memory") ;
    asm volatile("lwc2 $2,0(%0)\n\t" "lwc2 $3,4(%0)" : : "r"( v1 ) : "memory") ;
    asm volatile("lwc2 $4,0(%0)\n\t" "lwc2 $5,4(%0)" : : "r"( v2 ) : "memory") ;
    asm volatile("nop\n\t" ".word 0x4A280030" : : : "$8", "$9", "$10") ;
    do { gte_out0 = ( sxy0 ); gte_out1 = (  sxy1 ); gte_out2 = (  sxy2 ); } while (0) ;
    asm volatile("swc2 $12,0($8)" : : "r"(gte_out0) : "memory") ;
    asm volatile("swc2 $13,0($9)" : : "r"(gte_out1) : "memory") ;
    asm volatile("swc2 $14,0($10)" : : "r"(gte_out2) : "memory") ;
    asm volatile("cfc2 %0,$31" : "=r"( first_flags )) ;
    asm volatile("lwc2 $0,0(%0)"  "\n\t" "lwc2 $1,4(%0)"  : : "r"( v3 ) : "memory") ;
    asm volatile("nop\n\t" ".word 0x4A180001" : : : "$8", "$9", "$10") ;
    do { gte_out0 = ( sxy3 ); gte_out1 = (  p ); gte_out2 = (  flag ); } while (0) ;
    asm volatile("swc2 $14,0($8)" : : "r"(gte_out0) : "memory") ;
    asm volatile("swc2 $8,0($9)" : : "r"(gte_out1) : "memory") ;
    asm volatile("cfc2 %0,$31" : "=r"( flags )) ;
    asm volatile("mfc2 %0,$19" : "=r"( result )) ;
    flags |= first_flags;
    *(s32 *)gte_out2 = flags;
    asm volatile("" : : : "memory");
    return result >> 2;
}
static unsigned int tail_alignment_69B7C[] __attribute__((section(".text"))) = { 0x00000000, 0x00000000 };
