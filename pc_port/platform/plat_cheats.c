/* pe_plat cheats: opt-in cheat console (contract in pe_plat/cheats.h).
 *
 * Guest-state facts used (each from matched src/ or a verified port):
 *   Aya battle record  *0x8009D278: +0x0C HP (u16), +0x1C max HP (u16),
 *                      +0x68 weapon record: +0x0C low 10 bits = rounds loaded
 *                      (route_ammo_trace.h; the harness HP lock uses +12/+28).
 *   Aya stats          **0x8009D254: +0x08 PE, +0x28 max PE (16.16;
 *                      src/func_800515F8.c reads rec[5] / rec[0x15]).
 *   Ammo reserves      0x800A1E6E + k*32 (u16, k = 0..2).
 *   Enemy table        0x8009E000 + i*12 actor pointers; HP = *(actor)+0x10
 *                      (s32; script tag 0x2C, src/func_8003010C.c).
 *   Add item           func_80053D2C(id) (src/func_80053D2C.c; inventory
 *                      D_8009D048[D_8009D050] u16 slots).
 *   Give-item op       field VM op 0xA7 = func_800194B0 (D_800910A0[0xA7]):
 *                      argc 2, arg0 = item id.
 *   Room transfer      field VM op 0x31 = func_80017BB4(args): latches the
 *                      room token in D_8009D280 and sets D_8009D1A0 |= 0x2000.
 *   Story word         persist[74] = 0x800A7918.
 *   Entry door         persist[1] = 0x800A77F4 (s32): every exit script
 *                      writes it (field op 0x0A, func_800173F4) just before
 *                      op 0x31; the destination room's actors branch on it
 *                      (op 0x09 ==), so a warp must set it or the room runs
 *                      its default arm. M0027I/M0028I additionally require
 *                      movement into their normal encounter zones.
 * No retail data tables are embedded: item ids come from the loaded room
 * scripts at run time; the warp list is route checkpoint tokens.
 */
#include "pe_plat/cheats.h"
#include "pe_plat/mods.h"
#include "pe_plat/enhance.h"
#include "pe_guest_ram.h"
#include "host_framebuffer.h"
#include "pe_drive_tray.h"
extern int func_800698D4(void);   /* retail disc mount (bootstrap) */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Game routines (weak: small tools linking only the platform library). */
extern int func_80053D2C(int id) __attribute__((weak));
extern int func_80017BB4(pe_addr_t a0) __attribute__((weak));

#define GA_AYA_REC     0x8009D278u
#define GA_AYA_ACTOR   0x8009D254u
#define GA_ENEMIES     0x8009E000u
#define GA_D1A0        0x8009D1A0u
#define GA_MOVIE       0x800B0DBAu   /* field movie active (func_801216C4 sets, func_80121A00 clears) */
#define GA_TOKEN       0x8009D280u
#define GA_ROOM_CUR    0x8009D1C4u   /* func_8003F3C4 loops while == D_8009D280 */
#define GA_STORY       0x800A7918u
#define GA_DOOR        0x800A77F4u
#define GA_INV         0x8009D048u
#define GA_INV_COUNT   0x8009D050u
/* Scratch for the room-transfer argument block (unused hand window). */
#define PE_CHEAT_STACK 0x801FF460u   /* +0 args[0] -> +8 token word */

/* door WARP_DOOR_KEEP: leave persist[1] as it is, exactly as the env warp
 * (PE_WARP_ROOM/PE_WARP_STORY) that verified the entry did. */
#define WARP_DOOR_KEEP 0xFFFFFFFFu
typedef struct { const char *name; uint32_t token, story, door; int day; } CheatWarp;
/* Day-1 route checkpoints: first-entry room token and persist[74]
 * (build/lanes/port/rr_full30.log "[ROUTE] change" lines), and the entry
 * door the normal route's exit script writes to persist[1] before that
 * transfer (PE_VM_TRACE of a --route-pad run: the last op 0x0A store to
 * 0x800A77F4 before each op 0x31).  Names, doors and the 13-target New Game
 * sweep: main-repo e401df37 / 2fb417a0 (docs/evidence/codex-warp-sweep). */
static const CheatWarp k_warps[] = {
    {"FIELD START (CARNEGIE HALL)",  0xA8000148u, 0x08u, 0x0Au, 1},
    {"STAGE (M0005I)",               0xA80002C8u, 0x18u, 0x04u, 1},
    {"HALL RAT ENCOUNTER (M0013I)",  0xA80011C8u, 0x40u, 0x0Du, 1},
    {"EVE 1 ROOM (M0023I)",          0xA80021C8u, 0x5Bu, 0x17u, 1},
    {"SEWER ENTRY (M0026I)",         0xA8002348u, 0x68u, 0x17u, 1},
    {"SEWER (M0027I; WALK IN)",      0xA80023C8u, 0x68u, 0x1Au, 1},
    {"SEWER HALL (M0028I; WALK IN)", 0xA8002448u, 0x68u, 0x1Bu, 1},
    {"SEWER PASSAGE (M0031I)",       0xA80030C8u, 0x68u, 0x1Cu, 1},
    {"SEWER ROOM (M0032I)",          0xA8003148u, 0x68u, 0x1Fu, 1},
    {"SEWER ROOM (M0033I)",          0xA80031C8u, 0x68u, 0x20u, 1},
    {"M34 ALLIGATOR (M0034I)",       0xA8003248u, 0x6Cu, 0x21u, 1},
    {"SEWER POST-BOSS (M0359I)",     0xA80654C8u, 0x70u, 0x22u, 1},
    {"M0036I DAY-1 EXIT",            0xA8003348u, 0x78u, 0x01u, 1},
    /* Day-2 route checkpoints (main-repo 063c530b: the gated Day-2 route run,
     * probe 34, "[ROUTE] change" lines: first-entry token and persist[74]).
     * Doors: the predecessor's exit script writes persist[1] (op 0x0A) to the
     * value the destination's spawn arm compares (op 0x09 ==): M0042I<-0 (from
     * M0351I), M0043I<-42, M0047I<-43, M0195I<-191, M0230I<-195, M0239I<-230,
     * M0059I<-58, M0061I<-60, M0065I<-61.  World-map entries (M0351I, M0191I,
     * M0374I) are entered with door 0: their spawn arms compare only other
     * values, so 0 takes the default (world-map) arm. */
    {"DAY 2 START (M0351I)",         0xA80650C8u, 0x80u, 0u,   2},
    {"M0042I",                       0xA8004148u, 0x88u, 0u,   2},
    {"M0043I",                       0xA80041C8u, 0x90u, 42u,  2},
    {"M0047I",                       0xA80043C8u, 0xA4u, 43u,  2},
    {"CENTRAL PARK (M0191I)",        0xA80290C8u, 0xC0u, 0u,   2},
    {"CENTRAL PARK (M0195I)",        0xA80292C8u, 0xC8u, 191u, 2},
    {"CENTRAL PARK (M0230I)",        0xA8043048u, 0xC0u, 195u, 2},
    {"CENTRAL PARK (M0239I)",        0xA80434C8u, 0xC0u, 230u, 2},
    {"PARK (M0374I)",                0xA8067248u, 0xE4u, 0u,   2},
    {"PARK FIGHT (M0059I)",          0xA80054C8u, 0xE8u, 58u,  2},
    {"ZOO (M0061I)",                 0xA80060C8u, 0xE8u, 60u,  2},
    {"ZOO (M0065I)",                 0xA80062C8u, 0xE8u, 61u,  2},
    /* Day 3/4: the warp entries of the day3_route_regress.py gate segments
     * (day3_start, day3_mid, day3_end), which warp with PE_WARP_ROOM /
     * PE_WARP_STORY and leave persist[1] alone; each room's own script then
     * drives the story (0x148, 0x190; M0352I's room_transfer enters the Day-4
     * entry M0098I, which writes 0x1B8).  The warp keeps the current stats and
     * items: later Day-3/4 fights may need the cheats. */
    {"DAY 3 START (M0093I)",         0xA80091C8u, 0x140u, WARP_DOOR_KEEP, 3},
    {"DAY 3 (M0108I)",               0xA8020448u, 0x188u, WARP_DOOR_KEEP, 3},
    {"DAY 4 START (VIA M0352I)",     0xA8065148u, 0x1B0u, WARP_DOOR_KEEP, 4},
};
#define N_WARPS ((int)(sizeof(k_warps) / sizeof(k_warps[0])))

static struct {
    int inited, enabled, hook;
    int god, ammo, help, menu, menu_index, ff_index;
    int dev_page, dev_index, dev_day;
    int pending_win, pending_kit, pending_warp;
    int env_warp;                       /* PE_WARP_ROOM armed, not yet done */
    uint32_t env_token, env_story, env_at;
    int env_set_story;
    int env_disc;                       /* PE_WARP_DISC (dev harness) */
    /* PE_WARP_ROUTE (dev harness): legs warped in order, each PE_WARP_DWELL
     * presents after the previous leg's room is entered. */
    struct { uint32_t token, story; int set_story; } legs[64];
    int nlegs, leg;
    uint32_t dwell, arrive_token, arrive_from;
    unsigned wins, kits, warps;
    uint32_t ammo_gun, ammo_loaded_max, ammo_pool_max[3];
    char toast[64]; int toast_frames;
    uint64_t frames;
    struct { uint64_t frame; PeCheatKey key; } script[64];
    int nscript, iscript;
} C;

static const int k_ff_mult[4] = {0, 2, 4, 8};

static void toast(const char *msg)
{
    snprintf(C.toast, sizeof(C.toast), "%s", msg);
    C.toast_frames = 150;
    fprintf(stderr, "[CHEATS] %s\n", msg);
}

static PeCheatKey key_from_name(const char *s, size_t n)
{
    static const struct { const char *name; PeCheatKey key; } t[] = {
        {"F1",PE_CHEAT_KEY_F1},{"F2",PE_CHEAT_KEY_F2},{"F3",PE_CHEAT_KEY_F3},
        {"F4",PE_CHEAT_KEY_F4},{"F5",PE_CHEAT_KEY_F5},{"F7",PE_CHEAT_KEY_F7},
        {"F8",PE_CHEAT_KEY_F8},{"UP",PE_CHEAT_KEY_UP},{"DOWN",PE_CHEAT_KEY_DOWN},
        {"ENTER",PE_CHEAT_KEY_ENTER},{"BACK",PE_CHEAT_KEY_BACK},
        {"DEV_MENU",PE_CHEAT_KEY_DEV_MENU}};
    for (size_t i = 0; i < sizeof(t) / sizeof(t[0]); i++)
        if (strlen(t[i].name) == n && !strncmp(s, t[i].name, n)) return t[i].key;
    return PE_CHEAT_KEY_NONE;
}

static void load_script(void)
{
    const char *s = getenv("PE_CHEAT_KEYS");
    C.nscript = C.iscript = 0;
    while (s && *s && C.nscript < 64) {
        char *end; unsigned long long f = strtoull(s, &end, 10);
        const char *k, *e;
        if (end == s || *end != ':') break;
        k = end + 1; e = k;
        while (*e && *e != ',') e++;
        C.script[C.nscript].frame = f;
        C.script[C.nscript].key = key_from_name(k, (size_t)(e - k));
        if (C.script[C.nscript].key != PE_CHEAT_KEY_NONE) C.nscript++;
        s = *e ? e + 1 : e;
    }
}

/* Dev-only env warp (PE_DEV_MENU=1): start a run in any room, e.g. to probe
 * Day 2/3 rooms headless for CPU_BOUNDARY/REFUSED stops.
 *   PE_WARP_ROOM   m0041i (room package slot) or a raw token (A80040C8)
 *   PE_WARP_STORY  persist[74] story value in hex (default: unchanged)
 *   PE_WARP_AT     earliest host frame for the transfer (default 0)
 * The warp goes through the same field-VM room_transfer as the F8 warp menu,
 * once the field is idle.  Room token = 0xA8000048 | v << 7 with
 * v = hundreds * 0x400 + tens * 0x20 + units of the slot number (checked
 * against every Day 1 checkpoint token, e.g. m0359i -> A80654C8). */
uint32_t pe_plat_cheats_room_token(const char *room)
{
    unsigned n;
    char *end;
    if (!room || !*room) return 0u;
    if ((room[0] == 'm' || room[0] == 'M') && room[1]) {
        n = (unsigned)strtoul(room + 1, &end, 10);
        if (end == room + 1 || n == 0u || n > 999u) return 0u;
        return 0xA8000048u | (((n / 100u) * 0x400u + ((n / 10u) % 10u) * 0x20u + n % 10u) << 7);
    }
    n = (unsigned)strtoul(room, &end, 16);
    return (end != room && *end == '\0') ? (uint32_t)n : 0u;
}

/* PE_WARP_ROUTE="m0351i:80,m0042i:88,m0043i,..." (dev harness): a chain of
 * env warps.  Each leg goes through the same field-VM room_transfer; a leg
 * without ":story" keeps the story word.  The next leg is armed
 * PE_WARP_DWELL presents (default 1500) after the previous leg's room is
 * entered, and every entry is logged as "[WARP] route arrived".  Returns the
 * number of legs (0 = not set or malformed). */
static int load_env_route(void)
{
    const char *s = getenv("PE_WARP_ROUTE"), *dw = getenv("PE_WARP_DWELL");
    char buf[16];
    if (!s || !*s) return 0;
    C.nlegs = 0;
    while (*s) {
        size_t n = strcspn(s, ",");
        const char *colon = memchr(s, ':', n);
        size_t rn = colon ? (size_t)(colon - s) : n;
        if (rn == 0 || rn >= sizeof buf || C.nlegs == (int)(sizeof C.legs / sizeof C.legs[0])) {
            fprintf(stderr, "[WARP] PE_WARP_ROUTE malformed at '%s'\n", s);
            C.nlegs = 0;
            return 0;
        }
        memcpy(buf, s, rn); buf[rn] = '\0';
        C.legs[C.nlegs].token = pe_plat_cheats_room_token(buf);
        if (!C.legs[C.nlegs].token) {
            fprintf(stderr, "[WARP] PE_WARP_ROUTE room %s not understood\n", buf);
            C.nlegs = 0;
            return 0;
        }
        C.legs[C.nlegs].set_story = colon != NULL && colon + 1 < s + n;
        C.legs[C.nlegs].story = C.legs[C.nlegs].set_story ? (uint32_t)strtoul(colon + 1, NULL, 16) : 0u;
        C.nlegs++;
        s += n;
        if (*s == ',') s++;
    }
    C.dwell = dw && *dw ? (uint32_t)strtoul(dw, NULL, 10) : 1500u;
    C.leg = 0;
    C.env_token = C.legs[0].token;
    C.env_set_story = C.legs[0].set_story;
    C.env_story = C.legs[0].story;
    fprintf(stderr, "[WARP] route armed legs=%d dwell=%u\n", C.nlegs, (unsigned)C.dwell);
    return C.nlegs;
}

static void load_env_warp(void)
{
    const char *r = getenv("PE_WARP_ROOM"), *st = getenv("PE_WARP_STORY"), *at = getenv("PE_WARP_AT");
    if (getenv("PE_WARP_ROUTE") && *getenv("PE_WARP_ROUTE")) {
        if (!pe_plat_cheats_dev_mode()) {
            fprintf(stderr, "[WARP] PE_WARP_ROUTE ignored: dev-only (needs PE_DEV_MENU=1)\n");
            return;
        }
        if (!load_env_route()) return;
        C.env_at = at ? (uint32_t)strtoul(at, NULL, 10) : 0u;
        {
            const char *dsc = getenv("PE_WARP_DISC");
            C.env_disc = dsc ? atoi(dsc) : 0;
        }
        C.env_warp = 1;
        return;
    }
    if (!r || !*r) return;
    if (!pe_plat_cheats_dev_mode()) {
        fprintf(stderr, "[WARP] PE_WARP_ROOM ignored: dev-only (needs PE_DEV_MENU=1)\n");
        return;
    }
    C.env_token = pe_plat_cheats_room_token(r);
    if (!C.env_token) { fprintf(stderr, "[WARP] PE_WARP_ROOM=%s not understood\n", r); return; }
    C.env_set_story = st && *st;
    C.env_story = C.env_set_story ? (uint32_t)strtoul(st, NULL, 16) : 0u;
    C.env_at = at ? (uint32_t)strtoul(at, NULL, 10) : 0u;
    {
        const char *dsc = getenv("PE_WARP_DISC");
        C.env_disc = dsc ? atoi(dsc) : 0;
    }
    C.env_warp = 1;
    fprintf(stderr, "[WARP] armed room=%s token=%08X story=%s%X at>=%u\n", r,
            (unsigned)C.env_token, C.env_set_story ? "" : "(unchanged) ",
            (unsigned)C.env_story, (unsigned)C.env_at);
}

static void frame_hook(const PePlatModEventData *ev, void *user)
{
    (void)ev; (void)user;
    pe_plat_cheats_apply();
}

void pe_plat_cheats_init(void)
{
    const char *e;
    if (C.inited) return;
    C.inited = 1;
    e = getenv("PE_CHEATS");
    C.enabled = !(e && e[0] == '0');
    if (!C.enabled) return;
    load_script();
    load_env_warp();
    C.hook = pe_plat_mods_subscribe(PE_PLAT_EVENT_FRAME_TICK, frame_hook, NULL);
}

int pe_plat_cheats_enabled(void) { return C.inited ? C.enabled : 1; }
/* PE_DEV_MENU: "1" developer mode (adds the env warps PE_WARP_*), "0" off,
 * unset or anything else player mode.  In both reachable modes the menu opens
 * with Select+Start (Back/View + Start/Menu) on one controller, or F8; a lone
 * Select or Start still reaches the game (host_pad_combo.c withholds it only
 * for the combo window).  PE_CHEATS=0 turns every mode off. */
int pe_plat_cheats_dev_menu_enabled(void)
{
    const char *e = getenv("PE_DEV_MENU");
    return pe_plat_cheats_enabled() && !(e && strcmp(e, "0") == 0);
}
int pe_plat_cheats_dev_mode(void)
{
    const char *e = getenv("PE_DEV_MENU");
    return pe_plat_cheats_enabled() && e && strcmp(e, "1") == 0;
}

/* Day of each warp (1..6, 0 = invalid index), from the table. */
int pe_plat_cheats_warp_day(int index)
{
    return (index >= 0 && index < N_WARPS) ? k_warps[index].day : 0;
}
static int warp_day(int index) { return pe_plat_cheats_warp_day(index); }
static int day_warp_count(int day)
{
    int n = 0;
    for (int i = 0; i < N_WARPS; i++) if (warp_day(i) == day) n++;
    return n;
}
/* OPTIONS (page 4) lists one registry page per row; ASSISTS (page 6) is
 * reached from the root.  Rows keep this order (tests count keys). */
static const struct { int page; PeEnhCategory category; const char *label; } k_option_pages[] = {
    {5, PE_ENH_CAT_GAMEPLAY, "ENHANCEMENTS"},
    {7, PE_ENH_CAT_GRAPHICS, "GRAPHICS"},
    {8, PE_ENH_CAT_AUDIO,    "AUDIO"},
    {9, PE_ENH_CAT_CONTROLS, "CONTROLS"},
};
#define N_OPTION_PAGES ((int)(sizeof k_option_pages / sizeof k_option_pages[0]))
static int option_row(int page)
{
    for (int i = 0; i < N_OPTION_PAGES; i++) if (k_option_pages[i].page == page) return i;
    return -1;
}
static int registry_page(int page) { return page == 6 || option_row(page) >= 0; }
static PeEnhCategory page_category(int page)
{
    int row = option_row(page);
    return row >= 0 ? k_option_pages[row].category : PE_ENH_CAT_ASSIST;
}
static int dev_rows(void)
{
    switch (C.dev_page) {
    case 1: return 8;                        /* cheats, FF speed, warp, options, assists */
    case 2: return 6;                        /* DAY 1..6 */
    case 3: return day_warp_count(C.dev_day);
    case 4: return N_OPTION_PAGES;           /* OPTIONS */
    default: return registry_page(C.dev_page) ? pe_enh_category_count(page_category(C.dev_page)) : 6;
    }
}

static int day_warp_index(int day, int row)
{
    for (int i = 0; i < N_WARPS; i++)
        if (warp_day(i) == day && row-- == 0) return i;
    return -1;
}

int pe_plat_cheats_menu_open(void) { return C.enabled && C.menu; }
int pe_plat_cheats_overlay_ui(void) { return C.enabled && (C.menu || C.help || C.toast_frames); }
int pe_plat_cheats_ff_multiplier(void) { return k_ff_mult[C.ff_index & 3]; }
int pe_plat_cheats_warp_count(void) { return N_WARPS; }
const char *pe_plat_cheats_warp_name(int i) { return (i >= 0 && i < N_WARPS) ? k_warps[i].name : ""; }

void pe_plat_cheats_get_state(PePlatCheatState *o)
{
    o->god = C.god; o->ammo = C.ammo; o->help = C.help; o->menu = C.menu;
    o->menu_index = C.menu_index; o->ff_multiplier = pe_plat_cheats_ff_multiplier();
    o->wins = C.wins; o->kits = C.kits; o->warps = C.warps;
}

void pe_plat_cheats_reset(void)
{
    int inited = C.inited, enabled = C.enabled, hook = C.hook;
    memset(&C, 0, sizeof(C));
    C.inited = inited; C.enabled = enabled; C.hook = hook;
}

int pe_plat_cheats_key(PeCheatKey key, int pressed)
{
    char msg[64];
    if (!C.inited) pe_plat_cheats_init();
    if (!C.enabled) return 0;
    if (key == PE_CHEAT_KEY_DEV_MENU) {
        if (!pe_plat_cheats_dev_menu_enabled()) return 0;
        if (pressed) {
            C.menu = !C.menu; C.dev_page = C.menu ? 1 : 0;
            C.dev_index = 0; C.help = 0;
        }
        return 1;
    }
    if (key == PE_CHEAT_KEY_BACK) {
        if (!C.menu) return 0;
        if (pressed) {
            if (C.dev_page == 3) { C.dev_page = 2; C.dev_index = C.dev_day - 1; }
            else if (C.dev_page == 2) { C.dev_page = 1; C.dev_index = 5; }
            else if (option_row(C.dev_page) >= 0) { C.dev_index = option_row(C.dev_page); C.dev_page = 4; }
            else if (C.dev_page == 4) { C.dev_page = 1; C.dev_index = 6; }
            else if (C.dev_page == 6) { C.dev_page = 1; C.dev_index = 7; }
            else { C.menu = 0; C.dev_page = 0; }
        }
        return 1;
    }
    if (C.menu && C.dev_page &&
        (key == PE_CHEAT_KEY_UP || key == PE_CHEAT_KEY_DOWN || key == PE_CHEAT_KEY_ENTER)) {
        int n = dev_rows();
        if (!pressed) return 1;
        if (key == PE_CHEAT_KEY_UP && n) C.dev_index = (C.dev_index + n - 1) % n;
        else if (key == PE_CHEAT_KEY_DOWN && n) C.dev_index = (C.dev_index + 1) % n;
        else if (key == PE_CHEAT_KEY_ENTER) {
            if (C.dev_page == 1) {
                static const PeCheatKey actions[] = {PE_CHEAT_KEY_F2, PE_CHEAT_KEY_F3,
                    PE_CHEAT_KEY_F4, PE_CHEAT_KEY_F5, PE_CHEAT_KEY_F7};
                if (C.dev_index < 5) pe_plat_cheats_key(actions[C.dev_index], 1);
                else if (C.dev_index == 5) { C.dev_page = 2; C.dev_index = 0; }
                else if (C.dev_index == 6) { C.dev_page = 4; C.dev_index = 0; }
                else { C.dev_page = 6; C.dev_index = 0; }
            } else if (C.dev_page == 4) {
                C.dev_page = k_option_pages[C.dev_index].page; C.dev_index = 0;
            } else if (registry_page(C.dev_page)) {
                /* the registry pages list the option registry by category */
                int opt = pe_enh_category_option(page_category(C.dev_page), C.dev_index);
                if (opt >= 0) {
                    char m[64];
                    pe_enh_option_cycle(opt);
                    pe_enh_option_label(opt, m, sizeof m);
                    toast(m);
                }
            } else if (C.dev_page == 2) {
                C.dev_day = C.dev_index + 1;
                C.dev_page = 3; C.dev_index = 0;
            } else if (n) {
                int index = day_warp_index(C.dev_day, C.dev_index);
                if (index >= 0) { C.pending_warp = index + 1; C.menu = 0; C.dev_page = 0; }
            }
        }
        return 1;
    }
    if (C.menu && (key == PE_CHEAT_KEY_UP || key == PE_CHEAT_KEY_DOWN || key == PE_CHEAT_KEY_ENTER)) {
        if (pressed) {
            if (key == PE_CHEAT_KEY_UP) C.menu_index = (C.menu_index + N_WARPS - 1) % N_WARPS;
            else if (key == PE_CHEAT_KEY_DOWN) C.menu_index = (C.menu_index + 1) % N_WARPS;
            else { C.pending_warp = C.menu_index + 1; C.menu = 0; }
        }
        return 1;
    }
    if (key == PE_CHEAT_KEY_UP || key == PE_CHEAT_KEY_DOWN || key == PE_CHEAT_KEY_ENTER ||
        key == PE_CHEAT_KEY_NONE)
        return 0;
    if (!pressed) return 1;
    switch (key) {
    case PE_CHEAT_KEY_F1: C.help = !C.help; break;
    case PE_CHEAT_KEY_F2: C.god = !C.god; toast(C.god ? "GOD MODE ON" : "GOD MODE OFF"); break;
    case PE_CHEAT_KEY_F3:
        C.ammo = !C.ammo; C.ammo_gun = 0;
        toast(C.ammo ? "INFINITE AMMO + FULL PE ON" : "INFINITE AMMO + FULL PE OFF"); break;
    case PE_CHEAT_KEY_F4: C.pending_win = 1; toast("INSTANT WIN"); break;
    case PE_CHEAT_KEY_F5: C.pending_kit = 1; break;
    case PE_CHEAT_KEY_F7:
        C.ff_index = (C.ff_index + 1) & 3;
        if (k_ff_mult[C.ff_index]) snprintf(msg, sizeof msg, "FAST-FORWARD SPEED %dX", k_ff_mult[C.ff_index]);
        else snprintf(msg, sizeof msg, "FAST-FORWARD SPEED MAX");
        toast(msg); break;
    case PE_CHEAT_KEY_F8:
        C.menu = !C.menu;
        C.dev_page = C.menu && pe_plat_cheats_dev_menu_enabled() ? 1 : 0;
        if (C.dev_page) { C.dev_index = 0; C.help = 0; }
        break;
    default: break;
    }
    return 1;
}

/* ── game-state passes ─────────────────────────────────────────────── */
static int ram(pe_addr_t a, uint32_t n) { return a && PE_RangeIsRam(a, n); }

static void god_pass(void)
{
    pe_addr_t rec = PE_LoadU32(GA_AYA_REC);
    if (!ram(rec, 0x20u)) return;
    uint16_t max = PE_LoadU16(rec + 0x1Cu);
    if (max && PE_LoadU16(rec + 0x0Cu) != max) PE_StoreU16(rec + 0x0Cu, max);
}

static void ammo_pass(void)
{
    pe_addr_t rec = PE_LoadU32(GA_AYA_REC), actor, stats, gun;
    unsigned k;
    if (ram(rec, 0x70u) && ram(gun = PE_LoadU32(rec + 0x68u), 0x10u)) {
        uint32_t w = PE_LoadU32(gun + 0x0Cu), loaded = w & 0x3FFu;
        if (gun != C.ammo_gun) { C.ammo_gun = gun; C.ammo_loaded_max = loaded; }
        if (loaded > C.ammo_loaded_max) C.ammo_loaded_max = loaded;
        if (loaded < C.ammo_loaded_max)
            PE_StoreU32(gun + 0x0Cu, (w & ~0x3FFu) | C.ammo_loaded_max);
    }
    for (k = 0; k < 3u; k++) {
        uint16_t v = PE_LoadU16(0x800A1E6Eu + k * 32u);
        if (v > C.ammo_pool_max[k]) C.ammo_pool_max[k] = v;
        if (v < C.ammo_pool_max[k]) PE_StoreU16(0x800A1E6Eu + k * 32u, (uint16_t)C.ammo_pool_max[k]);
    }
    actor = PE_LoadU32(GA_AYA_ACTOR);
    if (ram(actor, 4u) && ram(stats = PE_LoadU32(actor), 0x2Cu)) {
        uint32_t max = PE_LoadU32(stats + 0x28u);
        if ((int32_t)max > 0 && PE_LoadU32(stats + 0x08u) != max) PE_StoreU32(stats + 0x08u, max);
    }
}

static int win_pass(void)
{
    pe_addr_t aya = PE_LoadU32(GA_AYA_ACTOR);
    unsigned i, n = 0;
    if (!(PE_LoadU32(GA_D1A0) & 2u)) return -1;          /* not in a battle */
    for (i = 0; i < 45u; i++) {
        pe_addr_t p = PE_LoadU32(GA_ENEMIES + i * 12u), body;
        if (!p) break;
        if (p == aya || !ram(p, 0x10u) || !ram(body = PE_LoadU32(p), 0x14u)) continue;
        if ((int32_t)PE_LoadU32(body + 0x10u) > 0) { PE_StoreU32(body + 0x10u, 0u); n++; }
    }
    return (int)n;
}

static int owned(uint32_t id)
{
    pe_addr_t inv = PE_LoadU32(GA_INV);
    uint32_t n = PE_LoadU32(GA_INV_COUNT), i;
    if (!ram(inv, 2u) || n > 512u) return 0;
    for (i = 0; i < n; i++) if (PE_LoadU16(inv + i * 2u) == id) return 1;
    return 0;
}

int pe_plat_cheats_area_kit(uint32_t lo, uint32_t hi)
{
    uint32_t a, seen[64]; int nseen = 0, added = 0;
    if (!func_80053D2C) return 0;
    for (a = lo & ~3u; a + 12u <= hi; a += 4u) {
        uint32_t h = PE_LoadU32(a), id;
        int dup = 0;
        /* op 0xA7, argc 2, arg0 mode 0 (immediate) */
        if ((h & 0x1FFFu) != 0xA7u || ((h >> 13) & 0xFu) != 2u || ((h >> 17) & 7u) != 0u) continue;
        id = PE_LoadU32(a + 8u);
        if (id < 0x100u || id >= 0x180u) continue;
        for (int i = 0; i < nseen; i++) if (seen[i] == id) dup = 1;
        if (dup) continue;
        if (nseen < 64) seen[nseen++] = id;
        if (owned(id)) continue;
        if (func_80053D2C((int)id) == 0) {
            added++;
            fprintf(stderr, "[CHEATS] area kit: added key item 0x%03X (script op at %08X)\n", id, a);
        }
    }
    return added;
}

/* The field state a warp may act in, from the main loop's own dispatch
 * (func_8001220C, bootstrap/func_8001220C_port.c): D_8009D280 reaches the
 * field room loop func_8003F3C4 unless it is the title state 0xA9400048
 * (title overlay func_801909B4: menu and attract movies), the New Game init
 * 0xAA108448, or 0xA8000048 (func_8019234C).  func_80017BB4 is a field-VM
 * op; outside that loop it has no room loop to act on (a warp picked on the
 * title latched story 0x5B into New Game init and froze it). */
#define TOK_TITLE      0xA9400048u
#define TOK_NEWGAME    0xAA108448u
#define TOK_A8000048   0xA8000048u
#define TOK_PROLOGUE   0xA80830C8u   /* New Game prologue room (FMV002) */

static int in_field(uint32_t tok)
{
    return tok != 0u && tok != TOK_TITLE && tok != TOK_NEWGAME && tok != TOK_A8000048;
}

/* 1 = transfer requested, 0 = wait (field running but busy),
 * -1 = no room_transfer linked, -2 = not in the field (refused). */
static int warp_to(uint32_t token, int set_story, uint32_t story,
                   int set_door, uint32_t door)
{
    uint32_t d1a0 = PE_LoadU32(GA_D1A0);
    uint32_t tok = PE_LoadU32(GA_TOKEN);
    if (!func_80017BB4) return -1;
    if (!in_field(tok)) return -2;
    if (PE_LoadU32(GA_ROOM_CUR) != tok) return 0;        /* room change in flight */
    if (d1a0 & (2u | 0x2000u)) return 0;                 /* battle / transfer pending */
    /* A field movie (New Game prologue FMV002 in A80830C8, field op 0x35)
     * owns the display while D_800B0DBA != 0: the field tick keeps calling
     * the movie overlay (func_80122040 reads its state at 0x801223F6..
     * 0x801228FA).  A room transfer now loads the destination room over that
     * overlay, and the next tick follows a garbage pointer (black screen /
     * PE_LoadU32 fatal).  Wait for func_80121A00 to clear the flag, and let
     * the prologue hand over to its first room before warping. */
    if (PE_LoadU8(GA_MOVIE) || tok == TOK_PROLOGUE) return 0;
    if (set_story) PE_StoreU32(GA_STORY, story);
    if (set_door) PE_StoreU32(GA_DOOR, door);
    PE_StoreU32(PE_CHEAT_STACK + 8u, token);
    PE_StoreU32(PE_CHEAT_STACK + 0u, PE_CHEAT_STACK + 8u);
    (void)func_80017BB4(PE_CHEAT_STACK);                 /* the field VM's room_transfer */
    return 1;
}

static int warp_pass(int index)
{
    return warp_to(k_warps[index].token, 1, k_warps[index].story,
                   k_warps[index].door != WARP_DOOR_KEEP, k_warps[index].door);
}

void pe_plat_cheats_apply(void)
{
    char msg[64];
    if (!C.enabled) return;
    if (C.god) god_pass();
    if (C.ammo) ammo_pass();
    if (C.pending_win) {
        int n = win_pass();
        if (n < 0) toast("INSTANT WIN: NOT IN A BATTLE");
        else { C.wins++; snprintf(msg, sizeof msg, "INSTANT WIN: %d ENEMIES", n); toast(msg); }
        C.pending_win = 0;
    }
    if (C.pending_kit) {
        int n = pe_plat_cheats_area_kit(0x80180000u, 0x801F0000u);
        C.kits++;
        snprintf(msg, sizeof msg, "AREA KIT: %d KEY ITEMS ADDED", n);
        toast(msg);
        C.pending_kit = 0;
    }
    if (C.arrive_token) {
        /* Arrived = the destination is the current room AND its field loop
         * has restarted (D_8009CDA4 counts ticks since the room load) and run
         * >= 30 ticks; the token alone is latched by the transfer request. */
        int presented = 0;
        uint32_t ticks = PE_LoadU32(0x8009CDA4u);
        HostFB_GetState(NULL, NULL, &presented, NULL);
        if (PE_LoadU32(GA_ROOM_CUR) == C.arrive_token && PE_LoadU32(GA_TOKEN) == C.arrive_token
            && (uint32_t)presented >= C.arrive_from + 60u && ticks >= 30u
            && ticks <= (uint32_t)presented - C.arrive_from) {
            fprintf(stderr, "[WARP] route arrived leg=%d token=%08X story=%08X frame=%d ticks=%u vm=%08X\n",
                    C.env_warp ? C.leg : C.nlegs, (unsigned)C.arrive_token,
                    (unsigned)PE_LoadU32(GA_STORY), presented, (unsigned)ticks,
                    (unsigned)PE_LoadU32(0x8009CE00u));
            C.arrive_token = 0u;
            if (C.env_warp) C.env_at = (uint32_t)presented + C.dwell;
        }
    }
    if (C.env_warp) {
        int presented = 0;
        HostFB_GetState(NULL, NULL, &presented, NULL);
        if ((uint32_t)presented >= C.env_at) {
            uint32_t tok = PE_LoadU32(GA_TOKEN);
            int r;
            /* PE_WARP_DISC=2: the player swapped to disc 2 before the warp
             * (disc-2-story rooms).  The tray inserts the --disc2-image and
             * retail's own mount (func_800698D4) finds PEDISC02.IDF; the
             * disc-swap screen then leaves D_800B0DCD = 2, as here. */
            if (in_field(tok) && C.env_disc == 2 && PE_Tray_ActiveNumber() != 2) {
                if (PE_Tray_InsertNow(2) && func_800698D4() == 0 &&
                    (PE_LoadU8(0x800B0DCDu) & 2u)) {
                    PE_StoreU8(0x800B0DCDu, 2u);
                    fprintf(stderr, "[WARP] disc 2 in the drive (mounted)\n");
                } else {
                    fprintf(stderr, "[WARP] disc 2 handoff unavailable\n");
                }
            }
            r = in_field(tok) ? warp_to(C.env_token, C.env_set_story, C.env_story, 0, 0) : 0;
            if (r > 0) {
                C.env_warp = 0;
                C.warps++;
                {   /* dev-only exploration aid: PE_WARP_PERSIST_FILE=<dump> loads a persist
                     * block (0x800A77F0, 0x200 words, as written by the route pilot's
                     * PE_D3_PERSIST_DUMP) after the warp, keeping the warp's own story
                     * (slot 74) and entrance (slot 1). Never used by the gates. */
                    const char *pf = getenv("PE_WARP_PERSIST_FILE");
                    FILE *f = pf && *pf ? fopen(pf, "rb") : NULL;
                    if (f) {
                        uint32_t w[0x200]; size_t n = fread(w, 4u, 0x200u, f);
                        uint32_t keep1 = PE_LoadU32(0x800A77F4u), keep74 = PE_LoadU32(GA_STORY);
                        fclose(f);
                        for (size_t k = 0; k < n; k++) PE_StoreU32(0x800A77F0u + (uint32_t)k * 4u, w[k]);
                        PE_StoreU32(0x800A77F4u, keep1); PE_StoreU32(GA_STORY, keep74);
                        fprintf(stderr, "[WARP] persist block loaded from %s (%u words)\n", pf, (unsigned)n);
                    }
                }
                fprintf(stderr, "[WARP] env transfer %08X -> %08X story=%08X frame=%d\n",
                        (unsigned)tok, (unsigned)C.env_token,
                        (unsigned)PE_LoadU32(GA_STORY), presented);
                if (C.nlegs) {
                    C.arrive_token = C.env_token;
                    C.arrive_from = (uint32_t)presented;
                    if (C.leg + 1 < C.nlegs) {      /* next leg waits for this arrival */
                        C.leg++;
                        C.env_token = C.legs[C.leg].token;
                        C.env_set_story = C.legs[C.leg].set_story;
                        C.env_story = C.legs[C.leg].story;
                        C.env_at = 0xFFFFFFFFu;
                        C.env_warp = 1;
                    }
                }
            } else if (r < 0) {
                C.env_warp = 0;
                fprintf(stderr, "[WARP] env transfer unavailable (%d)\n", r);
            }
        }
    }
    if (C.pending_warp) {
        int r = warp_pass(C.pending_warp - 1);
        if (r > 0) {
            C.warps++;
            snprintf(msg, sizeof msg, "WARP: %s", k_warps[C.pending_warp - 1].name);
            toast(msg);
            C.pending_warp = 0;
        } else if (r == -2) {
            /* Never carried across a mode change: the pick is dropped. */
            toast("WARP: ONLY IN THE FIELD");
            fprintf(stderr, "[CHEATS] warp refused: not in the field (token %08X)\n",
                    PE_LoadU32(GA_TOKEN));
            C.pending_warp = 0;
        } else if (r < 0) { toast("WARP UNAVAILABLE"); C.pending_warp = 0; }
        /* r == 0: wait (battle, room change or field movie in progress) */
    }
}

void pe_plat_cheats_frame(void)
{
    if (!C.inited) pe_plat_cheats_init();
    if (!C.enabled) return;
    C.frames++;
    {   /* Harness only: PE_WARP_TRACE=N logs the room loop every N presents
         * (D_8009CDA4 = field ticks, D_8009CE00 = field VM cursor). */
        static int every = -1;
        if (every < 0) every = getenv("PE_WARP_TRACE") ? atoi(getenv("PE_WARP_TRACE")) : 0;
        if (every > 0 && C.frames % every == 0)
            fprintf(stderr, "[CHEATS] trace present=%d token=%08X cur=%08X story=%08X d1a0=%08X ticks=%u vm=%08X\n",
                    C.frames, (unsigned)PE_LoadU32(GA_TOKEN), (unsigned)PE_LoadU32(GA_ROOM_CUR),
                    (unsigned)PE_LoadU32(GA_STORY), (unsigned)PE_LoadU32(GA_D1A0),
                    (unsigned)PE_LoadU32(0x8009CDA4u), (unsigned)PE_LoadU32(0x8009CE00u));
    }
    while (C.iscript < C.nscript && C.script[C.iscript].frame <= C.frames) {
        pe_plat_cheats_key(C.script[C.iscript].key, 1);
        pe_plat_cheats_key(C.script[C.iscript].key, 0);
        C.iscript++;
    }
    (void)pe_plat_mods_emit(PE_PLAT_EVENT_FRAME_TICK, (uint32_t)C.frames, 0u);
    if (C.toast_frames) C.toast_frames--;
}

/* ── overlay ───────────────────────────────────────────────────────── */
/* 5x7 font, ASCII 0x20..0x5F, one byte per column, bit 0 = top row. */
static const uint8_t k_font[64][5] = {
    {0,0,0,0,0},{0,0,0x5F,0,0},{0,7,0,7,0},{0x14,0x7F,0x14,0x7F,0x14},
    {0x24,0x2A,0x7F,0x2A,0x12},{0x23,0x13,8,0x64,0x62},{0x36,0x49,0x56,0x20,0x50},{0,8,7,3,0},
    {0,0x1C,0x22,0x41,0},{0,0x41,0x22,0x1C,0},{0x2A,0x1C,0x7F,0x1C,0x2A},{8,8,0x3E,8,8},
    {0,0x80,0x70,0x30,0},{8,8,8,8,8},{0,0,0x60,0x60,0},{0x20,0x10,8,4,2},
    {0x3E,0x51,0x49,0x45,0x3E},{0,0x42,0x7F,0x40,0},{0x72,0x49,0x49,0x49,0x46},{0x21,0x41,0x49,0x4D,0x33},
    {0x18,0x14,0x12,0x7F,0x10},{0x27,0x45,0x45,0x45,0x39},{0x3C,0x4A,0x49,0x49,0x31},{0x41,0x21,0x11,9,7},
    {0x36,0x49,0x49,0x49,0x36},{0x46,0x49,0x49,0x29,0x1E},{0,0,0x14,0,0},{0,0x40,0x34,0,0},
    {0,8,0x14,0x22,0x41},{0x14,0x14,0x14,0x14,0x14},{0,0x41,0x22,0x14,8},{2,1,0x59,9,6},
    {0x3E,0x41,0x5D,0x59,0x4E},{0x7C,0x12,0x11,0x12,0x7C},{0x7F,0x49,0x49,0x49,0x36},{0x3E,0x41,0x41,0x41,0x22},
    {0x7F,0x41,0x41,0x41,0x3E},{0x7F,0x49,0x49,0x49,0x41},{0x7F,9,9,9,1},{0x3E,0x41,0x41,0x51,0x73},
    {0x7F,8,8,8,0x7F},{0,0x41,0x7F,0x41,0},{0x20,0x40,0x41,0x3F,1},{0x7F,8,0x14,0x22,0x41},
    {0x7F,0x40,0x40,0x40,0x40},{0x7F,2,0x1C,2,0x7F},{0x7F,4,8,0x10,0x7F},{0x3E,0x41,0x41,0x41,0x3E},
    {0x7F,9,9,9,6},{0x3E,0x41,0x51,0x21,0x5E},{0x7F,9,0x19,0x29,0x46},{0x26,0x49,0x49,0x49,0x32},
    {3,1,0x7F,1,3},{0x3F,0x40,0x40,0x40,0x3F},{0x1F,0x20,0x40,0x20,0x1F},{0x3F,0x40,0x38,0x40,0x3F},
    {0x63,0x14,8,0x14,0x63},{3,4,0x78,4,3},{0x61,0x59,0x49,0x4D,0x43},{0,0x7F,0x41,0x41,0x41},
    {2,4,8,0x10,0x20},{0,0x41,0x41,0x41,0x7F},{4,2,1,2,4},{0x40,0x40,0x40,0x40,0x40},
};

static uint8_t *g_ov;
static int g_ov_w, g_ov_h;

static void put_px(int x, int y, uint8_t r, uint8_t g, uint8_t b)
{
    if (x < 0 || y < 0 || x >= g_ov_w || y >= g_ov_h) return;
    uint8_t *p = g_ov + ((size_t)y * (size_t)g_ov_w + (size_t)x) * 3u;
    p[0] = r; p[1] = g; p[2] = b;
}

static void draw_text(int x, int y, const char *s, uint8_t r, uint8_t g, uint8_t b)
{
    for (; *s; s++, x += 6) {
        int c = (unsigned char)*s;
        if (c >= 'a' && c <= 'z') c -= 32;
        if (c < 0x20 || c > 0x5F) c = '?';
        for (int col = 0; col < 5; col++) {
            uint8_t bits = k_font[c - 0x20][col];
            for (int row = 0; row < 8; row++) {
                if (bits & (1u << row)) put_px(x + col, y + row, r, g, b);
                else if (row < 7) put_px(x + col, y + row, 0, 0, 0);   /* legibility */
            }
        }
        for (int row = 0; row < 7; row++) put_px(x + 5, y + row, 0, 0, 0);
    }
}

const uint8_t *pe_plat_cheats_overlay(const uint8_t *rgb, int w, int h)
{
    int any_active, visible, y;
    static const char *const help[] = {
        "CHEATS (OPT-IN)", "F1 THIS HELP", "F2 GOD MODE", "F3 INF AMMO + FULL PE",
        "F4 INSTANT WIN", "F5 AREA KEY ITEMS", "F6 FAST-FORWARD", "F7 FF SPEED 2/4/8/MAX",
        "F8 WARP MENU", NULL};
    if (!C.enabled || !rgb || w <= 0 || h <= 0) return rgb;
    any_active = C.god || C.ammo || C.wins || C.kits || C.warps || C.ff_index;
    visible = any_active || C.help || C.menu || C.toast_frames;
    if (!visible) return rgb;
    if (!g_ov || g_ov_w != w || g_ov_h != h) {
        free(g_ov);
        g_ov = (uint8_t *)malloc((size_t)w * (size_t)h * 3u);
        if (!g_ov) return rgb;
        g_ov_w = w; g_ov_h = h;
    }
    memcpy(g_ov, rgb, (size_t)w * (size_t)h * 3u);
    if (any_active) draw_text(w - 6 * 6 - 2, 2, "CHEATS", 255, 220, 0);
    y = 2;
    if (C.help) {
        for (int i = 0; help[i]; i++, y += 9) draw_text(4, y, help[i], 255, 255, 255);
        y += 4;
    }
    if (C.menu && C.dev_page) {
        static const char *const rows[] = {"GOD MODE", "INF AMMO + FULL PE",
            "INSTANT WIN", "AREA KEY ITEMS", "FAST-FORWARD SPEED", "WARP BY DAY", "OPTIONS", "ASSISTS"};
        int n = dev_rows();
        char label[64];
        draw_text(4, y, "DEV MENU: A/ENTER OK, B/ESC BACK", 120, 220, 255); y += 10;
        for (int i = 0; i < n; i++, y += 9) {
            int sel = i == C.dev_index;
            if (C.dev_page == 1 && i < 2)
                snprintf(label, sizeof label, "%s: %s", rows[i], (i ? C.ammo : C.god) ? "ON" : "OFF");
            else if (C.dev_page == 1 && i == 4) {
                if (k_ff_mult[C.ff_index]) snprintf(label, sizeof label, "FAST-FORWARD SPEED: %dX", k_ff_mult[C.ff_index]);
                else snprintf(label, sizeof label, "FAST-FORWARD SPEED: MAX");
            } else if (C.dev_page == 1) snprintf(label, sizeof label, "%s", rows[i]);
            else if (C.dev_page == 4) snprintf(label, sizeof label, "%s", k_option_pages[i].label);
            else if (registry_page(C.dev_page))
                pe_enh_option_label(pe_enh_category_option(page_category(C.dev_page), i),
                                    label, sizeof label);
            else if (C.dev_page == 2) snprintf(label, sizeof label, "DAY %d (%d WARPS)", i + 1, day_warp_count(i + 1));
            else snprintf(label, sizeof label, "%s", k_warps[day_warp_index(C.dev_day, i)].name);
            if (sel) draw_text(4, y, ">", 255, 255, 0);
            draw_text(12, y, label, sel ? 255 : 170, sel ? 255 : 170, sel ? 0 : 170);
        }
        if (!n) draw_text(12, y, "NO VERIFIED WARPS FOR THIS DAY", 170, 170, 170);
    } else if (C.menu) {
        /* The list (all days) is taller than the screen: scroll it so the
         * cursor row stays visible above the toast line. */
        int fit, first = 0;
        draw_text(4, y, "WARP (UP/DOWN, ENTER, F8 CLOSES)", 120, 220, 255); y += 10;
        fit = (h - 14 - y) / 9;
        if (fit < 1) fit = 1;
        if (C.menu_index >= fit) first = C.menu_index - fit + 1;
        for (int i = first; i < N_WARPS && i < first + fit; i++, y += 9) {
            int sel = i == C.menu_index;
            if (sel) draw_text(4, y, ">", 255, 255, 0);
            draw_text(12, y, k_warps[i].name, sel ? 255 : 170, sel ? 255 : 170, sel ? 0 : 170);
        }
    }
    if (C.toast_frames) draw_text(4, h - 12, C.toast, 255, 255, 0);
    return g_ov;
}
