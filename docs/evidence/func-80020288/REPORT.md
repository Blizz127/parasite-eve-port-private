# func_80020288 — MATCHED (474 words, LINK_EXACT)

VRAM `0x80020288`, span `0x768`. era `-O2 -G0` with
`MASPSX_DISPATCH_FOLD=jtbl_8001070C` and
`MASPSX_THREE_WORD_SYMBOL_STORE=1`.

The `switch (e->f18->f1)` table is retail `jtbl_8001070C`. Without the
fold the indexed load is `0x800209F8` (2 words). With it,
`LEAF_STRONG=EXACT` span `0x768`, compiled `0x770`.
