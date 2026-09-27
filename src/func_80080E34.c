typedef struct { char b[4]; } Blk4;

extern int D_8009B6CC;
extern int D_8009B6C4;
extern int func_800822AC(void);
extern Blk4 *func_80080CDC(int);
extern int func_8007F0C8(int, Blk4 *, int, void (*)(int), int);
extern void func_80080F64(int);
extern int func_80073A44(int);
extern int func_800824F0(void (*)(void));
extern void func_80081110(void);

int func_80080E34(Blk4 *loc, int sectors, int dest, int flags)
{
    register int *base asm("$16");
    char buf[8];
    int result;
    int *q;
    int vs;

    base = &D_8009B6CC;
    if (*base == 1) {
        goto fail;
    }
    if (func_800822AC() != 0) {
        goto fail;
    }
    goto body;
fail:
    return 0;
body:
    base[-8] = 0x200;
    base[-7] = dest;
    base[-6] = sectors;
    if (loc == 0) {
        __builtin_memcpy(buf, func_80080CDC(0), 4);
        flags |= 0x20;
    } else {
        __builtin_memcpy(buf, loc, 4);
        flags |= 0x20;
    }
    result = func_8007F0C8(flags & 0xFF, (Blk4 *)buf, 6, func_80080F64, -1);
    if (result == 0) {
        goto fail;
    }
    vs = func_80073A44(-1);
    q = &D_8009B6C4;
    q[0] = vs;
    if (q[-3] & 1) {
        q[-1] = (int)func_800824F0(func_80081110);
    }
    q[2] = 1;
    return result;
}
