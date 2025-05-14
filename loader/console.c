// Copyright 2009  Segher Boessenkool  <segher@kernel.crashing.org>
// This code is licensed to you under the terms of the GNU GPL, version 2;
// see file COPYING or http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt


#include <stdarg.h>

#include "loader.h"


static void put(char c)
{
	fb_putc(c);
}


// __umoddi3() and friends are very big, and more general than we need:
// radix is always (very) small, so we can work by much bigger chunks
// than single bits, always.
static int extract_dig(u64 *x, u32 radix)
{
	u32 hi = *x >> 32;
	u32 lo = *x;
	u32 mod = hi % radix;
	hi /= radix;
	u32 n = (mod << 16) | (lo >> 16);
	mod = n % radix;
	n /= radix;
	lo = (mod << 16) | (lo & 0xffff);
	mod = lo % radix;
	lo /= radix;
	lo |= (n << 16);
	*x = ((u64)hi << 32) | lo;
	return mod;
}


// This implements conversions %{0}{number}{l,ll}[%cdsux] only.
// Field length is obeyed for numbers only.
// Always returns 0.

int printf(const char *restrict format, ...)
{
    va_list ap;
    va_start(ap, format);
    char buf[128];
    while (*format) {
        if (*format != '%') {
            put(*format++);
            continue;
        }
        format++;
        char spec = *format++;
        switch (spec) {
        case '%':
            put('%');
            break;
        case 'c': {
            char c = (char)va_arg(ap, int);
            put(c);
            break;
        }
        case 's': {
            char *s = va_arg(ap, char *);
            if (!s) {
                const char *err = "[NULL]";
                while (*err) put(*err++);
            } else {
                while (*s)
                    put(*s++);
            }
            break;
        }
        case 'd': {
            int n = va_arg(ap, int);
            int len = snprintf(buf, sizeof(buf), "%d", n);
            if (len < 0 || len >= (int)sizeof(buf)) {
                const char *err = "[ERR]";
                while (*err) put(*err++);
            } else {
                for (int i = 0; i < len; ++i) put(buf[i]);
            }
            break;
        }
        case 'u': {
            unsigned int n = va_arg(ap, unsigned int);
            int len = snprintf(buf, sizeof(buf), "%u", n);
            if (len < 0 || len >= (int)sizeof(buf)) {
                const char *err = "[ERR]";
                while (*err) put(*err++);
            } else {
                for (int i = 0; i < len; ++i) put(buf[i]);
            }
            break;
        }
        case 'x': {
            unsigned int n = va_arg(ap, unsigned int);
            int len = snprintf(buf, sizeof(buf), "%x", n);
            if (len < 0 || len >= (int)sizeof(buf)) {
                const char *err = "[ERR]";
                while (*err) put(*err++);
            } else {
                for (int i = 0; i < len; ++i) put(buf[i]);
            }
            break;
        }
        default: {
            const char *err = "[UNSUPPORTED]";
            put('%');
            while (*err) put(*err++);
            put(spec);
            break;
        }
        }
    }
    va_end(ap);
    return 0;
}

