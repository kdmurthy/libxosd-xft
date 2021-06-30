/*

Copyright 2021 Dakshinamurthy Karra (dakshinamurthy.karra@jaliansystems.com)

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.

*/

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <strings.h>
#include <ctype.h>

#define isodigit(c) ((c) >= '0' && (c) <= '7')
#define hextobin(c) ((c) >= 'a' && (c) <= 'f' ? (c) - 'a' + 10 : (c) >= 'A' && (c) <= 'F' ? (c) - 'A' + 10 \
                                                                                          : (c) - '0')
#define octtobin(c) ((c) - '0')

#ifndef FALLTHROUGH
#if __GNUC__ < 7
#define FALLTHROUGH ((void)0)
#else
#define FALLTHROUGH __attribute__((__fallthrough__))
#endif
#endif

static inline unsigned char to_uchar(char ch) { return ch; }

int u8_uctomb(char *s, unsigned int uc, int n)
{
  if (uc < 0x80)
  {
    if (n > 0)
    {
      s[0] = uc;
      return 1;
    }
    /* else return -2, below.  */
  }
  else
  {
    int count;

    if (uc < 0x800)
      count = 2;
    else if (uc < 0x10000)
    {
      if (uc < 0xd800 || uc >= 0xe000)
        count = 3;
      else
        return -1;
    }
    else if (uc < 0x110000)
      count = 4;
    else
      return -1;

    if (n >= count)
    {
      switch (count) /* note: code falls through cases! */
      {
      case 4:
        s[3] = 0x80 | (uc & 0x3f);
        uc = uc >> 6;
        uc |= 0x10000;
        FALLTHROUGH;
      case 3:
        s[2] = 0x80 | (uc & 0x3f);
        uc = uc >> 6;
        uc |= 0x800;
        FALLTHROUGH;
      case 2:
        s[1] = 0x80 | (uc & 0x3f);
        uc = uc >> 6;
        uc |= 0xc0;
        /*case 1:*/ s[0] = uc;
      }
      return count;
    }
  }
  return -2;
}

static int
print_unicode_char(unsigned int code, char **b)
{
  char inbuf[8];
  memset(inbuf, 0, sizeof(inbuf));
  int count;

  count = u8_uctomb((char *)inbuf, code, sizeof(inbuf));
  if (count < 0)
    return -3;
  memcpy(*b, inbuf, count);
  *b += count;
  return count;
}

static int
print_esc(const char *escstart, char **b)
{
  const char *p = escstart + 1;
  int esc_length; /* Length of \nnn escape. */

  if (*p == 'u' || *p == 'U')
  {
    char esc_char = *p;
    unsigned int uni_value;

    uni_value = 0;
    for (esc_length = (esc_char == 'u' ? 4 : 8), ++p;
         esc_length > 0;
         --esc_length, ++p)
    {
      if (!isxdigit(to_uchar(*p)))
        return -1;
      uni_value = uni_value * 16 + hextobin(*p);
    }

    /* A universal character name shall not specify a character short
         identifier in the range 00000000 through 00000020, 0000007F through
         0000009F, or 0000D800 through 0000DFFF inclusive. A universal
         character name shall not designate a character in the required
         character set.  */
    if ((uni_value <= 0x9f && uni_value != 0x24 && uni_value != 0x40 && uni_value != 0x60) || (uni_value >= 0xd800 && uni_value <= 0xdfff))
      return -2;

    print_unicode_char(uni_value, b);
  }
  else
  {
    char *c = *b;
    int i = 1;
    *c++ = '\\';
    if (*p)
    {
      i++;
      *c++ = *p++;
    }
    *b += i;
  }
  return p - escstart - 1;
}

#define UTF8_BUFFER_LENGTH 4096

char *print_utf8(const char *s)
{
  char buffer[UTF8_BUFFER_LENGTH];
  char *p = &buffer[0];

  while (*s)
  {
    if (*s == '\\')
    {
      int ret = print_esc(s, &p);
      if (ret < 0)
      {
        fprintf(stderr, "\nError: %d\n", ret);
        s += 1;
      }
      else
      {
        s += ret + 1;
      }
    }
    else
    {
      *p++ = *s++;
    }
  }
  *p = 0;
  return strdup(buffer);
}

#ifdef MAIN
int main(int argc, char **argv)
{
  for (int i = 1; i < argc; i++)
  {
    char *s = print_utf8(argv[i]);
    puts(s);
    free(s);
  }
  exit(0);
}
#endif

/* vim: foldmethod=marker tabstop=2 shiftwidth=2 expandtab
 */
