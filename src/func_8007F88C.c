typedef struct {
    int flag;
    unsigned char id;
    unsigned char data[11];
} Msg7F88C;

extern Msg7F88C D_800A3520;
extern Msg7F88C D_800A3530;
extern void (*D_800B8AB4)(int, unsigned char *);
extern void func_8007E704(int);
extern void func_80080998(unsigned char *, unsigned char *);

void func_8007F88C(unsigned char a0, unsigned char *a1)
{
    Msg7F88C *m;

    if (a0 == 5 && (a1[0] & 0x10)) {
        func_8007E704(5);
    }
    switch (a0) {
    case 1:
    case 5:
        m = &D_800A3520;
        break;
    case 4:
        m = &D_800A3530;
        break;
    default:
        goto tail;
    }
    m->flag = 1;
    m->id = a0;
    func_80080998((unsigned char *)m + 5, a1);
tail:
    if (D_800B8AB4 != 0) {
        D_800B8AB4(a0, a1);
    }
}
