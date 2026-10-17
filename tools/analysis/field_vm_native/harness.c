/* Standalone test transport. The three callees are test-only ordered mocks;
 * the production handler TU is compiled separately against its real headers.
 * No CPU interpreter, game, disc, device, or runtime boundary is linked here.
 */
#include "pe_guest_ram.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int func_800133E8(void);
int func_80019810(pe_addr_t args);

static unsigned char ram[0x200400];
static uint32_t seed, calls[4][4];
static unsigned call_count;

static size_t address(pe_addr_t a, unsigned size)
{
    size_t offset;
    if ((a & (size - 1u)) != 0u) abort();
    if ((a & 0x1FFFFC00u) == 0x1F800000u)
        return 0x200000u + (a & 0x3FFu);
    offset = a & 0x1FFFFFFFu;
    if (offset + size > 0x200000u) abort();
    return offset;
}
uint8_t PE_LoadU8(pe_addr_t a) { return ram[address(a, 1)]; }
uint16_t PE_LoadU16(pe_addr_t a)
{
    const unsigned char *p = ram + address(a, 2);
    return (uint16_t)(p[0] | (uint16_t)p[1] << 8);
}
uint32_t PE_LoadU32(pe_addr_t a)
{
    const unsigned char *p = ram + address(a, 4);
    return p[0] | (uint32_t)p[1] << 8 | (uint32_t)p[2] << 16 | (uint32_t)p[3] << 24;
}
void PE_StoreU32(pe_addr_t a, uint32_t v)
{
    unsigned char *p = ram + address(a, 4);
    for (unsigned n = 0; n < 4; ++n) p[n] = (unsigned char)(v >> (n * 8));
}

static uint32_t mock(uint32_t fn, unsigned count, uint32_t a0, uint32_t a1)
{
    uint32_t h = fn * 2654435761u + seed * 40503u;
    if (call_count >= 4) abort();
    calls[call_count][0] = fn; calls[call_count][1] = count;
    calls[call_count][2] = a0; calls[call_count][3] = a1;
    ++call_count;
    if (((seed * 2654435761u) >> 16) & 1u)
        return (uint32_t)((int32_t)(h & 0x1FFFu) - 0x1000);
    return 0x80100000u + (h & 0xDFFFCu);
}
int func_80077CF4(int angle) { return (int)mock(0x80077CF4u, 1, (uint32_t)angle, 0); }
int func_80077DC4(int angle) { return (int)mock(0x80077DC4u, 1, (uint32_t)angle, 0); }
uint32_t func_8003708C(uint32_t a, uint32_t b) { return mock(0x8003708Cu, 2, a, b); }

static int get_word(uint32_t *value)
{
    unsigned char b[4];
    if (fread(b, 1, 4, stdin) != 4) return -1;
    *value = b[0] | (uint32_t)b[1] << 8 | (uint32_t)b[2] << 16 | (uint32_t)b[3] << 24;
    return 0;
}
static int put_word(uint32_t value)
{
    unsigned char b[4];
    for (unsigned n = 0; n < 4; ++n) b[n] = (unsigned char)(value >> (n * 8));
    return fwrite(b, 1, 4, stdout) == 4 ? 0 : -1;
}
int main(void)
{
    unsigned total = 0;
    for (;;) {
        unsigned char magic[4];
        uint32_t entry, arg, result;
        const size_t got = fread(magic, 1, 4, stdin);
        if (got == 0 && feof(stdin) && !ferror(stdin)) return 0;
        if (got != 4 || memcmp(magic, "PEVM", 4) || ++total > 2048u) return 2;
        if (get_word(&entry) || get_word(&seed) || get_word(&arg) ||
            fread(ram, 1, sizeof ram, stdin) != sizeof ram) return 2;
        call_count = 0;
        if (entry == 0x800133E8u) result = (uint32_t)func_800133E8();
        else if (entry == 0x80019810u) result = (uint32_t)func_80019810(arg);
        else return 2;
        if (fwrite("VRES", 1, 4, stdout) != 4 || put_word(result) || put_word(call_count)) return 2;
        for (unsigned i = 0; i < call_count; ++i)
            for (unsigned k = 0; k < 4; ++k) if (put_word(calls[i][k])) return 2;
        if (fwrite(ram, 1, sizeof ram, stdout) != sizeof ram || fflush(stdout)) return 2;
    }
}
