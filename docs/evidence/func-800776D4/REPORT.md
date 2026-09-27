# func_800776D4

VRAM `0x800776D4`, size `0xEC`. Twin of `func_800775E8` (`D_800118E0`, slot `D_80095744[7]`).

```c
extern char D_800118E0;
extern void func_80074E28(char *name, int r);
extern int func_80073A44(int mode);
extern volatile unsigned int *D_80095860;
extern int D_80095888;
extern int D_8009588C;
extern volatile unsigned int *D_80095854;
extern int func_80077404(void);
extern void func_80073CF4(int slot, void (*fn)());
extern void func_80077A00(void);
extern unsigned int *D_80095744;

int func_800776D4(int a0, int a1) {
    func_80074E28(&D_800118E0, a0);
    D_80095888 = func_80073A44(-1) + 240;
    D_8009588C = 0;
    while ((*D_80095860 & 0x01000000) != 0 || (*D_80095854 & 0x04000000) == 0) {
        if (func_80077404() != 0) {
            return -1;
        }
    }
    func_80073CF4(2, func_80077A00);
    ((void (*)(int, int))D_80095744[7])(a0, a1);
    return 0;
}
```

Flags: `ERA_CC1_VER=2.8.1` `-O2 -G0 -mno-split-addresses`. Env: none beyond `ERA_CC1_VER`.

```
linked .text 240 bytes, target 0xec, word mismatches=0, nonzero_pad=0
LINK_EXACT
```
