extern int func_80073A44(int);
extern void func_80073C5C(char *);
extern void func_80071A74(char *, unsigned int, unsigned int, unsigned int, unsigned int);
extern void func_8007B9EC(void);
extern char D_80011B18[];
extern char D_80011B28[];
extern char D_80011C20[];
extern int D_800A3478;
extern int D_800A347C;
extern unsigned int D_800A3480;
extern unsigned char D_8009B294[];
extern unsigned int D_8009B05C[];
extern unsigned int D_8009AFDC[];
extern unsigned char D_8009AFD5;
extern volatile unsigned int *D_8009B2C0;

int func_8007BDDC(int a0)
{
    register int flag asm("$18");
    register int cap asm("$20");
    register unsigned char *bytes asm("$17");
    register unsigned int *table asm("$16");
    register unsigned int *msgs asm("$19");
    register int oldc asm("$3");
    register unsigned int b0 asm("$4");
    int loaded;
    int now;
    int rc;
    unsigned int b1;
    unsigned int slot;
    unsigned int msg;
    unsigned int word;

    flag = a0;
    now = func_80073A44(-1);
    cap = 0x3C0000;
    msgs = D_8009AFDC;
    bytes = D_8009B294;
    table = D_8009B05C;
    D_800A3478 = now + 0x3C0;
    D_800A347C = 0;
    D_800A3480 = (unsigned int)D_80011C20;
    for (;;) {
        now = func_80073A44(-1);
        if (D_800A3478 < now) {
            goto bad;
        }
        loaded = D_800A347C;
        oldc = loaded;
        loaded = loaded + 1;
        D_800A347C = loaded;
        oldc = cap < oldc;
        if (oldc) {
            goto bad;
        }
        rc = 0;
        goto after;
    bad:
        func_80073C5C(D_80011B18);
        b0 = bytes[0];
        b1 = bytes[1];
        slot = D_800A3480;
        asm volatile("" : "=r"(b0), "=r"(b1), "=r"(slot) : "0"(b0), "1"(b1), "2"(slot));
        b0 = b0 << 2;
        oldc = table[b1];
        loaded = D_8009AFD5;
        asm volatile("" : "=r"(b0), "=r"(oldc), "=r"(loaded) : "0"(b0), "1"(oldc), "2"(loaded));
        msg = msgs[loaded];
        word = *(unsigned int *)(b0 + (unsigned int)table);
        func_80071A74(D_80011B28, slot, msg, word, oldc);
        func_8007B9EC();
        rc = -1;
    after:
        if (rc != 0) {
            return -1;
        }
        if ((*D_8009B2C0 & 0x1000000) == 0) {
            return 0;
        }
        if (flag != 0) {
            return 1;
        }
    }
}
