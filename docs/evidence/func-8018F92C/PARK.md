# func_8018F92C (ovl_0700, blob 0x93C, 0x6C8) — PARKED (ovl14, 2026-09-28)

View-frustum plane setup. Best: ovl10 draft (`build/lanes/ovl14/drafts/park_func_8018F92C.c`),
**8 word mismatches** = a pure 4-register permutation of four long-lived products
(CA90 row x/y and CC04 row x/z get $30/$23/$22/$21 instead of retail $21/$22/$23/$30).

Cause (cc1 -dS -dg): the four products are global-alloc allocnos with equal ref counts; their
priority is refs/live_length with the *sched1* live lengths (`register 209 life extended from 2
to 39`, 213→37, 221→36, 228→35), so 228 (CC04.z) is allocated first. Retail needs 209 (CA90.x)
shortest, i.e. a different sched1 order for the CA90 row.
Tried: all 24 orders of each 4-statement group, the 14 valid interleavings of both groups,
16 association/parenthesis forms of the CA90/CC04 expressions (`-(a+b+c)` fixes y only, 6w
but changes the negu/subu shape), VECTOR-struct vs int field reads per row (174–378w), pinning
products (114–276w), other cc1 versions (2.6.3/2.7.2-cdk/2.8.x: 269–370w; 2.7.2.3 identical).
