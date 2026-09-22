# LOCALLY MAINTAINED — tracked in the Parasite-Eve-Decompilation repo.
# Vendored from upstream: https://github.com/mkst/maspsx
# This file is un-ignored via .gitignore negations under tools/era/;
# scripts/setup_era.sh re-clones upstream AROUND it (never overwrites it,
# restores it from git if absent). Local patches are marked "LOCAL PATCH".
# Local patch log:
#   1. fill_store_delay_slot (ctor arg, or env MASPSX_FILL_STORE_DELAY_SLOT=1 —
#      the env var keeps the untracked maspsx.py driver unpatched): schedule an
#      absolute `sw $r,SYM` store macro into the delay slot of a following bare
#      `j $31`. cc1 cannot schedule the macro (its `lui $at` + `sw` expansion
#      is opaque to cc1's delay-slot filler), so it emits `sw` / `j $31` with an
#      empty slot; ASPSX expands the macro and moves the trailing store into
#      the slot:  lui $at,%hi(SYM) / j $31 / sw $r,%lo(SYM)($at).
#      ROM evidence (Parasite Eve disc1): 14-member sw delay-slot family
#      (func_8007FBC0 et al) fills; sb/sh macro stores and multi-store
#      epilogues stay pre-jr with a nop slot. Default OFF: opt-in per leaf.
#   2. three_word_symbol_store (ctor arg, or env
#      MASPSX_THREE_WORD_SYMBOL_STORE=1): for standalone indexed symbolic
#      loads AND stores (lw/lh/lb/... and sw/sh/sb/...) under ASPSX
#      2.21/addiu_at, select the ASPSX 2.30 three-word lui/addu/op-%lo form
#      instead of lui/addiu/addu/op-0. Compound semicolon lines retain the
#      legacy expansion. Default OFF: opt in per leaf.
#   3. fill_epilogue_delay_slot (ctor arg, or env
#      MASPSX_FILL_EPILOGUE_DELAY_SLOT=1): schedule the frame deallocation
#      (`addu $sp,$sp,N` / `addiu $sp,$sp,N`) into the delay slot of a
#      following bare `j $31`. cc1 only fills the return slot itself when no
#      callee-saved restore immediately precedes the jump; with restores it
#      emits the stack adjust before the `j` and GNU as leaves a nop. ASPSX's
#      reorder-mode scheduler moves it into the slot. ROM evidence (Parasite
#      Eve disc1): func_800811E4 (ra/s0/s1 restores) fills at 0x8008124c/50;
#      the 58 G0-matched leaves that already fill do so in cc1 itself (no
#      restores), so this stays opt-in and flag-off is byte-identical.
#      Default OFF: opt in per leaf.
#   4. symbol_load_dest_temp (ctor arg, or env
#      MASPSX_SYMBOL_LOAD_DEST_TEMP=1): for compound indexed symbolic
#      loads of the form `op $d,SYM($b)` under ASPSX 2.21/addiu_at, emit the
#      naive GNU-as expansion using the DESTINATION register as the address
#      temp (lui $d,%hi(SYM) / addu $d,$d,$b / op $d,%lo(SYM)($d)) instead of
#      maspsx's lui $at / addiu $at,%lo / addu $at,$at,$b / op $d,0($at).
#      The same gate also covers an indexed symbolic STORE that immediately
#      precedes a `j $31`: ROM emits the 3-word $at materialization before the
#      jump and schedules the store into the delay slot
#      (lui $at,%hi(SYM) / addu $at,$at,$b / j $31 / op $src,%lo(SYM)($at)).
#      ROM evidence (Parasite Eve disc1): func_80076B44
#      (`lui $v0,%hi(D_800A3348)` / `addu $v0,$v0,$a0` / `lbu $v0,%lo(...)($v0)`)
#      and func_80076B20 (same shape with an `sb` in the jr delay slot).
#      When the destination IS the index register (`lhu $2,SYM($2)`) the
#      dest-temp form is impossible (the `lui` would clobber the index), so
#      the load falls back to the $at form like GNU as: func_80074A44 has
#      both `lhu $v1,D_800957CC($v0)` (dest-temp) and
#      `lhu $v0,D_800957D8($v0)` ($at) in one body.
#      Default OFF: opt in per leaf; flag-off is byte-identical.
#   5. symbol_at_temp (ctor arg, or env MASPSX_SYMBOL_AT_TEMP=1): for compound
#      indexed symbolic loads/stores of the form `op $d,SYM($b)` under ASPSX
#      2.21/addiu_at, emit the 3-word $at form WITH the %lo displacement kept
#      (lui $at,%hi(SYM) / addu $at,$at,$b / op $d,%lo(SYM)($at)) instead of
#      maspsx's legacy 4-word lui/addiu/addu/op-0($at). Unlike patch 4 this
#      uses the assembler temporary rather than the destination register, so
#      it also covers sign-extended loads and narrower stores. Env may also
#      be a comma list of symbol names (e.g. MASPSX_SYMBOL_AT_TEMP=jtbl_80011388)
#      to apply the 3-word $at form to those operands only; other indexed
#      symbolic ops keep the legacy 4-word expansion. ROM evidence
#      (Parasite Eve disc1): func_80042770/func_80042964
#      (lui $at,%hi(D_800A0ED4) / addu $at,$at,$v0 / lbu $v0,%lo(...)($at));
#      4658 non-$at indexed accesses exist in the split asm, most of which are
#      this shape with a wider register pair than patch 4 can reach.
#      Default OFF: opt in per leaf; flag-off is byte-identical.
#   6. fill_branch_delay_slot (ctor arg, or env
#      MASPSX_FILL_BRANCH_DELAY_SLOT=1): fill the delay slot of a REORDER-mode
#      conditional branch with the instruction that follows it, instead of the
#      unconditional `nop`.
#      WHY cc1 leaves the slot empty: gcc's reorg.c refuses any delay-slot
#      candidate that SETS a resource the branch itself NEEDS
#      (`insn_sets_resource_p (trial, &needed, 1)` in fill_slots_from_thread),
#      so a compare that overwrites the register the branch just tested can
#      never be scheduled by cc1 even though it is architecturally legal (the
#      branch latches its operands before the slot executes). Retail's build
#      does perform that fill.
#      WHY THIS IS NARROW: this fill moves an instruction from the
#      fall-through-only position into a slot that executes on BOTH edges, so
#      it is only legal when the instruction's destination register is DEAD at
#      the branch target. ROM evidence: of the 357 reorder-mode conditional
#      branches in the repo's LINK_EXACT leaves, 70 are followed by an
#      instruction that redefines a tested register and retail keeps a `nop`
#      in every one of them (e.g. func_80077DC4 `bgez $4,$L2` / `subu $4,$0,$4`
#      -- an abs() whose $4 is LIVE at the target; filling it would
#      miscompile). The shape alone therefore does NOT license the fill; only
#      liveness does. This patch performs a real, conservative reachability /
#      liveness walk of the assembly from the branch target and refuses the
#      fill on ANY construct it cannot fully account for (calls, indirect
#      jumps, inline asm, reorder-mode branches inside the walked region,
#      unknown mnemonics, function end, a step budget). The candidate itself
#      must be a single-word, non-memory, non-trapping, non-hi/lo ALU
#      instruction from a closed allow-list with an immediate that cannot
#      provoke an `$at` expansion, and must be the immediately following line
#      (no label, directive or comment in between).
#      TWO GATES, both mandatory: a SAFETY gate (the liveness walk above) and
#      a FIDELITY gate (the candidate must be a compare -- slt/slti/sltu/sltiu
#      -- whose result the immediately following conditional branch consumes).
#      The fidelity gate exists because liveness alone licenses 2 fills retail
#      does not take (func_80012E7C `sll $2,$3,2`, func_8006A2E8 `move $2,$5`
#      -- both legal, both a `nop` in ROM).  With both gates on, the knob
#      changes exactly ONE of the repo's 815 matched leaves: func_80043474.
#      The non-compare rows of branch_fill_candidates are therefore inert
#      today; they are the structural single-word analysis that a future
#      widening of the fidelity gate would need, and must not be treated as
#      ROM-evidenced on their own.
#      ROM evidence (Parasite Eve disc1): func_80043474 (band classifier) --
#      `bnez $v0,.L800434B4` / `slti $v0,$a0,0x2D3` at 0x8004349C, where $v0 is
#      killed by the epilogue `addu $v0,$v1,$zero` on the taken path.
#      Default OFF: opt in per leaf; flag-off is byte-identical.
#   7. sized-`.extern` gp classification for the load-delay nop (ALWAYS ON --
#      a correctness fix, not a knob). `preprocess_lines` used to skip
#      `.extern SYM, size` lines, so `_uses_gp` could never recognise a
#      cc1 `-G<n>` external as gp-relative. For `load $r` / `op $r,SYM` where
#      GNU as collapses the macro to ONE gp-relative instruction, the
#      mandatory MIPS-I load-delay nop was then emitted only by the
#      version-gated `nop_at_expansion` clause (ASPSX < 2.30), i.e. for the
#      wrong reason, and silently dropped at >= 2.30. Sized externs that fit
#      `-G<n>` are now recorded in `extern_entries` and consulted by
#      `_uses_gp` only; operand rewriting is unchanged (GNU as already places
#      sized externs gp-relative). Requires the driver to forward cc1's
#      `-G<n>` to maspsx (it did not before; `sdata_limit` was always 0).
#      ROM evidence (Parasite Eve disc1): func_800339A0 0x800339FC
#      `lhu $v0,0x0($v1)` / `nop` / `sh $v0,0x114($gp)`, func_80046DFC
#      0x80046E7C `lw $v0,0x19C($gp)` / `nop` / `sw $v0,0x248($gp)`,
#      func_80012700 0x80012708 `lw $v0,0x24($a2)` / `nop` /
#      `sw $v0,0x8C($gp)` -- the only three era leaves that matched at
#      2.21 but not 2.30 before this fix; all three match at both after it.
#   8. fill_call_delay_slot (ctor arg, or env MASPSX_FILL_CALL_DELAY_SLOT=1):
#      the CALL/JUMP sibling of patch 1. Schedule the second word of an
#      absolute store macro (`sw/sh/sb $r,SYM`) or of an address macro
#      (`la $r,SYM`) into the delay slot of a following plain
#      `jal <symbol>` / `j <label>`:
#        lui $at,%hi(SYM) / jal T / sw $r,%lo(SYM)($at)
#        lui $r,%hi(SYM)  / jal T / addiu $r,$r,%lo(SYM)
#      Why this cannot be done by cc1 (and so has to live here): the `$at`
#      form is only ever written by the assembler, so retail's store was a
#      macro and the fill was ASPSX's reorder pass. Verified: cc1 2.8.1
#      DOES fill jal slots, but only in its default -msplit-addresses mode,
#      where it materializes %hi into a real GPR (`lui $2,%hi(SYM)` /
#      `sw $r,%lo(SYM)($2)`) — never `$at`; under -mno-split-addresses it
#      emits the macro and leaves the jal slot empty, exactly like 2.7.2.
#      No --aspsx-version value changes this either: maspsx forces
#      `.set noreorder` per function and inserts its own branch/jump nop,
#      and config_for_aspsx_version has no scheduling knobs at all.
#      ROM evidence (Parasite Eve disc1, whole-image scan): 90 sites /
#      45 functions — 36 absolute-store splits (16 jal+sw, 19 j+sw,
#      1 jal+sh), 53 `la` splits (49 jal, 4 j) and 1 indexed-store split
#      (not covered by this patch).
#      HAZARDS (each one is a guard below; when a hazard cannot be decided
#      from maspsx's line-oriented view the knob refuses the site):
#        H1 the only instruction that MOVES is the macro's second word, from
#           just before the jump to just after it. `jal` writes $31, so a
#           moved word that READS $31 would read the new return address:
#           refuse when the stored value register is $31/$ra.
#        H2 the `la` arm's moved word WRITES its register. Refuse when that
#           register is $31/$ra (it would destroy the return address the
#           `jal` just stored).
#        H3 register-indirect jumps (`jalr`/`jr`, which cc1 spells `jal $2` /
#           `j $2`) read their target register AT jump time, i.e. before the
#           delay slot executes. They are IN scope — ROM: func_80074BB8's
#           `lui $a0,%hi(D_80011814)` / `jalr $v0` / `addiu $a0,$a0,%lo(...)`
#           — but only when the moved word cannot disturb that register:
#           the `la` arm refuses target == its own destination (the jump
#           would read a half-formed address), and the store arm refuses
#           target == `$at` (clobbered by the surviving `lui`). Register
#           names are compared canonically so `$4`/`$a0` cannot fail open.
#           `j $31` (the return) is always refused: that slot belongs to
#           patches 1 and 3 and their ROM evidence, and stealing it would
#           change their behaviour when several knobs are on.
#        H4 `$at` is written by the `lui` that stays BEFORE the jump and read
#           by the store in the slot. Nothing can clobber it in between: the
#           only instruction in between is the jump, and j/jal write $31
#           only. `$at` is never live across the call because the store
#           retires before control transfers. Still refused if the stored
#           value register is itself `$at`.
#        H5 a label or a `.set` directive between the macro and the jump
#           means the jump is a branch target, or that cc1 already scheduled
#           the slot inside its own `.set noreorder` block. Refused (the
#           lookahead deliberately does not skip labels or `.set`).
#        H6 conditional branches are NOT touched: only `j`/`jal`. A branch's
#           slot is a separate ROM family with its own scheduling rules.
#        H7 compound (`;`) macro lines on either side are refused: their
#           expansion order is not a single instruction pair.
#        H8 gp-relative destinations are a single instruction with no second
#           word to move, so the gp arm is tested first; the `la` arm also
#           refuses outright when gp_allow_la is in effect (ASPSX >= 2.80).
#        H9 numeric operands (`sw $2,56200($4)`, `la $2,-4`) are not
#           symbol macros; refused.
#      Default OFF: opt in per leaf; flag-off is byte-identical.
#   9. explicit `%lo` store passthrough (ALWAYS ON). gcc-2.8.1-psx cc1
#      (per-leaf ERA_CC1_VER=2.8.1) emits absolute stores as an explicit
#      `lui $b,%hi(SYM)` / `op $r,%lo(SYM)($b)` pair; 2.7.2 never does. The
#      `%lo` store is already one instruction with a LO16 reloc, so the store
#      arm now passes it through exactly like the load arm always did instead
#      of re-expanding it into the 4-word $at macro. 2.7.2 output is
#      byte-identical (it never produces the form). Durable test
#      tests/test_lo_store_passthrough.py.
import struct
import os
import re

from typing import List

branch_mnemonics = {
    "beq",
    "bgez",
    "bgtz",
    "blez",
    "bltz",
    "bne",
}
jump_mnemonics = {
    "j",
    "jal",
}
load_mnemonics = {
    "lb",
    "lbu",
    "lh",
    "lhu",
    "lw",
    "lwl",
    "lwr",
}
store_mnemonics = {
    "sb",
    "sh",
    "sw",
    "swl",
    "swr",
    "swc2",
}

single_reg_loads = {
    "mult",
    "multu",
    "div",
    "divu",
    "rem",
    "remu",
    "move",
    "negu",
    "nor",
}
double_reg_loads = {
    "and",
    "andi",
    "or",
    "ori",
    "xor",
    "xori",
    "addu",
    "subu",
    "sll",
    "slr",
    "srl",
    "sra",
    "slt",
    "slti",
    "sltu",
}


# --- LOCAL PATCH 6 (fill_branch_delay_slot) tables -------------------------
#
# Conditional branches, split by how many register operands precede the label.
# Anything not listed here is never treated as a conditional branch by the
# patch (and is refused when met inside the liveness walk).
cond_branch_two_reg = {"beq", "bne"}
cond_branch_one_reg = {"bgez", "bgtz", "blez", "bltz"}
cond_branch_mnemonics = cond_branch_two_reg | cond_branch_one_reg

# Closed allow-list of fill candidates: every entry is a single machine word
# after GNU as / maspsx processing, touches no memory, cannot trap, writes no
# hi/lo and writes exactly its first operand.  (`move` is rewritten to `addu`
# by expand_move; `li` is range-checked so as emits one word.)
#   name -> (operand count, immediate kind)
# immediate kind: None  = all operands must be registers
#                 "s16" = last operand may be a signed 16-bit literal
#                 "u16" = last operand may be an unsigned 16-bit literal
#                 "sh"  = last operand must be a 0..31 shift count
#                 "li"  = a single literal operand (range checked separately)
branch_fill_candidates = {
    "addu": (3, "s16"),
    "addiu": (3, "s16"),
    "subu": (3, "s16"),
    "and": (3, "u16"),
    "andi": (3, "u16"),
    "or": (3, "u16"),
    "ori": (3, "u16"),
    "xor": (3, "u16"),
    "xori": (3, "u16"),
    "nor": (3, None),
    "slt": (3, "s16"),
    "slti": (3, "s16"),
    "sltu": (3, "u15"),
    "sltiu": (3, "u15"),
    "sll": (3, "sh"),
    "srl": (3, "sh"),
    "sra": (3, "sh"),
    "sllv": (3, None),
    "srlv": (3, None),
    "srav": (3, None),
    "move": (2, None),
    "li": (2, "li"),
}

# Registers this patch refuses to treat as a fill destination: the ABI-fixed
# ones (whose liveness the walk cannot reason about) plus the assembler temp.
branch_fill_forbidden_dest = {
    "$0", "$zero", "$1", "$at",
    "$26", "$27", "$k0", "$k1",
    "$28", "$gp", "$29", "$sp", "$30", "$fp", "$s8", "$31", "$ra",
}

# Liveness classifier for the reachability walk.  An instruction whose
# mnemonic is in neither table makes the walk refuse the fill.
#   writes its FIRST operand, every other register operand is a read
live_write_first = {
    "addu", "addiu", "add", "addi", "subu", "sub", "and", "andi", "or", "ori",
    "xor", "xori", "nor", "sll", "srl", "sra", "sllv", "srlv", "srav",
    "slt", "slti", "sltu", "sltiu", "lui", "li", "la", "move", "negu", "neg",
    "not", "mflo", "mfhi", "lb", "lbu", "lh", "lhu", "lw",
}
#   writes no general register; every register operand is a read
live_write_none = {
    "sb", "sh", "sw", "nop", "break", "mtlo", "mthi",
}

reg_operand_re = re.compile(r"\$[A-Za-z0-9]+")

# Canonical GPR numbering, so `$v0` and `$2` compare equal.
_gpr_names = {
    "$zero": 0, "$at": 1, "$v0": 2, "$v1": 3,
    "$a0": 4, "$a1": 5, "$a2": 6, "$a3": 7,
    "$t0": 8, "$t1": 9, "$t2": 10, "$t3": 11,
    "$t4": 12, "$t5": 13, "$t6": 14, "$t7": 15,
    "$s0": 16, "$s1": 17, "$s2": 18, "$s3": 19,
    "$s4": 20, "$s5": 21, "$s6": 22, "$s7": 23,
    "$t8": 24, "$t9": 25, "$k0": 26, "$k1": 27,
    "$gp": 28, "$sp": 29, "$fp": 30, "$s8": 30, "$ra": 31,
}


def gpr_number(name: str):
    """Canonical GPR number for a register operand, or None if it is not a
    plain general-purpose register (float/COP registers included)."""
    num = _gpr_names.get(name)
    if num is not None:
        return num
    if re.match(r"^\$\d{1,2}$", name):
        num = int(name[1:])
        if 0 <= num <= 31:
            return num
    return None


def strip_comments(line: str) -> str:
    if line.count("#") > 0:
        line = line.split("#")[0]
    return line.strip()


def line_loads_from_reg(line: str, r_source: str) -> bool:
    """
    NOTE: Returns True even if line might use $at expansion
    """
    line = strip_comments(line)

    # escape dollar
    r_source = r_source.replace("$", r"\$")

    if match := re.match(r"^([A-z][A-z0-9]*)\s+(.*)$", line):
        op, rest = match.group(1, 2)
    else:
        return False

    if op in load_mnemonics:
        # lwl	$9,7($2)
        if re.match(rf"^.*\(\s*{r_source}\s*\)$", rest):
            return True

    elif op in store_mnemonics:
        if re.match(rf"^.*\(\s*{r_source}\s*\)$", rest):
            return True
        # "line_loads_from_reg" is a bit of a lie
        if re.match(rf"^{r_source},.*$", rest):
            return True

    elif op == "lwc2":
        # lwc2 $5, 4( $4
        if re.match(rf"^.*\(\s*{r_source}\s*\)$", rest):
            return True

    elif op == "jal":
        if re.match(rf"^.*,\s*{r_source}$", rest):
            return True

    elif op == "j":
        if re.match(rf"^{r_source}$", rest):
            return True

    elif op in ("ctc2", "mtc0", "mtc2"):
        if re.match(rf"^{r_source},.*$", rest):
            return True

    elif op in ("mtlo", "mthi"):
        if re.match(rf"^{r_source}$", rest):
            return True

    elif op in branch_mnemonics:
        if re.match(rf"^{r_source},.*$", rest):
            return True
        if re.match(rf"^.*,\s*{r_source},.*$", rest):
            return True

    elif op in single_reg_loads:
        if re.match(rf"^.*,\s*{r_source}$", rest):
            return True
        if op.startswith("mult"):
            if re.match(rf"^{r_source},.*$", rest):
                return True
        if op.startswith("div") or op.startswith("rem"):
            # e.g. div	$3,$3,$7
            if re.match(rf"^.*,{r_source}.*$", rest):
                return True

    elif op in double_reg_loads:
        if re.match(rf"^.*,\s*{r_source},.*$", rest):
            return True
        if re.match(rf"^.*,.*,\s*{r_source}$", rest):
            return True

    return False


def is_number(value: str) -> bool:
    if re.match(r"^-?\d+$", value) or re.match(r"^-?0x[A-Fa-f0-9]+$", value):
        return True
    return False


def uses_at(line: str) -> bool:
    line = strip_comments(line)

    # sw	$2,%lo(s_attr)($3)
    if match := re.match(r"^s[wbh]\s+(\$[a-z0-9]+),\s*%lo\(([^(]+)\)\(([^)]+)\)", line):
        return False

    # sw	$2,D_801813A4
    # sw	$3,g_CurrentRoom+40
    # sw	$2,D_us_8017863C.4
    if match := re.match(r"^s[wbh]\s+(\$[a-z0-9]+),\s*(-?[A-z0-9_.+]+)$", line):
        operand = match.group(2)
        if not is_number(operand):
            return True

    # sb	$2,g_InputSaveName($3)
    # sw	$2,-26($16)
    elif match := re.match(r"^s[wbh]\s+(\$[a-z0-9]+),\s*([^(]+)\(([^)]+)\)", line):
        operand = match.group(2)

    # lw	$2,-1000($16)
    elif match := re.match(r"l[a-z]+\s+(\$[a-z0-9]+),\s*([^(]+)\(([^)]+)\)", line):
        operand = match.group(2)

    else:
        return False

    if is_number(operand):
        num = int(operand, 0)
        if -32769 < num < 32768:
            return False

    return True


# LOCAL PATCH helper (patch 8): canonicalise a MIPS register spelling so that
# `$4` and `$a0` (or `$1` and `$at`) compare equal. cc1 only ever emits the
# numeric form, but the hazard guards must not fail open on an alias.
_MIPS_REG_NAMES = (
    "zero", "at", "v0", "v1", "a0", "a1", "a2", "a3",
    "t0", "t1", "t2", "t3", "t4", "t5", "t6", "t7",
    "s0", "s1", "s2", "s3", "s4", "s5", "s6", "s7",
    "t8", "t9", "k0", "k1", "gp", "sp", "fp", "ra",
)
_MIPS_REG_CANON = {}
for _i, _n in enumerate(_MIPS_REG_NAMES):
    _MIPS_REG_CANON[f"${_n}"] = _i
    _MIPS_REG_CANON[f"${_i}"] = _i
_MIPS_REG_CANON["$s8"] = 30  # gas alias for $fp


def canon_reg(token):
    """Return the register number for `token`, or the token itself if unknown."""
    if token is None:
        return None
    return _MIPS_REG_CANON.get(token.strip(), token)


def parse_load_or_store(rest: str):
    if match := re.match(r"(\$[a-z0-9]+),\s*%lo\(([^(]+)\)\(([^(]+)\)", rest):
        r_dest, operand, r_source = match.group(1, 2, 3)
        needs_expanding = False
    elif match := re.match(r"(\$[a-z0-9]+),\s*([^(]+)\(([^)]+)\)", rest):
        r_dest, operand, r_source = match.group(1, 2, 3)
        needs_expanding = True
    elif match := re.match(r"(\$[a-z0-9]+),\s*([^(]+)", rest):
        r_dest, operand = match.group(1, 2)
        r_source = None
        needs_expanding = True
    else:
        raise Exception(f"Unable to parse load/store instruction: {rest}")

    if re.match(r"^-?\d+$", operand) or re.match(r"^-?0x[A-Fa-f0-9]+$", operand):
        is_addend = False
    else:
        is_addend = True

    return (r_source, r_dest, operand, is_addend, needs_expanding)


def div_needs_expanding(line: str) -> bool:
    inst, *rest = line.split()
    if not (inst.startswith("div") or inst.startswith("rem")):
        return False

    r_dest, *_ = rest[0].split(",")
    return r_dest not in ("$zero", "$0")


def expand_load_immediate(line: str) -> List[str]:
    res = []

    match = re.match(r"li\s+(\$[0-9A-z]+),\s?(-?[x0-9a-fA-F]+)", line)
    assert match is not None, "li regex failed"

    r_dest = match.group(1)
    operand = int(match.group(2), 0)

    if 0 < operand < 0x10000:
        res.append(f"ori\t{r_dest},$zero,{operand}")
    elif operand >= 0x10000:
        res.append(f"lui\t{r_dest},({operand} >> 16) & 0xFFFF")
        if operand & 0xFFFF:
            res.append(f"ori\t{r_dest},{r_dest},{operand} & 0xFFFF")
    elif 0 > operand > -0x8000:
        res.append(f"addiu\t{r_dest},$zero,{operand}")
    elif operand == -0x8000:
        res.append(f"addiu\t{r_dest},$zero,{operand} & 0xFFFF")
    elif operand < -0x8000:
        res.append(f"lui\t{r_dest},({operand} >> 16) & 0xFFFF")
        if operand & 0xFFFF:
            res.append(f"ori\t{r_dest},{r_dest},{operand} & 0xFFFF")
    else:
        # TODO: raise an exception here instead?
        # ori is actually addiu on ASPSX 2.56+
        res.append(f"ori\t{r_dest},0")

    return res


def expand_move(line: str):
    line = strip_comments(line)
    op, *rest = line.split()
    if op == "move":
        args = " ".join(rest)
        r_dest, r_source = args.split(",")
        return f"addu\t{r_dest},{r_source},$zero"
    return line


def is_label(line: str):
    return re.match(r"\$L(b|e)?\d+:$", line)


def is_instruction(line: str, ignore_nop=False, ignore_set=False, ignore_label=False):
    if len(line) == 0:
        return False

    if ignore_nop and line == "#nop":
        return False
    if ignore_set and line in (
        ".set\treorder",
        ".set\tnoreorder",
        ".set\tvolatile",
        ".set\tnovolatile",
    ):
        return False
    if ignore_label and is_label(line):
        return False

    if line.startswith(".stab"):
        return False
    if line.startswith(".def") or line.startswith(".bend") or line.startswith(".begin"):
        return False
    if line.startswith(".loc"):
        return False

    if line.startswith("L") and line[1] not in "0123456789" and line.endswith(":"):
        return False
    if line in (".set\tmacro", ".set\tnomacro"):
        return False
    if line in ("#.set\tvolatile", "#.set\tnovolatile"):
        return False
    if line in ("#APP", "#NO_APP"):
        return False

    return True


def get_next_register(reg: str):
    lut = {
        # li $fx
        "$f0": "$f1",
        "$f2": "$f3",
        "$f4": "$f5",
        "$f6": "$f7",
        "$f12": "$f13",
        "$f14": "$f15",
        # names
        "$v0": "$v1",
        "$a0": "$a1",
        "$a2": "$a3",
        "$t0": "$t1",
        "$t2": "$t3",
        "$s0": "$s1",
        "$s2": "$s3",
        # nums
        "$2": "$3",  # $v0
        "$4": "$5",  # $a0
        "$6": "$7",  # $a2
        "$8": "$9",  # $t0
        "$10": "$11",  # t2
        "$18": "$19",  # s2
    }
    next_reg = lut.get(reg)
    assert next_reg is not None, f"Unknown mapping for {reg}"
    return next_reg


def expand_macro(line: str):
    res = []
    for l in strip_comments(line).split(";"):
        l = l.strip()
        if len(l) == 0:
            continue
        op, *rest = l.split()
        res.append(
            (op, " ".join(rest)),
        )
    return res


def load_immediate_single(line: str):
    res = []
    r1, value = line[5:].split(",")
    (num,) = struct.unpack(">i", struct.pack(">f", float(value)))
    upper = (num & 0xFFFF_0000) >> 16
    lower = (num & 0x0000_FFFF) >> 0

    res.append(f"lui\t{r1},0x{upper:X}")
    # we don't always need the lower part
    if lower:
        res.append(f"ori\t{r1},0x{lower:X}")
    return res


def load_immediate_double(line: str):
    res = []
    r1, value = line[5:].split(",")
    r2 = get_next_register(r1)
    (num,) = struct.unpack(">q", struct.pack(">d", float(value)))

    r1_upper = (num & 0x0000_0000_FFFF_0000) >> 16
    r1_lower = (num & 0x0000_0000_0000_FFFF) >> 0
    r2_upper = (num & 0xFFFF_0000_0000_0000) >> 48
    r2_lower = (num & 0x0000_FFFF_0000_0000) >> 32

    if r1_upper or r1_lower:
        res.append(f"lui\t{r1},0x{r1_upper:X}")
        if r1_lower:
            res.append(f"ori\t{r1},0x{r1_lower:X}")
    else:
        res.append(f"li\t{r1},0x0")

    res.append(f"lui\t{r2},0x{r2_upper:X}")
    if r2_lower:
        res.append(f"ori\t{r2},0x{r2_lower:X}")

    return res


class MaspsxProcessor:
    is_reorder = True
    skip_instructions = 0
    file_num = 1
    line_index = 0

    def __init__(
        self,
        lines: List[str],
        sdata_limit=0,
        expand_div=False,
        expand_li=False,
        nop_at_expansion=False,
        nop_mflo_mfhi=True,
        sltu_at=False,
        addiu_at=False,
        div_uses_tge=False,
        gp_allow_offset=False,
        gp_allow_la=False,
        use_comm_section=False,
        use_comm_for_lcomm=False,
        fill_store_delay_slot=False,
        fill_epilogue_delay_slot=False,
        fill_call_delay_slot=False,
        fill_branch_delay_slot=False,
        three_word_symbol_store=False,
        symbol_load_dest_temp=False,
        symbol_at_temp=False,
        dispatch_fold_symbol=None,
    ):
        self.lines = [x.strip() for x in lines]

        self.sdata_limit = sdata_limit

        self.expand_div = expand_div
        self.expand_li = expand_li

        self.nop_at_expansion = nop_at_expansion
        self.nop_mflo_mfhi = nop_mflo_mfhi

        self.sltu_at = sltu_at
        self.addiu_at = addiu_at
        self.div_uses_tge = div_uses_tge

        self.gp_allow_offset = gp_allow_offset
        self.gp_allow_la = gp_allow_la

        self.use_comm_section = use_comm_section
        self.use_comm_for_lcomm = use_comm_for_lcomm

        # LOCAL PATCH: opt-in via ctor arg or env var (the env var keeps the
        # untracked maspsx.py driver unpatched).
        self.fill_store_delay_slot = (
            fill_store_delay_slot
            or os.environ.get("MASPSX_FILL_STORE_DELAY_SLOT") == "1"
        )
        # LOCAL PATCH: opt-in epilogue stack-adjust into the return delay slot.
        self.fill_epilogue_delay_slot = (
            fill_epilogue_delay_slot
            or os.environ.get("MASPSX_FILL_EPILOGUE_DELAY_SLOT") == "1"
        )
        # LOCAL PATCH (patch 8): opt-in store/`la` macro second word into the
        # delay slot of a plain `jal <symbol>` / `j <label>`.
        self.fill_call_delay_slot = (
            fill_call_delay_slot
            or os.environ.get("MASPSX_FILL_CALL_DELAY_SLOT") == "1"
        )
        # LOCAL PATCH 6: opt-in fill of a reorder-mode conditional branch delay
        # slot from the following instruction (see the patch log).
        self.fill_branch_delay_slot = (
            fill_branch_delay_slot
            or os.environ.get("MASPSX_FILL_BRANCH_DELAY_SLOT") == "1"
        )
        self._branch_fill_label_map = None
        self._branch_fill_reorder = None
        # LOCAL PATCH: the env gate keeps the untracked maspsx.py driver
        # untouched and permits per-leaf selection.
        self.three_word_symbol_store = (
            three_word_symbol_store
            or os.environ.get("MASPSX_THREE_WORD_SYMBOL_STORE") == "1"
        )
        # LOCAL PATCH: naive dest-register address temp for compound indexed
        # symbolic loads/stores (see patch log entry 4).
        self.symbol_load_dest_temp = (
            symbol_load_dest_temp
            or os.environ.get("MASPSX_SYMBOL_LOAD_DEST_TEMP") == "1"
        )
        # LOCAL PATCH: 3-word $at address temp that keeps the %lo displacement
        # for compound indexed symbolic loads/stores (see patch log entry 5).
        # MASPSX_SYMBOL_AT_TEMP=1 (or ctor True) applies to every indexed
        # symbolic operand; a comma list restricts the rewrite to those names.
        raw_at_temp = os.environ.get("MASPSX_SYMBOL_AT_TEMP")
        if symbol_at_temp or raw_at_temp == "1":
            self.symbol_at_temp = True
            self.symbol_at_temp_only = None
        elif raw_at_temp and raw_at_temp not in ("0", "false"):
            self.symbol_at_temp = True
            self.symbol_at_temp_only = set(
                s for s in raw_at_temp.split(",") if s
            )
        else:
            self.symbol_at_temp = False
            self.symbol_at_temp_only = None
        # LOCAL PATCH (switch dispatch retarget): substitute the shared
        # rodata pool table symbol for cc1-local $L<n> switch-table
        # labels in compound loads/stores. See docs/design/
        # switch_table_ownership.md.
        self.dispatch_fold_symbol = (
            dispatch_fold_symbol
            or os.environ.get("MASPSX_DISPATCH_FOLD") or None
        )

        self.bss_entries: dict[str, int] = {}
        self.sbss_entries: dict[str, int] = {}
        self.sdata_entries: dict[str, int] = {}
        # LOCAL PATCH 7: sized `.extern SYM, size` declarations that fit the
        # small-data limit.  GNU as places these gp-relative exactly like
        # `.comm` symbols, so a load/store macro on them collapses to ONE
        # instruction and a preceding load needs its MIPS-I load-delay nop
        # (see _uses_gp / _handle_nop_before_next_instruction).
        self.extern_entries: dict[str, int] = {}

        self.comm_symbols: set[str] = set()

    def preprocess_lines(self) -> None:
        in_sdata = False
        uses_size = False

        for line in self.lines:
            if line == "":
                continue

            if line.startswith(".align"):
                # TODO: worry about alignment later
                continue

            if line.startswith(".globl"):
                continue

            if line.startswith(".text"):
                in_sdata = False
                continue
            if line.startswith(".data"):
                in_sdata = False
                continue
            if line.startswith(".rdata"):
                in_sdata = False
                continue

            if line.startswith(".section") and line.endswith(".text"):
                in_sdata = False
                continue

            if line.startswith("#"):
                continue

            if line.startswith(".sdata"):
                in_sdata = True
                continue

            if line.startswith(".file"):
                in_sdata = False
                continue

            if line.startswith(".extern"):
                # e.g.	.extern	D_8009CE84, 2   (cc1 -G<n> emits these for
                # every external whose size fits the small-data limit)
                in_sdata = False
                if match := re.match(
                    r"\.extern\s+([^,\s]+)\s*,\s*(\d+)", line
                ):
                    symbol, size = match.group(1), int(match.group(2))
                    if size <= self.sdata_limit:
                        self.extern_entries[symbol] = size
                continue

            if line.startswith(".comm") or line.startswith(".lcomm"):
                # e.g.	.comm	MENU_RadarScale_800AB480,4
                in_sdata = False
                _, var = line.split()
                symbol, size_str, *_ = var.split(",")
                size = int(size_str)
                if size <= self.sdata_limit:
                    self.sbss_entries[symbol] = size
                else:
                    self.bss_entries[symbol] = size

                if line.startswith(".comm"):
                    self.comm_symbols.add(symbol)
                continue

            if in_sdata:
                # NOTE: newer compilers emit .size for sdata, old ones do not...
                if match := re.match(r"\.size\s+([^,]+),([0-9]+)", line):
                    current_symbol = match.group(1)
                    size = int(match.group(2))
                    self.sdata_entries[current_symbol] = size
                    uses_size = True
                    continue

                if not uses_size:
                    if line.endswith(":"):
                        current_symbol = line.replace(":", "")
                        self.sdata_entries[current_symbol] = 0
                    else:
                        if line.startswith(".type"):
                            continue

                        if line.startswith(".space"):
                            _, size_str = line.split()
                            size = int(size_str)
                        elif line.startswith(".word"):
                            size = 4
                        elif line.startswith(".half") or line.startswith(".short"):
                            size = 2
                        elif line.startswith(".byte"):
                            size = 1
                        elif line.startswith(".ascii"):
                            # e.g. .ascii	"Map poly groups\000"
                            # NOTE: len('.ascii\t""') == 9
                            size = len(line) - 9
                        else:
                            raise Exception(
                                f"Unable to parse .sdata instruction: {line}"
                            )
                        self.sdata_entries[current_symbol] += size

    def process_lines(self):
        self.is_reorder = True
        self.skip_instructions = 0
        self.file_num = 1

        # LOCAL PATCH 6 caches are derived from self.lines; drop them here so a
        # reused processor re-derives them.
        self._branch_fill_label_map = None
        self._branch_fill_reorder = None

        self.bss_entries = {}
        self.sbss_entries = {}
        self.sdata_entries = {}
        self.extern_entries = {}

        self.preprocess_lines()

        res = []
        in_include_asm_hack = False
        for i, line in enumerate(self.lines):
            self.line_index = i

            if ".ent\t__maspsx_include_asm_hack" in line:
                in_include_asm_hack = True

            if in_include_asm_hack:
                if "# maspsx-keep" in line:
                    res += [line]
                else:
                    res += [f"# {line} # DEBUG: skipped due to include asm hack"]
                if ".end\t__maspsx_include_asm_hack" in line:
                    in_include_asm_hack = False
                continue

            if is_instruction(line) and self.skip_instructions > 0:
                self.skip_instructions -= 1
                res += [f"# {line}  # DEBUG: skipped"]
            else:
                res += self.process_line(line)

        for section, entries in [
            ("sbss", self.sbss_entries),
            ("bss", self.bss_entries),
        ]:
            for i, (symbol, size) in enumerate(entries.items()):
                if i == 0:
                    res.append(f".section .{section}")

                if self.use_comm_section and (
                    symbol in self.comm_symbols or self.use_comm_for_lcomm
                ):
                    # implicit alignment for COMMON
                    res.append(f"\t.comm {symbol},{size}")
                    continue

                if section == "sbss":
                    if size >= 8:
                        res.append("\t.align 3")
                    elif size >= 4:
                        res.append("\t.align 2")
                    elif size >= 2:
                        res.append("\t.align 1")

                # only mark bss symbols as global
                if section == "bss":
                    res.append(
                        f"\t.globl {symbol}",
                    )
                res.extend(
                    [
                        f"{symbol}:",
                        f"\t.space {size}",
                    ]
                )

        return res

    def get_next_instruction(
        self, skip=0, ignore_nop=False, ignore_set=False, ignore_label=False
    ):
        i = self.line_index + 1
        while i < len(self.lines):
            line = self.lines[i]
            if is_instruction(
                line,
                ignore_nop=ignore_nop,
                ignore_set=ignore_set,
                ignore_label=ignore_label,
            ):
                if skip == 0:
                    return line
                skip -= 1
            i += 1

        return ""  # warn user?

    def _symbol_at_temp_applies(self, operand: str) -> bool:
        # LOCAL PATCH helper for patch 5: True when the 3-word $at+%lo form
        # should rewrite this indexed symbolic operand. A None allow-list
        # means every symbol (MASPSX_SYMBOL_AT_TEMP=1 / ctor True).
        if not self.symbol_at_temp:
            return False
        if self.symbol_at_temp_only is None:
            return True
        return operand in self.symbol_at_temp_only

    def _next_line_is_return_jump(self) -> bool:
        # LOCAL PATCH helper: True when the next non-blank, non-comment input
        # line is exactly `j $31`. Deliberately does NOT skip `.set` or label
        # lines: a label between the store and the jump means the store is
        # conditional, and a cc1 `.set noreorder` block means cc1 already
        # scheduled the jump itself — filling either slot would be wrong.
        i = self.line_index + 1
        while i < len(self.lines):
            line = self.lines[i]
            if line == "" or line.startswith("#"):
                i += 1
                continue
            op, *rest = line.split()
            return op == "j" and rest == ["$31"]
        return False

    # --- LOCAL PATCH 6: fill_branch_delay_slot ---------------------------
    #
    # Everything below refuses the fill (returns None / False) whenever it
    # cannot fully account for the code it is looking at.  Refusing restores
    # the stock `nop`, so every bail-out is the pre-patch behaviour.

    def _branch_fill_labels(self) -> dict:
        if self._branch_fill_label_map is None:
            self._branch_fill_label_map = {
                line[:-1]: i
                for i, line in enumerate(self.lines)
                if is_label(line)
            }
        return self._branch_fill_label_map

    def _branch_fill_reorder_flags(self) -> List[bool]:
        """Per-input-line reorder state, mirroring process_line's tracking.

        Needed because a branch inside a cc1 `.set noreorder` block owns the
        next line as its delay slot, while a reorder-mode branch does not.
        """
        if self._branch_fill_reorder is None:
            flags = []
            state = True
            for line in self.lines:
                flags.append(state)
                if line.startswith(".set\t"):
                    if line.endswith("\tnoreorder"):
                        state = False
                    elif line.endswith("\treorder"):
                        state = True
            self._branch_fill_reorder = flags
        return self._branch_fill_reorder

    @staticmethod
    def _branch_fill_operands(line: str) -> List[str]:
        line = strip_comments(line)
        parts = line.split(None, 1)
        if len(parts) < 2:
            return []
        return [p.strip() for p in parts[1].split(",")]

    def _branch_fill_next_index(self, index: int):
        """Index of the next line that is a real instruction, or None.

        Blank lines, comments and `.set macro/nomacro/volatile/novolatile`
        noise are stepped over; a label or any other directive returns None so
        the caller refuses (we must never reason across a basic-block edge we
        did not model).
        """
        i = index + 1
        while i < len(self.lines):
            line = self.lines[i]
            if line == "":
                i += 1
                continue
            if line.startswith("#"):
                if line in ("#APP", "#NO_APP"):
                    return None
                i += 1
                continue
            if line in (
                ".set\tmacro",
                ".set\tnomacro",
                ".set\tvolatile",
                ".set\tnovolatile",
            ):
                i += 1
                continue
            if line.startswith(".") or is_label(line) or line.endswith(":"):
                return None
            return i
        return None

    def _branch_fill_delay_slot_index(self, index: int):
        """Index of the delay-slot instruction of the branch/jump at `index`.

        Strict on purpose: only blank lines and the `.set macro/nomacro` pair
        cc1 wraps a scheduled branch in may intervene.  A comment (cc1's own
        `#nop` placeholder included), a label or any other directive means we
        are not looking at a plain `branch` / `slot` pair, so refuse.
        """
        i = index + 1
        while i < len(self.lines):
            line = self.lines[i]
            if line == "" or line in (".set\tmacro", ".set\tnomacro"):
                i += 1
                continue
            if line.startswith("#") or line.startswith(".") or line.endswith(":"):
                return None
            return i
        return None

    def _branch_fill_consumer_index(self, index: int):
        """Index of the instruction that follows `index`, skipping the cc1
        `.set` noise that wraps a scheduled branch.  A label or any other
        directive returns None (the value could then be consumed elsewhere)."""
        i = index + 1
        while i < len(self.lines):
            line = self.lines[i]
            if line == "":
                i += 1
                continue
            if line.startswith("#"):
                if line in ("#APP", "#NO_APP"):
                    return None
                i += 1
                continue
            if line.startswith(".set\t"):
                i += 1
                continue
            if line.startswith(".") or is_label(line) or line.endswith(":"):
                return None
            return i
        return None

    def _branch_fill_effect(self, line: str, reg: int):
        """Effect of a single instruction on `reg`: "read", "write", "none" or
        "bail" (mnemonic or operand shape not fully understood)."""
        line = strip_comments(line)
        if ";" in line:
            return "bail"
        if line == "" or line == "nop":
            return "none"
        parts = line.split(None, 1)
        op = parts[0]
        if op in live_write_none:
            writes = None
        elif op in live_write_first:
            writes = "first"
        else:
            return "bail"
        operands = self._branch_fill_operands(line)
        regs = []
        for tok in reg_operand_re.findall(" ".join(operands)):
            num = gpr_number(tok)
            if num is None:
                return "bail"
            regs.append(num)
        if writes == "first":
            first = operands[0] if operands else ""
            dest = gpr_number(first)
            if dest is None:
                return "bail"
            # every register after the destination is a pure read
            if reg in regs[1:]:
                return "read"
            return "write" if dest == reg else "none"
        return "read" if reg in regs else "none"

    def _branch_fill_reg_dead_at(self, label: str, reg: int) -> bool:
        """True when `reg` is provably dead at `label`.

        A conservative forward reachability walk: every path leaving `label`
        must overwrite `reg` before any instruction reads it, and every
        construct the walk cannot model (a call, an indirect or computed jump,
        inline asm, a reorder-mode conditional branch, an unknown mnemonic,
        the end of the function, a blown step budget) makes it answer False.
        """
        labels = self._branch_fill_labels()
        if label not in labels:
            return False
        reorder = self._branch_fill_reorder_flags()

        stack = [labels[label]]
        seen = set()
        steps = 0
        budget = 4096

        while stack:
            i = stack.pop()
            killed = False
            while True:
                steps += 1
                if steps > budget or i is None or i >= len(self.lines):
                    return False
                if i in seen:
                    # this program point has already been proven safe on an
                    # earlier path (an unsafe one would have returned False)
                    killed = True
                    break
                line = self.lines[i]
                if line == "":
                    i += 1
                    continue
                if line.startswith("#"):
                    if line in ("#APP", "#NO_APP"):
                        return False
                    i += 1
                    continue
                if is_label(line):
                    i += 1
                    continue
                if line.startswith("."):
                    if line.startswith(".set\t") or line.startswith(".loc\t"):
                        i += 1
                        continue
                    # .end / .ent / section or data directives: stop guessing
                    return False
                if line.endswith(":"):
                    # a non-cc1 label (function entry, asm label)
                    return False

                seen.add(i)
                op = strip_comments(line).split(None, 1)[0]

                if op in cond_branch_mnemonics:
                    if reorder[i]:
                        # no textual delay slot, and with this very patch on
                        # the slot's contents are not knowable here
                        return False
                    operands = self._branch_fill_operands(line)
                    if not operands:
                        return False
                    target = operands[-1]
                    slot = self._branch_fill_delay_slot_index(i)
                    if slot is None or target not in labels:
                        return False
                    effect = self._branch_fill_effect(self.lines[slot], reg)
                    if effect in ("bail", "read"):
                        return False
                    if effect == "write":
                        killed = True
                        break
                    stack.append(labels[target])
                    i = slot + 1
                    continue

                if op in ("j", "jr"):
                    operands = self._branch_fill_operands(line)
                    if len(operands) != 1:
                        return False
                    target = operands[0]
                    if reorder[i]:
                        if target in ("$31", "$ra"):
                            # reg is live out of the function at a return we
                            # cannot prove kills it
                            return False
                        if target not in labels:
                            return False
                        i = labels[target]
                        continue
                    slot = self._branch_fill_delay_slot_index(i)
                    if slot is None:
                        return False
                    effect = self._branch_fill_effect(self.lines[slot], reg)
                    if effect in ("bail", "read"):
                        return False
                    if effect == "write":
                        killed = True
                        break
                    if target in ("$31", "$ra") or target not in labels:
                        return False
                    i = labels[target]
                    continue

                if op in ("jal", "jalr"):
                    return False

                effect = self._branch_fill_effect(line, reg)
                if effect in ("bail", "read"):
                    return False
                if effect == "write":
                    killed = True
                    break
                i += 1

            if not killed:
                return False
        return True

    def _branch_fill_candidate(self, branch_line: str):
        """The line to emit in this reorder-mode conditional branch's delay
        slot, or None to keep the stock `nop`."""
        if not self.fill_branch_delay_slot:
            return None
        branch_line = strip_comments(branch_line)
        op = branch_line.split(None, 1)[0]
        if op not in cond_branch_mnemonics:
            return None
        operands = self._branch_fill_operands(branch_line)
        want = 3 if op in cond_branch_two_reg else 2
        if len(operands) != want:
            return None
        target = operands[-1]
        if target not in self._branch_fill_labels():
            return None

        index = self._branch_fill_next_index(self.line_index)
        if index is None:
            return None
        # the candidate must be the IMMEDIATELY following line; only blank
        # lines may separate it, so `skip_instructions` lands on it exactly
        if any(self.lines[j] != "" for j in range(self.line_index + 1, index)):
            return None

        cand = self.lines[index]
        if ";" in cand:
            return None
        cand_stripped = strip_comments(cand)
        cop = cand_stripped.split(None, 1)[0]
        spec = branch_fill_candidates.get(cop)
        if spec is None:
            return None
        nargs, imm_kind = spec
        cargs = self._branch_fill_operands(cand_stripped)
        if len(cargs) != nargs:
            return None
        dest = cargs[0]
        if dest in branch_fill_forbidden_dest:
            return None
        dest_num = gpr_number(dest)
        if dest_num is None or dest_num in (0, 1) or dest_num >= 26:
            return None
        # operands other than an allowed trailing literal must be plain GPRs
        for arg in cargs[1:-1] if nargs == 3 else []:
            if gpr_number(arg) is None:
                return None
        if nargs == 2 and imm_kind is None and gpr_number(cargs[1]) is None:
            return None

        last = cargs[-1]
        if imm_kind == "li" and gpr_number(last) is not None:
            return None
        if gpr_number(last) is None:
            # a literal: it must be in range for a one-word encoding, so GNU
            # as / maspsx can never reach for `$at`
            if not is_number(last):
                return None
            value = int(last, 0)
            if imm_kind == "s16":
                if not -0x8000 <= value <= 0x7FFF:
                    return None
                if cop == "subu" and not -0x7FFF <= value <= 0x8000:
                    return None
            elif imm_kind == "u16":
                if not 0 <= value <= 0xFFFF:
                    return None
            elif imm_kind == "u15":
                if not 0 <= value <= 0x7FFF:
                    return None
            elif imm_kind == "sh":
                if not 0 <= value <= 31:
                    return None
            elif imm_kind == "li":
                # `li` is one word only for a 16-bit-representable value; the
                # expand_li path must also produce exactly one instruction
                if not -0x8000 <= value <= 0xFFFF:
                    return None
                if self.expand_li and len(expand_load_immediate(cand_stripped)) != 1:
                    return None
            else:
                return None
        elif imm_kind == "sh":
            # sllv/srlv/srav are spelled out separately; a register shift
            # count under sll/srl/sra would be a different instruction
            return None

        # FIDELITY GATE (as opposed to the safety gate below).  Liveness alone
        # licenses more fills than retail actually performs: of the 70
        # same-shape sites in the repo's LINK_EXACT leaves, retail keeps the
        # `nop` at all 70 even though 2 of them (func_80012E7C `sll $2,$3,2`,
        # func_8006A2E8 `move $2,$5`) are provably dead-at-target and would be
        # legal to fill.  The ROM-evidenced family is specifically a COMPARE
        # that the immediately following conditional branch consumes -- the
        # compare-chain shape of func_80043474 -- so require exactly that.
        if cop not in ("slt", "slti", "sltu", "sltiu"):
            return None
        consumer = self._branch_fill_consumer_index(index)
        if consumer is None:
            return None
        consumer_line = strip_comments(self.lines[consumer])
        cons_op = consumer_line.split(None, 1)[0]
        if cons_op not in cond_branch_mnemonics:
            return None
        cons_regs = [
            gpr_number(tok)
            for tok in reg_operand_re.findall(consumer_line)
        ]
        if dest_num not in cons_regs:
            return None

        # SAFETY GATE: the fill executes on the taken edge too, so the
        # destination must be provably dead there.
        if not self._branch_fill_reg_dead_at(target, dest_num):
            return None

        # emit exactly what the normal path would have emitted for this line
        if cop == "move":
            return expand_move(cand_stripped)
        if cop == "li" and self.expand_li:
            return expand_load_immediate(cand_stripped)[0]
        return cand

    def _next_line_is_plain_call(self):
        # LOCAL PATCH helper (patch 8): inspect the next non-blank,
        # non-comment input line and return `(line, target_register)` when it
        # is a plain jump/call whose delay slot may be filled with a macro's
        # second word, else None. `target_register` is None for a direct
        # `jal <symbol>` / `j <label>` and e.g. "$2" for the register-indirect
        # forms, which cc1 spells `jal $2` (jalr) and `j $2` (jr).
        #
        # Like _next_line_is_return_jump this deliberately does NOT skip
        # `.set` or label lines (hazard H5): a label means the jump is a
        # branch target and the macro is not guaranteed to have run, and a
        # cc1 `.set noreorder` block means cc1 already owns that slot.
        #
        # `j $31` (the function return) is always refused: that slot belongs
        # to patches 1 and 3, whose ROM evidence covers it, and stealing it
        # here would change their behaviour when both knobs are on.
        i = self.line_index + 1
        while i < len(self.lines):
            line = self.lines[i]
            if line == "" or line.startswith("#"):
                i += 1
                continue
            if ";" in line:  # H7: compound macro line
                return None
            parts = line.split()
            if len(parts) != 2:
                return None
            op, target = parts
            if op not in ("j", "jal"):  # H6: not a conditional branch
                return None
            if not target.startswith("$"):
                return (line, None)
            if canon_reg(target) == 31:  # patch 1 / patch 3 own this slot
                return None
            # H3: a register-indirect jump reads its target register at jump
            # time, i.e. BEFORE the delay slot runs. The caller must check
            # that the moved word does not write that register.
            return (line, target)
        return None

    def _uses_gp(self, line: str) -> bool:
        if self.sdata_limit == 0:
            return False

        line = strip_comments(line)
        if uses_at(line):
            op, *rest = line.split("\t")
            if op in load_mnemonics or op in store_mnemonics:
                (
                    _,
                    _,
                    operand,
                    _,
                    _,
                ) = parse_load_or_store(" ".join(rest))

                if operand.count("+") == 1:
                    symbol, _ = operand.split("+")
                    gp_allowed = self.gp_allow_offset or symbol not in self.comm_symbols
                else:
                    symbol = operand
                    gp_allowed = True

                if gp_allowed and (
                    symbol in self.sbss_entries
                    or symbol in self.sdata_entries
                    # LOCAL PATCH 7: sized externs resolve gp-relative too.
                    # Only the hazard decision consults this; the rewrite to
                    # an explicit %gp_rel operand stays GNU as's job (it
                    # already honours the sized `.extern`), so flag-off text
                    # is unchanged apart from the mandatory nop.
                    or symbol in self.extern_entries
                ):
                    return True

        return False

    def _handle_nop_before_next_instruction(
        self, next_instruction: str, r_dest: str
    ) -> List[str]:
        res: List[str] = []

        if line_loads_from_reg(next_instruction, r_dest):
            nop_required = False

            if not uses_at(next_instruction):
                reason = f"'{next_instruction}' does not use $at"
                nop_required = True
            if self._uses_gp(next_instruction):
                reason = f"'{next_instruction}' uses $gp"
                nop_required = True
            if uses_at(next_instruction) and self.nop_at_expansion:
                reason = (
                    f"'{next_instruction}' inject nop beween {r_dest} and $at expansion"
                )
                nop_required = True

            if nop_required:
                label = self.get_next_instruction(
                    skip=0, ignore_nop=True, ignore_set=True
                )
                if is_label(label):
                    res.append(label)
                    self.skip_instructions = 1
                res.append(f"nop # DEBUG: Reuse of '{r_dest}'. {reason}")
        else:
            res.append(
                f"#nop # DEBUG: '{next_instruction}' does not load from {r_dest}"
            )

        return res

    def _handle_mflo_mfhi(self, r_source=None) -> List[str]:
        # we cannot use a div/mult within 2 instructions of mflo/mfhi
        res: List[str] = []

        if not self.nop_mflo_mfhi:
            return res

        next_instruction = self.get_next_instruction(
            skip=0, ignore_nop=True, ignore_set=True, ignore_label=True
        )
        next_next_instruction = self.get_next_instruction(
            skip=1, ignore_nop=True, ignore_set=True, ignore_label=True
        )

        if any(
            next_instruction.startswith(x)
            for x in ["mult\t", "multu\t", "div\t", "divu\t", "rem\t", "remu\t"]
        ):
            # #nop
            # #nop
            # mult...
            skip = 0
            while True:
                inst = self.get_next_instruction(skip=skip)
                skip += 1
                if inst == next_instruction:
                    res.append("nop")
                    res.append("nop")
                    if div_needs_expanding(inst):
                        res.append("# DEBUG: div needs expanding")
                        skip -= 1
                    else:
                        res.append(expand_move(inst))
                    break
                if not inst.startswith("#"):
                    res.append(expand_move(inst))
            self.skip_instructions = skip

        elif any(
            next_next_instruction.startswith(x)
            for x in ["mult\t", "multu\t", "div\t", "divu\t", "rem\t", "remu\t"]
        ):

            # #nop
            # #nop
            # bne or addu or lh ...
            # mult ...
            skip = 0
            no_reorder = False
            while True:
                inst = self.get_next_instruction(skip=skip)

                if inst.startswith(".set") and inst.endswith("noreorder"):
                    no_reorder = True
                    skip += 1
                    continue

                skip += 1
                if inst == next_instruction:
                    op, *_ = inst.strip().split()
                    if op in load_mnemonics:
                        # allow for $at handling later in the script
                        skip = 0
                        break

                    if op in ("mflo", "mfhi"):
                        # allow for mflo/mfhi handling later on
                        skip = 0
                        break

                    if op == "li":
                        expanded = expand_load_immediate(inst)

                        if self.expand_li:
                            res += expanded
                        else:
                            res.append(inst)

                        if len(expanded) == 2:
                            res.append(
                                "#nop  # DEBUG: mflo/mfhi with mult/div/rem and li expands to 2 ops"
                            )
                        else:
                            res.append(
                                "nop  # DEBUG: mflo/mfhi with mult/div/rem and li expands to 1 op"
                            )

                    else:

                        if no_reorder:
                            res.append(
                                "nop  # DEBUG: mflo/mfhi with mult/div/rem and 1 instruction (noreorder)"
                            )
                            res.append(".set\tnoreorder")
                            res.append(expand_move(inst))
                        else:
                            if r_source and line_loads_from_reg(inst, r_source):
                                # NOTE: only relevant when div has been expanded (i.e. -0 flag)
                                res.extend(
                                    [
                                        f"nop  # DEBUG: mflo/mfhi with mult/div/rem and 1 instruction which loads from {r_source}",
                                        expand_move(inst),
                                    ]
                                )
                            else:
                                if op in branch_mnemonics:
                                    res.extend(
                                        [
                                            inst,
                                            "nop # DEBUG: mflo/mfhi with mult/div/rem and 1 instruction (branch)",
                                        ]
                                    )
                                else:
                                    maybe_label = self.get_next_instruction(skip=skip)
                                    if is_label(maybe_label):
                                        res.extend(
                                            [
                                                expand_move(inst),
                                                maybe_label,
                                                "nop  # DEBUG: mflo/mfhi with mult/div/rem and 1 instruction (label)",
                                            ]
                                        )
                                        skip += 1
                                    else:
                                        res.extend(
                                            [
                                                expand_move(inst),
                                                "nop  # DEBUG: mflo/mfhi with mult/div/rem and 1 instruction",
                                            ]
                                        )

                elif inst == next_next_instruction:
                    # reached mult/div/rem
                    if div_needs_expanding(inst):
                        res.append("# DEBUG: div needs expanding")
                        skip -= 1
                    else:
                        res.append(inst)
                    break
                elif not inst.startswith("#"):
                    res.append(expand_move(inst))
            self.skip_instructions = skip

        else:
            # do nothing
            pass

        return res

    def process_line(self, line: str):
        res = []

        if len(line) == 0:
            return [line]

        if line.startswith("#"):
            return []

        if line.startswith("."):
            if (
                line.startswith(".def\t")
                or line.startswith(".begin\t")
                or line.startswith(".bend\t")
            ):
                # skip these coff directives - gnu as does not like them
                pass

            elif line.startswith(".set\t"):
                if line.endswith("\tnoreorder"):
                    self.is_reorder = False
                elif line.endswith("\treorder"):
                    self.is_reorder = True

            elif line.startswith(".file\t"):
                # fix same-numbered files
                _, file_num, filename = line.split(maxsplit=2)
                res.append(f".file\t{self.file_num} {filename}")
                self.file_num += 1

            elif line.startswith(".ent\t"):
                # enforce noreorder for each function
                res.append(line)
                res.append(".set\tnoreorder")

            elif line.startswith(".comm") or line.startswith(".lcomm"):
                # already handled via preprocess_lines
                pass

            elif line.startswith(".data"):
                res.append(".section .data")
            elif line.startswith(".sdata"):
                res.append(".section .sdata")
            elif line.startswith(".rdata"):
                res.append(".section .rodata")

            else:
                res.append(line)

            return res

        if line.startswith("$L"):
            return [line]

        actual_r_dest = None
        is_macro = ";" in line
        if is_macro:
            expanded = expand_macro(line)
            if len(expanded) > 0:
                actual_op, *actual_rest = expanded[-1]
                if actual_op in load_mnemonics:
                    _, actual_r_dest, _, _, _ = parse_load_or_store(
                        " ".join(actual_rest)
                    )

        op, *rest = line.split()

        # LOCAL PATCH: fill the return delay slot with the epilogue frame
        # deallocation. Opt-in; see the patch log at the top of this file.
        if (
            self.fill_epilogue_delay_slot
            and op in ("addu", "addiu")
            and len(rest) == 1
            and rest[0].startswith("$sp,$sp,")
            and self._next_line_is_return_jump()
        ):
            frame = rest[0].split(",")[2]
            res.extend(
                [
                    "# FILL_EPILOGUE_DELAY_SLOT START",
                    ".set\tnoreorder",
                    "j\t$31",
                    f"addiu\t$sp,$sp,{frame}",
                    ".set\treorder",
                    "# FILL_EPILOGUE_DELAY_SLOT END",
                ]
            )
            self.skip_instructions = 1
            return res

        # LOCAL PATCH (patch 8, `la` arm): put the address macro's second word
        # into the delay slot of the following plain jal/j:
        #   lui $r,%hi(SYM) / jal T / addiu $r,$r,%lo(SYM)
        # This is a dedicated block rather than a widening of the store
        # branch's `la` condition so that nothing else in that chain becomes
        # newly reachable for `la` when the knob is on.
        if (
            self.fill_call_delay_slot
            and op == "la"
            and not is_macro  # H7
            and len(rest) == 1
            # H8: with gp_allow_la (ASPSX >= 2.80) a `la` of a small-data
            # symbol is a single gp-relative instruction with no second word
            # to move. That eligibility is not decidable here for every
            # operand spelling, so refuse the whole mode.
            and not self.gp_allow_la
        ):
            la_parts = rest[0].split(",")
            la_operand = la_parts[1] if len(la_parts) == 2 else ""
            call = self._next_line_is_plain_call()
            if (
                call is not None
                and la_operand
                and "(" not in la_operand  # not an indexed form
                # H2: the moved word WRITES this register; $31 would destroy
                # the return address `jal` has just stored.
                and canon_reg(la_parts[0]) not in (31, 1)
                # H3: a register-indirect jump reads its target register
                # BEFORE the slot runs, so the moved word must not be what
                # completes that address.
                and canon_reg(call[1]) != canon_reg(la_parts[0])
                # H9: a numeric operand is not a symbol macro.
                and not re.match(r"^-?(0x[0-9A-Fa-f]+|\d+)$", la_operand)
            ):
                r_dest = la_parts[0]
                res.extend(
                    [
                        "# FILL_CALL_DELAY_SLOT LA START",
                        f"lui\t{r_dest},%hi({la_operand})",
                        call[0],
                        f"addiu\t{r_dest},{r_dest},%lo({la_operand})",
                        "# FILL_CALL_DELAY_SLOT LA END",
                    ]
                )
                self.skip_instructions = 1
                return res

        if op in load_mnemonics:
            r_source, r_dest, operand, is_addend, needs_expanding = parse_load_or_store(
                " ".join(rest)
            )

            next_instruction = self.get_next_instruction(
                skip=0, ignore_nop=True, ignore_set=True, ignore_label=True
            )
            # Naively handle scenario where *next* line is a macro
            if ";" in next_instruction:
                next_instruction = next_instruction.split(";")[0]

            if not needs_expanding:
                # newer GCCs can emit %hi() and %lo() separately...
                res.append(f"{line} # DEBUG: leaving for assembler to expand")
                extra_nops = self._handle_nop_before_next_instruction(
                    next_instruction, r_dest
                )
                res.extend(extra_nops)

            elif is_addend and r_source is None:
                # e.g. lb	$s0,D_800E52E0
                if operand.count("+") == 1:
                    symbol, offset = operand.split("+")
                    gp_rel = f"%gp_rel({symbol}+{offset})($gp)"
                    gp_allowed = self.gp_allow_offset or symbol not in self.comm_symbols
                else:
                    symbol = operand
                    gp_rel = f"%gp_rel({symbol})($gp)"
                    gp_allowed = True

                if gp_allowed and (
                    symbol in self.sdata_entries or symbol in self.sbss_entries
                ):
                    res.append(f"{op}\t{r_dest},{gp_rel}")
                else:
                    res.append(line)

                extra_nops = self._handle_nop_before_next_instruction(
                    next_instruction, r_dest
                )
                res.extend(extra_nops)

            elif is_addend and r_source:
                # e.g. lw	$2,test_sym($4)
                # LOCAL PATCH (switch dispatch retarget): when
                # MASPSX_DISPATCH_FOLD=<sym> is set alongside the
                # three-word gate and the operand is a compiler-local
                # label ($L<digits> — cc1's own switch table), substitute
                # <sym> (the shared rodata pool table) for the local
                # label; named symbols are never substituted and gate-off
                # keeps every prior behavior.
                if (
                    self.three_word_symbol_store
                    and self.dispatch_fold_symbol
                    and re.match(r"^\$L\d+$", operand)
                ):
                    operand = self.dispatch_fold_symbol
                dest_temp_used = (
                    self.symbol_load_dest_temp
                    and not is_macro
                    and not re.match(r"^-?(0x[0-9A-Fa-f]+|\d+)$", operand)
                    # The destination cannot serve as the address temp when
                    # it IS the index register (`lhu $2,SYM($2)`): the
                    # `lui $2` would clobber the index before the `addu`
                    # read it.  GNU as (and retail: func_80074A44 has both
                    # `lhu $v1,D_800957CC($v0)` dest-temp and
                    # `lhu $v0,D_800957D8($v0)` via $at) fall back to the
                    # $at form in exactly that case.
                    and canon_reg(r_dest) != canon_reg(r_source)
                )
                # LOCAL PATCH (patch 5): 3-word $at address temp, %lo kept.
                at_temp_used = (
                    self._symbol_at_temp_applies(operand)
                    and not is_macro
                    and not re.match(r"^-?(0x[0-9A-Fa-f]+|\d+)$", operand)
                )
                if at_temp_used:
                    res.extend(
                        [
                            "# SYMBOL_AT_TEMP START",
                            ".set\tnoat",
                            f"lui\t$at,%hi({operand})",
                            f"addu\t$at,$at,{r_source}",
                            f"{op}\t{r_dest},%lo({operand})($at)",
                            ".set\tat",
                            "# SYMBOL_AT_TEMP END",
                        ]
                    )
                # LOCAL PATCH (patch 4): naive dest-register address temp.
                if dest_temp_used and not at_temp_used:
                    res.extend(
                        [
                            "# DEST_TEMP START",
                            ".set\tnoat",
                            f"lui\t{r_dest},%hi({operand})",
                            f"addu\t{r_dest},{r_dest},{r_source}",
                            f"{op}\t{r_dest},%lo({operand})({r_dest})",
                            ".set\tat",
                            "# DEST_TEMP END",
                        ]
                    )
                if dest_temp_used or at_temp_used:
                    pass
                elif self.addiu_at and not self.three_word_symbol_store:
                    res.extend(
                        [
                            "# EXPAND_AT START",
                            ".set\tnoat",
                            f"lui\t$at,%hi({operand})",
                            f"addiu\t$at,$at,%lo({operand})",
                            f"addu\t$at,$at,{r_source}",
                            f"{op}\t{r_dest},0x0($at)",
                            ".set\tat",
                            "# EXPAND_AT END",
                        ]
                    )
                elif self.addiu_at and self.three_word_symbol_store:
                    # Compound macro lines keep the legacy lui/addiu/addu
                    # expansion; standalone lines use the 3-word $at form.
                    if is_macro:
                        res.extend(
                            [
                                "# EXPAND_AT START",
                                ".set\tnoat",
                                f"lui\t$at,%hi({operand})",
                                f"addiu\t$at,$at,%lo({operand})",
                                f"addu\t$at,$at,{r_source}",
                                f"{op}\t{r_dest},0x0($at)",
                                ".set\tat",
                                "# EXPAND_AT END",
                            ]
                        )
                    else:
                        res.extend(
                            [
                                "# EXPAND_AT START",
                                ".set\tnoat",
                                f"lui\t$at,%hi({operand})",
                                f"addu\t$at,$at,{r_source}",
                                f"{op}\t{r_dest},%lo({operand})($at)",
                                ".set\tat",
                                "# EXPAND_AT END",
                            ]
                        )
                else:
                    res.extend(
                        [
                            "# EXPAND_AT START",
                            ".set\tnoat",
                            f"lui\t$at,%hi({operand})",
                            f"addu\t$at,$at,{r_source}",
                            f"{op}\t{r_dest},%lo({operand})($at)",
                            ".set\tat",
                            "# EXPAND_AT END",
                        ]
                    )

                extra_nops = self._handle_nop_before_next_instruction(
                    next_instruction, r_dest
                )
                res.extend(extra_nops)

            else:
                if r_source and (int(operand, 0) > 32767 or int(operand, 0) < -32768):
                    # e.g. lhu	$2,49344($2)
                    res.extend(
                        [
                            "# EXPAND_AT START",
                            ".set\tnoat",
                            f"lui\t$at,%hi({operand})",
                            f"addu\t$at,{r_source},$at",
                            f"{op}\t{r_dest},%lo({operand})($at)",
                            ".set\tat",
                            "# EXPAND_AT END",
                        ]
                    )
                else:
                    # e.g. lhu	$2,528482304
                    res.append(line)

                # Naively handle scenario where *current* line is a macro
                if actual_r_dest is not None:
                    r_dest = actual_r_dest

                extra_nops = self._handle_nop_before_next_instruction(
                    next_instruction, r_dest
                )
                res.extend(extra_nops)

        elif op in store_mnemonics or (op == "la" and self.sdata_limit > 0):
            r_source, r_dest, operand, is_addend, needs_expanding = parse_load_or_store(
                " ".join(rest)
            )

            if not needs_expanding:
                # LOCAL PATCH: an explicit `op $r,%lo(SYM)($base)` store (the
                # form gcc-2.8.1-psx cc1 emits after its own `lui $base,%hi`;
                # 2.7.2 never emits it) is already one instruction with a
                # LO16 reloc — pass it through exactly like the load arm above
                # instead of re-expanding it into the 4-word $at macro.
                res.append(f"{line} # DEBUG: leaving for assembler to expand")

            elif is_addend and r_source is None:
                # e.g. sw	$v0,D_800E52E0
                if operand.count("+") == 1:
                    symbol, offset = operand.split("+")
                    gp_rel = f"%gp_rel({symbol}+{offset})($gp)"
                    gp_allowed = self.gp_allow_offset or symbol not in self.comm_symbols
                else:
                    symbol = operand
                    gp_rel = f"%gp_rel({symbol})($gp)"
                    gp_allowed = True

                if op == "la" and not self.gp_allow_la:
                    gp_allowed = False

                if gp_allowed and (
                    symbol in self.sdata_entries or symbol in self.sbss_entries
                ):
                    res.append(f"{op}\t{r_dest},{gp_rel}")
                elif (
                    self.fill_store_delay_slot
                    and op == "sw"
                    and self._next_line_is_return_jump()
                ):
                    # LOCAL PATCH: fill the return delay slot with the store.
                    # cc1 cannot schedule the absolute-store macro (its
                    # `lui $at` + `sw` expansion is opaque to cc1's delay-slot
                    # filler) around `j $31`, so it emits `sw $r,SYM` / `j $31`
                    # with an empty slot. ASPSX's scheduler expands the macro
                    # and moves the trailing store into the slot:
                    #   lui $at,%hi(SYM) / j $31 / sw $r,%lo(SYM)($at)
                    # ROM-proven for the 14-member sw delay-slot family.
                    # sw only: sb/sh macro stores stay pre-jr (nop slot) in
                    # ROM. Consuming the `j $31` line via skip_instructions
                    # also suppresses the nop maspsx would append for it.
                    res.extend(
                        [
                            "# FILL_STORE_DELAY_SLOT START",
                            ".set\tnoat",
                            f"lui\t$at,%hi({operand})",
                            "j\t$31",
                            f"{op}\t{r_dest},%lo({operand})($at)",
                            ".set\tat",
                            "# FILL_STORE_DELAY_SLOT END",
                        ]
                    )
                    self.skip_instructions = 1
                elif (
                    # LOCAL PATCH (patch 8, absolute-store arm): the macro's
                    # second word goes into the delay slot of the following
                    # plain jal/j, exactly as ASPSX's reorder pass schedules
                    # it:  lui $at,%hi(SYM) / jal T / sw $r,%lo(SYM)($at)
                    self.fill_call_delay_slot
                    and op in ("sb", "sh", "sw")
                    and not is_macro  # H7
                    # H1: `jal` writes $31, so a value register of $31 would
                    # store the NEW return address once moved past the call.
                    # H4: $at is the address temp the `lui` sets up.
                    and canon_reg(r_dest) not in (31, 1)
                    and (call := self._next_line_is_plain_call()) is not None
                    # H3: the moved store writes no register, so a
                    # register-indirect target is safe unless it is the `$at`
                    # the surviving `lui` overwrites before the jump.
                    and canon_reg(call[1]) != 1
                ):
                    res.extend(
                        [
                            "# FILL_CALL_DELAY_SLOT START",
                            ".set\tnoat",
                            f"lui\t$at,%hi({operand})",
                            call[0],
                            f"{op}\t{r_dest},%lo({operand})($at)",
                            ".set\tat",
                            "# FILL_CALL_DELAY_SLOT END",
                        ]
                    )
                    self.skip_instructions = 1
                else:
                    res.append(line)
            elif is_addend and r_source:
                # e.g. sw	$a0,ctlbuf($v0)
                # LOCAL PATCH (patch 4 store arm): a bare indexed symbolic
                # store that immediately precedes a `j $31` in ROM emits the
                # 3-word $at materialization BEFORE the jump and schedules the
                # store itself into the delay slot:
                #   lui $at,%hi(SYM) / addu $at,$at,$b / j $31 /
                #   op $src,%lo(SYM)($at)
                # ROM: func_80076B20 tail (sb $a0,%lo(D_800A3348)($at) in the
                # jr delay slot). Everything else keeps the legacy path.
                store_at_temp_used = (
                    self._symbol_at_temp_applies(operand)
                    and not is_macro
                    and not re.match(r"^-?(0x[0-9A-Fa-f]+|\d+)$", operand)
                )
                if store_at_temp_used:
                    res.extend(
                        [
                            "# SYMBOL_AT_TEMP STORE START",
                            ".set\tnoat",
                            f"lui\t$at,%hi({operand})",
                            f"addu\t$at,$at,{r_source}",
                            f"{op}\t{r_dest},%lo({operand})($at)",
                            ".set\tat",
                            "# SYMBOL_AT_TEMP STORE END",
                        ]
                    )
                elif (
                    self.symbol_load_dest_temp
                    and not is_macro
                    and not re.match(r"^-?(0x[0-9A-Fa-f]+|\d+)$", operand)
                    and self._next_line_is_return_jump()
                ):
                    res.extend(
                        [
                            "# DEST_TEMP STORE_FILL START",
                            ".set\tnoat",
                            f"lui\t$at,%hi({operand})",
                            f"addu\t$at,$at,{r_source}",
                            "j\t$31",
                            f"{op}\t{r_dest},%lo({operand})($at)",
                            ".set\tat",
                            "# DEST_TEMP STORE_FILL END",
                        ]
                    )
                    self.skip_instructions = 1
                elif (
                    self.addiu_at
                    and op != "la"
                    and (not self.three_word_symbol_store or is_macro)
                ):
                    res.extend(
                        [
                            "# EXPAND_AT START",
                            ".set\tnoat",
                            f"lui\t$at,%hi({operand})",
                            f"addiu\t$at,$at,%lo({operand})",
                            f"addu\t$at,$at,{r_source}",
                            f"{op}\t{r_dest},0x0($at)",
                            ".set\tat",
                            "# EXPAND_AT END",
                        ]
                    )
                else:
                    res.append(line)
            elif r_source and (int(operand, 0) > 32767 or int(operand, 0) < -32768):
                # e.g. sw	$2,56200($4)
                res.extend(
                    [
                        "# EXPAND_AT START",
                        ".set\tnoat",
                        f"lui\t$at,%hi({operand})",
                        f"addu\t$at,{r_source},$at",
                        f"{op}\t{r_dest},%lo({operand})($at)",
                        ".set\tat",
                        "# EXPAND_AT END",
                    ]
                )
            else:
                res.append(line)

        elif op in branch_mnemonics or op in jump_mnemonics:
            res.append(line)
            if self.is_reorder:
                # LOCAL PATCH 6: opt-in fill from the following instruction
                # when that instruction's destination is provably dead at the
                # branch target; otherwise the stock nop.
                fill = self._branch_fill_candidate(line)
                if fill is not None:
                    res.extend(
                        [
                            "# FILL_BRANCH_DELAY_SLOT START",
                            fill,
                            "# FILL_BRANCH_DELAY_SLOT END",
                        ]
                    )
                    self.skip_instructions = 1
                else:
                    res.append("nop  # DEBUG: branch/jump")

        elif op == "move":
            # expand move $2,$16 to addu $2,$16,$zero
            res.append(expand_move(line))

        elif op in ("addu", "subu", "sra", "srl", "srr", "sll", "or"):
            # no extra processing required
            res.append(line)
            # TODO: check if this line is a macro and insert a nop if required...

        elif op == "li":
            # TODO: handle non-soft floats?
            if self.expand_li:
                res += expand_load_immediate(line)
            else:
                res.append(line)

        elif op == "li.s":
            res += load_immediate_single(line)

        elif op == "li.d":
            res += load_immediate_double(line)

        elif op in ("mflo", "mfhi"):
            res.append(line)
            res += self._handle_mflo_mfhi()

        elif op == "break":
            # turn 'break 7' into 'break 0x0,0x7'
            num = int(rest[0], 0)
            line = f"break\t0x{num >> 10:X},0x{num & 0x3FF:X}"
            res.append(line)

        elif op in ("div", "rem"):
            r_dest, r_source, r_operand = rest[0].split(",")
            if r_dest in ("$zero", "$0"):
                # e.g. div $zero, $v0, $a0
                return [line]

            move_from = "mfhi" if op == "rem" else "mflo"
            if self.expand_div:
                res.extend(
                    [
                        "# EXPAND_DIV START",
                        ".set\tnoat",
                        f"div\t$zero,{r_source},{r_operand}",
                        f"bnez\t{r_operand},.L_NOT_DIV_BY_ZERO_{self.line_index}",
                        "nop",
                        "break\t0x7",
                        f".L_NOT_DIV_BY_ZERO_{self.line_index}:",
                        "addiu\t$at,$zero,-1",
                        f"bne\t{r_operand},$at,.L_DIV_BY_POSITIVE_SIGN_{self.line_index}",
                        "lui\t$at,0x8000",
                        f"bne\t{r_source},$at,.L_DIV_BY_POSITIVE_SIGN_{self.line_index}",
                        "nop",
                        "tge\t$zero,$zero,93" if self.div_uses_tge else "break\t0x6",
                        f".L_DIV_BY_POSITIVE_SIGN_{self.line_index}:",
                        f"{move_from}\t{r_dest}",
                        ".set\tat",
                        "# EXPAND_DIV END",
                    ]
                )
            else:
                res.extend(
                    [
                        "# EXPAND_ZERO_DIV START",
                        f"div\t$zero,{r_source},{r_operand}",
                        f"{move_from}\t{r_dest}",
                        "# EXPAND_ZERO_DIV END",
                    ]
                )

            extra_nops = self._handle_mflo_mfhi(r_source=r_dest)
            if len(extra_nops) > 0:
                res += extra_nops
            else:
                next_instruction = self.get_next_instruction(
                    skip=0, ignore_set=True, ignore_label=True
                )
                extra_nops = self._handle_nop_before_next_instruction(
                    next_instruction, r_dest
                )
                res.extend(extra_nops)

        elif op in ("divu", "remu"):
            r_dest, r_source, r_operand = rest[0].split(",")
            if r_dest in ("$zero", "$0"):
                # e.g. divu $zero, $v1, $a2
                return [line]

            move_from = "mfhi" if op == "remu" else "mflo"
            if self.expand_div:
                res.extend(
                    [
                        "# EXPAND_DIVU START",
                        ".set\tnoat",
                        f"divu\t$zero,{r_source},{r_operand}",
                        f"bnez\t{r_operand},.L_NOT_DIV_BY_ZERO_{self.line_index}",
                        "nop",
                        "break\t0x7",
                        f".L_NOT_DIV_BY_ZERO_{self.line_index}:",
                        f"{move_from}\t{r_dest}",
                        ".set\tat",
                        "# EXPAND_DIVU END",
                    ]
                )
            else:
                res.extend(
                    [
                        "# EXPAND_ZERO_DIVU START",
                        f"divu\t$zero,{r_source},{r_operand}",
                        f"{move_from}\t{r_dest}",
                        "# EXPAND_ZERO_DIVU END",
                    ]
                )

            extra_nops = self._handle_mflo_mfhi(r_source=r_dest)
            if len(extra_nops) > 0:
                res += extra_nops
            else:
                next_instruction = self.get_next_instruction(
                    skip=0, ignore_set=True, ignore_label=True
                )
                extra_nops = self._handle_nop_before_next_instruction(
                    next_instruction, r_dest
                )
                res.extend(extra_nops)

        elif op == "sltu":
            r_dest, r_source, r_operand = rest[0].split(",")
            if re.match(r"^-?\d+$", r_operand) or re.match(
                r"^-?0x[A-Fa-f0-9]+$", r_operand
            ):
                value = int(r_operand)
                if self.sltu_at and value < 0:
                    res.append(f"li\t$at,{r_operand}")
                    res.append(f"{op}\t{r_dest},{r_source},$at")
                else:
                    # TODO: do we want to expand sltu into sltiu?
                    res.append(line)
            else:
                res.append(line)

        else:
            res.append(line)

        return res
