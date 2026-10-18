/* The ending credits (ovl_0457, PE.IMG [0x457,0x4FC) -> 0x80120D00) as native
 * code: func_8001220C reaches the credits main loop 0x801235DC through the
 * guest-code registry, and the credits' libpress calls into ovl_039F
 * (PE.IMG [0x39F,0x3C5) -> 0x8010BCF8) run on host replacements
 * (platform/pe_libpress.c) and the func_8010C89C hand port.
 *
 * The retail cases copy the two overlays from the owner's PE.IMG into guest
 * RAM, as func_8006F044 does before the call, and run the retail bytes in the
 * TEST-ONLY R3000A oracle (tests/pe_akao_interp_oracle.c) next to the native
 * code.  No retail byte is in git. */
#ifndef TEST_CREDITS_NATIVE_H
#define TEST_CREDITS_NATIVE_H
#include "pe_guestcode.h"
#include "pe_mdec.h"
#include "pe_audio_driver.h"

#define CREDITS_LIBPRESS_VMA 0x8010BCF8u
#define CREDITS_OVL_VMA      0x80120D00u
/* func_8012403C: D_80011610 + ((D_80093168 - D_80093166) << 11) + 0x38800 /
 * + 0x43000, with D_80011610 = 0x80120D00 and the pair (0x457, 0x4FC). */
#define CREDITS_VLC_OUT      (CREDITS_OVL_VMA + 0x52800u + 0x38800u)
#define CREDITS_VLC_TABLE    (CREDITS_OVL_VMA + 0x52800u + 0x43000u)
#define CREDITS_PICTURES     0x80125BA4u   /* D_80125BA4: 31 BS picture pointers */
#define CREDITS_VLC_BYTES    (CREDITS_VLC_TABLE - CREDITS_VLC_OUT)   /* 0xA800 */

static int credits_load_sectors(uint32_t first, uint32_t end, pe_addr_t dst)
{
    char err[160];
    uint32_t bytes = (end - first) * 0x800u;
    uint8_t *ram = (uint8_t *)PE_Translate(dst, bytes);
    if (!ram) return -1;
    return PE_DiscCache_FileRead(PE_DISC_SRC_PEIMG, first * 0x800u, ram, bytes, err, sizeof err);
}

static int credits_load_overlays(void)
{
    if (credits_load_sectors(0x39Fu, 0x3C5u, CREDITS_LIBPRESS_VMA) != 0) return -1;
    return credits_load_sectors(0x457u, 0x4FCu, CREDITS_OVL_VMA);
}

/* DecDCTvlcSize / DecDCTBufSize as the retail bytes define them
 * (func_8010C86C: addi at,a0,-1; blez -> 0xFFFFFF, else a0 << 1; the old
 * bound is returned from the delay slot). */
static void test_credits_libpress_replacements(void)
{
    TEST("credits_libpress_vlcsize_bufsize_outsync");
    ResetTestState();
    PE_StoreU32(0x8011EB8Cu, 0x00FFFFFFu);
    ASSERT(PE_DecDCTvlcSize_Guest(0x450u) == 0x00FFFFFF && PE_LoadU32(0x8011EB8Cu) == 0x8A0u,
           "vlcSize(0x450): old bound returned, new bound size<<1");
    ASSERT(PE_DecDCTvlcSize_Guest(2u) == 0x8A0 && PE_LoadU32(0x8011EB8Cu) == 4u, "vlcSize(2)");
    ASSERT(PE_DecDCTvlcSize_Guest(1u) == 4 && PE_LoadU32(0x8011EB8Cu) == 0x00FFFFFFu,
           "vlcSize(1): no bound");
    ASSERT(PE_DecDCTvlcSize_Guest(0u) == 0x00FFFFFF && PE_LoadU32(0x8011EB8Cu) == 0x00FFFFFFu,
           "vlcSize(0): no bound");
    ASSERT(PE_DecDCTvlcSize_Guest(0xFFFFFFFFu) == 0x00FFFFFF &&
           PE_LoadU32(0x8011EB8Cu) == 0x00FFFFFFu, "vlcSize(-1): signed compare");
    PE_StoreU32(0x80100000u, 0x38001140u);
    ASSERT(PE_DecDCTBufSize_Guest(0x80100000u) == 0x1140u, "BufSize: first halfword");
    /* DecDCToutSync(1) reads MDEC1 status bit 24 = the command's bit 26. */
    PE_MDEC_Init();
    PE_MDEC_BeginReset();
    ASSERT(PE_DecDCToutSync_Guest(1) == 0, "outSync(1) after reset (status 0x80040000)");
    ASSERT(PE_DecDCToutSync_Guest(0) == 0, "outSync(0) with DMA1 idle");
    PE_StoreU32(0x80100000u, 0x3C000020u);
    ASSERT(PE_MDEC_SubmitInputTable(0x80100000u, 0x20u) == 0, "submit signed-output command");
    ASSERT(PE_DecDCToutSync_Guest(1) == 1, "outSync(1) = CMD.26 set");
    PE_MDEC_BeginReset();
    PE_StoreU32(0x80100000u, 0x38000020u);
    ASSERT(PE_MDEC_SubmitInputTable(0x80100000u, 0x20u) == 0, "submit unsigned-output command");
    ASSERT(PE_DecDCToutSync_Guest(1) == 0, "outSync(1) = CMD.26 clear");
    PE_MDEC_Init();
    PASS();
}

/* The two pure replacements against the retail ovl_039F bytes. */
static void test_credits_libpress_retail_oracle(void)
{
    static const uint32_t sizes[] = {0u, 1u, 2u, 3u, 0x10u, 0x450u, 0x3FFFu, 0x7FFFu,
                                     0x7FFFFFFFu, 0x80000001u, 0xFFFFFFFEu, 0xFFFFFFFFu};
    unsigned i;
    TEST_RETAIL_DISC1("credits_libpress_vlcsize_bufsize_match_retail");
    ResetTestState();
    ASSERT(credits_load_overlays() == 0, "ovl_039F/ovl_0457 from PE.IMG");
    for (i = 0; i < sizeof sizes / sizeof sizes[0] + 64u; i++) {
        uint32_t size = i < sizeof sizes / sizeof sizes[0] ? sizes[i] : (i * 0x9E3779B9u) >> (i & 15u);
        uint32_t seed = 0xA5A50000u ^ (i * 0x01000193u), rv, nv, rb;
        int ok = 0;
        if (size == 0x80000000u) continue;   /* addi overflow trap */
        PE_StoreU32(0x8011EB8Cu, seed);
        rv = PE_AudioDriver_CallGuest(0x8010C86Cu, size, 0u, 0u, 0u, &ok);
        ASSERT(ok, "retail func_8010C86C ran");
        rb = PE_LoadU32(0x8011EB8Cu);
        PE_StoreU32(0x8011EB8Cu, seed);
        nv = (uint32_t)PE_DecDCTvlcSize_Guest(size);
        ASSERT(nv == rv && PE_LoadU32(0x8011EB8Cu) == rb, "vlcSize matches retail");
    }
    for (i = 0; i < 31u; i++) {
        pe_addr_t bs = PE_LoadU32(CREDITS_PICTURES + i * 4u);
        int ok = 0;
        uint32_t rv = PE_AudioDriver_CallGuest(0x8010BF94u, bs, 0u, 0u, 0u, &ok);
        ASSERT(ok && rv == PE_DecDCTBufSize_Guest(bs), "BufSize matches retail");
    }
    PASS();
}

/* func_8001220C reaches 0x801235DC with the reloaded ovl_0457 bytes there:
 * the registry must bind the generated credits loop, and every credits op
 * (the D_80125B4C / D_80125B68 tables it calls through) must be native.
 * Other bytes at the address stay a loud boundary. */
static void test_credits_registry_dispatch(void)
{
    static const pe_addr_t tables[2] = {0x80125B4Cu, 0x80125B68u};
    const PeGuestCodeEntry *e;
    unsigned t, k;
    TEST_RETAIL_DISC1("credits_main_loop_resolves_native");
    ResetTestState();
    PE_Overlay_ResetResidency();
    ASSERT(credits_load_overlays() == 0, "ovl_039F/ovl_0457 from PE.IMG");
    e = PE_GuestCode_Resolve(0x801235DCu);
    ASSERT(e && e->symbol && strcmp(e->symbol, "ovl_0457__func_801235DC") == 0,
           "0x801235DC resolves to the generated ovl_0457 credits loop");
    for (t = 0; t < 2u; t++)
        for (k = 0; k < 7u; k++) {
            pe_addr_t fn = PE_LoadU32(tables[t] + k * 4u);
            if (!fn) continue;
            e = PE_GuestCode_Resolve(fn);
            ASSERT(e && e->symbol && strncmp(e->symbol, "ovl_0457__", 10) == 0,
                   "every credits op resolves to a generated ovl_0457 TU");
        }
    PE_StoreU8(0x801235DCu, (uint8_t)(PE_LoadU8(0x801235DCu) ^ 0xFFu));
    ASSERT(PE_GuestCode_Resolve(0x801235DCu) == NULL, "other bytes at 0x801235DC do not resolve");
    PASS();
}

/* The credits decode every picture with a real VLC bound (DecDCTvlcSize of
 * BufSize >> 2, or >> 4 in func_80125094) and resume with func_8010C89C(0,
 * 0, table) until it returns 0.  Retail bytes in the oracle and the hand port
 * must leave identical guest RAM and the same return sequence; an unbounded
 * native decode must produce the same output stream. */
static uint8_t *g_credits_snap, *g_credits_ref, *g_credits_unbounded;

static unsigned credits_decode(int retail, pe_addr_t bs, unsigned shift, uint32_t *rets)
{
    unsigned n = 0;
    int ok = 1;
    uint32_t r;
    if (retail) {
        uint32_t size = PE_AudioDriver_CallGuest(0x8010BF94u, bs, 0u, 0u, 0u, &ok);
        (void)PE_AudioDriver_CallGuest(0x8010C86Cu, shift ? size >> shift : 0u, 0u, 0u, 0u, &ok);
        r = PE_AudioDriver_CallGuest(0x8010C89Cu, bs, CREDITS_VLC_OUT, CREDITS_VLC_TABLE, 0u, &ok);
    } else {
        uint32_t size = PE_DecDCTBufSize_Guest(bs);
        (void)PE_DecDCTvlcSize_Guest(shift ? size >> shift : 0u);
        r = (uint32_t)func_8010C89C(bs, CREDITS_VLC_OUT, CREDITS_VLC_TABLE, 0u);
    }
    for (;;) {
        if (!ok) return 0u;
        if (n < 4096u) rets[n] = r;
        n++;
        if (r == 0u || n >= 4096u) break;
        r = retail ? PE_AudioDriver_CallGuest(0x8010C89Cu, 0u, 0u, CREDITS_VLC_TABLE, 0u, &ok)
                   : (uint32_t)func_8010C89C(0u, 0u, CREDITS_VLC_TABLE, 0u);
    }
    return n;
}

static void test_credits_vlc_bounded_resume_oracle(void)
{
    static uint32_t rret[4096], nret[4096];
    uint8_t *ram = (uint8_t *)PE_Translate(0x80000000u, PE_RAM_SIZE);
    unsigned i, s, resumes = 0;
    int ok = 0;
    TEST_RETAIL_DISC1("credits_vlc_decode_bounded_resume_matches_retail");
    ResetTestState();
    ASSERT(credits_load_overlays() == 0, "ovl_039F/ovl_0457 from PE.IMG");
    if (!g_credits_snap) g_credits_snap = (uint8_t *)malloc(PE_RAM_SIZE);
    if (!g_credits_ref) g_credits_ref = (uint8_t *)malloc(PE_RAM_SIZE);
    if (!g_credits_unbounded) g_credits_unbounded = (uint8_t *)malloc(CREDITS_VLC_BYTES);
    ASSERT(g_credits_snap && g_credits_ref && g_credits_unbounded, "snapshot buffers");
    /* func_8012489C: func_8010BD4C builds the VLC table (retail bytes). */
    (void)PE_AudioDriver_CallGuest(0x8010BD4Cu, CREDITS_VLC_TABLE, 0u, 0u, 0u, &ok);
    ASSERT(ok, "retail VLC table build");
    for (i = 0; i < 31u; i++) {
        pe_addr_t bs = PE_LoadU32(CREDITS_PICTURES + i * 4u);
        for (s = 0; s < 3u; s++) {
            static const unsigned shifts[3] = {0u, 2u, 4u};
            unsigned rn, nn, k;
            memcpy(g_credits_snap, ram, PE_RAM_SIZE);
            rn = credits_decode(1, bs, shifts[s], rret);
            ASSERT(rn > 0u && rn < 4096u, "retail decode ran to the pad exit");
            memcpy(g_credits_ref, ram, PE_RAM_SIZE);
            memcpy(ram, g_credits_snap, PE_RAM_SIZE);
            nn = credits_decode(0, bs, shifts[s], nret);
            ASSERT(nn == rn, "same number of bounded calls as retail");
            for (k = 0; k < rn; k++)
                ASSERT(nret[k] == rret[k], "same return sequence as retail");
            if (memcmp(g_credits_ref, ram, PE_RAM_SIZE) != 0) {
                for (k = 0; k < PE_RAM_SIZE && g_credits_ref[k] == ram[k]; k++) {}
                printf("[credits] picture %u shift %u: first RAM difference 0x%08X retail %02X native %02X\n",
                       i, shifts[s], 0x80000000u + k, g_credits_ref[k], ram[k]);
                ASSERT(0, "guest RAM after the decode matches retail");
            }
            if (s == 0u)
                memcpy(g_credits_unbounded, ram + (CREDITS_VLC_OUT & 0x1FFFFFu), CREDITS_VLC_BYTES);
            else {
                resumes += rn - 1u;
                ASSERT(memcmp(g_credits_unbounded, ram + (CREDITS_VLC_OUT & 0x1FFFFFu), CREDITS_VLC_BYTES) == 0,
                       "bounded+resume output equals the unbounded decode");
            }
            memcpy(ram, g_credits_snap, PE_RAM_SIZE);
        }
    }
    printf("[credits] 31 pictures x 3 bounds, %u resumed calls, retail == native ", resumes);
    ASSERT(resumes > 0u, "the bound actually interrupted a decode");
    PASS();
}

static void test_credits_native_all(void)
{
    test_credits_libpress_replacements();
    test_credits_libpress_retail_oracle();
    test_credits_registry_dispatch();
    test_credits_vlc_bounded_resume_oracle();
}
#endif
