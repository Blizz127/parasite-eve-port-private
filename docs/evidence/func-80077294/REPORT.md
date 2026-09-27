# func_80077294

VRAM `0x80077294`, size `0x13C`.

```c
extern int func_800773D0(void);
extern void func_80076EE4(void);
extern int func_80077404(void);
extern volatile int D_80095874;
extern volatile int D_80095878;
extern volatile unsigned int *D_80095860;
extern volatile unsigned int *D_80095854;

int func_80077294(int a0) {
    if (a0 == 0) {
        func_800773D0();
        while (D_80095874 != D_80095878) {
            func_80076EE4();
            if (func_80077404() != 0) {
                return -1;
            }
        }
        while ((*D_80095860 & 0x01000000) != 0 || (*D_80095854 & 0x04000000) == 0) {
            if (func_80077404() != 0) {
                return -1;
            }
        }
        return 0;
    } else {
        int n = (D_80095874 - D_80095878) & 0x3F;
        if (n != 0) {
            func_80076EE4();
        }
        if (((*D_80095860 & 0x01000000) == 0 && (*D_80095854 & 0x04000000) != 0) || n != 0) {
            return n;
        }
        return 1;
    }
}
```

Flags: `ERA_CC1_VER=2.8.1` `-O2 -G0 -mno-split-addresses`. Env: none beyond `ERA_CC1_VER`.

```
linked .text 320 bytes, target 0x13c, word mismatches=0, nonzero_pad=0
LINK_EXACT
```
