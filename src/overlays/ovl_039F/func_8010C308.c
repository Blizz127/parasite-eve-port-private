/* ovl_039F — func_8010C308, blob offset 0x610, 0x94 bytes. Profile era_o2_g0_fill_call_delay_slot.
 * LINK_EXACT at the target VMA (lane ovl3 2026-09-27). */

extern volatile unsigned int *D_8010DB50;
extern char D_8010BD18[];
extern void func_8010C448();

int func_8010C308(void)
{
    volatile int cnt = 0x100000;

    while (*D_8010DB50 & 0x20000000) {
        if (--cnt == -1) {
            func_8010C448(D_8010BD18);
            return -1;
        }
    }
    return 0;
}
