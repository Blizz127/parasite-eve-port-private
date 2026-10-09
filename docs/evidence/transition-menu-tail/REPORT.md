# M0000I world-map menu-list tail (func_801942FC)

## Stop
The walked Day 3 route (m0093i, Daniel's car at story 0x148) entered the world
map M0000I and stopped on the first frame:
`[STOP] reason=unresolved-boundary` in PE_TransitionUpdate <- func_8019234C.
func_801942FC passed `menu_tail=NULL` because the original menu list
(801942FC sp+136, five halfwords) initializes only list[0]; list[1..4] are
stack residue that func_800375E0 turns into numeric message arguments.

## Where the residue lives
8001220C moves sp onto the scratchpad (sp = 1F8003F8) before calling the
M0000I loop 8019234C, so the loop frame is S = 1F8003C8 and the tail is
S-54..S-47 = scratchpad 1F800392..99.  The loaded overlay initializes
8019C018 = 1, so the menu path runs on the very first loop frame.

## Last writers (static proof, `pc_port/tools/pe_menu_tail_audit.py`)
* First frame: constructor 80196498 (frame 240) leaves its record offset at
  sp+184 (= S-56) = 52*10 = 520 and its position cursor at sp+192 (= S-48) =
  positions+84 => tail0 = 0, tail3 = lo16(positions+84).  It never writes
  sp+188..191 (tail1..2).  VSync(-1) returns before its 80073BBC helper;
  80191DE8, 80071A54/64 are stack-clean.  tail1..2 are therefore older
  scratchpad residue: power-on contents, or the previous visit's exit
  selector 80192030 register saves.  They stay **unknown**.
* After any draw pass: PutDrawEnv 80075424(env) -> libgpu table[2] 80076C34
  saves s0 = env+28 and s1 = env at S-56/S-52; DrawOTag 800752AC(ot) ->
  table[11] 80076354 saves s0 = ot at S-48.  ClearOTagR's 80076C34 sits at
  S-64 and PutDispEnv's table[4] 80076B20 is a leaf.  8019BF8C and the steady
  input path (8003EB04 -> 800825C0 -> leaf 80084B20) never reach S-56..S-45.
  All four halves are known.
* Input actuator path 800828F4 -> 80083BB8 saves ra = 8008291C at S-56 =>
  tail0 = 0x8008.
* Interrupts run on libetc's own stack (jmp_buf sp = 800955FC), the BIOS
  handler on its own; neither touches the scratchpad stack.
* SetGraphDebug level >= 2 inserts printf jalr calls: treated as unknown.
* The button exit path (held 0x0F000006) clobbers: treated as unknown.

## Native model (exact or stop)
* func_8019234C resets the tail state on entry; func_80196498 records its
  two locals; each draw pass records env/ot; the 800828F4 call counter marks
  the actuator write.
* Known halves are passed as values.  Unknown halves are passed to
  `PE_MessageOpenPartial`, which writes known digit groups exactly and marks
  the unknown group and every later one (an unknown value may be the -1
  terminator) unknown in a native side table.
* Only text code FB 08 reads digit groups (func_80037870); it stops with
  unresolved-boundary if it would draw an unknown group.  No digits are
  invented.

## Result
Re-running the walked route (m0092i@0x140 -> m0093i 0x148 -> gun shop ->
pharmacy -> car) now reaches M0000I and runs about 11,000 world-map frames to
the frame limit without a stop.  The location-label text never consumes the
unknown groups.  Test `message_partial_open` covers the side-table rules.
