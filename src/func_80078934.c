/* vendor-derived: khasinski/parasite-eve-decomp@3067919e7b82f0334b873117eef07e7157f4ed69 src/main/gte/Gte_RotateVec.c — attribution khasinski (Chris Hasinski) */
/* psyq libgte: Psy-Q SDK library code — matching build only; never pull into pc_port. */
/* inline GTE/asm: this function uses the vendor's inline PSY-Q GTE (COP2) asm macros — count separately from plain C. */
/* Imported under the owner's explicit authorization (2026-09-28, via Devbox Coordinator, vendor lane).
 * Upstream tree kept verbatim in vendor/khasinski-parasite-eve-decomp/ (see its PROVENANCE.md).
 * This file is that vendor TU run through the era cpp with the vendor CPP flags
 * (-undef -P, -I include, -I tools/m2c), `Gte_RotateVec` renamed to `func_80078934`, and vendor symbol
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
int func_80077DC4(int angle);
int func_80077CF4(int angle);
GteVector *func_80078934(const GteMatrix *matrix, const GteVector *v, GteVector *out) {
    register s32 x asm("$8");
    register s32 y asm("$9");
    register s32 z asm("$10");
    register s32 hx asm("$11");
    register s32 hy asm("$12");
    register s32 hz asm("$13");
    register int zero asm("$0");
    register GteVector *dst asm("$6") = out;
    const GteMatrixWords *m = (const GteMatrixWords *)matrix;
    x = m->r11_r12;
    y = m->r13_r21;
    z = m->r22_r23;
    hx = m->r31_r32;
    hy = m->r33_pad;
    asm volatile("ctc2 %0,$0" : : "r"( x )) ;
    asm volatile("ctc2 %0,$1" : : "r"( y )) ;
    asm volatile("ctc2 %0,$2" : : "r"( z )) ;
    asm volatile("ctc2 %0,$3" : : "r"( hx )) ;
    asm volatile("ctc2 %0,$4" : : "r"( hy )) ;
    x = v->x;
    y = v->y;
    z = v->z;
    if (x >= 0) {
        goto positive_x;
    }
    x = 0U - (u32)x;
    hx = x >> 15;
    hx = 0U - (u32)hx;
    x &= 0x7fff;
    asm volatile("" : "=r"(zero));
    x = 0U - (u32)x;
    if (zero == 0) {
        goto split_x;
    }
positive_x:
    asm volatile("" : "=r"(x) : "0"(x));
    hx = x >> 15;
    x &= 0x7fff;
split_x:
    if (y >= 0) {
        goto positive_y;
    }
    y = 0U - (u32)y;
    hy = y >> 15;
    hy = 0U - (u32)hy;
    y &= 0x7fff;
    asm volatile("" : "=r"(zero));
    y = 0U - (u32)y;
    if (zero == 0) {
        goto split_y;
    }
positive_y:
    asm volatile("" : "=r"(y) : "0"(y));
    hy = y >> 15;
    y &= 0x7fff;
split_y:
    if (z >= 0) {
        goto positive_z;
    }
    z = 0U - (u32)z;
    hz = z >> 15;
    hz = 0U - (u32)hz;
    z &= 0x7fff;
    asm volatile("" : "=r"(zero));
    z = 0U - (u32)z;
    if (zero == 0) {
        goto split_z;
    }
positive_z:
    asm volatile("" : "=r"(z) : "0"(z));
    hz = z >> 15;
    z &= 0x7fff;
split_z:
    asm volatile("mtc2 %0,$9" : : "r"( hx )) ;
    asm volatile("mtc2 %0,$10" : : "r"( hy )) ;
    asm volatile("mtc2 %0,$11" : : "r"( hz )) ;
    asm volatile("nop") ;
    asm volatile(".word 0x4A41E012") ;
    asm volatile("mfc2 %0,$25" : "=r"( hx )) ;
    asm volatile("mfc2 %0,$26" : "=r"( hy )) ;
    asm volatile("mfc2 %0,$27" : "=r"( hz )) ;
    asm volatile("mtc2 %0,$9" : : "r"( x )) ;
    asm volatile("mtc2 %0,$10" : : "r"( y )) ;
    asm volatile("mtc2 %0,$11" : : "r"( z )) ;
    asm volatile("nop") ;
    asm volatile(".word 0x4A49E012") ;
    if (hx >= 0) {
        goto shift_positive_x;
    }
    hx = 0U - (u32)hx;
    hx = (u32)hx << 3;
    asm volatile("" : "=r"(zero));
    hx = 0U - (u32)hx;
    if (zero == 0) {
        goto shifted_x;
    }
shift_positive_x:
    hx = (u32)hx << 3;
shifted_x:
    if (hy >= 0) {
        goto shift_positive_y;
    }
    hy = 0U - (u32)hy;
    hy = (u32)hy << 3;
    asm volatile("" : "=r"(zero));
    hy = 0U - (u32)hy;
    if (zero == 0) {
        goto shifted_y;
    }
shift_positive_y:
    hy = (u32)hy << 3;
shifted_y:
    if (hz >= 0) {
        goto shift_positive_z;
    }
    hz = 0U - (u32)hz;
    hz = (u32)hz << 3;
    asm volatile("" : "=r"(zero));
    hz = 0U - (u32)hz;
    if (zero == 0) {
        goto shifted_z;
    }
shift_positive_z:
    hz = (u32)hz << 3;
shifted_z:
    asm volatile("mfc2 %0,$25" : "=r"( x )) ;
    asm volatile("mfc2 %0,$26" : "=r"( y )) ;
    asm volatile("mfc2 %0,$27" : "=r"( z )) ;
    x = (u32)x + hx;
    y = (u32)y + hy;
    z = (u32)z + hz;
    dst->x = x;
    dst->y = y;
    dst->z = z;
    asm volatile("" : "=r"(dst) : "0"(dst));
    return dst;
}
