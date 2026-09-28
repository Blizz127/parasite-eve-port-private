# func_80066F60 — PARKED (lane exeC, 2026-09-27)

- VRAM `0x80066F60`, file offset `0x57760`, size `0x268` (154 words), `asm/disc1/574E8.s`.

## This lane (exeC)

- Not re-attacked this lane (time went to the closer candidates; 10 of 29 closed). Best known state is the
  prior fan-out record below.

## Prior fan-out record (build/archive/fanout/*/results.jsonl)

- `decompB`: {"status": "skipped-for-now", "note": "13-register body with caller-save spills of $t0/$t1 around both calls (-fcaller-saves shape); not drafted"}
- `decompB`: {"status": "parked", "profile": "era_o2_g0", "best_mismatches": 152, "draft": "drafts/func_80066F60.c", "note": "First full draft (two-pass sprite/DR_MODE primitive builder, 2 outer passes x n records). cc1 2.7.2 does reproduce the caller-save spills of the 0x80 constant around both calls, so -fcaller-saves is not the blocker. Residual is structural: loop.c turns the four walkers into offset givs (p+14, rs+4, ps) where retail keeps plain pointer bivs s0/s1/s3/s4 reloaded from stack slots each outer pass, and the D_800BD024 load / frame layout differ (0x68 vs 0x70). Next: index-based inner loop and explicit outer-pass stack locals."}

## Lane exeC3 (2026-09-27) — brief re-look, still 149

Confirmed the decompB diagnosis with the cc1 output: loop.c strength-reduces the inner walkers into
offset givs (`p+14` in $16 with -11..0 displacements, `rs+4` beside `rs`, `q+4`), where retail keeps
plain pointers (s0 p, s1 rs, s4 ps, s3 q with 0..14 displacements) reloaded from the spilled
`rec`/`pos` slots (0x20/0x18(sp)) every outer pass. Moving the pointer increments to the body end
(149) and a goto-built inner loop (141, prologue still differs: retail loads `D_800BD024` before
the frame is built and keeps `a1` in t0) did not remove the givs. Next idea: make the walkers
opaque to loop.c (asm barrier copies at the preheader) and keep `rec`/`pos` as stack-resident
locals (volatile or address-taken) so they reload per pass.
