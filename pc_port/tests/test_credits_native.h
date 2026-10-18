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

/* No CD provider serves XA streaming (PE_Cd_XaServiceAvailable), so
 * func_8001220C's credits arm stops on the credits_cd_xa_service boundary in
 * every mode instead of entering a loop that would spin.  Nothing of the
 * credits runs: the op-table pointer the loop stores first keeps its sentinel
 * and the libcd queue is untouched. */
static void test_credits_entry_stops_without_xa_service(void)
{
    static const uint32_t modes[] = { 0u, 0x10000u, 0x12000u, 0x20000u };
    unsigned m;
    TEST("credits_entry_stops_without_xa_service");
    ASSERT(PE_Cd_XaServiceAvailable() == 0, "no provider serves XA streaming");
    for (m = 0; m < 4u; m++) {
        ResetTestState();
        PE_StoreU32(0x8009D1A0u, modes[m]);
        PE_StoreU32(0x80172CD0u, 0xC0DEC0DEu);
        PE_StoreU32(0x8009B53Cu, 0x55u);
        ASSERT(PE_Main_EnterCredits() == 0, "credits arm reports not run");
        ASSERT(PE_Port_ShouldStop() &&
               PE_Port_GetStopReason() == PE_PORT_STOP_UNRESOLVED_BOUNDARY,
               "loud unresolved-boundary stop");
        ASSERT(g_stub_order_count == 1 &&
               strcmp(g_stub_order_log[0], "credits_cd_xa_service") == 0,
               "credits_cd_xa_service boundary identity");
        ASSERT(g_bootstrap_arg_call_count == 1 &&
               g_bootstrap_arg_calls[0].arg0 == (modes[m] & 0x30000u),
               "boundary records the credits mode bits");
        ASSERT(PE_LoadU32(0x80172CD0u) == 0xC0DEC0DEu, "credits loop not entered");
        ASSERT(PE_LoadU32(0x8009B53Cu) == 0x55u && PE_LoadU32(0x800A3608u) == 0u,
               "libcd queue untouched");
    }
    PASS();
}

/* Why the arm stops.  With the device disabled (the production provider) a
 * queued command never completes.  func_80125744 starts with CdlSetmode 0xC8
 * through func_8007EE84(14, mode, 0, -1) and polls func_8007F418 in a
 * for(;;) that only a 2 (or a retry on another nonzero) leaves; here every
 * poll reads 0 (pending) and nothing requests a stop.  The kick latches lane
 * D_8009B574 = 2 and only the IRQ chain clears it, so every later
 * func_8007F72C() == 1 wait (func_801255A4's drain among them) would spin as
 * well.  The completion ring stays empty, so func_80125894's
 * func_8007F418(0, 0) reads 6, never 2. */
static void test_credits_xa_commands_never_complete_without_device(void)
{
    const pe_addr_t mode = 0x801F0000u, resp = 0x801F0010u;
    PE_Disc *disc;
    char err[256];
    uint32_t seq;
    unsigned i;
    TEST_RETAIL_DISC1("credits_xa_commands_never_complete_without_device");
    ResetTestState();
    PE_CdReg_Reset();
    err[0] = 0;
    disc = BTL6_OpenDisc1(err, sizeof(err));
    ASSERT(disc != NULL, err[0] ? err : "PE_Disc_Open failed");
    PE_Disc_SetActive(disc);
    ASSERT(PE_GuestImage_LoadExe(disc, err, sizeof(err)) == 0, err);
    ASSERT(PE_Globals_AdoptRetailImage() == 0, "retail startup globals");
    ASSERT(!PE_CdReg_DeviceEnabled(), "production provider: device disabled");
    ASSERT(func_8007EC14() == 1 && func_8007ED58() == 1, "CdInit and reset");
    ASSERT(func_8007F72C() == 1 && func_8007F778() == 0, "drive idle, queue empty");
    PE_StoreU8(mode, 200u);
    seq = func_8007EE84(14u, mode, 0u, 0xFFFFFFFFu);
    ASSERT(seq != 0u && !PE_Port_ShouldStop() && PE_LoadU32(0x800A3608u) == 1u,
           "CdlSetmode queued and kicked");
    for (i = 0; i < 100000u; i++)
        if (func_8007F418(seq, resp) != 0) break;
    ASSERT(i == 100000u && !PE_Port_ShouldStop(),
           "CdlSetmode stays pending for 100000 polls with no stop");
    ASSERT(PE_LoadU32(0x8009B574u) == 2u && func_8007F72C() == 2,
           "lane stays 2, so later idle waits cannot pass");
    ASSERT(func_8007F418(0u, 0u) == 6, "empty completion ring: the channel-switch wait reads 6");
    PE_Disc_SetActive(NULL);
    PE_Disc_Close(disc);
    PASS();
}

/* The ending credits' CD-XA path on the register-level device: the retail
 * EXE image, ovl_039F/ovl_0457 from PE.IMG, and a synthetic Disc-2-like
 * image holding \XASTREAM\CREDITS.XA;1 laid out as the real file is
 * (file 1; channels 0 and 1 one sector in 8 with submode 64h, EOF-flagged
 * last ones; null sectors between).  No retail byte is in the fixture. */
#define CXA_SECTORS      512u
#define CXA_FILE_LBA     64u
#define CXA_FILE_SECTORS 48u
int32_t ovl_0457__func_8012562C(void);
int32_t ovl_0457__func_80125744(void);
int32_t ovl_0457__func_80125894(uint8_t a_a0);
int ovl_0457__func_801255A4(void);
void ovl_0457__func_801235DC(void);

/* total_sectors >= CXA_SECTORS: sectors past CXA_SECTORS are left untouched
 * (calloc'd zero pages, never written) so a long fixture costs no memory; the
 * drive delivers them as zero data sectors. */
static PE_Disc *CxaBuildDiscSized(uint8_t **out, uint32_t total_sectors)
{
    uint8_t *img = calloc(total_sectors, PE_DISC_RAW_SECTOR), *p;
    uint32_t i;
    int n;
    if (!img) return NULL;
    for (i = 0; i < CXA_SECTORS; i++) {
        uint8_t *raw = img + (size_t)i * PE_DISC_RAW_SECTOR;
        memset(raw + 1, 0xFF, 10);
        raw[15] = 2u;
    }
    p = FxUser(img, 16);
    p[0] = 1; memcpy(p + 1, "CD001", 5); p[6] = 1;
    memcpy(p + 40, "PARASITE_EVE                    ", 32);
    FxPutDirRec(p + 156, 20, 2048, 0x02, "\0", 1);
    p = FxUser(img, 20);
    n = FxPutDirRec(p, 20, 2048, 0x02, "\0", 1);
    n += FxPutDirRec(p + n, 20, 2048, 0x02, "\1", 1);
    FxPutDirRec(p + n, 21, 2048, 0x02, "XASTREAM", 8);
    p = FxUser(img, 21);
    n = FxPutDirRec(p, 21, 2048, 0x02, "\0", 1);
    n += FxPutDirRec(p + n, 20, 2048, 0x02, "\1", 1);
    FxPutDirRec(p + n, CXA_FILE_LBA, CXA_FILE_SECTORS * 2048u, 0x00, "CREDITS.XA;1", 12);
    for (i = 0; i < CXA_FILE_SECTORS; i++) {
        uint8_t *raw = img + (size_t)(CXA_FILE_LBA + i) * PE_DISC_RAW_SECTOR;
        uint8_t slot = (uint8_t)(i & 7u), audio = slot < 2u;
        uint8_t sm = audio ? (i + 8u >= CXA_FILE_SECTORS ? 0xE4u : 0x64u) : 0x00u;
        raw[16] = raw[20] = 1u;
        raw[17] = raw[21] = audio ? slot : 0u;
        raw[18] = raw[22] = sm;
        raw[19] = raw[23] = audio ? 1u : 0u;
    }
    *out = img;
    return PE_Disc_OpenMemory(img, (size_t)total_sectors * PE_DISC_RAW_SECTOR);
}

static PE_Disc *CxaBuildDisc(uint8_t **out)
{
    return CxaBuildDiscSized(out, CXA_SECTORS);
}

static void test_credits_xa_device_path(void)
{
    PE_Disc *d1, *cx;
    uint8_t *img = NULL;
    char err[256];
    PeCdDeviceState dev;
    unsigned t;
    TEST_RETAIL_DISC1("credits_xa_device_path");
    ResetTestState();
    PE_CdReg_Reset();
    err[0] = 0;
    d1 = BTL6_OpenDisc1(err, sizeof(err));
    ASSERT(d1 != NULL, err[0] ? err : "PE_Disc_Open failed");
    PE_Disc_SetActive(d1);
    ASSERT(PE_GuestImage_LoadExe(d1, err, sizeof(err)) == 0, err);
    ASSERT(PE_Globals_AdoptRetailImage() == 0, "retail startup globals");
    ASSERT(credits_load_overlays() == 0, "ovl_039F/ovl_0457 from PE.IMG");
    cx = CxaBuildDisc(&img);
    ASSERT(cx != NULL, "credits disc fixture");
    PE_Disc_SetActive(cx);
    HostFB_Init(); PE_GPU_Init(); PE_Callback_Init();
    ASSERT(PE_CdReg_EnableDevice(7u) && func_8007EC14() == 1 && !PE_Port_ShouldStop(),
           "CdInit on the device");
    for (t = 0; t < 20u && PE_LoadU32(0x8009B574u) != 1u && !PE_Port_ShouldStop(); t++)
        HostFB_VSync(0);
    ASSERT(PE_LoadU32(0x8009B574u) == 1u && !PE_Port_ShouldStop(), "drive ready");
    ASSERT(ovl_0457__func_8012562C() == 0 && !PE_Port_ShouldStop(), "func_8012562C: CdSearchFile");
    ASSERT(PE_LoadU32(0x80172CE8u) == CXA_FILE_LBA &&
           PE_LoadU32(0x80172CECu) == CXA_FILE_LBA + CXA_FILE_SECTORS, "credits start and end LBA");
    ASSERT(ovl_0457__func_80125744() == 0 && !PE_Port_ShouldStop(),
           "func_80125744: Setmode, Setfilter and ReadS complete through the real queue");
    PE_CdReg_GetDeviceState(&dev);
    ASSERT(dev.reading && dev.mode == 0xC8u, "ReadS running in mode 0xC8");
    for (t = 0; t < 600u && !PE_LoadU32(0x80172CE4u) && !PE_Port_ShouldStop(); t++)
        HostFB_VSync(0);
    PE_CdReg_GetDeviceState(&dev);
    ASSERT(!PE_Port_ShouldStop(), "no stop while the XA stream plays");
    ASSERT(PE_LoadU32(0x80172CE4u) == 1u, "GetlocP positions carried the credits past the file end");
    ASSERT((int32_t)PE_LoadU32(0x80172CF0u) > (int32_t)(CXA_FILE_LBA + CXA_FILE_SECTORS),
           "position past the end");
    ASSERT(dev.xa_sectors >= CXA_FILE_SECTORS / 4u, "every audio sector went to the SPU");
    ASSERT(ovl_0457__func_80125894(1u) == 0 && !PE_Port_ShouldStop(), "channel switch to 1");
    ASSERT(ovl_0457__func_801255A4() == 0 && !PE_Port_ShouldStop(), "func_801255A4 teardown");
    PE_CdReg_GetDeviceState(&dev);
    ASSERT(!dev.reading && PE_LoadU32(0x8009B574u) == 1u && PE_LoadU32(0x800A3608u) == 0u,
           "after teardown the drive is idle and the queue empty");
    PE_CdReg_Reset();
    PE_Disc_SetActive(NULL);
    PE_Disc_Close(cx);
    PE_Disc_Close(d1);
    free(img);
    PASS();
}

/* The credits CD steps from production's entry state.  Production boots libds
 * in host mode: func_8003E610 runs ResetCallback and the host DsInit
 * (D_8009B554 = 1), and func_8006A5BC's CdReset leaves lane 1.  The overlay's
 * own DsInit is then a no-op, so PE_Cd_AttachDevice must supply the device
 * branch's two hooks, and PE_Cd_DetachDevice must take both off again.
 * Afterwards the IRQ 2 slot, VSync slot 0, I_MASK and the registered-source
 * mask must equal their pre-attach values.  A host-mode read must still
 * complete. */
static void test_credits_attach_from_host_boot(void)
{
    PE_Disc *d1, *cx;
    uint8_t *img = NULL;
    char err[256];
    PeCdDeviceState dev;
    PeCdAttach at;
    uint32_t irq2, vsync0;
    uint16_t mask, registered;
    unsigned t;
    TEST_RETAIL_DISC1("credits_attach_from_host_boot");
    ResetTestState();
    PE_CdReg_Reset();
    err[0] = 0;
    d1 = BTL6_OpenDisc1(err, sizeof(err));
    ASSERT(d1 != NULL, err[0] ? err : "PE_Disc_Open failed");
    PE_Disc_SetActive(d1);
    ASSERT(PE_GuestImage_LoadExe(d1, err, sizeof(err)) == 0, err);
    ASSERT(PE_Globals_AdoptRetailImage() == 0, "retail startup globals");
    ASSERT(credits_load_overlays() == 0, "ovl_039F/ovl_0457 from PE.IMG");
    cx = CxaBuildDisc(&img);
    ASSERT(cx != NULL, "credits disc fixture");
    PE_Disc_SetActive(cx);
    HostFB_Init(); PE_GPU_Init(); PE_Callback_Init();
    func_80073C94();
    ASSERT(!PE_CdReg_DeviceEnabled() && func_8007EC14() == 1 &&
           PE_LoadU32(0x8009B554u) == 1u && func_8007ED58() == 1 &&
           func_8007F72C() == 1 && !PE_Port_ShouldStop(),
           "host-mode boot: DsInit host branch, CdReset, lane 1");
    irq2 = PE_LoadU32(0x800945E8u + 8u);
    vsync0 = PE_Callback_GetSlot(0u);
    mask = PE_IRQ_GetMask();
    registered = PE_LoadU16(0x80094614u);
    ASSERT(irq2 == 0u && vsync0 == 0u, "host boot installs neither device hook");
    ASSERT(PE_Cd_AttachDevice(&at) && PE_CdReg_DeviceEnabled() &&
           PE_LoadU32(0x800945E8u + 8u) == 0x8007C13Cu &&
           PE_Callback_GetSlot(0u) == 0x8007FE24u && (PE_IRQ_GetMask() & 4u),
           "attach installs InterruptCallback(2) and VSyncCallbacks(0)");
    {
        PeCdAttach again;
        ASSERT(!PE_Cd_AttachDevice(&again) && !again.attached, "attach refuses twice");
    }
    ASSERT(ovl_0457__func_8012562C() == 0 && !PE_Port_ShouldStop(),
           "func_8012562C: DsInit no-op, CdSearchFile");
    ASSERT(PE_LoadU32(0x80172CE8u) == CXA_FILE_LBA, "credits start LBA");
    ASSERT(ovl_0457__func_80125744() == 0 && !PE_Port_ShouldStop(),
           "func_80125744 through the attached drive");
    for (t = 0; t < 600u && !PE_LoadU32(0x80172CE4u) && !PE_Port_ShouldStop(); t++)
        HostFB_VSync(0);
    PE_CdReg_GetDeviceState(&dev);
    ASSERT(!PE_Port_ShouldStop() && PE_LoadU32(0x80172CE4u) == 1u &&
           dev.xa_sectors >= CXA_FILE_SECTORS / 4u, "XA stream played to its end");
    ASSERT(ovl_0457__func_801255A4() == 0 && !PE_Port_ShouldStop(), "teardown");
    PE_Cd_DetachDevice(&at);
    ASSERT(!PE_CdReg_DeviceEnabled(), "detach disables the device");
    ASSERT(PE_LoadU32(0x800945E8u + 8u) == irq2 && PE_Callback_GetSlot(0u) == vsync0 &&
           PE_IRQ_GetMask() == mask && PE_LoadU16(0x80094614u) == registered,
           "detach restores the IRQ 2 slot, VSync slot 0 and both masks");
    ASSERT(func_8006E6D4(16, 0, 0x801E0000u, 1) == 1 && !PE_Port_ShouldStop() &&
           memcmp(PE_Translate(0x801E0001u, 5u), "CD001", 5) == 0,
           "a host-mode read completes after detach");
    PE_CdReg_Reset();
    PE_Disc_SetActive(NULL);
    PE_Disc_Close(cx);
    PE_Disc_Close(d1);
    free(img);
    PASS();
}

/* The credits loop func_801235DC itself, mode 0 (Disc 2 ending), from the
 * production entry state on the fixture disc.  It runs in a child process
 * under alarm(): the loop has no stop test, so a hang must fail the test, not
 * the suite.  The exit it must reach: func_80123950 reports the scroll done
 * (state 0 -> 4), then D_80172CE0 = D_80172CE4, which the GetlocP callback
 * sets once the position passes the file end; then the func_801255A4
 * teardown and op 6, and bit 16 of D_8009D1A0 cleared. */
/* The scroll is 744 lines of 18 px at 1 px per loop iteration after a 224 px
 * lead-in (func_80123E08, the 0x80125C88 line table): about 13600
 * iterations.  The 2x ReadS keeps running past the 48-sector file until the
 * teardown, 2.5 sectors per VBlank, so the disc must outlast that. */
#define CXA_LOOP_SECTORS 80000u
typedef struct {
    uint32_t done, stopped, ce0, ce4, iterations, vsyncs, presented, reading, lane, queue, d1a0;
    uint32_t xa;
} CxaLoopResult;

static void test_credits_main_loop_runs_to_exit(void)
{
    int fds[2], status = 0;
    pid_t pid;
    CxaLoopResult r;
    TEST_RETAIL_DISC1("credits_main_loop_runs_to_exit");
    ASSERT(pipe(fds) == 0, "pipe");
    fflush(NULL);
    pid = fork();
    if (pid == 0) {
        PE_Disc *d1, *cx;
        uint8_t *img = NULL;
        char err[256];
        PeCdAttach at;
        PeCdDeviceState dev;
        int vs, ds, pr, mk;
        memset(&r, 0, sizeof r);
        close(fds[0]);
        alarm(240);
        ResetTestState();
        PE_CdReg_Reset();
        err[0] = 0;
        d1 = BTL6_OpenDisc1(err, sizeof(err));
        if (d1) {
            PE_Disc_SetActive(d1);
            if (PE_GuestImage_LoadExe(d1, err, sizeof(err)) == 0 &&
                PE_Globals_AdoptRetailImage() == 0 && credits_load_overlays() == 0 &&
                (cx = CxaBuildDiscSized(&img, CXA_LOOP_SECTORS)) != NULL) {
                PE_Disc_SetActive(cx);
                HostFB_Init(); PE_GPU_Init(); PE_Callback_Init();
                func_80073C94();
                Test_SeedDigitalController();   /* boot's pad init: 844E4/84B44 */
                if (func_8007EC14() == 1 && func_8007ED58() == 1 && PE_Cd_AttachDevice(&at)) {
                    PE_StoreU32(0x8009D1A0u, PE_LoadU32(0x8009D1A0u) & ~0x30000u);
                    ovl_0457__func_801235DC();
                    PE_CdReg_GetDeviceState(&dev);
                    r.reading = dev.reading;
                    r.xa = dev.xa_sectors;
                    r.lane = PE_LoadU32(0x8009B574u);
                    r.queue = PE_LoadU32(0x800A3608u);
                    PE_Cd_DetachDevice(&at);
                    r.done = 1;
                }
            }
        }
        r.stopped = (uint32_t)PE_Port_ShouldStop();
        r.ce0 = PE_LoadU32(0x80172CE0u);
        r.ce4 = PE_LoadU32(0x80172CE4u);
        r.iterations = PE_LoadU32(0x80172CD4u) - 420u;
        r.d1a0 = PE_LoadU32(0x8009D1A0u);
        HostFB_GetState(&vs, &ds, &pr, &mk);
        r.vsyncs = (uint32_t)vs;
        r.presented = (uint32_t)pr;
        if (write(fds[1], &r, sizeof r) != (ssize_t)sizeof r) _exit(3);
        _exit(0);
    }
    close(fds[1]);
    ASSERT(pid > 0, "fork");
    memset(&r, 0, sizeof r);
    {
        ssize_t got = read(fds[0], &r, sizeof r);
        close(fds[0]);
        ASSERT(waitpid(pid, &status, 0) == pid, "waitpid");
        ASSERT(!WIFSIGNALED(status), "the credits loop did not exit within 240 s");
        ASSERT(got == (ssize_t)sizeof r && WIFEXITED(status) && WEXITSTATUS(status) == 0,
               "child result");
    }
    fprintf(stderr, "[credits] loop: %u iterations, %u vsync calls, %u presents, %u XA sectors\n",
            r.iterations, r.vsyncs, r.presented, r.xa);
    ASSERT(r.done && !r.stopped, "setup, attach and the whole loop ran without a stop");
    ASSERT(r.ce0 == 1u && r.ce4 == 1u, "the loop left through D_80172CE0 = D_80172CE4 = 1");
    ASSERT(!r.reading && r.lane == 1u && r.queue == 0u,
           "the teardown left the drive idle and the queue empty");
    ASSERT(!(r.d1a0 & 0x10000u), "bit 16 of D_8009D1A0 clear on exit");
    ASSERT(r.xa >= CXA_FILE_SECTORS / 4u, "the XA channel played");
    PASS();
}

static void test_credits_native_all(void)
{
    test_credits_libpress_replacements();
    test_credits_libpress_retail_oracle();
    test_credits_registry_dispatch();
    test_credits_vlc_bounded_resume_oracle();
    test_credits_entry_stops_without_xa_service();
    test_credits_xa_commands_never_complete_without_device();
    test_credits_xa_device_path();
    test_credits_attach_from_host_boot();
    test_credits_main_loop_runs_to_exit();
}
#endif
