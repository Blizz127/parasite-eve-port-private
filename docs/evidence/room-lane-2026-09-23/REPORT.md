# Room lane: hand-written room leaves (2026-09-23, overlays2)

Each leaf below is hand-written C for one room target. It is `LINK_EXACT` at its room VMA
(`python3 tools/analysis/era_link_check.py --target room_<r> src/overlays/room_<r>/<f>.c <vram> <size> -O2 -G0`)
before it is carved (`tools/analysis/room_lane/addleaf.sh`). Each room's
`disc1_preflight.py --deep --target room_<r>` and `scripts/exact_rebuild_overlay.sh room_<r>`
cover it (gate lines in `docs/evidence/room_<r>-ports-2026-09-23/REPORT.md` and in
`docs/ai_context/OVERLAY_LANE.md` §4.4). Every leaf here then acts as a port source for the other
room texts (`tools/analysis/room_lane/port.py`). All use the default profile `era_o2_g0`.

## Levers found in room code

* **Copy-pointer local**: `T *u = t;` declared with an initializer, with the first block using `t` and later
  blocks using `u`. This reproduces retail's `move $5,$6` placed after the first load (timer family
  `func_8018F4F8` and its twins, `func_80190AC4`, `func_80192EC8`, `func_8018F470`, `func_8018FB44`). It closed
  a park that assigning `u = t` after the first block could not.
* **A `short t` local gives the 8-byte leaf frame** (`addiu $sp,-8` with no stack use): `t = p->f; if (t > K) p->f = t - K;`
  (`func_80190128`, `func_801904DC`, `func_8018FDCC`, `func_8018FD70`). Two such locals give 16 (`func_80190D3C`).
* **Flag clear before the count test**: retail clears the flag even when the count is non-zero
  (the store sits in a non-annulled delay slot), so the source is `if (flag == 1) { flag = 0; if (count == 0) ... }`.
* **`if (r != 1) { if (r < 2) {...} } else call`** reproduces a `==1` / `<2` / `!=0` tree with the call block
  last (`func_8018F500` family). A `switch` picks pivot 0.
* **An `int` temporary for a sign-extended short read** gives `lh`; a direct short-to-short copy gives `lhu`
  (`func_80192230`, `func_8018F854`).
* **Prototype-driven zero-extension**: `extern void f(unsigned short, unsigned short)` with both calls as
  arguments reproduces the late `andi 0xFFFF` in the delay slot (`func_8018F980`).
* **Unused 16-byte VECTOR copy**: `s2 = s; ScaleMatrix(&m, &s2);` reproduces retail's stack copy
  (`func_801909D8` family). Local declaration order sets the stack layout.
* **Pointer-to-volatile** for a double read of one global (`func_8018F3A4`), and a volatile field for
  store-before-load order (`func_8018FDF8`).

| target | leaf | offset, size | what |
| --- | --- | --- | --- |
| room_m0005i | `func_80190A6C` | 0x1A84 0x2C | When a0 == 1 set +0x12 = 1 on *D_800E2368; return D_80190B80. |
| room_m0014i | `func_80193148` | 0x4160 0x24 | Store a1..a3 into D_80193288..D_80193290; the first store goes through the returned pointer. |
| room_m0049i | `func_8018F094` | 0xAC 0x28 | func_800C2414(o, D_80192B7C); return 0. |
| room_m0049i | `func_8018F0BC` | 0xD4 0x70 | Two lookups OR-ed in one expression; on -1 call func_8018F12C; return 0. |
| room_m0075i | `func_8018F284` | 0x29C 0xC4 | Two lookups gated on func_800C6CE0 and the ctx mode/ready bytes; on -1 call func_8018F348. |
| room_m0075i | `func_8018F71C` | 0x734 0x80 | Countdown timer (see room_m0167i func_8018F4F8). |
| room_m0075i | `func_8018F79C` | 0x7B4 0x78 | Copy two func_800C2B50() vectors (+0x58.., +0x38..) to shorts; +0x10 = 1024. |
| room_m0075i | `func_8018FB44` | 0xB5C 0xA4 | Effect step keyed on q+2 windows (1-7, 21-49, 51+); done at 58. |
| room_m0075i | `func_8018FF10` | 0xF28 0x64 | Effect step keyed on q+2: before 16 grow +0x10 and shrink +0x12; done at 16. |
| room_m0075i | `func_80190AF4` | 0x1B0C 0x60 | Effect draw setup via func_800C2EAC/2FF0/3098/3238, then func_800C5A40 on +0xAC and +0x78. |
| room_m0104i | `func_8018FDF8` | 0xE10 0x44 | 16-byte slot setter; volatile h0 + volatile D_800942EC keep retail's store-before-load order. |
| room_m0126i | `func_8018FCC0` | 0xCD8 0x1C | Store a two-word pair; the first store goes through the returned pointer. |
| room_m0137i | `func_8018F428` | 0x440 0x80 | Countdown timer: decrement count; when flag == 1 clear it and, at count 0, reload and call func_800C6C18; flag +1 = 2 when func_800C2B68() == 1. |
| room_m0137i | `func_8018F4A8` | 0x4C0 0x1C0 | Spawn a spark at the linked model's transformed offset D_8018EFF4, with random or model-derived velocity; register via func_800C6800(o, 1411, p). |
| room_m0137i | `func_8018F668` | 0x680 0x1D0 | Draw a stretched sprite (+0x10 height) and its floor shadow at D_800942EC. |
| room_m0137i | `func_8018FA14` | 0xA2C 0x18 | Seed +0x10 = 600, +0x12 = 128, clear +0x15. |
| room_m0137i | `func_8018FA2C` | 0xA44 0x12C | Draw setup: rotation D_8018F014, scale (+0x10, +0x10, 4096), translate +0, brightness +0x15*2 into D_80190F60. |
| room_m0137i | `func_8018FB58` | 0xB70 0x84 | Effect fade step: +0x10 += 3; past the threshold +0x16 += 8, +0x15 = +0x16 >> 4, done at 8. |
| room_m0137i | `func_8018FBDC` | 0xBF4 0x14 | Seed +8 = 200 and +0xA = 128 in the effect block. |
| room_m0137i | `func_8018FBF0` | 0xC08 0x124 | Draw setup: rotation D_8018F014, uniform scale +8, translate +0, func_800C42A4(&D_80190F80, &m, 0). |
| room_m0137i | `func_8018FE18` | 0xE30 0x188 | Drops init: identity matrix scaled by D_8018F01C, 6 active drops with random velocities. |
| room_m0137i | `func_8018FFA0` | 0xFB8 0x110 | Draw 6 drops: sprite at each active position, then a quarter-size shadow at floor height D_800942EC. |
| room_m0141i | `func_8018F1BC` | 0x1D4 0x60 | State 4; if func_800C6CE0(o) >= 2 clear bits 24-29 of the ctx word. |
| room_m0141i | `func_8018F470` | 0x488 0x10C | Timer variant: at count 0 and flag 1, once the ctx actor reports 3/2, reload and raise the scene flags; else count down. |
| room_m0141i | `func_8018F57C` | 0x594 0x80 | Effect init at +0x108.. then func_800C4E50(p + 0x108). |
| room_m0141i | `func_8018F874` | 0x88C 0xDC | Effect update: +0x122 decays by 32 after frame 9, +0x120 drifts by +0x124 >> 4, then the func_800C6B90 probe. |
| room_m0141i | `func_8018F950` | 0x968 0x54 | Copy the func_800C2B50() position (+0x18..+0x20) to shorts, +8 = 1024. |
| room_m0141i | `func_8018F9A4` | 0x9BC 0xEC | Draw setup: identity MATRIX scaled by D_8018EFFC, translated to the func_800C2B50() position, then func_800C42A4. |
| room_m0141i | `func_8018FA90` | 0xAA8 0x1C | Flag +1 = 2 when +2 == 58. |
| room_m0141i | `func_8018FAAC` | 0xAC4 0xC4 | Init 16 particle positions at the func_800C2B50() point and unit directions via func_80077CF4/func_80077DC4. |
| room_m0141i | `func_8018FDCC` | 0xDE4 0xDC | Particle burst step for 16 particles (spread by +0x106, rise +8, fade via +0x104/+0x105). |
| room_m0141i | `func_8018FEA8` | 0xEC0 0x74 | Fill 16 short vectors with the func_800C2B50() position; +0x80 = 2048, +0x82 = 128. |
| room_m0141i | `func_801900B4` | 0x10CC 0x1C4 | Burst init: 6 particles at the func_800C2B50() point with random spread along a random angle, upward speed and gravity 500. |
| room_m0141i | `func_80191F38` | 0x2F50 0xA4 | Three-slot parameter setter (switch on unsigned a0; defaults 8 / 10 for a zero third word). |
| room_m0145i | `func_801924F8` | 0x3510 0x64 | Behaviour init (bytes -1/-1/-1/3, fn = func_801927C8) plus ctx flag bits 0x10002 / 0x400. |
| room_m0145i | `func_8019255C` | 0x3574 0x114 | Message handler (switch on unsigned a2: 25 link, 4, 0 actor search then func_80192CBC on miss, 12). |
| room_m0145i | `func_801927C8` | 0x37E0 0x134 | Follow the linked actor at +0x1C (copy position and rotation); switch behaviour when it reaches state 7; model flags via func_80030220. |
| room_m0167i | `func_8018F1EC` | 0x204 0x2C | Store the D_80193208 pointer through func_800C22F8's result; return 0. |
| room_m0167i | `func_8018F218` | 0x230 0x2C | Forward six arguments to func_800C2AF0; return 0. |
| room_m0167i | `func_8018F244` | 0x25C 0x48 | If func_800C6CE0(o) >= 2 (unsigned), func_800C2414(o, D_80193190); return 0. |
| room_m0167i | `func_8018F28C` | 0x2A4 0x8C | Two lookups when func_800C6CE0(o) >= 2; on -1 call func_8018F318; return 0. |
| room_m0167i | `func_8018F318` | 0x330 0x84 | State 4; if func_800C6CE0(o) >= 2 clear bits 24-29 of the ctx word and set its +0x18 byte to 4. |
| room_m0167i | `func_8018F4F8` | 0x510 0x80 | Countdown timer: decrement count; when flag == 1 clear it and, at count 0, reload and call func_800C6C18; flag +1 = 2 when func_800C2B68() == 1. |
| room_m0167i | `func_8018FD70` | 0xD88 0xF8 | Update 12 drip particles (state, fade cap, fall by +0x20, age counter; done when +0x26 reaches 0). |
| room_m0174i | `func_8018F254` | 0x26C 0xAC | Register D_80196CE0 through func_800C22F8 and seed the room light globals; return 0. |
| room_m0174i | `func_8018F5BC` | 0x5D4 0xC | Flag +1 = 2. |
| room_m0174i | `func_8018F5C8` | 0x5E0 0x3C | Clear +0, set +2 from *func_800C2B10(4). |
| room_m0174i | `func_8018F60C` | 0x624 0x70 | Store +2 through func_800C2B10(4), func_800C2B90(o, 2, ...), flag +1 = 2. |
| room_m0174i | `func_8019001C` | 0x1034 0x10 | Clear +2, set +4 = 128. |
| room_m0174i | `func_80190128` | 0x1140 0x54 | Frame counter at +2 (done after 20) and a -8 fade at +4 down to 8. |
| room_m0174i | `func_801901DC` | 0x11F4 0x64 | When func_800C6CE0(o) == 3, clear bit 30 of the ctx word and func_800C2414(o, D_80196DA4). |
| room_m0174i | `func_80190680` | 0x1698 0x80 | Countdown timer: decrement count; when flag == 1 clear it and, at count 0, reload and call func_800C6C18; flag +1 = 2 when func_800C2B68() == 1. |
| room_m0174i | `func_801909B8` | 0x19D0 0x20 | Seed an effect block (+0x10 = 5144, +0x12 = 255, clear +8..+0xC). |
| room_m0174i | `func_801909D8` | 0x19F0 0xEC | Draw setup: rotation from +8, uniform scale from +0x10, translate to +0, then func_800C42A4(&D_801973E8). |
| room_m0174i | `func_80190AC4` | 0x1ADC 0x80 | Effect step keyed on +0x14 (+/-200 on +0x10), +0x12 -= 16 (floor 0), done after frame 21. |
| room_m0174i | `func_80190B44` | 0x1B5C 0x24 | Seed an effect block (+0x10 = 2772, +0x12 = 255, clear +8..+0xC and +0x14). |
| room_m0174i | `func_80190D4C` | 0x1D64 0x58 | Effect step: +0x10 += 200, +0x12 -= 16 (floor 0), +0x14++, done at 8. |
| room_m0174i | `func_80191200` | 0x2218 0x50 | Effect step: +0x10 += 100, +0x12 += +0x16, +0x16 += 50, +0x14 -= 16; done below 0. |
| room_m0174i | `func_80191250` | 0x2268 0x94 | Init 8 particle slots (flags 1, alpha 128, height from +2) and the fall vector. |
| room_m0174i | `func_80191D3C` | 0x2D54 0x20 | Seed an effect block (+0x10 = 2848, clear +8..+0xC, +0x12 = 255). |
| room_m0174i | `func_80191D5C` | 0x2D74 0xEC | Draw setup (as func_801909D8 with func_800C3098(16) and D_801973F8). |
| room_m0174i | `func_80191E48` | 0x2E60 0x40 | Effect step: +0x12 -= 8, +0x10 += 30, +2 -= 15; done when +0x12 < 0. |
| room_m0174i | `func_80192EC8` | 0x3EE0 0x6C | Effect step: +0x30 -= 150 (floor 400); after frame 8 +0x34 -= 16, done below 0. |
| room_m0174i | `func_801933F8` | 0x4410 0x90 | Advance 4 particles by velocity, fade -8 (floor 0); done after frame 61. |
| room_m0174i | `func_801934A0` | 0x44B8 0x28 | Clear the header word and the 8 flag bytes at +0xB4. |
| room_m0186i | `func_8018FEE8` | 0xF00 0x48 | If func_800C6CE0(o) == 3, func_800C2414(o, D_801941E0); return 0. |
| room_m0186i | `func_8018FF30` | 0xF48 0x8C | Two lookups when func_800C6CE0(o) == 3; on -1 call func_8018FFBC; return 0. |
| room_m0186i | `func_8019021C` | 0x1234 0x38 | Flag +1 = 2 when func_800C2B68() == 1. |
| room_m0186i | `func_801904DC` | 0x14F4 0xF8 | Particle burst step: spread 8 particles along their directions, gravity 600, fade-out via +0x84/+0x85. |
| room_m0186i | `func_80190D3C` | 0x1D54 0xE0 | Particle step: +0x16 capped at 15, two fades, spread x/z by +0x14, drop +0x10; done after frame 21. |
| room_m0203i | `func_80193FC0` | 0x4FD8 0x10 | Store a1 into D_80194148 through the returned pointer. |
| room_m0245i | `func_801955E4` | 0x65FC 0x10 | Return the address of D_8019581C. |
| room_m0269i | `func_8018F160` | 0x178 0x2C | Set the behaviour to func_8018F18C when the ctx mode byte is 16. |
| room_m0273i | `func_8019A70C` | 0xB724 0x14 | Store a1 into D_8019AF6C.f2E through a field pointer; return the record. |
| room_m0358i | `func_80192550` | 0x3568 0x74 | Behaviour init (0x400 at +0x88, fn = func_80192814, zero fields). |
| room_m0358i | `func_80192744` | 0x375C 0x6C | func_800DFB78 dispatch (0: o->fn(o); 1: func_80193250). |
| room_m0358i | `func_801927B0` | 0x37C8 0x64 | Range gate on the +0x18C sub-context; sets the behaviour to func_80192814. |
| room_m0358i | `func_80193208` | 0x4220 0x48 | Call func_80193250(o) once the ctx +0x18C counter reaches its byte limit - 1. |
| room_m0358i | `func_80193250` | 0x4268 0x78 | State 4: clear ctx flag bits 0x10000 / 0x400 and set the +0x18C sub-actor byte to 4. |
| room_m0391i | `func_8018F3A8` | 0x3C0 0x144 | Scene-object init: copy the ctx +0x238 block, seed the light/colour globals. |
| room_m0391i | `func_8018F4F4` | 0x50C 0x80 | Countdown timer: decrement count; when flag == 1 clear it and, at count 0, reload and call func_800C6C18; flag +1 = 2 when func_800C2B68() == 1. |
| room_m0399i | `func_8018F1DC` | 0x1F4 0x2C | Store the D_80194208 pointer through func_800C22F8's result; return 0. |
| room_m0421i | `func_8018F050` | 0x68 0x2C | Store the D_80191A70 pointer through func_800C22F8's result; return 0. |
| room_m0421i | `func_8018F210` | 0x228 0x10C | Scene-object init: copy the ctx +0x238 block, seed the colour globals and the func_8006E498 handle. |
| room_m0421i | `func_8018F3A4` | 0x3BC 0x1B0 | As func_8018FBA8, then func_8006DF50(*q, 1406, 0, 128, 127) through a pointer-to-volatile D_800B0E64. |
| room_m0421i | `func_8018F854` | 0x86C 0xF4 | Effect update keyed on the frame counter at q+2 (grow until 32, flag at 16, done at 32). |
| room_m0421i | `func_8018F960` | 0x978 0x20 | Room timer/effect helper (see REPORT). |
| room_m0421i | `func_8018F980` | 0x998 0x1C4 | Fog/light draw: rotation +8, scale (+0x10, 200, +0x10), texture page per r->w24, then the func_800C6xxx render sequence on D_80191BE4. |
| room_m0421i | `func_8018FB44` | 0xB5C 0x64 | Room timer/effect helper (see REPORT). |
| room_m0421i | `func_8018FBA8` | 0xBC0 0x180 | Init two light/fog blocks (+0x108, +0x120) from the func_800C2B10(1..6) colour table (the second at half intensity). |

## Parked (no C in tree; drafts under the lane work dir)

* `m0391i:func_8018FA14`: 233-word 3D routine, not attempted.
* `m0137i:func_801900B0`: drop update loop: retail loads all three velocity halves before any store (vel.y reloaded for +4). No spelling found (locals, volatile).
* `m0137i:func_8018FD14`: load order of `p->h8` vs `r->hA * 6` (38 words).
* `m0137i:func_8018FD14`: load order of `p->h8` vs `r->hA * 6` (38 words).
* `m0137i:func_801900B0`: drop update loop: retail loads all three velocity halves before any store (vel.y reloaded for +4). No spelling found (locals, volatile).
* `m0075i:func_8018FBE8`: constant 128/255 register allocation (22 words).
* `m0075i:func_80190B54`: three `lh` reads of `D_8009D254->h2A` (source gives `lhu`) (3 words).
* `m0141i:func_80190470`: same early-velocity-load pattern as func_801900B0 (33 words).
* `m0145i:func_80192678`: scratchpad VECTOR stores: retail keeps them absolute (`lui $1`) where cc1 reuses the `$s3` base (59 words).
* `m0137i:func_8018F838`: loop pointer copy after the null test (5 words).
* `m0137i:func_8018F230`: constant-register allocation across 40 light-record stores (52 words).
* `m0141i:func_80190278`: constants pinned in saved registers plus mixed absolute/base stores to D_801921B0 (not attempted in full).
* GTE: the 39-text bodies `func_8018F630`, `func_80190370`, `func_80190D6C`, `func_80191100`,
  `func_80191884`, `func_80191D08`, `func_8018FC58`, `func_80192344` (room_m0022i numbering) contain Psy-Q inline
  GTE macros (`ctc2`/`lwc2`/`mvmva`/`swc2`). They are the `handwritten-gte-wrapper` class and not C under the
  current rules.
