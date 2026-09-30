/* Host-only menu controls. No SDL video/audio initialization or guest writes.
 * SDL is loaded only for PE_DEV_MENU=1; Linux joystick gameplay stays intact.
 */
#include "host_dev_menu.h"
#include "pe_plat/cheats.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef PE_HAVE_SDL2
#include <SDL.h>
#include <dlfcn.h>

#define MENU_PADS 8
#define MENU_UP 1u
#define MENU_DOWN 2u
#define MENU_OK 4u
#define MENU_BACK 8u
#define MENU_COMBO 16u
#define MENU_RESERVED 32u
#define MENU_DEADZONE 12000

/* Use the SDL headers for ABI/types, resolving the library at runtime. */
#define SDL_API_LIST(X) \
 X(SDL_InitSubSystem) X(SDL_QuitSubSystem) X(SDL_GetError) \
 X(SDL_PumpEvents) X(SDL_FlushEvents) X(SDL_GameControllerUpdate) X(SDL_NumJoysticks) \
 X(SDL_IsGameController) X(SDL_JoystickGetDeviceInstanceID) \
 X(SDL_GameControllerOpen) X(SDL_GameControllerClose) \
 X(SDL_GameControllerGetAttached) X(SDL_GameControllerGetButton) \
 X(SDL_GameControllerGetAxis)
#define DECLARE(name) static __typeof__(&name) p_##name;
SDL_API_LIST(DECLARE)
#undef DECLARE
static void *library;
static int attempted, initialized, capture, drain;
static struct {
    SDL_GameController *controller;
    SDL_JoystickID id;
    unsigned prev;
    int armed;
} pads[MENU_PADS];

static int start_sdl(void)
{
    if (attempted) return initialized;
    attempted = 1;
    library = dlopen("libSDL2-2.0.so.0", RTLD_NOW | RTLD_LOCAL);
    if (!library) library = dlopen("libSDL2.so", RTLD_NOW | RTLD_LOCAL);
    if (!library) {
        fprintf(stderr, "[DEV_MENU] SDL2 unavailable: %s\n", dlerror());
        return 0;
    }
#define RESOLVE(name) do { \
    *(void **)(&p_##name) = dlsym(library, #name); \
    if (!p_##name) { fprintf(stderr, "[DEV_MENU] SDL2 missing %s\n", #name); goto failed; } \
} while (0);
    SDL_API_LIST(RESOLVE)
#undef RESOLVE
    if (p_SDL_InitSubSystem(SDL_INIT_GAMECONTROLLER) != 0) {
        fprintf(stderr, "[DEV_MENU] SDL2 controller init failed: %s\n", p_SDL_GetError());
        goto failed;
    }
    initialized = 1;
    fprintf(stderr, "[DEV_MENU] SDL GameController enabled; Back+Start opens; A confirms; B returns\n");
    return 1;
failed:
    dlclose(library); library = NULL;
    return 0;
}

static unsigned held(SDL_GameController *c)
{
    unsigned bits = 0;
    int y = p_SDL_GameControllerGetAxis(c, SDL_CONTROLLER_AXIS_LEFTY);
    int back = p_SDL_GameControllerGetButton(c, SDL_CONTROLLER_BUTTON_BACK);
    int start = p_SDL_GameControllerGetButton(c, SDL_CONTROLLER_BUTTON_START);
    if (p_SDL_GameControllerGetButton(c, SDL_CONTROLLER_BUTTON_DPAD_UP) || y < -MENU_DEADZONE) bits |= MENU_UP;
    if (p_SDL_GameControllerGetButton(c, SDL_CONTROLLER_BUTTON_DPAD_DOWN) || y > MENU_DEADZONE) bits |= MENU_DOWN;
    if (p_SDL_GameControllerGetButton(c, SDL_CONTROLLER_BUTTON_A)) bits |= MENU_OK;
    if (p_SDL_GameControllerGetButton(c, SDL_CONTROLLER_BUTTON_B)) bits |= MENU_BACK;
    if (back && start) bits |= MENU_COMBO;
    if (back || start) bits |= MENU_RESERVED;
    for (int b = 0; b < SDL_CONTROLLER_BUTTON_MAX; b++)
        if (p_SDL_GameControllerGetButton(c, (SDL_GameControllerButton)b)) bits |= 64u;
    for (int a = 0; a < SDL_CONTROLLER_AXIS_MAX; a++) {
        int value = p_SDL_GameControllerGetAxis(c, (SDL_GameControllerAxis)a);
        if (value > MENU_DEADZONE || (a < SDL_CONTROLLER_AXIS_TRIGGERLEFT && value < -MENU_DEADZONE)) bits |= 64u;
    }
    return bits;
}

void HostDevMenu_Poll(void)
{
    unsigned all = 0, presses = 0;
    int was_open;
    pe_plat_cheats_init();
    if (!pe_plat_cheats_dev_menu_enabled()) {
        if (initialized) HostDevMenu_Close();
        return;
    }
    if (!start_sdl()) return;
    was_open = pe_plat_cheats_menu_open();
    p_SDL_PumpEvents();
    p_SDL_GameControllerUpdate();
    /* We sample controller state, so discard its queued events each poll. */
    p_SDL_FlushEvents(SDL_JOYAXISMOTION, SDL_CONTROLLERSENSORUPDATE);
    for (int i = 0; i < MENU_PADS; i++) {
        if (pads[i].controller && !p_SDL_GameControllerGetAttached(pads[i].controller)) {
            p_SDL_GameControllerClose(pads[i].controller);
            memset(&pads[i], 0, sizeof pads[i]);
        }
    }
    /* SDL enumerates mapped controllers, including additions after startup. */
    for (int device = 0; device < p_SDL_NumJoysticks(); device++) {
        SDL_JoystickID id = p_SDL_JoystickGetDeviceInstanceID(device);
        int free_slot = -1, found = 0;
        if (id < 0 || !p_SDL_IsGameController(device)) continue;
        for (int i = 0; i < MENU_PADS; i++) {
            if (pads[i].controller && pads[i].id == id) found = 1;
            if (!pads[i].controller && free_slot < 0) free_slot = i;
        }
        if (!found && free_slot >= 0) {
            SDL_GameController *c = p_SDL_GameControllerOpen(device);
            if (c) { pads[free_slot].controller = c; pads[free_slot].id = id; }
        }
    }
    for (int i = 0; i < MENU_PADS; i++) {
        unsigned bits;
        if (!pads[i].controller) continue;
        bits = held(pads[i].controller);
        all |= bits;
        /* Reconnected held buttons cannot open, confirm, or repeat actions. */
        if (!bits) pads[i].armed = 1;
        if (pads[i].armed) presses |= bits & ~pads[i].prev;
        pads[i].prev = bits;
    }
    if (presses & MENU_COMBO) {
        pe_plat_cheats_key(PE_CHEAT_KEY_DEV_MENU, 1);
    } else if (was_open) {
        if (presses & MENU_BACK) pe_plat_cheats_key(PE_CHEAT_KEY_BACK, 1);
        else if (presses & MENU_OK) pe_plat_cheats_key(PE_CHEAT_KEY_ENTER, 1);
        else if ((all & (MENU_UP | MENU_DOWN)) == MENU_UP && (presses & MENU_UP)) pe_plat_cheats_key(PE_CHEAT_KEY_UP, 1);
        else if ((all & (MENU_UP | MENU_DOWN)) == MENU_DOWN && (presses & MENU_DOWN)) pe_plat_cheats_key(PE_CHEAT_KEY_DOWN, 1);
    }
    /* Reserve Back/Start before the combo completes: neither opens retail
     * pause/select. Drain held inputs after closing so A/B cannot leak. */
    if (was_open || pe_plat_cheats_menu_open() || (all & MENU_RESERVED)) drain = 1;
    if (!all && !pe_plat_cheats_menu_open()) drain = 0;
    capture = pe_plat_cheats_menu_open() || drain;
}

int HostDevMenu_CapturesInput(void) { return capture; }
void HostDevMenu_Close(void)
{
    if (initialized) {
        for (int i = 0; i < MENU_PADS; i++)
            if (pads[i].controller) p_SDL_GameControllerClose(pads[i].controller);
        p_SDL_QuitSubSystem(SDL_INIT_GAMECONTROLLER);
    }
    if (library) dlclose(library);
    memset(pads, 0, sizeof pads);
    library = NULL; attempted = initialized = capture = drain = 0;
}
#else
void HostDevMenu_Poll(void)
{
    static int warned;
    pe_plat_cheats_init();
    if (pe_plat_cheats_dev_menu_enabled() && !warned) {
        fprintf(stderr, "[DEV_MENU] SDL2 support missing in this build; install SDL2 development files and rebuild\n");
        warned = 1;
    }
}
int HostDevMenu_CapturesInput(void) { return 0; }
void HostDevMenu_Close(void) {}
#endif
