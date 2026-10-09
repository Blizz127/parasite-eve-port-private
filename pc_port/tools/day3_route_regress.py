#!/usr/bin/env python3
"""Day-3 route regression (dev harness): the Day 3 story stretches that the
game plays through its OWN room scripts, checked against ordered checkpoints.

Same method as day2_route_regress.py (shared harness): one dev warp
(PE_WARP_ROOM / PE_WARP_STORY) into a room whose script then drives the story
by itself; the pad alternates Cross and Circle; every later room change and
story write comes from the room scripts, not from the harness.

Most of Day 3 needs the player to walk or fight (warping m0092i at 0x140 or
m0360i at 0x140 just waits; m0123i at 0x190 is a fight the mashing pad loses),
so the gate covers the script-driven stretches only:

  day3_start  warp m0093i story 0x140: its script writes 0x148.
  day3_mid    warp m0108i story 0x188: its script writes 0x190.
  day3_end    warp m0352i story 0x1B0 (Day-4 hop): its room_transfer enters
              m0098i (Day 4 entry), whose script writes 0x1B8.
  day3_walk   warp m0192i story 0x178, then a recorded walk: the route pilot
              (pc_port/src/route_day3_pilot.h) replays the committed waypoint
              list day3_walk_waypoints.txt (room token, story, optional
              persist[0x2C] mask; world X,Z points, Cross holds, timeouts) and
              fights the random battles (aim shim + Cross). It walks the
              precinct and the hospital (two flag examines), wins the
              hospital battles and the m0123i fight, and must reach story
              0x1B0. Two opt-in fixtures, because the warp keeps Day-1 stats:
              PE_ROUTE_MAXHP=300 (max HP in battle) and cheat F3 (infinite
              ammo). The waypoints are the harness's own coordinates; no disc
              bytes. Opt-in (--segment day3_walk) until the whole walk passes.
A segment fails on a FATAL, CPU_BOUNDARY/REFUSED or [STOP] line, a story drop,
a checkpoint firing out of order, or a missed checkpoint.
"""
import argparse
import os
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from day2_route_regress import run_segment  # noqa: E402

SEGMENTS = {
    "day3_start": dict(room="m0093i", story="140", frames=8000, checkpoints=[
        ("token", 0xA80091C8, "m0093i (story 0x140)"),
        ("story>=", 0x148, "m0093i script writes story 0x148"),
    ]),
    "day3_mid": dict(room="m0108i", story="188", frames=8000, checkpoints=[
        ("token", 0xA8020448, "m0108i (story 0x188)"),
        ("story>=", 0x190, "m0108i script writes story 0x190"),
    ]),
    "day3_end": dict(room="m0352i", story="1B0", frames=8000, checkpoints=[
        ("token", 0xA8065148, "m0352i Day-4 hop (story 0x1B0)"),
        ("token", 0xA8009448, "m0098i Day 4 entry, by m0352i's room_transfer"),
        ("story>=", 0x1B8, "m0098i script writes story 0x1B8"),
    ]),
}

WALK_FILE = os.path.join(os.path.dirname(os.path.abspath(__file__)), "day3_walk_waypoints.txt")


def walk_waypoints() -> str:
    with open(WALK_FILE) as f:
        return "|".join(l.strip() for l in f if l.strip() and not l.startswith("#"))


SEGMENTS["day3_walk"] = dict(room="m0192i", story="178", frames=90000, checkpoints=[
    ("token", 0xA80033C8, "m0037i precinct (map index 7)"),
    ("story>=", 0x180, "m0037i entry scene writes 0x180"),
    ("token", 0xA80201C8, "m0103i hospital entrance"),
    ("story>=", 0x188, "m0103i door writes 0x188"),
    ("token", 0xA80203C8, "m0107i (battle)"),
    ("story>=", 0x190, "m0108i scene writes 0x190"),
    ("token", 0xA80210C8, "m0111i (body examine)"),
    ("token", 0xA80094C8, "m0099i (object examine)"),
    ("token", 0xA80212C8, "m0115i vision"),
    ("token", 0xA8021348, "m0116i"),
    ("token", 0xA8004248, "m0044i scene"),
    ("token", 0xA8022148, "m0122i"),
    ("token", 0xA80221C8, "m0123i fight"),
    ("story>=", 0x1B0, "m0123i fight won, story 0x1B0"),
], opt_in=True, env_fn=lambda: {"PE_ROUTE_WAYPOINTS": walk_waypoints(), "PE_CHEAT_KEYS": "1010:F3",
                   "PE_ROUTE_MAXHP": "300"})


def main() -> int:
    ap = argparse.ArgumentParser()
    ap.add_argument("--bin", default="build/pcbuild-disc/parasite-eve-port")
    ap.add_argument("--segment", choices=sorted(SEGMENTS), action="append")
    ap.add_argument("--log-dir", default="build/lanes/day3")
    ap.add_argument("--list", action="store_true")
    a = ap.parse_args()
    if a.list:
        for n, s in SEGMENTS.items():
            print(f"{n}: warp {s['room']} story 0x{s['story']}, {s['frames']} frames")
            for k, v, d in s["checkpoints"]:
                print(f"    {k:8s} {v:#010x}  {d}")
        return 0
    disc = os.environ.get("DISC")
    if not disc:
        print("REGRESS FAIL no_disc: set DISC to the Disc 1 image")
        return 2
    if not os.access(a.bin, os.X_OK):
        print(f"REGRESS FAIL no_binary: {a.bin}")
        return 2
    os.makedirs(a.log_dir, exist_ok=True)
    names = a.segment or [n for n, s in SEGMENTS.items() if not s.get("opt_in")]
    for n in names:
        if "env_fn" in SEGMENTS[n]:
            SEGMENTS[n]["env"] = SEGMENTS[n].pop("env_fn")()
    ok = all([run_segment(n, SEGMENTS[n], a, disc, os.environ.get("DISC2"), prefix="day3")
              for n in names])
    print("DAY3_ROUTE=" + ("PASS" if ok else "FAIL"))
    return 0 if ok else 1


if __name__ == "__main__":
    sys.exit(main())
