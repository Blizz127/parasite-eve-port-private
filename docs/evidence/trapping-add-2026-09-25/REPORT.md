# Trapping `add` leaves — no C spelling

`func_800370BC` and `func_800370CC` (each 4 words) are

```
ori  $at, $zero, 0x8000
add  $v0, $a0, $at     /* handwritten instruction */
jr   $ra
sra  $v0, $v0, 16      # 370CC shifts by 8
```

cc1 emits `addu` for C addition. The overflow-trapping `add` is marked
handwritten in the split asm. There is no C operator for it, and inline
asm is not an accepted match. Both stay original retail bytes.
