/*
 * func_80084168 - libpad padseqd driver loop.  vram 0x80084168, size 0x37C (223 words).
 * era: ERA_CC1_VER=2.8.1  cc1 -O2 -G0 -mno-split-addresses
 *      + MASPSX_FILL_BRANCH_MACRO_SPLIT=1 (maspsx patch 10): the two absolute
 *      `sw $v0,D_8009B79C` stores sit in the blez/bgtz delay slots in retail.
 *
 * Levers: the `v = s1[0x36]; flag = (v == 0);` spelling keeps the byte in
 * $v0 and gives retail's `lbu $v0,0x36($s1)` / `sltiu $s4,$v0,1` (folding the
 * load into the compare lets cc1 load straight into $s4).  The
 * `asm ("" : "=r" (s1) : "0" (s1));` at loop3 is a no-code barrier that stops
 * cc1 stealing the loop body's first `move $a0,$s1` into the bgtz delay slot.
 */
extern void (*D_8009B730)(int);
extern void (*D_8009B744)(unsigned char *);
extern void (*D_8009B748)(unsigned char *);
extern int (*D_8009B72C)(unsigned char *, int);
extern void (*D_8009B724)(int);
extern unsigned char *D_8009B758;
extern unsigned char *D_8009B7A0;
extern int D_8009B770;
extern int D_8009B764;
extern int D_8009B79C;
extern int D_8009B77C[];
extern int func_800832B4(unsigned char *a0, int a1);
extern int func_800830DC(unsigned char *a0, int a1);
extern void func_80084FC4(int a0);
extern int func_800834E8(void);
extern void func_80083578(void);

int func_80084168(unsigned char *a0)
{
    register unsigned char *s1 asm("$17");
    register int flag asm("$20");
    register int i asm("$16");
    register int *p asm("$16");
    register int off asm("$18");
    register unsigned char *q asm("$19");
    register int three asm("$21");
    register int r asm("$2");
    int v;
    int n;
    unsigned char *rec;
    register int eight asm("$3");
    register unsigned char *arg asm("$4");
    register int r1 asm("$4");
    register int r2 asm("$3");
    register int z asm("$4");
    register int zv asm("$3");
    register int *pb asm("$3");

    s1 = a0;
    D_8009B730((int)a0);
    flag = 0;
    if (D_8009B770 != 0) {
        v = *(unsigned char *)(*(int *)(s1 + 0x3C));
        eight = 8;
        if ((v >> 4) == eight) {
            v = s1[0x36];
            flag = (v == 0);
        }
    }
    if (flag == 0) {
        goto after;
    }
    i = -1;
    off = -0xF0;
loop1:
    v = D_8009B79C - 1;
    D_8009B79C = v;
    if (v <= 0) {
        goto after;
    }
    if (i >= 0) {
        D_8009B730(*(int *)(s1 + 0xC) + off);
    }
    arg = s1;
    r = D_8009B72C(arg, 1);
    r = func_800832B4(s1, r & 0xFF);
    if (r < 0) {
        goto out;
    }
    func_80084FC4(0x3C);
    if (func_800834E8() == 0) {
        goto minus3;
    }
    i++;
    off += 0xF0;
    if (i < 4) {
        goto loop1;
    }
after:
    zv = D_8009B764;
    asm ("" : "=r" (zv) : "0" (zv));
    if (D_8009B79C < 2) {
        goto tail;
    }
    z = (zv == 0);
    asm ("" : "=r" (z) : "0" (z));
    pb = D_8009B77C;
    r = z << 2;
    p = (int *)(r + (int)pb);
    r = z << 4;
    r = r - z;
    off = r << 4;
    three = 3;
loop2:
    r1 = *p;
    if (r1 < 0) {
        goto tail;
    }
    if (r1 > 0) {
        r2 = (int)D_8009B758;
        r2 = off + r2;
        r2 = *(int *)(r2 + 0xC);
        r2 = r2 + r1 * 240;
        q = (unsigned char *)(r2 - 0xF0);
        D_8009B744(q);
    }
    r2 = *p;
    if (r2 == three) {
        goto is3;
    }
    asm ("" : "=r" (r2) : "0" (r2));
    if (r2 < 4) {
        if (r2 >= 2) {
            goto call;
        }
        if (r2 < 0) {
            goto call;
        }
        goto is01;
    }
    if (r2 != 4) {
        goto call;
    }
    *p = three;
    goto call;
is3:
    D_8009B744(q - 0xF0);
    v = 1;
    goto store;
is01:
    r = (int)D_8009B758;
    q = (unsigned char *)(r + off);
    D_8009B744(q);
    D_8009B748(q);
    v = -1;
store:
    *p = v;
call:
    arg = s1;
    r = D_8009B72C(arg, flag);
    r = func_800830DC(s1, r & 0xFF);
    if (r < 0) {
        goto out;
    }
    func_80084FC4(0x3C);
    if (func_800834E8() != 0) {
        goto cont;
    }
minus3:
    r = -3;
    goto out;
cont:
    v = D_8009B79C - 1;
    D_8009B79C = v;
    if (v >= 2) {
        goto loop2;
    }
    goto tail;
loop3:
    asm ("" : "=r" (s1) : "0" (s1));
    arg = s1;
    r = D_8009B72C(arg, flag);
    r = func_800830DC(s1, r & 0xFF);
    if (r < 0) {
        goto out;
    }
    func_80084FC4(0x3C);
    if (func_800834E8() == 0) {
        goto minus3;
    }
tail:
    v = D_8009B79C - 1;
    D_8009B79C = v;
    if (v > 0) {
        goto loop3;
    }
    func_80083578();
    r2 = *(unsigned char *)(s1 + 0x44);
    r = r2 + 1;
    *(unsigned char *)(s1 + 0x44) = r;
    r = (int)D_8009B7A0;
    arg = (unsigned char *)*(int *)(s1 + 0x3C);
    r = *(unsigned char *)r;
    r2 = r2 + (int)arg;
    *(unsigned char *)r2 = r;
    D_8009B724(0);
    r = 0;
out:
    return r;
}
