register volatile unsigned int *hw asm("$3");

void func_8007E370(void) {
    if ((hw[0x1074 / 4] & 0x80) != 0) {
        while ((hw[0x1044 / 4] & 0x80) != 0) {
        }
        goto *(void *)(*(unsigned int *)0xDFFC);
    }
}
