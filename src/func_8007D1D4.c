/* VRAM 0x8007D1D4 / file 0x6D9D4 / size 0x280.
 *
 * libspu _spu_init(mode): DPCR enable, SPU state reset, status poll with a
 * 0xF00 timeout, transfer-mode words, voice/reverb/key clears, and (mode 0)
 * the reverb/voice init with key-off waits. cc1 2.7.2 -O2 -G0
 * -fno-strength-reduce + maspsx patch 8 (printf `la` into the jal slot),
 * patch 3 (epilogue fill) and patch 21 (load-delay nop before the voice-loop
 * head, 0x8007D37C). Profile era_o2_g0_spu_init_8007d1d4.
 * Levers: SPU register block through the pointer global D_8009B3FC; the
 * main-volume, status and mode-0 block (fm/noise/cdvol/exvol) fields are
 * volatile (orders the poll after the volume stores and keeps the stores
 * before the func_8007D454 call so `li a1,16` fills its slot), spucnt is
 * not (its first store fills the first jal slot) and its last store goes
 * through a volatile cast; pointer walk for the 10-short clear; j=0 before
 * p= so reorg steals `move a0,zero` into the timeout `j` slot. 2.8.1: 62.
 */
typedef struct {
    unsigned short volL, volR;     /* 0x00 */
    unsigned short pitch;          /* 0x04 */
    unsigned short addr;           /* 0x06 */
    unsigned short adsr1, adsr2;   /* 0x08 */
    unsigned short adsrvol;        /* 0x0C */
    unsigned short loop;           /* 0x0E */
} SpuVoice;

typedef struct {
    SpuVoice voice[24];            /* 0x000 */
    volatile unsigned short mvolL, mvolR; /* 0x180 */
    unsigned short rvolL, rvolR;   /* 0x184 */
    unsigned short keyon0, keyon1; /* 0x188 */
    unsigned short keyoff0, keyoff1; /* 0x18C */
    volatile unsigned short fm0, fm1;       /* 0x190 */
    volatile unsigned short noise0, noise1; /* 0x194 */
    unsigned short rvon0, rvon1;   /* 0x198 */
    unsigned short endx0, endx1;   /* 0x19C */
    unsigned short pad1A0;         /* 0x1A0 */
    unsigned short rvaddr;         /* 0x1A2 */
    unsigned short irqaddr;        /* 0x1A4 */
    unsigned short trans_addr;     /* 0x1A6 */
    unsigned short data;           /* 0x1A8 */
    unsigned short spucnt;         /* 0x1AA */
    unsigned short trans_mode;     /* 0x1AC */
    volatile unsigned short spustat; /* 0x1AE */
    volatile unsigned short cdvolL, cdvolR; /* 0x1B0 */
    volatile unsigned short exvolL, exvolR; /* 0x1B4 */
} SpuRegs;

extern SpuRegs *D_8009B3FC;
extern unsigned int *D_8009B40C;
extern int D_8009B418;
extern int D_8009B41C;
extern short D_8009B414;
extern int D_8009B420;
extern int D_8009B424;
extern int D_8009B428;
extern int D_8009B42C;
extern int D_8009B430;
extern int D_8009B434;
extern int D_8009B438;
extern unsigned char D_8009B43C[];
extern short D_800B6900[];
extern char D_80011C4C[];
extern char D_80011C5C[];
extern void func_8007DCAC(void);
extern int func_80071A74(char *, ...);
extern void func_8007D454(unsigned char *, int);

int func_8007D1D4(int mode) {
    unsigned int i;
    int j;
    SpuVoice *v;
    short *p;

    *D_8009B40C |= 0xB0000;
    D_8009B418 = 0;
    D_8009B41C = 0;
    D_8009B414 = 0;
    D_8009B3FC->mvolL = 0;
    D_8009B3FC->mvolR = 0;
    D_8009B3FC->spucnt = 0;
    func_8007DCAC();
    D_8009B3FC->mvolL = 0;
    D_8009B3FC->mvolR = 0;
    i = 0;
    while (D_8009B3FC->spustat & 0x7FF) {
        if (++i > 0xF00) {
            func_80071A74(D_80011C4C, D_80011C5C);
            break;
        }
    }
    D_8009B420 = 2;
    D_8009B424 = 3;
    D_8009B428 = 8;
    D_8009B42C = 7;
    D_8009B3FC->trans_mode = 4;
    D_8009B3FC->rvolL = 0;
    D_8009B3FC->rvolR = 0;
    D_8009B3FC->keyoff0 = 0xFFFF;
    D_8009B3FC->keyoff1 = 0xFFFF;
    D_8009B3FC->rvon0 = 0;
    D_8009B3FC->rvon1 = 0;
    for (j = 0, p = D_800B6900; j < 10; j++) {
        *p++ = 0;
    }
    if (mode == 0) {
        D_8009B414 = 0x200;
        D_8009B3FC->fm0 = 0;
        D_8009B3FC->fm1 = 0;
        D_8009B3FC->noise0 = 0;
        D_8009B3FC->noise1 = 0;
        D_8009B3FC->cdvolL = 0;
        D_8009B3FC->cdvolR = 0;
        D_8009B3FC->exvolL = 0;
        D_8009B3FC->exvolR = 0;
        func_8007D454(D_8009B43C, 16);
        v = D_8009B3FC->voice;
        for (j = 0; j < 24; j++) {
            v->volL = 0;
            v->volR = 0;
            v->pitch = 0x3FFF;
            v->addr = 0x200;
            v->adsr1 = 0;
            v->adsr2 = 0;
            v++;
        }
        D_8009B3FC->keyon0 = 0xFFFF;
        D_8009B3FC->keyon1 = 0xFF;
        func_8007DCAC();
        func_8007DCAC();
        func_8007DCAC();
        func_8007DCAC();
        D_8009B3FC->keyoff0 = 0xFFFF;
        D_8009B3FC->keyoff1 = 0xFF;
        func_8007DCAC();
        func_8007DCAC();
        func_8007DCAC();
        func_8007DCAC();
    }
    D_8009B430 = 1;
    *(volatile unsigned short *)&D_8009B3FC->spucnt = 0xC000;
    D_8009B434 = 0;
    D_8009B438 = 0;
    return 0;
}
