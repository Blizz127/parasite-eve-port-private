# func_80077904

VRAM `0x80077904`, size `0xFC`.

```c
extern unsigned char D_8009574E;
extern char D_80011928;
extern void (*D_80095748)(char *fmt, int a);
extern int func_80073A44(int mode);
extern volatile unsigned int *D_80095860;
extern int D_80095888;
extern int D_8009588C;
extern volatile unsigned int *D_80095854;
extern int func_80077404(void);
extern void func_80073CF4(int slot, void (*fn)());
extern void func_80077A00(void);
extern unsigned int *D_80095744;

int func_80077904(int a0) {
    if (D_8009574E >= 2) {
        D_80095748(&D_80011928, a0);
    }
    D_80095888 = func_80073A44(-1) + 240;
    D_8009588C = 0;
    while ((*D_80095860 & 0x01000000) != 0 || (*D_80095854 & 0x04000000) == 0) {
        if (func_80077404() != 0) {
            return -1;
        }
    }
    func_80073CF4(2, func_80077A00);
    ((void (*)(int))D_80095744[6])(a0);
    return 0;
}
```

Flags: `ERA_CC1_VER=2.8.1` `-O2 -G0 -mno-split-addresses`. Env: none beyond `ERA_CC1_VER`.

```
linked .text 256 bytes, target 0xfc, word mismatches=0, nonzero_pad=0
LINK_EXACT
```
