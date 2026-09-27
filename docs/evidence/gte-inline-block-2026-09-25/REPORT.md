# gte-inline spans — instruction blocker

`tools/analysis/nonmatchable_spans.py --json` on the split asm after the 2033
gate still lists exactly five `gte-inline` functions. Each one contains a COP
transfer that era cc1 cannot emit. Real inline asm is not an accepted match,
so these spans stay retail bytes. They are not matching C, and the 2033 image
is not complete.

Probe, both compilers, integer stand-ins only (`lzcs_like`, empty
`gte_init_like`, `square_root_like`):

```
tools/era/gcc-2.7.2-psx/cpp probe.c > probe.i
tools/era/gcc-2.7.2-psx/cc1 -quiet -O2 -G0 probe.i -o probe.s
tools/era/gcc-2.8.1-psx/cc1 -quiet -O2 -G0 probe.i -o probe281.s
```

`grep` of `mtc2|mfc2|ctc2|cfc2|swc2|lwc2|mtc0|mfc0|cop2` on both assembly
files prints nothing (`PROBE_ASM_NONE`, `PROBE281_ASM_NONE`). `strings` of
each `cc1` has none of those mnemonics as a whole line
(`CC1_EMITS_NO_COP_MNEMONIC`, `CC1281_EMITS_NO_COP_MNEMONIC`).

Fresh classifier counts from that same `--json` run: `alignment-filler` 83 /
83w, `gte-inline` 5 / 192w, `handwritten-cop` 22 / 123w, `handwritten-gte-wrapper`
107 / 19957w, `handwritten-jr-t2` 53 / 163w, `handwritten-syscall` 2 / 8w.
Unclassified split functions outside those classes: 136 / 39180w.

| function | VRAM | size | file | instruction |
| --- | --- | --- | --- | --- |
| `func_8003EAC8` | `0x8003EAC8` | `0x3C` | `asm/disc1/2F2C8.s` | `0x8003EACC` `mtc2 $a0,$30` word `00F08448`; `0x8003EADC` `swc2 $31,0($sp)` word `0000BFEB` |
| `func_800130B4` | `0x800130B4` | `0x134` | `asm/disc1/38B4.s` | `0x80013188` `mtc2 $v0,$30` word `00F08248`; `0x800131A0` `swc2 $31,0($a2)` word `0000DFE8` |
| `func_80077F7C` | `0x80077F7C` | `0x80` | `asm/disc1/68768.s` | `0x80077F98` `mfc0 $v0,$12` word `00600240`; `0x80077FA4` `mtc0 $v0,$12` word `00608240`; `ctc2 $t0,$29` `00E8C848` at `0x80077FB0`; `ctc2 $t0,$30` `00F0C848` at `0x80077FBC`; `ctc2 $t0,$26` `00D0C848` at `0x80077FC8`; `ctc2 $t0,$27` `00D8C848` at `0x80077FD4`; `ctc2 $t0,$28` `00E0C848` at `0x80077FE0`; `ctc2 $zero,$24` `00C0C048` at `0x80077FE8`; `ctc2 $zero,$25` `00C8C048` at `0x80077FEC` |
| `func_80078004` | `0x80078004` | `0x84` | `asm/disc1/68768.s` | `0x80078004` `mtc2 $a0,$30` bytes `00F08448`; `0x80078010` `mfc2 $v0,$31` bytes `00F80248`. Also trapping `sub $t1,$t1,$t2` bytes `22482A01` at `0x80078030`, `addi $t3,$t2,-0x18` bytes `E8FF4B21` at `0x80078038`, `sub $t3,$t3,$t2` bytes `22586A01` at `0x80078050`, `addi $t4,$t4,-0x40` bytes `C0FF8C21` at `0x80078058`. cc1 emits `subu` / `addiu` |
| `func_80078094` | `0x80078094` | `0x8C` | `asm/disc1/68768.s` | `0x80078094` `mtc2 $a0,$30` word `00F08448`; `0x800780A0` `mfc2 $v0,$31` word `00F80248`. Same four trapping `sub` / `addi` at `0x800780C8`, `0x800780D0`, `0x800780E8`, `0x800780F0` |

The split lines for every COP op above carry `/* handwritten instruction */`.
No C spelling of these five bodies can be `LINK_EXACT`.
