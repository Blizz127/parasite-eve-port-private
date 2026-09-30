/* Opt-in SDL GameController input for the host-only development menu. */
#ifndef HOST_DEV_MENU_H
#define HOST_DEV_MENU_H
void HostDevMenu_Poll(void);
int HostDevMenu_CapturesInput(void);
void HostDevMenu_Close(void);
#endif
