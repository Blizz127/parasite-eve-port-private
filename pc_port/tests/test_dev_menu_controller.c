/* Hardware-free SDL virtual-device regression; production uses GameController.
 * The Joystick virtual API here injects inputs into SDL's controller mapping. */
#include <SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "host_dev_menu.h"
#include "pe_plat/cheats.h"
#include "pe_guest_ram.h"
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "FAIL line %d: %s (%s)\n", __LINE__, #x, SDL_GetError()); exit(1); } checks++; } while (0)
static int checks;
static PePlatCheatState state(void) { PePlatCheatState s; pe_plat_cheats_get_state(&s); return s; }
static void poll(void) { HostDevMenu_Poll(); }
static void button(SDL_Joystick *j, int b, int v) { CHECK(SDL_JoystickSetVirtualButton(j, b, v) == 0); poll(); }
static void tap(SDL_Joystick *j, int b) { button(j, b, 1); button(j, b, 0); }
static void combo(SDL_Joystick *j) {
    button(j, SDL_CONTROLLER_BUTTON_BACK, 1);
    button(j, SDL_CONTROLLER_BUTTON_START, 1);
    button(j, SDL_CONTROLLER_BUTTON_START, 0);
    button(j, SDL_CONTROLLER_BUTTON_BACK, 0);
}
static SDL_Joystick *attach(int *index) {
    *index = SDL_JoystickAttachVirtual(SDL_JOYSTICK_TYPE_GAMECONTROLLER, SDL_CONTROLLER_AXIS_MAX, SDL_CONTROLLER_BUTTON_MAX, 0);
    CHECK(*index >= 0); CHECK(SDL_IsGameController(*index));
    SDL_Joystick *j = SDL_JoystickOpen(*index); CHECK(j != NULL); return j;
}
int main(int argc, char **argv)
{
    int index, second;
    unsetenv("PE_DEV_MENU");
    if (argc > 1 && strcmp(argv[1], "--disabled") == 0) {
        setenv("PE_CHEATS", "0", 1); setenv("PE_DEV_MENU", "1", 1);
        poll(); CHECK(SDL_WasInit(SDL_INIT_GAMECONTROLLER) == 0);
        CHECK(pe_plat_cheats_key(PE_CHEAT_KEY_DEV_MENU, 1) == 0);
        CHECK(!HostDevMenu_CapturesInput());
        printf("DEV_MENU hard-disable PASS checks=%d\n", checks); return 0;
    }
    unsetenv("PE_CHEATS");
    PE_RamInit();
    unsigned char *before = malloc(PE_RAM_SIZE); CHECK(before != NULL);
    PE_StoreU32(0x80140000u, 0x12345678u);
    memcpy(before, PE_TranslateConst(PE_RAM_BASE, PE_RAM_SIZE), PE_RAM_SIZE);
    poll(); CHECK(SDL_WasInit(SDL_INIT_GAMECONTROLLER) == 0);
    CHECK(pe_plat_cheats_key(PE_CHEAT_KEY_DEV_MENU, 1) == 0);
    CHECK(!HostDevMenu_CapturesInput()); CHECK(!state().menu && !state().god && !state().ammo);
    CHECK(SDL_InitSubSystem(SDL_INIT_GAMECONTROLLER) == 0);
    SDL_Joystick *j = attach(&index);
    combo(j); CHECK(!state().menu); /* off remains inert with devices */
    setenv("PE_DEV_MENU", "1", 1); poll();
    button(j, SDL_CONTROLLER_BUTTON_BACK, 1);
    CHECK(HostDevMenu_CapturesInput()); CHECK(!state().menu);
    button(j, SDL_CONTROLLER_BUTTON_START, 1); CHECK(state().menu);
    pe_plat_cheats_frame(); pe_plat_cheats_frame();
    CHECK(memcmp(before, PE_TranslateConst(PE_RAM_BASE, PE_RAM_SIZE), PE_RAM_SIZE) == 0);
    free(before); /* opening/render aid with cheats off writes no guest RAM */
    poll(); CHECK(state().menu); /* no held combo repeat */
    button(j, SDL_CONTROLLER_BUTTON_START, 0); button(j, SDL_CONTROLLER_BUTTON_BACK, 0);
    button(j, SDL_CONTROLLER_BUTTON_A, 1); CHECK(state().god);
    poll(); poll(); CHECK(state().god); /* holding A does not toggle again */
    button(j, SDL_CONTROLLER_BUTTON_A, 0);
    tap(j, SDL_CONTROLLER_BUTTON_DPAD_DOWN); tap(j, SDL_CONTROLLER_BUTTON_A); CHECK(state().ammo);
    CHECK(SDL_JoystickSetVirtualAxis(j, SDL_CONTROLLER_AXIS_LEFTY, -20000) == 0); poll();
    CHECK(SDL_JoystickSetVirtualAxis(j, SDL_CONTROLLER_AXIS_LEFTY, 0) == 0); poll();
    tap(j, SDL_CONTROLLER_BUTTON_A); CHECK(!state().god); /* stick returned to first row */
    button(j, SDL_CONTROLLER_BUTTON_B, 1); CHECK(!state().menu); CHECK(HostDevMenu_CapturesInput());
    button(j, SDL_CONTROLLER_BUTTON_B, 0); CHECK(!HostDevMenu_CapturesInput());
    combo(j);
    for (int i = 0; i < 5; i++) tap(j, SDL_CONTROLLER_BUTTON_DPAD_DOWN);
    tap(j, SDL_CONTROLLER_BUTTON_A); /* Day selector */
    tap(j, SDL_CONTROLLER_BUTTON_DPAD_DOWN); tap(j, SDL_CONTROLLER_BUTTON_A); /* empty Day 2 */
    tap(j, SDL_CONTROLLER_BUTTON_A); CHECK(state().menu && state().warps == 0);
    tap(j, SDL_CONTROLLER_BUTTON_B); tap(j, SDL_CONTROLLER_BUTTON_B); /* root */
    tap(j, SDL_CONTROLLER_BUTTON_B); CHECK(!state().menu);
    /* A combo cannot be synthesized across two devices. */
    SDL_Joystick *k = attach(&second); poll();
    button(j, SDL_CONTROLLER_BUTTON_BACK, 1); button(k, SDL_CONTROLLER_BUTTON_START, 1);
    CHECK(!state().menu);
    button(j, SDL_CONTROLLER_BUTTON_BACK, 0); button(k, SDL_CONTROLLER_BUTTON_START, 0);
    SDL_JoystickClose(k); CHECK(SDL_JoystickDetachVirtual(second) == 0); poll();
    /* Disconnect while captured; reconnect with combo held must first release. */
    button(j, SDL_CONTROLLER_BUTTON_BACK, 1); CHECK(HostDevMenu_CapturesInput());
    SDL_JoystickClose(j); CHECK(SDL_JoystickDetachVirtual(index) == 0); poll();
    CHECK(!HostDevMenu_CapturesInput());
    j = attach(&index);
    CHECK(SDL_JoystickSetVirtualButton(j, SDL_CONTROLLER_BUTTON_BACK, 1) == 0);
    CHECK(SDL_JoystickSetVirtualButton(j, SDL_CONTROLLER_BUTTON_START, 1) == 0);
    poll(); CHECK(!state().menu);
    button(j, SDL_CONTROLLER_BUTTON_START, 0); button(j, SDL_CONTROLLER_BUTTON_BACK, 0);
    combo(j); CHECK(state().menu);
    SDL_JoystickClose(j); CHECK(SDL_JoystickDetachVirtual(index) == 0); poll();
    CHECK(state().menu); /* keyboard Escape still available with no controller */
    pe_plat_cheats_key(PE_CHEAT_KEY_BACK, 1); poll(); CHECK(!HostDevMenu_CapturesInput());
    HostDevMenu_Close(); SDL_QuitSubSystem(SDL_INIT_GAMECONTROLLER); PE_RamDestroy();
    printf("DEV_MENU SDL virtual hotplug/navigation PASS checks=%d\n", checks);
    return 0;
}
