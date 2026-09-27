/* ovl_039F — func_8010C1EC, blob offset 0x4F4, 0x90 bytes. Profile era_o2_g0_fill_epilogue_delay_slot.
 * LINK_EXACT at the target VMA (lane ovl3 2026-09-27). */

extern volatile unsigned int *D_8010DB54;
extern volatile unsigned int *D_8010DB1C;
extern volatile unsigned int *D_8010DB20;
extern volatile unsigned int *D_8010DB4C;
extern volatile unsigned int *D_8010DB24;
extern int func_8010C308();

void func_8010C1EC(unsigned int *a0, unsigned int a1)
{
    func_8010C308();
    *D_8010DB54 |= 0x88;
    *D_8010DB1C = (unsigned int)(a0 + 1);
    *D_8010DB20 = ((a1 >> 5) << 16) | 0x20;
    *D_8010DB4C = *a0;
    *D_8010DB24 = 0x1000201;
}
