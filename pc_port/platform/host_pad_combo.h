/* Select+Start dev-menu combo on the controller pad word (any controller, any backend).
 *
 * Fed with the held HOST_PAD_* bits of all controllers (ORed) and a monotonic
 * millisecond clock, as often as the host polls (the X11 pacer polls several
 * times per frame, so the timing is in milliseconds, not calls).  It returns
 * the bits the game may see and reports menu events:
 *   - Select and Start held together (either order, the second within
 *     HOST_PAD_COMBO_WINDOW_MS of the first) opens the menu once; the game
 *     sees neither button, and nothing until the buttons are released (drain).
 *   - A lone Select or Start is withheld for up to HOST_PAD_COMBO_WINDOW_MS.
 *     Released before that, it is delivered late as a HOST_PAD_COMBO_TAP_MS
 *     press, long enough for the game's once-per-frame pad read; held longer,
 *     it passes through live from then on.
 *   - While the menu is open the game sees nothing; Up/Down/Cross/Circle
 *     edges become menu events once every button has been released after the
 *     open (a held button cannot leak a confirm).
 *   - Directions never hold a drain or the menu's arming: a drifting stick
 *     must not freeze the pad.  A drain also ends after
 *     HOST_PAD_COMBO_DRAIN_MS, so a stuck button cannot freeze it either.
 * Pure: no I/O, no globals, so it is unit-tested step by step. */
#ifndef HOST_PAD_COMBO_H
#define HOST_PAD_COMBO_H
#include <stdint.h>

#define HOST_PAD_COMBO_WINDOW_MS 300u
#define HOST_PAD_COMBO_TAP_MS     60u
#define HOST_PAD_COMBO_DRAIN_MS 2000u

typedef struct {
    uint16_t prev;        /* held bits at the last step */
    int      waiting;     /* a lone Select/Start is being withheld */
    uint64_t wait_since;  /* ms: when it was first withheld */
    int      passthrough; /* the lone button outlived the window: deliver live */
    uint16_t tap;         /* a short lone press being delivered late */
    uint64_t tap_until;   /* ms */
    int      drain;       /* deliver nothing until the buttons are released */
    uint64_t drain_since; /* ms */
    int      armed;       /* menu navigation edges count (buttons released once) */
    int      menu_prev;   /* the menu was open at the last step */
} HostPadCombo;

typedef struct { int open, up, down, ok, back; } HostPadComboEvents;

/* held: HOST_PAD_* bits currently down.  menu_open: the dev menu is open.
 * now_ms: a monotonic clock.  Returns the HOST_PAD_* bits to deliver to the
 * game until the next step. */
uint16_t HostPadCombo_Step(HostPadCombo *c, uint16_t held, int menu_open, uint64_t now_ms,
                           HostPadComboEvents *ev);
#endif
