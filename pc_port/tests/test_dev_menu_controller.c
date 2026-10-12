/* Hardware-free dev-menu controller regression.  The menu reads the merged
 * controller word (HostPad_Held: SDL GameController and Linux joystick pads),
 * so the test steps HostDevMenu_Step with synthetic words and a millisecond
 * clock.  The fixed bug: with the menu on, a lone Start or Select used to be
 * reserved for the combo and never reached the game. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "host_dev_menu.h"
#include "host_pad.h"
#include "pe_plat/cheats.h"
#include "pe_guest_ram.h"
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "FAIL line %d: %s\n", __LINE__, #x); exit(1); } checks++; } while (0)
static int checks;
static uint64_t now_ms = 1000;
/* HostDevMenu_Poll's source in the game; the test steps directly. */
uint16_t HostPad_Held(void) { return 0; }
static PePlatCheatState state(void) { PePlatCheatState s; pe_plat_cheats_get_state(&s); return s; }
/* One host poll 16 ms after the last: the bits the game would read. */
static uint16_t step(uint16_t held) { now_ms += 16; HostDevMenu_Step(held, now_ms); return HostDevMenu_FilterPad(held); }
static void tap(uint16_t b) { step(b); step(0); }
static void combo(void) { step(HOST_PAD_SELECT); step(HOST_PAD_SELECT | HOST_PAD_START); step(HOST_PAD_START); step(0); }
int main(int argc, char **argv)
{
    unsetenv("PE_DEV_MENU");
    if (argc > 1 && strcmp(argv[1], "--disabled") == 0) {
        setenv("PE_CHEATS", "0", 1); setenv("PE_DEV_MENU", "1", 1);
        combo(); CHECK(!state().menu);
        CHECK(pe_plat_cheats_key(PE_CHEAT_KEY_DEV_MENU, 1) == 0);
        CHECK(!HostDevMenu_CapturesInput());
        CHECK(step(HOST_PAD_START) == HOST_PAD_START); /* hard-disabled: nothing withheld */
        printf("DEV_MENU hard-disable PASS checks=%d\n", checks); return 0;
    }
    unsetenv("PE_CHEATS");
    PE_RamInit();
    unsigned char *before = malloc(PE_RAM_SIZE); CHECK(before != NULL);
    PE_StoreU32(0x80140000u, 0x12345678u);
    memcpy(before, PE_TranslateConst(PE_RAM_BASE, PE_RAM_SIZE), PE_RAM_SIZE);
    /* Menu off: the combo is inert and Start/Select pass straight through. */
    CHECK(step(HOST_PAD_START) == HOST_PAD_START); step(0);
    combo(); CHECK(!state().menu); CHECK(!HostDevMenu_CapturesInput());
    setenv("PE_DEV_MENU", "1", 1);
    /* A lone Start reaches the game: withheld for the combo window, then live. */
    CHECK(step(HOST_PAD_START) == 0);
    for (int i = 0; i < 30; i++) (void)step(HOST_PAD_START);       /* held ~0.5 s */
    CHECK(step(HOST_PAD_START) == HOST_PAD_START);
    CHECK(!HostDevMenu_CapturesInput()); CHECK(!state().menu);
    CHECK(step(0) == 0);
    /* A short Select tap is delivered late, for several polls. */
    CHECK(step(HOST_PAD_SELECT) == 0);
    CHECK(step(0) == HOST_PAD_SELECT);
    CHECK(step(0) == HOST_PAD_SELECT);
    for (int i = 0; i < 4; i++) (void)step(0);
    CHECK(step(0) == 0);
    /* Select+Start opens the menu; neither button reaches the game. */
    CHECK(step(HOST_PAD_SELECT) == 0);
    CHECK(step(HOST_PAD_SELECT | HOST_PAD_START) == 0); CHECK(state().menu); CHECK(HostDevMenu_CapturesInput());
    pe_plat_cheats_frame(); pe_plat_cheats_frame();
    CHECK(memcmp(before, PE_TranslateConst(PE_RAM_BASE, PE_RAM_SIZE), PE_RAM_SIZE) == 0);
    free(before); /* opening the menu writes no guest RAM */
    (void)step(HOST_PAD_SELECT | HOST_PAD_START); CHECK(state().menu); /* no held repeat */
    (void)step(HOST_PAD_START); (void)step(0);
    (void)step(HOST_PAD_CROSS); CHECK(state().god);                    /* row 0: GOD MODE */
    (void)step(HOST_PAD_CROSS); (void)step(HOST_PAD_CROSS); CHECK(state().god); /* held: no repeat */
    (void)step(0);
    tap(HOST_PAD_DOWN); tap(HOST_PAD_CROSS); CHECK(state().ammo);        /* row 1 */
    tap(HOST_PAD_UP); tap(HOST_PAD_CROSS); CHECK(!state().god);           /* back on row 0 */
    (void)step(HOST_PAD_CIRCLE); CHECK(!state().menu); CHECK(HostDevMenu_CapturesInput()); /* root: closes */
    CHECK(step(HOST_PAD_CIRCLE) == 0);                                  /* the closing B drains */
    (void)step(0); CHECK(!HostDevMenu_CapturesInput());
    CHECK(step(HOST_PAD_CROSS) == HOST_PAD_CROSS); (void)step(0);       /* live again */
    /* WARP BY DAY: Day 5 lists nothing to pick, Day 2 lists its checkpoints. */
    combo(); CHECK(state().menu);
    for (int i = 0; i < 5; i++) tap(HOST_PAD_DOWN);
    tap(HOST_PAD_CROSS);                                                /* WARP BY DAY */
    for (int i = 0; i < 4; i++) tap(HOST_PAD_DOWN);
    tap(HOST_PAD_CROSS);                                                /* DAY 5 */
    tap(HOST_PAD_CROSS); CHECK(state().menu);                           /* empty: nothing picked */
    tap(HOST_PAD_CIRCLE);                                               /* back to the days */
    for (int i = 0; i < 3; i++) tap(HOST_PAD_UP);
    tap(HOST_PAD_CROSS);                                                /* DAY 2 */
    tap(HOST_PAD_CROSS); CHECK(!state().menu);                          /* DAY 2 START picked */
    (void)step(0); CHECK(!HostDevMenu_CapturesInput());
    /* A drifting stick does not hold the drain after the menu closes. */
    combo(); CHECK(state().menu);
    (void)step(HOST_PAD_CIRCLE); CHECK(!state().menu);
    CHECK(step(HOST_PAD_LEFT) == HOST_PAD_LEFT); CHECK(!HostDevMenu_CapturesInput());
    (void)step(0);
    /* Keyboard Escape still closes a menu opened from the keyboard. */
    pe_plat_cheats_key(PE_CHEAT_KEY_DEV_MENU, 1); (void)step(0); CHECK(state().menu);
    pe_plat_cheats_key(PE_CHEAT_KEY_BACK, 1); (void)step(0); (void)step(0); CHECK(!HostDevMenu_CapturesInput());
    HostDevMenu_Close(); PE_RamDestroy();
    printf("DEV_MENU controller combo/navigation PASS checks=%d\n", checks);
    return 0;
}
