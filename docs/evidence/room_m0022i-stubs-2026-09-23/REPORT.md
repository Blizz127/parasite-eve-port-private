# room_m0022i — two-word behaviour stubs (2026-09-23)

Fifteen 8-byte functions in the `room_m0022i` text window (VRAM base
`0x8018EFE8`). Retail bytes are `03E00008 00001021` (`jr ra; move v0,zero`
→ `int f(void) { return 0; }`) for fourteen of them, and `03E00008 00000000`
(`void f(void) {}`) for `func_80191D00`. Each is `LINK_EXACT` at its VMA
(`era_link_check.py --target room_m0022i src/overlays/room_m0022i/<f>.c <vram> 0x8 -O2 -G0`).

`func_8018F4F8` 0x510, `func_8018FF34` 0xF4C, `func_8018FF3C` 0xF54,
`func_80190238` 0x1250, `func_801909D8` 0x19F0, `func_801909E0` 0x19F8,
`func_80190CCC` 0x1CE4, `func_801914FC` 0x2514, `func_80191504` 0x251C,
`func_801917E4` 0x27FC, `func_80191D00` 0x2D18 (void), `func_801920EC` 0x3104,
`func_801920F4` 0x310C, `func_801921D0` 0x31E8, `func_801924E0` 0x34F8.

```
$ python3 tools/build/disc1_preflight.py --deep --target room_m0022i
disc1_preflight: PASS (deep, 41 c / 12 asm / 0 rodata)
$ scripts/exact_rebuild_overlay.sh room_m0022i
EXACT_REBUILD_GATE[room_m0022i]=PASS plan=dbf3824d3faf0625634028f669c0f5f44f1b324d5729447738aa427ee4b6d2ea yaml=fe2d8e1f55a61bda377d9523976ffc82582d1219997b04cb6968732f06b5d54b spans=[41 c, 12 asm, 0 rodata, 2 bin] sha1_orig=3231b287bdde6fb55bcc3734f5be6ff4906ab0c3 sha1_cand=3231b287bdde6fb55bcc3734f5be6ff4906ab0c3 sha1=3231b287bdde6fb55bcc3734f5be6ff4906ab0c3
```
