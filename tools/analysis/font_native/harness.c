/* Isolated font initializer transport with scripted native callee mocks.
 * No game runtime, CPU interpreter, assets or devices are linked here. */
#include "pe_port_compat.h"
#include "game_port.h"

unsigned char func_80038D74(void);

#define RAM_BYTES 0x200400u
#define MAX_CALLS 8192u
static uint8_t ram[RAM_BYTES];
static uint32_t calls[MAX_CALLS][5], call_count;
static uint32_t random_state, issue_failures, poll_mode, forced_seed;
static unsigned poll_index, seed_index, seeded;
static PEPortStopReason stop;
static unsigned stop_epoch;

static size_t address(pe_addr_t a, unsigned size)
{
    size_t offset;
    if (a & (size - 1u)) abort();
    if ((a & 0x1FFFFC00u) == 0x1F800000u) return 0x200000u + (a & 0x3FFu);
    offset = a & 0x1FFFFFFFu;
    if (offset + size > 0x200000u) abort();
    return offset;
}
uint8_t PE_LoadU8(pe_addr_t a) { return ram[address(a, 1)]; }
uint16_t PE_LoadU16(pe_addr_t a)
{
    const uint8_t *p = ram + address(a, 2);
    return (uint16_t)(p[0] | (uint16_t)p[1] << 8);
}
uint32_t PE_LoadU32(pe_addr_t a)
{
    const uint8_t *p = ram + address(a, 4);
    return p[0] | (uint32_t)p[1] << 8 | (uint32_t)p[2] << 16 | (uint32_t)p[3] << 24;
}
void PE_StoreU8(pe_addr_t a, uint8_t value) { ram[address(a, 1)] = value; }
void PE_StoreU32(pe_addr_t a, uint32_t value)
{
    uint8_t *p = ram + address(a, 4);
    for (unsigned i = 0; i < 4u; ++i) p[i] = (uint8_t)(value >> (i * 8u));
}
void PE_Port_RequestStop(PEPortStopReason reason) { stop = reason; ++stop_epoch; }
unsigned PE_Port_StopEpoch(void) { return stop_epoch; }

static void note(uint32_t fn, uint32_t arity, uint32_t a0, uint32_t a1, uint32_t a2)
{
    if (call_count >= MAX_CALLS) abort();
    calls[call_count][0] = fn; calls[call_count][1] = arity;
    calls[call_count][2] = a0; calls[call_count][3] = a1; calls[call_count][4] = a2;
    ++call_count;
}
int func_8006E6A8(int lba, pe_addr_t buffer, int sectors)
{
    note(0x8006E6A8u, 3, (uint32_t)lba, buffer, (uint32_t)sectors);
    if (issue_failures) { --issue_failures; return -1; }
    return 0;
}
int func_8006E7E8(void)
{
    static const int normal[] = {2, 1, 0}, retry[] = {1, -1, 2, 0};
    unsigned limit = poll_mode ? 4u : 3u;
    note(0x8006E7E8u, 0, 0, 0, 0);
    if (poll_index >= limit) abort();
    return poll_mode ? retry[poll_index++] : normal[poll_index++];
}
unsigned int func_80071A54(void)
{
    static const unsigned prefix[] = {0, 32767, 12345};
    note(0x80071A54u, 0, 0, 0, 0);
    if (!seeded && forced_seed && seed_index < 3u) return prefix[seed_index++];
    random_state = random_state * 0x41C64E6Du + 0x3039u;
    return (random_state >> 16) & 0x7FFFu;
}
void func_80071A64(uint32_t seed)
{
    note(0x80071A64u, 1, seed, 0, 0);
    random_state = seed; seeded = 1;
}
int func_800389DC(int page)
{
    note(0x800389DCu, 1, (uint32_t)page, 0, 0);
    /* A distinct caller-supplied page payload. The production page loader
     * is not linked; both sides get the same explicit mocked side effect. */
    memcpy(ram + 0x9ECD9u, ram + 0x118001u, 328u);
    return 0;
}

static int get_word(uint32_t *value)
{
    uint8_t b[4];
    if (fread(b, 1, 4, stdin) != 4) return -1;
    *value = b[0] | (uint32_t)b[1] << 8 | (uint32_t)b[2] << 16 | (uint32_t)b[3] << 24;
    return 0;
}
static int put_word(uint32_t value)
{
    uint8_t b[4];
    for (unsigned i = 0; i < 4u; ++i) b[i] = (uint8_t)(value >> (i * 8u));
    return fwrite(b, 1, 4, stdout) == 4 ? 0 : -1;
}
int main(void)
{
    unsigned total = 0;
    for (;;) {
        char magic[4];
        size_t count = fread(magic, 1, 4, stdin);
        if (!count && feof(stdin) && !ferror(stdin)) return 0;
        if (count != 4 || memcmp(magic, "FNT1", 4) || ++total > 128u) return 2;
        if (get_word(&random_state) || get_word(&issue_failures) ||
            get_word(&poll_mode) || get_word(&forced_seed) ||
            fread(ram, 1, sizeof ram, stdin) != sizeof ram) return 2;
        call_count = poll_index = seed_index = seeded = stop_epoch = 0;
        stop = PE_PORT_STOP_NONE;
        uint32_t result = func_80038D74();
        if (fwrite("FRES", 1, 4, stdout) != 4 || put_word(result) ||
            put_word(call_count) || put_word((uint32_t)stop)) return 2;
        for (unsigned i = 0; i < call_count; ++i)
            for (unsigned j = 0; j < 5u; ++j) if (put_word(calls[i][j])) return 2;
        if (fwrite(ram, 1, sizeof ram, stdout) != sizeof ram || fflush(stdout)) return 2;
    }
}
