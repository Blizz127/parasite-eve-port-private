extern char D_80011C08[];
extern char D_80011C14[];
extern char D_8009B298[];
extern unsigned char D_8009AFD4;
extern unsigned char D_8009AFD5;
extern int D_8009AFB4;
extern int D_8009AFB8;
extern int D_8009AFC4;
extern int D_8009AFC8;
extern volatile unsigned char *D_8009B27C;
extern volatile unsigned char *D_8009B284;
extern volatile unsigned char *D_8009B288;
extern unsigned int *D_8009B28C;
extern unsigned char D_8009B294;
extern void func_80073C5C(char *a0);
extern void func_80071A74(char *a0, ...);
extern void func_80073C94(void);
extern void func_80073CC4(int a0, void (*a1)());
extern void func_8007C13C();
extern int func_8007B558(int a0, int a1, int a2, int a3);
extern int func_8007B010(int a0, int a1);

int func_8007BBFC(void)
{
    volatile unsigned char *p;
    unsigned char t;
    volatile unsigned char *r;
    volatile unsigned char *s;
    register int z0 asm("$4");
    register int z1 asm("$5");
    register int z2 asm("$6");
    register int z3 asm("$7");
    int rv;

    func_80073C5C(D_80011C08);
    func_80071A74(D_80011C14, D_8009B298);
    D_8009AFD5 = 0;
    D_8009AFD4 = 0;
    D_8009AFB8 = 0;
    D_8009AFB4 = 0;
    D_8009AFC8 = 0;
    D_8009AFC4 = 0;
    func_80073C94();
    func_80073CC4(2, func_8007C13C);
    *D_8009B27C = 1;
    while (*D_8009B288 & 7) {
        *D_8009B27C = 1;
        *D_8009B288 = 7;
        *D_8009B284 = 7;
    }
    z0 = 1;
    asm volatile("" : "=r"(z0) : "0"(z0));
    z1 = 0;
    asm volatile("" : "=r"(z1) : "0"(z1));
    p = &D_8009B294;
    p[2] = 0;
    t = p[2];
    asm volatile("" : "=r"(p) : "0"(p));
    z2 = 0;
    asm volatile("" : "=r"(z2) : "0"(z2));
    p[1] = t;
    r = D_8009B27C;
    p[0] = 2;
    *r = 0;
    s = D_8009B288;
    z3 = 0;
    *s = 0;
    *D_8009B28C = 0x1325;
    func_8007B558(z0, z1, z2, z3);
    if (D_8009AFC4 & 0x10) {
        z0 = 1;
        asm volatile("" : "=r"(z0) : "0"(z0));
        z1 = 0;
        asm volatile("" : "=r"(z1) : "0"(z1));
        z2 = 0;
        asm volatile("" : "=r"(z2) : "0"(z2));
        z3 = 0;
        func_8007B558(z0, z1, z2, z3);
    }
    z0 = 10;
    asm volatile("" : "=r"(z0) : "0"(z0));
    z1 = 0;
    asm volatile("" : "=r"(z1) : "0"(z1));
    z2 = 0;
    asm volatile("" : "=r"(z2) : "0"(z2));
    z3 = 0;
    if (func_8007B558(z0, z1, z2, z3) != 0) {
        return -1;
    }
    z0 = 12;
    asm volatile("" : "=r"(z0) : "0"(z0));
    z1 = 0;
    asm volatile("" : "=r"(z1) : "0"(z1));
    z2 = 0;
    asm volatile("" : "=r"(z2) : "0"(z2));
    z3 = 0;
    if (func_8007B558(z0, z1, z2, z3) != 0) {
        return -1;
    }
    z0 = 0;
    asm volatile("" : "=r"(z0) : "0"(z0));
    z1 = 0;
    rv = func_8007B010(z0, z1);
    if (rv != 2) {
        return -1;
    }
    return 0;
}
