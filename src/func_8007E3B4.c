/* vendor-derived: khasinski/parasite-eve-decomp@3067919e7b82f0334b873117eef07e7157f4ed69 src/main/memcard/CardPatchFunctions.c — attribution khasinski (Chris Hasinski) */
/* Imported under the owner's explicit authorization (2026-09-28, via Devbox Coordinator, vendor lane).
 * Upstream tree kept verbatim in vendor/khasinski-parasite-eve-decomp/ (see its PROVENANCE.md).
 * This file is that vendor TU run through the era cpp with the vendor CPP flags
 * (-undef -P, -I include, -I tools/m2c), `func_8007E3B4` renamed to `func_8007E3B4`, and vendor symbol
 * names mapped (post-cpp, token-wise) to this repo's address names from the vendor sym tables. */
/* Single-function extract: every other function definition of the vendor TU is reduced to a
 * prototype so this file compiles only `func_8007E3B4` (the TU spans several of this repo's yaml functions). */
typedef signed char s8;
typedef unsigned char u8;
typedef signed short s16;
typedef unsigned short u16;
typedef signed int s32;
typedef unsigned int u32;
typedef signed long long s64;
typedef unsigned long long u64;
typedef float f32;
typedef struct CardPatchIoRegisters {
    u8 reserved0000[0x1044];
    volatile u32 serialStatus;
    u8 reserved1048[0x2C];
    volatile u32 interruptMask;
} CardPatchIoRegisters;
typedef char pe1_static_assert_card_patch_serial_status_offset [( ((u32)&((( CardPatchIoRegisters  *)0)->  serialStatus ))  == 0x1044 ) ? 1 : -1] ;
typedef char pe1_static_assert_card_patch_interrupt_mask_offset [( ((u32)&((( CardPatchIoRegisters  *)0)->  interruptMask ))  == 0x1074 ) ? 1 : -1] ;
extern void *volatile D_0000DFFC;
extern const u8 D_A000DFAC[];
extern const u32 func_8007E344[];
void func_8007E3B4(void);
register CardPatchIoRegisters *cardPatchIo asm("$3");
void CardPatchFunctions(void);
void func_8007E3B4(void) {
    goto *(void *)D_A000DFAC;
}
