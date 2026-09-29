/* Opt-in cheat console (pe_plat/cheats.h, platform/plat_cheats.c). */
#include "pe_plat/cheats.h"

#define CHT_REC    0x80140000u   /* Aya battle record (*0x8009D278) */
#define CHT_GUN    0x80140100u   /* weapon record (rec+0x68) */
#define CHT_ACTOR  0x80140200u   /* Aya actor (*0x8009D254) */
#define CHT_STATS  0x80140300u   /* *actor */
#define CHT_ENEMY  0x80140400u
#define CHT_EBODY  0x80140500u
#define CHT_INV    0x80140600u   /* D_8009D048 inventory (u16 slots) */
#define CHT_SCRIPT 0x80185000u   /* inside the F5 scan window 80180000..801F0000 */

static void cheats_fixture(void)
{
    ResetTestState();
    pe_plat_cheats_reset();
    PE_StoreU32(0x8009D278u, CHT_REC);
    PE_StoreU16(CHT_REC + 0x0Cu, 10u);             /* HP */
    PE_StoreU16(CHT_REC + 0x1Cu, 45u);             /* max HP */
    PE_StoreU32(CHT_REC + 0x68u, CHT_GUN);
    PE_StoreU32(CHT_GUN + 0x0Cu, 0x12340000u | 6u); /* 6 rounds loaded */
    PE_StoreU16(0x800A1E6Eu, 12u);                 /* reserve pool 0 */
    PE_StoreU32(0x8009D254u, CHT_ACTOR);
    PE_StoreU32(CHT_ACTOR, CHT_STATS);
    PE_StoreU32(CHT_STATS + 0x08u, 5u << 16);      /* PE 5 */
    PE_StoreU32(CHT_STATS + 0x28u, 60u << 16);     /* max PE 60 */
    PE_StoreU32(0x8009E000u, CHT_ENEMY);           /* enemy table, then null */
    PE_StoreU32(0x8009E00Cu, 0u);
    PE_StoreU32(CHT_ENEMY, CHT_EBODY);
    PE_StoreU32(CHT_EBODY + 0x10u, 1000092u);
    PE_StoreU32(0x8009D048u, CHT_INV);
    PE_StoreU32(0x8009D050u, 8u);
    PE_StoreU16(CHT_INV, 0x44u);                   /* one ordinary item owned */
    /* field op 0xA7 (give item), argc 2, arg0 immediate 0x123, arg1 kind 2 */
    PE_StoreU32(CHT_SCRIPT, 0xA7u | (2u << 13) | (2u << 20));
    PE_StoreU32(CHT_SCRIPT + 4u, 0u);
    PE_StoreU32(CHT_SCRIPT + 8u, 0x123u);
    PE_StoreU32(CHT_SCRIPT + 12u, 7u);
}

static void cheat_press(PeCheatKey k) { (void)pe_plat_cheats_key(k, 1); (void)pe_plat_cheats_key(k, 0); }

static void test_CHEATS_all_off_identity(void)
{
    static uint8_t before[0x200000];
    static uint8_t fb[320 * 240 * 3];
    TEST("CHEATS_all_off_identity");
    cheats_fixture();
    PE_StoreU32(0x8009D1A0u, 2u);                  /* in a battle */
    memcpy(before, PE_TranslateConst(0x80000000u, 0x200000u), sizeof(before));
    for (int i = 0; i < 5; i++) { pe_plat_cheats_apply(); pe_plat_cheats_frame(); }
    ASSERT(memcmp(before, PE_TranslateConst(0x80000000u, 0x200000u), sizeof(before)) == 0,
           "all cheats off: guest RAM byte-for-byte unchanged");
    ASSERT(pe_plat_cheats_overlay(fb, 320, 240) == fb, "all off: frame presented unmodified");
    ASSERT(pe_plat_cheats_ff_multiplier() == 0, "F7 default: uncapped fast-forward");
    /* F1 help alone draws text but still writes no guest state */
    cheat_press(PE_CHEAT_KEY_F1);
    pe_plat_cheats_apply();
    ASSERT(memcmp(before, PE_TranslateConst(0x80000000u, 0x200000u), sizeof(before)) == 0,
           "help overlay writes no guest state");
    ASSERT(pe_plat_cheats_overlay(fb, 320, 240) != fb, "help overlay drawn on a copy");
    pe_plat_cheats_reset();
    PASS();
}

static void test_CHEATS_toggles(void)
{
    PePlatCheatState s;
    static uint8_t fb[320 * 240 * 3];
    const uint8_t *shown;
    TEST("CHEATS_toggles");
    cheats_fixture();
    /* F2 god mode */
    cheat_press(PE_CHEAT_KEY_F2);
    pe_plat_cheats_apply();
    ASSERT(PE_LoadU16(CHT_REC + 0x0Cu) == 45u, "god mode holds HP at max");
    PE_StoreU16(CHT_REC + 0x0Cu, 3u); pe_plat_cheats_apply();
    ASSERT(PE_LoadU16(CHT_REC + 0x0Cu) == 45u, "god mode undoes damage");
    shown = pe_plat_cheats_overlay(fb, 320, 240);
    ASSERT(shown != fb, "CHEATS label drawn while a cheat is active");
    cheat_press(PE_CHEAT_KEY_F2);
    PE_StoreU16(CHT_REC + 0x0Cu, 3u); pe_plat_cheats_apply();
    ASSERT(PE_LoadU16(CHT_REC + 0x0Cu) == 3u, "god mode off: HP left alone");
    /* F3 ammo + PE */
    cheat_press(PE_CHEAT_KEY_F3);
    pe_plat_cheats_apply();
    ASSERT(PE_LoadU32(CHT_STATS + 0x08u) == (60u << 16), "F3 fills PE");
    PE_StoreU32(CHT_GUN + 0x0Cu, 0x12340000u | 2u);
    PE_StoreU16(0x800A1E6Eu, 4u);
    pe_plat_cheats_apply();
    ASSERT(PE_LoadU32(CHT_GUN + 0x0Cu) == (0x12340000u | 6u), "F3 restores loaded rounds, keeps other bits");
    ASSERT(PE_LoadU16(0x800A1E6Eu) == 12u, "F3 restores reserve ammo");
    cheat_press(PE_CHEAT_KEY_F3);
    /* F4 instant win: only in a battle */
    PE_StoreU32(0x8009D1A0u, 0u);
    cheat_press(PE_CHEAT_KEY_F4); pe_plat_cheats_apply();
    ASSERT(PE_LoadU32(CHT_EBODY + 0x10u) == 1000092u, "F4 outside a battle does nothing");
    PE_StoreU32(0x8009D1A0u, 2u);
    cheat_press(PE_CHEAT_KEY_F4); pe_plat_cheats_apply();
    ASSERT(PE_LoadU32(CHT_EBODY + 0x10u) == 0u, "F4 zeroes enemy HP (the game's own victory path follows)");
    /* F5 area kit: key item from the loaded script, via func_80053D2C */
    cheat_press(PE_CHEAT_KEY_F5); pe_plat_cheats_apply();
    ASSERT(PE_LoadU16(CHT_INV + 2u) == 0x123u, "F5 adds the script's key item into the first free slot");
    ASSERT(pe_plat_cheats_area_kit(0x80180000u, 0x801F0000u) == 0, "F5 does not add an owned item twice");
    /* F7 speed cycle */
    cheat_press(PE_CHEAT_KEY_F7); ASSERT(pe_plat_cheats_ff_multiplier() == 2, "F7 -> 2x");
    cheat_press(PE_CHEAT_KEY_F7); ASSERT(pe_plat_cheats_ff_multiplier() == 4, "F7 -> 4x");
    cheat_press(PE_CHEAT_KEY_F7); ASSERT(pe_plat_cheats_ff_multiplier() == 8, "F7 -> 8x");
    cheat_press(PE_CHEAT_KEY_F7); ASSERT(pe_plat_cheats_ff_multiplier() == 0, "F7 -> max");
    /* F8 warp: menu consumes arrows/Enter; transfer waits for the field */
    ASSERT(!pe_plat_cheats_key(PE_CHEAT_KEY_ENTER, 1), "Enter goes to the game while the menu is closed");
    cheat_press(PE_CHEAT_KEY_F8);
    ASSERT(pe_plat_cheats_menu_open(), "F8 opens the warp menu");
    cheat_press(PE_CHEAT_KEY_DOWN); cheat_press(PE_CHEAT_KEY_DOWN); cheat_press(PE_CHEAT_KEY_DOWN);
    ASSERT(pe_plat_cheats_key(PE_CHEAT_KEY_ENTER, 1), "menu consumes Enter");
    (void)pe_plat_cheats_key(PE_CHEAT_KEY_ENTER, 0);
    PE_StoreU32(0x8009D1A0u, 2u);
    pe_plat_cheats_apply();
    ASSERT(PE_LoadU32(0x8009D280u) == 0u, "warp waits while a battle runs");
    PE_StoreU32(0x8009D1A0u, 0u);
    pe_plat_cheats_apply();
    ASSERT(PE_LoadU32(0x8009D280u) == 0xA80021C8u, "warp latches the Eve 1 room token (room_transfer)");
    ASSERT(PE_LoadU32(0x800A7918u) == 0x5Bu, "warp sets the checkpoint story word");
    ASSERT(PE_LoadU32(0x8009D1A0u) & 0x2000u, "room transfer requested via func_80017BB4");
    pe_plat_cheats_get_state(&s);
    ASSERT(s.wins == 1 && s.kits == 1 && s.warps == 1 && !s.menu, "counters");
    ASSERT(pe_plat_cheats_warp_count() >= 13, "Day-1 checkpoint list");
    pe_plat_cheats_reset();
    ResetTestState();
    PASS();
}

static void test_CHEATS_all(void)
{
    pe_plat_cheats_init();
    test_CHEATS_all_off_identity();
    test_CHEATS_toggles();
}
