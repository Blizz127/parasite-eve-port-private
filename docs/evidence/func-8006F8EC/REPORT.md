# `func_8006F8EC` — dispatch a command record's offset-0xC handler

> Retail instruction words and byte strings in this file are elided (`xxxxxxxx`, `<N bytes …>`): no game data is committed. Read them from your own disc (`tools/extract/disc_cache.py`, EXE `SLUS_006.62`).

Outcome: **MATCHED** on era `-O2 -G0` (default profile). Integrated as
matching-C leaf. Linked at its retail VMA with its three referenced symbols
defined, the object `.text` is **byte-identical** to retail (`LINK_EXACT`).

## Function hood and retail span

- File span `[0x600EC,0x601F0)` = `0x104` bytes = 65 words.
- VRAM span `[0x8006F8EC,0x8006F9F0)`.
- Carved directly after matched `func_8006F820`; followed by `func_8006F9F0`.
- Uses the `D_800942E4`/`D_800942E8` arenas and the `D_800942E0` handler
  table (same family as `func_8006F224`/`func_8006F2C4`/`func_8006F820`).

## Semantics (from retail bytes)

```text
8006f8ec  xxxxxxxx  addiu sp,sp,-0x18
8006f8f0  xxxxxxxx  sltiu v0,a0,0x16      ; id < 0x16 ?
8006f8f4  xxxxxxxx  bnez  v0,0x8006F904
8006f8f8  xxxxxxxx  sw    ra,0x10(sp)      ; delay
8006f8fc  xxxxxxxx  j     0x8006F9E0
8006f900  xxxxxxxx  li    v0,-0x10         ; delay: return -0x10
.L8006F904:
8006f904  xxxxxxxx  sltiu v0,a0,0xB        ; id < 0xB ?
8006f908  xxxxxxxx  bnez  v0,0x8006F934
8006f90c  xxxxxxxx  sll   v0,a0,2          ; delay
   (id>=0xB) ... / lw v0,D_800942E8         ; stride 0x10C
.L8006F934:
   (id<0xB)  ... / lw v1,D_800942E4         ; stride 0xA0C
8006f954  addu a0,v1,v0                     ; a0 = record
8006f958  lbu v0,0(a0)
8006f960  addiu v0,v0,-1
8006f964  sltiu v0,v0,2                     ; state in {1,2} ?
8006f968  beqz v0,0x8006F9E0
8006f96c  xxxxxxxx  move  v0,zero           ; delay: return 0
8006f970  lbu a1,1(a0)                      ; handler byte
8006f978  sltiu v0,a1,0xC0
8006f97c  bnez  v0,0x8006F98C
8006f980  xxxxxxxx  sltiu v0,a1,0x55         ; delay
8006f984  xxxxxxxx  j     0x8006F9E0
8006f988  xxxxxxxx  li    v0,-0x11           ; delay
.L8006F98C:
8006f98c  xxxxxxxx  bnez  v0,0x8006F998
8006f990  xxxxxxxx  nop
8006f994  xxxxxxxx  li    a1,0x55            ; clamp
.L8006F998:
8006f998  xxxxxxxx  lui   v1,0x8009
8006f99c  xxxxxxxx  lw    v1,D_800942E0
8006f9a0  xxxxxxxx  sll   v0,a1,2 / addu / lw v0,0(v0)
8006f9b4  xxxxxxxx  bnez  v0,0x8006F9C0
8006f9bc  xxxxxxxx  j     0x8006F9E0 / delay li v0,-0x12   (null handler)
.L8006F9C0:
8006f9c0  xxxxxxxx  lw    v0,0xC(v0)         ; handler[0xC]
8006f9c8  xxxxxxxx  bnez  v0,0x8006F9D8
8006f9d0  xxxxxxxx  j     0x8006F9E0 / delay li v0,-1
.L8006F9D8:
8006f9d8  xxxxxxxx  jalr  v0                 ; fn(record)
8006f9dc  xxxxxxxx  nop
.L8006F9E0:
8006f9e0  xxxxxxxx  lw    ra,0x10(sp)
8006f9e4  xxxxxxxx  addiu sp,sp,0x18
8006f9e8  xxxxxxxx  jr    ra
8006f9ec  xxxxxxxx  nop
```

C shape (`src/func_8006F8EC.c`):

```c
int func_8006F8EC(int idx) {
    unsigned char *p;
    int h;
    int (*fn)(unsigned char *);
    if ((unsigned int)idx >= 0x16)
        return -0x10;
    if ((unsigned int)idx >= 0xB)
        p = D_800942E8 + (idx - 0xB) * 0x10C;
    else
        p = D_800942E4 + idx * 0xA0C;
    if ((unsigned int)(p[0] - 1) >= 2)
        return 0;
    h = p[1];
    if ((unsigned int)h >= 0xC0)
        return -0x11;
    if ((unsigned int)h >= 0x55)
        h = 0x55;
    if (D_800942E0[h] == 0)
        return -0x12;
    fn = *(int (**)(unsigned char *))((unsigned char *)D_800942E0[h] + 0xC);
    if (fn != 0)
        return fn(p);
    return -1;
}
```

## Levers

1. **`h` must be `int`, not `unsigned char`.** With a char local cc1 emits
   `andi v1,a1,0xff` before the `sltiu`/`slti` chain (10 extra bytes);
   retail compares the `lbu` result directly.
2. **Pointer-global arenas** (`lui`/`lw`) as in the sibling leaves.
3. **`fn != 0` (not `fn == 0`) for the last test** so cc1 places the `jalr`
   call as the branch target and the `return -1` as fallthrough, exactly as
   retail's `bnez v0,0x8006F9D8`.

## Single-leaf object

```text
AS=tools/mipsel-host/usr/bin/mipsel-linux-gnu-as \
OBJDUMP=tools/mipsel-host/usr/bin/mipsel-linux-gnu-objdump \
OBJCOPY=tools/mipsel-host/usr/bin/mipsel-linux-gnu-objcopy \
  tools/analysis/era_leaf_match.sh src/func_8006F8EC.c 0x8006F8EC 0x104 -O2 -G0
```

Result: `MISMATCHES=11`, size `0x110` (12-byte link pad) vs ROM `0x104`. All
mismatches are link-time relocation fields (`lui`/`lw` of the three symbols
and the local `j` targets):

| VRAM | retail | object | relocation |
|---|---|---|---|
| `0x8006F8FC` | `0801be78` | `0800003d` | R_MIPS_26 local `.L8006F9E0` |
| `0x8006F924` | `3c028009` | `3c020000` | R_MIPS_HI16 `D_800942E8` |
| `0x8006F928` | `8c4242e8` | `8c420000` | R_MIPS_LO16 `D_800942E8` |
| `0x8006F92C` | `0801be55` | `0800001a` | R_MIPS_26 local `.L8006F954` |
| `0x8006F948` | `3c038009` | `3c030000` | R_MIPS_HI16 `D_800942E4` |
| `0x8006F94C` | `8c6342e4` | `8c630000` | R_MIPS_LO16 `D_800942E4` |
| `0x8006F984` | `0801be78` | `0800003d` | R_MIPS_26 local |
| `0x8006F998` | `3c038009` | `3c030000` | R_MIPS_HI16 `D_800942E0` |
| `0x8006F99C` | `8c6342e0` | `8c630000` | R_MIPS_LO16 `D_800942E0` |
| `0x8006F9B8` | `0801be78` | `0800003d` | R_MIPS_26 local |
| `0x8006F9D0` | `0801be78` | `0800003d` | R_MIPS_26 local |

Link-level proof (assemble, link at `0x8006F8EC` with
`--defsym D_800942E4=0x800942E4 --defsym D_800942E8=0x800942E8
--defsym D_800942E0=0x800942E0`, compare the linked `.text` word-for-word
with the ROM):

```text
linked .text 272 bytes, target 0x104, word mismatches=0
LINK_EXACT
```

## Registration

- Source: `src/func_8006F8EC.c`; YAML carve
  `- [0x600EC, c, func_8006F8EC]` (asm resumes at `0x601F0`). Default
  `era_o2_g0` profile.
- `python3 tools/build/disc1_plan.py --check` → 860 spans
  (570 c, 288 asm, 2 rodata), geometry `0x1EE000`.
- `python3 tools/build/test_disc1_plan.py` → 8 tests OK.
