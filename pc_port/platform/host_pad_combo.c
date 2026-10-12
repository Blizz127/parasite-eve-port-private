/* Select+Start dev-menu combo state machine.  See host_pad_combo.h. */
#include "host_pad_combo.h"
#include "host_pad.h"
#include <string.h>

#define SS   (HOST_PAD_SELECT | HOST_PAD_START)
#define DIRS (HOST_PAD_UP | HOST_PAD_DOWN | HOST_PAD_LEFT | HOST_PAD_RIGHT)

uint16_t HostPadCombo_Step(HostPadCombo *c, uint16_t held, int menu_open, uint64_t now_ms,
                           HostPadComboEvents *ev)
{
    uint16_t ss = held & SS, out = held & (uint16_t)~SS, presses = held & (uint16_t)~c->prev;
    uint16_t buttons = held & (uint16_t)~DIRS;
    memset(ev, 0, sizeof *ev);
    if (menu_open) {
        if (!c->menu_prev) c->armed = 0;            /* just opened: wait for a release */
        if (!buttons) c->armed = 1;
        if (c->armed) {
            ev->up   = (presses & HOST_PAD_UP)   && !(held & HOST_PAD_DOWN);
            ev->down = (presses & HOST_PAD_DOWN) && !(held & HOST_PAD_UP);
            ev->ok   = (presses & HOST_PAD_CROSS) != 0;
            ev->back = (presses & HOST_PAD_CIRCLE) != 0;
        }
        c->drain = 1; c->drain_since = now_ms;
        c->waiting = c->passthrough = 0; c->tap = 0;
        c->prev = held; c->menu_prev = 1;
        return 0;                                    /* the game sees nothing while open */
    }
    if (c->menu_prev) { c->menu_prev = 0; c->drain = 1; c->drain_since = now_ms; }
    if (c->drain) {                                  /* after the combo / the menu: release first */
        if (!buttons || now_ms - c->drain_since >= HOST_PAD_COMBO_DRAIN_MS) c->drain = 0;
        else { c->prev = held; return 0; }
        c->prev = held;
        return held & DIRS;                          /* drain over: directions go live at once */
    }
    if (ss == SS) {                                  /* both down: open, swallow, drain */
        ev->open = 1;
        c->drain = 1; c->drain_since = now_ms;
        c->waiting = c->passthrough = 0; c->tap = 0;
        c->prev = held;
        return 0;
    }
    if (ss) {                                        /* a lone Select or Start */
        if (!c->waiting) { c->waiting = 1; c->wait_since = now_ms; }
        if (!c->passthrough && now_ms - c->wait_since >= HOST_PAD_COMBO_WINDOW_MS) c->passthrough = 1;
        if (c->passthrough) out |= ss;
    } else {
        if (c->waiting && !c->passthrough) {         /* short press: deliver it late */
            c->tap = c->prev & SS;
            c->tap_until = now_ms + HOST_PAD_COMBO_TAP_MS;
        }
        c->waiting = c->passthrough = 0;
    }
    if (c->tap) {
        if (now_ms < c->tap_until) out |= c->tap;
        else c->tap = 0;
    }
    c->prev = held;
    return out;
}
