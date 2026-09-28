/* vendor-derived: khasinski/parasite-eve-decomp@3067919e7b82f0334b873117eef07e7157f4ed69 src/main/psyq/libgte/RotMatrixYXZ.c — attribution khasinski (Chris Hasinski) */
/* inline GTE/asm: this function uses vendor inline asm (hand-written instructions) — count separately from plain C. */
/* psyq libgte: Psy-Q SDK library code — matching build only; never pull into pc_port. */
/* Local lever: the vendor GNU-as route ends with an `asm volatile("nop")` before the return that GNU as moves into the
 * `jr $ra` delay slot; on this repo's maspsx route it is an empty asm barrier (keeps the final store out of the slot). */
/* Imported under the owner's explicit authorization (2026-09-28, via Devbox Coordinator, vendor lane).
 * Upstream tree kept verbatim in vendor/khasinski-parasite-eve-decomp/ (see its PROVENANCE.md).
 * This file is that vendor TU run through the era cpp with the vendor CPP flags
 * (-undef -P, -I include, -I tools/m2c), `RotMatrixYXZ` renamed to `func_80079754`, and vendor symbol
 * names mapped (post-cpp, token-wise) to this repo's address names from the vendor sym tables. */
/* vendor markers: ASSEMBLER: GNU */
/* Single-function extract: every other function definition of the vendor TU is reduced to a
 * prototype so this file compiles only `func_80079754` (the TU spans several of this repo's yaml functions). */
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
GteMatrix *func_80079754(GteShortVector *angles, GteMatrix *matrix) {
    register GteMatrix *result;
    unsigned long long product;
    unsigned low;
    register s32 cx asm("$8");
    register s32 cy asm("$9");
    register s32 cz asm("$10");
    register s32 sx asm("$11");
    register s32 sy asm("$12");
    register s32 sz asm("$13");
    register s32 value asm("$14");
    register s32 angle asm("$15");
    register s32 temp asm("$24");
    register s32 word asm("$25");
    angle = angles->x;
    result = matrix;
    word = (u32)angle & 0xFFF;
    if (angle >= 0)
        goto positive_x;
    angle = 0U - (u32)angle;
    if (angle < 0)
        asm volatile("" : : : "15");
    else
        angle &= 0xfff;
    asm volatile("" : : : "24");
    temp = ((u32)angle) << 2;
    word = *(u32 *)((char *)D_800966EC + temp);
    asm volatile("nop" : :  "r" (  word ) : "memory") ;
    value = ((u32)word) << 16;
    value = (s32)value >> 16;
    sx = 0U - (u32)value;
    cx = (s32)word >> 16;
    goto x_ready;
positive_x:;
    temp = ((u32)word) << 2;
    word = *(u32 *)((char *)D_800966EC + temp);
    asm volatile("nop" : :  "r" (  word ) : "memory") ;
    temp = ((u32)word) << 16;
    sx = (s32)temp >> 16;
    value = 0U - (u32)sx;
    cx = (s32)word >> 16;
    asm volatile("" : "=r"(cx) : "0"(cx));
x_ready:;
    angle = angles->y;
    asm volatile("nop" : :  "r" (  angle ) : "memory") ;
    word = (u32)angle & 0xFFF;
    if (angle >= 0)
        goto positive_y;
    angle = 0U - (u32)angle;
    if (angle < 0)
        asm volatile("" : : : "15");
    else
        angle &= 0xfff;
    asm volatile("" : : : "24");
    temp = ((u32)angle) << 2;
    word = *(u32 *)((char *)D_800966EC + temp);
    asm volatile("nop" : :  "r" (  word ) : "memory") ;
    temp = ((u32)word) << 16;
    temp = (s32)temp >> 16;
    sy = 0U - (u32)temp;
    cy = (s32)word >> 16;
    goto y_ready;
positive_y:;
    temp = ((u32)word) << 2;
    word = *(u32 *)((char *)D_800966EC + temp);
    asm volatile("nop" : :  "r" (  word ) : "memory") ;
    temp = ((u32)word) << 16;
    sy = (s32)temp >> 16;
    cy = (s32)word >> 16;
    asm volatile("" : "=r"(cy) : "0"(cy));
y_ready:;
    product = (unsigned long long)(u32)sy * (u32)cx;
    asm volatile("" : : "x"(product) : "memory");
    angle = angles->z;
    matrix->m[1][2] = value;
    asm volatile("" : "=l"(low) : "x"(product));
    temp = low;
    asm volatile("" : "=r"(temp) : "0"(temp));
    value = (s32)temp >> 12;
    asm volatile("nop" : :  "r" (  value ) : "memory") ;
    product = (unsigned long long)(u32)cy * (u32)cx;
    asm volatile("" : : "x"(product) : "memory");
    matrix->m[0][2] = value;
    word = (u32)angle & 0xFFF;
    if (angle >= 0)
        goto positive_z;
    asm volatile("" : "=l"(low) : "x"(product));
    temp = low;
    asm volatile("" : "=r"(temp) : "0"(temp));
    value = (s32)temp >> 12;
    matrix->m[2][2] = value;
    angle = 0U - (u32)angle;
    asm volatile("" : "=r"(angle) : "0"(angle));
    if (angle < 0)
        asm volatile("" : : : "15");
    else
        angle &= 0xfff;
    asm volatile("" : : : "24");
    temp = ((u32)angle) << 2;
    asm volatile("" : "=r"(temp) : "0"(temp));
    word = *(u32 *)((char *)D_800966EC + temp);
    asm volatile("nop" : :  "r" (  word ) : "memory") ;
    temp = ((u32)word) << 16;
    asm volatile("" : "=r"(temp) : "0"(temp));
    temp = (s32)temp >> 16;
    asm volatile("" : "=r"(temp) : "0"(temp));
    sz = 0U - (u32)temp;
    cz = (s32)word >> 16;
    goto z_ready;
positive_z:;
    asm volatile("" : "=l"(low) : "x"(product));
    angle = low;
    asm volatile("" : "=r"(angle) : "0"(angle));
    value = (s32)angle >> 12;
    matrix->m[2][2] = value;
    asm volatile("" : : "m"(matrix->m[2][2]) : "memory");
    temp = ((u32)word) << 2;
    asm volatile("" : "=r"(temp) : "0"(temp));
    word = *(u32 *)((char *)D_800966EC + temp);
    asm volatile("nop" : :  "r" (  word ) : "memory") ;
    temp = ((u32)word) << 16;
    asm volatile("" : "=r"(temp) : "0"(temp));
    sz = (s32)temp >> 16;
    cz = (s32)word >> 16;
    asm volatile("" : "=r"(cz) : "0"(cz));
z_ready:;
    product = (unsigned long long)(u32)sz * (u32)cx;
    asm volatile("" : : "x"(product) : "memory");
    asm volatile("nop" : :  "x" (  product ) : "memory") ;
    asm volatile("nop" : :  "x" (  product ) : "memory") ;
    asm volatile("" : "=l"(low) : "x"(product));
    angle = low;
    asm volatile("" : "=r"(angle) : "0"(angle));
    value = (s32)angle >> 12;
    matrix->m[1][0] = value;
    asm volatile("" : : "m"(matrix->m[1][0]) : "memory");
    product = (unsigned long long)(u32)cz * (u32)cx;
    asm volatile("" : : "x"(product) : "memory");
    asm volatile("nop" : :  "x" (  product ) : "memory") ;
    asm volatile("nop" : :  "x" (  product ) : "memory") ;
    asm volatile("" : "=l"(low) : "x"(product));
    angle = low;
    asm volatile("" : "=r"(angle) : "0"(angle));
    value = (s32)angle >> 12;
    asm volatile("nop" : :  "r" (  value ) : "memory") ;
    product = (unsigned long long)(u32)sy * (u32)sx;
    asm volatile("" : : "x"(product) : "memory");
    matrix->m[1][1] = value;
    asm volatile("nop" : :  "x" (  product ) : "memory") ;
    asm volatile("" : "=l"(low) : "x"(product));
    angle = low;
    asm volatile("" : "=r"(angle) : "0"(angle));
    temp = (s32)angle >> 12;
    asm volatile("" : "=r"(temp) : "0"(temp));
    asm volatile("nop" : :  "r" (  temp ) : "memory") ;
    product = (unsigned long long)(u32)temp * (u32)sz;
    asm volatile("" : : "x"(product) : "memory");
    asm volatile("nop" : :  "x" (  product ) : "memory") ;
    asm volatile("nop" : :  "x" (  product ) : "memory") ;
    asm volatile("" : "=l"(low) : "x"(product));
    angle = low;
    asm volatile("" : "=r"(angle) : "0"(angle));
    value = (s32)angle >> 12;
    asm volatile("nop" : :  "r" (  value ) : "memory") ;
    product = (unsigned long long)(u32)cy * (u32)cz;
    asm volatile("" : : "x"(product) : "memory");
    asm volatile("nop" : :  "x" (  product ) : "memory") ;
    asm volatile("nop" : :  "x" (  product ) : "memory") ;
    asm volatile("" : "=l"(low) : "x"(product));
    angle = low;
    asm volatile("" : "=r"(angle) : "0"(angle));
    word = (s32)angle >> 12;
    angle = (u32)word + value;
    asm volatile("" : "=r"(angle) : "0"(angle));
    product = (unsigned long long)(u32)cy * (u32)sz;
    asm volatile("" : : "x"(product) : "memory");
    matrix->m[0][0] = angle;
    asm volatile("nop" : :  "x" (  product ) : "memory") ;
    asm volatile("" : "=l"(low) : "x"(product));
    value = low;
    asm volatile("" : "=r"(value) : "0"(value));
    word = (s32)value >> 12;
    angle = 0U - (u32)word;
    asm volatile("" : "=r"(angle) : "0"(angle));
    product = (unsigned long long)(u32)temp * (u32)cz;
    asm volatile("" : : "x"(product) : "memory");
    asm volatile("nop" : :  "x" (  product ) : "memory") ;
    asm volatile("nop" : :  "x" (  product ) : "memory") ;
    asm volatile("" : "=l"(low) : "x"(product));
    value = low;
    asm volatile("" : "=r"(value) : "0"(value));
    word = (s32)value >> 12;
    value = (u32)angle + word;
    asm volatile("" : "=r"(value) : "0"(value));
    product = (unsigned long long)(u32)cy * (u32)sx;
    asm volatile("" : : "x"(product) : "memory");
    matrix->m[0][1] = value;
    asm volatile("nop" : :  "x" (  product ) : "memory") ;
    asm volatile("" : "=l"(low) : "x"(product));
    angle = low;
    asm volatile("" : "=r"(angle) : "0"(angle));
    temp = (s32)angle >> 12;
    asm volatile("" : "=r"(temp) : "0"(temp));
    asm volatile("nop" : :  "r" (  temp ) : "memory") ;
    product = (unsigned long long)(u32)temp * (u32)cz;
    asm volatile("" : : "x"(product) : "memory");
    asm volatile("nop" : :  "x" (  product ) : "memory") ;
    asm volatile("nop" : :  "x" (  product ) : "memory") ;
    asm volatile("" : "=l"(low) : "x"(product));
    angle = low;
    asm volatile("" : "=r"(angle) : "0"(angle));
    value = (s32)angle >> 12;
    asm volatile("" : "=r"(value) : "0"(value));
    asm volatile("nop" : :  "r" (  value ) : "memory") ;
    product = (unsigned long long)(u32)sy * (u32)sz;
    asm volatile("" : : "x"(product) : "memory");
    asm volatile("nop" : :  "x" (  product ) : "memory") ;
    asm volatile("nop" : :  "x" (  product ) : "memory") ;
    asm volatile("" : "=l"(low) : "x"(product));
    angle = low;
    asm volatile("" : "=r"(angle) : "0"(angle));
    word = (s32)angle >> 12;
    angle = (u32)word + value;
    asm volatile("" : "=r"(angle) : "0"(angle));
    product = (unsigned long long)(u32)sy * (u32)cz;
    asm volatile("" : : "x"(product) : "memory");
    matrix->m[2][1] = angle;
    asm volatile("nop" : :  "x" (  product ) : "memory") ;
    asm volatile("" : "=l"(low) : "x"(product));
    value = low;
    asm volatile("" : "=r"(value) : "0"(value));
    word = (s32)value >> 12;
    angle = 0U - (u32)word;
    asm volatile("" : "=r"(angle) : "0"(angle));
    product = (unsigned long long)(u32)temp * (u32)sz;
    asm volatile("" : : "x"(product) : "memory");
    asm volatile("nop" : :  "x" (  product ) : "memory") ;
    asm volatile("nop" : :  "x" (  product ) : "memory") ;
    asm volatile("" : "=l"(low) : "x"(product));
    value = low;
    asm volatile("" : "=r"(value) : "0"(value));
    word = (s32)value >> 12;
    value = (u32)angle + word;
    asm volatile("" : "=r"(value) : "0"(value));
    matrix->m[2][0] = value;
    asm volatile("" : : : "memory"); return result;
}
