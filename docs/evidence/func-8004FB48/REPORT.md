# func_8004FB48

LINK_EXACT at full size `0xF4` (244 bytes).

```
python3 tools/analysis/era_link_check.py --all-mismatches src/func_8004FB48.c 0x8004FB48 0xF4 -O2 -G8
linked .text 256 bytes, target 0xf4, word mismatches=0, nonzero_pad=0
LINK_EXACT
```

Flags: `-O2 -G8`. No maspsx env. `-G0` misses (the `D_8009CEF4` store becomes `lui`/`sw`).

The else arm's empty `asm volatile("" : "=r"(mask) : "0"(mask))` barrier emits no bytes. It stops gcc from cross-jumping the two `excluded = -1` blocks, so reorg keeps retail's `beq` / `nop` / `j` / `li $s1,-1` / `li $s1,-1`.

```c
extern int D_8009CEF4;

extern int func_80062A34(int, int);
extern int func_80063428(int);
extern int func_80059F08(int);
extern int func_8005415C(int);
extern int func_80052F0C(void);
extern void func_80052E30(int);
extern void func_800543CC(int, int);
extern int func_80054288(void);
extern void func_800647D0(int, int);
extern void func_800430A0();
extern void func_800638D8(int, void (*)());

void func_8004FB48(int a0) {
    int bank;
    int mask;
    int excluded;
    int kind;

    bank = func_80063428(func_80062A34(2, 54));
    if (bank >= 0) {
        mask = 830;
        if (*(int *)(a0 + 36) == 7) {
            excluded = func_80059F08(0);
            kind = func_8005415C(excluded);
            if (kind && kind < 6) {
                mask = 62;
            } else {
                mask = 1 << kind;
            }
            if (func_80052F0C() != bank) {
                excluded = -1;
            }
        } else {
            excluded = -1;
            asm volatile("" : "=r"(mask) : "0"(mask));
        }
        func_80052E30(bank);
        func_800543CC(mask, excluded);
        func_800647D0(a0, func_80054288());
    }
    D_8009CEF4 = a0;
    func_800638D8(a0, func_800430A0);
}
```
