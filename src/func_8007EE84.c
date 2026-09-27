typedef struct {
    int id;                                   /* 0x00 */
    unsigned char com;                        /* 0x04 */
    unsigned char param[7];                   /* 0x05 */
    unsigned char *pparam;                    /* 0x0C */
    void (*cb)(unsigned char, unsigned char *); /* 0x10 */
    int retry;                                /* 0x14 */
} DsQEnt;                                     /* 0x18 */

typedef struct {
    DsQEnt q[8];                              /* 0x00 */
    int tail;                                 /* 0xC0 */
    int head;                                 /* 0xC4 */
    int count;                                /* 0xC8 */
} DsQueue;

extern int D_8009B53C;
extern DsQueue D_800A3540;
extern int D_8009B4BC[];
extern DsQEnt *func_8007E6B0(void);
extern void func_80080950(unsigned char *, unsigned char *);
extern int func_8007FBF0(int);
extern void func_8007E8F4(void);

static __inline__ int push(unsigned char com, unsigned char *param,
                void (*cb)(unsigned char, unsigned char *), int retry) {
    int id;
    int t;
    int one;
    DsQEnt *e;

    if (D_800A3540.count >= 8) {
        return 0;
    }
    t = ++D_8009B53C;
    if (t == 0) {
        t = ++D_8009B53C;
    }
    id = t;
    e = func_8007E6B0();
    e->id = id;
    e->com = com;
    if (param != 0) {
        func_80080950(e->param, param);
        e->pparam = param;
    } else {
        e->pparam = 0;
    }
    e->cb = cb;
    e->retry = retry;
    D_800A3540.count++;
    t = func_8007FBF0(0);
    asm("" : "=r"(one) : "0"(1));
    if (t == one && D_800A3540.q[D_800A3540.head].id == id) {
        func_8007E8F4();
    }
    return id;
}

int func_8007EE84(unsigned char com, unsigned char *param,
                  void (*cb)(unsigned char, unsigned char *), int retry_) {
    register int retry asm("$21");
    int off = com * 4;
    asm volatile("");
    retry = retry_;
    if (*(int *)((char *)D_8009B4BC + off) != 0 && param != 0) {
        asm("" : "=r"(param) : "0"(param));
        if (push(2, param, 0, 0) == 0) {
            return 0;
        }
    }
    return push(com, param, cb, retry);
}
