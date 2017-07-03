/* zutil.c -- target dependent utility functions for the compression library
 * Copyright (C) 1995-2003 Jean-loup Gailly.
 * For conditions of distribution and use, see copyright notice in zlib.h
 */

/* @(#) $Id: zutil.c,v 1.1 2005/11/23 14:29:59 stingerx Exp $ */

#include "zutil.h"

#ifndef NO_DUMMY_DECL
  struct internal_state
  {
    int dummy;
  }; /* for buggy compilers */
#endif

#ifndef STDC
  extern void exit OF((int));
#endif

const char *const z_errmsg[10] =
{
  "need dictionary",  /* Z_NEED_DICT       2  */
  "stream end",  /* Z_STREAM_END      1  */
  "",  /* Z_OK              0  */
  "file error",  /* Z_ERRNO         (-1) */
  "stream error",  /* Z_STREAM_ERROR  (-2) */
  "data error",  /* Z_DATA_ERROR    (-3) */
  "insufficient memory",  /* Z_MEM_ERROR     (-4) */
  "buffer error",  /* Z_BUF_ERROR     (-5) */
  "incompatible version",  /* Z_VERSION_ERROR (-6) */
  ""
};


const char *ZEXPORT zlibZIPVersion()
{
  return ZLIB_VERSION;
}

//-------------------------------------------------------------------------

DWORD ZEXPORT zlibCompileFlags()
{
  DWORD flags;

  flags = 0;
  flags += 1;
  flags += 1 << 2;
  flags += 1 << 4;


  switch (sizeof(z_off_t))
  {
    case 2:
      break;
    case 4:
      flags += 1 << 6;
      break;
    case 8:
      flags += 2 << 6;
      break;
      default: flags += 3 << 6;
  }

  #ifdef DEBUG
    flags += 1 << 8;
  #endif

  #if defined(ASMV) || defined(ASMINF)
    flags += 1 << 9;
  #endif

  #ifdef ZLIB_WINAPI
    flags += 1 << 10;
  #endif

  #ifdef BUILDFIXED
    flags += 1 << 12;
  #endif

  #ifdef DYNAMIC_CRC_TABLE
    flags += 1 << 13;
  #endif

  #ifdef NO_GZCOMPRESS
    flags += 1 << 16;
  #endif

  #ifdef NO_GZIP
    flags += 1 << 17;
  #endif

  #ifdef PKZIP_BUG_WORKAROUND
    flags += 1 << 20;
  #endif

  #ifdef FASTEST
    flags += 1 << 21;
  #endif

  #ifdef STDC
    #ifdef NO_vsnprintf
      flags += 1 << 25;
      #ifdef HAS_vsprintf_void
        flags += 1 << 26;
      #endif
    #else
      #ifdef HAS_vsnprintf_void
        flags += 1 << 26;
      #endif
    #endif
  #else
    flags += 1 << 24;
    #ifdef NO_snprintf
      flags += 1 << 25;
      #ifdef HAS_sprintf_void
        flags += 1 << 26;
      #endif
    #else
      #ifdef HAS_snprintf_void
        flags += 1 << 26;
      #endif
    #endif
  #endif
  return flags;
}

//-------------------------------------------------------------------------

void *zcalloc(void *opaque, DWORD items, DWORD size)
{
  return (void*)malloc(items *size);
}

void zcfree(void *opaque, void *ptr)
{
  free(ptr);
}
