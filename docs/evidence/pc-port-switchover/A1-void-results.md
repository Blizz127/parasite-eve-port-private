# Class A1: void headers for four leaves whose retail $v0 is never read

port3 switch-over, class A1. The canonical pc_port headers of these leaves
returned int, but the matched C is `void`. The headers now follow the matched
C, the generated TUs replace the hand ports, and the port callers and tests
no longer read a result.

| leaf | retail $v0 at `jr ra` | observable instead |
|---|---|---|
| func_80052594 | `subu v0,a1,v0` = bytes copied | stored byte D_8009169D |
| func_800614AC | `or v0,a1,v0` = blended colour | stored word D_8009D150 |
| func_800868F0 | func_8008CBA8's return (tail `jal`) | none (AKAO command queued) |
| func_80086A28 | func_8008CBA8's return (tail `jal`) | none (AKAO command queued) |

## Every retail call site

Tool: `build/lanes/port3/tools/v0_live.py` (not committed; lane tool).

- **What it scans:**
  - The retail EXE, with file offset f mapped to VA 0x8000F800+f.
  - Every PE.IMG blob:
    - blobs with a manifest `vram` use that base;
    - room `*_c2` blobs use 0x8018EFE8;
    - any other blob is scanned without a base.
- **What it looks for:**
  - every `jal <leaf>` word;
  - every aligned data word equal to the leaf address;
  - every `lui`/`addiu|ori` pair that builds the address.

  No data word or address pair was found for any of the four leaves, so none
  of them is called indirectly.
- **How each call is judged:** the tool walks forward from call+8 and follows
  both branch arms. The delay slot runs before the callee, so the walk starts
  after it. The first event decides the site:
  - `$v0` read → LIVE;
  - `$v0` written → DEAD;
  - another call → CALL (the ABI clobbers `$v0`);
  - `jr ra` → RETURNED (the value becomes the enclosing function's result,
    so that function's callers are checked in turn).
- **Validation:** on func_80062A7C the tool reproduces the known RETURNED
  site 0x80044E7C (func_80044E14, batch O).

```
== func_80052594
  jal  exe                      0x8004E28C  dead   ['DEAD'] 
  jal  exe                      0x8004E428  dead   ['DEAD'] 
  jal  exe                      0x8004E6A8  dead   ['DEAD'] 
  jal  exe                      0x8005C46C  REVIEW ['RETURNED'] RETURNED@0x8005C480
  jal  exe                      0x8005D898  dead   ['CALL'] CALL@0x8005D8A0
== func_800614AC
  jal  exe                      0x80043564  dead   ['DEAD'] 
  jal  exe                      0x8004358C  dead   ['CALL'] CALL@0x80043598
  jal  exe                      0x8004B440  dead   ['CALL'] CALL@0x8004B448
  jal  exe                      0x8004B494  dead   ['CALL'] CALL@0x8004B49C
  jal  exe                      0x8004B504  dead   ['CALL'] CALL@0x8004B50C
  jal  exe                      0x8004FF10  dead   ['CALL'] CALL@0x8004FF18
  jal  exe                      0x8005C3C0  dead   ['CALL'] CALL@0x8005C3D0
  jal  exe                      0x8005D8C8  dead   ['CALL'] CALL@0x8005D8F0
== func_800868F0
  jal  exe                      0x80016328  dead   ['DEAD'] 
  jal  ovl_0700.bin             0x80191F98  dead   ['CALL'] CALL@0x80191FA8
== func_80086A28
  jal  exe                      0x800163BC  dead   ['DEAD'] 
  jal  ovl_0700.bin             0x80191FA8  dead   ['CALL'] CALL@0x80191FBC
```

- func_80052594 @0x8005C46C is RETURNED. It returns into func_8005C374,
  whose matched C (`src/func_8005C374.c`) is `void`. func_8005C374 has one
  call site, and `$v0` is dead there:

```
== func_8005C374
  jal  exe                      0x80042400  dead   ['DEAD']
```

## Leaf tails

Command:
`mips-linux-gnu-objdump -D -b binary -m mips:3000 -EL --adjust-vma=0x8000F800 build/extracted/disc1/SLUS_006.62`

```
func_80052594:  800525dc addiu v0,v1,-9 / 800525e0 subu v0,a1,v0 / 800525e4 jr ra / 800525e8 sb v0,0(v1)
func_800614AC:  80061530 or v0,a1,v0   / 80061534 jr ra / 80061538 sw v0,0(a3)
func_800868F0:  80086930 jal 0x8008cba8 / ... / 80086940 jr ra
func_80086A28:  (same shape, command 0xA2)
```

## Inertness

`gen_decomp_ports.py` was run on 13b1f9ab with and without the change.
`diff -rq` of `pc_port/game/decomp/` shows only the four new TUs; every
other generated TU is byte-identical.

## Not in A1

- func_8003AF14: `$v0` is dead at both call sites, but its hand port differs
  structurally from the matched C, so switching it changes render behaviour.
  It needs its own batch.
- func_80086464, func_80086608, func_80087090: `$v0` IS read (LIVE sites
  0x80015E90/0x8006DAC0, 0x8006DDA0 plus func_8006DF50's return, and
  0x8006CF98). These need a `$v0`-passthrough rule (A2).

## Port callers that read a result

Retail never reads these results, so any port caller that did was itself
diverging. Each one now takes the value from memory, where retail leaves it:

- `pc_port/game/boot/func_8005D6F4_port.c`: `r = func_80052594(src); (void)r;`
  becomes a plain call, since r was never used.
- Tests in `pc_port/tests/test_native.c`:
  - The 52594 tests read the count byte D_8009169D through
    `B27_RUN() = (func_80052594(B27_SRC), PE_LoadU8(B27_COUNT))`.
  - The 614AC tests read D_8009D150 through
    `(func_800614AC(x), PE_LoadU32(0x8009D150u))`.
- `pc_port/tests/test_transition_sound.h`: the `== 0` result asserts on
  868F0 and 86A28 are dropped. These tests pin the FIFO/state hashes, not a
  return value.

The remaining port callers already discard the result, some with `(void)`:
func_8005C374 (generated), func_8004FEEC (generated), absent_lo_menu,
func_8004B394, func_80015DAC cases 302/306 and func_80191DE8.
