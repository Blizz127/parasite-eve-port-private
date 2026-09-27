/*
 * func_800862F4 - libspu per-voice volume write (left/right volume + sweep mode
 * into D_8009B3FC[voice*8 + 0/1]) followed by the libspu volatile x13 wait loop.
 * vram 0x800862F4, size 0x16C.
 * era: ERA_CC1_VER=2.8.1  cc1 -O2 -G0 -mno-split-addresses
 *      MASPSX_THREE_WORD_SYMBOL_STORE=1
 *      MASPSX_DISPATCH_FOLD=jtbl_800120FC,jtbl_8001211C   (two switch tables,
 *        each 7 entries + one .align 3 zero word in the pool)
 *      MASPSX_FILL_JUMP_STACK_STORE=1   (maspsx patch 11: the loop-entry
 *        `j` carries the volatile `sw $0,0($sp)` in its delay slot)
 *
 * Load-bearing levers:
 *  - the SPU register pointer's target is volatile: keeps the right-channel
 *    store ahead of the volatile `v = 1` / `i = 0` stores;
 *  - lvol/rvol are unsigned short (the int form schedules the prologue
 *    `andi` after the stack-argument `lhu`);
 *  - the masked right volume lives in the dead `lmode` parameter and the
 *    right mode in `rvol`: that is retail's $a3/$a2 assignment and the
 *    `or $v1,$a3,$a2` operand order.
 */
extern volatile unsigned short *D_8009B3FC;

void func_800862F4(int voice, unsigned short lvol, unsigned short rvol, short lmode, short rmode)
{
    volatile int i;
    volatile int v;
    int lm;
    lvol &= 0x7FFF;
    lm = 0;
    voice *= 8;
    switch (lmode) {
    case 1: lm = 0x8000; break;
    case 2: lm = 0x9000; break;
    case 3: lm = 0xA000; break;
    case 4: lm = 0xB000; break;
    case 5: lm = 0xC000; break;
    case 6: lm = 0xD000; break;
    case 7: lm = 0xE000; break;
    }
    lmode = rvol & 0x7FFF;
    rvol = 0;
    D_8009B3FC[voice] = lvol | lm;
    switch (rmode) {
    case 1: rvol = 0x8000; break;
    case 2: rvol = 0x9000; break;
    case 3: rvol = 0xA000; break;
    case 4: rvol = 0xB000; break;
    case 5: rvol = 0xC000; break;
    case 6: rvol = 0xD000; break;
    case 7: rvol = 0xE000; break;
    }
    D_8009B3FC[voice + 1] = lmode | rvol;
    v = 1;
    for (i = 0; i < 2; i++) {
        v *= 13;
    }
}
