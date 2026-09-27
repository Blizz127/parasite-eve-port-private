extern void (*jtbl_800A34CC)();

void func_8007E218(void) {
    register void (*f)() asm("$9");
    f = jtbl_800A34CC;
    goto *(void *)f;
}
