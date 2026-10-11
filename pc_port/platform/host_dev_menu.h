/* Opt-in SDL GameController input for the host-only development menu. */
#ifndef HOST_DEV_MENU_H
#define HOST_DEV_MENU_H
void HostDevMenu_Poll(void);
int HostDevMenu_CapturesInput(void);
#include <stdint.h>
/* Player mode: the controller pad bits (HOST_PAD_*, 1 = held) the game sees
 * while the Back+Start combo is being formed (see host_dev_menu.c). */
uint16_t HostDevMenu_FilterHeld(uint16_t held);
void HostDevMenu_Close(void);
#endif
