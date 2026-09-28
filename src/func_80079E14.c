/* vendor-derived: khasinski/parasite-eve-decomp@3067919e7b82f0334b873117eef07e7157f4ed69 src/main/psyq/libgte/RotMatrixZ.c — attribution khasinski (Chris Hasinski) */
/* inline GTE/asm: this function uses vendor inline asm (hand-written instructions) — count separately from plain C. */
/* psyq libgte: Psy-Q SDK library code — matching build only; never pull into pc_port. */
/* Local lever: the vendor GNU-as route ends with an `asm volatile("nop")` before the return that GNU as moves into the
 * `jr $ra` delay slot; on this repo's maspsx route it is an empty asm barrier (keeps the final store out of the slot). */
/* Imported under the owner's explicit authorization (2026-09-28, via Devbox Coordinator, vendor lane).
 * Upstream tree kept verbatim in vendor/khasinski-parasite-eve-decomp/ (see its PROVENANCE.md).
 * This file is that vendor TU run through the era cpp with the vendor CPP flags
 * (-undef -P, -I include, -I tools/m2c), `RotMatrixZ` renamed to `func_80079E14`, and vendor symbol
 * names mapped (post-cpp, token-wise) to this repo's address names from the vendor sym tables. */
/* vendor markers: ASSEMBLER: GNU */
/* Single-function extract: every other function definition of the vendor TU is reduced to a
 * prototype so this file compiles only `func_80079E14` (the TU spans several of this repo's yaml functions). */
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
extern u32 D_800966EC[];
GteMatrix *func_80079E14(s32 angle, GteMatrix *m) {
    unsigned long long product;
    unsigned low;
    register s32 a asm("$15") = angle;
    register GteMatrix *ret = m;
    register u32 packed asm("$25");
    register u32 temp asm("$24");
    register s32 scratch asm("$14");
    register s32 cosine asm("$8");
    register s32 negative_sine asm("$9");
    register s32 x0 asm("$10");
    register s32 x1 asm("$11");
    register s32 x2 asm("$12");
    register s32 y0 asm("$13");
    register s32 y1 asm("$14");
    register s32 y2 asm("$15");
    asm volatile("" : "=r"(a), "=r"(ret) : "0"(a), "1"(ret));
    packed = (u32)a & 0xfff;
    if (a < 0) {
        a = 0U - (u32)a;
        if (a < 0)
            asm volatile("" : : : "15");
        else
            a &= 0xfff;
        asm volatile("" : : : "24");
        temp = (u32)a << 2;
        packed = *(u32 *)((char *)D_800966EC + temp);
        scratch = packed << 16;
        asm volatile("" : "=r"(scratch) : "0"(scratch));
        scratch = scratch >> 16;
        asm volatile("" : "=r"(scratch) : "0"(scratch));
        negative_sine = 0U - (u32)scratch;
        cosine = (s32)packed >> 16;
    } else {
        temp = packed << 2;
        packed = *(u32 *)((char *)D_800966EC + temp);
        temp = packed << 16;
        asm volatile("" : "=r"(temp) : "0"(temp));
        negative_sine = (s32)temp >> 16;
        cosine = (s32)packed >> 16;
        asm volatile("" : "=r"(cosine) : "0"(cosine));
    }
    x0 = m->m[0][0];
    y0 = m->m[1][0];
    asm volatile("" : : "r"(x0), "r"(y0));
    product = (unsigned long long)(u32)cosine * (u32)x0;
    x1 = m->m[0][1];
    asm volatile("" : : "r"(x1) : "memory");
    y1 = m->m[1][1];
    asm volatile("" : : "x"(product), "r"(x1), "r"(y1));
    asm volatile("" : "=l"(low) : "x"(product));
    temp = low;
    asm volatile("" : "=r"(temp) : "0"(temp));
    x2 = m->m[0][2];
    asm volatile("" : : "r"(x2) : "memory");
    y2 = m->m[1][2];
    asm volatile("" : : "r"(temp), "r"(x2), "r"(y2));
    product = (unsigned long long)(u32)negative_sine * (u32)y0;
    asm volatile("nop" : :  "x" (  product )) ;
    asm volatile("nop" : :  "x" (  product )) ;
    asm volatile("" : "=l"(low) : "x"(product));
    packed = low;
    asm volatile("" : "=r"(temp), "=r"(packed) : "0"(temp), "1"(packed));
    packed = temp - packed;
    temp = (s32)packed >> 12;
    asm volatile("" : : "r"(temp));
    m->m[0][0] = temp;
    product = (unsigned long long)(u32)cosine * (u32)x1;
    asm volatile("nop" : :  "x" (  product )) ;
    asm volatile("" : "=l"(low) : "x"(product));
    temp = low;
    asm volatile("" : "=r"(temp) : "0"(temp));
    product = (unsigned long long)(u32)negative_sine * (u32)y1;
    asm volatile("nop" : :  "x" (  product )) ;
    asm volatile("nop" : :  "x" (  product )) ;
    asm volatile("" : "=l"(low) : "x"(product));
    packed = low;
    asm volatile("" : "=r"(temp), "=r"(packed) : "0"(temp), "1"(packed));
    packed = temp - packed;
    temp = (s32)packed >> 12;
    asm volatile("" : : "r"(temp));
    m->m[0][1] = temp;
    product = (unsigned long long)(u32)cosine * (u32)x2;
    asm volatile("nop" : :  "x" (  product )) ;
    asm volatile("" : "=l"(low) : "x"(product));
    temp = low;
    asm volatile("" : "=r"(temp) : "0"(temp));
    product = (unsigned long long)(u32)negative_sine * (u32)y2;
    asm volatile("nop" : :  "x" (  product )) ;
    asm volatile("nop" : :  "x" (  product )) ;
    asm volatile("" : "=l"(low) : "x"(product));
    packed = low;
    asm volatile("" : "=r"(temp), "=r"(packed) : "0"(temp), "1"(packed));
    packed = temp - packed;
    temp = (s32)packed >> 12;
    asm volatile("" : : "r"(temp));
    m->m[0][2] = temp;
    product = (unsigned long long)(u32)negative_sine * (u32)x0;
    asm volatile("nop" : :  "x" (  product )) ;
    asm volatile("" : "=l"(low) : "x"(product));
    temp = low;
    asm volatile("" : "=r"(temp) : "0"(temp));
    product = (unsigned long long)(u32)cosine * (u32)y0;
    asm volatile("nop" : :  "x" (  product )) ;
    asm volatile("nop" : :  "x" (  product )) ;
    asm volatile("" : "=l"(low) : "x"(product));
    packed = low;
    asm volatile("" : "=r"(temp), "=r"(packed) : "0"(temp), "1"(packed));
    packed = temp + packed;
    temp = (s32)packed >> 12;
    asm volatile("" : : "r"(temp));
    m->m[1][0] = temp;
    product = (unsigned long long)(u32)negative_sine * (u32)x1;
    asm volatile("nop" : :  "x" (  product )) ;
    asm volatile("" : "=l"(low) : "x"(product));
    temp = low;
    asm volatile("" : "=r"(temp) : "0"(temp));
    product = (unsigned long long)(u32)cosine * (u32)y1;
    asm volatile("nop" : :  "x" (  product )) ;
    asm volatile("nop" : :  "x" (  product )) ;
    asm volatile("" : "=l"(low) : "x"(product));
    packed = low;
    asm volatile("" : "=r"(temp), "=r"(packed) : "0"(temp), "1"(packed));
    packed = temp + packed;
    temp = (s32)packed >> 12;
    asm volatile("" : : "r"(temp));
    m->m[1][1] = temp;
    product = (unsigned long long)(u32)negative_sine * (u32)x2;
    asm volatile("nop" : :  "x" (  product )) ;
    asm volatile("" : "=l"(low) : "x"(product));
    temp = low;
    asm volatile("" : "=r"(temp) : "0"(temp));
    product = (unsigned long long)(u32)cosine * (u32)y2;
    asm volatile("nop" : :  "x" (  product )) ;
    asm volatile("nop" : :  "x" (  product )) ;
    asm volatile("" : "=l"(low) : "x"(product));
    packed = low;
    asm volatile("" : "=r"(temp), "=r"(packed) : "0"(temp), "1"(packed));
    packed = temp + packed;
    temp = (s32)packed >> 12;
    asm volatile("" : : "r"(temp));
    m->m[1][2] = temp;
    asm volatile("" : : : "memory"); return ret;
}
