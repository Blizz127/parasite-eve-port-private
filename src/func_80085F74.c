/*
 * func_80085F74 - libspu SpuSetCommonAttr.  vram 0x80085F74, size 0x37C (223 words;
 * the terminal `jr $ra` is at 0x800862E8 and the word at 0x800862F0 is a lone `nop`
 * alignment pad that stays asm).
 * era: ERA_CC1_VER=2.8.1  cc1 -O2 -G0 -mno-split-addresses
 *      MASPSX_THREE_WORD_SYMBOL_STORE=1
 *      MASPSX_DISPATCH_FOLD=jtbl_800120BC,jtbl_800120DC -> LEAF_STRONG=EXACT
 *
 * Two switches, two pool tables. The multi-table fold maps cc1's .rdata switch
 * tables in emission order (left volume-mode switch first, right second) onto
 * the two contiguous pool tables; the swapped list order fails the link check.
 */
extern unsigned char *D_8009B3FC;

void func_80085F74(unsigned char *a0)
{
    register unsigned int mask asm("$9");
    register int z asm("$10");
    register int lvol asm("$6");
    register int rvol asm("$8");
    register int lmode asm("$5");
    register int rmode asm("$5");
    register int lsv asm("$7");
    register int luv asm("$3");
    register int rsv asm("$6");
    register int ruv asm("$3");
    register int t asm("$2");

    lvol = 0;
    asm ("" : "=r" (lvol) : "0" (lvol));
    mask = *(unsigned int *)a0;
    z = (mask == 0);
    rvol = 0;
    if (z || (mask & 1)) {
        if (z || (mask & 4)) {
            switch (*(short *)(a0 + 8)) {
            case 0:
                goto lplain;
            case 1:
                lmode = 0x8000;
                break;
            case 2:
                lmode = 0x9000;
                break;
            case 3:
                lmode = 0xA000;
                break;
            case 4:
                lmode = 0xB000;
                break;
            case 5:
                lmode = 0xC000;
                break;
            case 6:
                lmode = 0xD000;
                break;
            case 7:
                lmode = 0xE000;
                break;
            default:
                goto lplain;
            }
        } else {
lplain:
            lvol = *(unsigned short *)(a0 + 4);
            lmode = 0;
        }
        if (lmode != 0) {
            lsv = *(short *)(a0 + 4);
            luv = *(unsigned short *)(a0 + 4);
            if (lsv >= 0x80) {
                lvol = 0x7F;
            } else {
                lvol = 0;
                if (lsv >= 0) {
                    lvol = luv;
                }
            }
        }
        t = lvol & 0x7FFF;
        *(unsigned short *)(D_8009B3FC + 0x180) = t | lmode;
    }
    if (z || (mask & 2)) {
        if (z || (mask & 8)) {
            switch (*(short *)(a0 + 0xA)) {
            case 0:
                goto rplain;
            case 1:
                rmode = 0x8000;
                break;
            case 2:
                rmode = 0x9000;
                break;
            case 3:
                rmode = 0xA000;
                break;
            case 4:
                rmode = 0xB000;
                break;
            case 5:
                rmode = 0xC000;
                break;
            case 6:
                rmode = 0xD000;
                break;
            case 7:
                rmode = 0xE000;
                break;
            default:
                goto rplain;
            }
        } else {
rplain:
            rvol = *(unsigned short *)(a0 + 6);
            rmode = 0;
        }
        if (rmode != 0) {
            rsv = *(short *)(a0 + 6);
            ruv = *(unsigned short *)(a0 + 6);
            if (rsv >= 0x80) {
                rvol = 0x7F;
            } else {
                rvol = 0;
                if (rsv >= 0) {
                    rvol = ruv;
                }
            }
        }
        t = rvol & 0x7FFF;
        *(unsigned short *)(D_8009B3FC + 0x182) = t | rmode;
    }
    if (z || (mask & 0x40)) {
        *(unsigned short *)(D_8009B3FC + 0x1B0) = *(unsigned short *)(a0 + 0x10);
    }
    if (z || (mask & 0x80)) {
        *(unsigned short *)(D_8009B3FC + 0x1B2) = *(unsigned short *)(a0 + 0x12);
    }
    if (z || (mask & 0x400)) {
        *(unsigned short *)(D_8009B3FC + 0x1B4) = *(unsigned short *)(a0 + 0x1C);
    }
    if (z || (mask & 0x800)) {
        *(unsigned short *)(D_8009B3FC + 0x1B6) = *(unsigned short *)(a0 + 0x1E);
    }
    if (z || (mask & 0x100)) {
        if (*(int *)(a0 + 0x14) == 0) {
            *(unsigned short *)(D_8009B3FC + 0x1AA) =
                *(unsigned short *)(D_8009B3FC + 0x1AA) & 0xFFFB;
        } else {
            *(unsigned short *)(D_8009B3FC + 0x1AA) =
                *(unsigned short *)(D_8009B3FC + 0x1AA) | 4;
        }
    }
    if (z || (mask & 0x200)) {
        if (*(int *)(a0 + 0x18) == 0) {
            *(unsigned short *)(D_8009B3FC + 0x1AA) =
                *(unsigned short *)(D_8009B3FC + 0x1AA) & 0xFFFE;
        } else {
            *(unsigned short *)(D_8009B3FC + 0x1AA) =
                *(unsigned short *)(D_8009B3FC + 0x1AA) | 1;
        }
    }
    if (z || (mask & 0x1000)) {
        if (*(int *)(a0 + 0x20) == 0) {
            *(unsigned short *)(D_8009B3FC + 0x1AA) =
                *(unsigned short *)(D_8009B3FC + 0x1AA) & 0xFFF7;
        } else {
            *(unsigned short *)(D_8009B3FC + 0x1AA) =
                *(unsigned short *)(D_8009B3FC + 0x1AA) | 8;
        }
    }
    t = mask & 0x2000;
    if (z || t) {
        if (*(int *)(a0 + 0x24) == 0) {
            *(unsigned short *)(D_8009B3FC + 0x1AA) =
                *(unsigned short *)(D_8009B3FC + 0x1AA) & 0xFFFD;
        } else {
            *(unsigned short *)(D_8009B3FC + 0x1AA) =
                *(unsigned short *)(D_8009B3FC + 0x1AA) | 2;
        }
    }
    asm ("" : : : "memory");
}
