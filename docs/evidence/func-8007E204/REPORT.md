# func_8007E204 — LINK_EXACT

VRAM `0x8007E204`, size `0x14` (5 words). Tail jump through the pointer at `jtbl_800A34C8`.

```c
extern void (*jtbl_800A34C8)();

void func_8007E204(void) {
    register void (*f)() asm("$9");
    f = jtbl_800A34C8;
    goto *(void *)f;
}
```

Retail is `lui $t1` / `lw $t1, %lo` / `nop` / `jr $t1` / `nop`. A call emits `jalr` plus a frame; the computed goto is a real `jr`, and the `$9` pin keeps the destination register. Flags `-O2 -G0` (cc1 2.7.2, aspsx 2.30). No extra env.

`python3 tools/analysis/era_link_check.py --all-mismatches src/func_8007E204.c 0x8007E204 0x14 -O2 -G0`

```
linked .text 32 bytes, target 0x14, word mismatches=0, nonzero_pad=0
LINK_EXACT
```
