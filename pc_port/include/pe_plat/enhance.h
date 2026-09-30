/* Opt-in gameplay enhancements (Options > Enhancements in the dev menu).
 *
 * Movement speed scales ONLY Aya's player walk/run velocity in field scenes
 * (not battle, NPCs, scripted moves, cutscenes, timers or RNG).  "Always run"
 * makes the field walk handler take the run path.  Both default off: at the
 * default (100 %, always-run off) the hooks return their input untouched and
 * write no guest state, so the game runs the retail code path unchanged.
 *
 * Settings persist in $XDG_CONFIG_HOME/parasite-eve-port/enhancements.conf
 * (~/.config/... when XDG_CONFIG_HOME is unset) as `move_speed=<pct>` and
 * `always_run=<0|1>` lines.  Overrides:
 *   PE_ENH_CONFIG=<path>  config file to read and write instead (tests use it)
 *   PE_MOVE_SPEED=<pct>   100, 125, 150, 175 or 200 (not written back)
 *   PE_ALWAYS_RUN=<0|1>   (not written back)
 *
 * Assists (dev menu: ASSISTS) live in the same file and default off:
 *   one_hit_kills=<0|1>  PE_ONE_HIT_KILLS=<0|1>   every hit Aya lands is lethal
 *   god_mode=<0|1>       PE_GOD_MODE=<0|1>        Aya's HP never goes down
 *   exp_mult=<1|2|3>     PE_EXP_MULT=<1|2|3>      battle EXP and BP times N
 * At the defaults every assist hook returns its input and writes nothing.
 */
#ifndef PE_PLAT_ENHANCE_H
#define PE_PLAT_ENHANCE_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define PE_ENH_MOVE_SPEED_COUNT 5
extern const int pe_enh_move_speed_steps[PE_ENH_MOVE_SPEED_COUNT]; /* 100..200 */

/* Loads the config once (idempotent). */
void pe_enh_init(void);
/* Test hook: forget loaded state so the next query re-reads config/env. */
void pe_enh_reset(void);

int  pe_enh_move_speed_pct(void);          /* 100 (default) .. 200 */
int  pe_enh_always_run(void);              /* 0 (default) or 1 */
/* Menu setters: clamp to a listed step and persist to the config file. */
void pe_enh_set_move_speed_pct(int pct);
void pe_enh_cycle_move_speed(void);        /* 100 -> 125 -> ... -> 200 -> 100 */
void pe_enh_set_always_run(int on);

/* Game-side hooks (called from the field walk/run handlers).  `actor` is the
 * guest actor address.  They only act when the actor is Aya (*0x8009D254) and
 * the game is not in battle (D_8009D1A0 & 2 clear). */
int32_t pe_enh_field_speed(uint32_t actor, int32_t speed);  /* speed * pct/100 */
int  pe_enh_field_always_run(uint32_t actor);

/* ── Assists ─────────────────────────────────────────────────────────── */
#define PE_ENH_EXP_MULT_MAX 3
int  pe_enh_one_hit_kills(void);       /* 0 (default) or 1 */
int  pe_enh_god_mode(void);            /* 0 (default) or 1 */
int  pe_enh_exp_mult(void);            /* 1 (default), 2 or 3 */
void pe_enh_set_one_hit_kills(int on);
void pe_enh_set_god_mode(int on);
void pe_enh_set_exp_mult(int mult);    /* clamped to 1..PE_ENH_EXP_MULT_MAX */
void pe_enh_cycle_exp_mult(void);      /* 1 -> 2 -> 3 -> 1 */

/* Game-side assist hooks.
 * pe_enh_enemy_damage: the battle damage routine (func_80028574) passes the
 *   final retail damage just before subtracting it from the enemy's HP
 *   (body+0x10).  With 1-hit kills on it returns max(damage, HP) so the HP
 *   reaches exactly 0 and retail's own death, EXP, BP and item-drop path runs.
 * pe_enh_player_shielded: 1 while god mode is on.  The player damage paths
 *   (func_8001F4D4 enemy hits, the contact-damage tick in func_8001D340, the
 *   HP-halving / HP-to-1 status effects in func_80020288) skip only their HP
 *   store; hit reactions, flashes, damage numbers and every other effect run.
 * pe_enh_exp_award / pe_enh_bp_award: the per-kill EXP and BP that
 *   func_80028E94 adds to the battle totals (D_8009D304 / D_8009D21C), times
 *   the multiplier; the level-up logic that consumes the totals is untouched. */
int32_t  pe_enh_enemy_damage(uint32_t body, int32_t damage);
int      pe_enh_player_shielded(void);
uint32_t pe_enh_exp_award(uint32_t exp);
uint32_t pe_enh_bp_award(uint32_t bp);

#ifdef __cplusplus
}
#endif

#endif /* PE_PLAT_ENHANCE_H */
