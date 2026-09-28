typedef struct { int v; } __attribute__((packed)) UI;
typedef void (*DsCb)(unsigned char, unsigned char *);

typedef struct {
    int id;                                   /* 0x00 */
    unsigned char com;                        /* 0x04 */
    unsigned char param[7];                   /* 0x05 */
    unsigned char *pparam;                    /* 0x0C */
    DsCb cb;                                  /* 0x10 */
    int retry;                                /* 0x14 */
} DsQEnt;                                     /* 0x18 */

typedef struct {
    DsQEnt q[8];                              /* 0x00 */
    int tail;                                 /* 0xC0 */
    int head;                                 /* 0xC4 */
    int count;                                /* 0xC8 */
} DsQueue;

typedef struct {
    unsigned char com;                        /* 0x00 */
    unsigned char param[7];                   /* 0x01 */
    unsigned char *pparam;                    /* 0x08 */
    DsCb cb;                                  /* 0x0C */
} DsPkt;                                      /* 0x10 */

extern int D_8009B53C;
extern DsQueue D_800A3540;
extern int D_800A3608;
extern int D_800A3604;
extern int func_80080C48(unsigned char *);
extern DsQEnt *func_8007E6B0(void);
extern void func_80080950(unsigned char *, unsigned char *);
extern int func_8007FBF0(int);
extern void func_8007E8F4(void);

static __inline__ int newid(void) {
    int t = ++D_8009B53C;
    if (t == 0) {
        return ++D_8009B53C;
    }
    return t;
}

static __inline__ int push(DsPkt *cmd, int n, int retry) {
    int id;
    int i;
    int old;
    DsQEnt *e;

    if (D_800A3608 + n > 8) {
        return 0;
    }
    id = newid();
    for (i = 0; i < n; cmd++, i++) {
        e = func_8007E6B0();
        if (e == 0) {
            return 0;
        }
        e->id = id;
        e->com = cmd->com;
        if (cmd->pparam != 0) {
            func_80080950(e->param, cmd->param);
            e->pparam = e->param;
        } else {
            e->pparam = 0;
        }
        e->cb = cmd->cb;
        e->retry = retry;
        D_800A3540.count++;
    }
    if (func_8007FBF0(0) == 1 && D_800A3540.q[D_800A3604].id == id) {
        func_8007E8F4();
    }
    return id;
}

int func_8007F0C8(unsigned char mode, unsigned char *pos, unsigned char com,
                  DsCb cb, int retry) {
    DsPkt cmds[5];
    int i;
    int id;

    for (i = 0; i < 5; i++) {
        cmds[i].com = 0;
        cmds[i].pparam = 0;
        cmds[i].cb = 0;
    }
    i = 2;
    cmds[0].com = 9;
    cmds[1].com = 14;
    {
        DsPkt *p = &cmds[1];
        p->param[0] = mode;
        p->pparam = p->param;
    }
    if (func_80080C48(pos) < 0) {
        return 0;
    }
    cmds[i].com = 2;
    ((UI *)cmds[i].param)->v = ((UI *)pos)->v;
    cmds[i].pparam = cmds[i].param;
    i = 3;
    switch (com) {
    case 21:
    case 22:
        cmds[i].com = com;
        cmds[i].cb = cb;
        return push(cmds, i + 1, retry);
    case 3:
    case 6:
    case 27:
        cmds[i].com = com;
        cmds[i].cb = cb;
        return push(cmds, i + 1, retry);
    default:
        return 0;
    }
}
