/*
 * func_80072334 - memmove(dst, src, n) returning dst.  vram 0x80072334, size 0x6C
 * (the word at 0x800723A0 is a nop pad that stays asm).
 * era: ERA_CC1_VER=2.8.1  cc1 -O2 -G0 -mno-split-addresses
 *      MASPSX_RETURN_VIA_EPILOGUE_JUMP=1 (maspsx patch 12; 1 word without it:
 *      2.8.1 turns the backward-copy exit into a second `jr $ra`, retail jumps
 *      to the final `jr $ra` with the `move $v0,$a3` in the slot).
 * Draft by lane4.
 */
char *func_80072334(char *dst, char *src, int n) {
    if (dst >= src) {
        while (n-- > 0) dst[n] = src[n];
    } else {
        while (n-- > 0) *dst++ = *src++;
    }
    return dst;
}
