extern int D_8009B6EC;
extern unsigned char func_8008227C[];
void func_800824C8(int a);
int func_8007FC28(void);
int func_80080C48(int a);
int func_8007FC18(void);
int func_8007FC44(void);
void func_8007F0C8(int a, int b, int c, void *d, int e);

void func_80082204(void) {
    int a;
    int b;
    int c;

    func_800824C8(0);
    D_8009B6EC = func_80080C48(func_8007FC28());
    a = func_8007FC18() & 0xFF;
    b = func_8007FC28();
    c = func_8007FC44() & 0xFF;
    func_8007F0C8(a, b, c, func_8008227C, -1);
}
