typedef void (*DsCb)(int, unsigned char *, unsigned char *);

typedef struct {
    int pos;                       /* 0x00 D_8009B6EC */
    int last;                      /* 0x04 */
    DsCb cb;                       /* 0x08 */
    int mode;                      /* 0x0C D_8009B6F8 */
    int cnt;                       /* 0x10 */
    int a;                         /* 0x14 D_8009B700 */
    int b;                         /* 0x18 D_8009B704 */
    int c;                         /* 0x1C */
    int d;                         /* 0x20 D_8009B70C */
} DsRcb;

extern DsRcb D_8009B6EC;
extern int D_8009B700;
extern int D_8009B704;
extern int D_8009B70C;
extern int func_8007FC28(void);
extern int func_80080C48(void *);
extern int func_8007FC18(void);
extern int func_800824F0(int);
extern void func_80080AE4(unsigned char *, int);
extern void func_800824C8(int);
extern void func_800824DC(int);
extern int func_8007F778(void);
extern void func_80082204(void);
extern int func_8007EE84(unsigned char, unsigned char *, void *, int);

void func_80081E70(unsigned char intr, unsigned char *result) {
    DsRcb *r = &D_8009B6EC;
    unsigned char buf[16];
    register int one asm("$19");
    int t;
    int n;
    int iv;

    if (r->pos == -1) {
        r->pos = func_80080C48((void *)func_8007FC28());
    }
    iv = intr;
    asm("" : "=r"(one) : "0"(1));
    if (iv == one) {
        if (func_8007FC18() & 0x20) {
            t = func_800824F0(0);
            func_80080AE4(buf, 3);
            func_800824F0(t);
            n = func_80080C48(buf);
            if (n != r->pos) {
                goto err;
            }
            if (r->cb != 0 && r->last < n) {
                r->cb(1, result, buf);
                r->last = n;
            }
        } else {
            if (r->cb != 0 && r->pos > r->last) {
                r->cb(1, result, buf);
                r->last = r->pos;
            }
        }
        D_8009B6EC.pos++;
    } else if (iv == 4) {
        if (r->cb != 0) {
            r->cb(4, result, buf);
        }
    } else {
        if (*result & 0x10) {
            if (r->c == one) {
                func_800824C8(0);
                return;
            }
            func_800824C8(r->a);
            func_800824DC(r->b);
            r->d = 0;
            if (r->cb != 0) {
                r->cb(iv, result, buf);
            }
            return;
        }
        if (func_8007F778() == 0 && (*result & 0xA0) == 0) {
        err:
            r->mode = one;
        }
    }
    if (D_8009B6EC.mode == 1) {
        if (D_8009B6EC.cnt > 0 || D_8009B6EC.cnt == -1) {
            func_80082204();
            if (D_8009B6EC.cnt > 0) {
                D_8009B6EC.cnt--;
            }
        } else {
            if (D_8009B70C == 1) {
                func_800824C8(D_8009B700);
                func_800824DC(D_8009B704);
                {
                    unsigned char *y;
                    asm("" : "=r"(y) : "0"(0));
                    func_8007EE84(9, y, 0, -1);
                }
            }
            D_8009B70C = 0;
            if (D_8009B6EC.cb != 0) {
                D_8009B6EC.cb(5, result, buf);
            }
        }
        D_8009B6EC.mode = 0;
    }
}
