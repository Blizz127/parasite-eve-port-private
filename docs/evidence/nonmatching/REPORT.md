# NON_MATCHING native ports (functionally equivalent, not byte-exact)

Native-only does not require byte matching.  When a function will not match in
reasonable time, its C is installed as a functionally equivalent native port:

* source: `src/nonmatching/overlays/<ovl>/<func>.c` (header says NON_MATCHING);
* span: `configs/USA/overlays/nonmatching.yaml` (`leaves:`), while the overlay
  YAML keeps the span `asm`, so `scripts/exact_rebuild_overlay.sh` and the
  retail SHA-1 gate are unchanged (it is NOT LINK_EXACT);
* `tools/analysis/gen_overlay_ports.py` lowers it like a matched leaf (E21) and
  registers it; the runtime registry still keys it to the retail bytes (FNV-1a
  over the span), so it only runs where the retail code it replaces is resident;
* `tools/analysis/e21_diff.py` differential-tests it against the retail asm in
  the offline MIPS-I/GTE model: guest RAM and scratchpad after the call, the
  ordered call log with every argument (stack arguments too; stack-local
  pointers compared by the bytes the retail code wrote), and the return value.
  Inputs: seeded random RAM with the directed terms from `nonmatching.yaml`
  (`inputs:`), and snapshots recorded from the game with
  `PE_RECORD_GUESTCALL=VMA:DIR[:N[:EVERY]]` (`--recorded DIR`).

| Leaf | Span | Why not matched | Directed seeds | Recorded (game) |
| --- | --- | --- | --- | --- |
| room_m0156i func_8018F058 (particle emitter) | 0x70 +0x11D8 | mode 1 exact; mode 2 register allocation / division sequences | 476 ok of 600, 1023/1142 words | 63 ok (m0156i fight), modes 1/2, cases 0,2-5 |
| room_m0156i func_80192208 (debris effect) | 0x3220 +0x7E0 | written from the asm, not matched | 223 ok of 300, 458/504 words | 36 ok (m0380i fight end) |
| room_m0156i func_80191B94 (debris particle) | 0x2BAC +0x674 | written from the asm, not matched | 600 ok, 397/413 words | 24 ok, 367/413 words |

The same bytes run in room m0380i (no overlay config of its own): the registry
resolves it there by fingerprint.  The differential test caught one real bug
in the first 8018F058 draft (mode 2 case 1 reset `pos` once instead of every
iteration), which the byte diff had only shown as "mismatch".
