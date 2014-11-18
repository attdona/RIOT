/*
 * printf.c
 *
 *  Created on: 04/nov/2014
 *      Author: so000112
 */

#include <stdarg.h>

int putchar(int c);

void puts(const char *);

void puts(const char* s) {
	char c;

    // Loops through each character in string 's'
    while ((c = *s++)) {
        putchar(c);
    }
}

static const unsigned long dv[] = {
    //  4294967296      // 32 bit unsigned max
    1000000000,     // +0
    100000000,     // +1
    10000000,     // +2
    1000000,     // +3
    100000,     // +4
    //       65535      // 16 bit unsigned max
    10000,     // +5
    1000,     // +6
    100,     // +7
    10,     // +8
    1,     // +9
};

static void xtoa(unsigned long x, const unsigned long *dp)
{
    char c;
    unsigned long d;

    if (x) {
        while (x < *dp) {
            ++dp;
        }

        do {
            d = *dp++;
            c = '0';

            while (x >= d) {
                ++c, x -= d;
            }

            putchar(c);
        }
        while (!(d & 1));
    }
    else {
        putchar('0');
    }
}

static void puth(unsigned n)
{
    static const char hex[16] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};
    putchar(hex[n & 15]);
}

void printf(char *format, ...)
{
    char c;
    int i;
    long n;

    va_list a;
    va_start(a, format);

    while ((c = *format++)) {
        if (c == '%') {
        	c = *format++;

        	// RIOT uses %-xxs printf format
        	if (c == '-') {
        		c = *format++;
        		while ((c <= '9') && (c >= '0')) {
        			c = *format++;
        		}
        	}

            switch (c) {
                case 's':                       // String
                    puts(va_arg(a, char *));
                    break;

                case 'c':                       // Char
                    putchar(va_arg(a, int));
                    break;

                case 'i':                       // 16 bit Integer
                case 'u':                       // 16 bit Unsigned
                    i = va_arg(a, int);

                    if (c == 'i' && i < 0) {
                        i = -i, putchar('-');
                    }

                    xtoa((unsigned)i, dv + 5);
                    break;

                case 'l':                       // 32 bit Long
                case 'n':                       // 32 bit uNsigned loNg
                    n = va_arg(a, long);

                    if (c == 'l' &&  n < 0) {
                        n = -n, putchar('-');
                    }

                    xtoa((unsigned long)n, dv);
                    break;

                case 'x':                       // 16 bit heXadecimal
                    i = va_arg(a, int);
                    puth(i >> 12);
                    puth(i >> 8);
                    puth(i >> 4);
                    puth(i);
                    break;

                case 0:
                    return;

                default:
                    goto bad_fmt;
            }
        }
        else {
        bad_fmt:
            putchar(c);
        }
    }

    va_end(a);
}

