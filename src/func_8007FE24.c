/* VRAM 0x8007FE24 / file 0x70624 / size 0x2D0.
 *
 * libds per-vsync state machine: the command timer (func_800800F4 on
 * expiry), two wait counters, the state-2 command dispatch (11..17 as a
 * compare chain, sub-commands 21..24 under 14), the ready-callback hook,
 * and the final retry kick. cc1 2.8.1 -O2 -G0 -mno-split-addresses +
 * maspsx patch 8 (absolute-store splits into j/jal slots) + patch 14
 * (load-delay nop before the cross-jump label at 0x80080038). Profile
 * era_o2_g0_cc1_281_fill_call_delay_slot_load_delay_nop_before_label.
 * Levers: every "timer <= 0 -> kick" is written inline (KICK) so cc1
 * cross-jumps the identical tails into the shared kick exactly like retail
 * (cmd 13 -> 0x80080038, sub 22 -> 0x80080040, sub 23 -> 0x80080048);
 * the cmd-16/17 tests put the non-zero (reset) arm first; the later timer
 * reads go through D_8009B598 (the struct's +0x28 field, which retail reads
 * absolutely, not through the $a0 struct base); the callback pointer
 * D_800A36A0 is volatile (retail reloads it before the jalr); a buffer
 * pointer local for the cmd-12 func_8007FCFC(14, buf) call.
 */
typedef struct {
    int intr;                      /* 0x00 D_8009B570 */
    int state;                     /* 0x04 D_8009B574 */
    int cmd;                       /* 0x08 D_8009B578 */
    int sub;                       /* 0x0C D_8009B57C */
    int pad10[2];                  /* 0x10 */
    unsigned char f18;             /* 0x18 D_8009B588 */
    unsigned char f19;
    unsigned char f1A;             /* 0x1A D_8009B58A */
    unsigned char f1B;             /* 0x1B D_8009B58B */
    int cnt;                       /* 0x1C D_8009B58C */
    int pad20;                     /* 0x20 */
    int wait;                      /* 0x24 D_8009B594 */
    int timer;                     /* 0x28 D_8009B598 */
    int pad2C;                     /* 0x2C */
    int wait2;                     /* 0x30 D_8009B5A0 */
} DsSys;

extern DsSys D_8009B570;
extern int D_8009B6A4;
extern int D_8009B598;
extern int D_8009B554;
extern void (*volatile D_800A36A0)(void);
extern void func_800800F4(void);
extern void func_8007FCFC(int, unsigned char *);

#define KICK(c)                                  \
    if (D_8009B598 <= 0) {                       \
        D_8009B570.intr = 32;                    \
        func_8007FCFC(c, 0);                     \
    }

void func_8007FE24(void) {
    unsigned char buf[8];
    int cmd;
    int sub;
    int eleven;

    if (D_8009B570.timer > 0) {
        if (--D_8009B570.timer == 0) {
            func_800800F4();
            return;
        }
    }
    if (D_8009B570.wait > 0) {
        D_8009B570.wait--;
    }
    if (D_8009B570.wait2 > 0) {
        D_8009B570.wait2--;
    }
    if (D_8009B570.state == 1) {
    } else if (D_8009B570.state == 2) {
        cmd = D_8009B570.cmd;
        eleven = 11;
        if (cmd == eleven) {
        } else if (cmd == 12) {
            if (D_8009B6A4 != 0) {
                unsigned char *bp = buf;
                bp[0] = 0;
                if (D_8009B598 <= 0) {
                    D_8009B570.intr = 32;
                    func_8007FCFC(14, bp);
                }
                D_8009B6A4 = 0;
            } else {
                KICK(1)
                D_8009B6A4 = 1;
            }
        } else if (cmd == 13) {
            D_8009B6A4 = 0;
            KICK(1)
        } else if (cmd == 14) {
            sub = D_8009B570.sub;
            if (sub == 21) {
                KICK(1)
            } else if (sub == 22) {
                D_8009B570.cnt++;
                KICK(1)
            } else if (sub == 23) {
                KICK(19)
            } else if (sub == 24) {
                KICK(1)
            }
        } else if (cmd == 15) {
            if (D_8009B570.wait == 0) {
                D_8009B570.state = 1;
                D_8009B570.cmd = eleven;
            }
        } else if (cmd == 16) {
            if (D_8009B570.f18 != 0) {
                D_8009B570.state = 1;
                D_8009B570.cmd = eleven;
            } else {
                KICK(1)
            }
        } else if (cmd == 17) {
            if (D_8009B570.f1A != 0) {
                D_8009B570.state = 1;
                D_8009B570.cmd = eleven;
            } else {
                KICK(1)
            }
        }
    }
    if (D_800A36A0 != 0 && D_8009B554 != 0) {
        D_800A36A0();
    }
    if ((D_8009B570.state == 1 && D_8009B570.f1B == 0) || D_8009B570.state == 3) {
        if (D_8009B598 <= 0) {
            D_8009B570.intr = 33;
            func_8007FCFC(1, 0);
        }
    }
}
