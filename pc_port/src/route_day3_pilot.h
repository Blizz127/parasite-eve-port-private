/* Day-3 route pilot (harness-only: --route-pad with PE_PAD_SCRIPT and
 * PE_ROUTE_WAYPOINTS; never active in a normal start).
 *
 * After the hand-over the pad script supplies the text/menu pad (Cross /
 * Circle). This pilot overrides it when there is something to do:
 *   * a battle is live (0x8009D1A0 & 2): RouteBattleShim (shared Day-1 battle
 *     aid: HP lock + keep range + Cross on full ATB / default target);
 *   * field control (mode 0) in a room with waypoints: walk to the next
 *     waypoint with the learned pad->world map (RouteM34ShimMove).
 * It only emits controller buttons, apart from the battle aid's HP lock.
 *
 * PE_ROUTE_WAYPOINTS = entry|entry|...
 *   entry  = TOKEN[@MINSTORY][#VISIT]=wp;wp;...  (hex token, hex story,
 *            VISIT: only on the VISIT-th entry into that room, 1-based)
 *   wp     = X,Z[,R][!N]                      (world units; R arrive radius,
 *            default 100; !N: on arrival press Cross for N frames)
 * The entry used is the one for the current token with the highest MINSTORY
 * <= story, a matching #VISIT entry winning over one without. Its waypoints run in order; after the last one the pilot holds
 * still (the pad script resumes) until the room or story changes.
 *
 * World map (token A8000048): the entry's first wp is TARGET,CONFIRM[,STEP]
 * where TARGET is the location index (0x8019CC52), CONFIRM an active-low
 * pad mask (0 = Cross) and STEP the active-low mask that steps the selection
 * (default 0xFF7F, internal edge 0x40 = next location).  The pilot pulses Down until the selection matches, then
 * pulses CONFIRM (first press previews, second leaves). */
#ifndef PC_PORT_ROUTE_DAY3_PILOT_H
#define PC_PORT_ROUTE_DAY3_PILOT_H

typedef struct { int x, z, r, act; } D3Wp;
typedef struct { uint32_t token, story; int visit, n; D3Wp wp[24]; } D3Entry;
static D3Entry g_d3[160];
static int g_d3_n = -1;

static void D3Parse(void)
{
    const char *s = getenv("PE_ROUTE_WAYPOINTS");
    g_d3_n = 0;
    while (s && *s && g_d3_n < (int)(sizeof g_d3 / sizeof g_d3[0])) {
        D3Entry *e = &g_d3[g_d3_n];
        char *end;
        memset(e, 0, sizeof *e);
        e->token = (uint32_t)strtoul(s, &end, 16); s = end;
        if (*s == '@') { e->story = (uint32_t)strtoul(s + 1, &end, 16); s = end; }
        if (*s == '#') { e->visit = (int)strtol(s + 1, &end, 10); s = end; }
        if (*s != '=') break;
        s++;
        while (*s && *s != '|' && e->n < 24) {
            D3Wp *w = &e->wp[e->n];
            w->x = (int)strtol(s, &end, 10); s = end;
            if (*s != ',') break;
            w->z = (int)strtol(s + 1, &end, 10); s = end;
            w->r = 100;
            if (*s == ',') { w->r = (int)strtol(s + 1, &end, 10); s = end; }
            if (*s == '!') { w->act = (int)strtol(s + 1, &end, 10); s = end; }
            e->n++;
            if (*s == ';') s++;
        }
        g_d3_n++;
        while (*s && *s != '|') s++;
        if (*s == '|') s++;
    }
    fprintf(stderr, "[D3PILOT] %d waypoint entries\n", g_d3_n);
}

static uint16_t RouteDay3Pilot(uint16_t script_mask)
{
    static uint32_t cur_tok = 0, cur_story = 0xFFFFFFFFu;
    static int ent = -1, idx = 0, act_until = 0, last_log = 0, done_logged = 0;
    uint32_t tok = (uint32_t)GA_TOKEN, story = PE_LoadU32(0x800A7918u);
    unsigned mode = PE_LoadU32(0x8009D28Cu), d1a0 = PE_LoadU32(0x8009D1A0u);
    pe_addr_t aya = PE_LoadU32(0x8009D254u);
    int ax, az;

    if (g_d3_n < 0) D3Parse();
    if (!g_d3_n) return script_mask;
    {   /* the Day-2 gate's text pad, unbounded: Cross, release, Circle, release (40f each) */
        static const uint16_t seq[4] = {0xBFFFu, 0xFFFFu, 0xDFFFu, 0xFFFFu};
        script_mask = seq[((g_frame - g_hand_over_frame) / 40) % 4];
    }
    if (d1a0 & 2u) return RouteBattleShim();
    if (tok != cur_tok || story != cur_story) {
        static uint32_t vtok[64]; static int vcnt[64], nv;
        int best = -1, visit = 0, k;
        if (tok != cur_tok) {
            for (k = 0; k < nv && vtok[k] != tok; k++) ;
            if (k == nv && nv < 64) { vtok[nv] = tok; vcnt[nv++] = 0; }
            if (k < 64) vcnt[k]++;
        }
        for (k = 0; k < nv; k++) if (vtok[k] == tok) visit = vcnt[k];
        for (int i = 0; i < g_d3_n; i++) {
            if (g_d3[i].token != tok || g_d3[i].story > story) continue;
            if (g_d3[i].visit && g_d3[i].visit != visit) continue;
            if (best < 0 || (g_d3[i].visit && !g_d3[best].visit) ||
                (!g_d3[i].visit == !g_d3[best].visit && g_d3[i].story >= g_d3[best].story)) best = i;
        }
        if (tok != cur_tok || best != ent) {
            ent = best; idx = 0; act_until = 0; done_logged = 0;
            g_m34s_init = 0;              /* new room: reseed the pad map */
            fprintf(stderr, "[D3PILOT] frame=%d token=%08X story=%X visit=%d entry=%d\n",
                    g_frame, (unsigned)tok, (unsigned)story, visit, ent);
        }
        cur_tok = tok; cur_story = story;
    }
    if (g_frame < act_until) return (g_frame % 8) == 3 ? 0xBFFFu : 0xFFFFu;
    if (tok == 0xA8000048u && ent >= 0 && g_d3[ent].n > 0) {
        int target = g_d3[ent].wp[0].x, cur = (int16_t)PE_LoadU16(0x8019CC52u);
        uint16_t confirm = g_d3[ent].wp[0].z ? (uint16_t)g_d3[ent].wp[0].z : 0xBFFFu;
        if (g_frame - last_log >= 60) {
            last_log = g_frame;
            fprintf(stderr, "[D3PILOT] frame=%d map sel=%d target=%d c024=%d c026=%d c034=%d c044=%d c045=%d bff8=%08X f4=%08X\n",
                    g_frame, cur, target, (int16_t)PE_LoadU16(0x8019C024u), (int16_t)PE_LoadU16(0x8019C026u),
                    (int16_t)PE_LoadU16(0x8019C034u), PE_LoadU8(0x8019C044u), PE_LoadU8(0x8019C045u),
                    (unsigned)PE_LoadU32(0x8019BFF8u), (unsigned)PE_LoadU32(0x8009D1F4u));
        }
        if (PE_LoadU16(0x8019C024u) || PE_LoadU16(0x8019C026u)) return 0xFFFFu;
        uint16_t step = g_d3[ent].wp[0].r > 0xFF ? (uint16_t)g_d3[ent].wp[0].r : 0xFF7Fu;
        if (cur != target) return (g_frame % 24) < 4 ? step : 0xFFFFu;
        return (g_frame % 48) < 4 ? confirm : 0xFFFFu;
    }
    if (ent < 0 && aya && g_frame - last_log >= 120) {   /* no entry: still trace where Aya is */
        last_log = g_frame;
        fprintf(stderr, "[D3PILOT] frame=%d token=%08X idle pos=%d,%d mode=%X d1a0=%X\n", g_frame, (unsigned)tok,
                (int)((int32_t)PE_LoadU32(aya + 40u) >> 16), (int)((int32_t)PE_LoadU32(aya + 48u) >> 16), mode, d1a0);
    }
    if (ent < 0 || !aya || mode != 0u || (d1a0 & 4u)) return script_mask;   /* window/menu: text pad */
    if (idx >= g_d3[ent].n) {
        if (!done_logged) { done_logged = 1;
            fprintf(stderr, "[D3PILOT] frame=%d token=%08X waypoints done\n", g_frame, (unsigned)tok); }
        return script_mask;
    }
    ax = (int32_t)PE_LoadU32(aya + 40u) >> 16;
    az = (int32_t)PE_LoadU32(aya + 48u) >> 16;
    {
        D3Wp *w = &g_d3[ent].wp[idx];
        int wx = w->x - ax, wz = w->z - az;
        if (g_frame - last_log >= 60) {
            last_log = g_frame;
            fprintf(stderr, "[D3PILOT] frame=%d token=%08X pos=%d,%d wp%d=%d,%d d1a0=%X held=%08X cam=%d,%d\n",
                    g_frame, (unsigned)tok, ax, az, idx, w->x, w->z, d1a0,
                    (unsigned)PE_LoadU32(0x8009D26Cu),
                    (int16_t)PE_LoadU16(0x800BD000u), (int16_t)PE_LoadU16(0x800BD004u));
        }
        if ((long long)wx * wx + (long long)wz * wz <= (long long)w->r * w->r) {
            fprintf(stderr, "[D3PILOT] frame=%d token=%08X reached wp%d at %d,%d\n",
                    g_frame, (unsigned)tok, idx, ax, az);
            if (w->act) act_until = g_frame + w->act;
            idx++;
            return 0xFFFFu;
        }
        {   /* Cross pulse while walking: advances text the field mode does not flag */
            uint16_t m = RouteM34ShimMove(ax, az, wx, wz, 0);
            if ((g_frame % 16) == 3 || (g_frame % 16) == 4) m &= 0xBFFFu;
            return m;
        }
    }
}

#endif /* PC_PORT_ROUTE_DAY3_PILOT_H */
