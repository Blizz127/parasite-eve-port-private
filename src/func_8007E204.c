extern void (*jtbl_800A34C8)();

void func_8007E204(void) {
    register void (*f)() asm("$9");
    f = jtbl_800A34C8;
    goto *(void *)f;
}
