/* SUPERSEDED 2026-10-09 01:27 CT: matched and installed at
 * src/overlays/room_m0350i/func_80192E4C.c (LINK_EXACT, profile
 * room_m0350i_narrow_any_dest with K3/K3b/K3c). Day 6 stop cleared.
 * See docs/evidence/day2-warp-probe/REPORT.md.
 *
 * Final lever (kept here as a note): store D_800F3368.f8 first, then
 * f4=3 / f6=1, then an always-false `if (f6 == 0 && f4 == 3) return 0;`
 * so CSE bases f2 on $s0=&f8 and sched places lh -6($s0) after the
 * outgoing-arg andi/sw. Do not restore this draft.
 */
