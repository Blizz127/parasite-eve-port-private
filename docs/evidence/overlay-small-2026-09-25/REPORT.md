# Overlay small leaves — 2026-09-25

Twenty overlay bodies carved after `era_link_check.py --target <id>` printed
`LINK_EXACT` at the full declared size, era `-O2 -G0` (cc1 2.7.2). Each then
passed `disc1_preflight.py --target <id> --deep --only <func>`.

`leaf_strong_check.py` is the EXE checker (it requires a final row at
`0x1EE800`), so these leaves are proven by the overlay link check plus the
deep size check.

| target | function | size | shape |
| --- | --- | --- | --- |
| room_m0137i | func_8018F070 | 0x8 | `return 0` |
| room_m0141i | func_8018F088 | 0x8 | `return 0` |
| room_m0152i | func_8018F218 | 0x8 | `return 0` |
| room_m0203i | func_8018F220 | 0x8 | `return 0` |
| room_m0231i | func_8018F22C | 0x8 | `return 0` |
| room_m0017i | func_8018F1AC | 0x10 | `*a0 = 4; return 0` |
| room_m0272i | func_8018F178 | 0x10 | `D_801991C8 = a3; return 0` |
| room_m0273i | func_80192E4C | 0x18 | zero `+0xC/+0x10/+0x14` words and `+0x20` half |
| ovl_039F | func_8010BF94 | 0xC | `return *a0` (`lhu`) |
| room_m0418i | func_80192C78 | 0xC | two zero halfword stores |
| ovl_039F | func_8010C430 | 0x18 | `return *D_8010DB50` |
| room_m0273i | func_80199F70 | 0x14 | store `a2` at `D_8019AE98`, return `p - 6` |
| room_m0350i | func_80199E3C | 0x14 | store `a1` at `D_8019A85E`, return `p - 0x15` |
| room_m0350i | func_8019A120 | 0x14 | store `a1` at `D_8019A8B8`, return `p - 0x24` |
| room_m0418i | func_80191830 | 0x20 | zero shorts at 0,1,2,5,6 and `0x400` at index 4 |
| ovl_039F | func_8010C01C | 0x20 | call `func_8010C27C` |
| room_m0418i | func_80191990 | 0x20 | 16 halfwords at `+0x1C0`, pointer steps by 2 |
| room_m0418i | func_80192788 | 0x20 | same walk as `func_80191990` |
| room_m0418i | func_80192D70 | 0x20 | 0x20-byte zero fill |
| room_m0418i | func_80191970 | 0x20 | if halfword at `+2` >= 0x79, byte at `+1` = 2 |

Captured link lines are `linked .text … word mismatches=0` / `LINK_EXACT` for
every row. Sources live in `src/overlays/<target>/`.
