/* Opt-in assists (pe_plat/enhance.h): 1-hit kills, god mode, EXP/BP
 * multiplier.  Hooks: func_80028574 (enemy damage), func_8001F4D4 /
 * func_8001D340 / func_80020288 (player HP), func_80028E94 (EXP/BP award).
 * Uses the ATK17 / ATK18 retail fixtures (test_damage.h, test_defeat.h) and
 * enh_isolate() (test_enhance.h). */

#define AST_ACTOR   0x80141000u
#define AST_BODY    0x80143000u
#define AST_REC     0x80144000u
#define AST_ARMOR   0x80144100u
#define AST_ACTION  0x80145000u
#define AST_AYA     0x80140000u

/* ATK18 case that runs func_80028E94 state 3 (the EXP/BP award) and awards. */
static const int *ast_award_case(void)
{
    unsigned k;
    for (k = 0; k < sizeof(ATK18_defeat_cases) / sizeof(ATK18_defeat_cases[0]); k++) {
        const int *c = ATK18_defeat_cases[k];
        if (c[0] == 0 && c[1] == 3 && c[7] == 0) return c;
    }
    return NULL;
}

/* Aya hit by an enemy attack: retail func_8001F4D4 with no crit, no armor,
 * no status, power 30 -> HP 45 - 30 = 15. */
static void ast_player_hit_fixture(void)
{
    ResetTestState();
    D_8009D1A0 = 2u;
    PE_StoreU32(0x8009D1A0u, 2u);
    PE_StoreU32(0x8009D254u, AST_AYA);
    PE_StoreU32(0x8009D278u, AST_REC);
    PE_StoreU32(AST_ACTOR, AST_BODY);
    PE_StoreU32(AST_BODY + 0x18u, AST_ACTION);
    PE_StoreU8(AST_BODY + 144u, 0u);              /* crit chance 0 */
    PE_StoreU8(AST_BODY + 146u, 7u);              /* contact damage */
    PE_StoreU16(AST_ACTION + 12u, 30u);           /* attack power */
    PE_StoreU8(AST_ACTION + 1u, 0u);              /* no status kind */
    PE_StoreU8(AST_ACTION + 14u, 1u);
    PE_StoreU16(AST_REC + 0x0Cu, 45u);            /* HP */
    PE_StoreU16(AST_REC + 0x0Eu, 45u);
    PE_StoreU16(AST_REC + 0x1Cu, 45u);            /* max HP */
    PE_StoreU16(AST_REC + 0x20u, 0u);             /* defense */
    PE_StoreU32(AST_REC + 0x6Cu, AST_ARMOR);
    PE_StoreU32(AST_ARMOR, 0u);
    PE_StoreU32(AST_ARMOR + 4u, 0u);
}

static void test_ASSIST_defaults_are_retail(void)
{
    static uint8_t before[0x200000];
    const int *c;
    uint64_t hash;
    static const uint32_t r17[][2] = {{0x140000, 0x6000}};
    TEST("ASSIST_defaults_are_retail");
    enh_isolate();
    unsetenv("PE_ONE_HIT_KILLS"); unsetenv("PE_GOD_MODE"); unsetenv("PE_EXP_MULT");
    pe_enh_reset();
    ASSERT(!pe_enh_one_hit_kills() && !pe_enh_god_mode() && pe_enh_exp_mult() == 1,
           "defaults: 1-hit kills off, god mode off, EXP/BP x1");
    ResetTestState();
    PE_StoreU32(AST_BODY + 0x10u, 1000u);
    memcpy(before, PE_TranslateConst(0x80000000u, 0x200000u), sizeof(before));
    for (int32_t d = -5000; d <= 5000; d += 37)
        ASSERT(pe_enh_enemy_damage(AST_BODY, d) == d, "x1/off: enemy damage passes through");
    ASSERT(!pe_enh_player_shielded(), "off: player HP not shielded");
    for (uint32_t v = 0; v < 70000u; v += 1234u)
        ASSERT(pe_enh_exp_award(v) == v && pe_enh_bp_award(v) == v, "x1: EXP and BP pass through");
    ASSERT(memcmp(before, PE_TranslateConst(0x80000000u, 0x200000u), sizeof(before)) == 0,
           "hooks at the defaults write no guest state");
    /* battle state hashes: the retail damage and defeat/results cases hold. */
    atk17_damage_fixture(ATK17_damage_cases[0]);
    func_80028574(AST_ACTOR);
    hash = hit_camera_hash(r17, 1u);
    ASSERT(hash == ATK17_damage_expected[0], "defaults: retail damage state hash (ATK17 case 0)");
    c = ast_award_case();
    ASSERT(c != NULL, "an ATK18 award case exists");
    if (c) {
        unsigned k = (unsigned)(c - &ATK18_defeat_cases[0][0]) / 12u;
        static const uint32_t r18[][2]={{0x140000,0x7000},{0xA5D58,7*220},{0xA7FF0,40},{0xB00E8,0xD4},
            {0xB6920,0x38},{0xBCEA8,224},{0xB8628,72},{0xBCD80,32},{0x9D1A0,4},{0x9D1D0,8},
            {0x9CE3C,1},{0x9D21C,2},{0x9D234,1},{0x9D244,1},{0x9D268,4},{0x9D28C,4},
            {0x9D298,12},{0x9D304,4},{0x9D2E8,4},{0x9D2F4,4},{0xB0CD8,4}};
        atk18_defeat_fixture(c);
        func_80028E94(AST_ACTOR);
        hash = hit_camera_hash(r18, sizeof(r18) / sizeof(r18[0]));
        ASSERT(hash == ATK18_defeat_expected[k], "defaults: retail defeat/results state hash");
        ASSERT(PE_LoadU32(0x8009D304u) == 20u && PE_LoadU16(0x8009D21Cu) == 70u,
               "retail award: EXP 20, BP 70 for the fixture enemy");
    }
    ASSERT(!PE_Port_ShouldStop(), "retail paths execute natively");
    PASS();
}

static void test_ASSIST_one_hit_kills(void)
{
    static const uint32_t r17[][2] = {{0x140000, 0x6000}};
    const int *c;
    uint64_t hash;
    TEST("ASSIST_one_hit_kills");
    enh_isolate();
    pe_enh_set_one_hit_kills(1);
    ASSERT(pe_enh_one_hit_kills() == 1, "1-hit kills on");
    /* ATK17 case 0: enemy HP 1000, retail damage far below it. */
    atk17_damage_fixture(ATK17_damage_cases[0]);
    ASSERT((int32_t)PE_LoadU32(AST_BODY + 0x10u) == 1000, "fixture enemy starts at 1000 HP");
    func_80028574(AST_ACTOR);
    ASSERT((int32_t)PE_LoadU32(AST_BODY + 0x10u) == 0, "1-hit kills: the hit takes the enemy to exactly 0 HP");
    ASSERT((PE_LoadU32(AST_BODY) & 0x6000u) == 0x4000u, "retail hit-state bits still set (normal death path follows)");
    ASSERT(!PE_Port_ShouldStop(), "executes natively");
    /* A retail-lethal hit is not changed: HP 1 -> 0 either way. */
    atk17_damage_fixture(ATK17_damage_cases[0]);
    PE_StoreU32(AST_BODY + 0x10u, 1u);
    func_80028574(AST_ACTOR);
    ASSERT((int32_t)PE_LoadU32(AST_BODY + 0x10u) <= 0, "already-lethal hit still kills");
    /* Story-scripted battles (game flag 0x800, the Eve stage fight) are exempt: retail damage. */
    atk17_damage_fixture(ATK17_damage_cases[0]);
    PE_StoreU32(0x8009D1A0u, PE_LoadU32(0x8009D1A0u) | 0x800u);
    func_80028574(AST_ACTOR);
    ASSERT((int32_t)PE_LoadU32(AST_BODY + 0x10u) > 0 && (int32_t)PE_LoadU32(AST_BODY + 0x10u) < 1000,
           "scripted battle (flag 0x800): retail damage, enemy survives");
    /* Script-invulnerable enemies (HP marked +1000000) are exempt as well. */
    PE_StoreU32(0x8009D1A0u, PE_LoadU32(0x8009D1A0u) & ~0x800u);
    PE_StoreU32(AST_BODY + 0x10u, 1000450u);
    ASSERT(pe_enh_enemy_damage(AST_BODY, 30) == 30, "HP > 1000000 (script-invulnerable): retail damage");
    PE_StoreU32(AST_BODY + 0x10u, 450u);
    ASSERT(pe_enh_enemy_damage(AST_BODY, 30) == 450, "marker cleared: the assist kills again");
    PE_StoreU32(0x8009D1A0u, PE_LoadU32(0x8009D1A0u) | 0x800u);
    /* The kill then awards retail EXP and BP through func_80028E94. */
    c = ast_award_case();
    if (c) {
        atk18_defeat_fixture(c);
        func_80028E94(AST_ACTOR);
        ASSERT(PE_LoadU32(0x8009D304u) == 20u && PE_LoadU16(0x8009D21Cu) == 70u,
               "1-hit kills: EXP 20 and BP 70 awarded as retail (no multiplier)");
    }
    /* Off again: the retail state hash returns. */
    pe_enh_set_one_hit_kills(0);
    atk17_damage_fixture(ATK17_damage_cases[0]);
    func_80028574(AST_ACTOR);
    hash = hit_camera_hash(r17, 1u);
    ASSERT(hash == ATK17_damage_expected[0], "off: retail damage state hash (ATK17 case 0)");
    unlink(enh_conf); pe_enh_reset();
    PASS();
}

static void test_ASSIST_exp_mult(void)
{
    const int *c = ast_award_case();
    int m;
    TEST("ASSIST_exp_mult");
    enh_isolate();
    ASSERT(c != NULL, "an ATK18 award case exists");
    if (!c) { PASS(); return; }
    for (m = 1; m <= PE_ENH_EXP_MULT_MAX; m++) {
        pe_enh_set_exp_mult(m);
        ASSERT(pe_enh_exp_mult() == m, "multiplier set");
        atk18_defeat_fixture(c);
        func_80028E94(AST_ACTOR);
        ASSERT(PE_LoadU32(0x8009D304u) == 20u * (uint32_t)m, "EXP award is an exact multiple");
        ASSERT(PE_LoadU16(0x8009D21Cu) == 70u * (uint32_t)m, "BP award is an exact multiple");
        ASSERT(PE_LoadU16(0x800A7FF0u) == PE_LoadU8(0x800A5D5Cu + 0x9Eu), "item drop unchanged");
        ASSERT(!PE_Port_ShouldStop(), "executes natively");
    }
    pe_enh_set_exp_mult(4); ASSERT(pe_enh_exp_mult() == 3, "clamped to x3");
    pe_enh_set_exp_mult(0); ASSERT(pe_enh_exp_mult() == 1, "clamped to x1");
    pe_enh_cycle_exp_mult(); pe_enh_cycle_exp_mult(); pe_enh_cycle_exp_mult();
    ASSERT(pe_enh_exp_mult() == 1, "cycle 1 -> 2 -> 3 -> 1");
    unlink(enh_conf); pe_enh_reset();
    PASS();
}

static void test_ASSIST_god_mode(void)
{
    TEST("ASSIST_god_mode");
    enh_isolate();
    /* Retail: the hit lands. */
    ast_player_hit_fixture();
    func_8001F4D4(AST_ACTOR);
    ASSERT(PE_LoadU16(AST_REC + 0x0Cu) == 15u, "off: enemy hit takes 45 -> 15 HP");
    ASSERT(!PE_Port_ShouldStop(), "retail hit executes natively");
    /* God mode: same hit, HP unchanged, hit still registers. */
    pe_enh_set_god_mode(1);
    ast_player_hit_fixture();
    func_8001F4D4(AST_ACTOR);
    ASSERT(PE_LoadU16(AST_REC + 0x0Cu) == 45u, "god mode: a scripted hit leaves HP unchanged");
    ASSERT(PE_LoadU32(0x8009D1D0u) == AST_ACTOR, "god mode: the hit reaction still registers the attacker");
    /* Status effects that would cut or zero HP. */
    ast_player_hit_fixture();
    PE_StoreU8(AST_ACTION + 1u, 8u);            /* halve HP */
    func_80020288(AST_BODY);
    ASSERT(PE_LoadU16(AST_REC + 0x0Cu) == 45u, "god mode: HP-halving status leaves HP");
    ast_player_hit_fixture();
    PE_StoreU8(AST_ACTION + 1u, 9u);            /* HP to 1 (or death at 1) */
    func_80020288(AST_BODY);
    ASSERT(PE_LoadU16(AST_REC + 0x0Cu) == 45u, "god mode: HP-to-1 status leaves HP");
    ast_player_hit_fixture();
    PE_StoreU16(AST_REC + 0x0Cu, 1u); PE_StoreU8(AST_ACTION + 1u, 9u);
    func_80020288(AST_BODY);
    ASSERT(PE_LoadU16(AST_REC + 0x0Cu) == 1u, "god mode: the instant-death status cannot kill at 1 HP");
    /* Off: the same statuses act as retail. */
    pe_enh_set_god_mode(0);
    ast_player_hit_fixture();
    PE_StoreU8(AST_ACTION + 1u, 8u);
    func_80020288(AST_BODY);
    ASSERT(PE_LoadU16(AST_REC + 0x0Cu) == 22u, "off: HP-halving status 45 -> 22");
    ast_player_hit_fixture();
    PE_StoreU16(AST_REC + 0x0Cu, 1u); PE_StoreU8(AST_ACTION + 1u, 9u);
    func_80020288(AST_BODY);
    ASSERT(PE_LoadU16(AST_REC + 0x0Cu) == 65535u, "off: instant-death status at 1 HP kills (retail)");
    ASSERT(!PE_Port_ShouldStop(), "executes natively");
    unlink(enh_conf); pe_enh_reset();
    PASS();
}

static void test_ASSIST_config_and_env(void)
{
    TEST("ASSIST_config_and_env");
    enh_isolate();
    pe_enh_set_one_hit_kills(1); pe_enh_set_god_mode(1); pe_enh_set_exp_mult(3);
    pe_enh_reset();
    ASSERT(pe_enh_one_hit_kills() && pe_enh_god_mode() && pe_enh_exp_mult() == 3,
           "assists persist in the enhancements config");
    ASSERT(pe_enh_move_speed_pct() == 100 && !pe_enh_always_run(), "movement settings untouched");
    pe_enh_set_one_hit_kills(0); pe_enh_set_god_mode(0); pe_enh_set_exp_mult(1);
    pe_enh_reset();
    ASSERT(!pe_enh_one_hit_kills() && !pe_enh_god_mode() && pe_enh_exp_mult() == 1, "cleared and persisted");
    setenv("PE_ONE_HIT_KILLS", "1", 1); setenv("PE_GOD_MODE", "1", 1); setenv("PE_EXP_MULT", "2", 1);
    pe_enh_reset();
    ASSERT(pe_enh_one_hit_kills() && pe_enh_god_mode() && pe_enh_exp_mult() == 2, "env overrides apply");
    unsetenv("PE_ONE_HIT_KILLS"); unsetenv("PE_GOD_MODE"); unsetenv("PE_EXP_MULT");
    pe_enh_reset();
    ASSERT(!pe_enh_one_hit_kills() && !pe_enh_god_mode() && pe_enh_exp_mult() == 1,
           "env overrides are not written back");
    unlink(enh_conf); pe_enh_reset();
    PASS();
}

static void test_ASSIST_dev_menu_rows(void)
{
    static uint8_t fb[320 * 240 * 3];
    TEST("ASSIST_dev_menu_rows");
    enh_isolate();
    setenv("PE_DEV_MENU", "1", 1);
    pe_plat_cheats_reset();
    (void)pe_plat_cheats_key(PE_CHEAT_KEY_DEV_MENU, 1);
    for (int i = 0; i < 7; i++) (void)pe_plat_cheats_key(PE_CHEAT_KEY_DOWN, 1);   /* ASSISTS (row 8) */
    (void)pe_plat_cheats_key(PE_CHEAT_KEY_ENTER, 1);                             /* -> ASSISTS page */
    ASSERT(pe_plat_cheats_overlay(fb, 320, 240) != fb, "assists page draws");
    (void)pe_plat_cheats_key(PE_CHEAT_KEY_ENTER, 1);
    ASSERT(pe_enh_one_hit_kills() == 1, "row 1 toggles 1-hit kills");
    (void)pe_plat_cheats_key(PE_CHEAT_KEY_DOWN, 1);
    (void)pe_plat_cheats_key(PE_CHEAT_KEY_ENTER, 1);
    ASSERT(pe_enh_god_mode() == 1, "row 2 toggles god mode");
    (void)pe_plat_cheats_key(PE_CHEAT_KEY_DOWN, 1);
    (void)pe_plat_cheats_key(PE_CHEAT_KEY_ENTER, 1);
    ASSERT(pe_enh_exp_mult() == 2, "row 3 cycles the EXP/BP multiplier");
    (void)pe_plat_cheats_key(PE_CHEAT_KEY_ENTER, 1);
    (void)pe_plat_cheats_key(PE_CHEAT_KEY_ENTER, 1);
    ASSERT(pe_enh_exp_mult() == 1, "cycle wraps x3 -> x1");
    (void)pe_plat_cheats_key(PE_CHEAT_KEY_UP, 1);
    (void)pe_plat_cheats_key(PE_CHEAT_KEY_ENTER, 1);
    (void)pe_plat_cheats_key(PE_CHEAT_KEY_UP, 1);
    (void)pe_plat_cheats_key(PE_CHEAT_KEY_ENTER, 1);
    ASSERT(!pe_enh_one_hit_kills() && !pe_enh_god_mode(), "toggles turn back off");
    (void)pe_plat_cheats_key(PE_CHEAT_KEY_BACK, 1);
    (void)pe_plat_cheats_key(PE_CHEAT_KEY_BACK, 1);
    ASSERT(!pe_plat_cheats_menu_open(), "B backs out to closed");
    /* OPTIONS is still row 7 (test_ENH_dev_menu_rows relies on it). */
    (void)pe_plat_cheats_key(PE_CHEAT_KEY_DEV_MENU, 1);
    for (int i = 0; i < 6; i++) (void)pe_plat_cheats_key(PE_CHEAT_KEY_DOWN, 1);
    (void)pe_plat_cheats_key(PE_CHEAT_KEY_ENTER, 1);
    (void)pe_plat_cheats_key(PE_CHEAT_KEY_ENTER, 1);
    (void)pe_plat_cheats_key(PE_CHEAT_KEY_ENTER, 1);
    ASSERT(pe_enh_move_speed_pct() == 125, "OPTIONS > ENHANCEMENTS unchanged at row 7");
    (void)pe_plat_cheats_key(PE_CHEAT_KEY_DEV_MENU, 1);
    unsetenv("PE_DEV_MENU");
    pe_plat_cheats_reset();
    unlink(enh_conf); pe_enh_reset();
    PASS();
}

static void test_ASSIST_option_registry(void);
static void test_ASSIST_all(void)
{
    test_ASSIST_option_registry();
    test_ASSIST_defaults_are_retail();
    test_ASSIST_one_hit_kills();
    test_ASSIST_exp_mult();
    test_ASSIST_god_mode();
    test_ASSIST_config_and_env();
    test_ASSIST_dev_menu_rows();
    enh_isolate();                                  /* leave defaults for later tests */
    unsetenv("PE_ENH_CONFIG");
}

/* The option registry: one entry per player-facing option, retail first. */
static void test_ASSIST_option_registry(void)
{
    char label[64];
    int n = pe_enh_option_count();
    TEST("ASSIST_option_registry");
    enh_isolate();
    ASSERT(n == 5, "five registered options");
    ASSERT(pe_enh_category_count(PE_ENH_CAT_GAMEPLAY) == 2 && pe_enh_category_count(PE_ENH_CAT_ASSIST) == 3 &&
           pe_enh_category_count(PE_ENH_CAT_GRAPHICS) == 0, "categories: 2 gameplay, 3 assists, 0 graphics");
    for (int i = 0; i < n; i++) {
        const PeEnhOption *o = pe_enh_option(i);
        ASSERT(o && o->id && o->label && o->env && o->values && o->nvalues >= 2, "option is fully described");
        ASSERT(pe_enh_option_index(o->id) == i, "ids are unique and resolve");
        ASSERT(pe_enh_option_is_retail(i) && pe_enh_option_value(i) == o->values[0], "default is the retail value");
        for (int j = 0; j < i; j++) ASSERT(strcmp(pe_enh_option(j)->id, o->id) != 0, "no duplicate ids");
        pe_enh_option_label(i, label, sizeof label);
        ASSERT(strncmp(label, o->label, strlen(o->label)) == 0 && strstr(label, ": "), "label renders as NAME: VALUE");
        for (int k = 0; k < o->nvalues; k++) pe_enh_option_cycle(i);
        ASSERT(pe_enh_option_is_retail(i), "cycling through every value returns to retail");
    }
    ASSERT(pe_enh_category_option(PE_ENH_CAT_GAMEPLAY, 0) == pe_enh_option_index("move_speed") &&
           pe_enh_category_option(PE_ENH_CAT_ASSIST, 2) == pe_enh_option_index("exp_mult") &&
           pe_enh_category_option(PE_ENH_CAT_ASSIST, 3) == -1, "category order is the menu order");
    ASSERT(pe_enh_option(pe_enh_option_index("move_speed"))->credit != NULL &&
           pe_enh_option(pe_enh_option_index("god_mode"))->credit != NULL, "credits recorded");
    pe_enh_option_set(pe_enh_option_index("move_speed"), 160);
    ASSERT(pe_enh_move_speed_pct() == 150, "set snaps to a listed value (160 -> 150)");
    pe_enh_option_set(pe_enh_option_index("exp_mult"), 9);
    ASSERT(pe_enh_exp_mult() == 3, "set snaps to the largest listed value");
    unlink(enh_conf); pe_enh_reset();
    PASS();
}
