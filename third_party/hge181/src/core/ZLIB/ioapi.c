/* ioapi.c -- IO base function header for compress/uncompress .zip
files using zlib + zip or unzip API

Version 1.01, May 8th, 2004

Copyright (C) 1998-2004 Gilles Vollant
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "zlib.h"
#include "ioapi.h"


void *ZCALLBACK fopen_file_func(void *opaque, const char *filename, int mode)
{
  FILE *file = NULL;

  const char *mode_fopen = NULL;

  if ((mode &ZLIB_FILEFUNC_MODE_READWRITEFILTER) == ZLIB_FILEFUNC_MODE_READ)
  {
    mode_fopen = "rb";
  }
  else
  {
    if (mode &ZLIB_FILEFUNC_MODE_EXISTING)
    {
      mode_fopen = "r+b";
    }
    else
    {
      if (mode &ZLIB_FILEFUNC_MODE_CREATE)
      {
        mode_fopen = "wb";
      }
    }
  }

  if ((filename != NULL) && (mode_fopen != NULL))
  {
    file = fopen(filename, mode_fopen);
  }
  return file;
}

//-------------------------------------------------------------------------


DWORD ZCALLBACK fread_file_func(void *opaque, void *stream, void *buf, DWORD size)
{
  return fread(buf, 1, (size_t)size, (FILE*)stream);
}

//-------------------------------------------------------------------------


DWORD ZCALLBACK fwrite_file_func(void *opaque, void *stream, const void *buf, DWORD size)
{
  return fwrite(buf, 1, (size_t)size, (FILE*)stream);
}

//-------------------------------------------------------------------------

long ZCALLBACK ftell_file_func(void *opaque, void *stream)
{
  return ftell((FILE*)stream);
}

//-------------------------------------------------------------------------

long ZCALLBACK fseek_file_func(void *opaque, void *stream, DWORD offset, int origin)
{
  fseek((FILE*)stream, offset, origin);

  return 0;
}

//-------------------------------------------------------------------------

int ZCALLBACK fclose_file_func(void *opaque, void *stream)
{
  return fclose((FILE*)stream);
}

//-------------------------------------------------------------------------

int ZCALLBACK ferror_file_func(void *opaque, void *stream)
{
  return ferror((FILE*)stream);
}

//-------------------------------------------------------------------------

void fill_fopen_filefunc(zlib_filefunc_def *pzlib_filefunc_def)
{
  pzlib_filefunc_def->zopen_file = fopen_file_func;
  pzlib_filefunc_def->zread_file = fread_file_func;
  pzlib_filefunc_def->zwrite_file = fwrite_file_func;
  pzlib_filefunc_def->ztell_file = ftell_file_func;
  pzlib_filefunc_def->zseek_file = fseek_file_func;
  pzlib_filefunc_def->zclose_file = fclose_file_func;
  pzlib_filefunc_def->zerror_file = ferror_file_func;
  pzlib_filefunc_def->opaque = NULL;
}
