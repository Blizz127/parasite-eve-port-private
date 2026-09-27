> **RESOLVED 2026-09-27 (lane exeC, second pass): LINK_EXACT — see `REPORT.md`.**

# func_8007136C — PARKED (lane exeC, 2026-09-27)

- VRAM `0x8007136C`, file offset `0x61B6C`, size `0x338` (206 words), `asm/disc1/618A4.s`.

## This lane (exeC)

- 87 -> 49 (832 bytes, only the $s0/$s1 swap of sp vs state/ang left) with the func_800710A4 levers: tail r (r += 0x800; r += ...; r &= 0xFFF), asm volatile("") between func_80077CF4 and the sp negate, int state param reused as ang. Retail here keeps sp in $s0 and state+ang in $s1 (0x17 in $s3). Un-merging ang (own local pinned $17) makes the whole head exact through 0x800714D8 but the d-pad cascade then cross-jumps the -sp arms into the +sp tails (cc1 negates $s0 in place: `subu $16,$0,$16`), 816 bytes / 97 words; asm("") before the -sp stores restores 832 bytes but the f/a (D_8009D26C/D_800BD020) registers swap (v1/a0) -> 90. sp pinned $16 shrinks to 816 (97). Drafts: build/lanes/exeC/drafts/func_8007136C_headexact.c.

## Prior fan-out record (build/archive/fanout/*/results.jsonl)

- `decompB`: {"status": "parked", "profile": "era_o2_g0", "best_mismatches": 87, "draft": "drafts/func_8007136C.c", "note": "running twin of func_800710A4 (speed base 0x168000/0xB4000/0x21C000 from the anim flags, state 0x17 or the model's +0x13). Structure exact on 2.7.2; residual is the same register-priority swap as 800710A4: retail speed in s0 and state->angle sharing s1, cc1 gives the speed s1. Pinning the state/angle to $17 inserts an extra param copy (200w); also the ang+0x1000 fold and the D_8009D26C/D_800BD020 load registers."}
