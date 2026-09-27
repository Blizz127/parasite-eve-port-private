/*
 * func_800727B4 — libapi file-open device hook: copy the device prefix of
 * `name` (chars >= ';', i.e. up to the ':') into D_800A32D8, look it up in
 * the kernel device table (base at 0x150, byte size at 0x154, 0x50-byte
 * entries); if absent return 0, else save its hook (+0x34) in D_800A32D0,
 * repeat the lookup to patch the hook to func_80072950, and forward to
 * func_80072A64(name, arg).
 *
 * ROM: era gcc-2.7.2-psx -O2 -G0 + MASPSX_FILL_CALL_DELAY_SLOT=1 (patch 8: the
 * `la $a1,D_800A32D8` second word in the jal slot) + MASPSX_FILL_EPILOGUE_DELAY_SLOT=1
 * (patch 3). Load-bearing spellings: `n`/`base` locals with `e < base + n`
 * (loop.c hoists the bound into $s1 via a $v1 copy, as retail), and `found`
 * set only at the loop exits (goto) so it is not held in a callee-saved reg.
 */
typedef struct {
    char *name;
    char pad4[0x30];
    void *hook;
    char pad38[0x18];
} Dev;
extern char D_800A32D8[];
extern void *D_800A32D0;
extern int func_80072A54(char *, char *);
extern int func_80072A64(char *, int);
extern void func_80072950();
int func_800727B4(char *name, int arg) {
    signed char *s = (signed char *)name;
    char *d = D_800A32D8;
    Dev *e;
    Dev *base;
    unsigned int n;
    int found;
    while (*s >= 0x3B) {
        *d++ = *s++;
    }
    *d = 0;
    n = *(unsigned int *)0x154 / sizeof(Dev);
    base = *(Dev **)0x150;
    for (e = base; e < base + n; e++) {
        if (e->name != 0 && func_80072A54(e->name, D_800A32D8) == 0) {
            D_800A32D0 = e->hook;
            found = 1;
            goto done;
        }
    }
    found = 0;
done:
    if (!found) {
        return 0;
    }
    n = *(unsigned int *)0x154 / sizeof(Dev);
    base = *(Dev **)0x150;
    for (e = base; e < base + n; e++) {
        if (e->name != 0 && func_80072A54(e->name, D_800A32D8) == 0) {
            e->hook = func_80072950;
            break;
        }
    }
    return func_80072A64(name, arg);
}
