#!/usr/bin/env python3
"""Self-test for retail_data_guard.py and strip_retail_seeds.py.

Uses a synthetic, seeded pseudo-random "EXE" (no retail bytes) so it runs in
CI without a disc.  Run: python3 tools/analysis/test_retail_data_guard.py
"""
from __future__ import annotations

import random
import struct
import sys
import tempfile
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / "tools/analysis"))
import retail_data_guard as g  # noqa: E402
import strip_retail_seeds as srs  # noqa: E402

VRAM = 0x80010000


def fake_exe() -> bytes:
    rng = random.Random(1234)
    body = bytearray(rng.getrandbits(8) for _ in range(0x4000))
    body[0x2000:0x2200] = bytes(512)                       # zero fill
    body[0x2200:0x2280] = bytes(range(128))                # identity ramp
    body[0x2400:0x2480] = bytes([0, 1, 2, 0] * 32)         # low-entropy flags
    for i in range(16):                                    # pointer table
        struct.pack_into("<I", body, 0x2600 + 4 * i, 0x80012000 + 0x40 * i + (i % 3) * 4)
    hdr = bytearray(0x800)
    hdr[:8] = b"PS-X EXE"
    struct.pack_into("<II", hdr, 0x18, VRAM, len(body))
    return bytes(hdr + body)


EXE = fake_exe()
BODY = EXE[0x800:]


def ref_for(exe: bytes = EXE) -> g.Reference:
    r = g.Reference()
    r.add_blob("SLUS_006.62", exe[0x800:], 4, VRAM, tier2=True)
    return r


def words(off: int, n: int):
    return [struct.unpack_from("<I", BODY, off + 4 * i)[0] for i in range(n)]


def scan(text: str, ref=None):
    return g.scan_bytes("x.h", text.encode(), ref or ref_for(), set())


class GuardTest(unittest.TestCase):
    def test_hex_word_array_hits(self):
        t = "static const uint32_t T[]={" + ",".join(f"0x{w:08X}u" for w in words(0x100, 16)) + "};"
        hits = scan(t)
        self.assertEqual(len(hits), 1)
        self.assertEqual(hits[0][4], "data")
        self.assertIn("vaddr 0x80010100", hits[0][3])

    def test_pair_rows_hit(self):
        rows = ",\n".join(f"{{0x{0x10200 + 4 * i:X}u,0x{w:08X}u}}" for i, w in enumerate(words(0x200, 12)))
        self.assertTrue(scan("static const uint32_t R[][2]={\n" + rows + "\n};"))

    def test_decimal_byte_array_hits(self):
        t = "static const uint8_t B[]={" + ",".join(str(b) for b in BODY[0x301:0x341]) + "};"
        self.assertTrue(scan(t))

    def test_store_calls_hit(self):
        t = "\n".join(f"    PE_StoreU32(0x{VRAM + 0x400 + 4 * i:08X}u, 0x{w:08X}u);"
                      for i, w in enumerate(words(0x400, 10)))
        self.assertTrue(scan(t))

    def test_uninformative_windows_ignored(self):
        self.assertFalse(scan("uint8_t z[]={" + ",".join(["0"] * 64) + "};"))
        self.assertFalse(scan("enum{" + ",".join(f"A{i}={i}" for i in range(40)) + "};"))
        self.assertFalse(scan(" ".join(str(i) for i in range(128))))

    def test_short_copy_not_flagged(self):
        t = ",".join(f"0x{w:08X}" for w in words(0x500, 7))  # 28 bytes < window
        self.assertFalse(scan(t))

    def test_low_entropy_table_needs_long_run(self):
        flags = BODY[0x2400:0x2480]
        self.assertTrue(scan("uint8_t f[]={" + ",".join(str(b) for b in flags) + "};"))
        self.assertFalse(scan("uint8_t f[]={" + ",".join(str(b) for b in flags[:64]) + "};"))

    def test_pointer_table_is_address_class(self):
        t = ",".join(f"0x{w:08X}u" for w in words(0x2600, 16))
        hits = scan(t)
        self.assertTrue(hits)
        self.assertTrue(all(h[4] == "address-table" for h in hits))

    def test_binary_file_hits(self):
        blob = b"\x89junk" + BODY[0x700:0x780] + b"tail"
        hits = g.scan_bytes("x.bin", blob, ref_for(), set())
        self.assertTrue(hits)

    def test_signatures_detect_without_disc(self):
        r = g.Reference()
        for w, label in [(BODY[0x800:0x820], "known_table@0")]:
            b = g.blake8(w)
            import zlib
            r.sigs.setdefault(zlib.crc32(w), set()).add(b)
            r.sig_labels[b] = label
        t = ",".join(f"0x{w:08X}u" for w in words(0x800, 8))
        hits = g.scan_bytes("x.h", t.encode(), r, set())
        self.assertTrue(hits and "signature known_table" in hits[0][3])


class StripTest(unittest.TestCase):
    HEADER = (
        "static const uint32_t X_ranges[][2]={\n    {0x10100u,0x40u},\n};\n"
        "static const uint32_t X_common[][2]={\n"
        + "".join(f"    {{0x{0x10100 + 4 * i:X}u,0x{w:X}u}},\n" for i, w in enumerate(words(0x100, 16)))
        + "    {0x150000u,0x12345u},\n};\n"
        "static const uint8_t X_tab[]={1,2," + ",".join(str(b) for b in BODY[0x900:0x940]) + ",9};\n"
        "static const struct { unsigned first,end; uint64_t hash; } X_cases[]={{0,1,UINT64_C(0x1)}};\n"
    )

    def test_strip_then_guard_clean_and_restore_exact(self):
        ref = ref_for()
        new, n, fx = srs.strip(self.HEADER.encode(), ref, "RETAILFIX_x")
        self.assertEqual(n, 16 + 64)
        self.assertFalse(g.scan_bytes("x.h", new, ref_for(), set()))
        self.assertIn(b"static uint32_t X_common", new)
        self.assertIn(b"static uint8_t X_tab", new)
        self.assertIn(b"{0x150000u,0x12345u}", new)          # synthetic row kept
        again, n2, _ = srs.strip(new, ref, "RETAILFIX_x")
        self.assertEqual(again, new)                         # idempotent
        self.assertEqual(n2, 0)
        # restore from the fixup table and compare with the original values
        kinds = {(k, p) for k, p, *_ in fx}
        self.assertIn(("PE_RF_ROWS", "&X_common[0][0]"), kinds)
        self.assertIn(("PE_RF_BYTES", "X_tab"), kinds)
        for k, p, a, b, src, off in fx:
            self.assertEqual(src, "PE_RF_EXE")
            if k == "PE_RF_ROWS":
                self.assertEqual([struct.unpack_from("<I", EXE, off + 4 * i)[0] for i in range(b)],
                                 words(0x100, 16)[a:a + b])
            else:
                self.assertEqual(EXE[off:off + b], (bytes([1, 2]) + BODY[0x900:0x940] + b"\x09")[a:a + b])


if __name__ == "__main__":
    unittest.main()
