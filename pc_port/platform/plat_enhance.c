/* Opt-in gameplay enhancements: Aya's field movement speed and "Always run".
 * See pe_plat/enhance.h.  At the defaults the hooks are pure pass-throughs. */
#include "pe_plat/enhance.h"
#include "pe_guest_ram.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>

const int pe_enh_move_speed_steps[PE_ENH_MOVE_SPEED_COUNT] = {100, 125, 150, 175, 200};
static const int k_toggle[] = {0, 1};
static const int k_mult[]   = {1, 2, 3};

/* The registry.  Order within a category is the menu order. */
enum { OPT_MOVE_SPEED, OPT_ALWAYS_RUN, OPT_ONE_HIT, OPT_GOD, OPT_EXP_MULT, OPT_COUNT };
static const PeEnhOption k_options[OPT_COUNT] = {
    {"move_speed",    PE_ENH_CAT_GAMEPLAY, "MOVEMENT SPEED",       PE_ENH_FMT_PERCENT,
        pe_enh_move_speed_steps, PE_ENH_MOVE_SPEED_COUNT, "PE_MOVE_SPEED",    "u/stellarola"},
    {"always_run",    PE_ENH_CAT_GAMEPLAY, "ALWAYS RUN",           PE_ENH_FMT_TOGGLE,
        k_toggle, 2, "PE_ALWAYS_RUN",    "u/stellarola"},
    {"one_hit_kills", PE_ENH_CAT_ASSIST,   "1-HIT KILLS",          PE_ENH_FMT_TOGGLE,
        k_toggle, 2, "PE_ONE_HIT_KILLS", NULL},
    {"god_mode",      PE_ENH_CAT_ASSIST,   "GOD MODE (NO DAMAGE)", PE_ENH_FMT_TOGGLE,
        k_toggle, 2, "PE_GOD_MODE",      "u/SandmansSlave"},
    {"exp_mult",      PE_ENH_CAT_ASSIST,   "EXP/BP MULTIPLIER",    PE_ENH_FMT_MULT,
        k_mult, 3, "PE_EXP_MULT",       "u/SandmansSlave"},
};

#define GA_AYA_ACTOR  0x8009D254u   /* *(u32 *) = Aya's field actor */
#define GA_GAME_FLAGS 0x8009D1A0u   /* bit 1 = battle */

static struct {
    int inited;
    int value[OPT_COUNT];
} E;

/* Snap to a listed value: the largest listed value <= v, else the retail one. */
static int snap(int i, int v)
{
    const PeEnhOption *o = &k_options[i];
    int best = o->values[0];
    for (int k = 0; k < o->nvalues; k++)
        if (o->values[k] <= v && o->values[k] >= best) best = o->values[k];
    if (o->format == PE_ENH_FMT_TOGGLE) return v != 0;
    return best;
}

static int config_path(char *out, size_t n)
{
    const char *p = getenv("PE_ENH_CONFIG");
    const char *x;
    if (p && *p) return snprintf(out, n, "%s", p) < (int)n;
    x = getenv("XDG_CONFIG_HOME");
    if (x && *x) return snprintf(out, n, "%s/parasite-eve-port/enhancements.conf", x) < (int)n;
    x = getenv("HOME");
    if (x && *x) return snprintf(out, n, "%s/.config/parasite-eve-port/enhancements.conf", x) < (int)n;
    return 0;
}

static void load_config(void)
{
    char path[1024], line[128];
    FILE *f;
    if (!config_path(path, sizeof path)) return;
    f = fopen(path, "r");
    if (!f) return;
    while (fgets(line, sizeof line, f)) {
        char *eq = strchr(line, '=');
        if (!eq || line[0] == '#') continue;
        *eq = 0;
        for (int i = 0; i < OPT_COUNT; i++)
            if (strcmp(line, k_options[i].id) == 0) { E.value[i] = snap(i, atoi(eq + 1)); break; }
    }
    fclose(f);
}

static void mkdir_parent(const char *path)
{
    char dir[1024];
    char *s;
    snprintf(dir, sizeof dir, "%s", path);
    s = strrchr(dir, '/');
    if (!s || s == dir) return;
    *s = 0;
    for (char *q = dir + 1; *q; q++) {
        if (*q != '/') continue;
        *q = 0; (void)mkdir(dir, 0700); *q = '/';
    }
    (void)mkdir(dir, 0700);
}

static void save_config(void)
{
    char path[1024];
    FILE *f;
    if (!config_path(path, sizeof path)) return;
    mkdir_parent(path);
    f = fopen(path, "w");
    if (!f) {
        fprintf(stderr, "[ENH] cannot write %s: %s\n", path, strerror(errno));
        return;
    }
    fprintf(f, "# Parasite Eve port enhancements and assists (dev menu > Options / Assists)\n");
    fprintf(f, "# Every value's default is the retail behaviour; delete this file to reset.\n");
    for (int i = 0; i < OPT_COUNT; i++)
        fprintf(f, "%s=%d\n", k_options[i].id, E.value[i]);
    fclose(f);
}

void pe_enh_init(void)
{
    int any = 0;
    if (E.inited) return;
    E.inited = 1;
    for (int i = 0; i < OPT_COUNT; i++) E.value[i] = k_options[i].values[0];
    load_config();
    for (int i = 0; i < OPT_COUNT; i++) {
        const char *e = getenv(k_options[i].env);
        if (e && *e) E.value[i] = snap(i, atoi(e));
        if (E.value[i] != k_options[i].values[0]) any = 1;
    }
    if (any) {
        fprintf(stderr, "[ENH]");
        for (int i = 0; i < OPT_COUNT; i++)
            if (E.value[i] != k_options[i].values[0]) fprintf(stderr, " %s=%d", k_options[i].id, E.value[i]);
        fprintf(stderr, "\n");
    }
}

void pe_enh_reset(void) { memset(&E, 0, sizeof E); }

/* ── registry access ─────────────────────────────────────────────────── */
int pe_enh_option_count(void) { return OPT_COUNT; }
const PeEnhOption *pe_enh_option(int i) { return (i >= 0 && i < OPT_COUNT) ? &k_options[i] : NULL; }
int pe_enh_option_index(const char *id)
{
    for (int i = 0; id && i < OPT_COUNT; i++) if (strcmp(id, k_options[i].id) == 0) return i;
    return -1;
}
int pe_enh_category_count(PeEnhCategory cat)
{
    int n = 0;
    for (int i = 0; i < OPT_COUNT; i++) if (k_options[i].category == cat) n++;
    return n;
}
int pe_enh_category_option(PeEnhCategory cat, int n)
{
    for (int i = 0; i < OPT_COUNT; i++) if (k_options[i].category == cat && n-- == 0) return i;
    return -1;
}
int pe_enh_option_value(int i)
{
    pe_enh_init();
    return (i >= 0 && i < OPT_COUNT) ? E.value[i] : 0;
}
int pe_enh_option_is_retail(int i)
{
    return i >= 0 && i < OPT_COUNT && pe_enh_option_value(i) == k_options[i].values[0];
}
void pe_enh_option_set(int i, int value)
{
    if (i < 0 || i >= OPT_COUNT) return;
    pe_enh_init();
    E.value[i] = snap(i, value);
    save_config();
}
void pe_enh_option_cycle(int i)
{
    const PeEnhOption *o = pe_enh_option(i);
    int k;
    if (!o) return;
    pe_enh_init();
    for (k = 0; k < o->nvalues; k++) if (o->values[k] == E.value[i]) break;
    pe_enh_option_set(i, o->values[(k + 1) % o->nvalues]);
}
void pe_enh_option_label(int i, char *buf, size_t n)
{
    const PeEnhOption *o = pe_enh_option(i);
    int v;
    if (!o) { if (n) buf[0] = 0; return; }
    v = pe_enh_option_value(i);
    switch (o->format) {
    case PE_ENH_FMT_TOGGLE:  snprintf(buf, n, "%s: %s", o->label, v ? "ON" : "OFF"); break;
    case PE_ENH_FMT_PERCENT: snprintf(buf, n, "%s: %d%%%s", o->label, v, pe_enh_option_is_retail(i) ? " (RETAIL)" : ""); break;
    default:                 snprintf(buf, n, "%s: %dX%s", o->label, v, pe_enh_option_is_retail(i) ? " (RETAIL)" : ""); break;
    }
}

/* ── named accessors (thin wrappers over the registry) ───────────────── */
int  pe_enh_move_speed_pct(void)          { return pe_enh_option_value(OPT_MOVE_SPEED); }
int  pe_enh_always_run(void)              { return pe_enh_option_value(OPT_ALWAYS_RUN); }
void pe_enh_set_move_speed_pct(int pct)   { pe_enh_option_set(OPT_MOVE_SPEED, pct); }
void pe_enh_cycle_move_speed(void)        { pe_enh_option_cycle(OPT_MOVE_SPEED); }
void pe_enh_set_always_run(int on)        { pe_enh_option_set(OPT_ALWAYS_RUN, on); }
int  pe_enh_one_hit_kills(void)           { return pe_enh_option_value(OPT_ONE_HIT); }
int  pe_enh_god_mode(void)                { return pe_enh_option_value(OPT_GOD); }
int  pe_enh_exp_mult(void)                { return pe_enh_option_value(OPT_EXP_MULT); }
void pe_enh_set_one_hit_kills(int on)     { pe_enh_option_set(OPT_ONE_HIT, on); }
void pe_enh_set_god_mode(int on)          { pe_enh_option_set(OPT_GOD, on); }
void pe_enh_set_exp_mult(int mult)        { pe_enh_option_set(OPT_EXP_MULT, mult); }
void pe_enh_cycle_exp_mult(void)          { pe_enh_option_cycle(OPT_EXP_MULT); }

static int aya_in_field(uint32_t actor)
{
    return actor != 0u && actor == PE_LoadU32(GA_AYA_ACTOR)
        && (PE_LoadU32(GA_GAME_FLAGS) & 2u) == 0u;
}

int32_t pe_enh_field_speed(uint32_t actor, int32_t speed)
{
    int pct = pe_enh_move_speed_pct();
    if (pct == 100 || !aya_in_field(actor)) return speed;   /* retail path */
    return (int32_t)(((int64_t)speed * pct) / 100);
}

int pe_enh_field_always_run(uint32_t actor)
{
    return pe_enh_always_run() && aya_in_field(actor);
}

/* ── Assist hooks ────────────────────────────────────────────────────── */
int32_t pe_enh_enemy_damage(uint32_t body, int32_t damage)
{
    int32_t hp;
    if (!pe_enh_one_hit_kills() || body == 0u) return damage;    /* retail path */
    /* Story-scripted battles (game flag 0x800: the Eve stage fight on Day 1) are ended by
     * their script, not by the enemy's HP; killing the enemy there leaves the script
     * waiting forever (observed on the Day-1 autopilot).  The assist stays inactive. */
    if (PE_LoadU32(GA_GAME_FLAGS) & 0x800u) return damage;
    hp = (int32_t)PE_LoadU32(body + 0x10u);
    /* Scripts also make an enemy invulnerable for a scripted phase by adding 1000000
     * to its HP (rooms test `HP <= 1000000`, e.g. room_m0429i func_801926B4).  Killing
     * it there skips the script's own exit: the Day-4 m0156i roof fight then sat in
     * op_C3 forever with "NO TARGET".  Retail damage while the marker is set. */
    if (hp > 1000000) return damage;
    return (hp > 0 && damage < hp) ? hp : damage;
}

int pe_enh_player_shielded(void) { return pe_enh_god_mode(); }

uint32_t pe_enh_exp_award(uint32_t exp)
{
    int m = pe_enh_exp_mult();
    return m == 1 ? exp : exp * (uint32_t)m;
}

uint32_t pe_enh_bp_award(uint32_t bp)
{
    int m = pe_enh_exp_mult();
    return m == 1 ? bp : bp * (uint32_t)m;
}
