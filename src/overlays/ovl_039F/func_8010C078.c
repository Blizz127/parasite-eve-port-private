int func_8010C39C(void);
unsigned int func_8010C430(void);

int func_8010C078(int a0)
{
    unsigned int v;

    if (a0) {
        v = func_8010C430();
        return (v >> 24) & 1;
    }
    return func_8010C39C();
}
