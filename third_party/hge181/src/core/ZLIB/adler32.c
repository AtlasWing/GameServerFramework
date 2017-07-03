/* adler32.c -- compute the Adler-32 checksum of a data stream
 * Copyright (C) 1995-2003 Mark Adler
 * For conditions of distribution and use, see copyright notice in zlib.h
 */

/* @(#) $Id: adler32.c,v 1.1 2005/11/23 14:29:59 stingerx Exp $ */

#define ZLIB_INTERNAL

#include "zlib.h"


#define DO1(buf,i)  { s1 += buf[i]; s2 += s1; }
#define DO2(buf,i)  DO1(buf,i); DO1(buf,i+1);
#define DO4(buf,i)  DO2(buf,i); DO2(buf,i+2);
#define DO8(buf,i)  DO4(buf,i); DO4(buf,i+4);
#define DO16(buf)   DO8(buf,0); DO8(buf,8);

/* ========================================================================= */

DWORD ZEXPORT adler32(DWORD adler, const BYTE *buf, DWORD len)
{
  DWORD s1 = adler &0xFFFF;
  DWORD s2 = (adler >> 16) &0xFFFF;
  DWORD k;

  if (buf == Z_NULL)
  {
    return 1;
  }

  while (len)
  {
    k = len < 5552 ? len : 5552;

    len -= k;

    while (k >= 16)
    {
      DO16(buf);
      buf += 16;
      k -= 16;
    }

    while (k)
    {
      s1 +=  *buf++;
      s2 += s1;
      k--;
    }

    s1 = s1 % 65521;
    s2 = s2 % 65521;
  }

  return (s2 << 16) | s1;
}
