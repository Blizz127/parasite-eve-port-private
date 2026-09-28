#include "common.h"

/*
 * St_InstallDmaHandler compares the kernel exception handler with the first
 * six words and, when it matches, replaces it with the following six words.
 * These are instruction templates stored in CDROM.OBJ's executable section;
 * they are copied as data and are never entered at these addresses.
 */
/*
 * headclean 2026-09-28: the two 6-word initializers were removed (retail EXE
 * words at 0x8007A1E0 / D_8007A1F8); they come from the split of the user's
 * disc.  Mnemonics, for reference:
 *   St_DmaCompleteCallback: sw $at,0x04($k0); sw $v0,0x08($k0);
 *     sw $v1,0x0C($k0); sw $ra,0x7C($k0); mfc0 $v1,$14; nop
 *   St_DmaPatchedCallback:  sw $at,0x04($k0); sw $v0,0x08($k0);
 *     mfc0 $v0,$13; sw $v1,0x0C($k0); mfc0 $v1,$14; sw $ra,0x7C($k0)
 */
extern const u32 St_DmaCompleteCallback[6];
extern const u32 St_DmaPatchedCallback[6] __asm__("D_8007A1F8");

const u32 St_DmaHandlerTemplatesEnd
    __asm__("D_8007A210") __attribute__((section(".text"))) = 0;
