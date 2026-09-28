/* vendor-derived: khasinski/parasite-eve-decomp@3067919e7b82f0334b873117eef07e7157f4ed69 src/main/psyq/libgte/smp_02.c — attribution khasinski (Chris Hasinski) */
/* psyq libgte: Psy-Q SDK library code — matching build only; never pull into pc_port. */
/* inline GTE/asm: this function uses the vendor's inline PSY-Q GTE (COP2) asm macros — count separately from plain C. */
/* Imported under the owner's explicit authorization (2026-09-28, via Devbox Coordinator, vendor lane).
 * Upstream tree kept verbatim in vendor/khasinski-parasite-eve-decomp/ (see its PROVENANCE.md).
 * This file is that vendor TU run through the era cpp with the vendor CPP flags
 * (-undef -P, -I include, -I tools/m2c), `RotTransPers` renamed to `func_80079244`, and vendor symbol
 * names mapped (post-cpp, token-wise) to this repo's address names from the vendor sym tables. */
int func_80077DC4(int angle);
int func_80077CF4(int angle);
typedef signed char s8;
typedef unsigned char u8;
typedef signed short s16;
typedef unsigned short u16;
typedef signed int s32;
typedef unsigned int u32;
typedef signed long long s64;
typedef unsigned long long u64;
typedef float f32;
typedef struct GteMatrix {
    s16 m[3][3];
    s32 t[3];
} GteMatrix;
typedef struct GteMatrixWords {
    u32 r11_r12;
    u32 r13_r21;
    u32 r22_r23;
    u32 r31_r32;
    u32 r33_pad;
    s32 tx, ty, tz;
} GteMatrixWords;
typedef char pe1_static_assert_gte_matrix_words_size [( sizeof(GteMatrixWords) == sizeof(GteMatrix) ) ? 1 : -1] ;
typedef char pe1_static_assert_gte_matrix_words_last_rotation_offset [( ((u32)&((( GteMatrixWords  *)0)->  r33_pad ))  == 16 ) ? 1 : -1] ;
typedef char pe1_static_assert_gte_matrix_words_translation_offset [( ((u32)&((( GteMatrixWords  *)0)->  tx ))  == ((u32)&((( GteMatrix  *)0)->  t ))  ) ? 1 : -1] ;
typedef union GteMatrixStorage {
    GteMatrix matrix;
    u32 words[8];
} GteMatrixStorage;
typedef char pe1_static_assert_gte_matrix_storage_size [( sizeof(GteMatrixStorage) == 32 ) ? 1 : -1] ;
typedef struct GteShortVector {
    s16 x, y, z, pad;
} GteShortVector;
typedef struct GteVector {
    s32 x, y, z, pad;
} GteVector;
GteMatrix *func_80079754(GteShortVector *angles, GteMatrix *matrix);
GteShortVector *func_80078C34(const GteMatrix *matrix,
                            const GteShortVector *v, GteShortVector *out);
GteVector *func_80078934(const GteMatrix *matrix, const GteVector *v,
                         GteVector *out);
void func_800783E4(void *first, void *second, int first_scale,
                        int second_scale, void *output);
GteMatrix *func_80078CC4(GteMatrix *matrix, const GteVector *scale);
int func_80079FB4(int y, int x);
int func_80078004(int value);
void func_80078134(GteVector *vector, GteVector *unit);
typedef char pe1_static_assert_gte_matrix_size [( sizeof(GteMatrix) == 32 ) ? 1 : -1] ;
typedef char pe1_static_assert_gte_matrix_translation_offset [( ((u32)&((( GteMatrix  *)0)->  t ))  == 20 ) ? 1 : -1] ;
typedef char pe1_static_assert_gte_short_vector_size [( sizeof(GteShortVector) == 8 ) ? 1 : -1] ;
typedef char pe1_static_assert_gte_vector_size [( sizeof(GteVector) == 16 ) ? 1 : -1] ;
typedef struct GteRotation {
    s16 x;
    s16 y;
    s16 z;
    s16 flags;
} GteRotation;
int func_80079244(const GteShortVector *v, s32 *sxy, s32 *p, s32 *flag) {
    int result;
    int flagValue;
    asm volatile("lwc2 $0,0(%0)"  : : "r"( v ) : "memory") ;
    asm volatile("lwc2 $1,4(%0)"  : : "r"( v ) : "memory") ;
    asm volatile("nop") ;
    asm volatile(".word 0x4A180001") ;
    asm volatile("swc2 $14,0(%0)" : : "r"( sxy ) : "memory") ;
    asm volatile("swc2 $8,0(%0)" : : "r"( p ) : "memory") ;
    asm volatile("cfc2 %0,$31" : "=r"( flagValue )) ;
    asm volatile("mfc2 %0,$19" : "=r"( result )) ;
    *flag = flagValue;
    return result >> 2;
}
