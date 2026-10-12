/* Select+Start dev-menu combo (platform/host_pad_combo.c).  A lone Select or
 * Start reaches the game (late if short, live if held past the window); the
 * pair opens the menu and the game never sees it; the open menu swallows input
 * and drains after closing; a drifting stick or a stuck button cannot freeze
 * the pad.  Steps carry explicit millisecond times: the X11 pacer polls
 * several times per frame, so the window and the late tap are timed, not
 * counted in calls. */
static uint16_t padcombo_step(HostPadCombo *c, uint16_t held, int open, uint64_t t, HostPadComboEvents *ev)
{
    return HostPadCombo_Step(c, held, open, t, ev);
}

static void test_PADCOMBO_lone_buttons(void)
{
    HostPadCombo c;
    HostPadComboEvents ev;
    TEST("pad_combo_lone_buttons");
    memset(&c, 0, sizeof c);
    ASSERT(padcombo_step(&c, HOST_PAD_START, 0, 1000, &ev) == 0 && !ev.open, "a lone Start is withheld first");
    ASSERT(padcombo_step(&c, HOST_PAD_START | HOST_PAD_CROSS, 0, 1100, &ev) == HOST_PAD_CROSS,
           "other buttons stay live while Start is withheld");
    ASSERT(padcombo_step(&c, HOST_PAD_START, 0, 1000 + HOST_PAD_COMBO_WINDOW_MS - 1, &ev) == 0,
           "still withheld inside the window");
    ASSERT(padcombo_step(&c, HOST_PAD_START, 0, 1000 + HOST_PAD_COMBO_WINDOW_MS, &ev) == HOST_PAD_START,
           "held past the window: delivered live");
    ASSERT(padcombo_step(&c, HOST_PAD_START, 0, 1500, &ev) == HOST_PAD_START, "stays live while held");
    ASSERT(padcombo_step(&c, 0, 0, 1510, &ev) == 0, "released after going live: no extra late press");
    /* a short tap: delivered late, for HOST_PAD_COMBO_TAP_MS of polls */
    ASSERT(padcombo_step(&c, HOST_PAD_SELECT, 0, 2000, &ev) == 0, "short Select withheld");
    ASSERT(padcombo_step(&c, 0, 0, 2050, &ev) == HOST_PAD_SELECT, "short Select delivered on release");
    for (int i = 0; i < 20; i++)
        ASSERT(padcombo_step(&c, 0, 0, 2051, &ev) == HOST_PAD_SELECT, "many polls in one frame keep the tap");
    ASSERT(padcombo_step(&c, 0, 0, 2050 + HOST_PAD_COMBO_TAP_MS - 1, &ev) == HOST_PAD_SELECT, "tap lasts its time");
    ASSERT(padcombo_step(&c, 0, 0, 2050 + HOST_PAD_COMBO_TAP_MS, &ev) == 0, "tap ends");
    PASS();
}

static void test_PADCOMBO_opens_menu(void)
{
    HostPadCombo c;
    HostPadComboEvents ev;
    uint16_t seen = 0;
    TEST("pad_combo_opens_menu");
    memset(&c, 0, sizeof c);
    seen |= padcombo_step(&c, HOST_PAD_SELECT, 0, 100, &ev);
    ASSERT(!ev.open, "Select alone does not open");
    seen |= padcombo_step(&c, HOST_PAD_SELECT | HOST_PAD_START, 0, 150, &ev);
    ASSERT(ev.open, "Select then Start inside the window opens the menu");
    seen |= padcombo_step(&c, HOST_PAD_SELECT | HOST_PAD_START, 0, 200, &ev);
    ASSERT(!ev.open, "held combo does not repeat");
    seen |= padcombo_step(&c, HOST_PAD_START, 0, 250, &ev);
    seen |= padcombo_step(&c, 0, 0, 300, &ev);
    seen |= padcombo_step(&c, 0, 0, 400, &ev);
    ASSERT(!(seen & (HOST_PAD_SELECT | HOST_PAD_START)), "the game never sees the combo buttons");
    ASSERT(padcombo_step(&c, HOST_PAD_CROSS, 0, 500, &ev) == HOST_PAD_CROSS, "live again after the release");
    PASS();
}

static void test_PADCOMBO_menu_navigation(void)
{
    HostPadCombo c;
    HostPadComboEvents ev;
    TEST("pad_combo_menu_navigation");
    memset(&c, 0, sizeof c);
    (void)padcombo_step(&c, HOST_PAD_SELECT | HOST_PAD_START, 0, 0, &ev);
    ASSERT(ev.open, "combo opens");
    ASSERT(padcombo_step(&c, HOST_PAD_SELECT | HOST_PAD_START | HOST_PAD_CROSS, 1, 10, &ev) == 0 && !ev.ok,
           "buttons held since the open do not navigate");
    (void)padcombo_step(&c, 0, 1, 20, &ev);
    (void)padcombo_step(&c, HOST_PAD_CROSS, 1, 30, &ev);
    ASSERT(ev.ok, "Cross confirms once armed");
    (void)padcombo_step(&c, HOST_PAD_CROSS, 1, 40, &ev);
    ASSERT(!ev.ok, "a held Cross does not repeat");
    (void)padcombo_step(&c, 0, 1, 50, &ev);
    (void)padcombo_step(&c, HOST_PAD_DOWN, 1, 60, &ev);
    ASSERT(ev.down && !ev.up, "Down moves");
    (void)padcombo_step(&c, HOST_PAD_DOWN | HOST_PAD_UP, 1, 70, &ev);
    ASSERT(!ev.up, "Up with Down held is ignored");
    (void)padcombo_step(&c, 0, 1, 80, &ev);
    (void)padcombo_step(&c, HOST_PAD_CIRCLE, 1, 90, &ev);
    ASSERT(ev.back, "Circle returns");
    ASSERT(padcombo_step(&c, HOST_PAD_CIRCLE, 0, 100, &ev) == 0, "closing Circle drains, not delivered");
    ASSERT(padcombo_step(&c, 0, 0, 110, &ev) == 0 && !c.drain, "drain ends on release");
    ASSERT(padcombo_step(&c, HOST_PAD_CROSS, 0, 120, &ev) == HOST_PAD_CROSS, "live after the drain");
    PASS();
}

static void test_PADCOMBO_drift_and_stuck(void)
{
    HostPadCombo c;
    HostPadComboEvents ev;
    TEST("pad_combo_drift_and_stuck");
    memset(&c, 0, sizeof c);
    (void)padcombo_step(&c, HOST_PAD_SELECT | HOST_PAD_START | HOST_PAD_LEFT, 0, 0, &ev);
    ASSERT(ev.open, "combo with the stick off-centre opens");
    ASSERT(padcombo_step(&c, HOST_PAD_LEFT, 0, 10, &ev) == HOST_PAD_LEFT && !c.drain,
           "a drifting stick does not hold the drain");
    memset(&c, 0, sizeof c);
    (void)padcombo_step(&c, HOST_PAD_SELECT | HOST_PAD_START, 0, 0, &ev);
    (void)padcombo_step(&c, HOST_PAD_RIGHT, 1, 10, &ev);
    (void)padcombo_step(&c, HOST_PAD_RIGHT | HOST_PAD_CROSS, 1, 20, &ev);
    ASSERT(ev.ok, "the menu arms with a drifting stick");
    memset(&c, 0, sizeof c);
    (void)padcombo_step(&c, HOST_PAD_SELECT | HOST_PAD_START, 0, 0, &ev);
    ASSERT(padcombo_step(&c, HOST_PAD_SQUARE, 0, 10, &ev) == 0, "a held button drains");
    ASSERT(padcombo_step(&c, HOST_PAD_SQUARE, 0, HOST_PAD_COMBO_DRAIN_MS - 1, &ev) == 0, "still draining");
    (void)padcombo_step(&c, HOST_PAD_SQUARE, 0, HOST_PAD_COMBO_DRAIN_MS, &ev);
    ASSERT(!c.drain, "a stuck button ends the drain after HOST_PAD_COMBO_DRAIN_MS");
    ASSERT(padcombo_step(&c, HOST_PAD_SQUARE, 0, HOST_PAD_COMBO_DRAIN_MS + 1, &ev) == HOST_PAD_SQUARE,
           "and goes live");
    PASS();
}

static void test_PADCOMBO_all(void)
{
    test_PADCOMBO_lone_buttons();
    test_PADCOMBO_opens_menu();
    test_PADCOMBO_menu_navigation();
    test_PADCOMBO_drift_and_stuck();
}
