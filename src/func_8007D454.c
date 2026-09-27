typedef struct {
    unsigned short pad[0xD3];
    unsigned short trans_addr;
    volatile unsigned short data;
    unsigned short spucnt;
    unsigned short trans_mode;
    volatile unsigned short spustat;
} SpuRegs;

extern SpuRegs *D_8009B3FC;
extern unsigned short D_8009B414;
extern char D_80011C4C[];
extern char D_80011C6C[];
extern char D_80011C80[];
extern void func_8007DCAC(void);
extern int func_80071A74(char *, ...);

void func_8007D454(unsigned short *addr, unsigned long size)
{
    unsigned long n;
    unsigned short *src;
    long chunk;
    unsigned short saved;

    n = size;
    src = addr;
    saved = D_8009B3FC->spustat & 0x7FF;
    D_8009B3FC->trans_addr = D_8009B414;
    func_8007DCAC();
    while (n != 0) {
        if (n < 0x41) {
            chunk = n;
        } else {
            chunk = 0x40;
        }
        {
            register SpuRegs *r asm("$4");
            long i;

            i = 0;
            if (chunk > 0) {
                r = D_8009B3FC;
                do {
                    r->data = *src++;
                    i += 2;
                } while (i < chunk);
            }
        }
        {
            register SpuRegs *q asm("$3");
            register unsigned short cur asm("$4");
            register unsigned short nxt asm("$2");

            q = D_8009B3FC;
            cur = q->spucnt;
            nxt = (cur & 0xFFCF) | 0x10;
            q->spucnt = nxt;
            func_8007DCAC();
        }
        {
            register unsigned int c asm("$3");

            c = 0;
            while (D_8009B3FC->spustat & 0x400) {
                if (++c >= 0xF01) {
                    func_80071A74(D_80011C4C, D_80011C6C);
                    break;
                }
            }
        }
        func_8007DCAC();
        n -= chunk;
        func_8007DCAC();
    }
    {
        register SpuRegs *q asm("$2");

        q = D_8009B3FC;
        {
            register unsigned short cur asm("$4");
            register unsigned short nxt asm("$3");

            cur = q->spucnt;
            nxt = cur & 0xFFCF;
            *(volatile unsigned short *)&q->spucnt = nxt;
        }
        {
            register unsigned int c asm("$3");
            register unsigned short st asm("$2");

            c = 0;
            st = q->spustat;
            while ((st & 0x7FF) != saved) {
                if (++c >= 0xF01) {
                    func_80071A74(D_80011C4C, D_80011C80);
                    break;
                }
                q = D_8009B3FC;
                st = q->spustat;
            }
        }
    }
}
