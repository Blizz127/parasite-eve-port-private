/* Opt-in assists (pe_plat/enhance.h): 1-hit kills, god mode, EXP/BP
 * multiplier.  Hooks: func_80028574 (enemy damage), func_8001F4D4 /
 * func_8001D340 / func_80020288 (player HP), func_80028E94 (EXP/BP award).
 * Uses the ATK17 / ATK18 retail fixtures (test_damage.h, test_defeat.h) and
 * enh_isolate() (test_enhance.h). */
#include "pe_spu.h"

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
    /* Script-phase enemies (HP marked +1000000) are taken to the marker, never killed. */
    PE_StoreU32(0x8009D1A0u, PE_LoadU32(0x8009D1A0u) & ~0x800u);
    PE_StoreU32(AST_BODY + 0x10u, 1000450u);
    ASSERT(pe_enh_enemy_damage(AST_BODY, 30) == 450, "HP > 1000000 (script phase): one hit down to the marker, not a kill");
    PE_StoreU32(AST_BODY + 0x10u, 1000000u);
    ASSERT(pe_enh_enemy_damage(AST_BODY, 30) == 30, "at the marker (script exit window): retail damage");
    PE_StoreU32(AST_BODY + 0x10u, 999999u);
    ASSERT(pe_enh_enemy_damage(AST_BODY, 30) == 30, "just below the marker (script exit window): retail damage");
    PE_StoreU32(AST_BODY + 0x10u, 450u);
    ASSERT(pe_enh_enemy_damage(AST_BODY, 30) == 450, "marker cleared: the assist kills again");
    /* Scripted battle (flag 0x800) with a marked enemy (Day-5 Statue of Liberty Eve): to the marker. */
    PE_StoreU32(0x8009D1A0u, PE_LoadU32(0x8009D1A0u) | 0x800u);
    PE_StoreU32(AST_BODY + 0x10u, 1001589u);
    ASSERT(pe_enh_enemy_damage(AST_BODY, 30) == 1589, "flag 0x800 + HP > 1000000: one hit down to the marker");
    PE_StoreU32(AST_BODY + 0x10u, 1589u);
    ASSERT(pe_enh_enemy_damage(AST_BODY, 30) == 30, "flag 0x800, unmarked: retail damage");
    PE_StoreU32(0x8009D1A0u, PE_LoadU32(0x8009D1A0u) & ~0x800u);
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

/* OPTIONS > GRAPHICS (row 2) and OPTIONS > AUDIO (row 3) registry pages. */
static void test_ASSIST_dev_menu_graphics_page(void)
{
    static uint8_t fb[320 * 240 * 3];
    TEST("ASSIST_dev_menu_graphics_page");
    enh_isolate();
    setenv("PE_DEV_MENU", "1", 1);
    pe_plat_cheats_reset();
    (void)pe_plat_cheats_key(PE_CHEAT_KEY_DEV_MENU, 1);
    for (int i = 0; i < 6; i++) (void)pe_plat_cheats_key(PE_CHEAT_KEY_DOWN, 1);   /* OPTIONS (row 7) */
    (void)pe_plat_cheats_key(PE_CHEAT_KEY_ENTER, 1);                             /* -> OPTIONS */
    (void)pe_plat_cheats_key(PE_CHEAT_KEY_DOWN, 1);                              /* GRAPHICS */
    (void)pe_plat_cheats_key(PE_CHEAT_KEY_ENTER, 1);                             /* -> GRAPHICS */
    ASSERT(pe_plat_cheats_overlay(fb, 320, 240) != fb, "graphics page draws");
    (void)pe_plat_cheats_key(PE_CHEAT_KEY_ENTER, 1);
    ASSERT(pe_enh_display_scaler() == PE_ENH_SCALER_SHARP_BILINEAR, "row 1 cycles the display scaler");
    (void)pe_plat_cheats_key(PE_CHEAT_KEY_DOWN, 1);
    (void)pe_plat_cheats_key(PE_CHEAT_KEY_ENTER, 1);
    ASSERT(pe_enh_internal_scale() == 2 && PE_GPU_GetRenderScale() == 2, "row 2 cycles the internal resolution");
    for (int i = 0; i < 3; i++) (void)pe_plat_cheats_key(PE_CHEAT_KEY_ENTER, 1);
    ASSERT(pe_enh_internal_scale() == 1 && PE_GPU_GetRenderScale() == 1, "4X wraps to NATIVE");
    (void)pe_plat_cheats_key(PE_CHEAT_KEY_DOWN, 1);
    (void)pe_plat_cheats_key(PE_CHEAT_KEY_ENTER, 1);
    ASSERT(PE_GPU_GetTextureFilter() == 1 && PE_GPU_GetRenderScale() == 2, "row 3 turns on the polygon filter at 2x");
    (void)pe_plat_cheats_key(PE_CHEAT_KEY_ENTER, 1);
    (void)pe_plat_cheats_key(PE_CHEAT_KEY_DOWN, 1);
    (void)pe_plat_cheats_key(PE_CHEAT_KEY_ENTER, 1);
    ASSERT(HostFB_GetSSAA() == 1 && PE_GPU_GetRenderScale() == 2, "row 4 turns on supersample AA at 2x");
    (void)pe_plat_cheats_key(PE_CHEAT_KEY_ENTER, 1);
    ASSERT(!HostFB_GetSSAA() && PE_GPU_GetRenderScale() == 1, "AA off restores native rendering");
    (void)pe_plat_cheats_key(PE_CHEAT_KEY_DOWN, 1);
    (void)pe_plat_cheats_key(PE_CHEAT_KEY_ENTER, 1);
    ASSERT(PE_GPU_GetShadowDither() == 1 && PE_GPU_GetRenderScale() == 2, "row 5 selects fine dithering at 2x");
    (void)pe_plat_cheats_key(PE_CHEAT_KEY_ENTER, 1);
    ASSERT(PE_GPU_GetShadowDither() == 2 && PE_GPU_GetRenderScale() == 2, "then dithering off");
    (void)pe_plat_cheats_key(PE_CHEAT_KEY_ENTER, 1);
    ASSERT(PE_GPU_GetShadowDither() == 0 && PE_GPU_GetRenderScale() == 1, "retail dithering restores native rendering");
    (void)pe_plat_cheats_key(PE_CHEAT_KEY_BACK, 1);                              /* -> OPTIONS row 2 */
    (void)pe_plat_cheats_key(PE_CHEAT_KEY_DOWN, 1);
    (void)pe_plat_cheats_key(PE_CHEAT_KEY_ENTER, 1);                             /* -> AUDIO (row 3) */
    ASSERT(pe_plat_cheats_overlay(fb, 320, 240) != fb, "audio page draws");
    (void)pe_plat_cheats_key(PE_CHEAT_KEY_ENTER, 1);
    ASSERT(pe_enh_master_volume() == 90, "audio row 1 steps the master volume");
    (void)pe_plat_cheats_key(PE_CHEAT_KEY_DOWN, 1);
    (void)pe_plat_cheats_key(PE_CHEAT_KEY_ENTER, 1);
    ASSERT(pe_enh_movie_volume() == 90 && PE_Spu_CdGain() == 90, "audio row 2 steps the movie volume");
    (void)pe_plat_cheats_key(PE_CHEAT_KEY_DOWN, 1);
    (void)pe_plat_cheats_key(PE_CHEAT_KEY_ENTER, 1);
    ASSERT(pe_enh_music_volume() == 90 && PE_Spu_MusicGain() == 90 && PE_Spu_SfxGain() == 100,
           "audio row 3 steps the music volume");
    (void)pe_plat_cheats_key(PE_CHEAT_KEY_DOWN, 1);
    (void)pe_plat_cheats_key(PE_CHEAT_KEY_ENTER, 1);
    ASSERT(pe_enh_sfx_volume() == 90 && PE_Spu_SfxGain() == 90, "audio row 4 steps the SFX volume");
    (void)pe_plat_cheats_key(PE_CHEAT_KEY_BACK, 1);                              /* -> OPTIONS row 3 */
    (void)pe_plat_cheats_key(PE_CHEAT_KEY_UP, 1);
    (void)pe_plat_cheats_key(PE_CHEAT_KEY_UP, 1);
    (void)pe_plat_cheats_key(PE_CHEAT_KEY_ENTER, 1);                             /* -> ENHANCEMENTS */
    (void)pe_plat_cheats_key(PE_CHEAT_KEY_ENTER, 1);
    ASSERT(pe_enh_move_speed_pct() == 125, "BACK returns to OPTIONS, whose row 1 is still ENHANCEMENTS");
    pe_enh_set_master_volume(100);
    pe_enh_set_movie_volume(100);
    pe_enh_set_music_volume(100);
    pe_enh_set_sfx_volume(100);
    (void)pe_plat_cheats_key(PE_CHEAT_KEY_DEV_MENU, 1);
    pe_enh_set_display_scaler(PE_ENH_SCALER_NEAREST);
    unsetenv("PE_DEV_MENU");
    pe_plat_cheats_reset();
    unlink(enh_conf); pe_enh_reset();
    PASS();
}

static void test_ASSIST_option_registry(void);
static void test_ASSIST_all(void)
{
    test_ASSIST_option_registry();
    test_ASSIST_dev_menu_graphics_page();
    test_ASSIST_defaults_are_retail();
    test_ASSIST_one_hit_kills();
    test_ASSIST_exp_mult();
    test_ASSIST_god_mode();
    test_ASSIST_config_and_env();
    test_ASSIST_dev_menu_rows();
    enh_isolate();                                  /* leave defaults for later tests */
    enh_hermetic();
}

/* The option registry: one entry per player-facing option, retail first. */
static void test_ASSIST_option_registry(void)
{
    char label[64];
    int n = pe_enh_option_count();
    TEST("ASSIST_option_registry");
    enh_isolate();
    ASSERT(n == 15, "fifteen registered options");
    ASSERT(pe_enh_category_count(PE_ENH_CAT_GAMEPLAY) == 2 && pe_enh_category_count(PE_ENH_CAT_ASSIST) == 3 &&
           pe_enh_category_count(PE_ENH_CAT_GRAPHICS) == 5 && pe_enh_category_count(PE_ENH_CAT_AUDIO) == 4 &&
           pe_enh_category_count(PE_ENH_CAT_CONTROLS) == 1,
           "categories: 2 gameplay, 3 assists, 5 graphics, 4 audio, 1 controls");
    {   /* the controls row: ANALOG STICK, a retail-OFF toggle, last in the registry */
        int a = pe_enh_option_index("analog_stick");
        const PeEnhOption *o = pe_enh_option(a);
        ASSERT(a == n - 1 && pe_enh_category_option(PE_ENH_CAT_CONTROLS, 0) == a &&
               pe_enh_category_option(PE_ENH_CAT_CONTROLS, 1) == -1, "analog_stick is the one controls row, last");
        ASSERT(o && o->format == PE_ENH_FMT_TOGGLE && o->values[0] == 0 && o->nvalues == 2 &&
               strcmp(o->env, "PE_ANALOG_STICK") == 0 && strcmp(o->label, "ANALOG STICK") == 0 &&
               o->credit && strcmp(o->credit, "the owner") == 0, "analog_stick row as registered");
        ASSERT(pe_enh_analog_stick() == 0, "ANALOG STICK defaults OFF (retail digital pad)");
    }
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
           pe_enh_category_option(PE_ENH_CAT_GRAPHICS, 0) == pe_enh_option_index("display_scaler") &&
           pe_enh_category_option(PE_ENH_CAT_GRAPHICS, 1) == pe_enh_option_index("internal_scale") &&
           pe_enh_category_option(PE_ENH_CAT_ASSIST, 3) == -1, "category order is the menu order");
    {   /* the graphics row: named values, retail first, env override, labels */
        int g = pe_enh_option_index("display_scaler");
        const PeEnhOption *o = pe_enh_option(g);
        int highest = o->values[o->nvalues - 1];
#ifdef _WIN32
        ASSERT(o->nvalues == 2 && highest == PE_ENH_SCALER_SHARP_BILINEAR, "Windows excludes unavailable FSR");
#else
        ASSERT(o->nvalues == 3 && highest == PE_ENH_SCALER_FSR1, "threaded presenter offers FSR");
#endif
        ASSERT(o && o->format == PE_ENH_FMT_NAMED && o->names &&
               o->values[0] == PE_ENH_SCALER_NEAREST && o->category == PE_ENH_CAT_GRAPHICS, "display_scaler is a named graphics option");
        ASSERT(pe_enh_display_scaler() == PE_ENH_SCALER_NEAREST, "default display scaler is NEAREST (retail)");
        pe_enh_option_label(g, label, sizeof label);
        ASSERT(strcmp(label, "DISPLAY SCALER: NEAREST (RETAIL)") == 0, "label names the retail value");
        pe_enh_option_cycle(g);
        pe_enh_option_label(g, label, sizeof label);
        ASSERT(pe_enh_display_scaler() == PE_ENH_SCALER_SHARP_BILINEAR && strcmp(label, "DISPLAY SCALER: SHARP BILINEAR") == 0,
               "A cycles to SHARP BILINEAR");
        pe_enh_option_cycle(g);
        pe_enh_option_label(g, label, sizeof label);
        if (highest == PE_ENH_SCALER_FSR1) {
            ASSERT(pe_enh_display_scaler() == PE_ENH_SCALER_FSR1 && strcmp(label, "DISPLAY SCALER: FSR 1 (CPU)") == 0, "then FSR 1");
            pe_enh_option_cycle(g);
        }
        ASSERT(pe_enh_display_scaler() == PE_ENH_SCALER_NEAREST, "available modes wrap to NEAREST");
        pe_enh_set_display_scaler(7);
        ASSERT(pe_enh_display_scaler() == highest, "set snaps to the largest supported value");
        pe_enh_set_display_scaler(0);
        setenv("PE_DISPLAY_SCALER", "2", 1); pe_enh_reset();
        ASSERT(pe_enh_display_scaler() == highest, "PE_DISPLAY_SCALER env override respects capability");
        unsetenv("PE_DISPLAY_SCALER"); pe_enh_reset();
        ASSERT(pe_enh_display_scaler() == PE_ENH_SCALER_NEAREST, "env override is not written back");
    }
    {
        int g = pe_enh_option_index("internal_scale");
        ASSERT(pe_enh_internal_scale() == 1, "internal scale defaults native");
        pe_enh_option_label(g, label, sizeof label);
        ASSERT(strcmp(label, "INTERNAL RESOLUTION: NATIVE (RETAIL)") == 0, "native resolution labeled");
        pe_enh_set_internal_scale(4);
        pe_enh_reset();
        ASSERT(pe_enh_internal_scale() == 4, "internal scale persists");
        setenv("PE_INTERNAL_SCALE", "2", 1); pe_enh_reset();
        ASSERT(pe_enh_internal_scale() == 2, "internal scale env overrides config");
        unsetenv("PE_INTERNAL_SCALE"); pe_enh_reset();
        ASSERT(pe_enh_internal_scale() == 4, "internal scale env does not overwrite config");
        pe_enh_set_internal_scale(1);
    }
    {
        int aa = pe_enh_option_index("ssaa");
        pe_enh_option_set(aa, 1);
        ASSERT(pe_enh_internal_scale() == 1 && PE_GPU_GetRenderScale() == 2 && HostFB_GetSSAA(),
               "SSAA alone renders at 2x without overwriting the selected scale");
        pe_enh_set_internal_scale(3);
        ASSERT(PE_GPU_GetRenderScale() == 3, "explicit internal scale remains authoritative");
        pe_enh_option_set(aa, 0);
        ASSERT(PE_GPU_GetRenderScale() == 3 && !HostFB_GetSSAA(), "AA off preserves explicit scale");
        pe_enh_set_internal_scale(1);
        ASSERT(PE_GPU_GetRenderScale() == 1, "all graphics defaults restore native rendering");
        int filter = pe_enh_option_index("texture_filter");
        pe_enh_option_set(filter, 1);
        ASSERT(PE_GPU_GetRenderScale() == 2 && pe_enh_internal_scale() == 1,
               "polygon filtering alone selects a 2x shadow");
        pe_enh_option_set(filter, 0);
        ASSERT(PE_GPU_GetRenderScale() == 1, "filter off restores native scale");
        int dither = pe_enh_option_index("dithering");
        pe_enh_option_label(dither, label, sizeof label);
        ASSERT(strcmp(label, "DITHERING: RETAIL (RETAIL)") == 0 && PE_GPU_GetShadowDither() == 0, "dithering defaults retail");
        pe_enh_option_set(dither, 1);
        ASSERT(PE_GPU_GetShadowDither() == 1 && PE_GPU_GetRenderScale() == 2, "fine dithering alone selects a 2x shadow");
        pe_enh_option_set(dither, 0);
        ASSERT(PE_GPU_GetShadowDither() == 0 && PE_GPU_GetRenderScale() == 1, "retail dithering restores native scale");
    }
    {   /* audio rows: percent levels falling from the retail 100 */
        int m = pe_enh_option_index("master_volume"), v = pe_enh_option_index("movie_volume");
        ASSERT(m >= 0 && v >= 0 && pe_enh_option(m)->category == PE_ENH_CAT_AUDIO, "audio rows registered");
        ASSERT(pe_enh_master_volume() == 100 && pe_enh_movie_volume() == 100 && PE_Spu_CdGain() == 100,
               "audio defaults are retail and the CD gain is unity");
        pe_enh_option_label(m, label, sizeof label);
        ASSERT(strcmp(label, "MASTER VOLUME: 100% (RETAIL)") == 0, "master volume label");
        pe_enh_option_cycle(m);
        ASSERT(pe_enh_master_volume() == 90, "A steps the level down by 10");
        pe_enh_set_master_volume(55);
        ASSERT(pe_enh_master_volume() == 50, "a level snaps down to a listed step (55 -> 50)");
        pe_enh_set_master_volume(-5);
        ASSERT(pe_enh_master_volume() == 0, "below the list snaps to 0");
        pe_enh_set_master_volume(150);
        ASSERT(pe_enh_master_volume() == 100, "above the list snaps to 100");
        pe_enh_set_movie_volume(40);
        ASSERT(pe_enh_movie_volume() == 40 && PE_Spu_CdGain() == 40, "movie volume drives the CD gain");
        pe_enh_reset();
        ASSERT(pe_enh_movie_volume() == 40 && PE_Spu_CdGain() == 40, "movie volume persists and re-applies");
        setenv("PE_MOVIE_VOLUME", "70", 1); pe_enh_reset();
        ASSERT(pe_enh_movie_volume() == 70 && PE_Spu_CdGain() == 70, "env override applies");
        unsetenv("PE_MOVIE_VOLUME"); pe_enh_reset();
        ASSERT(pe_enh_movie_volume() == 40, "env override is not written back");
        pe_enh_set_movie_volume(100);
        ASSERT(PE_Spu_CdGain() == 100, "100 restores the unity CD gain");
        ASSERT(HostAudio_GainSample(12345, 100) == 12345 && HostAudio_GainSample(-32768, 50) == -16384 &&
               HostAudio_GainSample(32767, 0) == 0, "device-sink gain helper");
        {   /* the ramp: from == to is the constant gain; a change ramps across the submit */
            int16_t src[2 * 4] = {1000, -1000, 1000, -1000, 1000, -1000, 1000, -1000}, out[2 * 4];
            HostAudio_GainRamp(out, src, 0, 4, 4, 50, 50);
            ASSERT(out[0] == 500 && out[7] == -500, "a constant ramp is the plain gain");
            HostAudio_GainRamp(out, src, 0, 4, 4, 100, 0);
            ASSERT(out[0] == 1000 && out[2] == 750 && out[4] == 500 && out[6] == 250, "100 -> 0 ramps down");
        }
        {   /* strict parsing: garbage keeps retail instead of muting */
            FILE *f = fopen(enh_conf, "w");
            ASSERT(f != NULL, "write a hand-edited config");
            fprintf(f, "master_volume=loud\nmovie_volume=\nmove_speed=150\n");
            fclose(f);
            pe_enh_reset();
            ASSERT(pe_enh_master_volume() == 100 && pe_enh_movie_volume() == 100 && pe_enh_move_speed_pct() == 150,
                   "unparsable config values keep retail; valid ones load");
            setenv("PE_MASTER_VOLUME", "50%", 1); pe_enh_reset();
            ASSERT(pe_enh_master_volume() == 100, "an unparsable env override keeps retail");
            unsetenv("PE_MASTER_VOLUME");
            /* an env override is never persisted by a later menu save */
            setenv("PE_MOVE_SPEED", "200", 1); pe_enh_reset();
            ASSERT(pe_enh_move_speed_pct() == 200, "env override in effect");
            pe_enh_set_master_volume(80);
            unsetenv("PE_MOVE_SPEED"); pe_enh_reset();
            ASSERT(pe_enh_move_speed_pct() == 150 && pe_enh_master_volume() == 80,
                   "saving another option writes the config's own value, not the env override");
            pe_enh_set_move_speed_pct(100); pe_enh_set_master_volume(100);
        }
    }
    {   /* MUSIC / SFX VOLUME: the audio page's rows 3 and 4, SPU voice gains by class */
        int mu = pe_enh_option_index("music_volume"), sf = pe_enh_option_index("sfx_volume");
        ASSERT(pe_enh_category_option(PE_ENH_CAT_AUDIO, 2) == mu && pe_enh_category_option(PE_ENH_CAT_AUDIO, 3) == sf &&
               sf == mu + 1 && pe_enh_category_option(PE_ENH_CAT_AUDIO, 4) == -1,
               "music and SFX are the audio page's last two rows");
        ASSERT(pe_enh_option(mu)->category == PE_ENH_CAT_AUDIO && pe_enh_option(sf)->category == PE_ENH_CAT_AUDIO &&
               pe_enh_option(mu)->format == PE_ENH_FMT_PERCENT && pe_enh_option(sf)->format == PE_ENH_FMT_PERCENT &&
               pe_enh_option(mu)->nvalues == 11 && pe_enh_option(sf)->nvalues == 11 &&
               strcmp(pe_enh_option(mu)->env, "PE_MUSIC_VOLUME") == 0 && strcmp(pe_enh_option(sf)->env, "PE_SFX_VOLUME") == 0 &&
               strcmp(pe_enh_option(mu)->credit, "the owner") == 0 && strcmp(pe_enh_option(sf)->credit, "the owner") == 0,
               "music/SFX rows: audio, percent 100..0, env, credit");
        ASSERT(pe_enh_category_option(PE_ENH_CAT_AUDIO, 2) == mu && pe_enh_category_option(PE_ENH_CAT_AUDIO, 3) == sf,
               "AUDIO page rows 3 and 4");
        ASSERT(pe_enh_music_volume() == 100 && pe_enh_sfx_volume() == 100 &&
               PE_Spu_MusicGain() == 100 && PE_Spu_SfxGain() == 100, "music/SFX default retail, unity voice gains");
        pe_enh_option_label(mu, label, sizeof label);
        ASSERT(strcmp(label, "MUSIC VOLUME: 100% (RETAIL)") == 0, "music volume label");
        pe_enh_option_label(sf, label, sizeof label);
        ASSERT(strcmp(label, "SFX VOLUME: 100% (RETAIL)") == 0, "SFX volume label");
        pe_enh_set_music_volume(50);
        ASSERT(pe_enh_music_volume() == 50 && PE_Spu_MusicGain() == 50 && PE_Spu_SfxGain() == 100,
               "music volume drives only the music gain");
        pe_enh_set_sfx_volume(35);
        ASSERT(pe_enh_sfx_volume() == 30 && PE_Spu_SfxGain() == 30 && PE_Spu_MusicGain() == 50,
               "SFX volume snaps (35 -> 30) and drives only the SFX gain");
        pe_enh_reset();
        ASSERT(PE_Spu_MusicGain() == 100 && PE_Spu_SfxGain() == 100, "reset restores unity gains");
        ASSERT(pe_enh_music_volume() == 50 && PE_Spu_MusicGain() == 50 && PE_Spu_SfxGain() == 30,
               "music/SFX volume persist and re-apply");
        setenv("PE_SFX_VOLUME", "80", 1); setenv("PE_MUSIC_VOLUME", "0", 1); pe_enh_reset();
        ASSERT(pe_enh_sfx_volume() == 80 && pe_enh_music_volume() == 0 && PE_Spu_SfxGain() == 80 &&
               PE_Spu_MusicGain() == 0, "env overrides apply");
        unsetenv("PE_SFX_VOLUME"); unsetenv("PE_MUSIC_VOLUME"); pe_enh_reset();
        ASSERT(pe_enh_sfx_volume() == 30 && pe_enh_music_volume() == 50, "env overrides are not written back");
        pe_enh_set_music_volume(100);
        pe_enh_set_sfx_volume(100);
        ASSERT(PE_Spu_MusicGain() == 100 && PE_Spu_SfxGain() == 100, "100 restores the unity voice gains");
    }
    ASSERT(pe_enh_option(pe_enh_option_index("move_speed"))->credit != NULL &&
           pe_enh_option(pe_enh_option_index("god_mode"))->credit != NULL, "credits recorded");
    pe_enh_option_set(pe_enh_option_index("move_speed"), 160);
    ASSERT(pe_enh_move_speed_pct() == 150, "set snaps to a listed value (160 -> 150)");
    pe_enh_option_set(pe_enh_option_index("exp_mult"), 9);
    ASSERT(pe_enh_exp_mult() == 3, "set snaps to the largest listed value");
    unlink(enh_conf); pe_enh_reset();
    PASS();
}
