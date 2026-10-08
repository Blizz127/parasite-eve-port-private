#!/usr/bin/env python3
"""Guest-memory lowering of a matched leaf (the generator's E21 path).

The verbatim path (gen_decomp_ports.py) keeps a leaf's body as written and
lets host pointers stand in for guest pointers.  That is only sound while
every struct the leaf touches has the same layout on both sides, so a leaf
that reads a pointer *out of* guest RAM (`o->ctx->fE`), stores one into it,
or calls through a code pointer held in room data (`o->fn(o)`) is refused
(E15/E18 and friends).  Those shapes are most of the room code.

This module lowers such a leaf instead of copying it.  The matched C is
parsed (pycparser) and re-emitted with the retail ABI made explicit:

  * every pointer value is a 32-bit guest address (`uint32_t`);
  * struct/array/global layout is computed with the MIPS O32 guest rules
    (4-byte pointers, natural alignment, `packed` honoured) and every memory
    access becomes PE_LoadU8/U16/U32 / PE_StoreU8/U16/U32 at that address;
  * a call through a code pointer is the retail `jalr`: PE_GuestCall, which
    resolves to native C through the guest-code registry or STOPs loudly;
  * a direct call to a callee with a native definition goes through its host
    prototype (guest addresses are translated only where that prototype takes
    a host pointer); a callee with no native code is a loud boundary.

Nothing is invented: operand order, constants, control flow and integer types
are those of the matched leaf; only the address arithmetic the era compiler
performed implicitly is spelled out.  Anything outside the supported subset
(floats, bitfields, string literals, initialised local aggregates, a host
stack address flowing into a guest-address slot, ...) raises LowerError and
the leaf stays refused.  The rendered TU still has to pass the E17 host
compile gate.  Unit tests: test_guest_lower.py.
"""

from __future__ import annotations

import re
import subprocess

from pycparser import c_ast, c_parser


class LowerError(Exception):
    pass


# ── guest types ──────────────────────────────────────────────────────────

class T:
    pass


class TVoid(T):
    def __repr__(self):
        return "void"


class TInt(T):
    def __init__(self, size: int, signed: bool):
        self.size, self.signed = size, signed

    def __repr__(self):
        return f"{'s' if self.signed else 'u'}{self.size * 8}"


class TFloat(T):
    pass


class TPtr(T):
    def __init__(self, to: T):
        self.to = to


class THostPtr(T):
    """Address of a host-local aggregate (never a guest value)."""
    def __init__(self, to: T):
        self.to = to


class TArr(T):
    def __init__(self, of: T, n: int | None):
        self.of, self.n = of, n


class TFunc(T):
    def __init__(self, ret: T, params: list[T] | None, variadic: bool):
        self.ret, self.params, self.variadic = ret, params, variadic


class TStruct(T):
    def __init__(self, low: "Lowerer", tag: str | None, node, union: bool):
        self.low, self.tag, self.node, self.union = low, tag, node, union
        self._fields = None
        self.packed = False

    def decls(self):
        node = self.node
        if node is None or node.decls is None:
            node = self.low.struct_defs.get((self.union, self.tag))
            if node is None:
                raise LowerError(f"incomplete struct {self.tag}")
        return node

    def layout(self):
        if self._fields is not None:
            return
        node = self.decls()
        packed = self.packed or self.low.is_packed(node, self.tag)
        fields: list[tuple[str, T, int]] = []
        off = size = 0
        align = 1
        for d in node.decls:
            if getattr(d, "bitsize", None) is not None:
                raise LowerError("bitfield member")
            ft = self.low.ty(d.type)
            a = 1 if packed else alignof(ft)
            s = sizeof(ft)
            align = max(align, a)
            if self.union:
                fo = 0
                size = max(size, s)
            else:
                off = (off + a - 1) // a * a
                fo = off
                off += s
                size = off
            if d.name is None and isinstance(ft, TStruct):
                ft.layout()
                fields += [(n, t, fo + o) for n, t, o in ft._fields]
            else:
                fields.append((d.name, ft, fo))
        self._size = (size + align - 1) // align * align
        self._align = align
        self._fields = fields

    def field(self, name: str) -> tuple[T, int]:
        self.layout()
        for n, t, o in self._fields:
            if n == name:
                return t, o
        raise LowerError(f"no member {name}")


def sizeof(t: T) -> int:
    if isinstance(t, TInt):
        return t.size
    if isinstance(t, (TPtr, THostPtr)):
        return 4
    if isinstance(t, TArr):
        if t.n is None:
            raise LowerError("sizeof incomplete array")
        return t.n * sizeof(t.of)
    if isinstance(t, TStruct):
        t.layout()
        return t._size
    if isinstance(t, TFloat):
        raise LowerError("float")
    raise LowerError(f"sizeof {t!r}")


def alignof(t: T) -> int:
    if isinstance(t, TInt):
        return min(t.size, 8)
    if isinstance(t, (TPtr, THostPtr)):
        return 4
    if isinstance(t, TArr):
        return alignof(t.of)
    if isinstance(t, TStruct):
        t.layout()
        return t._align
    raise LowerError(f"alignof {t!r}")


S32, U32 = TInt(4, True), TInt(4, False)
HOST_INT = {(1, True): "int8_t", (1, False): "uint8_t", (2, True): "int16_t",
            (2, False): "uint16_t", (4, True): "int32_t", (4, False): "uint32_t",
            (8, True): "int64_t", (8, False): "uint64_t"}


def htype(t: T) -> str:
    if isinstance(t, TInt):
        return HOST_INT[(t.size, t.signed)]
    if isinstance(t, TPtr):
        return "uint32_t"
    if isinstance(t, TFloat):
        raise LowerError("float")
    raise LowerError(f"no scalar host type for {t!r}")


def is_scalar(t: T) -> bool:
    return isinstance(t, (TInt, TPtr))


def arith(a: T, b: T) -> T:
    """Usual arithmetic conversions (ILP32 guest; int is 32-bit on the host too)."""
    size = max(4, a.size, b.size)
    def rank_unsigned(t):
        return (not t.signed) and t.size >= size
    return TInt(size, not (rank_unsigned(a) or rank_unsigned(b)))


def promote(a: T) -> T:
    return a if a.size >= 4 else S32


# ── preprocessing / parsing ──────────────────────────────────────────────

ATTR_PACKED_RE = re.compile(r"__attribute__\s*\(\(\s*packed\s*\)\)")
ATTR_RE = re.compile(r"__attribute__\s*\(\((?:[^()]|\([^()]*\))*\)\)")


def strip_c_comments(text: str) -> str:
    """Drop /* */ and // comments, leaving string and char literals intact."""
    out, i, n = [], 0, len(text)
    while i < n:
        ch = text[i]
        if ch in "\"'":
            j = i + 1
            while j < n and text[j] != ch:
                j += 2 if text[j] == "\\" else 1
            out.append(text[i:j + 1])
            i = j + 1
        elif text.startswith("/*", i):
            j = text.find("*/", i + 2)
            j = n if j < 0 else j + 2
            out.append(" " + "\n" * text.count("\n", i, j))
            i = j
        elif text.startswith("//", i):
            j = text.find("\n", i)
            i = n if j < 0 else j
        else:
            out.append(ch)
            i += 1
    return "".join(out)


def preprocess(text: str) -> str:
    try:
        out = subprocess.run(["cpp", "-P", "-undef", "-nostdinc", "-"], input=text,
                             capture_output=True, text=True, timeout=60)
    except (OSError, subprocess.TimeoutExpired) as exc:
        raise LowerError(f"cpp: {exc}")
    if out.returncode != 0:
        raise LowerError("cpp failed")
    return out.stdout


# ── the lowerer ──────────────────────────────────────────────────────────

class Var:
    def __init__(self, ty: T, cname: str, buf: bool):
        self.ty, self.cname, self.buf = ty, cname, buf


class LV:
    """g: guest address expr; b: host uint8_t* expr; v: host variable."""
    def __init__(self, kind: str, code: str, ty: T):
        self.kind, self.code, self.ty = kind, code, ty


class Lowerer:
    def __init__(self, name: str, text: str, known: set[str], sigs: dict[str, dict],
                 decls: dict[str, str] | None = None, site: str | None = None):
        self.name = name
        self.known = known
        self.sigs = sigs
        self.decls = decls or {}
        self.site = site or name
        self.typedefs: dict[str, object] = {}
        self.tcache: dict[str, T] = {}
        self.struct_defs: dict[tuple[bool, str], object] = {}
        self.enums: dict[str, int] = {}
        self.globals: dict[str, T] = {}
        self.funcs: dict[str, TFunc] = {}
        self.scopes: list[dict[str, Var]] = []
        self.helpers: set[str] = set()
        self.callees: set[str] = set()
        self.tmp = 0
        self.packed_tags: set[str] = set()
        self.packed_nodes: set[int] = set()
        self.addr_taken: set[str] = set()
        self.frame = 0
        self.asm_table: list[dict] = []
        self.aliases: dict[str, str] = {}
        self._parse(text)

    # parsing ---------------------------------------------------------
    def _parse(self, text: str) -> None:
        for m in re.finditer(r"\}\s*" + ATTR_PACKED_RE.pattern + r"\s*(\w+)", text):
            self.packed_tags.add(m.group(1))
        for m in re.finditer(r"struct\s+(\w+)\s*\{[^{}]*\}\s*" + ATTR_PACKED_RE.pattern, text):
            self.packed_tags.add(m.group(1))
        text = ATTR_RE.sub("", text)
        text = re.sub(r"\b__inline\b|\binline\b", "", text)
        try:
            ast = c_parser.CParser().parse(text, filename=self.name)
        except Exception as exc:  # pycparser raises ParseError
            raise LowerError(f"parse: {str(exc)[:80]}")
        self.fdef = None
        for ext in ast.ext:
            if isinstance(ext, c_ast.Typedef):
                self.typedefs[ext.name] = ext.type
                self._collect_structs(ext.type, ext.name)
            elif isinstance(ext, c_ast.Decl):
                self._collect_structs(ext.type, None)
                if ext.name:
                    self._global_decl(ext)
            elif isinstance(ext, c_ast.FuncDef):
                if ext.decl.name == self.name:
                    self.fdef = ext
                self._collect_structs(ext.decl.type, None)
                if ext.decl.name != self.name:
                    self._global_decl(ext.decl)
        if self.fdef is None:
            raise LowerError("definition not found")
        if len([e for e in ast.ext if isinstance(e, c_ast.FuncDef)]) != 1:
            raise LowerError("more than one function definition")

    def _collect_structs(self, node, tdname) -> None:
        for n in walk(node):
            if isinstance(n, (c_ast.Struct, c_ast.Union)) and n.decls is not None:
                if n.name:
                    self.struct_defs[(isinstance(n, c_ast.Union), n.name)] = n
                if tdname and tdname in self.packed_tags:
                    self.packed_nodes.add(id(n))
            if isinstance(n, c_ast.Enum) and n.values is not None:
                v = -1
                for e in n.values.enumerators:
                    v = self.cev(e.value) if e.value is not None else v + 1
                    self.enums[e.name] = v

    def is_packed(self, node, tag) -> bool:
        return id(node) in self.packed_nodes or (tag is not None and tag in self.packed_tags)

    def _global_decl(self, d) -> None:
        if "typedef" in (d.storage or []):
            return
        t = d.type
        if isinstance(t, c_ast.FuncDecl):
            self.funcs[d.name] = self.ty(t)
        else:
            self.globals[d.name] = t  # resolved lazily

    # types -----------------------------------------------------------
    def ty(self, node) -> T:
        if isinstance(node, c_ast.Typename):
            return self.ty(node.type)
        if isinstance(node, c_ast.TypeDecl):
            return self.ty(node.type)
        if isinstance(node, c_ast.PtrDecl):
            return TPtr(self.ty(node.type))
        if isinstance(node, c_ast.ArrayDecl):
            n = self.cev(node.dim) if node.dim is not None else None
            return TArr(self.ty(node.type), n)
        if isinstance(node, c_ast.FuncDecl):
            ret = self.ty(node.type)
            params, variadic = [], False
            if node.args is None:
                return TFunc(ret, None, False)
            for p in node.args.params:
                if isinstance(p, c_ast.EllipsisParam):
                    variadic = True
                    continue
                pt = self.ty(p.type)
                if isinstance(pt, TVoid):
                    continue
                params.append(decay_param(pt))
            return TFunc(ret, params, variadic)
        if isinstance(node, (c_ast.Struct, c_ast.Union)):
            st = TStruct(self, node.name, node if node.decls is not None else None,
                         isinstance(node, c_ast.Union))
            if node.decls is not None and id(node) in self.packed_nodes:
                st.packed = True
            return st
        if isinstance(node, c_ast.Enum):
            return S32
        if isinstance(node, c_ast.IdentifierType):
            return self.base(node.names)
        raise LowerError(f"type node {type(node).__name__}")

    def base(self, names: list[str]) -> T:
        names = [n for n in names if n not in ("const", "volatile", "register")]
        if len(names) == 1 and names[0] in self.typedefs:
            nm = names[0]
            if nm not in self.tcache:
                self.tcache[nm] = self.ty(self.typedefs[nm])
            return self.tcache[nm]
        s = set(names)
        if "void" in s:
            return TVoid()
        if "float" in s or "double" in s:
            return TFloat()
        unsigned = "unsigned" in s
        if "char" in s:
            # gcc-2.7.2-psx: plain `char` is UNSIGNED (see pc_port/CMakeLists.txt)
            return TInt(1, "signed" in s)
        if "short" in s:
            return TInt(2, not unsigned)
        if names.count("long") >= 2:
            return TInt(8, not unsigned)
        if s <= {"int", "long", "signed", "unsigned"} and s:
            return TInt(4, not unsigned)
        if len(names) == 1 and names[0] in ("pe_addr_t", "uint32_t"):
            return U32
        raise LowerError(f"unknown type {' '.join(names)}")

    # constants -------------------------------------------------------
    def cev(self, e) -> int:
        if isinstance(e, c_ast.Constant):
            v, _ = parse_const(e)
            return v
        if isinstance(e, c_ast.ID) and e.name in self.enums:
            return self.enums[e.name]
        if isinstance(e, c_ast.UnaryOp):
            if e.op == "sizeof":
                if isinstance(e.expr, c_ast.Typename):
                    return sizeof(self.ty(e.expr))
                return sizeof(self.static_ty(e.expr))
            if e.op == "&":
                return self.const_offset(e.expr)
            v = self.cev(e.expr)
            return {"-": -v, "+": v, "~": ~v, "!": int(not v)}[e.op]
        if isinstance(e, c_ast.Cast):
            return self.cev(e.expr)
        if isinstance(e, c_ast.BinaryOp):
            a, b = self.cev(e.left), self.cev(e.right)
            ops = {"+": a + b, "-": a - b, "*": a * b, "&": a & b, "|": a | b,
                   "^": a ^ b, "<<": a << b, ">>": a >> b, "==": int(a == b),
                   "!=": int(a != b), "<": int(a < b), ">": int(a > b),
                   "<=": int(a <= b), ">=": int(a >= b), "&&": int(bool(a and b)),
                   "||": int(bool(a or b))}
            if e.op in ("/", "%"):
                if b == 0:
                    raise LowerError("constant division by zero")
                q = abs(a) // abs(b) * (1 if (a >= 0) == (b >= 0) else -1)
                return q if e.op == "/" else a - q * b
            return ops[e.op]
        if isinstance(e, c_ast.TernaryOp):
            return self.cev(e.iftrue) if self.cev(e.cond) else self.cev(e.iffalse)
        raise LowerError(f"non-constant {type(e).__name__}")

    def const_offset(self, e) -> int:
        """offsetof idiom: &((T *)0)->f[.g][k]."""
        if isinstance(e, c_ast.StructRef):
            if e.type == "->":
                if not isinstance(e.name, c_ast.Cast) or self.cev(e.name.expr) != 0:
                    raise LowerError("non-constant address")
                st = self.ty(e.name.to_type)
                if not isinstance(st, TPtr) or not isinstance(st.to, TStruct):
                    raise LowerError("offsetof base")
                return st.to.field(e.field.name)[1]
            base_t = self.static_ty(e.name)
            return self.const_offset(e.name) + base_t.field(e.field.name)[1]
        if isinstance(e, c_ast.ArrayRef):
            et = self.static_ty(e)
            return self.const_offset(e.name) + self.cev(e.subscript) * sizeof(et)
        raise LowerError("non-constant address")

    def static_ty(self, e) -> T:
        """Type of an expression without emitting code (sizeof, offsetof)."""
        saved = (set(self.helpers), set(self.callees), self.tmp)
        try:
            if isinstance(e, (c_ast.StructRef, c_ast.ArrayRef, c_ast.ID)) or (
                    isinstance(e, c_ast.UnaryOp) and e.op == "*"):
                try:
                    return self.lv(e).ty
                except LowerError:
                    pass
            if isinstance(e, c_ast.StructRef) and e.type == "->" and isinstance(e.name, c_ast.Cast):
                st = self.ty(e.name.to_type)
                return st.to.field(e.field.name)[0]
            if isinstance(e, c_ast.StructRef):
                return self.static_ty(e.name).field(e.field.name)[0]
            if isinstance(e, c_ast.ArrayRef):
                bt = self.static_ty(e.name)
                return bt.of if isinstance(bt, TArr) else bt.to
            return self.rv(e)[1]
        finally:
            self.helpers, self.callees, self.tmp = saved

    # scopes ----------------------------------------------------------
    def lookup(self, name: str) -> Var | None:
        for sc in reversed(self.scopes):
            if name in sc:
                return sc[name]
        return None

    def newtmp(self) -> str:
        self.tmp += 1
        return f"pe_t{self.tmp}"

    def global_ty(self, name: str) -> T:
        t = self.globals[name]
        if not isinstance(t, T):
            t = self.ty(t)
            self.globals[name] = t
        return t

    # lvalues ---------------------------------------------------------
    def lv(self, e) -> LV:
        if isinstance(e, c_ast.ID):
            v = self.lookup(e.name)
            if v is not None:
                return LV("g" if v.buf else "v", v.cname, v.ty)
            if e.name in self.globals:
                addr = data_address(self.aliases.get(e.name, e.name))
                return LV("g", f"0x{addr:08X}u", self.global_ty(e.name))
            raise LowerError(f"unknown object {e.name}")
        if isinstance(e, c_ast.UnaryOp) and e.op == "*":
            code, t = self.rv(e.expr)
            if isinstance(t, TPtr):
                return LV("g", code, t.to)
            if isinstance(t, THostPtr):
                return LV("b", code, t.to)
            raise LowerError("dereference of non-pointer")
        if isinstance(e, c_ast.ArrayRef):
            bc, bt = self.rv(e.name)
            ic, it = self.rv(e.subscript)
            if isinstance(it, (TPtr, THostPtr)) and isinstance(bt, TInt):
                bc, bt, ic, it = ic, it, bc, bt
            if not isinstance(bt, (TPtr, THostPtr)) or not isinstance(it, TInt):
                raise LowerError("subscript")
            return self.offset_lv(bc, bt, ic, it)
        if isinstance(e, c_ast.StructRef):
            if e.type == "->":
                bc, bt = self.rv(e.name)
                if not isinstance(bt, (TPtr, THostPtr)) or not isinstance(bt.to, TStruct):
                    raise LowerError("-> on non-struct pointer")
                ft, off = bt.to.field(e.field.name)
                kind = "g" if isinstance(bt, TPtr) else "b"
                return LV(kind, add_off(bc, off, kind), ft)
            base = self.lv(e.name)
            if not isinstance(base.ty, TStruct) or base.kind == "v":
                raise LowerError(". on non-struct")
            ft, off = base.ty.field(e.field.name)
            return LV(base.kind, add_off(base.code, off, base.kind), ft)
        raise LowerError(f"not an lvalue: {type(e).__name__}")

    def offset_lv(self, bc, bt, ic, it) -> LV:
        size = sizeof(bt.to)
        if isinstance(bt, TPtr):
            return LV("g", f"({bc} + (uint32_t)({ic}) * {size}u)", bt.to)
        return LV("b", f"({bc} + (int32_t)({ic}) * {size})", bt.to)

    def load(self, lv: LV) -> tuple[str, T]:
        t = lv.ty
        if isinstance(t, TArr):
            return (lv.code, (TPtr if lv.kind == "g" else THostPtr)(t.of)) if lv.kind != "v" \
                else (lv.code, THostPtr(t.of))
        if isinstance(t, TStruct):
            raise LowerError("struct rvalue")
        if isinstance(t, TFunc):
            raise LowerError("function object")
        if lv.kind == "v":
            return lv.code, t
        size = sizeof(t)
        if size not in (1, 2, 4, 8):
            raise LowerError(f"load size {size}")
        if lv.kind == "g":
            if size == 8:
                self.helpers.add("ld64")
                raw = f"pe_low_ld64({lv.code})"
            else:
                raw = f"PE_LoadU{size * 8}({lv.code})"
        else:
            self.helpers.add(f"hld{size}")
            raw = f"pe_low_hld{size}({lv.code})"
        return f"(({htype(t)}){raw})", t

    def store(self, lv: LV, val: str) -> str:
        t = lv.ty
        if lv.kind == "v":
            return f"{lv.code} = ({htype(t)})({val})"
        size = sizeof(t)
        if not is_scalar(t) or size not in (1, 2, 4, 8):
            raise LowerError("store of non-scalar")
        if lv.kind == "g":
            if size == 8:
                self.helpers.add("st64")
                return f"pe_low_st64({lv.code}, (uint64_t)({val}))"
            return f"PE_StoreU{size * 8}({lv.code}, (uint{size * 8}_t)({val}))"
        self.helpers.add(f"hst{size}")
        return f"pe_low_hst{size}({lv.code}, (uint{size * 8}_t)({val}))"

    # rvalues ---------------------------------------------------------
    def rv(self, e) -> tuple[str, T]:
        if isinstance(e, c_ast.Constant):
            v, t = parse_const(e)
            if isinstance(t, TInt) and not t.signed:
                return f"0x{v & ((1 << (8 * t.size)) - 1):X}u" + ("LL" if t.size == 8 else ""), t
            return (f"{v}" + ("LL" if t.size == 8 else "")), t
        if isinstance(e, c_ast.ID):
            if e.name in self.enums and self.lookup(e.name) is None:
                return str(self.enums[e.name]), S32
            if self.lookup(e.name) is None and e.name not in self.globals:
                if e.name in self.funcs or FUNC_NAME_RE.fullmatch(e.name):
                    return f"0x{code_address(e.name):08X}u", TPtr(self.funcs.get(e.name) or TFunc(S32, None, False))
            return self.load(self.lv(e))
        if isinstance(e, (c_ast.ArrayRef, c_ast.StructRef)):
            return self.load(self.lv(e))
        if isinstance(e, c_ast.Cast):
            to = self.ty(e.to_type)
            c, t = self.rv(e.expr)
            if isinstance(to, TVoid):
                return f"((void)({c}))", to
            if isinstance(t, THostPtr):
                if isinstance(to, TPtr):
                    return c, THostPtr(to.to)
                raise LowerError("host pointer narrowed to integer")
            if not is_scalar(to):
                raise LowerError("cast to non-scalar")
            if isinstance(t, TVoid):
                raise LowerError("void value")
            return f"(({htype(to)})({c}))", to
        if isinstance(e, c_ast.UnaryOp):
            return self.unary(e)
        if isinstance(e, c_ast.BinaryOp):
            return self.binary(e)
        if isinstance(e, c_ast.Assignment):
            return self.assign(e, True)
        if isinstance(e, c_ast.TernaryOp):
            cc, _ = self.cond(e.cond)
            a, at = self.rv(e.iftrue)
            b, bt = self.rv(e.iffalse)
            if isinstance(at, THostPtr) or isinstance(bt, THostPtr):
                raise LowerError("host pointer in ?:")
            if isinstance(at, TPtr) or isinstance(bt, TPtr):
                rt = at if isinstance(at, TPtr) else bt
            elif isinstance(at, TVoid) or isinstance(bt, TVoid):
                return f"({cc} ? (void)({a}) : (void)({b}))", TVoid()
            else:
                rt = arith(at, bt)
            return f"({cc} ? ({htype(rt)})({a}) : ({htype(rt)})({b}))", rt
        if isinstance(e, c_ast.FuncCall):
            return self.call(e)
        if isinstance(e, c_ast.ExprList):
            parts = [self.rv(x) for x in e.exprs]
            return "(" + ", ".join(p[0] for p in parts) + ")", parts[-1][1]
        raise LowerError(f"expression {type(e).__name__}")

    def cond(self, e) -> tuple[str, T]:
        c, t = self.rv(e)
        if isinstance(t, THostPtr):
            raise LowerError("host pointer as condition")
        return f"({c})", t

    def unary(self, e) -> tuple[str, T]:
        op = e.op
        if op == "sizeof":
            if isinstance(e.expr, c_ast.Typename):
                return f"{sizeof(self.ty(e.expr))}u", U32
            return f"{sizeof(self.static_ty(e.expr))}u", U32
        if op == "&":
            x = e.expr
            if isinstance(x, c_ast.ID) and self.lookup(x.name) is None and x.name not in self.globals \
                    and (x.name in self.funcs or FUNC_NAME_RE.fullmatch(x.name)):
                return f"0x{code_address(x.name):08X}u", TPtr(self.funcs.get(x.name) or TFunc(S32, None, False))
            lv = self.lv(x)
            if lv.kind == "g":
                return lv.code, TPtr(lv.ty)
            if lv.kind == "b":
                return f"({lv.code})", THostPtr(lv.ty)
            raise LowerError("address of register local")
        if op == "*":
            return self.load(self.lv(e))
        if op in ("++", "--", "p++", "p--"):
            return self.incdec(e, True)
        c, t = self.rv(e.expr)
        if op == "!":
            if isinstance(t, THostPtr):
                raise LowerError("host pointer as condition")
            return f"(!({c}))", S32
        if not isinstance(t, TInt):
            raise LowerError(f"unary {op} on non-integer")
        rt = promote(t)
        if op == "-" and rt.signed:
            ut = TInt(rt.size, False)
            return f"(({htype(rt)})(({htype(ut)})0 - ({htype(ut)})({c})))", rt
        return f"({op}({htype(rt)})({c}))", rt

    def binary(self, e) -> tuple[str, T]:
        op = e.op
        if op in ("&&", "||"):
            a, _ = self.cond(e.left)
            b, _ = self.cond(e.right)
            return f"({a} {op} {b})", S32
        a, at = self.rv(e.left)
        b, bt = self.rv(e.right)
        return self.arith_op(op, a, at, b, bt)

    def arith_op(self, op, a, at, b, bt) -> tuple[str, T]:
        if isinstance(at, THostPtr) or isinstance(bt, THostPtr):
            raise LowerError("host pointer arithmetic")
        ap, bp = isinstance(at, TPtr), isinstance(bt, TPtr)
        if isinstance(at, (TVoid, TFloat)) or isinstance(bt, (TVoid, TFloat)):
            raise LowerError("void/float operand")
        if op in ("==", "!=", "<", ">", "<=", ">="):
            if ap or bp:      # guest pointers compare unsigned (sltu)
                return f"((uint32_t)({a}) {op} (uint32_t)({b}))", S32
            rt = arith(at, bt)
            return f"(({htype(rt)})({a}) {op} ({htype(rt)})({b}))", S32
        if op in ("+", "-") and (ap or bp):
            if ap and bp:
                if op != "-":
                    raise LowerError("pointer + pointer")
                sz = sizeof(at.to) if not isinstance(at.to, TVoid) else 1
                return f"((int32_t)((uint32_t)({a}) - (uint32_t)({b})) / {sz})", S32
            if bp:
                if op == "-":
                    raise LowerError("int - pointer")
                a, at, b, bt = b, bt, a, at
            sz = 1 if isinstance(at.to, (TVoid, TFunc)) else sizeof(at.to)
            return f"((uint32_t)({a}) {op} (uint32_t)({b}) * {sz}u)", at
        if ap or bp:
            # integer arithmetic on a guest address (rare: masks etc.)
            at = U32 if ap else at
            bt = U32 if bp else bt
        if op in ("<<", ">>"):
            rt = promote(at)
            if op == "<<" and rt.signed:      # modular, like the guest (no host UB)
                ut = TInt(rt.size, False)
                return f"(({htype(rt)})(({htype(ut)})({a}) << ({b})))", rt
            return f"(({htype(rt)})({a}) {op} ({b}))", rt
        rt = arith(at, bt)
        if op in ("+", "-", "*") and rt.signed:   # wrap like the guest ALU
            ut = TInt(rt.size, False)
            return f"(({htype(rt)})(({htype(ut)})({a}) {op} ({htype(ut)})({b})))", rt
        if op in ("/", "%") and rt.size == 4 and not safe_divisor(b):
            # R3000 div/divu never fault the host: a zero divisor is retail's
            # break-7 divide check (a loud guest trap here), INT_MIN / -1
            # wraps like the hardware.  Constant non-zero divisors stay plain.
            kind = ("div" if op == "/" else "mod") + ("s" if rt.signed else "u")
            self.helpers.add(kind)
            return f"pe_low_{kind}(({htype(rt)})({a}), ({htype(rt)})({b}), __func__)", rt
        return f"(({htype(rt)})({a}) {op} ({htype(rt)})({b}))", rt

    def assign(self, e, want: bool) -> tuple[str, T]:
        lv = self.lv(e.lvalue)
        if isinstance(lv.ty, TStruct):
            if e.op != "=" or want:
                raise LowerError("struct assignment as value")
            src = self.lv(e.rvalue)
            if not isinstance(src.ty, TStruct) or sizeof(src.ty) != sizeof(lv.ty):
                raise LowerError("struct copy type")
            if lv.kind == "v" or src.kind == "v":
                raise LowerError("struct copy kind")
            self.helpers.add(f"copy_{lv.kind}{src.kind}")
            return f"pe_low_copy_{lv.kind}{src.kind}({lv.code}, {src.code}, {sizeof(lv.ty)}u)", TVoid()
        if not is_scalar(lv.ty):
            raise LowerError("assignment to non-scalar")
        if e.op == "=":
            v, vt = self.rv(e.rvalue)
            if isinstance(vt, THostPtr):
                raise LowerError("host pointer stored")
            if isinstance(vt, (TVoid, TFloat)):
                raise LowerError("void value assigned")
            if lv.kind == "v" or not want:
                s = self.store(lv, v)
                return (f"({s})" if want else s), lv.ty
            tv = self.newtmp()
            tv_lv = LV(lv.kind, lv.code, lv.ty)
            return (f"({{ {htype(lv.ty)} {tv} = ({htype(lv.ty)})({v}); "
                    f"{self.store(tv_lv, tv)}; {tv}; }})"), lv.ty
        op = e.op[:-1]
        r, rt = self.rv(e.rvalue)
        return self.rmw(lv, lambda cur: self.arith_op(op, cur, lv.ty, r, rt)[0], want, post=False)

    def rmw(self, lv: LV, fn, want: bool, post: bool) -> tuple[str, T]:
        ht = htype(lv.ty)
        if lv.kind == "v":
            if post:
                tv = self.newtmp()
                return f"({{ {ht} {tv} = {lv.code}; {lv.code} = ({ht})({fn(lv.code)}); {tv}; }})", lv.ty
            return f"({lv.code} = ({ht})({fn(lv.code)}))", lv.ty
        ta = self.newtmp()
        atype = "uint32_t" if lv.kind == "g" else "uint8_t *"
        alv = LV(lv.kind, ta, lv.ty)
        cur, _ = self.load(alv)
        tv = self.newtmp()
        if post:
            body = (f"{atype} {ta} = {lv.code}; {ht} {tv} = {cur}; "
                    f"{self.store(alv, fn(tv))};")
        else:
            body = (f"{atype} {ta} = {lv.code}; {ht} {tv} = ({ht})({fn(cur)}); "
                    f"{self.store(alv, tv)};")
        return (f"({{ {body} {tv}; }})" if want else f"({{ {body} }})"), lv.ty

    def incdec(self, e, want: bool) -> tuple[str, T]:
        lv = self.lv(e.expr)
        if not is_scalar(lv.ty):
            raise LowerError("++ on non-scalar")
        sign = "+" if "++" in e.op else "-"
        post = e.op.startswith("p")
        return self.rmw(lv, lambda cur: self.arith_op(sign, cur, lv.ty, "1", S32)[0], want, post)

    # calls -----------------------------------------------------------
    def call(self, e) -> tuple[str, T]:
        discard = getattr(self, "_discard_call", False)
        self._discard_call = False      # nested argument calls are value uses
        args = list(e.args.exprs) if e.args is not None else []
        fn = e.name
        if isinstance(fn, c_ast.UnaryOp) and fn.op == "*":
            fn = fn.expr
        name = fn.name if isinstance(fn, c_ast.ID) else None
        if name and self.lookup(name) is None and name not in self.globals:
            ft = self.funcs.get(name)
            if name == self.name:
                return self.direct_call(name, args, ft or self.self_ty, self.self_sig)
            if FUNC_NAME_RE.fullmatch(name):
                if name in self.known:
                    return self.direct_call(name, args, ft, self.sigs.get(name), discard)
                return self.boundary_call(name, args, ft)
            if DATA_NAME_RE.fullmatch(name) and ft is not None:
                return self.guest_call(f"0x{data_address(name):08X}u", args, ft)
            raise LowerError(f"call to unknown {name}")
        c, t = self.rv(fn)
        if not isinstance(t, TPtr):
            raise LowerError("call through non-pointer")
        ft = t.to if isinstance(t.to, TFunc) else None
        return self.guest_call(c, args, ft)

    def scalar_args(self, args) -> list[str]:
        out = []
        for a in args:
            c, t = self.rv(a)
            if isinstance(t, THostPtr):
                raise LowerError("host pointer passed to guest code")
            if not is_scalar(t):
                raise LowerError("non-scalar argument")
            out.append(c)
        return out

    def ret_of(self, ft) -> T:
        return ft.ret if isinstance(ft, TFunc) else S32

    GUEST_CALL_MAX_ARGS = 8   # PE_GUESTCALL_MAX_ARGS (pe_guestcode.h)

    def guest_call(self, fcode, args, ft) -> tuple[str, T]:
        if len(args) > self.GUEST_CALL_MAX_ARGS:
            raise LowerError(f"indirect call with more than {self.GUEST_CALL_MAX_ARGS} arguments")
        rt = self.ret_of(ft)
        if len(args) > 4:
            # O32 stack arguments (sp+16...): PE_GuestCallN carries them to a
            # 5..8-parameter thunk; a callee taking more than the site passes
            # is a loud boundary there.
            a = self.scalar_args(args)
            call = (f"PE_GuestCallN(\"@{self.site}:jalr\", (pe_addr_t)({fcode}), {len(args)}u, "
                    "(const uintptr_t[]){"
                    + ", ".join(f"(uintptr_t)(uint32_t)({x})" for x in a) + "})")
            return self.typed_ret(call, rt)
        a = self.scalar_args(args) + ["0"] * (4 - len(args))
        call = (f"PE_GuestCall(\"@{self.site}:jalr\", (pe_addr_t)({fcode}), {len(args)}u, "
                + ", ".join(f"(uintptr_t)(uint32_t)({x})" for x in a) + ")")
        return self.typed_ret(call, rt)

    def typed_ret(self, call, rt) -> tuple[str, T]:
        if isinstance(rt, TVoid):
            return f"((void){call})", rt
        if isinstance(rt, (TStruct, TFloat, TArr)):
            raise LowerError("aggregate/float return")
        return f"(({htype(rt)}){call})", rt

    def boundary_call(self, name, args, ft) -> tuple[str, T]:
        a = self.scalar_args(args)
        n = min(len(a), 4)
        if n == 0:
            call = f'PE_D_COMP_BOUNDARY0("{name}", 0x{name[5:]}u)'
        else:
            call = f'PE_D_COMP_BOUNDARY{n}("{name}", 0x{name[5:]}u, {", ".join(a)})'
        return self.typed_ret(call, self.ret_of(ft))

    def unread_args_droppable(self, args, ft, sig, n) -> bool:
        """E21c: retail passes more arguments than the callee takes.  Under
        O32 the callee never reads the extra argument registers or the
        caller's stack slots past its own parameters.  So dropping them is
        exact when (a) the leaf declares the callee without a prototype
        (`extern void f();`, K&R: the call site fixes no count), (b) the
        callee's matched retail definition takes exactly the host
        prototype's n parameters, and (c) every dropped argument is free of
        side effects (a constant or a named local/parameter).  Fewer
        arguments than parameters is never droppable: the callee would read
        slots the caller never wrote."""
        if len(args) <= n or not (isinstance(ft, TFunc) and ft.params is None):
            return False
        if sig.get("retail_nparams") != n:
            return False
        for a in args[n:]:
            if isinstance(a, c_ast.Constant):
                continue
            if isinstance(a, c_ast.ID) and self.lookup(a.name) is not None:
                continue
            return False
        return True

    def direct_call(self, name, args, ft, sig, discard=False) -> tuple[str, T]:
        self.callees.add(name)
        rt = self.ret_of(ft)
        if sig is None:
            if name != self.name:
                raise LowerError(f"no host prototype for {name}")
        params = [p for p in sig["params"] if p.strip() not in ("", "void")]
        if any(p.strip() == "..." for p in params):
            raise LowerError(f"variadic callee {name}")
        if len(params) != len(args):
            if not self.unread_args_droppable(args, ft, sig, len(params)):
                raise LowerError(f"call arity {name} {len(args)}!={len(params)}")
            args = args[:len(params)]
        out = []
        for a, p in zip(args, params):
            c, t = self.rv(a)
            if not (is_scalar(t) or isinstance(t, THostPtr)):
                raise LowerError("non-scalar argument")
            hp = host_pointer_param(p)
            if hp:
                if isinstance(t, THostPtr):
                    out.append(f"(void *)({c})")
                elif isinstance(a, c_ast.Constant) and parse_const(a)[0] == 0:
                    out.append("NULL")
                elif isinstance(t, TPtr):
                    self.helpers.add("host")
                    out.append(f"pe_low_host({c})")
                else:
                    raise LowerError(f"integer passed to host pointer parameter of {name}")
            else:
                if isinstance(t, THostPtr):
                    raise LowerError(f"host pointer passed to {name} parameter {p.strip()}")
                out.append(c)
        call = f"{name}({', '.join(out)})"
        hret = sig["ret"].replace("extern", "").replace("static", "").strip()
        if "*" in hret:
            call = f"PE_HostToGuest({call})"
        elif hret == "void" and not isinstance(rt, TVoid):
            if not discard:
                raise LowerError(f"value of void callee {name} used")
            return f"((void){call})", TVoid()
        return self.typed_ret(call, rt)

    # statements ------------------------------------------------------
    def stmt(self, s, ind: str, out: list[str]) -> None:
        I = ind
        if s is None:
            return
        if isinstance(s, c_ast.Compound):
            out.append(I + "{")
            self.block(s, I + "    ", out)
            out.append(I + "}")
        elif isinstance(s, c_ast.Decl):
            self.local_decl(s, I, out)
        elif isinstance(s, c_ast.DeclList):
            for d in s.decls:
                self.local_decl(d, I, out)
        elif (isinstance(s, c_ast.FuncCall) and isinstance(s.name, c_ast.ID)
              and s.name.name == "__pe_asm"):
            self.asm_stmt(s, I, out)
        elif isinstance(s, c_ast.If):
            c, _ = self.cond(s.cond)
            out.append(f"{I}if ({c}) {{")
            self.sub(s.iftrue, I, out)
            if s.iffalse is not None:
                out.append(f"{I}}} else {{")
                self.sub(s.iffalse, I, out)
            out.append(I + "}")
        elif isinstance(s, c_ast.While):
            c, _ = self.cond(s.cond)
            out.append(f"{I}while ({c}) {{")
            self.sub(s.stmt, I, out)
            out.append(I + "}")
        elif isinstance(s, c_ast.DoWhile):
            out.append(f"{I}do {{")
            self.sub(s.stmt, I, out)
            c, _ = self.cond(s.cond)
            out.append(f"{I}}} while ({c});")
        elif isinstance(s, c_ast.For):
            self.scopes.append({})
            out.append(I + "{")
            J = I + "    "
            if isinstance(s.init, c_ast.DeclList):
                for d in s.init.decls:
                    self.local_decl(d, J, out)
                init = ""
            else:
                init = self.ex(s.init) if s.init is not None else ""
            c = self.cond(s.cond)[0] if s.cond is not None else ""
            nx = self.ex(s.next) if s.next is not None else ""
            out.append(f"{J}for ({init}; {c}; {nx}) {{")
            self.sub(s.stmt, J, out)
            out.append(J + "}")
            out.append(I + "}")
            self.scopes.pop()
        elif isinstance(s, c_ast.Switch):
            c, t = self.rv(s.cond)
            if not isinstance(t, TInt):
                raise LowerError("switch on non-integer")
            out.append(f"{I}switch (({htype(promote(t))})({c})) {{")
            self.sub(s.stmt, I, out)
            out.append(I + "}")
        elif isinstance(s, c_ast.Case):
            out.append(f"{I}case {self.cev(s.expr)}:")
            for x in s.stmts or []:
                self.stmt(x, I + "    ", out)
        elif isinstance(s, c_ast.Default):
            out.append(f"{I}default:")
            for x in s.stmts or []:
                self.stmt(x, I + "    ", out)
        elif isinstance(s, c_ast.Label):
            out.append(f"{I}{s.name}:;")
            self.stmt(s.stmt, I, out)
        elif isinstance(s, c_ast.Goto):
            out.append(f"{I}goto {s.name};")
        elif isinstance(s, c_ast.Break):
            out.append(f"{I}break;")
        elif isinstance(s, c_ast.Continue):
            out.append(f"{I}continue;")
        elif isinstance(s, c_ast.Return):
            if s.expr is None:
                out.append(f"{I}return;")
            else:
                c, t = self.rv(s.expr)
                if isinstance(self.ret, TVoid):
                    out.append(f"{I}(void)({c}); return;")
                else:
                    if isinstance(t, THostPtr):
                        raise LowerError("host pointer returned")
                    out.append(f"{I}return ({htype(self.ret)})({c});")
        elif isinstance(s, c_ast.EmptyStatement):
            out.append(f"{I};")
        elif isinstance(s, c_ast.Typedef):
            raise LowerError("local typedef")
        elif isinstance(s, (c_ast.Pragma, c_ast.FuncDef)):
            raise LowerError("pragma/nested function")
        else:
            out.append(f"{I}{self.ex(s)};")

    def sub(self, s, I, out):
        if isinstance(s, c_ast.Compound):
            self.block(s, I + "    ", out)
        else:
            self.scopes.append({})
            self.stmt(s, I + "    ", out)
            self.scopes.pop()

    def asm_stmt(self, s, I, out):
        args = list(s.args.exprs)
        spec = self.asm_table[int(args[0].value)]
        outs, ins, opers = spec["outs"], spec["ins"], args[1:]
        nout = len(outs)
        self.tmp += 1
        tag = f"pe_asm{self.tmp}"
        reg = {k: k for k in range(nout)}
        J = I + "    "
        body = [I + "{   /* inline GTE asm: retail cop2 sequence on the native GTE */"]
        decl = [f"{tag}_{k}" for k in range(nout)]
        for j, (c, _) in enumerate(ins):
            k = nout + j
            reg[k] = int(c) if c.isdigit() else k
            if not c.isdigit():
                decl.append(f"{tag}_{k}")
        gprs = sorted({o[-1][1] for o in spec["ops"] if o[0] == "load" and o[-1][0] == "gpr"}
                      | {o[2][1] for o in spec["ops"] if o[0] == "alu" and o[2][0] == "gpr"}
                      | {o[1][1] for o in spec["ops"] if o[0] in ("cfc2", "mfc2") and o[1][0] == "gpr"})
        decl += [f"{tag}_r{n}" for n in gprs]
        if decl:
            body.append(J + "uint32_t " + ", ".join(f"{d} = 0u" for d in decl) + ";")
        for j, (c, _) in enumerate(ins):
            k = nout + j
            cv, t = self.rv(opers[k])
            if isinstance(t, THostPtr) or not is_scalar(t):
                raise LowerError("asm: non-scalar operand")
            body.append(f"{J}{tag}_{reg[k]} = (uint32_t)({cv});")

        def r(x):
            return f"{tag}_{reg[x[1]]}" if x[0] == "op" else f"{tag}_r{x[1]}"
        for o in spec["ops"]:
            if o[0] == "load":
                _, mn, off, base, d = o
                fn = {"lw": "(uint32_t)PE_LoadU32", "lhu": "(uint32_t)PE_LoadU16",
                      "lh": "(uint32_t)(int32_t)(int16_t)PE_LoadU16", "lbu": "(uint32_t)PE_LoadU8",
                      "lb": "(uint32_t)(int32_t)(int8_t)PE_LoadU8"}[mn]
                body.append(f"{J}{r(d)} = {fn}({r(base)} + {off & 0xFFFFFFFF}u);")
            elif o[0] == "alu":
                _, mn, d, a, b = o
                if mn in ("addiu",):
                    bv = f"{(b[1] if b[1] < 0x8000 or b[1] > 0xFFFF else b[1] - 0x10000) & 0xFFFFFFFF}u"
                elif b[0] == "imm":
                    bv = f"{b[1] & (0x1F if mn in ('sll', 'srl', 'sra') else 0xFFFF)}u"
                else:
                    bv = r(b)
                if mn == "sra":
                    body.append(f"{J}{r(d)} = (uint32_t)((int32_t){r(a)} >> {bv});")
                else:
                    body.append(f"{J}{r(d)} = {r(a)} {ALU_OPS[mn]} {bv};")
            elif o[0] == "store":
                _, mn, off, base, v = o
                fn = {"sw": "PE_StoreU32", "sh": "PE_StoreU16", "sb": "PE_StoreU8"}[mn]
                cast = {"sw": "uint32_t", "sh": "uint16_t", "sb": "uint8_t"}[mn]
                body.append(f"{J}{fn}({r(base)} + {off & 0xFFFFFFFF}u, ({cast}){r(v)});")
            elif o[0] == "ctc2":
                body.append(f"{J}PE_GTE_WriteCtrl({o[2]}u, {r(o[1])});")
            elif o[0] == "mtc2":
                body.append(f"{J}PE_GTE_WriteData({o[2]}u, {r(o[1])});")
            elif o[0] == "cfc2":
                body.append(f"{J}{r(o[1])} = PE_GTE_ReadCtrl({o[2]}u);")
            elif o[0] == "mfc2":
                body.append(f"{J}{r(o[1])} = PE_GTE_ReadData({o[2]}u);")
            elif o[0] == "lwc2":
                body.append(f"{J}PE_GTE_WriteData({o[1]}u, PE_LoadU32({r(o[3])} + {o[2] & 0xFFFFFFFF}u));")
            elif o[0] == "swc2":
                body.append(f"{J}PE_StoreU32({r(o[3])} + {o[2] & 0xFFFFFFFF}u, PE_GTE_ReadData({o[1]}u));")
            elif o[0] == "cmd":
                body.append(f"{J}PE_GTE_Command(0x{o[1]:07X}u);")
        for k in range(nout):
            lv = self.lv(opers[k])
            if not is_scalar(lv.ty):
                raise LowerError("asm: non-scalar output")
            body.append(J + self.store(lv, f"{tag}_{k}") + ";")
        body.append(I + "}")
        out.extend(body)

    def block(self, s, I, out):
        self.scopes.append({})
        for x in s.block_items or []:
            self.stmt(x, I, out)
        self.scopes.pop()

    def ex(self, e) -> str:
        """Expression evaluated for side effects only."""
        if isinstance(e, c_ast.Assignment):
            return self.assign(e, False)[0]
        if isinstance(e, c_ast.UnaryOp) and e.op in ("++", "--", "p++", "p--"):
            return self.incdec(e, False)[0]
        if isinstance(e, c_ast.ExprList):
            return ", ".join(f"({self.ex(x)})" for x in e.exprs)
        if isinstance(e, c_ast.FuncCall):
            # A call statement discards its value, so a guest prototype that
            # says int for a host void function is harmless here.
            self._discard_call = True
        c, t = self.rv(e)
        return f"(void)({c})"

    def local_decl(self, d, I, out):
        st = d.storage or []
        if "extern" in st:
            if isinstance(d.type, c_ast.FuncDecl):
                self.funcs[d.name] = self.ty(d.type)
            else:
                self.globals[d.name] = d.type
            return
        if "static" in st:
            raise LowerError("static local")
        if "typedef" in st:
            raise LowerError("local typedef")
        if isinstance(d.type, c_ast.FuncDecl):
            self.funcs[d.name] = self.ty(d.type)
            return
        t = self.ty(d.type)
        if isinstance(t, TArr) and t.n is None:
            raise LowerError("incomplete local array")
        cname = f"v_{d.name}"
        if self.lookup(d.name) is not None:
            self.tmp += 1
            cname = f"v_{d.name}_{self.tmp}"
        if not is_scalar(t) or d.name in self.addr_taken:
            # Retail keeps this object in its stack frame; it gets a slot in
            # the native guest frame so its address is a real guest address.
            a = alignof(t)
            off = (self.frame + a - 1) // a * a
            self.frame = off + sizeof(t)
            addr = f"(pe_fp + {off}u)" if off else "pe_fp"
            self.scopes[-1][d.name] = Var(t, addr, True)
            out.append(f"{I}/* {d.name}: guest frame +0x{off:X} ({sizeof(t)} bytes) */")
            if d.init is not None:
                self.init_obj(addr, t, d.init, I, out, top=True)
            return
        init = ""
        if d.init is not None:
            if isinstance(d.init, c_ast.InitList):
                raise LowerError("init list")
            c, vt = self.rv(d.init)
            if isinstance(vt, THostPtr):
                raise LowerError("host pointer in a guest-pointer local")
            init = f" = ({htype(t)})({c})"
        self.scopes[-1][d.name] = Var(t, cname, False)
        out.append(f"{I}{htype(t)} {cname}{init};")

    def init_obj(self, addr: str, t: T, init, I: str, out: list[str], top=False) -> None:
        if isinstance(init, c_ast.InitList):
            if top:
                out.append(f"{I}PE_Fill({addr}, {sizeof(t)}u, 0u);")
            items = init.exprs
            if isinstance(t, TArr):
                if t.n is not None and len(items) > t.n:
                    raise LowerError("too many initialisers")
                parts = [(i * sizeof(t.of), t.of) for i in range(len(items))]
            elif isinstance(t, TStruct):
                t.layout()
                if t.union:
                    parts = [(0, t._fields[0][1])][:len(items)]
                else:
                    parts = [(o, ft) for _, ft, o in t._fields][:len(items)]
                if len(items) > len(parts):
                    raise LowerError("too many initialisers")
            elif len(items) == 1 and is_scalar(t):
                return self.init_obj(addr, t, items[0], I, out)
            else:
                raise LowerError("init list for scalar")
            for (off, ft), x in zip(parts, items):
                if isinstance(x, c_ast.NamedInitializer):
                    raise LowerError("designated initialiser")
                self.init_obj(add_off(addr, off, "g"), ft, x, I, out)
            return
        if isinstance(t, TStruct):
            src = self.lv(init)
            if not isinstance(src.ty, TStruct) or sizeof(src.ty) != sizeof(t) or src.kind != "g":
                raise LowerError("aggregate initialised from expression")
            self.helpers.add("copy_gg")
            out.append(f"{I}pe_low_copy_gg({addr}, {src.code}, {sizeof(t)}u);")
            return
        if not is_scalar(t):
            raise LowerError("aggregate initialised from expression")
        c, vt = self.rv(init)
        if isinstance(vt, THostPtr):
            raise LowerError("host pointer stored")
        out.append(f"{I}{self.store(LV('g', addr, t), c)};")

    # driver ----------------------------------------------------------
    def lower(self) -> tuple[str, str, str]:
        fd = self.fdef
        ft = self.ty(fd.decl.type)
        if ft.variadic:
            raise LowerError("variadic definition")
        if fd.param_decls:
            raise LowerError("K&R definition")
        self.ret = ft.ret
        if isinstance(self.ret, (TStruct, TFloat, TArr)):
            raise LowerError("aggregate/float return")
        for n in walk(fd.body):
            if isinstance(n, c_ast.UnaryOp) and n.op == "&" and isinstance(n.expr, c_ast.ID):
                self.addr_taken.add(n.expr.name)
        params = []
        scope: dict[str, Var] = {}
        pnodes = fd.decl.type.args.params if fd.decl.type.args else []
        prelude = []
        for i, p in enumerate(pnodes):
            if isinstance(p, c_ast.EllipsisParam):
                raise LowerError("variadic")
            pt = decay_param(self.ty(p.type))
            if isinstance(pt, TVoid):
                continue
            if not is_scalar(pt):
                raise LowerError("aggregate parameter")
            pname = p.name or f"unnamed{i}"
            cname = f"a_{pname}"
            params.append(f"{htype(pt)} {cname}")
            if pname in self.addr_taken:
                raise LowerError("address of parameter")
            scope[pname] = Var(pt, cname, False)
        rett = "void" if isinstance(self.ret, TVoid) else htype(self.ret)
        self.self_ty = ft
        self.self_sig = {"ret": rett, "params": params or ["void"]}
        self.scopes = [scope]
        body: list[str] = []
        self.block(fd.body, "    ", body)
        plist = ", ".join(params) or "void"
        sig = f"{rett} {self.name}({plist})"
        if not self.frame:
            return sig, "\n".join(prelude + body), rett
        # Frame-owning leaf: the body runs inside a guest frame it can address.
        n = (self.frame + 7) // 8 * 8
        inner = f"pe_lowered_{self.name}"
        args = ", ".join(["pe_fp"] + [p.split()[-1] for p in params])
        wrapper = [f"static {rett} {inner}(pe_addr_t pe_fp{', ' + plist if params else ''})",
                   "{", *body, "}", "", sig, "{",
                   f"    const pe_addr_t pe_fp = PE_GuestFrame_Enter({n}u);"]
        if rett == "void":
            wrapper += [f"    {inner}({args});"]
        else:
            wrapper += [f"    {rett} pe_r = {inner}({args});"]
        wrapper += [f"    PE_GuestFrame_Leave(pe_fp, {n}u);"]
        if rett != "void":
            wrapper += ["    return pe_r;"]
        return None, "\n".join(wrapper), rett


def walk(node):
    if node is None:
        return
    stack = [node]
    while stack:
        n = stack.pop()
        yield n
        for _, ch in n.children():
            stack.append(ch)


def add_off(code: str, off: int, kind: str) -> str:
    if off == 0:
        return code
    return f"({code} + {off}u)" if kind == "g" else f"({code} + {off})"


def decay_param(t: T) -> T:
    if isinstance(t, TArr):
        return TPtr(t.of)
    if isinstance(t, TFunc):
        return TPtr(t)
    return t


FUNC_NAME_RE = re.compile(r"func_[0-9A-Fa-f]{8}")
DATA_NAME_RE = re.compile(r"D_[0-9A-Fa-f]{7,8}")


def data_address(name: str) -> int:
    if not DATA_NAME_RE.fullmatch(name):
        raise LowerError(f"global {name} has no retail address name")
    return int(name[2:], 16)


def code_address(name: str) -> int:
    if FUNC_NAME_RE.fullmatch(name):
        return int(name[5:], 16)
    if DATA_NAME_RE.fullmatch(name):
        return int(name[2:], 16)
    raise LowerError(f"code symbol {name} has no retail address name")


def parse_const(e) -> tuple[int, T]:
    if e.type == "string":
        raise LowerError("string literal")
    s = e.value
    if e.type == "char":
        body = s[1:-1]
        esc = {"\\n": 10, "\\t": 9, "\\0": 0, "\\\\": 92, "\\'": 39, "\\r": 13}
        if body in esc:
            return esc[body], S32
        if len(body) == 1:
            return ord(body), S32
        if body.startswith("\\x"):
            return int(body[2:], 16), S32
        raise LowerError("char constant")
    if e.type in ("float", "double"):
        raise LowerError("float")
    m = re.fullmatch(r"(0[xX][0-9A-Fa-f]+|[0-9]+)([uUlL]*)", s)
    if not m:
        raise LowerError(f"constant {s}")
    digits, suf = m.group(1), m.group(2).lower()
    if digits.lower().startswith("0x"):
        v = int(digits, 16)
    elif len(digits) > 1 and digits.startswith("0"):
        v = int(digits, 8)
    else:
        v = int(digits)
    unsigned = "u" in suf
    if suf.count("l") >= 2 or v > 0xFFFFFFFF:
        return v, TInt(8, not unsigned)
    if v > 0x7FFFFFFF:
        unsigned = True          # ILP32 (C89 decimal -> unsigned long, 32-bit)
    return v, TInt(4, not unsigned)


# ── inline GTE asm (E21b) ───────────────────────────────────────────────
# The vendor room C keeps PSY-Q's inline COP2 macros (gte_ldv0, gte_SetRotMatrix,
# gte_rtv0, gte_stlvnl, ...).  Their templates use a small, fixed instruction
# set; each asm statement becomes the same register-level sequence against the
# native GTE model (pe_gte.c: PE_GTE_ReadData/WriteData/ReadCtrl/WriteCtrl/
# Command).  Anything outside this set keeps the "asm" refusal.

ASM_HEAD_RE = re.compile(r"\b(?:__asm__|asm)\s*(?:__volatile__|volatile)?\s*\(")
# COP2 commands with a native implementation: MVMVA (mx=RT, v in {V0, IR},
# cv in {TR, none}), NCLIP, RTPS and RTPT (sf=1, lm=0: PE_GTE_RTPS/RTPT_coordinates).
# Readable data registers include the SZ0-SZ3 FIFO ($16-$19, gte_stszotz).
GTE_DATA_W = set(range(0, 15)) | {24, 25, 26, 27, 30}
GTE_DATA_R = set(range(0, 15)) | {16, 17, 18, 19, 24, 25, 26, 27, 31}
GTE_CTRL = set(range(0, 24))


ALU_OPS = {"sll": "<<", "srl": ">>", "sra": ">>", "addu": "+", "addiu": "+", "subu": "-",
           "or": "|", "ori": "|", "and": "&", "andi": "&", "xor": "^", "xori": "^"}


def gte_command_ok(word: int) -> bool:
    if (word >> 25) != 0x25:            # 0100101 = COP2 + command bit
        return False
    cmd = word & 0x1FFFFFF
    op = cmd & 0x3F
    if op == 0x06:
        return True                     # NCLIP
    if op == 0x12:
        mx, v, cv = (cmd >> 17) & 3, (cmd >> 15) & 3, (cmd >> 13) & 3
        return mx == 0 and v in (0, 3) and cv in (0, 3)
    if op in (0x01, 0x30):              # RTPS / RTPT, sf=1 lm=0
        return (cmd >> 19) & 1 == 1 and (cmd >> 10) & 1 == 0
    return False


def _scan(text: str, i: int, close: str) -> int:
    """Index just past the `close` matching the opener before text[i]."""
    depth = 1
    while i < len(text):
        ch = text[i]
        if ch in "\"'":
            q = ch
            i += 1
            while i < len(text) and text[i] != q:
                i += 2 if text[i] == "\\" else 1
        elif ch in "([{":
            depth += 1
        elif ch in ")]}":
            depth -= 1
            if depth == 0:
                return i + 1
        i += 1
    raise LowerError("asm: unbalanced")


def _split_top(s: str, sep: str) -> list[str]:
    out, depth, cur, i = [], 0, [], 0
    while i < len(s):
        ch = s[i]
        if ch in "\"'":
            j = i + 1
            while j < len(s) and s[j] != ch:
                j += 2 if s[j] == "\\" else 1
            cur.append(s[i:j + 1])
            i = j + 1
            continue
        if ch in "([{":
            depth += 1
        elif ch in ")]}":
            depth -= 1
        if ch == sep and depth == 0:
            out.append("".join(cur))
            cur = []
        else:
            cur.append(ch)
        i += 1
    out.append("".join(cur))
    return out


STR_RE = re.compile(r'"((?:[^"\\]|\\.)*)"')


def _template(sec: str) -> str:
    rest = STR_RE.sub("", sec).strip()
    if rest:
        raise LowerError("asm: non-literal template")
    raw = "".join(STR_RE.findall(sec))
    return raw.replace("\\n", "\n").replace("\\t", " ").replace('\\"', '"')


def _operands(sec: str) -> list[tuple[str, str]]:
    sec = sec.strip()
    if not sec:
        return []
    ops = []
    for item in _split_top(sec, ","):
        m = re.fullmatch(r'\s*"([^"]*)"\s*\((.*)\)\s*', item, re.S)
        if not m:
            raise LowerError("asm: operand form")
        ops.append((m.group(1).strip(), m.group(2).strip()))
    return ops


def _reg(tok: str, kind: str):
    """('gpr', n) / ('op', k) / ('cop', n)."""
    tok = tok.strip()
    m = re.fullmatch(r"%(\d+)", tok)
    if m:
        return ("op", int(m.group(1)))
    m = re.fullmatch(r"\$(\d+)", tok)
    if m:
        return (kind, int(m.group(1)))
    raise LowerError(f"asm: register {tok}")


def _mem(tok: str):
    m = re.fullmatch(r"\s*(-?(?:0x[0-9A-Fa-f]+|\d+))?\s*\(\s*([%$]\d+)\s*\)\s*", tok)
    if not m:
        raise LowerError(f"asm: memory operand {tok.strip()}")
    return int(m.group(1) or "0", 0), _reg(m.group(2), "gpr")


def compile_asm(tpl: str, outs, ins) -> list[tuple]:
    """Template -> micro-ops; raises LowerError outside the supported set."""
    nops = len(outs) + len(ins)
    for c, _ in outs:
        if c not in ("=r", "=&r"):
            raise LowerError(f"asm: output constraint {c}")
    for c, _ in ins:
        if not (c in ("r", "i") or (c.isdigit() and int(c) < len(outs))):
            raise LowerError(f"asm: input constraint {c}")
    tied = {int(c) for c, _ in ins if c.isdigit()}
    written = set(tied) | set(range(len(outs), nops))   # operands holding a value
    gpr_set: set[int] = set()
    ops: list[tuple] = []

    def src(r):
        if r[0] == "op":
            if r[1] >= nops:
                raise LowerError("asm: operand index")
            if r[1] not in written:
                raise LowerError("asm: output read before written")
        elif r[1] not in gpr_set:
            raise LowerError(f"asm: ${r[1]} read before written")
        return r

    def dst(r):
        if r[0] == "op":
            if r[1] >= len(outs):
                raise LowerError("asm: write to input operand")
            written.add(r[1])
        else:
            if r[1] == 0 or r[1] in (29, 31):
                raise LowerError(f"asm: write to ${r[1]}")
            gpr_set.add(r[1])
        return r

    for line in re.split(r"[\n;]", tpl):
        line = line.strip()
        if not line or line == "nop":
            continue
        mnem, _, rest = line.partition(" ")
        args = [a.strip() for a in _split_top(rest, ",")] if rest.strip() else []
        if mnem in ("lw", "lhu", "lh", "lbu", "lb") and len(args) == 2:
            off, base = _mem(args[1])
            ops.append(("load", mnem, off, src(base), dst(_reg(args[0], "gpr"))))
        elif mnem in ALU_OPS and len(args) == 3:
            a = src(_reg(args[1], "gpr"))
            if mnem in ("sll", "srl", "sra", "addiu", "ori", "andi", "xori"):
                b = ("imm", int(args[2], 0) & 0xFFFFFFFF)
            else:
                b = src(_reg(args[2], "gpr"))
            ops.append(("alu", mnem, dst(_reg(args[0], "gpr")), a, b))
        elif mnem in ("sw", "sh", "sb") and len(args) == 2:
            off, base = _mem(args[1])
            ops.append(("store", mnem, off, src(base), src(_reg(args[0], "gpr"))))
        elif mnem in ("ctc2", "mtc2") and len(args) == 2:
            r = src(_reg(args[0], "gpr"))
            n = _reg(args[1], "cop")[1]
            if n not in (GTE_CTRL if mnem == "ctc2" else GTE_DATA_W):
                raise LowerError(f"asm: {mnem} ${n}")
            ops.append((mnem, r, n))
        elif mnem in ("cfc2", "mfc2") and len(args) == 2:
            n = _reg(args[1], "cop")[1]
            if n not in (GTE_CTRL if mnem == "cfc2" else GTE_DATA_R):
                raise LowerError(f"asm: {mnem} ${n}")
            ops.append((mnem, dst(_reg(args[0], "gpr")), n))
        elif mnem in ("lwc2", "swc2") and len(args) == 2:
            n = _reg(args[0], "cop")[1]
            off, base = _mem(args[1])
            if n not in (GTE_DATA_W if mnem == "lwc2" else GTE_DATA_R):
                raise LowerError(f"asm: {mnem} ${n}")
            ops.append((mnem, n, off, src(base)))
        elif mnem == ".word" and len(args) == 1:
            w = int(args[0], 0)
            if not gte_command_ok(w):
                raise LowerError(f"asm: cop2 command 0x{w:08X}")
            ops.append(("cmd", w & 0x1FFFFFF))
        else:
            raise LowerError(f"asm: instruction {mnem}")
    for k in range(len(outs)):
        if k not in written:
            raise LowerError("asm: output never written")
    return ops


ASM_SET_RE = re.compile(r'\b(?:__asm__|asm)\s*\(\s*"\s*\.set\s+([A-Za-z_]\w*)\s*,\s*([A-Za-z_]\w*)\s*"\s*\)\s*;')


def apply_asm_sets(text: str, aliases: dict[str, str]) -> str:
    """File-scope `asm(".set ALIAS, SYM");`: functions are renamed to SYM at
    every use, objects keep their declaration and resolve to SYM's address."""
    sets = [(m.group(1), m.group(2)) for m in ASM_SET_RE.finditer(text)]
    text = ASM_SET_RE.sub("", text)
    for alias, sym in sets:
        if FUNC_NAME_RE.fullmatch(sym):
            text = re.sub(rf"(?<![\w.>])\b{re.escape(alias)}\b", sym, text)
        elif DATA_NAME_RE.fullmatch(sym):
            aliases[alias] = sym
        else:
            raise LowerError(f"asm: .set target {sym}")
    return text


ASM_LABEL_RE = re.compile(r'\b(?:__asm__|asm)\s*\(\s*"([A-Za-z_]\w*)"\s*\)\s*;')


def apply_asm_labels(text: str) -> tuple[str, dict[str, str]]:
    """Resolve `extern ... name asm("SYM");` symbol labels.

    Objects keep their own declaration (and so its type) and resolve to the
    retail address of SYM (returned alias map).  Functions are renamed to SYM
    at every use; a separate declaration of SYM, if any, wins."""
    aliases: dict[str, str] = {}
    text = apply_asm_sets(text, aliases)
    while True:
        m = ASM_LABEL_RE.search(text)
        if not m:
            return text, aliases
        sym = m.group(1)
        start = max(text.rfind(";", 0, m.start()), text.rfind("}", 0, m.start()),
                    text.rfind("{", 0, m.start())) + 1
        decl = text[start:m.start()].rstrip()
        # A function declarator is extern by default (C89 6.1.2.2) unless static.
        if not (re.match(r"\s*extern\b", decl)
                or (decl.endswith(")") and not re.search(r"\bstatic\b", decl))):
            raise LowerError("asm: label on a non-extern declaration")
        core, is_fn = decl, False
        if core.endswith(")"):                      # function declarator
            depth, i = 0, len(core) - 1
            while i >= 0:
                depth += core[i] == ")"
                depth -= core[i] == "("
                if depth == 0:
                    break
                i -= 1
            core, is_fn = core[:i].rstrip(), True
        core = re.sub(r"(\s*\[[^\]]*\])+$", "", core)
        nm = re.search(r"([A-Za-z_]\w*)$", core)
        if not nm:
            raise LowerError("asm: label declarator")
        name = nm.group(1)
        if not is_fn:
            if not DATA_NAME_RE.fullmatch(sym):
                raise LowerError(f"asm: object label {sym}")
            aliases[name] = sym
            text = text[:start] + decl + ";" + text[m.end():]
            continue
        if not FUNC_NAME_RE.fullmatch(sym):
            raise LowerError(f"asm: function label {sym}")
        rest = text[:start] + text[m.end():]
        if re.search(rf"\b{re.escape(sym)}\b", rest):
            decl2 = ""                              # SYM declared directly: keep that one
        else:
            decl2 = decl[:nm.start()] + sym + decl[nm.start() + len(name):] + ";"
        body = re.sub(rf"(?<![\w.>])\b{re.escape(name)}\b", sym, rest[start:])
        text = rest[:start] + decl2 + body

def extract_asm(text: str) -> tuple[str, list[dict]]:
    """Replace each supported asm statement with `__pe_asm(K, operands...)`."""
    text, aliases = apply_asm_labels(text)
    table: list[dict] = [{"aliases": aliases}]
    out, pos = [], 0
    while True:
        m = ASM_HEAD_RE.search(text, pos)
        if not m:
            break
        end = _scan(text, m.end(), ")")
        tail = re.match(r"\s*;", text[end:])
        if not tail:
            raise LowerError("asm: not a statement")
        secs = _split_top(text[m.end():end - 1], ":")
        if len(secs) > 4:
            raise LowerError("asm: goto form")
        secs += [""] * (4 - len(secs))
        tpl = _template(secs[0])
        outs, ins = _operands(secs[1]), _operands(secs[2])
        if (not tpl.strip() and not outs
                and all(c in ("m", "r", "i") and not re.search(r"\+\+|--|[^=!<>]=[^=]|\w\s*\(", " " + e)
                        for c, e in ins)):
            # Empty template with side-effect-free inputs only (e.g. an "m"
            # fence that pins a store's position for cc1): nothing to run.
            out.append(text[pos:m.start()])
            out.append(";")
            pos = end + tail.end()
            continue
        ops = compile_asm(tpl, outs, ins)
        table.append({"outs": outs, "ins": ins, "ops": ops})
        args = "".join(f", ({e})" for _, e in outs + ins)
        out.append(text[pos:m.start()])
        out.append(f"__pe_asm({len(table) - 1}{args});")
        pos = end + tail.end()
    out.append(text[pos:])
    return "".join(out), table


_INT_LIT = re.compile(r"\(*\s*(-?)\s*(0[xX][0-9A-Fa-f]+|\d+)[uUlL]*\s*\)*")


def safe_divisor(b: str) -> bool:
    """True when the lowered divisor is an integer literal other than 0 and -1
    (no zero trap and no INT_MIN / -1 overflow possible)."""
    m = _INT_LIT.fullmatch(b.strip())
    if not m:
        return False
    v = int(m.group(2), 0)
    return v != 0 and not (m.group(1) and v == 1)


def host_pointer_param(decl: str) -> bool:
    d = decl.strip()
    return "*" in d or "[" in d


HELPERS = {
    "host": "static inline void *pe_low_host(uint32_t a) { return a ? PE_Translate(a, 1u) : (void *)0; }",
    "ld64": "static inline uint64_t pe_low_ld64(uint32_t a) { return (uint64_t)PE_LoadU32(a) | ((uint64_t)PE_LoadU32(a + 4u) << 32); }",
    "st64": "static inline void pe_low_st64(uint32_t a, uint64_t v) { PE_StoreU32(a, (uint32_t)v); PE_StoreU32(a + 4u, (uint32_t)(v >> 32)); }",
    "copy_gg": "static inline void pe_low_copy_gg(uint32_t d, uint32_t s, uint32_t n) { uint32_t i; for (i = 0; i < n; i++) PE_StoreU8(d + i, PE_LoadU8(s + i)); }",
    "copy_gb": "static inline void pe_low_copy_gb(uint32_t d, const uint8_t *s, uint32_t n) { uint32_t i; for (i = 0; i < n; i++) PE_StoreU8(d + i, s[i]); }",
    "copy_bg": "static inline void pe_low_copy_bg(uint8_t *d, uint32_t s, uint32_t n) { uint32_t i; for (i = 0; i < n; i++) d[i] = PE_LoadU8(s + i); }",
    "copy_bb": "static inline void pe_low_copy_bb(uint8_t *d, const uint8_t *s, uint32_t n) { __builtin_memmove(d, s, n); }",
}
# Runtime division (see arith_op): R3000 results, zero divisor = guest trap.
HELPERS["divs"] = ("static inline int32_t pe_low_divs(int32_t a, int32_t b, const char *fn) "
                   "{ if (b == 0) { PE_Decomp_GuestTrapDivZero(fn); return a < 0 ? 1 : -1; } "
                   "if (b == -1) return (int32_t)(0u - (uint32_t)a); return a / b; }")
HELPERS["mods"] = ("static inline int32_t pe_low_mods(int32_t a, int32_t b, const char *fn) "
                   "{ if (b == 0) { PE_Decomp_GuestTrapDivZero(fn); return a; } "
                   "if (b == -1) return 0; return a % b; }")
HELPERS["divu"] = ("static inline uint32_t pe_low_divu(uint32_t a, uint32_t b, const char *fn) "
                   "{ if (b == 0) { PE_Decomp_GuestTrapDivZero(fn); return 0xFFFFFFFFu; } return a / b; }")
HELPERS["modu"] = ("static inline uint32_t pe_low_modu(uint32_t a, uint32_t b, const char *fn) "
                   "{ if (b == 0) { PE_Decomp_GuestTrapDivZero(fn); return a; } return a % b; }")
for _n in (1, 2, 4, 8):
    HELPERS[f"hld{_n}"] = (f"static inline uint{_n * 8}_t pe_low_hld{_n}(const uint8_t *p) "
                           f"{{ uint{_n * 8}_t v; __builtin_memcpy(&v, p, {_n}); return v; }}")
    HELPERS[f"hst{_n}"] = (f"static inline void pe_low_hst{_n}(uint8_t *p, uint{_n * 8}_t v) "
                           f"{{ __builtin_memcpy(p, &v, {_n}); }}")


def lower_leaf(name: str, text: str, known: set[str], sigs: dict[str, dict],
               strip_asm=None) -> dict:
    """Lower one matched leaf.  Returns {'sig','body','helpers','callees'}."""
    # cpp drops comments too, but the asm passes below run first and would
    # read an `asm(...)` quoted in a comment as code.
    text = strip_c_comments(text)
    if strip_asm is not None:
        text = strip_asm(text)
    text = preprocess(text)
    text, asm_table = extract_asm(text)
    if re.search(r"\b(?:__asm__|asm)\b", text):
        raise LowerError("asm")
    for lit in re.findall(r"\b0x[0-9A-Fa-f]{8}\b", text):
        v = int(lit, 16)
        # MMIO / BIOS spaces stay refused; the scratchpad is guest memory.
        if (v & 0x1FFFF000) in (0x1F801000, 0x1F802000) or (v & 0xFF000000) in (0xA0000000, 0xB0000000) \
                or (v & 0x1FF00000) == 0x1FC00000:
            raise LowerError(f"hardware/BIOS literal {lit}")
    low = Lowerer(name, text, known, sigs)
    low.asm_table = asm_table
    low.aliases = asm_table[0]["aliases"]
    sig, body, ret = low.lower()
    return {"sig": sig, "body": body, "ret": ret,
            "helpers": [HELPERS[h] for h in sorted(low.helpers)],
            "callees": sorted(low.callees - {name})}


def render_lowered(name: str, leaf: dict, res: dict, protos: list[str],
                   evidence: str) -> str:
    lines = [
        "/*\n"
        f" * decomp-source: {name}\n"
        f" * matched VMA 0x{leaf['vram']:08X}  file 0x{leaf['file_offset']:X}"
        f"  span 0x{leaf['file_size']:X} bytes / {leaf['words']} words\n"
        f" * evidence: {evidence}\n"
        " *\n"
        " * GENERATED by tools/analysis/gen_decomp_ports.py from"
        f" src/{name}.c — do not\n"
        " * edit by hand; the matching leaf is the authority.  E21 guest-memory\n"
        " * lowering: the body is the leaf's own statements with every pointer a\n"
        " * 32-bit guest address and every access an explicit guest load/store at\n"
        " * the retail (O32) layout offset; calls through code pointers held in\n"
        " * guest RAM are retail jalr (PE_GuestCall).  See tools/analysis/guest_lower.py.\n"
        " */\n",
        '#include "pe_guest_decomp.h"\n',
    ]
    if res["helpers"]:
        lines.append("/* guest-lowering accessors */")
        lines += res["helpers"]
        lines.append("")
    if protos:
        lines.append("/* host prototypes of decomp-derived callees (generated TUs) */")
        lines += protos
        lines.append("")
    if res["sig"] is None:      # frame-owning leaf: body already carries both definitions
        lines.append(f"{res['body']}\n}}\n")
    else:
        lines.append(f"{res['sig']}\n{{\n{res['body']}\n}}\n")
    return "\n".join(lines)
