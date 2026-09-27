/*
 * portverify regressions: hand ports that disagreed with the matching decomp.
 *
 * Each case below was found by the differential harness (generated TU vs the
 * pre-existing hand port, forked per trial on identical seeded guest RAM) and
 * confirmed against the retail disassembly (tools/analysis/pe_dis.sh).  The
 * matched C in src/ is the authority; these pin its behaviour so a future hand
 * edit cannot silently reintroduce the old divergence.  Record:
 * docs/evidence/portverify/REPORT.md.
 */

static void pv_seed_vm_task(pe_addr_t task, pe_addr_t actor)
{
    PE_StoreU32(0x8009D300u, task);
    PE_StoreU32(0x8009D2F0u, actor);
    PE_StoreU32(0x8009CE00u, 0x8012300Cu);
}

static void test_PV_661EC_mask_and_mode(void)
{
    TEST("PV_661EC_mask_and_mode");
    /* Retail 0x80066220 `li v0,-16` (mask ~0xF keeps bits 31..4) and
     * 0x8006624C `bne a3,8` -> `ori 1`, fall-through `ori 9`. */
    ResetTestState();
    PE_StoreU32(0x800BCF88u, 0xABCD0047u);
    ASSERT(func_800661EC(1, 2, 3, 8) == 0, "accepted");
    ASSERT(PE_LoadU32(0x800BCF88u) == 0xABCD0049u, "a3 == 8 -> low nibble 9, high bits kept");
    PE_StoreU32(0x800BCF88u, 0xABCD0047u);
    ASSERT(func_800661EC(1, 2, 3, 3) == 0, "accepted");
    ASSERT(PE_LoadU32(0x800BCF88u) == 0xABCD0041u, "a3 != 8 -> low nibble 1, high bits kept");
    PASS();
}

static void test_PV_19B08_bit8_polarity(void)
{
    const pe_addr_t task = 0x80108700u, actor = 0x80108800u;
    TEST("PV_19B08_bit8_polarity");
    /* Retail 0x80019B30..0x80019B4C: armed (task+8 & 0x20) and actor+0x98
     * bit 8 SET -> clear 0x20 and return 1; bit 8 clear -> yield 8. */
    ResetTestState();
    pv_seed_vm_task(task, actor);
    PE_StoreU16(task + 8u, 0x20u);
    PE_StoreU32(actor + 0x98u, 8u);
    ASSERT(func_80019B08() == 1, "bit 8 set completes");
    ASSERT((PE_LoadU16(task + 8u) & 0x20u) == 0u, "armed bit cleared");
    ASSERT(PE_LoadU32(0x8009CE00u) == 0x8012300Cu, "no rewind on completion");
    PE_StoreU16(task + 8u, 0x20u);
    PE_StoreU32(actor + 0x98u, 0u);
    ASSERT(func_80019B08() == 0, "bit 8 clear waits");
    ASSERT(PE_LoadU32(0x8009CE00u) == 0x80123004u, "wait rewinds 8");
    PASS();
}

static void test_PV_144FC_out_of_table_completes(void)
{
    static const uint8_t states[] = {0x3Cu, 0x80u, 0xFFu};
    const pe_addr_t task = 0x80108700u;
    unsigned i;
    TEST("PV_144FC_out_of_table_completes");
    /* Retail 0x80014520 `sltiu v0,v1,60` / `beqz -> 0x80014658`
     * (`j 0x8001467C`, `li v0,1`). */
    for (i = 0; i < sizeof(states); i++) {
        ResetTestState();
        pv_seed_vm_task(task, 0x80108800u);
        PE_StoreU8(0x800B0CD8u + 0xF4u, states[i]);
        ASSERT(func_800144FC(0x80120F80u) == 1, "out-of-table state returns 1");
        ASSERT(PE_LoadU32(0x8009CE00u) == 0x8012300Cu, "no yield rewind");
        ASSERT(PE_LoadU32(task + 0x10u) == 0u, "task not parked");
    }
    PASS();
}

static void test_PV_4F464_clears_flag(void)
{
    TEST("PV_4F464_clears_flag");
    /* src/func_8004F464.c: call func_8004E97C then D_8009D008 = 0. */
    ResetTestState();
    PE_StoreU32(0x8009D008u, 5u);
    func_8004F464();
    ASSERT(PE_LoadU32(0x8009D008u) == 0u, "pending word cleared after the call");
    PASS();
}

static void test_PV_6B35C_plus6_halfwords(void)
{
    unsigned i;
    TEST("PV_6B35C_plus6_halfwords");
    /* src/func_8006B35C.c: for 4 records of 8 bytes, zero +6 (from
     * D_80094488) and +4 (D_8009448C); +0/+2 are untouched. */
    ResetTestState();
    for (i = 0; i < 0x20u; i += 2u)
        PE_StoreU16(0x80094488u + i, 0xFFFFu);
    func_8006B35C();
    for (i = 0; i < 4u; i++) {
        pe_addr_t r = 0x80094488u + i * 8u;
        ASSERT(PE_LoadU16(r + 6u) == 0u, "+6 halfword cleared");
        ASSERT(PE_LoadU16(r + 4u) == 0u, "+4 halfword cleared");
        ASSERT(PE_LoadU16(r + 0u) == 0xFFFFu && PE_LoadU16(r + 2u) == 0xFFFFu,
               "+0/+2 untouched");
    }
    PASS();
}

static void test_PV_6BE4C_previous_half(void)
{
    TEST("PV_6BE4C_previous_half");
    /* Retail 0x8006BE94: right = (CE3 - 10) / 2 (the old port dropped -10). */
    ResetTestState();
    PE_StoreU8(0x800B0CE2u, 12u);
    PE_StoreU8(0x800B0CE3u, 13u);            /* (12-10)>>1 == (13-10)/2 == 1 */
    ASSERT(func_8006BE4C() == 0, "v0");
    ASSERT((PE_LoadU8(0x800B0CE6u) & 4u) == 0u, "equal halves leave CE6");
    PE_StoreU8(0x800B0CE3u, 0u);             /* 1 vs -5 */
    ASSERT(func_8006BE4C() == 0, "v0");
    ASSERT((PE_LoadU8(0x800B0CE6u) & 4u) == 4u, "different halves set CE6 bit 2");
    PASS();
}

static void pv_seed_colour_container(void)
{
    const pe_addr_t c = 0x80140000u, e = 0x80140100u, q = 0x80141000u;
    PE_StoreU32(0x800B1624u, c);
    PE_StoreU16(c + 6u, 1u);                 /* one record */
    PE_StoreU32(c + 0x14u, 0x100u);          /* records at c + 0x100 */
    PE_StoreU32(e + 0x30u, q);
    PE_StoreU16(e + 0x26u, 2u);              /* two 16-byte entries */
}

static void test_PV_67B74_fade_step(void)
{
    const pe_addr_t q = 0x80141000u;
    TEST("PV_67B74_fade_step");
    /* src/func_80067B74.c; the old hand port returned without doing anything. */
    ResetTestState();
    pv_seed_colour_container();
    PE_StoreU32(0x800BCF88u, 0x400u);
    PE_StoreU8(0x800BCFFBu, 2u);
    PE_StoreU8(0x800BCFFAu, 5u);             /* d = (2 << 7) / 4 = 64, col 0x40 */
    ASSERT(func_80067B74() == 0, "v0");
    ASSERT(PE_LoadU8(q + 4u) == 0x40u && PE_LoadU8(q + 5u) == 0x40u &&
           PE_LoadU8(q + 6u) == 0x40u && PE_LoadU8(q + 0x16u) == 0x40u,
           "both entries coloured");
    ASSERT(PE_LoadU8(0x800BCFFBu) == 3u, "step counter advanced");
    ASSERT(PE_LoadU32(0x800BCF88u) == 0x400u, "not finished yet");
    PE_StoreU8(0x800BCFFBu, 4u);             /* t = 5 >= 5 finishes */
    PE_StoreU32(0x8009CDDCu, 1u);            /* entries offset by c << 4 */
    ASSERT(func_80067B74() == 0, "v0");
    ASSERT(PE_LoadU8(q + 0x20u + 4u) == (uint8_t)(0x80 - (4 << 7) / 4), "offset entry coloured");
    ASSERT(PE_LoadU32(0x800BCF88u) == 0x800u, "0x400 phase -> 0x800");
    ASSERT(PE_LoadU16(0x80140026u) == 0x1FF0u, "container +0x26 = 0x1FF0");
    PASS();
}

static void test_PV_67D18_hold_phase(void)
{
    const pe_addr_t q = 0x80141000u;
    TEST("PV_67D18_hold_phase");
    /* src/func_80067D18.c; the old hand port returned without doing anything. */
    ResetTestState();
    pv_seed_colour_container();
    PE_StoreU32(0x800BCF88u, 0x1000u | 0x2000u | 0x4000u);
    PE_StoreU8(0x800BCFFCu, 0x33u);
    ASSERT(func_80067D18() == 0, "v0");
    ASSERT(PE_LoadU8(q + 4u) == 0x33u && PE_LoadU8(q + 0x16u) == 0x33u, "held colour");
    ASSERT(PE_LoadU32(0x800BCF88u) == 0xB000u, "0x4000 -> clear 0xC000, set 0x8000");
    PE_StoreU32(0x800BCF88u, 0x1000u | 0x8000u);
    ASSERT(func_80067D18() == 0, "v0");
    ASSERT(PE_LoadU8(q + 4u) == 0x80u, "default colour 0x80");
    ASSERT(PE_LoadU32(0x800BCF88u) == 0u, "0x8000 -> clear 0xC000 and 0x1000");
    PASS();
}

static void test_PV_52BCC_unsigned_char_head(void)
{
    const pe_addr_t dst = 0x80140000u, src = 0x80140100u;
    TEST("PV_52BCC_unsigned_char_head");
    /* src/func_80052BCC.c keeps the head byte in plain `char`, which is
     * unsigned in the era compiler (retail `lbu`/`andi 0xFF`); decomp TUs
     * are built with -funsigned-char.  A signed host char missed the 0xFF
     * head terminator (retail oracle INV8 case 80). */
    ResetTestState();
    PE_StoreU8(src, 0xFFu);
    PE_StoreU8(src + 1u, 0x11u);
    PE_StoreU8(src + 2u, 0xFFu);
    PE_StoreU8(dst + 1u, 0xAAu);
    func_80052BCC(dst, src);
    ASSERT(PE_LoadU8(dst) == 0xFFu, "terminator copied");
    ASSERT(PE_LoadU8(dst + 1u) == 0xAAu, "copy stops at the head terminator");
    PASS();
}


static int pv_load_aborts(pe_addr_t a)
{
    int status = 0;
    pid_t pid;
    fflush(NULL);
    pid = fork();
    if (pid == 0) {
        (void)freopen("/dev/null", "w", stderr);
        (void)PE_LoadU32(a);
        _exit(0);
    }
    if (pid < 0 || waitpid(pid, &status, 0) != pid) return -1;
    return WIFSIGNALED(status) && WTERMSIG(status) == SIGABRT;
}

static void test_PV_ram_mirror(void)
{
    TEST("PV_ram_mirror");
    /* PS1 main RAM is visible at KUSEG 0x00000000, KSEG0 0x80000000 and
     * KSEG1 0xA0000000 (psx-spx memory map); retail data carries KUSEG
     * aliases, e.g. func_80064C20(list = 0) writes 0x00000070. */
    ResetTestState();
    PE_StoreU32(0x00001234u, 0xCAFEF00Du);
    ASSERT(PE_LoadU32(0x80001234u) == 0xCAFEF00Du, "KUSEG store lands in KSEG0");
    ASSERT(PE_LoadU32(0xA0001234u) == 0xCAFEF00Du, "KSEG1 alias reads the same RAM");
    PE_StoreU16(0xA01FFFFEu, 0xBEEFu);
    ASSERT(PE_LoadU16(0x801FFFFEu) == 0xBEEFu, "last KSEG1 halfword");
    ASSERT(PE_RamCanonical(0x001FFFFCu, 4u) == 0x801FFFFCu, "last KUSEG word maps");
    ASSERT(PE_RamCanonical(0x001FFFFEu, 4u) == 0x001FFFFEu, "range crossing 2 MiB is not folded");
    ASSERT(PE_RamCanonical(0x00200000u, 1u) == 0x00200000u, "beyond 2 MiB is not RAM");
    ASSERT(PE_RamCanonical(0x1F800000u, 4u) == 0x1F800000u, "scratchpad is not a RAM alias");
    ASSERT(!PE_AddressIsRam(0x00001234u), "PE_AddressIsRam stays KSEG0-only");
#if !defined(__SANITIZE_ADDRESS__)
    ASSERT(pv_load_aborts(0x00200000u) == 1, "0x00200000 still aborts");
    ASSERT(pv_load_aborts(0x001FFFFEu) == 1, "a word crossing the mirror end aborts");
    ASSERT(pv_load_aborts(0xA0200000u) == 1, "KSEG1 beyond 2 MiB aborts");
#endif
    PASS();
}


static void test_PV_52F70_signed_cap(void)
{
    TEST("PV_52F70_signed_cap");
    /* Retail 0x80052F94 `slti v1,v1,51`: the cap test is signed. */
    ResetTestState();
    PE_StoreU8(0x800C0E0Cu, 3u);
    D_8009D018 = (uint32_t)-10;
    ASSERT(func_80052F70() == (unsigned int)-7, "negative sum takes the add path, not the cap");
    D_8009D018 = 60u;
    ASSERT(func_80052F70() == 50u, "sum >= 51 caps at 50");
    D_8009D018 = 0u;
    PASS();
}

static void test_PV_all(void)
{
    test_PV_52F70_signed_cap();
    test_PV_ram_mirror();
    test_PV_661EC_mask_and_mode();
    test_PV_19B08_bit8_polarity();
    test_PV_144FC_out_of_table_completes();
    test_PV_4F464_clears_flag();
    test_PV_6B35C_plus6_halfwords();
    test_PV_6BE4C_previous_half();
    test_PV_67B74_fade_step();
    test_PV_67D18_hold_phase();
    test_PV_52BCC_unsigned_char_head();
}
