/* Controller input for the host-only development menu (any controller: the
 * merged SDL GameController + Linux joystick pad word from host_pad.c). */
#ifndef HOST_DEV_MENU_H
#define HOST_DEV_MENU_H
#include <stdint.h>
/* Once per host poll, after HostPad_Poll(). */
void HostDevMenu_Poll(void);
/* The same step with an explicit controller word and clock (tests). */
void HostDevMenu_Step(uint16_t held, uint64_t now_ms);
/* The menu is open or still draining: the game sees no controller input. */
int HostDevMenu_CapturesInput(void);
/* The controller bits the game may see this poll (held, with a lone
 * Select/Start withheld while it could still become the menu combo). */
uint16_t HostDevMenu_FilterPad(uint16_t held);
void HostDevMenu_Close(void);
#endif
