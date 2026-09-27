# func_800D004C

VRAM `0x800D004C`, declared size `0x4` (one word), `asm/disc1/C084C.s`.

The span is the single instruction `addiu $sp, $sp, -0xF0`. The next YAML asm span, at `0xC0850`, is labeled `func_800D0050` (`0x6D8`, `asm/disc1/C0850.s`) and starts with `sw $s3, 0xD4($sp)`. That save is inside the `0xF0` frame. Its epilogue is `addiu $sp, $sp, 0xF0` / `jr $ra`. Callers `jal func_800D004C` (`asm/disc1/CE8A8.s`, `asm/disc1/C5A90.s`). `func_800D0050` has no reference outside its own file.

The two labels are one function that splat split after the stack adjustment. A C body for the declared `0x4` span cannot match: cc1 will not emit a lone `addiu` with no `jr`, and a full function's linked `.text` is not a 4-byte zero tail. It stays asm until the two spans are joined. Not integrated.
