/*
 * func_8007C564 -- CD streaming sector-ready callback (libpress-style): reads
 * the CD result bytes, validates the 0x160 STR sector header in the ring slot
 * D_800C0DC8 + D_800BE998 * 32, handles frame/sector mismatches (status codes
 * 1..10 in D_8009B374), DMAs the payload via func_8007CE80 / func_8007CEAC and
 * advances the ring.  Stock cc1 2.7.2 -O2 -G0 -fno-strength-reduce (the
 * header byte loop and the 8-word header copy keep their index counters in
 * retail), maspsx: FILL_CALL_DELAY_SLOT, BEQZ_SYM_STORE_DELAY(+_ANY),
 * LOAD_DELAY_NOP_BEFORE_LOOP_LABEL.  Drafted and closed in lane exeF.
 * Levers: `asm("")` at the head of the post-check block stops reorg stealing
 * the `lui` of 0x20943 into the preceding beqz slot; `i = 0` hoisted above
 * the `if (D_800A8020 == 0)` so reorg fills that bnez slot from before; the
 * B33C pointer read into a local ahead of the 4-byte header copy; direct
 * D_800A34A0[k] reads (not a local) in the two late tests.
 */
typedef struct { unsigned char b[4]; } Hdr4;
typedef struct { unsigned char pad[0x1C]; Hdr4 loc; } SecHdr;

extern int D_800B89F4;
extern int D_800A801C;
extern volatile unsigned int *D_8009B34C;
extern int D_800C0DB8;
extern int D_800B0CD0;
extern int D_800BCD7C;
extern int D_8009B374;
extern int D_800BE998;
extern unsigned char *D_800C0DC8;
extern unsigned short *D_800A34A0;
extern unsigned char *D_8009B32C;
extern unsigned char *D_8009B338;
extern unsigned int *D_8009B33C;
extern unsigned int *D_8009B340;
extern volatile unsigned char *D_8009B334;
extern int D_800A8020;
extern volatile unsigned int *D_8009B35C;
extern int D_800C0DC0;
extern int D_800B6918;
extern int D_800B8620;
extern short D_800A8018;
extern int D_800A5D54;
extern int D_800BE9E4;
extern unsigned int D_800C0DBC;
extern void (*D_800B0CCC)(void);
extern unsigned int D_800C20C4;
extern unsigned char *D_800C0DC4;
extern int D_800B6914;

extern int func_8007A488(int, unsigned char *);
extern void func_8007CE80(void *, int, int, int);
extern void func_8007CEAC(int, void *, int, int, int, int, int);
extern void func_8007C444(int, int);
extern void func_8007C214(void);

void func_8007C564(void)
{
    volatile unsigned short st[3];
    Hdr4 hdr;
    unsigned char res[8];
    unsigned short *p;
    unsigned int i;
    int mode;

    if (D_800B89F4 == 1) {
        return;
    }
    if (D_800A801C != 0 && (*D_8009B34C & 0x1000000)) {
        D_800B0CD0 = 1;
        if (D_800C0DB8 != 0) {
            D_800BCD7C++;
        }
        D_8009B374 = 1;
        return;
    }
    if (func_8007A488(1, res) == 5) {
        return;
    }
    st[1] = res[0];
    st[2] = res[1];
    if (st[1] & 4) {
        D_8009B374 = 3;
        return;
    }
    D_800A34A0 = (unsigned short *)(D_800C0DC8 + D_800BE998 * 32);
    if (D_800A34A0[0] != 0) {
        if (D_800C0DB8 != 0) {
            D_800BCD7C++;
        }
        D_8009B374 = 4;
        return;
    }
    asm("");
    *D_8009B32C = 0;
    *D_8009B338 = 0;
    *D_8009B32C = 0;
    *D_8009B338 = 0x80;
    *D_8009B33C = 0x20943;
    *D_8009B340 = 0x1323;
    i = 0;
    if (D_800A8020 == 0) {
        for (; i < 4; i++) {
            hdr.b[i] = *D_8009B334;
        }
        for (i = 0; i < 8; i++) {
            *D_8009B334;
        }
    }
    mode = 0x11000000;
    if (D_800C0DB8 != 0) {
        func_8007CE80((void *)D_800A34A0, D_800C0DB8 + (D_800BCD7C << 11), 8, 0);
    } else {
        func_8007CEAC(3, (void *)D_800A34A0, 0, 8, mode, 0, 0);
    }
    while (*D_8009B35C & 0x1000000) {
    }
    { unsigned int *q = D_8009B33C;
    ((SecHdr *)D_800A34A0)->loc = hdr;
    *q = 0x20843; }
    *D_8009B340 = 0x1325;
    if (D_800C0DC0 == 1 && D_800B6918 != 0) {
        if (D_800B6918 != D_800A34A0[4]) {
            D_800A34A0[0] = 0;
            if (D_800C0DB8 != 0) {
                D_800BCD7C++;
            }
            return;
        }
        D_800C0DC0 = 0;
    }
    p = D_800A34A0;
    if (p[0] != 0x160 || ((p[1] >> 10) & 0x1F) != D_800B8620) {
        if (D_800C0DB8 != 0) {
            D_800BCD7C = 0;
        } else {
            *(volatile unsigned short *)p;
        }
        D_8009B374 = 5;
        D_800A34A0[0] = 0;
        return;
    }
    if (D_800A8018 != p[2] || (D_800A5D54 != 0 && D_800A5D54 != p[4])) {
        D_800A5D54 = 0;
        D_800A8018 = 0;
        func_8007C444(D_800BE9E4, D_800BE998 - D_800BE9E4);
        D_800BE998 = D_800BE9E4;
        D_800A34A0[0] = 0;
        if (D_800C0DB8 != 0) {
            D_800BCD7C++;
        }
        D_8009B374 = 6;
        return;
    }
    if (D_800A34A0[2] == 0) {
        unsigned short n = D_800A34A0[4];
        D_800A8018 = 0;
        D_800A5D54 = n;
        if (D_800C0DBC != 0 && n >= D_800C0DBC) {
            D_800A5D54 = 0;
            D_800A8018 = 0;
            func_8007C444(D_800BE9E4, D_800BE998 - D_800BE9E4);
            D_800BE998 = D_800BE9E4;
            D_800A34A0[0] = 0;
            D_800C0DC0 = 1;
            if (D_800B0CCC != 0) {
                D_800B0CCC();
            }
            if (D_800C0DB8 != 0) {
                D_800BCD7C++;
            }
            D_8009B374 = 7;
            return;
        }
        p = D_800A34A0;
        if (D_800C20C4 - D_800BE998 - 1 < p[3]) {
            if (D_800C0DBC == 0) {
                p[0] = 1;
                D_800C0DC0 = 1;
                if (D_800B0CCC != 0) {
                    D_800B0CCC();
                }
                if (D_800C0DB8 != 0) {
                    D_800BCD7C++;
                }
                D_8009B374 = 8;
                return;
            }
            if (*(short *)D_800C0DC8 != 0) {
                p[0] = 0;
                if (D_800C0DB8 != 0) {
                    D_800BCD7C++;
                }
                D_8009B374 = 9;
                return;
            }
            p[0] = 1;
            {
                unsigned int *d = (unsigned int *)D_800C0DC8;
                volatile unsigned int *s = (volatile unsigned int *)D_800A34A0;
                D_800BE998 = 0;
                for (i = 0; i < 8; i++) {
                    *d++ = *s++;
                }
            }
            D_800A34A0 = (unsigned short *)D_800C0DC8;
        }
        D_800BE9E4 = D_800BE998;
    }
    D_8009B374 = 10;
    D_800A8018++;
    D_800C0DC4 = D_800C0DC8 + D_800C20C4 * 32 + D_800BE998 * 2016;
    if (D_800A801C != 0) {
        mode = 0x11000000;
        *D_8009B33C = 0x20943;
        *D_8009B340 = 0x1323;
    } else {
        *D_8009B33C = 0x21020843;
        mode = 0x11400100;
    }
    if (D_800A34A0[3] - 1 == D_800A34A0[2]) {
        D_800B89F4 = 1;
        if (D_800C0DB8 != 0) {
            func_8007CE80(D_800C0DC4, D_800C0DB8 + (D_800BCD7C << 11) + 0x20, 0x1F8, 1);
            D_800BCD7C++;
        } else {
            func_8007CEAC(3, D_800C0DC4, 0, 0x1F8, mode, 1, 0);
        }
        D_800A8018 = 0;
        D_800A5D54 = 0;
        D_800B8620 = D_800B6914;
    } else {
        if (D_800C0DB8 != 0) {
            func_8007CE80(D_800C0DC4, D_800C0DB8 + (D_800BCD7C << 11) + 0x20, 0x1F8, 0);
            D_800BCD7C++;
        } else {
            func_8007CEAC(3, D_800C0DC4, 0, 0x1F8, mode, 0, 0);
        }
    }
    *D_8009B340 = 0x1325;
    D_800A34A0[0] = 3;
    D_800BE998++;
    if (D_800C0DB8 != 0 && D_800B89F4 != 0) {
        func_8007C214();
    }
}
