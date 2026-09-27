/* ovl_039F — func_8010C27C, blob offset 0x584, 0x8C bytes. Profile era_o2_g0_fill_epilogue_delay_slot.
 * LINK_EXACT at the target VMA (lane ovl3 2026-09-27). */

extern volatile unsigned int *D_8010DB54;
extern volatile unsigned int *D_8010DB30;
extern volatile unsigned int *D_8010DB28;
extern volatile unsigned int *D_8010DB2C;
extern int func_8010C39C();

void func_8010C27C(unsigned int a0, unsigned int a1)
{
    func_8010C39C();
    *D_8010DB54 |= 0x88;
    *D_8010DB30 = 0;
    *D_8010DB28 = a0;
    *D_8010DB2C = ((a1 >> 5) << 16) | 0x20;
    *D_8010DB30 = 0x1000200;
}
