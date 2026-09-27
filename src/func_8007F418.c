extern int D_800A3610;
extern int D_800A3614;
extern int D_800A3618;
extern int D_800A361C;
extern int D_800A3690;
extern int D_800A3500;
extern int func_8007FC64(int);
extern void func_80080998(unsigned char *, unsigned char *);

int func_8007F418(int seq, unsigned char *resp)
{
    register int i asm("$4");
    register int idx asm("$3");
    int off;
    int jdx;
    int rec;

    if (seq != 0) {
        int sh;

        func_8007FC64(0);
        idx = D_800A3690;
        i = 0;
        sh = idx << 4;
        for (;;) {
            if (*(int *)((char *)&D_800A3610 + sh) == seq) {
                goto set7;
            }
            idx = idx + 1;
            if (idx >= 8) {
                idx = 0;
            }
            i = i + 1;
            if (i >= 8) {
                break;
            }
            sh = idx << 4;
        }
        idx = 0;
        if (seq >= *(int *)((char *)&D_800A3610 + (D_800A3690 << 4))) {
            goto chk;
        }
set7:
        idx = 7;
chk:
        if (idx != 7) {
            return 0;
        }
        {
            register int base asm("$2");

            base = D_800A3690;
            idx = base - 1;
        }
        if (idx < 0) {
            idx = 7;
        }
        if (seq != 0) {
            i = 0;
            off = idx << 4;
            do {
                if (*(int *)((char *)&D_800A3610 + off) == seq) {
                    goto hit;
                }
                idx = idx - 1;
                if (idx < 0) {
                    idx = 7;
                }
                i = i + 1;
                off = idx << 4;
            } while (i < 8);
            rec = 0;
            goto done;
        } else {
            off = idx << 4;
            if (*(int *)((char *)&D_800A3610 + off) != 0) {
                goto hit;
            }
            rec = 0;
            asm volatile("");
            goto done;
        }
    } else {
        jdx = D_800A3690 - 1;
        off = jdx << 4;
        if (jdx < 0) {
            jdx = 7;
            asm volatile("" : "=r"(jdx) : "0"(jdx));
            off = jdx << 4;
        }
        if (*(int *)((char *)&D_800A3610 + off) == 0) {
            rec = 0;
            goto done;
        }
    }
hit:
    {
        register int *dst asm("$5");
        register int w0 asm("$2");
        register int w1 asm("$3");
        register int w2 asm("$4");

        w0 = *(int *)((char *)&D_800A3610 + off);
        w1 = *(int *)((char *)&D_800A3614 + off);
        w2 = *(int *)((char *)&D_800A3618 + off);
        dst = &D_800A3500;
        asm volatile("" : "=r"(dst) : "0"(dst));
        dst[0] = w0;
        dst[1] = w1;
        dst[2] = w2;
        dst[3] = *(int *)((char *)&D_800A361C + off);
        asm volatile("" : "=r"(dst) : "0"(dst));
        rec = (int)&D_800A3500;
    }
done:
    if (rec == 0) {
        return 6;
    }
    func_80080998(resp, (unsigned char *)rec + 5);
    return ((unsigned char *)rec)[4];
}
