extern void func_80073C74(int);
extern void func_80072714(void);
extern int func_8007DEB0(void);
extern void func_8007E314(int);
extern void func_8007E4E0(void);
extern void func_8007E3DC(void);
extern void func_8007E470(void);
extern void func_80072724(void);

void func_8007DDD4(int a0)
{
    func_80073C74(0);
    func_80072714();
    if (func_8007DEB0() == 0) {
        a0 = 0;
    }
    func_8007E314(a0);
    func_8007E4E0();
    func_8007E3DC();
    func_8007E470();
    func_80072724();
}
