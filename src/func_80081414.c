/*
 * func_80081414 - libds DsSearchFile.  vram 0x80081414, size 0x2E0 (184 words).
 * era: ERA_CC1_VER=2.8.1  cc1 -O2 -G0 -mno-split-addresses -fno-schedule-insns
 *      MASPSX_SYMBOL_LOAD_DEST_TEMP=1 MASPSX_FILL_CALL_DELAY_SLOT=1   -> LINK_EXACT
 *
 *      MASPSX_LOAD_DELAY_NOP_BEFORE_LABEL=1 (maspsx patch 14): retail has the
 *      load-delay nop BEFORE the `$L8:` join label (label opens a cc1
 *      `.set noreorder` block); stock maspsx puts it after the label, which
 *      moves the label one word (1 mismatched branch offset without it).
 */
typedef struct { int w0, w1, w2, w3, w4, w5; } Rec;

extern int D_8009B6E0;
extern int D_8009AFC0;
extern unsigned char D_800A36B8[];
extern char D_80011E6C[];
extern char D_80011E88[];
extern char D_80011EA0[];
extern char D_80011EBC[];
extern char D_80011EDC[];
extern char D_80011EE8[];
extern int func_8007F7A8(void);
extern int func_80081714(void);
extern int func_800819D8(int a0, signed char *a1);
extern int func_80081A7C(void);
extern int func_800816F4(unsigned char *a0, signed char *a1);
extern void func_80071A74();
extern void func_80073C5C(char *a0);

unsigned char *func_80081414(unsigned char *dst, signed char *path)
{
    signed char buf[0x20];
    register unsigned char *dstp asm("$22");
    register signed char *p asm("$19");
    register unsigned char *key asm("$19");
    register signed char *src asm("$16");
    register unsigned char *usrc asm("$16");
    register unsigned char *rec asm("$16");
    register signed char *w asm("$17");
    register int off asm("$17");
    register int n asm("$18");
    register int minus1 asm("$20");
    register int slash asm("$21");
    register int slash2 asm("$5");
    register int t asm("$4");
    register unsigned char *base asm("$2");
    register int q0 asm("$2");
    register int q1 asm("$3");
    register int q2 asm("$4");
    register int q3 asm("$5");
    register int c asm("$2");
    int uc;

    dstp = dst;
    p = path;
    if (D_8009B6E0 < func_8007F7A8()) {
        if (func_80081714() == 0) {
            return 0;
        }
        D_8009B6E0 = func_8007F7A8();
    }
    if (*p != 0x5C) {
        return 0;
    }
    buf[0] = 0;
    t = 1;
    src = p;
    n = 0;
    slash = 0x5C;
    minus1 = -1;
    do {
        c = *src;
        usrc = (unsigned char *)src;
        asm ("" : "=r" (usrc) : "0" (usrc));
        uc = *usrc;
        w = buf;
        if (c != slash) {
            slash2 = 0x5C;
            asm ("" : "=r" (slash2) : "0" (slash2));
        inner:
            if (uc == 0) {
                goto endloop;
            }
            src++;
            *w = uc;
            c = *src;
            usrc = (unsigned char *)src;
            asm ("" : "=r" (usrc) : "0" (usrc));
            uc = *usrc;
            w++;
            if (c != slash2) {
                goto inner;
            }
            asm ("" : "=r" (src) : "0" (src));
            c = *src;
        }
        if (c == 0) {
            goto endloop;
        }
        src++;
        *w = 0;
        t = func_800819D8(t, buf);
        if (t == minus1) {
            buf[0] = 0;
            break;
        }
        n++;
    } while (n < 8);
endloop:
    if (n >= 8) {
        if (D_8009AFC0 <= 0) {
            return 0;
        }
        func_80071A74(D_80011E6C, p, n);
        return 0;
    }
    if (buf[0] == 0) {
        if (D_8009AFC0 <= 0) {
            return 0;
        }
        func_80071A74(D_80011E88, p);
        return 0;
    }
    *w = 0;
    if (func_80081A7C() == 0) {
        if (D_8009AFC0 <= 0) {
            return 0;
        }
        func_80073C5C(D_80011EA0);
        return 0;
    }
    if (D_8009AFC0 >= 2) {
        func_80071A74(D_80011EBC, buf);
    }
    n = 0;
    asm ("" : "=r" (n) : "0" (n));
    base = D_800A36B8;
    rec = base - 8;
    key = base;
    off = 0;
    asm ("" : "=r" (off) : "0" (off));
tloop:
    if (*(signed char *)(D_800A36B8 + off) == 0) {
        goto notfound;
    }
    if (func_800816F4(key, buf) != 0) {
        if (D_8009AFC0 >= 2) {
            func_80071A74(D_80011EDC, buf);
        }
        q0 = *(int *)(rec + 0);
        q1 = *(int *)(rec + 4);
        q2 = *(int *)(rec + 8);
        q3 = *(int *)(rec + 0xC);
        *(int *)(dstp + 0) = q0;
        *(int *)(dstp + 4) = q1;
        *(int *)(dstp + 8) = q2;
        *(int *)(dstp + 0xC) = q3;
        q0 = *(int *)(rec + 0x10);
        q1 = *(int *)(rec + 0x14);
        *(int *)(dstp + 0x10) = q0;
        *(int *)(dstp + 0x14) = q1;
        asm ("" : : : "memory");
        return rec;
    }
    asm ("" : "=r" (key) : "0" (key));
    rec += 0x18;
    key += 0x18;
    n++;
    off += 0x18;
    if (n < 0x40) {
        goto tloop;
    }
notfound:
    if (D_8009AFC0 <= 0) {
        return 0;
    }
    func_80071A74(D_80011EE8, buf);
    return 0;
}
