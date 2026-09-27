typedef struct { unsigned low, high; } UD;
int func_80073244(UD a, UD b)
{
    if (a.high > b.high) goto pos;
    if (a.high < b.high) goto neg;
    if (a.low > b.low) goto pos;
    if (a.low < b.low) goto neg;
    return 0;
pos:
    return 1;
neg:
    return -1;
}
