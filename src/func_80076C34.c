typedef struct {
    int (*func)();
    int a1;
    int a2;
    int buf[21];
} GpuQ;

typedef struct {
    unsigned char b0;
    unsigned char b1;
    unsigned char pad[6];
    int flag;
    void (*func)();
} GpuCb;

extern volatile unsigned int *D_80095854;
extern volatile unsigned int *D_80095860;
extern volatile int D_80095874;
extern volatile int D_80095878;
extern int D_8009587C;
extern GpuCb D_8009574C;
extern GpuQ D_800BD030[64];
extern void func_800773D0(void);
extern int func_80077404(void);
extern int func_80076EE4(void);
extern int func_80073E10(int);
extern void func_80073CF4(int, int (*)());

int func_80076C34(int (*func)(), int *a1, int n, int a3)
{
    GpuCb *cb;
    volatile unsigned int *p;
    int i;

    func_800773D0();
    while (((D_80095874 + 1) & 0x3F) == D_80095878) {
        if (func_80077404() != 0) {
            return -1;
        }
        func_80076EE4();
    }
    D_8009587C = func_80073E10(0);
    cb = &D_8009574C;
    cb->flag = 1;
    if (cb->b1 == 0 || (D_80095874 == D_80095878 && !(*D_80095860 & 0x1000000) && cb->func == 0)) {
        p = D_80095854;
        do {
        } while (!(*p & 0x4000000));
        func(a1, a3);
        func_80073E10(D_8009587C);
        return 0;
    }
    func_80073CF4(2, func_80076EE4);
    if (n != 0) {
        {
            volatile int *b;
            int *src;

            i = 0;
            b = (volatile int *)D_800BD030[0].buf;
            src = a1;
            while (i < n / 4) {
                int x = *src++;

                *(volatile int *)((char *)b + D_80095874 * sizeof(GpuQ) + i++ * 4) = x;
            }
        }
        D_800BD030[D_80095874].a1 = (int)D_800BD030[D_80095874].buf;
    } else {
        D_800BD030[D_80095874].a1 = (int)a1;
    }
    *(int *)((char *)D_800BD030 + D_80095874 * sizeof(GpuQ) + 8) = a3;
    *(int (**)())((char *)D_800BD030 + D_80095874 * sizeof(GpuQ)) = func;
    D_80095874 = (D_80095874 + 1) & 0x3F;
    func_80073E10(D_8009587C);
    func_80076EE4();
    return (D_80095874 - D_80095878) & 0x3F;
}
