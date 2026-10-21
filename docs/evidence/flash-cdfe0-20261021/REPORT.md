# Eve's flash: binding the framed weapon-update callback 800CDFE0

Branch `claude/pe-cdfe0-20261020` (worktree `build/wt-pe-cdfe0-20261020`), commits `7fa5400ce` (DEV_MENU.md
wording) and `9f13cb73c` (binding + test) on `c7254fb50`. Local only: not pushed, not packaged, not part of
the owner-private candidate `native-only-c7254fb5-p3` (whose binary c0c298fc predates this change).

## Why 800CDFE0 keeps the borrowed bytes

KNOWN_DIVERGENCES #17 models Eve's flash position, which retail reads from caller-stack bytes another routine
left behind (the effect-stack window [S-112, S-106)). Any callback that could write that window makes the model
forget the position, and the flash then falls back to the owner joint position (`[DAY1_SHIM]` line). The
frameless update leaves were already bound; 800CDFE0 (entry 4 of the `D_800E0F38` hit table) has a 32-byte
frame, so it was treated as a writer. Retail bytes (`objdump` of the EXE text; excerpts kept with the lane
evidence in `/home/blizz/scratch/pe-cdfe0-20261020/dis-*.txt`):

- C9B90/CD8F0/CE16C (frame 32) call C251C at S-32; C251C (frame 40) dispatches record callbacks by `jalr` at
  800C2638, so a callback enters at S-72.
- 800CDFE0 runs at sp S-104 and saves s0/s1/ra at sp+16..+27 = S-88..S-77, above the window.
- Its one call is the BIOS rand stub 80071A54 (`li t2,0xA0; jr t2; li t1,0x2F`), which touches no EXE stack.
  The BIOS A(2Fh) body is not modelled, as for every BIOS call (M34_FRAME_STACK_PRESERVATION.md).
- Its stores go through a1 (`sb 1(a1)`: retire when life < 3) and a2 (`sb 1`, `sh 4`, `sh 6`); the update is
  life -= 3, data+6 -= 4, data+4 += rand() % 11 - 5 (the `0x2E8BA2E9` multiply is the divide by 11).

`k_framed_update` lists it with the same record/data bounds test as the frameless leaves.

## Results (binary fcab4d4c…, gcc 15.2, built from 9f13cb73 in a fresh worktree)

| check | result |
| --- | --- |
| generated trees regenerated from empty | identical to the reference tree and the integration tree; zero-fingerprint rows 2206 = 2206 |
| native suite (PE_DISC1_BIN set) | 1701/1701, 0 skipped; `FLASHVEC_cdfe0_update_preserves_known_z` PASS |
| CTest `-E 'route-boot-day2\|^native-tests$'` | 36 passed, 2 skipped (leaf-oracle, akao-tick-oracle), 0 failed, of 38 |
| psyq_port_guard default / strict | PASS (976 known) / FAIL (976), both finding sets identical to c0c298fc's |
| retail_data_guard --strict | OK on the 452 files changed since db9ca12b and the 5 files changed since c7254fb5 |
| Day-1 route, pad only, defaults | FAIL at eve1_victory, game over at frame 44602 (inherited); route.log `be58ad31…` byte-identical to c0c298fc's and to the 049784820 reference; 0 behavioural differences against all 10 reference logs; DAY1_SHIM 0 |
| movies_day2_check WITH the harness route aids (coverage only, not play evidence) | MOVIES_DAY2=PASS (5 movies, 0 dropped). Flash approximations 2 -> 0: both of c0c298fc's hits named 800CDFE0. Behaviour lines 1264 = 1264, identical. The only other log difference is the final GPU counter: 64 fewer polygons and draw-mode writes, identical polygon pixels |

Gate checklist: 5 of 7 green, so this is not a gated port commit. `day1_end_80` FAILs (inherited,
byte-identical to c0c298fc), and the movies run passes only with the harness aids.

## What this does not show

- That the flash is now drawn where retail draws it: there is no oracle frame for it. Zero hits only shows the
  model no longer falls back on these two routes.
- Anything about later play: the pad-only route still ends at the inherited 44602 game over, and the movies
  route uses the harness HP lock and auto-fight.

## Reproduce

Scripts (copies of the integration packet's, paths changed; outputs local, not committed) are in
`/home/blizz/scratch/pe-cdfe0-20261020`: `build/setup-worktree.sh`, `build/gen.sh`, `build/build-and-gate.sh`
(JOBS=4, nice 19), `route/stage-kit.sh` (kit `pe-route-cdfe0-9f13cb73.tar.gz`, sha256 `2bb68efe…`), then
`route/day1/run-launch.sh selftest|run`, `route/movies/run-launch.sh selftest|run`, `route/compare.py` and
`route/compare-movies.py`. Gate log `gates/STEPS.txt` sha256 `93a06c8c…`; route logs `be58ad31…` (Day 1) and
`74053878…` (movies).
