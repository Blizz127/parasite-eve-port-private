extern volatile int *volatile D_8010DB50;
extern volatile int *volatile D_8010DB24;
extern volatile int *volatile D_8010DB30;
extern char D_8010BD38[];
void func_80071A74(char *a0, int a1);

void func_8010C448(int a0)
{
    register int v1 asm("$3");
    register int *raw asm("$4");
    int z;

    func_80071A74(D_8010BD38, a0);
    *D_8010DB50 = (int)0x80000000;
    *D_8010DB24 = 0;
    *D_8010DB30 = 0;
    z = 0;
    asm volatile("" : "=r"(z) : "0"(z));
    v1 = (int)D_8010DB30;
    raw = (int *)D_8010DB50;
    v1 = *(int *)v1;
    asm volatile("" : "=r"(v1) : "0"(v1));
    v1 = 0x60000000;
    *raw = v1;
}
