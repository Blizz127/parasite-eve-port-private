# func_800133E8

VRAM `0x800133E8`, size `0x12C` (75 words), `asm/disc1/3BE8.s`. Frameless. No calls.

Reads the object at `D_8009D2F0`. Stores `1` at `D_8009D300[4]`. Returns `0` immediately when the high half of the word at `+0x14` equals the unsigned short at `+0x12`. Otherwise adds the word at `+0x1C` to the word at `+0x14` and keeps that sum in `$a0`. Two range checks against `short << 16` also return `0`. The unsigned byte at `+0xF` plus one is the divisor for the sum's high half: the positive arm is `hi % divs`, the negative arm is `divs + (hi % divs)`. Either scaled result can return `0`. The fall-through subtracts 8 from `D_8009CE00`.

era `-O2 -G8` with `MASPSX_FORCE_ABSOLUTE_SYMBOLS=D_8009D2F0`, `MASPSX_EXPAND_DIV=1`, and `MASPSX_DIV_DELAY_REUSE_V0=1`. Profile `era_o2_g8_div_delay_reuse_v0_expand_div_force_d8009d2f0_absolute`. `LINK_EXACT`, word mismatches 0, `nonzero_pad=0`. The linked section is 304 bytes (`0x130`) because `.text` is aligned to 16 and `0x12C` is 4 bytes short of that boundary; those bytes are zero. `leaf_strong_check.py`: `LEAF_STRONG=EXACT span=0x12C compiled=0x130`. `disc1_preflight.py --deep --only func_800133E8` passed before the rebuild.

The new knob rewrites one window only: cc1 parks `b + 1` in `$a3` because `$v0` still holds the `slt` the preceding `beq` consumed. Retail reuses `$v0` for `addu $v0,$a1,1` in that delay. The first `mfhi $v1` / `sll $a0,$v1,16` stay. The second remainder moves to `mfhi $v1` / `addu $v0,$v0,$v1` / `sll $a0,$v0,16`, and the join compares move back to `$v0`. Dropping the knob is 15 words (`nonzero_pad=0`). Dropping `MASPSX_EXPAND_DIV` is 30 words. Dropping the forced-absolute symbol is 73 words. The same source at `-O2 -G0` with no knobs does not match.

```
TMPDIR=build/tmp \
MASPSX_FORCE_ABSOLUTE_SYMBOLS=D_8009D2F0 \
MASPSX_EXPAND_DIV=1 \
MASPSX_DIV_DELAY_REUSE_V0=1 \
python3 tools/analysis/era_link_check.py \
  src/func_800133E8.c 0x800133E8 0x12C -O2 -G8
```
