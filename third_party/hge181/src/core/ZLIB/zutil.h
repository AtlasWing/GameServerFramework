/* zutil.h -- internal interface and configuration of the compression library
 * Copyright (C) 1995-2003 Jean-loup Gailly.
 * For conditions of distribution and use, see copyright notice in zlib.h
 */

/* WARNING: this file should *not* be used by applications. It is
part of the implementation of the compression library and is
subject to change. Applications should only use zlib.h.
 */

/* @(#) $Id: zutil.h,v 1.1 2005/11/23 14:29:59 stingerx Exp $ */

#ifndef ZUTIL_H
  #define ZUTIL_H

  #define ZLIB_INTERNAL
  #include "zlib.h"

  #ifdef STDC
    #include <stddef.h>
    #include <string.h>
    #include <stdlib.h>
  #endif
  #ifdef NO_ERRNO_H
    extern int errno;
  #else
    #include <errno.h>
  #endif

  extern const char *const z_errmsg[10]; /* indexed by 2-zlib_error */
  /* (size given to avoid silly warnings with Visual C++) */

  #define ERR_MSG(err) z_errmsg[Z_NEED_DICT-(err)]
  #define ERR_RETURN(strm,err) return (strm->msg = (char*)ERR_MSG(err), (err))

  /* To be used only when the state is known to be valid */

  /* common constants */

  #ifndef DEF_WBITS
    #define DEF_WBITS MAX_WBITS
  #endif

  /* default windowBits for decompression. MAX_WBITS is for compression only */

  #if MAX_MEM_LEVEL >= 8
    #define DEF_MEM_LEVEL 8
  #else
    #define DEF_MEM_LEVEL  MAX_MEM_LEVEL
  #endif

  /* default memLevel */

  #define STORED_BLOCK 0
  #define STATIC_TREES 1
  #define DYN_TREES    2

  /* The three kinds of block type */

  #define MIN_MATCH  3
  #define MAX_MATCH  258

  /* The minimum and maximum match lengths */

  #define PRESET_DICT 0x20 /* preset dictionary flag in zlib header */

  /* target dependencies */

  #ifdef WIN32
    #ifndef __CYGWIN__  /* Cygwin is Unix, not Win32 */
      #define OS_CODE  0x0b
    #endif
  #endif

  #if (defined(_MSC_VER) && (_MSC_VER > 600))
    #if defined(_WIN32_WCE)
      #define fdopen(fd,mode) NULL /* No fdopen() */
      #ifndef _PTRDIFF_T_DEFINED
        typedef int ptrdiff_t;
        #define _PTRDIFF_T_DEFINED
      #endif
    #else
      #define fdopen(fd,type)  _fdopen(fd,type)
    #endif
  #endif

  /* common defaults */

  #ifndef OS_CODE
    #define OS_CODE  0x03  /* assume Unix */
  #endif

  #ifndef F_OPEN
    #define F_OPEN(name, mode) fopen((name), (mode))
  #endif

  /* functions */

  #if defined(STDC99) || (defined(__TURBOC__) && __TURBOC__ >= 0x550)
    #ifndef HAVE_VSNPRINTF
      #define HAVE_VSNPRINTF
    #endif
  #endif
  #if defined(__CYGWIN__)
    #ifndef HAVE_VSNPRINTF
      #define HAVE_VSNPRINTF
    #endif
  #endif
  #ifndef HAVE_VSNPRINTF
    #ifdef MSDOS
      /* vsnprintf may exist on some MS-DOS compilers (DJGPP?),
      but for now we just assume it doesn't. */
      #define NO_vsnprintf
    #endif
    #ifdef __TURBOC__
      #define NO_vsnprintf
    #endif
    #ifdef WIN32
      /* In Win32, vsnprintf is available as the "non-ANSI" _vsnprintf. */
      #if !defined(vsnprintf) && !defined(NO_vsnprintf)
        #define vsnprintf _vsnprintf
      #endif
    #endif
    #ifdef __SASC
      #define NO_vsnprintf
    #endif
  #endif

  #ifdef HAVE_STRERROR
    extern char *strerror OF((int));
    #define zstrerror(errnum) strerror(errnum)
  #else
    #define zstrerror(errnum) ""
  #endif


  #define zmemcpy memcpy
  #define zmemcmp memcmp
  #define zmemzero(dest, len) memset(dest, 0, len)

  /* Diagnostic functions */


  void *zcalloc(void *opaque, DWORD items, DWORD size);
  void zcfree(void *opaque, void *ptr);

  #define ZALLOC(strm, items, size) (*((strm)->zalloc))((strm)->opaque, (items), (size))
  #define ZFREE(strm, addr)  (*((strm)->zfree))((strm)->opaque, (void *)(addr))
  #define TRY_FREE(s, p) {if (p) ZFREE(s, p);}

#endif /* ZUTIL_H */
