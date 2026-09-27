/*
 * func_800830DC / func_800832B4 — memory-card byte exchange over SIO0, from
 * the matched src/func_800830DC.c and src/func_800832B4.c.  D_8009B788 is
 * the SIO0 base (0x1F801040 in retail data) and D_8009B784 the I_STAT word
 * (0x1F801070); both reach the device models through the MMIO table
 * (pc_port/platform/pe_mmio.h, pe_sio0.h, pe_rcnt2.h), with every retail
 * access width kept: DATA lbu/sb, STAT lhu, BAUD sh (+0xE), I_STAT lw,
 * RCNT2 count/target/mode lhu (pe_dis.sh 0x800830DC 0x1D8, 0x800832B4 0x234).
 *
 * Busy waits.  Each iteration of a retail poll loop is one guest poll: it
 * advances device time exactly like the port's other poll loops
 * (HostFB_VSync(-1): the 1024-cycle counter-query quantum; when the CD
 * device clock is disabled that quantum is applied to RCNT2 directly, so the
 * RCNT2 timeouts still elapse).  With no memory card attached (pe_sio0.h),
 * /ACK never arrives (I_STAT bit 7 stays clear), so the exchanges end on
 * their RCNT2 timeouts as on retail.  A status wait that cannot change under
 * the model (e.g. RXRDY with nothing sent) would spin forever on retail; it
 * is bounded by PE_SIO_POLL_LIMIT polls and then requests a run-control stop
 * (PE_PORT_STOP_EXPLICIT), never a silent spin.
 */
#include "pe_guest_decomp.h"
#include "host_framebuffer.h"
#include "pe_cdreg.h"
#include "pe_rcnt2.h"
#include "game_port.h"

#define GA_SIO  0x8009B788u   /* volatile unsigned char *D_8009B788 */
#define GA_ISTP 0x8009B784u   /* volatile unsigned int *D_8009B784 */
#define PE_SIO_POLL_LIMIT 0x100000u

static void sio_poll_tick(void)
{
    (void)HostFB_VSync(-1);
    if (!PE_CdReg_DeviceEnabled())
        PE_Rcnt2_Advance(1024u);
}

/* Wait until STAT (lhu +4) has `bit`; bounded, see the header. */
static void sio_wait_stat(uint16_t bit)
{
    uint32_t n = 0;

    while (!(PE_LoadU16(PE_LoadU32(GA_SIO) + 4u) & bit)) {
        if (PE_Port_ShouldStop())
            return;
        if (++n >= PE_SIO_POLL_LIMIT) {
            PE_Port_RequestStop(PE_PORT_STOP_EXPLICIT);
            return;
        }
        sio_poll_tick();
    }
}

static int sio_irq7(void)
{
    return (PE_LoadU32(PE_LoadU32(GA_ISTP)) & 0x80u) != 0;
}

/* src/func_800830DC.c: send a1 and collect the byte that arrives. */
int func_800830DC(pe_addr_t a0, int a1)
{
    int r;
    int mode;
    uint16_t v;
    unsigned int t;

    if (a1 < 0) {
        r = PE_LoadU8(PE_LoadU32(GA_SIO));
        PE_StoreU8(a0 + 0x44u, 0xFFu);
        PE_StoreU8(a0 + 0x45u, 1u);
        PE_StoreU8(PE_LoadU32(a0 + 0x40u), (uint8_t)~a1);
        sio_wait_stat(1u);
        while (func_80084FE4() == 0) {
            if (PE_Port_ShouldStop())
                break;
            sio_poll_tick();
        }
        PE_StoreU8(PE_LoadU32(GA_SIO), (uint8_t)~a1);
        return r;
    }
    mode = 0x88;
    {
        int w = PE_LoadU8(PE_LoadU32(a0 + 0x3Cu));

        if ((w >> 4) == 8 && PE_LoadU8(a0 + 0x44u) >= 9u)
            mode = 0x22;
    }
    t = PE_LoadU16(0x1F801120u);                     /* RCNT2 count */
    v = PE_LoadU16(PE_LoadU32(GA_SIO) + 4u);
    PE_StoreU32(0x800BD02Cu, 0x1AEu);
    PE_StoreU32(0x800A76D0u, t);
    if (!(v & 2u))
        sio_wait_stat(2u);
    r = PE_LoadU8(PE_LoadU32(GA_SIO));
    PE_StoreU16(PE_LoadU32(GA_SIO) + 0xEu, (uint16_t)(short)mode);
    if (!sio_irq7()) {
        do {
            if (func_80084FE4() != 0)
                return -0x14;
            if (PE_Port_ShouldStop())
                return -0x14;
            sio_poll_tick();
        } while (!sio_irq7());
    }
    PE_StoreU8(PE_LoadU32(GA_SIO), (uint8_t)a1);
    PE_StoreU8(a0 + 0x45u, (uint8_t)(PE_LoadU8(a0 + 0x45u) + 1u));
    PE_StoreU8(PE_LoadU32(a0 + 0x3Cu) + PE_LoadU8(a0 + 0x44u), (uint8_t)r);
    PE_StoreU8(a0 + 0x44u, (uint8_t)(PE_LoadU8(a0 + 0x44u) + 1u));
    return r;
}

/* src/func_800832B4.c: collect the byte of the previous send, then send a1. */
int func_800832B4(pe_addr_t a0, int a1)
{
    int mode;
    int r;
    pe_addr_t b;

    mode = 0x88;
    {
        int w = PE_LoadU8(PE_LoadU32(a0 + 0x3Cu));

        if ((w >> 4) == 8 && PE_LoadU8(a0 + 0x44u) >= 9u)
            mode = 0x22;
    }
    sio_wait_stat(2u);
    func_80084FC4(0x190u);
    b = PE_LoadU32(GA_SIO);
    r = PE_LoadU8(b);
    if (PE_LoadU8(a0 + 0x44u) != 0 || (r >> 4) != 8)
        PE_StoreU16(b + 0xEu, (uint16_t)(short)mode);
    else
        PE_StoreU16(b + 0xEu, 0x22u);
    if (!sio_irq7()) {
        do {
            unsigned int t = PE_LoadU16(0x1F801120u);            /* count */

            if (t < PE_LoadU32(0x800A76D0u)) {
                if (PE_LoadU16(0x1F801128u) != 0)                /* target */
                    t = PE_LoadU16(0x1F801128u) + t;
                else
                    t += 0x10000u;
            }
            if (PE_LoadU16(0x1F801124u) & 0x200u) {              /* mode */
                if (t - PE_LoadU32(0x800A76D0u) >= PE_LoadU32(0x800BD02Cu))
                    return -2;
            }
            if ((t - PE_LoadU32(0x800A76D0u)) >> 3 >= PE_LoadU32(0x800BD02Cu))
                return -2;
            if (PE_Port_ShouldStop())
                return -2;
            sio_poll_tick();
        } while (!sio_irq7());
    }
    if (PE_LoadU8(a0 + 0xE8u) != 8u && (int)PE_LoadU32(0x8009B768u) == 2) {
        func_80084FC4(0x3Cu);
        while (func_80084FE4() == 0) {
            if (PE_Port_ShouldStop())
                break;
            sio_poll_tick();
        }
    }
    PE_StoreU8(PE_LoadU32(GA_SIO), (uint8_t)a1);
    PE_StoreU8(a0 + 0x45u, (uint8_t)(PE_LoadU8(a0 + 0x45u) + 1u));
    if (PE_LoadU8(a0 + 0x44u) != 0xFFu)
        PE_StoreU8(PE_LoadU32(a0 + 0x3Cu) + PE_LoadU8(a0 + 0x44u), (uint8_t)r);
    PE_StoreU8(a0 + 0x44u, (uint8_t)(PE_LoadU8(a0 + 0x44u) + 1u));
    return r;
}
