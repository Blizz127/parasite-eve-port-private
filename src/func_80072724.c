/* vendor-derived: khasinski/parasite-eve-decomp@3067919e7b82f0334b873117eef07e7157f4ed69 src/main/psyq/libapi/critical_section.c — attribution khasinski (Chris Hasinski) */
/* inline GTE/asm: this function uses vendor inline asm (hand-written instructions) — count separately from plain C. */
/* Imported under the owner's explicit authorization (2026-09-28, via Devbox Coordinator, vendor lane).
 * Upstream tree kept verbatim in vendor/khasinski-parasite-eve-decomp/ (see its PROVENANCE.md).
 * This file is that vendor TU run through the era cpp with the vendor CPP flags
 * (-undef -P, -I include, -I tools/m2c), `ExitCriticalSection` renamed to `func_80072724`, and vendor symbol
 * names mapped (post-cpp, token-wise) to this repo's address names from the vendor sym tables. */
/* Single-function extract: every other function definition of the vendor TU is reduced to a
 * prototype so this file compiles only `func_80072724` (the TU spans several of this repo's yaml functions). */
int func_80071A74(const char *func_80072784, ...);
int func_80073C5C(const char *text);
int func_80072734(char *path, int mode);
int func_80072774(int descriptor);
int func_800727A4(char *path);
int func_80072714(void);
void func_80072724(void);
int func_80074394(void);
void func_80074384(void);
void func_800743A4(void *entry);
void func_80073C74(long enabled);
int func_80073C84(int counter, int enabled);
void func_80071A24(void *destination, int length);
int func_80072714(void);
void func_80072724(void) {
    register int service asm("$4") = 2;
    register int result asm("$2");
    __asm__ volatile("syscall" : "=r"( result ), "=r"(  service ) : "1"(  service ) : "memory") ;
}
