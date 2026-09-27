extern volatile int *volatile D_8010DB50;
extern volatile int *volatile D_8010DB24;
extern volatile int *volatile D_8010DB30;
extern char D_8010DA0C[];
extern char D_8010DA90[];
extern char D_8010BCFC[];
void func_8010C1EC(char *a0, int a1);
void func_80071A74(char *a0);
void func_8010C0FC(int a0)
{
    register int m asm("$5");
    register int k asm("$2");
    volatile int *p30;
    volatile int *p50;
    register int *raw asm("$3");
    char *q;
    m = a0;
    switch (m) {
    case 0:
        asm volatile("" : "=r"(m) : "0"(m));
        *D_8010DB50 = (int)0x80000000;
        *D_8010DB24 = 0;
        *D_8010DB30 = 0;
        raw = (int *)D_8010DB50;
        *raw = 0x60000000;
        func_8010C1EC(D_8010DA0C, 0x20);
        q = D_8010DA90;
        asm volatile("" : "=r"(q) : "0"(q));
        func_8010C1EC(q, 0x20);
        break;
    case 1:
        k = (int)0x80000000;
        asm volatile("" : "=r"(k) : "0"(k));
        p50 = D_8010DB50;
        *p50 = k;
        *D_8010DB24 = 0;
        *D_8010DB30 = 0;
        p30 = D_8010DB30;
        raw = (int *)D_8010DB50;
        asm volatile("" : "=r"(p30), "=r"(raw) : "0"(p30), "1"(raw));
        k = *p30;
        asm volatile("" : "=r"(k) : "0"(k));
        *raw = 0x60000000;
        break;
    default:
        func_80071A74(D_8010BCFC);
        break;
    }
}
