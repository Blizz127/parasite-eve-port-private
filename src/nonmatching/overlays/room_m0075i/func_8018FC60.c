/* NON_MATCHING: functionally equivalent to retail room_m0075i 0xC78..0xF28
 * (func_8018FC60).  Written from the retail asm, not byte-matched; native port
 * differential-tested by tools/analysis/e21_diff.py.
 * Battle weapon/effect shadow callback (Day 2 Central Park random battles,
 * m0075i and its twins): the scale-a2[8] times rot(D_8018EFF4) matrix of
 * func_8018F814, then the effect object at a2+0x114 (its +0x14 halfword set
 * from a2[9]) drawn by func_800C4FC4 at the floor (D_800942EC) under the
 * actor's two joints.  Retail's GTE transform of the rotated matrix's
 * uninitialised translation is overwritten before use and is not reproduced. */
typedef struct { short m[3][3]; int t[3]; } MATRIX;
typedef struct { int vx, vy, vz, pad; } VECTOR;
typedef struct { short vx, vy, vz, pad; } SVECTOR;
extern unsigned char *func_800C2B50();
extern void func_800C3238();
extern void func_80071A44();
extern void func_80078CC4();
extern void func_800794C4();
extern void func_800C4FC4();
extern SVECTOR D_8018EFF4;
extern short D_800942EC;

void func_8018FC60(void *a0, void *a1, short *a2)
{
    SVECTOR ang;
    MATRIX m;
    MATRIX m2;
    VECTOR v;
    VECTOR s;
    unsigned char *x;
    unsigned char *r;
    MATRIX *pm;
    short *col;
    int c;

    x = func_800C2B50();
    ang = D_8018EFF4;
    m.m[2][2] = 4096;
    m.m[1][1] = 4096;
    m.m[0][0] = 4096;
    m.t[2] = 0;
    m.t[1] = 0;
    m.t[0] = 0;
    m.m[2][1] = 0;
    m.m[2][0] = 0;
    m.m[1][2] = 0;
    m.m[1][0] = 0;
    m.m[0][2] = 0;
    m.m[0][1] = 0;
    func_80071A44(&v, 0, 16);
    v.vx = a2[8];
    v.vy = a2[8];
    v.vz = a2[8];
    s = v;
    func_80078CC4(&m, &s);
    func_800794C4(&ang, &m2);
    pm = &m;
    asm volatile("lw $12,0(%0)\n\t" "lw $13,4(%0)\n\t" "ctc2 $12,$0\n\t" "ctc2 $13,$1\n\t" "lw $12,8(%0)\n\t" "lw $13,12(%0)\n\t" "lw $14,16(%0)\n\t" "ctc2 $12,$2\n\t" "ctc2 $13,$3\n\t" "ctc2 $14,$4" : : "r"(pm) : "$12", "$13", "$14");
    for (c = 0; c < 3; c++) {
        col = &m2.m[0][c];
        asm volatile("lhu $12,0(%0)\n\t" "lhu $13,6(%0)\n\t" "lhu $14,12(%0)\n\t" "mtc2 $12,$9\n\t" "mtc2 $13,$10\n\t" "mtc2 $14,$11" : : "r"(col) : "$12", "$13", "$14");
        asm volatile("nop\n\t" "nop\n\t" ".word 0x4A49E012");
        asm volatile("mfc2 $12,$9\n\t" "mfc2 $13,$10\n\t" "mfc2 $14,$11\n\t" "sh $12,0(%0)\n\t" "sh $13,6(%0)\n\t" "sh $14,12(%0)" : : "r"(col) : "$12", "$13", "$14", "memory");
    }
    func_800C3238(2);
    r = *(unsigned char **)(*(unsigned char **)x + 0x238);
    a2[0x94] = a2[9];
    m2.t[0] = *(int *)(r + 0x134);
    m2.t[1] = D_800942EC;
    m2.t[2] = *(int *)(r + 0x13C);
    func_800C4FC4((unsigned char *)a2 + 0x114, &m2, 1);
    m2.t[0] = *(int *)(r + 0x1B4);
    m2.t[1] = D_800942EC;
    m2.t[2] = *(int *)(r + 0x1BC);
    func_800C4FC4((unsigned char *)a2 + 0x114, &m2, 1);
}
