/* BIOS A-function string/memory adapters; contract and source in
 * pe_bios_string.h.  Quotes are verbatim from psx-spx "BIOS String
 * Functions" / "BIOS Memory Fill/Copy/Compare". */
#include "pe_bios_string.h"

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
