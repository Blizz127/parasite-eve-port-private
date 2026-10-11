/* NON_MATCHING: functionally equivalent to retail room_m0075i 0x82C..0xB5C
 * (func_8018F814).  Written from the retail asm, not byte-matched; native port
 * differential-tested by tools/analysis/e21_diff.py.
 * Battle weapon/effect draw callback (Day 2 Central Park random battles,
 * m0075i and its twins): builds a scaled identity matrix (scale a2[8]) and
 * that scale times the rotation of D_8018EFF4 (GTE MVMVA column products),
 * then draws the two billboard packets at the actor's two joints (+0x134,
 * +0x1B4) and their floor shadows at D_800942EC.  Retail also transforms the
 * (uninitialised) translation of the rotated matrix with the GTE; that result
 * is overwritten before use and is not reproduced. */
typedef struct { short m[3][3]; int t[3]; } MATRIX;
typedef struct { int vx, vy, vz, pad; } VECTOR;
typedef struct { short vx, vy, vz, pad; } SVECTOR;
typedef struct { unsigned char pad[10]; short va; } OBJ;
extern unsigned char *func_800C2B50();
extern void func_800C2EAC();
extern void func_800C2FF0();
extern void func_800C3098();
extern void func_800C3238();
extern void func_80071A44();
extern void func_80078CC4();
extern void func_800794C4();
extern void func_800C42A4();
extern SVECTOR D_8018EFF4;
extern OBJ D_80194628;
extern OBJ D_80194638;
extern short D_800942EC;

void func_8018F814(void *a0, void *a1, short *a2)
{
    MATRIX m;
    MATRIX m2;
    SVECTOR ang;
    VECTOR v;
    VECTOR s;
    unsigned char *x;
    unsigned char *r;
    MATRIX *pm;
    short *col;
    int c;

    x = func_800C2B50();
    ang = D_8018EFF4;
    r = *(unsigned char **)(*(unsigned char **)x + 0x238);
    func_800C2EAC(x[0x6C]);
    func_800C2FF0(32, 32);
    func_800C3098(16);
    func_800C3238(2);
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
    D_80194628.va = a2[9];
    m.t[0] = *(int *)(r + 0x134);
    m.t[1] = *(int *)(r + 0x138);
    m.t[2] = *(int *)(r + 0x13C);
    func_800C42A4(&D_80194628, &m, 1);
    m.t[0] = *(int *)(r + 0x1B4);
    m.t[1] = *(int *)(r + 0x1B8);
    m.t[2] = *(int *)(r + 0x1BC);
    func_800C42A4(&D_80194628, &m, 1);
    m2.t[0] = *(int *)(r + 0x134);
    m2.t[1] = D_800942EC;
    m2.t[2] = *(int *)(r + 0x13C);
    func_800C42A4(&D_80194638, &m2, 1);
    m2.t[0] = *(int *)(r + 0x1B4);
    m2.t[1] = D_800942EC;
    m2.t[2] = *(int *)(r + 0x1BC);
    func_800C42A4(&D_80194638, &m2, 1);
}
