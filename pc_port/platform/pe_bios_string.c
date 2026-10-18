/* BIOS A-function string/memory adapters; contract and source in
 * pe_bios_string.h.  Quotes are verbatim from psx-spx "BIOS String
 * Functions" / "BIOS Memory Fill/Copy/Compare". */
#include "pe_bios_string.h"
#include <stdio.h>
#include <string.h>

/* Byte values are compared as loaded unsigned bytes; psx-spx specifies the
 * result as "[str1+N]-[str2+N]" without stating signedness, and every
 * matched caller (func_800727B4, func_80072950, ...) only tests == 0. */

/* A(17h) strcmp: "Compares the strings up to including ending 00h byte.
 * Returns 0 if they are identical, or otherwise [str1+N]-[str2+N] ... The
 * function rejects str1/str2 values of 00000000h (and returns 0=both are
 * zero, -1=only str1 is zero, and +1=only str2 is zero)." */
int32_t func_80072A54(pe_addr_t str1, pe_addr_t str2)
{
    if (str1 == 0 || str2 == 0)
        return (str1 == 0 && str2 == 0) ? 0 : (str1 == 0 ? -1 : 1);
    for (uint32_t n = 0;; n++) {
        int32_t a = PE_LoadU8(str1 + n), b = PE_LoadU8(str2 + n);
        if (a != b)
            return a - b;
        if (a == 0)
            return 0;
    }
}

/* A(18h) strncmp: "Same as 'strcmp' but stops after comparing 'maxlen'
 * characters (and returns 0 if they did match). If the strings are shorter,
 * then comparision stops at the ending 00h byte."  ("Same as strcmp" also
 * covers the 00000000h rejection.) */
int32_t func_80071A04(pe_addr_t str1, pe_addr_t str2, int32_t maxlen)
{
    if (str1 == 0 || str2 == 0)
        return (str1 == 0 && str2 == 0) ? 0 : (str1 == 0 ? -1 : 1);
    for (int32_t n = 0; n < maxlen; n++) {
        int32_t a = PE_LoadU8(str1 + (uint32_t)n), b = PE_LoadU8(str2 + (uint32_t)n);
        if (a != b)
            return a - b;
        if (a == 0)
            return 0;
    }
    return 0;
}

/* A(19h) strcpy: "Copies data from src to dst, up to including the ending
 * 00h byte. Refuses to copy anything if src or dst is 00000000h. Returns
 * the incoming dst address (or 0 if copy was refused)." */
pe_addr_t func_80071A14(pe_addr_t dst, pe_addr_t src)
{
    uint8_t c;
    uint32_t n = 0;

    if (dst == 0 || src == 0)
        return 0;
    do {
        c = PE_LoadU8(src + n);
        PE_StoreU8(dst + n, c);
        n++;
    } while (c != 0);
    return dst;
}

/* A(1Bh) strlen: "Returns the length of the string up to excluding the
 * ending 00h byte (or 0 when src is 00000000h)." */
int32_t func_80072314(pe_addr_t src)
{
    int32_t n = 0;

    if (src == 0)
        return 0;
    while (PE_LoadU8(src + (uint32_t)n) != 0)
        n++;
    return n;
}

/* A(2Ah) memcpy: "Refuses to copy any data when dst=00000000h or when
 * len>7FFFFFFFh. The return value is always the incoming 'dst' value." */
pe_addr_t func_80071A34(pe_addr_t dst, pe_addr_t src, int32_t len)
{
    if (dst != 0 && len >= 0) {
        for (int32_t n = 0; n < len; n++)
            PE_StoreU8(dst + (uint32_t)n, PE_LoadU8(src + (uint32_t)n));
    }
    return dst;
}

/* A(2Bh) memset: "Refuses to fill memory when dst=00000000h or when
 * len>7FFFFFFFh. The return value is the incoming 'dst' value (or zero,
 * when len=0 or len>7FFFFFFFh)." */
pe_addr_t func_80071A44(pe_addr_t dst, int32_t fillbyte, int32_t len)
{
    if (len <= 0)
        return 0;
    if (dst == 0)
        return dst;
    for (int32_t n = 0; n < len; n++)
        PE_StoreU8(dst + (uint32_t)n, (uint8_t)fillbyte);
    return dst;
}

/* Host sprintf (see pe_bios_string.h).  Standard C formatting semantics. */
int32_t PE_Bios_SprintfHost(char *dst, uint32_t cap, pe_addr_t fmt, const uint32_t *args, int nargs)
{
    uint32_t out = 0, f = fmt;
    if (!cap) return 0;
    int ai = 0;
#define NEXT_ARG() (ai < nargs ? args[ai++] : 0u)
#define EMIT(ch) do { if (out + 1u < cap) dst[out] = (char)(ch); out++; } while (0)
    for (;;) {
        char c = (char)PE_LoadU8(f++);
        char digits[40];
        int n = 0, left = 0, zero = 0, plus = 0, space = 0, width = 0, prec = -1;
        int neg = 0;
        uint32_t v;
        if (!c) break;
        if (c != '%') { EMIT(c); continue; }
        for (;;) {
            c = (char)PE_LoadU8(f++);
            if (c == '-') left = 1;
            else if (c == '0') zero = 1;
            else if (c == '+') plus = 1;
            else if (c == ' ') space = 1;
            else if (c == '#') {}
            else break;
        }
        if (c == '*') { width = (int32_t)NEXT_ARG(); c = (char)PE_LoadU8(f++); }
        else while (c >= '0' && c <= '9') { width = width * 10 + (c - '0'); c = (char)PE_LoadU8(f++); }
        if (c == '.') {
            prec = 0; c = (char)PE_LoadU8(f++);
            if (c == '*') { prec = (int32_t)NEXT_ARG(); c = (char)PE_LoadU8(f++); }
            else while (c >= '0' && c <= '9') { prec = prec * 10 + (c - '0'); c = (char)PE_LoadU8(f++); }
        }
        while (c == 'h' || c == 'l' || c == 'L') c = (char)PE_LoadU8(f++);
        if (width < 0) { left = 1; width = -width; }
        switch (c) {
        case 'c':
            digits[n++] = (char)NEXT_ARG();
            break;
        case 's': {
            pe_addr_t s = NEXT_ARG();
            int len = 0;
            while (s && PE_LoadU8(s + (uint32_t)len) && (prec < 0 || len < prec)) len++;
            if (!left) for (int i = len; i < width; i++) EMIT(' ');
            for (int i = 0; i < len; i++) EMIT(PE_LoadU8(s + (uint32_t)i));
            if (left) for (int i = len; i < width; i++) EMIT(' ');
            continue;
        }
        case 'd': case 'i': case 'u': case 'x': case 'X': case 'o': case 'p': {
            unsigned base = (c == 'x' || c == 'X' || c == 'p') ? 16u : c == 'o' ? 8u : 10u;
            const char *hex = c == 'X' ? "0123456789ABCDEF" : "0123456789abcdef";
            v = NEXT_ARG();
            if ((c == 'd' || c == 'i') && (int32_t)v < 0) { neg = 1; v = 0u - v; }
            do { digits[n++] = hex[v % base]; v /= base; } while (v && n < 32);
            while (prec >= 0 && n < prec && n < 32) digits[n++] = '0';
            if (neg) digits[n++] = '-';
            else if ((c == 'd' || c == 'i') && plus) digits[n++] = '+';
            else if ((c == 'd' || c == 'i') && space) digits[n++] = ' ';
            if (zero && !left && prec < 0) {
                int sign = neg || ((c == 'd' || c == 'i') && (plus || space));
                char s = sign ? digits[--n] : 0;
                while (n + sign < width && n < 38) digits[n++] = '0';
                if (sign) digits[n++] = s;
            }
            /* digits are reversed */
            for (int i = 0; i < n / 2; i++) { char t = digits[i]; digits[i] = digits[n - 1 - i]; digits[n - 1 - i] = t; }
            break;
        }
        case '%':
            digits[n++] = '%';
            break;
        case 0:
            f--;
            continue;
        default:
            digits[n++] = c;
            break;
        }
        if (!left) for (int i = n; i < width; i++) EMIT(' ');
        for (int i = 0; i < n; i++) EMIT(digits[i]);
        if (left) for (int i = n; i < width; i++) EMIT(' ');
    }
    dst[out < cap ? out : cap - 1u] = 0;
    return (int32_t)out;
#undef NEXT_ARG
#undef EMIT
}

int32_t PE_Bios_Sprintf(pe_addr_t dst, pe_addr_t fmt, const uint32_t *args, int nargs)
{
    char buf[1024];
    int32_t n = PE_Bios_SprintfHost(buf, sizeof buf, fmt, args, nargs);
    uint32_t len = (uint32_t)n < sizeof buf ? (uint32_t)n : (uint32_t)sizeof buf - 1u;
    for (uint32_t i = 0; i < len; i++) PE_StoreU8(dst + i, (uint8_t)buf[i]);
    PE_StoreU8(dst + len, 0u);
    return n;
}

static struct { unsigned n; pe_addr_t fmt; uint32_t args[4]; } g_printf_log;

unsigned PE_Bios_PrintfLog(pe_addr_t *last_fmt, uint32_t last_args[4])
{
    if (last_fmt) *last_fmt = g_printf_log.fmt;
    if (last_args) memcpy(last_args, g_printf_log.args, sizeof g_printf_log.args);
    return g_printf_log.n;
}

void PE_Bios_PrintfLogReset(void) { memset(&g_printf_log, 0, sizeof g_printf_log); }

int32_t PE_Bios_Printf(pe_addr_t fmt, const uint32_t *args, int nargs)
{
    char buf[1024];
    g_printf_log.n++;
    g_printf_log.fmt = fmt;
    memset(g_printf_log.args, 0, sizeof g_printf_log.args);
    for (int i = 0; i < nargs && i < 4; i++) g_printf_log.args[i] = args[i];
    int32_t n = PE_Bios_SprintfHost(buf, sizeof buf, fmt, args, nargs);
    size_t len = strlen(buf);
    fprintf(stderr, "[TTY] %s%s", buf, (len && buf[len - 1u] == '\n') ? "" : "\n");
    return n;
}

/* Host replacement for func_80071A74 (printf through the BIOS A(3Fh) vector;
 * pe_port_compat.h): lowered code calls it with the guest format and up to
 * seven word arguments (0 past the call site's own). */
int32_t PE_Printf_Guest(pe_addr_t fmt, uint32_t a1, uint32_t a2, uint32_t a3,
                        uint32_t a4, uint32_t a5, uint32_t a6, uint32_t a7)
{
    const uint32_t args[7] = {a1, a2, a3, a4, a5, a6, a7};
    return PE_Bios_Printf(fmt, args, 7);
}
