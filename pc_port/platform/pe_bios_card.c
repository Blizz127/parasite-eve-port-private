/*
 * Host BIOS / libcard memory-card entry points at their retail trampoline
 * addresses, backed by the .mcd image layer (pe_memcard.c).
 *
 * Retail trampolines (tools/analysis/pe_dis.sh <addr> 0x10):
 *   0x80072734  li t2,B0h; jr t2; li t1,32h   B(32h) open(name, mode)
 *   0x80072744                        33h     B(33h) lseek(fd, off, type)
 *   0x80072754                        34h     B(34h) read(fd, dst, len)
 *   0x80072764                        35h     B(35h) write(fd, src, len)
 *   0x80072774                        36h     B(36h) close(fd)
 *   0x80072784                        41h     B(41h) format(device)
 *   0x80072794                        43h     B(43h) nextfile(direntry)
 *   0x800727A4                        45h     B(45h) delete(name)
 *   0x800727B4  libapi firstfile wrapper -> B(42h) firstfile(name, dir)
 *               (its kernel device-table hook stays in absent_sdk_port.c;
 *               the host kernel has no device table, so card callers use
 *               PE_Bios_FirstFile for the B(42h) result directly)
 *   0x800726F4                        0Bh     B(0Bh) TestEvent(handle)
 *   0x8007DD44  li t2,A0h; jr t2; li t1,ABh   A(ABh) _card_info(chan)
 *   0x8007DD54                        ACh     A(ACh) _card_load(chan)
 *   0x8007DDB4  li t2,B0h; ...        4Eh     B(4Eh) write_card_sector
 *   0x8007DDC4                        50h     B(50h) allow_new_card
 *
 * These are kernel/SDK services (host-side by policy).  The card results
 * are delivered as the libcard events the game opened in func_800409B4
 * (pe_libcard.c): software class 0xF4000001 for _card_info/_card_load and
 * hardware class 0xF0000011 for the sector write, spec 0004h (IOE),
 * 0100h (timeout = no card), 2000h (new card / unformatted).  Delivery is
 * synchronous: the game's status machine reads the handler flags on its
 * next pass, exactly where it would after a retail interrupt.
 */
#include "psx_compat.h"
#include "pe_sdk.h"
#include "pe_memcard.h"
#include "pe_guest_ram.h"

#include <string.h>

extern int func_80042BD8(void);
extern int func_80042BEC(void);
extern int func_80042C00(void);
extern int func_80042C14(void);
extern int func_80042C28(void);
extern int func_80042C3C(void);
extern int func_80042C50(void);
extern int func_80042C64(void);

static void deliver(uint32_t cls, uint32_t spec)
{
    switch (PE_Event_DeliverCard(cls, spec)) {
    case 0x80042BD8u: (void)func_80042BD8(); break;
    case 0x80042BECu: (void)func_80042BEC(); break;
    case 0x80042C00u: (void)func_80042C00(); break;
    case 0x80042C14u: (void)func_80042C14(); break;
    case 0x80042C28u: (void)func_80042C28(); break;
    case 0x80042C3Cu: (void)func_80042C3C(); break;
    case 0x80042C50u: (void)func_80042C50(); break;
    case 0x80042C64u: (void)func_80042C64(); break;
    default: break;
    }
}

/* Guest NUL-terminated string -> host buffer (truncating). */
static void guest_string(pe_addr_t address, char *out, size_t cap)
{
    size_t i = 0;
    if (!address) { out[0] = 0; return; }
    for (; i + 1 < cap; i++) {
        uint8_t c = PE_LoadU8(address + (uint32_t)i);
        if (!c) break;
        out[i] = (char)c;
    }
    out[i] = 0;
}

static void store_direntry(pe_addr_t dir, const PeMcDirEntry *e)
{
    for (uint32_t i = 0; i < 20u; i++) PE_StoreU8(dir + i, (uint8_t)e->name[i]);
    PE_StoreU32(dir + 0x14u, e->attr);
    PE_StoreU32(dir + 0x18u, e->size);
    PE_StoreU32(dir + 0x1Cu, e->next);
    PE_StoreU32(dir + 0x20u, e->head);
    for (uint32_t i = 0; i < 4u; i++) PE_StoreU8(dir + 0x24u + i, e->system[i]);
}

/* ── libcard / kernel card service ─────────────────────────────────── */

/* B(0Bh) TestEvent */
int func_800726F4(uint32_t handle)
{
    return PE_Event_Test((int)handle);
}

/* A(ABh) _card_info */
int func_8007DD44(uint32_t chan)
{
    deliver(0xF4000001u, (uint32_t)PE_Memcard_CardInfoResult(chan));
    return 1;
}

/* A(ACh) _card_load */
int func_8007DD54(uint32_t chan)
{
    deliver(0xF4000001u, (uint32_t)PE_Memcard_CardLoadResult(chan));
    return 1;
}

/* B(50h) allow_new_card */
void func_8007DDC4(pe_addr_t chan)
{
    (void)chan;
}

/* B(4Eh) write_card_sector(chan, sector, src).  The only caller,
 * func_8007DD74, writes sector 3Fh (the write-test frame) to acknowledge a
 * new card; the host acknowledges without changing the image. */
int func_8007DDB4(pe_addr_t chan, int sector, int src)
{
    (void)sector; (void)src;
    deliver(0xF0000011u, (uint32_t)PE_Memcard_CardClearResult(chan));
    return 1;
}

/* ── BIOS file functions ───────────────────────────────────────────── */

/* B(32h) open */
int func_80072734(pe_addr_t name, uint32_t mode)
{
    char s[64];
    guest_string(name, s, sizeof s);
    return PE_Memcard_Open(s, mode);
}

/* B(33h) lseek */
int func_80072744(int fd, int32_t offset, int whence)
{
    return PE_Memcard_Lseek(fd, offset, whence);
}

/* B(34h) read */
int func_80072754(int fd, pe_addr_t dst, int32_t length)
{
    uint8_t buf[PE_MC_BLOCK_SIZE];
    int n;
    if (length < 0 || length > (int32_t)sizeof buf) return -1;
    n = PE_Memcard_Read(fd, buf, length);
    for (int i = 0; i < n; i++) PE_StoreU8(dst + (uint32_t)i, buf[i]);
    return n;
}

/* B(35h) write */
int func_80072764(int fd, pe_addr_t src, int32_t length)
{
    uint8_t buf[PE_MC_BLOCK_SIZE];
    if (length < 0 || length > (int32_t)sizeof buf) return -1;
    for (int32_t i = 0; i < length; i++) buf[i] = PE_LoadU8(src + (uint32_t)i);
    return PE_Memcard_Write(fd, buf, length);
}

/* B(36h) close */
int func_80072774(int fd)
{
    return PE_Memcard_Close(fd);
}

/* B(41h) format */
int func_80072784(pe_addr_t device)
{
    char s[64];
    guest_string(device, s, sizeof s);
    return PE_Memcard_Format(s);
}

/* B(42h) firstfile(name, direntry): the result the libapi wrapper at
 * 0x800727B4 returns (the direntry address, or 0). */
pe_addr_t PE_Bios_FirstFile(pe_addr_t pattern, pe_addr_t dir)
{
    char s[64];
    PeMcDirEntry e;
    guest_string(pattern, s, sizeof s);
    if (!PE_Memcard_FirstFile(s, &e)) return 0;
    store_direntry(dir, &e);
    return dir;
}

/* B(43h) nextfile */
pe_addr_t func_80072794(pe_addr_t dir)
{
    PeMcDirEntry e;
    if (!PE_Memcard_NextFile(&e)) return 0;
    store_direntry(dir, &e);
    return dir;
}

/* B(45h) delete */
int func_800727A4(pe_addr_t name)
{
    char s[64];
    guest_string(name, s, sizeof s);
    return PE_Memcard_Delete(s);
}
