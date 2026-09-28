/* func_80071A84: Psy-Q libc sprintf-style formatter. */
struct Spec {
    unsigned int minus : 1;
    unsigned int plus : 1;
    unsigned int alt : 1;
    unsigned int zero : 1;
    unsigned int hasprec : 1;
    unsigned int h : 1;
    unsigned int l : 1;
    unsigned int L : 1;
    signed char sign;
    int width;
    int prec;
};

extern struct Spec D_80094528;
extern char D_8001161C[];
extern char D_80011630[];
extern int func_80072314(char *);
extern char *func_80072324(char *, int, int);
extern void func_80072334(char *, char *, int);

#define VA_ARG(ap, t) (*((t *)ctx.ap)++)

int func_80071A84(char *out, signed char *fmt, ...)
{
    signed char buf[512];
    struct { struct Spec s; int pad; char *ap; } ctx;
    int idx;
    int c;
    signed char *p;
    int n;
    unsigned int v;
    register char *digits asm("$7");
    signed char *r;
    unsigned int fl;

    ctx.ap = (char *)(&fmt + 1);
    idx = 0;
    for (c = *fmt; c != 0; c = *++fmt) {
        if (c != '%') {
            out[idx++] = c;
            continue;
        }
        {
            register struct Spec *tp asm("$5") = &D_80094528;
            __asm__("" : "=r"(tp) : "0"(tp));
            ctx.s = *tp;
        }
        for (;;) {
            c = *++fmt;
            if (c == '-') ctx.s.minus = 1;
            else if (c == '+') ctx.s.plus = 1;
            else if (c == ' ') ctx.s.sign = c;
            else if (c == '#') ctx.s.alt = 1;
            else if (c == '0') ctx.s.zero = 1;
            else break;
        }
        if (c == '*') {
            ctx.s.width = VA_ARG(ap, int);
            if (ctx.s.width < 0) {
                ctx.s.width = -ctx.s.width;
                ctx.s.minus = 1;
            }
            c = *++fmt;
        } else {
            while (c >= '0' && c <= '9') {
                ctx.s.width = ctx.s.width * 10 + (c - '0');
                c = *++fmt;
            }
        }
        if (c == '.') {
            c = *++fmt;
            if (c == '*') {
                ctx.s.prec = VA_ARG(ap, int);
                c = *++fmt;
            } else {
                while (c >= '0' && c <= '9') {
                    ctx.s.prec = ctx.s.prec * 10 + (c - '0');
                    c = *++fmt;
                }
            }
            if (ctx.s.prec >= 0) ctx.s.hasprec = 1;
        }
        p = buf + sizeof(buf);
        if (ctx.s.minus) ctx.s.zero = 0;
    again:
        switch (c) {
        case 'h':
            ctx.s.h = 1;
            c = *++fmt;
            goto again;
        case 'l':
            ctx.s.l = 1;
            c = *++fmt;
            goto again;
        case 'L':
            ctx.s.L = 1;
            c = *++fmt;
            goto again;
        case 'd':
        case 'i':
            v = VA_ARG(ap, int);
            __asm__ volatile("");
            fl = *(unsigned int *)&ctx.s;
            if ((fl >> 5) & 1) v = (short)v;
            if ((int)v < 0) {
                v = -v;
                ctx.s.sign = '-';
            } else if (ctx.s.plus) {
                ctx.s.sign = '+';
            }
            goto dec;
        case 'u':
            v = VA_ARG(ap, int);
            __asm__ volatile("");
            if (ctx.s.h) v &= 0xFFFF;
            ctx.s.sign = 0;
        dec:
            if (!ctx.s.hasprec) {
                if (ctx.s.zero) {
                    ctx.s.prec = ctx.s.width;
                    if (ctx.s.sign) ctx.s.prec = ctx.s.width - 1;
                }
                if (ctx.s.prec < 1) ctx.s.prec = 1;
            }
            for (n = 0; v != 0; v /= 10, n++) *--p = v % 10 + '0';
            for (; n < ctx.s.prec; n++) *--p = '0';
            if (ctx.s.sign) {
                *--p = ctx.s.sign;
                n++;
            }
            break;
        case 'o':
            v = VA_ARG(ap, int);
            __asm__ volatile("");
            if (ctx.s.h) v &= 0xFFFF;
            if (!ctx.s.hasprec) {
                if (ctx.s.zero) ctx.s.prec = ctx.s.width;
                if (ctx.s.prec < 1) ctx.s.prec = 1;
            }
            for (n = 0; v != 0; v >>= 3, n++) *--p = (v & 7) + '0';
            if (ctx.s.alt && n != 0 && *p != '0') {
                *--p = '0';
                n++;
            }
            for (; n < ctx.s.prec; n++) *--p = '0';
            break;
        case 'p':
            __asm__ volatile("");
            __asm__ volatile("");
            __asm__ volatile("");

            ctx.s.prec = 8;
            ctx.s.hasprec = 1;
            ctx.s.l = 1;
        case 'X':
            digits = D_8001161C;
            goto hex;
        case 'x':
            digits = D_80011630;
        hex:
            v = VA_ARG(ap, int);
            __asm__ volatile("");
            if (ctx.s.h) v &= 0xFFFF;
            if (!ctx.s.hasprec) {
                if (ctx.s.zero) {
                    ctx.s.prec = ctx.s.width;
                    if (ctx.s.alt) ctx.s.prec = ctx.s.width - 2;
                }
                if (ctx.s.prec < 1) ctx.s.prec = 1;
            }
            for (n = 0; v != 0; v >>= 4, n++) *--p = digits[v & 0xF];
            for (; n < ctx.s.prec; n++) *--p = '0';
            if (ctx.s.alt) {
                *--p = c;
                *--p = '0';
                n += 2;
            }
            break;
        case 'c':
            *--p = VA_ARG(ap, int);
            n = 1;
            break;
        case 's':
            p = VA_ARG(ap, signed char *);
            if (ctx.s.alt) {
                n = *(unsigned char *)p++;
                if (ctx.s.hasprec && ctx.s.prec < n) n = ctx.s.prec;
            } else if (!ctx.s.hasprec) {
                n = func_80072314((char *)p);
            } else {
                r = (signed char *)func_80072324((char *)p, 0, ctx.s.prec);
                if (r != 0) n = r - p;
                else n = ctx.s.prec;
            }
            break;
        case 'n':
            fl = *(unsigned int *)&ctx.s;
            p = VA_ARG(ap, signed char *);
            __asm__ volatile("" : "=r"(fl) : "0"(fl));
            if ((fl >> 5) & 1) *(short *)p = idx;
            else *(int *)p = idx;
            continue;
        default:
            if (c != '%') goto done;
            out[idx++] = c;
            continue;
        }
        if (n < ctx.s.width && !ctx.s.minus) {
            for (; n < ctx.s.width; ctx.s.width--) out[idx++] = ' ';
        }
        func_80072334(out + idx, (char *)p, n);
        idx += n;
        for (; n < ctx.s.width; n++) out[idx++] = ' ';
    }
done:
    out[idx] = 0;
    return idx;
}
