"""LOCAL PATCH K3c: MASPSX_NARROW_SHIFTED_ANY_DEST.

With K3/K3b on, `lw $d,...` ... `sra $x,$d,N` (x != d, 16 <= N <= 31)
also narrows to `lh $d,...+2` ... `sra $x,$d,N-16` when $d is dead after
the shift (the next straight-line reference writes $d). Window:
room_m0350i func_80192E4C (retail `lh $2,2($2)` / `sra $18,$2,5`).
"""
import os
import unittest
from unittest.mock import patch

from maspsx import MaspsxProcessor

from .util import strip_comments


def window(shift="21", after=("addiu\t$2,$0,3",), label=True):
    out = [
        ".set\treorder",
        "lw\t$2,0($2)",
        f"sra\t$18,$2,{shift}",
    ]
    if label:
        out.append("$L7:")
    out += list(after)
    out += ["jal\tfunc_800CEE20", "sw\t$18,28($sp)"]
    return out


def kept(lines):
    return [x for x in strip_comments(lines)
            if x and not x.startswith(".") and not x.endswith(":")]


class TestNarrowShiftedAnyDest(unittest.TestCase):
    @staticmethod
    def process(lines, **env):
        base = {"MASPSX_NARROW_SHIFTED_REG_LOAD": "0",
                "MASPSX_NARROW_SHIFTED_WORD_LOAD": "0",
                "MASPSX_NARROW_SHIFTED_ANY_DEST": "0"}
        base.update(env)
        with patch.dict(os.environ, base):
            return MaspsxProcessor(list(lines)).process_lines()

    ON = {"MASPSX_NARROW_SHIFTED_REG_LOAD": "1",
          "MASPSX_NARROW_SHIFTED_ANY_DEST": "1"}

    def test_narrows_other_dest_when_dead(self):
        body = kept(self.process(window(), **self.ON))
        self.assertIn("lh\t$2,2($2)", body)
        self.assertIn("sra\t$18,$2,5", body)
        self.assertNotIn("lw\t$2,0($2)", body)

    def test_shift_of_16_becomes_move(self):
        body = kept(self.process(window("16"), **self.ON))
        self.assertIn("lh\t$2,2($2)", body)
        self.assertIn("addu\t$18,$2,$0", body)

    def test_live_dest_is_left(self):
        for after in (("addu\t$3,$2,1",), ("addu\t$2,$2,1",)):
            body = kept(self.process(window(after=after), **self.ON))
            self.assertIn("lw\t$2,0($2)", body, after)

    def test_branch_before_redefinition_is_left(self):
        body = kept(self.process(
            window(after=("bne\t$3,$0,$L9", "addiu\t$2,$0,3")), **self.ON))
        self.assertIn("lw\t$2,0($2)", body)

    def test_k3c_needs_k3_or_k3b(self):
        self.assertEqual(
            self.process(window(), MASPSX_NARROW_SHIFTED_ANY_DEST="1"),
            self.process(window()),
        )

    def test_k3b_alone_leaves_other_dest(self):
        self.assertEqual(
            self.process(window(), MASPSX_NARROW_SHIFTED_REG_LOAD="1"),
            self.process(window()),
        )


if __name__ == "__main__":
    unittest.main()
