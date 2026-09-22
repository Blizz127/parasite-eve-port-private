extern void func_800724F4(char *, char *, int);

void func_800723A4(void *b, unsigned int n, int size, int (*cmp)())
{
    char *base = (char *)b;
    char *mid;
    char *last;
    unsigned int i;
    unsigned int cnt;

    mid = base + size;
    cnt = 0;
    if (n < 2) {
        return;
    }
    if (n == 2) {
        if (cmp(base, mid) > 0) {
            func_800724F4(base, mid, size);
        }
        return;
    }
    func_800724F4(base, base + (n >> 1) * size, size);
    last = base;
    for (i = 1; i < n; i++) {
        if (cmp(mid, base) < 0) {
            last = last + size;
            cnt++;
            if (mid != last) {
                func_800724F4(mid, last, size);
            }
        }
        mid = mid + size;
    }
    if (last != base) {
        func_800724F4(base, last, size);
    }
    func_800723A4(base, cnt, size, cmp);
    func_800723A4(last + size, n - cnt - 1, size, cmp);
}
