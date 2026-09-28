# func_80070D6C — bounded register-home/scheduling residual

> Retail instruction words and byte strings in this file are elided (`xxxxxxxx`, `<N bytes …>`): no game data is committed. Read them from your own disc (`tools/extract/disc_cache.py`, EXE `SLUS_006.62`).

Status: `PARKED-REGISTER-HOME-AND-CROSS-BLOCK-SCHEDULING`.

Disposition: `ACCEPTED-RESIDUAL`. This is evidence disposition only, not a
byte-identity claim. The leaf remains assembly and does not increment the
matching-C count.

## Function hood and boundaries

The exact span is file `[0x6156C,0x615D0)`, VRAM
`[0x80070D6C,0x80070DD0)`: `0x64` bytes / 25 words. It ends with `jr ra` at
`0x80070DC8` and a live cursor store in the delay slot at `0x80070DCC`.

Four distinct raw direct calls target the exact start (the old pool's `3/3`
screen missed the adjacent wrapper call):

```text
caller PC   file off  jal word
80017728    007F28    0C01C35B
8003E6BC    02EEBC    0C01C35B
8003E924    02F124    0C01C35B
80070DD4    0615D4    0C01C35B
```

The preceding real initializer ends at file `0x61564/0x61568` with `jr ra`
and its live store delay slot. The following ranged-RNG wrapper begins at
file `0x615D0` with `or v1,zero,ra`. Both boundary sides are executable.

```text
FUNCTION_HOOD=PROVEN_BY_4_DIRECT_CALLERS_AND_CANONICAL_RETURN
```

## Frame decomposition and screens

There is no stack frame, saved register, callee, relocation, or loop. The
function is a no-argument leaf returning the full 32-bit sum in `$v0`.

| screen | retail result | consequence |
|---|---|---|
| callee buckets | zero `jal` in body | no callee return/clobber analysis |
| frame | no stack adjustment or saves | pure leaf; no prologue lever |
| coloring pressure | live bases in `t0/t7/t8`, cursors `t1/t2`, slots `t3/t4`, values `t5/t6` | unusually complete `$t`-register home plan |
| `$v0` liveness | sum copied `t5 -> v0` before cursor-update branches | return home deliberately separated from sum home |
| address retention | three absolute bases retained; computed `t3` survives load/add/store | address lifetimes are central |
| addressing temp | no symbolic relocation or `$at`; fixed addresses use destination registers | no maspsx 3-word/temp gate applies |
| `-O` signal | both branch slots and return slot filled; load-delay nop only when unavoidable | era `-O2 -G0` is the justified probe |
| back-edge owner | none | straight-line two-guard CFG |

## State provenance and Stage-0 census

Independent retail/native evidence in `pc_port/tools/rng_oracle.py`,
`pc_port/game/boot/func_80070D6C_port.c`, and the BTL0 RNG reports proves a
lagged-Fibonacci advance:

- `0x80070E04`: signed write cursor;
- `0x80070E08`: signed read cursor;
- `0x80070E0C`: start of the 17-word state table;
- add the two selected words, write the sum back through cursor 1, decrement
  both byte offsets, wrap cursor 1 to `0x40`, wrap cursor 2 with `| 0x40`,
  store both cursors, and return the sum.

The odd cursor-2 OR is retail behavior. Its reachable cycle can read through
the routine/wrapper tail immediately before the table; the C candidate does
not “repair” that behavior.

A full-executable exact-address scan finds zero raw pointer words and only
these same-register `lui/ori` constructions:

```text
address     construction PCs
80070E04    80070D48 (initializer), 80070D74 (this leaf)
80070E08    80070D50 (initializer), 80070D7C (this leaf)
80070E0C    80070D10 (initializer), 80070D6C (this leaf)
```

Thus Stage-0 closes locally: `func_80070D10` initializes all three regions;
`func_80070D6C` is their sole advance/read-write routine; the adjacent
`func_80070DD0` consumes its return but reaches state only through this call.

## Retail body (all 25 words)

```text
off  word      instruction
00   xxxxxxxx  lui   t0,0x8007
04   xxxxxxxx  ori   t0,t0,0x0E0C
08   xxxxxxxx  lui   t7,0x8007
0C   xxxxxxxx  ori   t7,t7,0x0E04
10   xxxxxxxx  lui   t8,0x8007
14   xxxxxxxx  ori   t8,t8,0x0E08
18   xxxxxxxx  lw    t1,0(t7)
1C   xxxxxxxx  lw    t2,0(t8)
20   xxxxxxxx  addu  t3,t0,t1
24   xxxxxxxx  addu  t4,t0,t2
28   xxxxxxxx  lw    t5,0(t3)
2C   xxxxxxxx  lw    t6,0(t4)
30   xxxxxxxx  nop
34   xxxxxxxx  addu  t5,t5,t6
38   xxxxxxxx  sw    t5,0(t3)
3C   xxxxxxxx  or    v0,zero,t5
40   xxxxxxxx  addiu t1,t1,-4
44   xxxxxxxx  bgez  t1,0x50
48   xxxxxxxx  addiu t2,t2,-4
4C   xxxxxxxx  ori   t1,zero,0x40
50   xxxxxxxx  bgez  t2,0x5C
54   xxxxxxxx  sw    t1,0(t7)
58   xxxxxxxx  ori   t2,t2,0x40
5C   xxxxxxxx  jr    ra
60   xxxxxxxx  sw    t2,0(t8)
```

## Two bounded C phrasings

Attempt 1 used three natural pointer bases and one expression for the two
table loads. Attempt 2 exposed the integer table base, both slot pointers,
both loaded values, and the final sum separately—the closest direct mapping
of retail's `t3/t4/t5/t6` dataflow:

```c
unsigned int func_80070D6C(void)
{
    unsigned int table = 0x80070E0C;
    int *cursor1 = (int *)0x80070E04;
    int *cursor2 = (int *)0x80070E08;
    int index1 = *cursor1;
    int index2 = *cursor2;
    unsigned int *slot1 = (unsigned int *)(table + index1);
    unsigned int *slot2 = (unsigned int *)(table + index2);
    unsigned int value1 = *slot1;
    unsigned int value2 = *slot2;
    unsigned int value = value1 + value2;

    *slot1 = value;
    index1 -= 4;
    index2 -= 4;
    if (index1 < 0) {
        index1 = 0x40;
    }
    if (index2 < 0) {
        index2 |= 0x40;
    }
    *cursor1 = index1;
    *cursor2 = index2;
    return value;
}
```

Both use era GCC 2.7.2 `-O2 -G0`, maspsx ASPSX 2.21 with the standard
`--dont-expand-li`, and GNU MIPS-I `as`. No pin, inline asm, volatile ordering
trick, mismatch-hiding macro, or extra nop was tried.

## Object result and residual

Attempts 1 and 2 are byte-identical. Each emits 24 words / `0x60` bytes:

```text
3C028007 34420E0C 3C088007 35080E04 3C078007 34E70E08
8D060000 8CE50000 00C22021 24C6FFFC 00A21021 8C830000
8C420000 24A5FFFC 00621021 04C10002 AC820000 24060040
04A10002 00000000 34A50040 AD060000 03E00008 ACE50000
```

The first word differs and positional comparison is `0/24`; retail has the
additional 25th content word. The differences are nevertheless precisely
structural, not semantic:

- cc1 uses `v0/t0/a3` for the three fixed bases and `a2/a1/a0/v1` for state,
  whereas retail keeps the complete operation in `t0–t8` until the explicit
  result copy;
- cc1 decrements cursor 1 while constructing/loading cursor 2's slot and
  decrements cursor 2 in that second load gap;
- retail preserves an explicit load-delay nop, then uses cursor-2 decrement,
  cursor-1 store, and cursor-2 store in the three later delay slots;
- cc1 computes the sum directly in `$v0`, so retail's `or v0,zero,t5` is
  absent; and
- cc1's `index1 = 0x40` becomes `addiu`, while retail's hand-shaped register
  assignment is `ori`.

Exposing the retail two-load/two-value shape changes none of these decisions.
Further source perturbations would exceed R6 or become R7 mismatch hiding.

```text
RESIDUAL_MECHANISM=REGISTER_HOME_AND_CROSS_BLOCK_SCHEDULING
FUTURE_UNBLOCKER=a compiler/allocation-scheduler lever that reproduces the
  t0-t8 home plan and declines the two early cursor-decrement steals
```

The candidate is preserved in labeled stash
`park func_80070D6C register-schedule residual`; both generated attempt
objects remain under `build/attempts/func_80070D6C/` locally. There is no
YAML/build/verifier integration. The exact matching-C count remains 336.
