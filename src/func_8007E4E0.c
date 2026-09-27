extern unsigned char func_8007E334[];
extern unsigned char func_8007E3B4[];

void func_8007E4E0(void) {
    register unsigned int *dst asm("$2");
    register unsigned int *src asm("$10");
    register unsigned int *end asm("$9");
    register unsigned int w asm("$3");
    dst = (unsigned int *)0xDF80;
    src = (unsigned int *)(func_8007E334 + 0x10);
    end = (unsigned int *)func_8007E3B4;
    do {
        w = *src;
        *dst = w;
        src += 1;
        dst += 1;
    } while (src != end);
}
