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

#define GA_AYA_ACTOR  0x8009D254u   /* *(u32 *) = Aya's field actor */
#define GA_GAME_FLAGS 0x8009D1A0u   /* bit 1 = battle */

static struct {
    int inited;
    int move_pct;
    int always_run;
    int one_hit, god, exp_mult;                     /* assists */
    int env_speed, env_run;   /* set when an env override pinned the value */
} E;

static int clamp_mult(int m) { return m < 1 ? 1 : m > PE_ENH_EXP_MULT_MAX ? PE_ENH_EXP_MULT_MAX : m; }

static int clamp_step(int pct)
{
    int best = 100;
    for (int i = 0; i < PE_ENH_MOVE_SPEED_COUNT; i++)
        if (pe_enh_move_speed_steps[i] <= pct) best = pe_enh_move_speed_steps[i];
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
        int v;
        if (sscanf(line, "move_speed=%d", &v) == 1) E.move_pct = clamp_step(v);
        else if (sscanf(line, "always_run=%d", &v) == 1) E.always_run = v != 0;
        else if (sscanf(line, "one_hit_kills=%d", &v) == 1) E.one_hit = v != 0;
        else if (sscanf(line, "god_mode=%d", &v) == 1) E.god = v != 0;
        else if (sscanf(line, "exp_mult=%d", &v) == 1) E.exp_mult = clamp_mult(v);
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
    fprintf(f, "# Parasite Eve port enhancements (Options > Enhancements)\n");
    fprintf(f, "move_speed=%d\nalways_run=%d\n", E.move_pct, E.always_run);
    fprintf(f, "# Assists (dev menu > Assists)\n");
    fprintf(f, "one_hit_kills=%d\ngod_mode=%d\nexp_mult=%d\n", E.one_hit, E.god, E.exp_mult);
    fclose(f);
}

void pe_enh_init(void)
{
    const char *e;
    if (E.inited) return;
    E.inited = 1;
    E.move_pct = 100;
    E.always_run = 0;
    E.one_hit = 0; E.god = 0; E.exp_mult = 1;
    load_config();
    e = getenv("PE_MOVE_SPEED");
    if (e && *e) { E.move_pct = clamp_step(atoi(e)); E.env_speed = 1; }
    e = getenv("PE_ALWAYS_RUN");
    if (e && *e) { E.always_run = e[0] != '0'; E.env_run = 1; }
    e = getenv("PE_ONE_HIT_KILLS");
    if (e && *e) E.one_hit = e[0] != '0';
    e = getenv("PE_GOD_MODE");
    if (e && *e) E.god = e[0] != '0';
    e = getenv("PE_EXP_MULT");
    if (e && *e) E.exp_mult = clamp_mult(atoi(e));
    if (E.move_pct != 100 || E.always_run)
        fprintf(stderr, "[ENH] movement speed %d%%, always run %s\n",
                E.move_pct, E.always_run ? "on" : "off");
    if (E.one_hit || E.god || E.exp_mult != 1)
        fprintf(stderr, "[ENH] assists: 1-hit kills %s, god mode %s, EXP/BP x%d\n",
                E.one_hit ? "on" : "off", E.god ? "on" : "off", E.exp_mult);
}

void pe_enh_reset(void) { memset(&E, 0, sizeof E); }

int pe_enh_move_speed_pct(void) { pe_enh_init(); return E.move_pct; }
int pe_enh_always_run(void) { pe_enh_init(); return E.always_run; }

void pe_enh_set_move_speed_pct(int pct)
{
    pe_enh_init();
    E.move_pct = clamp_step(pct);
    save_config();
}

void pe_enh_cycle_move_speed(void)
{
    int i;
    pe_enh_init();
    for (i = 0; i < PE_ENH_MOVE_SPEED_COUNT; i++)
        if (pe_enh_move_speed_steps[i] == E.move_pct) break;
    pe_enh_set_move_speed_pct(pe_enh_move_speed_steps[(i + 1) % PE_ENH_MOVE_SPEED_COUNT]);
}

void pe_enh_set_always_run(int on)
{
    pe_enh_init();
    E.always_run = on != 0;
    save_config();
}

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

/* ── Assists ─────────────────────────────────────────────────────────── */
int pe_enh_one_hit_kills(void) { pe_enh_init(); return E.one_hit; }
int pe_enh_god_mode(void)      { pe_enh_init(); return E.god; }
int pe_enh_exp_mult(void)      { pe_enh_init(); return E.exp_mult; }

void pe_enh_set_one_hit_kills(int on) { pe_enh_init(); E.one_hit = on != 0; save_config(); }
void pe_enh_set_god_mode(int on)      { pe_enh_init(); E.god = on != 0; save_config(); }
void pe_enh_set_exp_mult(int mult)    { pe_enh_init(); E.exp_mult = clamp_mult(mult); save_config(); }
void pe_enh_cycle_exp_mult(void)
{
    pe_enh_init();
    pe_enh_set_exp_mult(E.exp_mult >= PE_ENH_EXP_MULT_MAX ? 1 : E.exp_mult + 1);
}

int32_t pe_enh_enemy_damage(uint32_t body, int32_t damage)
{
    int32_t hp;
    if (!pe_enh_one_hit_kills() || body == 0u) return damage;    /* retail path */
    hp = (int32_t)PE_LoadU32(body + 0x10u);
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
