#!/usr/bin/env python3
"""Render one room leaf's port TU privately and run the e21 differential on it.

    python3 tools/analysis/e21_leaf.py room_m0429i func_801982A4 [--seeds 6]
        [--check-gte] [--coverage] [--keep DIR]

The leaf must already be a room leaf: a `c` span in its room YAML or an entry
in configs/USA/overlays/nonmatching.yaml (for NON_MATCHING C).  For a new
NON_MATCHING leaf, `--nonmatching OFF:SIZE` (blob offset and size, hex) adds
it for this run only, reading src/nonmatching/overlays/<room>/<leaf>.c, so
the shared manifest is edited once, after the leaf passes.  The room's
TUs are generated exactly as `gen_overlay_ports.py --rooms` does, but for
that one room and into a private directory (a temp dir, or --keep), so the
shared pc_port/game/decomp_ovl/rooms and the registry are never touched and
several leaves can be checked at once.  A one-room run forms the same
dedup groups for the room's own leaves, so the leaf's TU body is the same
as in a full run; only the member list in its header can be shorter.

Prints `e21_leaf: <status> <detail>` and exits 0 only on `ok`.
"""
from __future__ import annotations

import argparse
import shutil
import sys
import tempfile
import types
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import gen_overlay_ports as o  # noqa: E402
import e21_diff as e  # noqa: E402


def render_room(room: str, out_dir: Path) -> tuple[dict, dict]:
    """Generate `room`'s TUs into out_dir.  Returns (skipped, uncompilable)."""
    saved = o.room_ids, o.ROOMS_DIR
    o.room_ids = lambda: [room]
    o.ROOMS_DIR = out_dir.resolve()
    try:
        args = types.SimpleNamespace(check=False, verify=False, no_compile_check=False,
                                     verbose=False)
        _w, _c, skipped, uncompilable = o.run_rooms(args, o.all_windows())
    finally:
        o.room_ids, o.ROOMS_DIR = saved
    return skipped, uncompilable


def main() -> int:
    ap = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    ap.add_argument("room", help="room overlay id, e.g. room_m0429i")
    ap.add_argument("leaf", help="leaf name, e.g. func_801982A4")
    ap.add_argument("--seeds", type=int, default=6)
    ap.add_argument("--check-gte", action="store_true")
    ap.add_argument("--coverage", action="store_true")
    ap.add_argument("--keep", type=Path, help="write the room's TUs here (kept)")
    ap.add_argument("--nonmatching", metavar="OFF:SIZE",
                    help="treat the leaf as NON_MATCHING at this blob offset/size for this run")
    ns = ap.parse_args()
    if not ns.room.startswith("room_"):
        ns.room = "room_" + ns.room
    if ns.nonmatching:
        off, size = (int(x, 16) for x in ns.nonmatching.split(":"))
        src = o.NONMATCH_SRC_DIR / ns.room / f"{ns.leaf}.c"
        if not src.is_file():
            print(f"e21_leaf: nosrc {src}")
            return 2
        base = o.nonmatching_leaves

        def with_extra():
            out = base()
            out.setdefault(ns.room, {})[ns.leaf] = (off, size)
            return out
        o.nonmatching_leaves = with_extra
    ov = o.parse_overlay(ns.room)
    if ns.leaf not in ov["leaves"]:
        print(f"e21_leaf: noleaf {ns.leaf} is neither a c span of {ns.room} nor a NON_MATCHING leaf")
        return 2
    tmp = None
    if ns.keep:
        out = ns.keep
        out.mkdir(parents=True, exist_ok=True)
    else:
        tmp = tempfile.mkdtemp(prefix="e21_leaf_")
        out = Path(tmp)
    try:
        skipped, unc = render_room(ns.room, out)
        why = skipped.get(f"{ns.room}/{ns.leaf}")
        if why:
            print(f"e21_leaf: skipped {why}")
            return 1
        tus = [p for p in out.glob(f"{ns.leaf}__*_port.c")
               if f"decomp-source-overlay: {ns.room}/{ns.leaf}" in p.read_text(errors="replace")]
        if not tus:
            bad = [k for k in unc if k.startswith(ns.leaf + "__")]
            print(f"e21_leaf: notu {('uncompilable: ' + unc[bad[0]][:300]) if bad else 'no TU rendered'}")
            return 1
        cc = shutil.which("cc") or shutil.which("gcc")
        st, detail = e.one(tus[0], ns.leaf, ns.room, ns.seeds, cc, ns.coverage, "",
                           e.directed_specs(), ns.check_gte)
        print(f"e21_leaf: {st} {tus[0].name} {detail}")
        return 0 if st == "ok" else 1
    finally:
        if tmp:
            shutil.rmtree(tmp, ignore_errors=True)


if __name__ == "__main__":
    raise SystemExit(main())
