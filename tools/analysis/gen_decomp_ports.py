#!/usr/bin/env python3
"""gen_decomp_ports.py — derive pc_port TUs verbatim from matching src/ leaves.

Mechanism (documented in docs/ai_context/PC_PORT_FROM_DECOMP.md): the verified
matching C leaf at `src/func_XXXXXXXX.c` is the *authority*.  This tool copies
its body **verbatim** into

    pc_port/game/decomp/func_XXXXXXXX_port.c

and emits only the mechanical host adaptation around it:

  * a provenance banner naming the matched leaf, its VMA/span, word count and
    evidence report;
  * `#include "pe_guest_decomp.h"`;
  * for every `extern <type> D_XXXXXXXX;` / `extern <type> D_XXXXXXXX[];` in the
    leaf, a `#define D_XXXXXXXX ...` expander chosen by the declaration shape
    (scalar lvalue / guest array base / pointer-global);
  * for every callee of the leaf that pc_port does not already implement, a
    loud-boundary macro returning 0 and recording the symbol;
  * the leaf's declarations and body, copied byte-for-byte otherwise.

`src/` is never written to and never needs to be.  Re-running the tool after
matching advances refreshes the port set; `docs/ai_context/PC_PORT_FROM_DECOMP.md`
records the per-leaf recipe and the eligibility contract.

Eligibility (conservative by construction; a rejected leaf is reported in
`--check`, never guessed):

  E1  the leaf is a matched `c` span in configs/USA/disc1.yaml;
  E2  no `asm`/`__asm__` (era register pins / scheduler fences);
  E3  no MMIO / scratchpad / BIOS-space 32-bit literal (0x1F8xxxxx, 0x1F80xxxx,
      0xA0xxxxxx, 0xB0xxxxxx); ordinary masks like 0xFFFEFFFF stay verbatim;
  E4  exactly one file-scope `func_*` definition (no collisions);
  E5  no indirect call through a local pointer;
  E6  every `func_*` referenced is a known pc_port definition (boundable
      callee or linkable descriptor);
  E7  no pointer-typed parameter (a guest pointer held in a host pointer is
      not 32-bit; those leaves need a hand-written adapter, so they are
      reported rather than silently mis-ported);
  E8  no typed pointer cast whose operand does not derive from a `D_` data
      symbol (value parameters used as guest addresses are not representable);
  E9  no `&func_*` (function-pointer values are not host-compatible);
  E10 no store through a pointer-pointer cast of a `D_` symbol (would write a
      host pointer into guest RAM);
  E11 every called `func_*` with a canonical pc_port prototype is called with
      that prototype's arity;
  E12 no assignment to a pointer-global slot;
  E13 no `func_*` call result bound to a pointer object.  A retail callee that
      returns `int *` returns a 32-bit *guest* address; host-side that callee
      is either a canonical `pe_port_compat.h` prototype returning `pe_addr_t`
      or a loud boundary returning `int`, so binding the result to a host
      pointer is not representable (it is the same class as E7, one call
      deeper).  Rejected unless the callee's resolved host return type is
      itself a pointer;
  E14 no host-pointer expression passed to a callee parameter that is not a
      host pointer.  A local aggregate (`char buf[8]`), a pointer local, or
      `&local` is host stack memory; a canonical prototype taking `pe_addr_t`
      wants a guest address.  There is no guest frame for a generated leaf, so
      such a leaf needs a hand-written adapter and is reported, not guessed.
      (A bare `D_` symbol argument is *not* this case: `fix_argument_addresses`
      already substitutes the retail guest address.  A pointer *parameter* is
      not either: the E7b wrapper forwards it as the guest address `pe_<name>`.)
  E15 no guest array/global *of pointers* (`extern T *D_XXXX[];`,
      `extern T **D_XXXX;`).  The slot is a 32-bit guest address; typing the
      element as a host pointer would read 8 bytes on a 64-bit host.  One
      level (`extern T *D_XXXX;`) is fine — PE_DECOMP_PTRGLOBAL loads the
      32-bit slot and translates it;
  E16 no host pointer narrowed to an integer (`(unsigned int)D_XXXX`,
      `(int)ptr_param`).  Retail's value is a 32-bit guest address; the host
      accessor is a 64-bit pointer, so the cast truncates it silently.

E13-E16 all describe the same underlying fact from different angles: a 32-bit
guest address and a 64-bit host pointer are not the same object, so one must
never be substituted for the other.  E15/E16 matter most, because those two
shapes *compile* (with only a warning) and are silently wrong at run time.

Final gate (E17, the one that cannot be fooled): every rendered TU is
syntax-checked with the host C compiler, using pc_port's own include paths and
defines, before it is written.  A leaf whose TU does not compile is reported
and not emitted, so `pc_port/game/decomp/` is always buildable even when a new
matched leaf has a shape no static rule anticipated.  `--no-compile-check`
disables it; a host with no C compiler is warned about, not silently skipped.

Pointer-parameter adaptation (E7b, opt-in by construction): a leaf whose only
pointer parameters are address-valued (level-1 primitive or `void`) is ported
with a **verbatim body** behind a thin host adapter.  The adapter signature
replaces each `T *p` parameter with `pe_addr_t p` (a guest address), derives a
translated host pointer `host$p = (T *)PE_Translate(p, 1)`, and the verbatim
body is rewritten mechanically: `p` -> `host$p`, a whole-word `*host$p` -> the
dereference `host$p`, and the generated `D_` accessor forms are unwrapped.  No
control flow, operand order, constant, or type is invented; only the address
convention and the load/store primitive are adapted.  Leaves that pass a
pointer through to a callee, store a host pointer into guest RAM (`**`), or use
a non-primitive pointee are reported, not guessed.
"""

from __future__ import annotations

import argparse
import concurrent.futures
import json
import os
import re
import shutil
import subprocess
import sys
import tempfile
from pathlib import Path

REPO_ROOT = Path(__file__).resolve().parents[2]
CONFIG = Path("configs/USA/disc1.yaml")
SRC_DIR = Path("src")
OUT_DIR = Path("pc_port/game/decomp")
EVIDENCE_DIR = Path("docs/evidence")

LOAD_VRAM = 0x80010000
HEADER_BYTES = 0x800
RAM_LO, RAM_HI = 0x80000000, 0x80200000

C_SPAN_RE = re.compile(r"-\s*\[(0x[0-9A-Fa-f]+),\s*c,\s*(func_[0-9A-Fa-f]{8})\]")
EDGE_RE = re.compile(r"^[ \t]*-[ \t]*\[(0x[0-9A-Fa-f]+)", re.MULTILINE)
ASM_RE = re.compile(r"__asm__|\basm\s*\(")
NUM_RE = re.compile(r"0x[0-9A-Fa-f]{8}\b")
FUNC_NAME_RE = re.compile(r"\b(func_[0-9A-Fa-f]{8})\b")
CALL_RE = re.compile(r"\b(func_[0-9A-Fa-f]{8})\s*\(")
DATA_NAME_RE = re.compile(r"\b(D_[0-9A-Fa-f]{7,8})\b")
EXTERN_RE = re.compile(r"^[ \t]*extern\s+(.+?);", re.MULTILINE)
DEF_RE = re.compile(
    r"^[A-Za-z_][A-Za-z0-9_ \t\*]*?\b(func_[0-9A-Fa-f]{8})\s*\([^;{]*?\)\s*\{",
    re.MULTILINE,
)
HDR_RE = re.compile(
    r"^[ \t]*[A-Za-z_][A-Za-z0-9_ \t\*]*?\b(func_[0-9A-Fa-f]{8})\s*\(([^;{]*?)\)\s*\{",
    re.MULTILINE,
)
BLOCK_COMMENT_RE = re.compile(r"/\*.*?\*/", re.S)
LINE_COMMENT_RE = re.compile(r"//[^\n]*")
PTR_CAST_RE = re.compile(
    r"\(\s*(?:struct\s+\w+\s*\*|unsigned\s+\w+\s*\*|signed\s+\w+\s*\*"
    r"|\w+\s*\*)\s*\)\s*([^;,)\]]{1,60})"
)
PTRPTR_STORE_RE = re.compile(r"\*\s*\([^()]*\*\s*\*[^()]*\)\s*(D_[0-9A-Fa-f]{7,8})")
# `func_XXX(` at a call site, with the balanced argument text.
CALL_HEAD_RE = re.compile(r"\b(func_[0-9A-Fa-f]{8})\s*\(")
PROTO_RE = re.compile(
    r"\b(func_[0-9A-Fa-f]{8})\s*\(([^;{}]*?)\)\s*;", re.MULTILINE
)
# Same prototypes as PROTO_RE, but with the return type captured.  Used by the
# E13/E14 host-pointer screens, which need the callee's host *types*, not only
# its arity.
SIG_RE = re.compile(
    r"(?:^|[;}])[ \t]*((?:extern[ \t]+)?[A-Za-z_][A-Za-z0-9_ \t\*]*?)"
    r"\b(func_[0-9A-Fa-f]{8})[ \t]*\(([^;{}]*?)\)[ \t]*;",
    re.MULTILINE,
)
# A statement that declares local objects: `int *p;`, `char buf[8];`,
# `unsigned char *p, *q;`.  The leading keyword filter keeps statements such as
# `return p[0];` out.
LOCAL_DECL_RE = re.compile(
    r"^[ \t]*((?:(?:register|static|const|volatile|unsigned|signed|struct|union|enum)"
    r"[ \t]+)*[A-Za-z_]\w*(?:[ \t]+[A-Za-z_]\w*)*)[ \t]+([^;{}()\n]+);[ \t]*$",
    re.MULTILINE,
)
# A cast to an *integer* type (no `*`): `(unsigned int)D_800119CC`, `(int)arg0`.
# Groups: 1 = the integer type, 2 = `&` or empty, 3 = the operand's root name.
INT_CAST_RE = re.compile(
    r"\(\s*((?:(?:unsigned|signed|long|short|const|volatile)[ \t]+)*"
    r"(?:int|long|short|char|unsigned|signed|pe_addr_t|uint32_t|size_t))\s*\)"
    r"\s*(&?)\s*([A-Za-z_]\w*)")
STATEMENT_KEYWORDS = {
    "return", "if", "else", "while", "for", "do", "switch", "case", "goto",
    "break", "continue", "sizeof", "default", "typedef",
}


def call_args(text: str) -> list[tuple[str, str]]:
    """Return (callee, argument text) for every call, paren-balanced."""
    out: list[tuple[str, str]] = []
    for m in CALL_HEAD_RE.finditer(text):
        i = m.end()
        depth = 1
        while i < len(text) and depth:
            if text[i] == "(":
                depth += 1
            elif text[i] == ")":
                depth -= 1
            i += 1
        if depth == 0:
            out.append((m.group(1), text[m.end(): i - 1]))
    return out


def arity(args: str) -> int:
    args = args.strip()
    if not args or args == "void":
        return 0
    depth = 0
    n = 1
    for ch in args:
        if ch in "([":
            depth += 1
        elif ch in ")]":
            depth -= 1
        elif ch == "," and depth == 0:
            n += 1
    return n


def canonical_protos() -> dict[str, int]:
    """func_* symbol -> parameter count from the pc_port shim headers."""
    proto: dict[str, int] = {}
    roots = [REPO_ROOT / "pc_port" / "include", REPO_ROOT / "pc_port" / "platform",
             REPO_ROOT / "pc_port" / "game"]
    for root in roots:
        if not root.is_dir():
            continue
        for path in root.rglob("*.h"):
            if "build" in path.parts:
                continue
            text = path.read_text(encoding="utf-8", errors="replace")
            for m in PROTO_RE.finditer(strip_comments(text)):
                proto.setdefault(m.group(1), arity(m.group(2)))
    return proto


def shim_header_roots() -> list[Path]:
    return [REPO_ROOT / "pc_port" / "include", REPO_ROOT / "pc_port" / "platform",
            REPO_ROOT / "pc_port" / "game"]


def canonical_signatures() -> dict[str, dict]:
    """func_* symbol -> {'ret': <return type>, 'params': [<parameter decls>]}.

    The pc_port shim headers are the host authority for a callee's type.  A
    leaf's own `extern` declaration describes the *retail* type (where an
    `int *` is a 32-bit guest address), so the two must be compared before a
    pointer is allowed to cross a call boundary.
    """
    sigs: dict[str, dict] = {}
    for root in shim_header_roots():
        if not root.is_dir():
            continue
        for path in sorted(root.rglob("*.h")):
            if "build" in path.parts:
                continue
            text = strip_comments(path.read_text(encoding="utf-8", errors="replace"))
            for m in SIG_RE.finditer(text):
                name = m.group(2)
                if name in sigs:
                    continue
                ret = re.sub(r"^\s*extern\s+", "", m.group(1)).strip()
                sigs[name] = {"ret": ret, "params": split_top(m.group(3))}
    return sigs


def split_top(text: str) -> list[str]:
    """Split a parameter/argument list on top-level commas."""
    text = text.strip()
    if not text or text == "void":
        return []
    parts: list[str] = []
    depth = 0
    cur = ""
    for ch in text:
        if ch in "([":
            depth += 1
        elif ch in ")]":
            depth -= 1
        if ch == "," and depth == 0:
            parts.append(cur.strip())
            cur = ""
        else:
            cur += ch
    parts.append(cur.strip())
    return parts


def is_host_pointer_type(decl: str) -> bool:
    """Does this host declaration denote a pointer (as opposed to pe_addr_t)?"""
    return "*" in decl or re.search(r"\[\s*\w*\s*\]", decl) is not None


def local_objects(body: str) -> tuple[set[str], set[str]]:
    """(every local declared in the leaf, those that are host memory).

    `int *p;` holds a host address once assigned and `char buf[8];` decays to
    one; neither is a guest address.  A plain scalar local is only host memory
    when its address is taken (`&n`), which the caller checks.
    """
    every: set[str] = set()
    pointerish: set[str] = set()
    for m in LOCAL_DECL_RE.finditer(body):
        head = m.group(1).split()
        if head[0] in STATEMENT_KEYWORDS or head[-1] in STATEMENT_KEYWORDS:
            continue
        for declarator in split_top(m.group(2)):
            ident = re.search(r"([A-Za-z_]\w*)", declarator)
            if not ident:
                continue
            every.add(ident.group(1))
            if declarator.lstrip().startswith("*") or re.search(
                    r"\[\s*[^\]]*\]", declarator):
                pointerish.add(ident.group(1))
    return every, pointerish


def strip_comments(text: str) -> str:
    return LINE_COMMENT_RE.sub("", BLOCK_COMMENT_RE.sub("", text))


def vram_of(file_offset: int) -> int:
    return LOAD_VRAM + file_offset - HEADER_BYTES


def parse_matched() -> dict[str, dict]:
    cfg = (REPO_ROOT / CONFIG).read_text(encoding="utf-8")
    offsets = [int(m.group(1), 16) for m in EDGE_RE.finditer(cfg)]
    out: dict[str, dict] = {}
    for m in C_SPAN_RE.finditer(cfg):
        start = int(m.group(1), 16)
        end = next((o for o in offsets if o > start), None)
        if end is None:
            raise SystemExit(f"gen_decomp_ports: cannot resolve end of {m.group(2)}")
        out[m.group(2)] = {
            "name": m.group(2),
            "file_offset": start,
            "file_size": end - start,
            "vram": vram_of(start),
            "words": (end - start) // 4,
        }
    return out


def pc_port_definitions() -> dict[str, set[str]]:
    """func_* symbol -> set of pc_port source files that define it."""
    defined: dict[str, set[str]] = {}
    for path in (REPO_ROOT / "pc_port").rglob("*.c"):
        if "build" in path.parts:
            continue
        rel = path.relative_to(REPO_ROOT).as_posix()
        text = path.read_text(encoding="utf-8", errors="replace")
        for m in DEF_RE.finditer(text):
            defined.setdefault(m.group(1), set()).add(rel)
    return defined


def pc_port_macros() -> set[str]:
    """D_ symbols already provided as macros by the pc_port shim headers."""
    names: set[str] = set()
    for rel in ("pc_port/include/psx_compat.h", "pc_port/include/pe_port_compat.h"):
        text = (REPO_ROOT / rel).read_text(encoding="utf-8", errors="replace")
        for m in re.finditer(r"^[ \t]*#define\s+(D_[0-9A-Fa-f]{7,8})\b", text,
                             re.MULTILINE):
            names.add(m.group(1))
    return names


def declared_type(decl: str) -> str:
    """Extract the element type from `extern <type> NAME[, NAME2 ...];`."""
    cut = DATA_NAME_RE.search(decl)
    body = decl[: cut.start()] if cut else decl
    return re.sub(r"\s+", " ", body).strip() or "unsigned int"


def pointer_pointee(etype: str) -> str:
    """Pointee of a pointer-global declaration.

    `unsigned char *` -> `unsigned char`, and `unsigned int *volatile`
    (a volatile *slot*, not a volatile pointee) -> `unsigned int`.  Dropping
    the slot qualifier is behaviour-preserving: PE_DECOMP_PTRGLOBAL re-reads
    the guest word with PE_LoadU32 on every expansion anyway.
    """
    t = re.sub(r"\s*\*\s*(?:(?:const|volatile)\s*)*$", "", etype).strip()
    return t or "unsigned char"


def classify_data(decl: str, sym: str) -> str:
    if "[" in decl:
        return "array"
    head = decl[: DATA_NAME_RE.search(decl).start()] if DATA_NAME_RE.search(decl) else decl
    if "*" in head:
        return "ptrglobal"
    return "scalar"


def parse_params(header: str) -> tuple[list[str], bool]:
    """Return (parameter names, has_pointer_parameter)."""
    inner = header.strip()
    if inner in ("", "void"):
        return [], False
    parts: list[str] = []
    depth = 0
    cur = ""
    for ch in inner:
        if ch in "([":
            depth += 1
        elif ch in ")]":
            depth -= 1
        if ch == "," and depth == 0:
            parts.append(cur)
            cur = ""
        else:
            cur += ch
    parts.append(cur)
    names: list[str] = []
    ptr = False
    for p in parts:
        p = p.strip()
        if not p or p == "void":
            continue
        if "*" in p or re.search(r"\[\s*\]", p):
            ptr = True
        m = re.search(r"(\w+)\s*(\[\s*\])?\s*$", p)
        if m:
            names.append(m.group(1))
    return names, ptr


C_KEYWORDS = {
    "void", "char", "short", "int", "long", "unsigned", "signed", "float",
    "double", "const", "volatile", "struct", "union", "enum", "restrict",
    "_Bool",
}
DOUBLE_PTR_TOKEN_RE = re.compile(r"\*\s*\*")
PTR_DEREF_CAST_RE = re.compile(
    r"\*\s*\(\s*(?:[A-Za-z_]\w*\s+)*[A-Za-z_]\w*\s*\*+\s*\*")


def pointer_params(inner: str) -> list[tuple[str, int, list[str], str]]:
    """(name, pointer level, named-type words, declaration) for pointer params."""
    out: list[tuple[str, int, list[str], str]] = []
    for part in inner.split(","):
        p = part.strip()
        if not p or p == "void":
            continue
        m = re.search(r"(\w+)\s*$", p)
        name = m.group(1) if m else ""
        # Keep the full text before the parameter's own name: array binders
        # like `unsigned char a0[4]` leave the trailing `[4]` after the name.
        decl = p[: m.end()].strip() if m else p
        toks = [t for t in re.findall(r"[A-Za-z_]\w*", decl)
                if t not in C_KEYWORDS and t != name]
        level = p.count("*") + (1 if re.search(r"\[\s*\]", p) else 0)
        if level > 0:
            out.append((name, level, toks, p))
    return out


def wrapper_safe(name: str, stripped: str, inner: str) -> tuple[bool, str]:
    """Can this pointer-param leaf be adapted by the verbatim-wrapper recipe?"""
    if DOUBLE_PTR_TOKEN_RE.search(stripped):
        return False, "pointer-to-pointer value (guest pointer in RAM)"
    # A pointer parameter whose address is stored (e.g. `p->link = arg`) writes
    # a host pointer into guest RAM; only value/address uses are adaptable.
    for pname, _lvl, _toks, _decl in pointer_params(inner):
        if re.search(rf"&\s*{re.escape(pname)}\b", stripped):
            return False, f"address of pointer parameter {pname}"
        # Storing a host pointer into guest RAM (`dst->source = source;`) is not
        # representable: the guest word must hold a 32-bit guest address.  An
        # indexed use (`dst->value = arg0[1];`) is a load of guest RAM through
        # the parameter, not a pointer store, so it stays adaptable.
        if re.search(
                rf"(?:->|\.|\[)[^;]*?=[^=]\s*{re.escape(pname)}\b(?!\s*\[)",
                stripped):
            return False, f"pointer parameter {pname} stored into guest RAM"
    # A recursive self-call passes a host pointer where the adapted signature
    # takes a guest address; that needs a reverse translation, so it is
    # reported rather than guessed.
    inner_body = extract_body(stripped, name)
    if inner_body is not None and re.search(
            rf"\b{re.escape(name)}\s*\(", inner_body):
        return False, "recursive self-call"
    if PTR_DEREF_CAST_RE.search(stripped):
        return False, "store through a pointer-dereference cast"
    for pname, level, _toks, _decl in pointer_params(inner):
        if level > 1:
            return False, f"parameter {pname} is level-{level}"
    if re.search(r"&\s*func_[0-9A-Fa-f]{8}", stripped):
        return False, "address of function"
    return True, ""


def analyze(name: str, src_text: str, matched: dict,
            known: set[str], existing_macros: set[str],
            proto: dict[str, int], sigs: dict[str, dict]) -> dict:
    stripped = strip_comments(src_text)
    if ASM_RE.search(stripped):
        return {"eligible": False, "reason": "asm"}
    # Reject hardware / BIOS address spaces (MMIO 0x1F8xxxxx, scratchpad
    # 0x1F800000, BIOS ramps 0xA0xxxxxx/0xB0xxxxxx).  Other 32-bit literals are
    # ordinary masks/constants and stay verbatim.
    for lit in NUM_RE.findall(stripped):
        hi = int(lit, 16) & 0xFF000000
        if hi in (0x1F000000, 0xA0000000, 0xB0000000):
            return {"eligible": False, "reason": f"hardware/BIOS literal {lit}"}
    if DEF_RE.findall(stripped) != [name]:
        return {"eligible": False, "reason": "definition set"}
    if re.search(r"\(\s*\*\s*\w+\s*\)\s*\(", stripped):
        return {"eligible": False, "reason": "indirect call through local"}

    hdr = HDR_RE.search(stripped)
    if hdr is None:
        return {"eligible": False, "reason": "no function header"}
    ret_type = hdr.group(0)[: hdr.group(0).find(name)]
    if "*" in ret_type:
        return {"eligible": False, "reason": "pointer return type"}
    params, has_ptr_param = parse_params(hdr.group(2))
    wrap: dict | None = None
    if has_ptr_param:
        ok, why = wrapper_safe(name, stripped, hdr.group(2))
        if not ok:
            return {"eligible": False, "reason": f"pointer parameter: {why}"}
        wrap = {"param_inner": hdr.group(2)}
    # A callee returning a guest address is likewise not representable: the
    # call result is a 32-bit guest pointer, and indexing/dereferencing it in
    # C would need a host pointer.  Such leaves need a hand-written adapter.
    if re.search(r"\bfunc_[0-9A-Fa-f]{8}\s*\([^;]*?\)\s*(\[|->|\.)", stripped):
        return {"eligible": False, "reason": "callee result dereferenced"}

    if re.search(r"&\s*func_[0-9A-Fa-f]{8}", stripped):
        return {"eligible": False, "reason": "address of function"}
    # A bare `func_XXX` not followed by `(` is a function address used as a
    # value (a callback argument).  32-bit code pointers are not host
    # pointers, so those leaves need a hand-written adapter.
    for m in FUNC_NAME_RE.finditer(stripped):
        if not stripped[m.end():].lstrip().startswith("("):
            return {"eligible": False,
                    "reason": f"function address value {m.group(1)}"}
    if PTRPTR_STORE_RE.search(stripped):
        return {"eligible": False, "reason": "pointer-pointer store to D_ symbol"}

    externs: list[tuple[str, str]] = []
    for m in EXTERN_RE.finditer(stripped):
        decl = m.group(1)
        for sym in DATA_NAME_RE.findall(decl):
            externs.append((sym, decl))

    # locals/aggregates assigned from a D_ symbol: a pointer cast whose operand
    # mentions one of them is demonstrably a guest address, not a value param.
    from_d = set(re.findall(r"\b(\w+)\s*=\s*[^;{}]*?\bD_[0-9A-Fa-f]{7,8}\b",
                            stripped))
    from_d |= {s for s, _ in externs}
    # A pointer parameter is, by the wrapper's contract, a guest address (it is
    # translated to a host pointer in the adapter), so a pointer cast applied to
    # it is an address reinterpretation/harmless requalification, not a value
    # parameter used as an address.
    from_d |= {p for p, _lvl, _toks, _d in pointer_params(hdr.group(2))}
    for m in PTR_CAST_RE.finditer(stripped):
        operand = m.group(1)
        if set(re.findall(r"\b\w+\b", operand)) & from_d:
            continue
        return {"eligible": False,
                "reason": f"pointer cast of non-D_ operand: {operand.strip()[:32]}"}

    callees = sorted(set(CALL_RE.findall(stripped)) - {name})

    # A callee with a canonical pc_port prototype must be called with the same
    # arity, or the host build would reinterpret its arguments.  The matched
    # leaf is the authority for the call site, so a mismatch is reported for a
    # hand-written adapter instead of being coerced by codegen.
    for callee, args in call_args(stripped):
        if callee == name or callee not in proto:
            continue
        if arity(args) != proto[callee]:
            return {"eligible": False,
                    "reason": f"call arity {callee} {arity(args)}!={proto[callee]}"}

    # E13/E14 — a host pointer must not cross a call boundary in either
    # direction.  Retail passes and returns 32-bit *guest* addresses; host-side
    # the callee is a canonical `pe_addr_t` prototype or a loud boundary
    # returning `int`, so either conversion silently reinterprets a host
    # pointer as a guest address (or vice versa).  The leaf's own `extern`
    # declaration describes the retail type and is neutralised by the
    # generator, so it cannot rescue the call site.
    body_text = extract_body(stripped, name)
    _all_locals, host_locals = local_objects(
        body_text if body_text is not None else stripped)
    # A pointer *parameter* is not host memory at a call site: the E7b wrapper
    # forwards it as `pe_<name>`, the guest address the callee's `pe_addr_t`
    # parameter wants.  It is still a host pointer as an assignment *target*
    # (the wrapper rewrites it to the translated `host_<name>`).
    assign_targets = host_locals | {
        p for p, _lvl, _toks, _d in pointer_params(hdr.group(2))}

    def host_return(callee: str) -> bool:
        """Does the callee's *host* declaration return a pointer?"""
        if callee in sigs:
            return is_host_pointer_type(sigs[callee]["ret"])
        if callee not in known:
            return False  # becomes a loud boundary macro: returns int
        return True  # a pc_port definition with no shim prototype: leaf's own

    for m in re.finditer(r"\b([A-Za-z_]\w*)\s*=\s*(func_[0-9A-Fa-f]{8})\s*\(",
                         stripped):
        if m.group(1) in assign_targets and not host_return(m.group(2)):
            return {"eligible": False,
                    "reason": f"guest-address result of {m.group(2)} bound to "
                              f"host pointer {m.group(1)}"}
    for m in re.finditer(
            r"\*+\s*([A-Za-z_]\w*)\s*=\s*(func_[0-9A-Fa-f]{8})\s*\(", stripped):
        # `T *p = func_X(...)` — a declaration with an initialiser, which the
        # local-declaration scan deliberately does not parse.
        decl_head = stripped[: m.start()].rstrip().rsplit("\n", 1)[-1].strip()
        if re.fullmatch(r"(?:[A-Za-z_]\w*\s+)+", decl_head + " ") and \
                not host_return(m.group(2)):
            return {"eligible": False,
                    "reason": f"guest-address result of {m.group(2)} bound to "
                              f"host pointer {m.group(1)}"}

    for callee, args in call_args(stripped):
        if callee == name or callee not in sigs:
            continue
        hparams = sigs[callee]["params"]
        for i, arg in enumerate(split_top(args)):
            bare = arg.strip().strip("()").strip()
            if bare.startswith("&"):
                # `&local`, `&D_SYM[i]`, `&s.field` — address-of always yields a
                # *host* address in the generated TU, where retail took a guest
                # one.  (A bare `D_SYM` argument is not this case;
                # fix_argument_addresses substitutes the guest address.)
                pass
            elif re.fullmatch(r"[A-Za-z_]\w*", bare):
                if bare not in host_locals:
                    continue
            else:
                continue
            if i < len(hparams) and not is_host_pointer_type(hparams[i]):
                return {"eligible": False,
                        "reason": f"host pointer {bare} passed to {callee} "
                                  f"parameter {i} ({hparams[i].strip()})"}

    # A func_* referenced but never called is a function-address value; those
    # were rejected above.  Everything else is a known definition or becomes a
    # loud boundary.
    boundaries = [c for c in callees if c not in known]
    boundary_arity: dict[str, int] = {}
    for callee, args in call_args(stripped):
        if callee in boundaries:
            boundary_arity.setdefault(callee, min(arity(args), 4))

    # Assigning the pointer-global *slot itself* stores a 32-bit guest address
    # where the host type is a pointer, which the generated accessor cannot
    # represent.  A *dereferenced* store (`*D_80095854 = x`) is fine: the
    # accessor already yields the translated pointer.
    for sym, decl in externs:
        if classify_data(decl, sym) != "ptrglobal":
            continue
        if re.search(rf"(?<!\*)\b{sym}\b\s*(=|\+=|-=|\|=|&=|\^=)", stripped):
            return {"eligible": False, "reason": f"pointer-global store {sym}"}

    # E15 — a guest array (or global) *of pointers*.  `extern unsigned char
    # *D_800B0E38[];` is 4-byte guest slots; the generated accessor types the
    # element as a host pointer, so `D_800B0E38[i]` would read 8 bytes on a
    # 64-bit host and yield a host pointer where retail has a guest address.
    # One level of indirection is fine (PE_DECOMP_PTRGLOBAL loads the 32-bit
    # slot and translates it); two is not representable.
    for sym, decl in externs:
        head = decl[: DATA_NAME_RE.search(decl).start()]
        stars = head.count("*")
        if stars >= 2 or (stars >= 1 and "[" in decl):
            return {"eligible": False,
                    "reason": f"guest array/global of pointers {sym} "
                              f"({' '.join(decl.split())[:40]})"}

    # E16 — a host pointer narrowed to an integer.  `(unsigned int)D_800119CC`
    # is the retail *guest address* of the symbol; host-side the accessor is a
    # 64-bit host pointer, so the cast truncates it.  The value is then either
    # returned, stored, or cast back and dereferenced — silently wrong code
    # (only a -Wpointer-to-int-cast warning), which is worse than a rejection.
    host_roots = {sym for sym, decl in externs
                  if classify_data(decl, sym) in ("array", "ptrglobal")}
    host_roots |= host_locals
    host_roots |= {p for p, _lvl, _toks, _d in pointer_params(hdr.group(2))}
    for m in INT_CAST_RE.finditer(stripped):
        root = m.group(3)
        if m.group(2) == "&":
            # `(unsigned int)&D_XXXX[i]` — the address of guest storage, taken
            # host-side and then truncated.
            if root not in host_roots and root not in _all_locals \
                    and root not in {s for s, _ in externs}:
                continue
        else:
            # Without `&` the cast must apply to the object itself, not to a
            # value read through it: `(signed char)D_XXXX[i].b` casts the
            # member, which is fine.
            tail = stripped[m.end():].lstrip()
            if tail[:1] in ("[", ".", "(") or tail[:2] == "->":
                continue
            if root not in host_roots:
                continue
        return {"eligible": False,
                "reason": f"host pointer {root} narrowed to "
                          f"{m.group(1).strip()}"}

    data_types = {sym: decl for sym, decl in externs}
    d_syms_used = [s for s in data_types
                   if re.search(rf"\b{re.escape(s)}\b", stripped)]
    return {
        "eligible": True,
        "params": params,
        "header": hdr.group(0),
        "externs": externs,
        "data_types": data_types,
        "d_syms_used": d_syms_used,
        "callees": callees,
        "boundaries": boundaries,
        "boundary_arity": boundary_arity,
        "shadowed": [s for s, _ in externs if s in existing_macros],
        "body": src_text,
        "wrap": wrap,
        "param_inner": hdr.group(2) if wrap else "",
    }


def neutralize_declarations(body: str, shimmed_data: set[str],
                            boundary_funcs: set[str],
                            canonical: set[str]) -> str:
    """Comment out leaf declarations that generated macros / pc_port now own.

    A `#define D_800942E4 ...` would rewrite the leaf's own `extern` into
    nonsense, so those declarations are replaced with a provenance comment.
    Likewise a callee the port links against a canonical `pe_port_compat.h`
    prototype, or a loud boundary: keeping the leaf's redeclaration risks a
    signedness/arity clash with the host type, and `src/` stays authoritative
    for the call site, not the host prototype.  A callee that is itself a
    decomp-derived leaf keeps the leaf's own prototype verbatim (there is no
    canonical host header for it, and the prototype is part of the matched
    unit).  Everything else (typedefs, structs, unrelated prototypes) stays
    byte-for-byte.
    """
    def names_in(text: str) -> list[str]:
        return DATA_NAME_RE.findall(text)

    def mention(text: str) -> bool:
        if DATA_NAME_RE.search(text):
            return True
        for f in FUNC_NAME_RE.findall(text):
            if f in boundary_funcs or f in canonical:
                return True
        # A bare prototype of a decomp-derived leaf is the callee's host
        # prototype (there is no canonical header for it); keep it verbatim.
        return False

    patterns = [
        EXTERN_RE,
        # A bare file-scope prototype of a symbol the port already provides
        # (`func_80050708`)'s `void func_80064C54(int value);`) would clash with
        # the canonical pe_port_compat.h type; the matched call site is what
        # matters, so the redeclaration is replaced by a provenance comment.
        # A file-scope prototype ends its line with `;` (no initializer, no
        # statement text after it); `return f(...);` and other call statements
        # must not be mistaken for a declaration.
        re.compile(
            r"^(?:extern\s+)?[A-Za-z_][A-Za-z0-9_ \t\*]*?"
            r"\b(func_[0-9A-Fa-f]{8})\s*\([^\n;{]*?\)\s*;[ \t]*$",
            re.MULTILINE,
        ),
    ]
    spans: list[tuple[int, int, str]] = []
    for pat in patterns:
        for m in pat.finditer(body):
            decl = m.group(1)
            if mention(decl):
                spans.append((m.start(), m.end(), decl))
    spans.sort()
    out: list[str] = []
    pos = 0
    for start, end, decl in spans:
        if start < pos:
            continue
        out.append(body[pos:start])
        names = list(dict.fromkeys(
            names_in(decl) + FUNC_NAME_RE.findall(decl)))
        out.append("/* shimmed by pe_guest_decomp.h: " + ", ".join(names) + " */")
        pos = end
    out.append(body[pos:])
    return "".join(out)


def fix_argument_addresses(body: str, data_syms: set[str]) -> str:
    """Pass guest *addresses* where a bare data symbol is a call argument.

    In the matched leaf `f(arg0, D_800E0824)` the array decays to the retail
    guest address.  Host-side the macro expands to a host pointer, which is not
    a `pe_addr_t`; substituting the guest address literal keeps the argument
    semantics (and the call arity) retail-accurate.
    """
    def repl(m: re.Match) -> str:
        sym = m.group(2)
        if sym not in data_syms:
            return m.group(0)
        return f"{m.group(1)}(pe_addr_t)0x{sym[2:]}u{m.group(3)}"

    return re.sub(r"([(,]\s*)(D_[0-9A-Fa-f]{7,8})(\s*[,)])", repl, body)


def render(name: str, plan: dict, matched: dict, known_funcs: set[str],
           proto: dict[str, int]) -> str:
    leaf = matched[name]
    report = EVIDENCE_DIR / name / "REPORT.md"
    if plan["wrap"]:
        return render_wrapper(name, plan, leaf, report, proto)
    lines: list[str] = []
    lines.append(
        "/*\n"
        f" * decomp-source: {name}\n"
        f" * matched VMA 0x{leaf['vram']:08X}  file 0x{leaf['file_offset']:X}"
        f"  span 0x{leaf['file_size']:X} bytes / {leaf['words']} words\n"
        f" * evidence: {report.as_posix()}\n"
        " *\n"
        " * GENERATED by tools/analysis/gen_decomp_ports.py from"
        f" src/{name}.c — do not\n"
        " * edit by hand; the matching leaf is the authority.  The body is copied\n"
        " * verbatim; only the host adaptation (guest-RAM data symbols + loud\n"
        " * boundaries) is generated.  See docs/ai_context/PC_PORT_FROM_DECOMP.md.\n"
        " */\n"
    )
    lines.append('#include "pe_guest_decomp.h"\n')

    emitted_data = False
    for sym, decl in plan["externs"]:
        if sym in plan["shadowed"]:
            # The shim header may declare this address with a different host
            # type; the matched leaf's own declaration wins, so rebind it.
            lines.append(f"#undef {sym}")
        kind = classify_data(decl, sym)
        etype = declared_type(decl)
        if kind == "scalar":
            lines.append(f"#define {sym} PE_DECOMP_SCALAR(0x{sym[2:]}u, {etype})")
        elif kind == "array":
            lines.append(f"#define {sym} PE_DECOMP_ARRAY(0x{sym[2:]}u, {etype})")
        else:
            pointee = pointer_pointee(etype)
            lines.append(
                f"#define {sym} PE_DECOMP_PTRGLOBAL(0x{sym[2:]}u, {pointee})")
        emitted_data = True
    if emitted_data:
        lines.append("")

    for callee in plan["boundaries"]:
        lines.append(
            f"/* boundary: {callee} has no pc_port implementation yet */"
        )
        arity_n = plan["boundary_arity"][callee]
        if arity_n == 0:
            lines.append(
                f"#define {callee}(...) "
                f'PE_D_COMP_BOUNDARY0("{callee}", 0x{callee[5:]}u)'
            )
        else:
            lines.append(
                f"#define {callee}(...) "
                f'PE_D_COMP_BOUNDARY{arity_n}('
                f'"{callee}", 0x{callee[5:]}u, __VA_ARGS__)'
            )
        lines.append("")
    if plan["boundaries"]:
        lines.append("")

    shimmed_data = {s for s, _ in plan["externs"] if s not in plan["shadowed"]}
    body = neutralize_declarations(plan["body"].rstrip("\n"), shimmed_data,
                                   set(plan["boundaries"]), set(proto))

    lines.append("/* ── verbatim matching leaf (src/%s.c) ───────────────────────── */" % name)
    lines.append("")
    lines.append(fix_argument_addresses(body, shimmed_data) + "\n")
    return "\n".join(lines) + "\n"


def param_base_type(decl: str, pname: str) -> str:
    """Base element type of a pointer parameter declaration.

    Handles `unsigned char *a0`, `short *a1 = x`, `unsigned char a0[4]`, and
    `int **a0`.  Array binders and initializers are dropped.
    """
    text = decl
    if pname:
        text = re.sub(rf"\b{re.escape(pname)}\b.*$", "", text)
    text = re.sub(r"\[[^\]]*\]", " ", text)
    text = re.sub(r"\*+", " ", text)
    text = re.sub(r"\b(const|volatile|register|restrict)\b", " ", text)
    return re.sub(r"\s+", " ", text).strip() or "unsigned char"


def normalize_base(decl: str) -> str:
    return re.sub(r"\s+", "", decl).replace("const", "").replace("volatile", "")


def wrapper_casts_ok(stripped: str, inner: str) -> tuple[bool, str]:
    """Every pointer cast applied directly to a pointer parameter must match
    the parameter's declared base type, so deleting the cast is a no-op."""
    for pname, level, _toks, decl in pointer_params(inner):
        if level != 1:
            continue
        pbase = normalize_base(re.sub(r"\*+\s*$", "", decl))
        for m in re.finditer(rf"\(([^()]*?\*+)\)\s*{re.escape(pname)}\b", stripped):
            cbase = normalize_base(re.sub(r"\*+\s*$", "", m.group(1)))
            if cbase and cbase != pbase:
                return False, f"cast {m.group(1)} on {pname} ({decl}) changes width"
    return True, ""


WRAPPER_SIG_RE = re.compile(
    r"^([A-Za-z_][A-Za-z0-9_ \t\*]*?\b)(func_[0-9A-Fa-f]{8})\s*\(([^;{]*?)\)\s*\{",
    re.MULTILINE,
)


def extract_typedefs(text: str) -> str:
    """Top-level `typedef ...;` declarations from a leaf, brace-balanced."""
    out: list[str] = []
    for m in re.finditer(r"^[ \t]*typedef\b", text, re.MULTILINE):
        i = m.end()
        depth = 0
        while i < len(text):
            if text[i] == "{":
                depth += 1
            elif text[i] == "}":
                depth -= 1
            elif text[i] == ";" and depth == 0:
                i += 1
                break
            i += 1
        out.append(text[m.start():i].strip())
    return "\n".join(out)


def extract_body(text: str, name: str) -> str | None:
    """Inner text of the named `func_*` definition, brace-balanced, without the
    opening/closing braces.  Works from the `{` after the parameter list."""
    for m in WRAPPER_SIG_RE.finditer(text):
        if m.group(2) != name:
            continue
        depth = 1
        i = m.end()
        while i < len(text) and depth:
            if text[i] == "{":
                depth += 1
            elif text[i] == "}":
                depth -= 1
                if depth == 0:
                    return text[m.end(): i]
            i += 1
    return None


def render_wrapper(name: str, plan: dict, leaf: dict, report: Path,
                   proto: dict[str, int]) -> str:
    """Emit a pointer-parameter leaf behind a guest-address host adapter.

    The signature and the `extern` data declarations change (address convention
    only); the function body is the matched leaf's, moved verbatim behind
    `#define` alias macros that route guest addresses through `PE_Translate`.
    `src/` is untouched.
    """
    stripped = strip_comments(plan["body"])
    sig = None
    for m in WRAPPER_SIG_RE.finditer(stripped):
        if m.group(2) == name:
            sig = m
    assert sig is not None
    ret_type = sig.group(1).strip()

    pparams = pointer_params(plan["param_inner"])
    pnames = {p for p, _, _, _ in pparams}

    # Wrapper signature: pointer params become guest addresses; others verbatim.
    sig_params: list[str] = []
    for part in plan["param_inner"].split(","):
        p = part.strip()
        if not p:
            continue
        m = re.search(r"(\w+)\s*$", p)
        pname = m.group(1) if m else ""
        if pname in pnames:
            sig_params.append(f"pe_addr_t pe_{pname}")
        else:
            sig_params.append(p)

    # The verbatim body runs inside its own generated function.  Each guest
    # symbol becomes a local host pointer (`host_NAME`) and every occurrence of
    # the leaf's name is textually substituted, so a scalar/array/pointer-global
    # access keeps exactly the leaf's expression shape.  A whole-word leaf
    # dereference `*NAME` replaces the generated macro dereference; a bare
    # `NAME` used as a call argument was already rewritten to a guest address
    # literal, so the remaining bare forms are array/pointer-global values.
    body = plan["body"].rstrip("\n")
    body = neutralize_declarations(body, set(), set(plan["boundaries"]),
                                   set(proto))
    body = fix_argument_addresses(body, set(plan["d_syms_used"]))
    for sym in sorted(plan["d_syms_used"], key=len, reverse=True):
        kind = classify_data(plan["data_types"][sym], sym)
        if kind == "scalar":
            # Scalar leaf access is an lvalue; the macro becomes the pointee.
            body = re.sub(rf"\*\s*{re.escape(sym)}\b", f"host_{sym}", body)
            body = re.sub(rf"\b{re.escape(sym)}\b", f"(*host_{sym})", body)
        else:
            body = re.sub(rf"\b{re.escape(sym)}\b", f"host_{sym}", body)
    for pname, _lvl, _toks, decl in pparams:
        body = re.sub(
            rf"\(\s*{re.escape(re.sub(r'\*+\s*$', '', decl).strip())}\s*\*+\s*\)"
            rf"\s*{re.escape(pname)}\b",
            f"host_{pname}", body)
        # A pointer parameter *is* the guest address; when the leaf hands it
        # straight to a callee, the callee's host prototype takes a `pe_addr_t`
        # (canonical) or the boundary records the guest argument, so the guest
        # address is what must be forwarded.  Memory access still goes through
        # the translated `host_` pointer.
        body = re.sub(
            rf"(?<=[(,])\s*{re.escape(pname)}\s*(?=[,)])", f" pe_{pname}", body)
        # A struct member with the same name is not the parameter.
        body = re.sub(rf"(?<![.>])\b{re.escape(pname)}\b", f"host_{pname}", body)
    body_only = extract_body(body, name)
    assert body_only is not None, name

    lines: list[str] = []
    lines.append(
        "/*\n"
        f" * decomp-source: {name}\n"
        f" * matched VMA 0x{leaf['vram']:08X}  file 0x{leaf['file_offset']:X}"
        f"  span 0x{leaf['file_size']:X} bytes / {leaf['words']} words\n"
        f" * evidence: {report.as_posix()}\n"
        " *\n"
        " * GENERATED by tools/analysis/gen_decomp_ports.py from"
        f" src/{name}.c — do not\n"
        " * edit by hand; the matching leaf is the authority.  Pointer parameters\n"
        " * are adapted from host pointers to guest addresses (`pe_addr_t`); the\n"
        " * function body is otherwise verbatim.  See\n"
        " * docs/ai_context/PC_PORT_FROM_DECOMP.md.\n"
        " */\n"
    )
    lines.append('#include "pe_guest_decomp.h"\n')
    lines.append("")

    typedefs = extract_typedefs(stripped)
    if typedefs:
        lines.append("/* verbatim leaf types */\n")
        lines.append(typedefs + "\n\n")

    # The wrapper emits only the definition body, so a callee whose host
    # prototype is not in a canonical header still needs the leaf's own
    # declaration (verbatim) to be visible here.
    callee_protos: list[str] = []
    for m in re.finditer(
            r"^[ \t]*(?:extern\s+)?[A-Za-z_][A-Za-z0-9_ \t\*]*?"
            r"\b(func_[0-9A-Fa-f]{8})\s*\([^\n;{]*?\)\s*;[ \t]*$",
            stripped, re.MULTILINE):
        callee = m.group(1)
        if callee == name or callee in proto or callee in plan["boundaries"]:
            continue
        callee_protos.append(re.sub(r"[ \t]+", " ",
                                    m.group(0).strip()))
    if callee_protos:
        lines.append("/* verbatim callee declarations */\n")
        for decl in dict.fromkeys(callee_protos):
            lines.append(decl + "\n")
        lines.append("\n")

    for callee in plan["boundaries"]:
        lines.append(f"/* boundary: {callee} has no pc_port implementation yet */\n")
        arity_n = plan["boundary_arity"][callee]
        if arity_n == 0:
            lines.append(
                f'#define {callee}(...) PE_D_COMP_BOUNDARY0("{callee}", 0x{callee[5:]}u)\n')
        else:
            lines.append(
                f'#define {callee}(...) PE_D_COMP_BOUNDARY{arity_n}('
                f'"{callee}", 0x{callee[5:]}u, __VA_ARGS__)\n')
        lines.append("\n")
    if plan["boundaries"]:
        lines.append("")

    lines.append(f"{ret_type} {name}({', '.join(sig_params)})\n{{\n")
    for decl_line in _host_locals(plan, pparams, body_only):
        lines.append(decl_line + "\n")
    lines.append("    /* verbatim body (src/%s.c) */\n" % name)
    for bl in body_only.split("\n"):
        lines.append(bl + "\n")
    lines.append("}\n")
    return "".join(lines)


def _host_locals(plan: dict, pparams: list, body: str) -> list[str]:
    """Translated host locals for the symbols/parameters the body actually
    dereferences.  A value forwarded to a callee or boundary keeps only the
    guest address in the body, so no host pointer local is emitted for it."""
    out: list[str] = []
    for sym in plan["d_syms_used"]:
        if sym in plan["shadowed"]:
            continue
        if f"host_{sym}" not in body:
            continue
        kind = classify_data(plan["data_types"][sym], sym)
        etype = declared_type(plan["data_types"][sym])
        if kind == "scalar":
            out.append(f"    {etype} *host_{sym} = &PE_DECOMP_SCALAR(0x{sym[2:]}u, {etype});")
        elif kind == "array":
            out.append(f"    {etype} *host_{sym} = PE_DECOMP_ARRAY(0x{sym[2:]}u, {etype});")
        else:
            pointee = pointer_pointee(etype)
            out.append(f"    {pointee} *host_{sym} = PE_DECOMP_PTRGLOBAL(0x{sym[2:]}u, {pointee});")
    for pname, _lvl, _toks, decl in pparams:
        if f"host_{pname}" not in body:
            continue
        base = param_base_type(decl, pname)
        out.append(
            f"    {base} *host_{pname} = ({base} *)PE_Translate(pe_{pname}, 1);")
    return out


# Host compile flags, mirroring pc_port/CMakeLists.txt's pe_field_runtime
# target (`cmake` writes the same set into build/**/flags.make).  A generated
# TU is only emitted if the host compiler accepts it.
COMPILE_DEFINES = ["-DPE_PORT_FB_HEIGHT=240", "-DPE_PORT_FB_WIDTH=320",
                   "-DPE_PORT_HEADLESS=1"]
COMPILE_INCLUDES = ["pc_port", "pc_port/include", "pc_port/platform",
                    "pc_port/bootstrap", "pc_port/src"]


def find_compiler() -> str | None:
    for cand in (os.environ.get("CC"), "cc", "gcc", "clang"):
        if cand and shutil.which(cand):
            return shutil.which(cand)
    return None


def compile_check(cc: str, items: list[tuple[str, str]]) -> dict[str, str]:
    """Syntax-check each (name, rendered source); return name -> first error.

    This is the generator's last line of defence and the one that cannot be
    fooled: the eligibility rules say *why* a leaf is not host-representable,
    but only the compiler proves the emitted TU actually builds.  A leaf whose
    TU does not compile is reported and not written, so the generated tree is
    always buildable.
    """
    cmd_head = [cc, "-std=gnu11", "-fsyntax-only", *COMPILE_DEFINES]
    for inc in COMPILE_INCLUDES:
        cmd_head += ["-I", str(REPO_ROOT / inc)]
    failures: dict[str, str] = {}

    def one(item: tuple[str, str]) -> tuple[str, str]:
        name, content = item
        with tempfile.TemporaryDirectory() as tmp:
            path = Path(tmp) / f"{name}_port.c"
            path.write_text(content, encoding="utf-8")
            proc = subprocess.run(cmd_head + [str(path)],
                                  capture_output=True, text=True)
            if proc.returncode == 0:
                return name, ""
            first = ""
            for line in proc.stderr.splitlines():
                if ": error:" in line:
                    first = line.split(": error:", 1)[1].strip()
                    break
            return name, first or "compile failed"

    workers = min(16, (os.cpu_count() or 4))
    with concurrent.futures.ThreadPoolExecutor(max_workers=workers) as pool:
        for name, err in pool.map(one, items):
            if err:
                failures[name] = err
    return failures


def main() -> int:
    ap = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    ap.add_argument("--check", action="store_true",
                    help="report eligibility/coverage without writing files")
    ap.add_argument("--list", action="store_true",
                    help="list eligible leaves (one per line)")
    ap.add_argument("--json", metavar="PATH",
                    help="write the full eligible-leaf plan as JSON")
    ap.add_argument("--only", action="append", default=[],
                    help="restrict to a leaf name (repeatable)")
    ap.add_argument("--verify", action="store_true",
                    help="fail (exit 1) if a generated TU has drifted from the "
                         "matching leaf; do not write")
    ap.add_argument("--limit", type=int, default=0,
                    help="cap the number of leaves emitted (0 = all)")
    ap.add_argument("--verbose", action="store_true",
                    help="with --check, print every skipped leaf and reason")
    ap.add_argument("--no-compile-check", action="store_true",
                    help="do not syntax-check each rendered TU with the host "
                         "compiler before emitting it (the gate that keeps "
                         "pc_port/game/decomp buildable)")
    args = ap.parse_args()

    matched = parse_matched()
    # Emit to a fixed point.  Whether a callee gets its own prototype or a loud
    # boundary depends on whether that callee's TU exists in the generated
    # directory, so adding or pruning a TU changes the correct content of its
    # callers.  Re-run the whole pipeline until nothing changes (typically one
    # round in steady state; more from an empty directory).
    MAX_ROUNDS = 8
    written = 0
    conflicts: list[str] = []
    drift: list[str] = []
    orphans: list[str] = []
    uncompilable: dict[str, str] = {}
    for _round in range(MAX_ROUNDS):
        defs = pc_port_definitions()
        known = set(defs)
        shim_macros = pc_port_macros()
        protos = canonical_protos()
        sigs = canonical_signatures()
        out_dir = REPO_ROOT / OUT_DIR

        eligible: list[str] = []
        skipped: dict[str, str] = {}
        plans: dict[str, dict] = {}

        for name in sorted(matched):
            src = REPO_ROOT / SRC_DIR / f"{name}.c"
            if not src.exists():
                skipped[name] = "no src file"
                continue
            plan = analyze(name, src.read_text(encoding="utf-8", errors="replace"),
                           matched[name], known, shim_macros, protos, sigs)
            if not plan["eligible"]:
                skipped[name] = plan["reason"]
                continue
            plans[name] = plan
            eligible.append(name)

        if args.only:
            wanted = set(args.only)
            eligible = [n for n in eligible if n in wanted]
        if args.limit:
            eligible = eligible[: args.limit]

        if args.json:
            payload = {
                "eligible": {n: {"words": matched[n]["words"],
                                 "callees": plans[n]["callees"],
                                 "boundaries": plans[n]["boundaries"]}
                             for n in eligible},
                "skipped": skipped,
            }
            Path(args.json).write_text(
                json.dumps(payload, indent=2, sort_keys=True) + "\n", encoding="utf-8")

        if args.list:
            for n in eligible:
                print(n)
            return 0

        total_words = sum(matched[n]["words"] for n in eligible)
        if args.check:
            print(f"gen_decomp_ports: eligible {len(eligible)} leaves "
                  f"({total_words} words); ineligible {len(skipped)}")
            if args.verbose:
                for name in sorted(skipped):
                    print(f"  skip {name}: {skipped[name]}")
            return 0

        out_dir.mkdir(parents=True, exist_ok=True)
        changed = 0
        written = 0
        conflicts: list[str] = []
        drift: list[str] = []
        emitted: set[str] = set()
        rendered: list[tuple[str, str]] = []
        for name in eligible:
            target = out_dir / f"{name}_port.c"
            others = [f for f in defs.get(name, ())
                      if f != target.relative_to(REPO_ROOT).as_posix()]
            if others:
                conflicts.append(f"{name}: also defined in {', '.join(sorted(others))}")
                continue
            rendered.append((name, render(name, plans[name], matched, known, protos)))

        uncompilable: dict[str, str] = {}
        if not args.no_compile_check:
            cc = find_compiler()
            if cc is None:
                print("gen_decomp_ports: WARNING no host C compiler found "
                      "(set CC); emitting without the compile gate", file=sys.stderr)
            else:
                uncompilable = compile_check(cc, rendered)
                rendered = [(n, c) for n, c in rendered if n not in uncompilable]

        for name, content in rendered:
            target = out_dir / f"{name}_port.c"
            if target.exists() and target.read_text(encoding="utf-8") == content:
                written += 1
                emitted.add(target.name)
                continue
            if args.verify:
                drift.append(str(target.relative_to(REPO_ROOT)))
                emitted.add(target.name)
                continue
            target.write_text(content, encoding="utf-8")
            written += 1
            changed += 1
            emitted.add(target.name)

        # Orphan pruning.  This directory is generated output that CMake globs, so
        # a TU left behind after its leaf became ineligible (or was renamed, or now
        # collides with a hand port) is still compiled and can break the build.
        # The generator owns every `func_XXXXXXXX_port.c` here, so anything it did
        # not just emit is removed.  `--only`/`--limit` restrict the emit set, so
        # pruning is suppressed for those runs.
        orphans: list[str] = []
        if out_dir.is_dir() and not args.only and not args.limit:
            for path in sorted(out_dir.glob("*_port.c")):
                if not re.fullmatch(r"func_[0-9A-Fa-f]{8}_port\.c", path.name):
                    continue
                if path.name in emitted:
                    continue
                orphans.append(path.name)
                if not args.verify:
                    path.unlink()
                    changed += 1

        # --verify writes nothing, so a second round would be identical.
        if args.verify or changed == 0:
            break
    else:
        print(f'gen_decomp_ports: WARNING emit set still changing after '
              f'{MAX_ROUNDS} rounds', file=sys.stderr)

    if args.verify:
        print(f"gen_decomp_ports: verified {written} port TU(s) up to date; "
              f"{len(drift) + len(orphans)} drifted")
        for rel in drift:
            print(f"  stale {rel}")
        for nm in orphans:
            print(f"  orphan {(OUT_DIR / nm).as_posix()} "
                  "(no longer emitted; re-run without --verify to prune)")
        return 1 if (drift or orphans) else 0

    print(f"gen_decomp_ports: wrote {written} port TU(s) to {OUT_DIR}")
    if uncompilable:
        print(f"gen_decomp_ports: {len(uncompilable)} leaf/leaves not emitted "
              "(rendered TU does not compile)")
        for nm in sorted(uncompilable):
            print(f"  uncompilable {nm}: {uncompilable[nm]}")
    if orphans:
        print(f"gen_decomp_ports: pruned {len(orphans)} orphaned TU(s)")
    print(f"gen_decomp_ports: {len(conflicts)} conflict(s) not emitted "
          "(matched C would collide with an existing pc_port definition)")
    if args.verbose:
        for c in conflicts:
            print(f"  {c}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
