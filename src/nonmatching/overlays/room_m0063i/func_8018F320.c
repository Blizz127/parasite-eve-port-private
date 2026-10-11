/* NON_MATCHING: functionally equivalent to retail room_m0063i 0x338..0xD2C
 * (func_8018F320).  Written from the retail asm, not byte-matched; native port
 * differential-tested by tools/analysis/e21_diff.py.
 * Actor state callback (installed at o+0xC, run by func_8018F250) of the
 * Day 2 Central Park leaping enemy (m0063i and its twins, e.g. m0083i).
 * First call: start the leap — aim from the model's rotated offset toward the
 * target (o+0x4C), set the speed from the distance and build the flight
 * rotation at o+0x1C.  Every call: integrate the arc (Y-rotation by the
 * heading at o+0x90, gravity on o+0x80), land when the vertical speed turns
 * downward near the ground (then install func_8018FD14), else place the
 * actor's matrix/position from the flight frame and flag a hit on the player
 * when within 200 units.  Scratchpad 0x1F800000 is the GTE work area, as in
 * retail. */
extern int D_800966EC[];
extern unsigned char *D_8009D254;
extern int func_800DFF80();
extern int func_800DFC80();
extern int func_800DFC44();
extern int func_800DFE20();
extern void func_800DFE94();
extern void func_80079754();

#define SPAD ((unsigned char *)0x1F800000)
#define SH(o) (*(short *)(SPAD + (o)))
#define SW(o) (*(int *)(SPAD + (o)))
#define B(p, o) (*(unsigned char *)((unsigned char *)(p) + (o)))
#define H(p, o) (*(short *)((unsigned char *)(p) + (o)))
#define UH(p, o) (*(unsigned short *)((unsigned char *)(p) + (o)))
#define W(p, o) (*(int *)((unsigned char *)(p) + (o)))
#define P(p, o) (*(unsigned char **)((unsigned char *)(p) + (o)))

/* Rotation about Y into a scratchpad MATRIX at byte offset base. */
#define ROTY(base, ang)                                     \
    do {                                                    \
        short *t_ = (short *)&D_800966EC[(ang) & 0xFFF];    \
        SH((base) + 2) = 0;                                 \
        SH((base) + 0) = t_[1];                             \
        SH((base) + 6) = 0;                                 \
        SH((base) + 8) = 4096;                              \
        SH((base) + 10) = 0;                                \
        SH((base) + 14) = 0;                                \
        SH((base) + 16) = t_[1];                            \
        SH((base) + 4) = t_[0];                             \
        SH((base) + 12) = -t_[0];                           \
    } while (0)

#define SETRT(m) asm volatile("lw $12,0(%0)\n\t" "lw $13,4(%0)\n\t" "ctc2 $12,$0\n\t" "ctc2 $13,$1\n\t" "lw $12,8(%0)\n\t" "lw $13,12(%0)\n\t" "lw $14,16(%0)\n\t" "ctc2 $12,$2\n\t" "ctc2 $13,$3\n\t" "ctc2 $14,$4" : : "r"(m) : "$12", "$13", "$14")
#define LDV0(v) asm volatile("lwc2 $0,0(%0)\n\t" "lwc2 $1,4(%0)" : : "r"(v))
#define RTV0() asm volatile("nop\n\t" "nop\n\t" ".word 0x4A486012")
#define STMAC(v) asm volatile("swc2 $25,0(%0)\n\t" "swc2 $26,4(%0)\n\t" "swc2 $27,8(%0)" : : "r"(v) : "memory")
#define MULCOL(src, dst)                                                                                       \
    do {                                                                                                       \
        asm volatile("lhu $12,0(%0)\n\t" "lhu $13,6(%0)\n\t" "lhu $14,12(%0)\n\t" "mtc2 $12,$9\n\t" "mtc2 $13,$10\n\t" "mtc2 $14,$11" : : "r"(src) : "$12", "$13", "$14"); \
        asm volatile("nop\n\t" "nop\n\t" ".word 0x4A49E012");                                                  \
        asm volatile("mfc2 $12,$9\n\t" "mfc2 $13,$10\n\t" "mfc2 $14,$11\n\t" "sh $12,0(%0)\n\t" "sh $13,6(%0)\n\t" "sh $14,12(%0)" : : "r"(dst) : "$12", "$13", "$14", "memory"); \
    } while (0)

void func_8018F320(unsigned char *o)
{
    unsigned char *q;
    unsigned char *act;
    unsigned char *mdl;
    unsigned char *p;
    unsigned char *sp;
    int d;
    int a;
    int lo;

    q = o + 12;
    act = P(o, 8);
    mdl = P(act, 396);
    sp = SPAD;
    if (B(o, 26) == 0) {
        B(o, 26) = 1;
        W(P(o, 8), 152) |= 0x10000;
        UH(P(o, 8), 592) |= 0x400;
        W(o, 4) = 0;
        W(P(mdl, 0), 0) |= 0x40000000;
        SH(112) = 128;
        SH(114) = 0;
        SH(116) = -350;
        ROTY(0, UH(mdl, 58));
        SETRT(sp);
        LDV0(sp + 112);
        RTV0();
        STMAC(sp + 64);
        W(o, 92) = W(mdl, 40) + (SW(64) << 16);
        W(o, 96) = W(mdl, 44) + (SW(68) << 16);
        W(o, 100) = W(mdl, 48) + (SW(72) << 16);
        if (P(o, 124) != 0) {
            W(o, 76) = W(P(o, 124), 40);
            W(o, 80) = W(P(o, 124), 44);
            W(o, 84) = W(P(o, 124), 48);
        }
        a = func_800DFF80(o + 92, o + 76);
        ROTY(0, a);
        SETRT(sp);
        LDV0(o + 108);
        RTV0();
        STMAC(sp + 64);
        W(o, 76) = W(o, 76) + (SW(64) << 16);
        W(o, 80) = W(o, 80) + (SW(68) << 16);
        W(o, 84) = W(o, 84) + (SW(72) << 16);
        d = func_800DFC80(o + 92, o + 76) + ((W(o, 132) * 112) >> 21);
        if (d < 328)
            d = 328;
        d = (0x05160000 / d) << 8;
        W(o, 140) = d;
        W(o, 144) = d >> 3;
        func_800DFE94(o + 92, o + 76, o + 116);
        func_80079754(o + 116, o + 28);
    }
    SH(144) = 0;
    ROTY(0, ((W(q, 132) + 0x8000) >> 16) - 1024);
    SH(112) = 0;
    SH(114) = 0;
    SH(116) = W(q, 116) >> 12;
    SETRT(sp);
    LDV0(sp + 112);
    RTV0();
    STMAC(sp + 96);
    W(q, 48) = W(q, 48) + (SW(96) << 12);
    W(q, 56) = W(q, 56) + (SW(104) << 12);
    if (SW(104) >= 0) {
        a = W(q, 116) + W(q, 120);
        lo = ((a + 0x8000) >> 16) * (W(q, 128) >> 12);
        W(q, 116) = a;
    } else {
        lo = ((W(q, 116) + 0x8000) >> 16) * (W(q, 128) >> 12);
        W(q, 116) = W(q, 116) - W(q, 120);
    }
    W(q, 132) = W(q, 132) + ((lo >> 8) << 12);
    SH(130) = 0;
    SH(128) = (((W(q, 48) + 0x8000) >> 16) * W(q, 124) + 2048) >> 12;
    SH(132) = (W(q, 56) + 0x8000) >> 16;
    if (SW(104) < 0) {
        if (func_800DFC44(H(q, 50) * H(q, 50) + H(q, 58) * H(q, 58), 0x8000) < 256)
            SH(144) = 1;
    }
    if (UH(SPAD, 144) != 0) {
        W(mdl, 20) = 0x380000;
        W(P(o, 8), 152) &= 0xFFFEFFFF;
        UH(P(o, 8), 592) &= 0xFBFF;
        W(q, 0) = 0x8018FD14;   /* func_8018FD14, the landing state */
        return;
    }
    SH(112) = 0;
    SH(116) = -1024;
    SH(114) = W(o, 4) * 640 + 1600;
    func_80079754(sp + 112, sp);
    ROTY(32, UH(SPAD, 114));
    SH(112) = 256;
    SH(114) = 0;
    SH(116) = 0;
    SETRT(sp + 32);
    LDV0(sp + 112);
    RTV0();
    STMAC(sp + 64);
    SH(138) = -200;
    SH(136) = UH(SPAD, 128) + SW(64);
    SH(140) = UH(SPAD, 132) + SW(72);
    SETRT(q + 16);
    LDV0(sp + 128);
    RTV0();
    STMAC(sp + 64);
    LDV0(sp + 136);
    RTV0();
    STMAC(sp + 80);
    SETRT(q + 16);
    MULCOL(sp + 0, sp + 32);
    MULCOL(sp + 2, sp + 34);
    MULCOL(sp + 4, sp + 36);
    W(act, 40) = W(q, 80) + (SW(64) << 16);
    W(act, 44) = W(q, 84) + (SW(68) << 16);
    W(act, 48) = W(q, 88) + (SW(72) << 16);
    W(act, 508) = H(q, 82) + SW(80);
    W(act, 512) = H(q, 86) + SW(84);
    W(act, 516) = H(q, 90) + SW(88);
    UH(act, 488) = UH(SPAD, 32);
    UH(act, 490) = UH(SPAD, 34);
    UH(act, 492) = UH(SPAD, 36);
    UH(act, 494) = UH(SPAD, 38);
    UH(act, 496) = UH(SPAD, 40);
    UH(act, 498) = UH(SPAD, 42);
    UH(act, 500) = UH(SPAD, 44);
    UH(act, 502) = UH(SPAD, 46);
    UH(act, 504) = UH(SPAD, 48);
    if (H(q, 136) == 0) {
        if (func_800DFE20(act + 40, D_8009D254 + 40) < 200) {
            H(q, 136) = 1;
            p = P(D_8009D254, 0);
            W(p, 76) |= 0x4000;
            p = P(mdl, 0);
            if (p != 0)
                W(p, 0) |= 0x80000000;
        }
    }
    if (UH(mdl, 26) >= 46)
        W(mdl, 20) = 0x180000;
}
