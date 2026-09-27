/*
 * func_800742B8 - interrupt-state enable.  vram 0x800742B8, size 0x78.
 * era: ERA_CC1_VER=2.8.1  cc1 -O2 -G0 -mno-split-addresses
 *      MASPSX_LOAD_DELAY_CALL_SLOT_STORE=1 (maspsx patch 13; 11 words without
 *      it: retail keeps the load-delay nop between `lw $v0,0x34($s0)` and the
 *      `jal func_80072724` whose delay slot is `sw $v0,0($v1)`).
 * Draft by lane4.
 */
typedef struct {
    unsigned short active;
    char pad2[0x30];
    unsigned short mask;
    int f34;
    int f38;
} IntrState;
extern IntrState D_800945E4;
extern unsigned short *D_80095674;
extern int *D_80095678;
extern void func_800743A4(void *);
extern void func_80072724(void);

void *func_800742B8(void) {
    IntrState *s = &D_800945E4;
    if (s->active != 0) {
        return 0;
    }
    func_800743A4(&s->f38);
    s->active = 1;
    *D_80095674 = s->mask;
    *D_80095678 = s->f34;
    func_80072724();
    return s;
}
