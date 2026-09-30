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

#ifdef __cplusplus
}
#endif

#endif /* PE_PLAT_ENHANCE_H */
