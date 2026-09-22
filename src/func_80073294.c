typedef struct {
    unsigned int lo;
    unsigned int hi;
} S64;

S64 func_80073294(int dir, S64 v, int n)
{
    register int i asm("$4");
    unsigned int t;

    if (dir == 0) {
        i = 0;
        if (n > 0) {
            do {
                t = v.hi << 1;
                v.hi = t;
                if ((v.lo & 0x80000000) != 0) {
                    v.hi = t | 1;
                }
                v.lo = v.lo << 1;
                i++;
            } while (i < n);
        }
    } else {
        i = 0;
        if (n > 0) {
            do {
                t = v.lo >> 1;
                v.lo = t;
                if ((v.hi & 1) != 0) {
                    v.lo = t | 0x80000000;
                }
                v.hi = v.hi >> 1;
                i++;
            } while (i < n);
        }
    }
    return v;
}
