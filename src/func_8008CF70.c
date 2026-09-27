typedef struct {
    int mask;
    unsigned char body[64];
} RevPreset;

extern long D_8009B46C[];
typedef struct {
    long mask;
    long mode;
    short depth_l;
    short depth_r;
    long delay;
    long feedback;
} RevAttr;

extern RevAttr D_8009B39C;
extern long D_8009B398;
extern volatile unsigned short *D_8009B3FC;
extern unsigned char D_8009C8C0[];
extern long func_80085BB4(long a0);
extern void func_8008D140(RevPreset *a0);
extern long func_8008D610(long a0);
extern long func_8007DAE0(long a0, long a1, long a2);

long func_8008CF70(long a0)
{
    RevPreset entry;
    unsigned char *p;
    unsigned char *q;
    long clear;
    long on;
    long n;
    long *sa;

    clear = 0;
    if (a0 & 0x100) {
        a0 &= ~0x100;
        clear = 1;
    }
    if ((unsigned long)a0 >= 10) {
        goto fail;
    }
    {
        register long off asm("$2") = a0 << 2;
        sa = (long *)((char *)D_8009B46C + off);
    }
    if (func_80085BB4(*sa) != 0) {
fail:
        return -1;
    }
    q = (unsigned char *)&entry;
    p = D_8009C8C0 + a0 * 68;
    D_8009B39C.mode = a0;
    D_8009B398 = *sa;
    n = 67;
    do {
        *q++ = *p++;
    } while (--n != -1);
    entry.mask = 0;
    switch (a0) {
    case 7:
        D_8009B39C.feedback = 0x7F;
        D_8009B39C.delay = 0x7F;
        break;
    case 8:
        D_8009B39C.feedback = 0;
        D_8009B39C.delay = 0x7F;
        break;
    default:
        D_8009B39C.feedback = 0;
        D_8009B39C.delay = 0;
        break;
    }
    on = (D_8009B3FC[0xD5] & 0x80) != 0;
    if (on) {
        D_8009B3FC[0xD5] &= 0xFF7F;
    }
    D_8009B3FC[0xC2] = 0;
    D_8009B3FC[0xC3] = 0;
    D_8009B39C.depth_l = 0;
    D_8009B39C.depth_r = 0;
    func_8008D140(&entry);
    if (clear) {
        func_8008D610(a0);
    }
    func_8007DAE0(0xD1, D_8009B398, 0);
    if (on) {
        D_8009B3FC[0xD5] |= 0x80;
    }
    return 0;
}
