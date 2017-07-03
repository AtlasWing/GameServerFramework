/* inflate.c -- zlib decompression
 * Copyright (C) 1995-2003 Mark Adler
 * For conditions of distribution and use, see copyright notice in zlib.h
 */

/*
 * Change history:
 *
 * 1.2.beta0    24 Nov 2002
 * - First version -- complete rewrite of inflate to simplify code, avoid
 *   creation of window when not needed, minimize use of window when it is
 *   needed, make inffast.c even faster, implement gzip decoding, and to
 *   improve code readability and style over the previous zlib inflate code
 *
 * 1.2.beta1    25 Nov 2002
 * - Use pointers for available input and output checking in inffast.c
 * - Remove input and output counters in inffast.c
 * - Change inffast.c entry and loop from avail_in >= 7 to >= 6
 * - Remove unnecessary second byte pull from length extra in inffast.c
 * - Unroll direct copy to three copies per loop in inffast.c
 *
 * 1.2.beta2    4 Dec 2002
 * - Change external routine names to reduce potential conflicts
 * - Correct filename to inffixed.h for fixed tables in inflate.c
 * - Make hbuf[] unsigned char to match parameter type in inflate.c
 * - Change strm->next_out[-state->offset] to *(strm->next_out - state->offset)
 *   to avoid negation problem on Alphas (64 bit) in inflate.c
 *
 * 1.2.beta3    22 Dec 2002
 * - Add comments on state->bits assertion in inffast.c
 * - Add comments on op field in inftrees.h
 * - Fix bug in reuse of allocated window after inflateReset()
 * - Remove bit fields--back to byte structure for speed
 * - Remove distance extra == 0 check in inflate_fast()--only helps for lengths
 * - Change post-increments to pre-increments in inflate_fast(), PPC biased?
 * - Add compile time option, POSTINC, to use post-increments instead (Intel?)
 * - Make MATCH copy in inflate() much faster for when inflate_fast() not used
 * - Use local copies of stream next and avail values, as well as local bit
 *   buffer and bit count in inflate()--for speed when inflate_fast() not used
 *
 * 1.2.beta4    1 Jan 2003
 * - Split ptr - 257 statements in inflate_table() to avoid compiler warnings
 * - Move a comment on output buffer sizes from inffast.c to inflate.c
 * - Add comments in inffast.c to introduce the inflate_fast() routine
 * - Rearrange window copies in inflate_fast() for speed and simplification
 * - Unroll last copy for window match in inflate_fast()
 * - Use local copies of window variables in inflate_fast() for speed
 * - Pull out common write == 0 case for speed in inflate_fast()
 * - Make op and len in inflate_fast() unsigned for consistency
 * - Add FAR to lcode and dcode declarations in inflate_fast()
 * - Simplified bad distance check in inflate_fast()
 * - Added inflateBackInit(), inflateBack(), and inflateBackEnd() in new
 *   source file infback.c to provide a call-back interface to inflate for
 *   programs like gzip and unzip -- uses window as output buffer to avoid
 *   window copying
 *
 * 1.2.beta5    1 Jan 2003
 * - Improved inflateBack() interface to allow the caller to provide initial
 *   input in strm.
 * - Fixed stored blocks bug in inflateBack()
 *
 * 1.2.beta6    4 Jan 2003
 * - Added comments in inffast.c on effectiveness of POSTINC
 * - Typecasting all around to reduce compiler warnings
 * - Changed loops from while (1) or do {} while (1) to for (;;), again to
 *   make compilers happy
 * - Changed type of window in inflateBackInit() to unsigned char *
 *
 * 1.2.beta7    27 Jan 2003
 * - Changed many types to unsigned or unsigned short to avoid warnings
 * - Added inflateCopy() function
 *
 * 1.2.0        9 Mar 2003
 * - Changed inflateBack() interface to provide separate opaque descriptors
 *   for the in() and out() functions
 * - Changed inflateBack() argument and in_func typedef to swap the length
 *   and buffer address return values for the input function
 * - Check next_in and next_out for Z_NULL on entry to inflate()
 *
 * The history for versions after 1.2.0 are in ChangeLog in zlib distribution.
 */

#include "zutil.h"
#include "inftrees.h"
#include "inflate.h"
#include "inffast.h"

/* function prototypes */

static void fixedtables(struct inflate_state *state);

static int updatewindow(z_streamp strm, DWORD out);


static DWORD syncsearch(DWORD *have, BYTE *buf, DWORD len);

int ZEXPORT inflateReset(z_streamp strm)
{
  struct inflate_state *state;

  if (strm == Z_NULL || strm->state == Z_NULL)
  {
    return Z_STREAM_ERROR;
  }

  state = (struct inflate_state*)strm->state;
  strm->total_in = strm->total_out = state->total = 0;
  strm->msg = Z_NULL;
  state->mode = HEAD;
  state->last = 0;
  state->havedict = 0;
  state->wsize = 0;
  state->whave = 0;
  state->hold = 0;
  state->bits = 0;
  state->lencode = state->distcode = state->next = state->codes;
  return Z_OK;
}

//-------------------------------------------------------------------------

int ZEXPORT inflateInit2_(z_streamp strm, int windowBits, const BYTE *version, int stream_size)
{
  struct inflate_state *state;

  if (version == Z_NULL || version[0] != ZLIB_VERSION[0] || stream_size != (int)(sizeof(z_stream)))
  {
    return Z_VERSION_ERROR;
  } if (strm == Z_NULL)
  {
    return Z_STREAM_ERROR;
  }

  strm->msg = Z_NULL; /* in case we return an error */

  if (strm->zalloc == (alloc_func)0)
  {
    strm->zalloc = zcalloc;
    strm->opaque = (void*)0;
  }

  if (strm->zfree == (free_func)0)
  {
    strm->zfree = zcfree;
  }

  state = (struct inflate_state FAR*)ZALLOC(strm, 1, sizeof(struct inflate_state));

  if (state == Z_NULL)
  {
    return Z_MEM_ERROR;
  }

  strm->state = (void*)state;

  if (windowBits < 0)
  {
    state->wrap = 0;
    windowBits =  - windowBits;
  }
  else
  {
    state->wrap = (windowBits >> 4) + 1;
    #ifdef GUNZIP
      if (windowBits < 48)
      {
        windowBits &= 15;
      }
    #endif
  }

  if (windowBits < 8 || windowBits > 15)
  {
    ZFREE(strm, state);
    strm->state = Z_NULL;
    return Z_STREAM_ERROR;
  }

  state->wbits = (DWORD)windowBits;
  state->window = Z_NULL;
  return inflateReset(strm);
}

//-------------------------------------------------------------------------

int ZEXPORT inflateInit_(z_streamp strm, const BYTE *version, int stream_size)
{
  return inflateInit2_(strm, DEF_WBITS, version, stream_size);
}

/*
Return state with length and distance decoding tables and index sizes set to
fixed code decoding.  Normally this returns fixed tables from inffixed.h.
If BUILDFIXED is defined, then instead this routine builds the tables the
first time it's called, and returns those tables the first time and
thereafter.  This reduces the size of the code by about 2K bytes, in
exchange for a little execution time.  However, BUILDFIXED should not be
used for threaded applications, since the rewriting of the tables and virgin
may not be thread-safe.
 */

static void fixedtables(struct inflate_state *state)
{
  static DWORD init = 0;
  static code *next;
  static code *lenfix;
  static code *distfix;
  static code fixed[544];

  /* build fixed huffman tables if first call (may not be thread safe) */

  if (!init)
  {
    DWORD sym = 0;
    DWORD bits;

    init++;

    /* literal/length table */

    while (sym < 144)
    {
      state->lens[sym++] = 8;
    }

    while (sym < 256)
    {
      state->lens[sym++] = 9;
    }

    while (sym < 280)
    {
      state->lens[sym++] = 7;
    }

    while (sym < 288)
    {
      state->lens[sym++] = 8;
    }

    next = fixed;
    lenfix = next;
    bits = 9;

    inflate_table(LENS, state->lens, 288, &next, &bits, state->work);

    /* distance table */

    for (sym = 0; sym < 32;)
    {
      state->lens[sym++] = 5;
    }

    distfix = next;
    bits = 5;
    inflate_table(DISTS, state->lens, 32, &next, &bits, state->work);
  }

  state->lencode = lenfix;
  state->lenbits = 9;
  state->distcode = distfix;
  state->distbits = 5;
}


/*
Update the window with the last wsize (normally 32K) bytes written before
returning.  If window does not exist yet, create it.  This is only called
when a window is already in use, or when output has been written during this
inflate call, but the end of the deflate stream has not been reached yet.
It is also called to create a window for dictionary data when a dictionary
is loaded.

Providing output buffers larger than 32K to inflate() should provide a speed
advantage, since only the last 32K of output is copied to the sliding window
upon return from inflate(), and since all distances after the first 32K of
output will fall in the output data, making match copies simpler and faster.
The advantage may be dependent on the size of the processor's data caches.
 */

static int updatewindow(z_streamp strm, DWORD out)
{
  struct inflate_state *state;
  DWORD copy, dist;

  state = (struct inflate_state FAR*)strm->state;

  /* if it hasn't been done already, allocate space for the window */

  if (state->window == Z_NULL)
  {
    state->window = (BYTE FAR*)ZALLOC(strm, 1U << state->wbits, sizeof(BYTE));

    if (state->window == Z_NULL)
    {
      return 1;
    }
  }

  /* if window not in use yet, initialize */

  if (state->wsize == 0)
  {
    state->wsize = 1U << state->wbits;
    state->write = 0;
    state->whave = 0;
  }

  /* copy state->wsize or less output bytes into the circular window */

  copy = out - strm->avail_out;

  if (copy >= state->wsize)
  {
    zmemcpy(state->window, strm->next_out - state->wsize, state->wsize);
    state->write = 0;
    state->whave = state->wsize;
  }
  else
  {
    dist = state->wsize - state->write;

    if (dist > copy)
    {
      dist = copy;
    }

    zmemcpy(state->window + state->write, strm->next_out - copy, dist);

    copy -= dist;

    if (copy)
    {
      zmemcpy(state->window, strm->next_out - copy, copy);
      state->write = copy;
      state->whave = state->wsize;
    }
    else
    {
      state->write += dist;

      if (state->write == state->wsize)
      {
        state->write = 0;
      }

      if (state->whave < state->wsize)
      {
        state->whave += dist;
      }
    }
  }
  return 0;
}

/* Macros for inflate(): */

/* check function to use adler32() for zlib or crc32() for gzip */
#ifdef GUNZIP
  #define UPDATE(check, buf, len) \
  (state->flags ? crc32(check, buf, len) : adler32(check, buf, len))
#else
  #define UPDATE(check, buf, len) adler32(check, buf, len)
#endif

/* check macros for header crc */
#ifdef GUNZIP
  #define CRC2(check, word) \
  { \
  hbuf[0] = (BYTE)(word); \
  hbuf[1] = (BYTE)((word) >> 8); \
  check = crc32(check, hbuf, 2); \
  }

  #define CRC4(check, word) \
  { \
  hbuf[0] = (BYTE)(word); \
  hbuf[1] = (BYTE)((word) >> 8); \
  hbuf[2] = (BYTE)((word) >> 16); \
  hbuf[3] = (BYTE)((word) >> 24); \
  check = crc32(check, hbuf, 4); \
  } 
#endif

/* Load registers with state in inflate() for speed */
#define LOAD() \
{ \
put = strm->next_out; \
left = strm->avail_out; \
next = strm->next_in; \
have = strm->avail_in; \
hold = state->hold; \
bits = state->bits; \
}

/* Restore state from registers in inflate() */
#define RESTORE() \
{ \
strm->next_out = put; \
strm->avail_out = left; \
strm->next_in = next; \
strm->avail_in = have; \
state->hold = hold; \
state->bits = bits; \
}

/* Clear the input bit accumulator */
#define INITBITS() { hold = 0; bits = 0; } 

/* Get a byte of input into the bit accumulator, or return from inflate()
if there is no input available. */
#define PULLBYTE()  { if (have == 0) goto inf_leave; have--; hold += (DWORD)(*next++) << bits; bits += 8; } 

/* Assure that there are at least n bits in the bit accumulator.  If there is
not enough available input to do that, then return from inflate(). */
#define NEEDBITS(n) { while (bits < (DWORD)(n)) { PULLBYTE(); } } 

/* Return the low n bits of the bit accumulator (n < 16) */
#define BITS(n) ((DWORD)hold & ((1U << (n)) - 1))

/* Remove n bits from the bit accumulator */
#define DROPBITS(n) { hold >>= (n); bits -= (DWORD)(n); } 

/* Remove zero to seven bits as needed to go to a byte boundary */
#define BYTEBITS()  { hold >>= bits & 7; bits -= bits & 7; } 

/* Reverse the bytes in a 32-bit value */
#define REVERSE(q) (q >> 24 | q << 24 | ((q << 8) & 0x00FF0000) | ((q >> 8) & 0x0000FF00))

//((((q) >> 24) & 0xff) + (((q) >> 8) & 0xff00) + \
//(((q) & 0xff00) << 8) + (((q) & 0xff) << 24))

/*
inflate() uses a state machine to process as much input data and generate as
much output data as possible before returning.  The state machine is
structured roughly as follows:

for (;;) switch (state) {
...
case STATEn:
if (not enough input data or output space to make progress)
return;
... make progress ...
state = STATEm;
break;
...
}

so when inflate() is called again, the same case is attempted again, and
if the appropriate resources are provided, the machine proceeds to the
next state.  The NEEDBITS() macro is usually the way the state evaluates
whether it can proceed or should return.  NEEDBITS() does the return if
the requested bits are not available.  The typical use of the BITS macros
is:

NEEDBITS(n);
... do something with BITS(n) ...
DROPBITS(n);

where NEEDBITS(n) either returns from inflate() if there isn't enough
input left to load n bits into the accumulator, or it continues.  BITS(n)
gives the low n bits in the accumulator.  When done, DROPBITS(n) drops
the low n bits off the accumulator.  INITBITS() clears the accumulator
and sets the number of available bits to zero.  BYTEBITS() discards just
enough bits to put the accumulator on a byte boundary.  After BYTEBITS()
and a NEEDBITS(8), then BITS(8) would return the next byte in the stream.

NEEDBITS(n) uses PULLBYTE() to get an available byte of input, or to return
if there is no input available.  The decoding of variable length codes uses
PULLBYTE() directly in order to pull just enough bytes to decode the next
code, and no more.

Some states loop until they get enough input, making sure that enough
state information is maintained to continue the loop where it left off
if NEEDBITS() returns in the loop.  For example, want, need, and keep
would all have to actually be part of the saved state in case NEEDBITS()
returns:

case STATEw:
while (want < need) {
NEEDBITS(n);
keep[want++] = BITS(n);
DROPBITS(n);
}
state = STATEx;
case STATEx:

As shown above, if the next state is also the next case, then the break
is omitted.

A state may also return if there is not enough output space available to
complete that state.  Those states are copying stored data, writing a
literal byte, and copying a matching string.

When returning, a "goto inf_leave" is used to update the total counters,
update the check value, and determine whether any progress has been made
during that inflate() call in order to return the proper return code.
Progress is defined as a change in either strm->avail_in or strm->avail_out.
When there is a window, goto inf_leave will update the window with the last
output written.  If a goto inf_leave occurs in the middle of decompression
and there is no window currently, goto inf_leave will create one and copy
output to the window for the next call of inflate().

In this implementation, the flush parameter of inflate() only affects the
return code (per zlib.h).  inflate() always writes as much as possible to
strm->next_out, given the space available and the provided input--the effect
documented in zlib.h of Z_SYNC_FLUSH.  Furthermore, inflate() always defers
the allocation of and copying into a sliding window until necessary, which
provides the effect documented in zlib.h for Z_FINISH when the entire input
stream available.  So the only thing the flush parameter actually does is:
when flush is set to Z_FINISH, inflate() cannot return Z_OK.  Instead it
will return Z_BUF_ERROR if it has not reached the end of the stream.
 */

int ZEXPORT inflate(z_streamp strm, int flush)
{
  struct inflate_state *state;
  BYTE *next; /* next input */
  BYTE *put; /* next output */
  DWORD have, left; /* available input and output */
  DWORD hold; /* bit buffer */
  DWORD bits; /* bits in bit buffer */
  DWORD in, out; /* save starting available input and output */
  DWORD copy; /* number of stored or match bytes to copy */
  BYTE *from; /* where to copy match bytes from */
  code this; /* current decoding table entry */
  code last; /* parent table entry */
  DWORD len; /* length to copy for repeats, bits to drop */
  int ret; /* return code */

  #ifdef GUNZIP
    BYTE hbuf[4]; /* buffer for gzip header crc calculation */
  #endif

  static const WORD order[19] =  /* permutation of code lengths */
  {
    16, 17, 18, 0, 8, 7, 9, 6, 10, 5, 11, 4, 12, 3, 13, 2, 14, 1, 15
  };

  if (strm == Z_NULL || strm->state == Z_NULL || strm->next_out == Z_NULL || (strm->next_in == Z_NULL && strm->avail_in != 0))
  {
    return Z_STREAM_ERROR;
  }

  state = (struct inflate_state FAR*)strm->state;
  if (state->mode == TYPE)
  {
    state->mode = TYPEDO;
  }
  /* skip check */
  LOAD();
  in = have;
  out = left;
  ret = Z_OK;
  for (;;)
  switch (state->mode)
  {
    case HEAD:
      if (state->wrap == 0)
      {
        state->mode = TYPEDO;
        break;
      }
      NEEDBITS(16);
      #ifdef GUNZIP
        if ((state->wrap &2) && hold == 0x8b1f)
        {
          /* gzip header */
          state->check = crc32(0L, Z_NULL, 0);
          CRC2(state->check, hold);
          INITBITS();
          state->mode = FLAGS;
          break;
        }
        state->flags = 0; /* expect zlib header */
        if (!(state->wrap &1) ||  /* check if zlib header allowed */
      #else
        if (
      #endif
      ((BITS(8) << 8) + (hold >> 8)) % 31)
      {
        state->mode = BAD; break;
      }
      if (BITS(4) != Z_DEFLATED)
      {
        state->mode = BAD; break;
      }
      DROPBITS(4); if (BITS(4) + 8 > state->wbits)
      {
        state->mode = BAD; break;
      }
      strm->adler = state->check = adler32(0L, Z_NULL, 0); state->mode = hold &0x200 ? DICTID : TYPE; INITBITS(); break;
      #ifdef GUNZIP
      case FLAGS:
        NEEDBITS(16); state->flags = (int)(hold); if ((state->flags &0xff) != Z_DEFLATED)
        {
          state->mode = BAD; break;
        }
        if (state->flags &0xe000)
        {
          state->mode = BAD; break;
        }
      if (state->flags &0x0200)CRC2(state->check, hold); INITBITS(); state->mode = TIME; case TIME:
      NEEDBITS(32); if (state->flags &0x0200)CRC4(state->check, hold); INITBITS(); state->mode = OS; case OS:
      NEEDBITS(16); if (state->flags &0x0200)CRC2(state->check, hold); INITBITS(); state->mode = EXLEN; case EXLEN:
        if (state->flags &0x0400)
        {
          NEEDBITS(16); state->length = (DWORD)(hold); if (state->flags &0x0200)CRC2(state->check, hold); INITBITS();
        }
      state->mode = EXTRA; case EXTRA:
        if (state->flags &0x0400)
        {
          copy = state->length; if (copy > have)copy = have; if (copy)
          {
            if (state->flags &0x0200)state->check = crc32(state->check, next, copy); have -= copy; next += copy; state->length -= copy;
          }
          if (state->length)goto inf_leave;
        }
      state->mode = NAME; case NAME:
        if (state->flags &0x0800)
        {
          if (have == 0)goto inf_leave; copy = 0;
          do
          {
            len = (DWORD)(next[copy++]);
          }
          while (len && copy < have); if (state->flags &0x02000)state->check = crc32(state->check, next, copy); have -= copy; next += copy; if (len)goto inf_leave;
        }
      state->mode = COMMENT; case COMMENT:
        if (state->flags &0x1000)
        {
          if (have == 0)goto inf_leave; copy = 0;
          do
          {
            len = (DWORD)(next[copy++]);
          }
          while (len && copy < have); if (state->flags &0x02000)state->check = crc32(state->check, next, copy); have -= copy; next += copy; if (len)goto inf_leave;
        }
      state->mode = HCRC; case HCRC:
        if (state->flags &0x0200)
        {
          NEEDBITS(16); if (hold != (state->check &0xffff))
          {
            state->mode = BAD; break;
          }
          INITBITS();
        }
        strm->adler = state->check = crc32(0L, Z_NULL, 0); state->mode = TYPE; break;
      #endif
    case DICTID:
    NEEDBITS(32); strm->adler = state->check = REVERSE(hold); INITBITS(); state->mode = DICT; case DICT:
      if (state->havedict == 0)
      {
        RESTORE(); return Z_NEED_DICT;
      }
    strm->adler = state->check = adler32(0L, Z_NULL, 0); state->mode = TYPE; case TYPE:
    if (flush == Z_BLOCK)goto inf_leave; case TYPEDO:
      if (state->last)
      {
        BYTEBITS(); state->mode = CHECK; break;
      }
      NEEDBITS(3); state->last = BITS(1); DROPBITS(1); switch (BITS(2))
      {
      case 0:
        /* stored block */
        state->mode = STORED; break;

      case 1:
        /* fixed block */
        fixedtables(state);

        state->mode = LEN;  /* decode codes */
      break; case 2:
        /* dynamic block */
      state->mode = TABLE; break; case 3:
        state->mode = BAD;
      }
    DROPBITS(2); break; case STORED:
      BYTEBITS();  /* go to byte boundary */
      NEEDBITS(32); if ((hold &0xffff) != ((hold >> 16) ^ 0xffff))
      {
        state->mode = BAD; break;
      }
    state->length = (DWORD)hold &0xffff; INITBITS(); state->mode = COPY; case COPY:
      copy = state->length; if (copy)
      {
        if (copy > have)copy = have; if (copy > left)copy = left; if (copy == 0)goto inf_leave; zmemcpy(put, next, copy); have -= copy; next += copy; left -= copy; put += copy; state->length -= copy; break;
      }
    state->mode = TYPE; break; case TABLE:
      NEEDBITS(14); state->nlen = BITS(5) + 257; DROPBITS(5); state->ndist = BITS(5) + 1; DROPBITS(5); state->ncode = BITS(4) + 4; DROPBITS(4);
      #ifndef PKZIP_BUG_WORKAROUND
        if (state->nlen > 286 || state->ndist > 30)
        {
          state->mode = BAD; break;
        }
      #endif
    state->have = 0; state->mode = LENLENS; case LENLENS:
      while (state->have < state->ncode)
      {
        NEEDBITS(3); state->lens[order[state->have++]] = (WORD)BITS(3); DROPBITS(3);
      }
      while (state->have < 19)state->lens[order[state->have++]] = 0; state->next = state->codes; state->lencode = (code const FAR*)(state->next); state->lenbits = 7; ret = inflate_table(CODES, state->lens, 19, &(state->next), &(state->lenbits), state->work); if (ret)
      {
        state->mode = BAD; break;
      }
    state->have = 0; state->mode = CODELENS; case CODELENS:
      while (state->have < state->nlen + state->ndist)
      {
        for (;;)
        {
          this = state->lencode[BITS(state->lenbits)]; if ((DWORD)(this.bits) <= bits)break; PULLBYTE();
        }
        if (this.val < 16)
        {
          NEEDBITS(this.bits); DROPBITS(this.bits); state->lens[state->have++] = this.val;
        }
        else
        {
          if (this.val == 16)
          {
            NEEDBITS(this.bits + 2); DROPBITS(this.bits); if (state->have == 0)
            {
              state->mode = BAD; break;
            }
            len = state->lens[state->have - 1]; copy = 3+BITS(2); DROPBITS(2);
          }
          else if (this.val == 17)
          {
            NEEDBITS(this.bits + 3); DROPBITS(this.bits); len = 0; copy = 3+BITS(3); DROPBITS(3);
          }
          else
          {
            NEEDBITS(this.bits + 7); DROPBITS(this.bits); len = 0; copy = 11+BITS(7); DROPBITS(7);
          }
          if (state->have + copy > state->nlen + state->ndist)
          {
            state->mode = BAD; break;
          }
          while (copy--)state->lens[state->have++] = (WORD)len;
        }
      }

      /* build code tables */
      state->next = state->codes; state->lencode = (code const*)(state->next); state->lenbits = 9; ret = inflate_table(LENS, state->lens, state->nlen, &(state->next), &(state->lenbits), state->work); if (ret)
      {
        state->mode = BAD; break;
      }

      state->distcode = (code const FAR*)(state->next); state->distbits = 6; ret = inflate_table(DISTS, state->lens + state->nlen, state->ndist, &(state->next), &(state->distbits), state->work); if (ret)
      {
        state->mode = BAD; break;
      }

    state->mode = LEN; case LEN:

      if (have >= 6 && left >= 258)
      {
        RESTORE(); inflate_fast(strm, out); LOAD(); break;
      }
      for (;;)
      {
        this = state->lencode[BITS(state->lenbits)]; if ((DWORD)(this.bits) <= bits)break; PULLBYTE();
      }
      if (this.op && (this.op &0xf0) == 0)
      {
        last = this; for (;;)
        {
          this = state->lencode[last.val + (BITS(last.bits + last.op) >> last.bits)]; if ((DWORD)(last.bits + this.bits) <= bits)break; PULLBYTE();
        }
        DROPBITS(last.bits);
      }
      DROPBITS(this.bits); state->length = (DWORD)this.val; if ((int)(this.op) == 0)
      {
        state->mode = LIT; break;
      }
      if (this.op &32)
      {
        state->mode = TYPE; break;
      }
      if (this.op &64)
      {
        state->mode = BAD; break;
      }
    state->extra = (DWORD)(this.op) &15; state->mode = LENEXT; case LENEXT:
      if (state->extra)
      {
        NEEDBITS(state->extra); state->length += BITS(state->extra); DROPBITS(state->extra);
      }
    state->mode = DIST; case DIST:
      for (;;)
      {
        this = state->distcode[BITS(state->distbits)]; if ((DWORD)(this.bits) <= bits)break; PULLBYTE();
      }
      if ((this.op &0xf0) == 0)
      {
        last = this; for (;;)
        {
          this = state->distcode[last.val + (BITS(last.bits + last.op) >> last.bits)]; if ((DWORD)(last.bits + this.bits) <= bits)break; PULLBYTE();
        }
        DROPBITS(last.bits);
      }
      DROPBITS(this.bits); if (this.op &64)
      {
        state->mode = BAD; break;
      }
    state->offset = (DWORD)this.val; state->extra = (DWORD)(this.op) &15; state->mode = DISTEXT; case DISTEXT:
      if (state->extra)
      {
        NEEDBITS(state->extra); state->offset += BITS(state->extra); DROPBITS(state->extra);
      }
      if (state->offset > state->whave + out - left)
      {
        state->mode = BAD; break;
      }
    state->mode = MATCH; case MATCH:
      if (left == 0)goto inf_leave; copy = out - left; if (state->offset > copy)
      {
        /* copy from window */
        copy = state->offset - copy; if (copy > state->write)
        {
          copy -= state->write; from = state->window + (state->wsize - copy);
        }
        else
        from = state->window + (state->write - copy); if (copy > state->length)copy = state->length;
      }
      else
      {
        /* copy from output */
        from = put - state->offset; copy = state->length;
      }
      if (copy > left)copy = left; left -= copy; state->length -= copy;
      do
      {
        *put++ =  *from++;
      }
    while (--copy); if (state->length == 0)state->mode = LEN; break; case LIT:
    if (left == 0)goto inf_leave;  *put++ = (BYTE)(state->length); left--; state->mode = LEN; break; case CHECK:
      if (state->wrap)
      {
        NEEDBITS(32); out -= left; strm->total_out += out; state->total += out; if (out)strm->adler = state->check = UPDATE(state->check, put - out, out); out = left; if ((
        #ifdef GUNZIP
          state->flags ? hold :
        #endif
        REVERSE(hold)) != state->check)
        {
          state->mode = BAD; break;
        }
        INITBITS();
      }
      #ifdef GUNZIP
      state->mode = LENGTH; case LENGTH:
        if (state->wrap && state->flags)
        {
          NEEDBITS(32); if (hold != (state->total &0xffffffffUL))
          {
            state->mode = BAD; break;
          }
          INITBITS();
        }
      #endif
    state->mode = DONE; case DONE:
    ret = Z_STREAM_END; goto inf_leave; case BAD:
    ret = Z_DATA_ERROR; goto inf_leave; case MEM:
    return Z_MEM_ERROR; case SYNC:
      default: return Z_STREAM_ERROR;
  }

  /*
  Return from inflate(), updating the total counts and the check value.
  If there was no progress during the inflate() call, return a buffer
  error.  Call updatewindow() to create and/or update the window state.
  Note: a memory error from inflate() is non-recoverable.
   */
  inf_leave: RESTORE(); if (state->wsize || (state->mode < CHECK && out != strm->avail_out))if (updatewindow(strm, out))
  {
    state->mode = MEM; return Z_MEM_ERROR;
  } in -= strm->avail_in; out -= strm->avail_out; strm->total_in += in; strm->total_out += out; state->total += out; if (state->wrap && out)strm->adler = state->check = UPDATE(state->check, strm->next_out - out, out); strm->data_type = state->bits + (state->last ? 64 : 0) + (state->mode == TYPE ? 128 : 0); if (((in == 0 && out == 0) || flush == Z_FINISH) && ret == Z_OK)ret = Z_BUF_ERROR; return ret;
}

//-------------------------------------------------------------------------

int ZEXPORT inflateEnd(z_streamp strm)
{
  struct inflate_state *state;

  if (strm == Z_NULL || strm->state == Z_NULL || strm->zfree == (free_func)0)
  {
    return Z_STREAM_ERROR;
  }

  state = (struct inflate_state FAR*)strm->state;

  if (state->window != Z_NULL)
  {
    ZFREE(strm, state->window);
  }

  ZFREE(strm, strm->state);

  strm->state = Z_NULL;

  return Z_OK;
}

//-------------------------------------------------------------------------

int ZEXPORT inflateSetDictionary(z_streamp strm, const Bytef *dictionary, DWORD dictLength)
{
  struct inflate_state *state; DWORD id;

  /* check state */
  if (strm == Z_NULL || strm->state == Z_NULL)
  {
    return Z_STREAM_ERROR;
  }

  state = (struct inflate_state FAR*)strm->state;

  if (state->mode != DICT)
  {
    return Z_STREAM_ERROR;
  }

  /* check for correct dictionary id */
  id = adler32(0L, Z_NULL, 0); id = adler32(id, dictionary, dictLength);

  if (id != state->check)
  {
    return Z_DATA_ERROR;
  }

  /* copy dictionary to window */

  if (updatewindow(strm, strm->avail_out))
  {
    state->mode = MEM; return Z_MEM_ERROR;
  }

  if (dictLength > state->wsize)
  {
    zmemcpy(state->window, dictionary + dictLength - state->wsize, state->wsize); state->whave = state->wsize;
  }
  else
  {
    zmemcpy(state->window + state->wsize - dictLength, dictionary, dictLength); state->whave = dictLength;
  }
  state->havedict = 1; return Z_OK;
}

/*
Search buf[0..len-1] for the pattern: 0, 0, 0xff, 0xff.  Return when found
or when out of input.  When called, *have is the number of pattern bytes
found in order so far, in 0..3.  On return *have is updated to the new
state.  If on return *have equals four, then the pattern was found and the
return value is how many bytes were read including the last byte of the
pattern.  If *have is less than four, then the pattern has not been found
yet and the return value is len.  In the latter case, syncsearch() can be
called again with more data and the *have state.  *have is initialized to
zero for the first call.
 */
static DWORD syncsearch(DWORD *have, BYTE *buf, DWORD len)
{
  DWORD got; DWORD next;

  got =  *have; next = 0;
  while (next < len && got < 4)
  {
    if ((int)(buf[next]) == (got < 2 ? 0 : 0xff))got++;
    else if (buf[next])got = 0;
    else
    got = 4-got; next++;
  }
  *have = got; return next;
}

//-------------------------------------------------------------------------

int ZEXPORT inflateSync(z_streamp strm)
{
  DWORD len;  /* number of bytes to look at or looked at */
  DWORD in, out;  /* temporary to save total_in and total_out */
  BYTE buf[4];  /* to restore bit buffer to byte string */
  struct inflate_state *state;

  /* check parameters */
  if (strm == Z_NULL || strm->state == Z_NULL)return Z_STREAM_ERROR; state = (struct inflate_state FAR*)strm->state; if (strm->avail_in == 0 && state->bits < 8)return Z_BUF_ERROR;

  /* if first time, start search in bit buffer */
  if (state->mode != SYNC)
  {
    state->mode = SYNC; state->hold <<= state->bits &7; state->bits -= state->bits &7; len = 0;
    while (state->bits >= 8)
    {
      buf[len++] = (BYTE)(state->hold); state->hold >>= 8; state->bits -= 8;
    } state->have = 0; syncsearch(&(state->have), buf, len);
  }

  /* search available input */
  len = syncsearch(&(state->have), strm->next_in, strm->avail_in); strm->avail_in -= len; strm->next_in += len; strm->total_in += len;

  /* return no joy or set up to restart inflate() on a new block */
  if (state->have != 4)return Z_DATA_ERROR; in = strm->total_in; out = strm->total_out; inflateReset(strm); strm->total_in = in; strm->total_out = out; state->mode = TYPE; return Z_OK;
}

/*
Returns true if inflate is currently at the end of a block generated by
Z_SYNC_FLUSH or Z_FULL_FLUSH. This function is used by one PPP
implementation to provide an additional safety check. PPP uses
Z_SYNC_FLUSH but removes the length bytes of the resulting empty stored
block. When decompressing, PPP checks that at the end of input packet,
inflate is waiting for these length bytes.
 */
int ZEXPORT inflateSyncPoint(z_streamp strm)
{
  struct inflate_state *state;

  if (strm == Z_NULL || strm->state == Z_NULL)
  {
    return Z_STREAM_ERROR;
  }

  state = (struct inflate_state FAR*)strm->state;

  return state->mode == STORED && state->bits == 0;
}

//-------------------------------------------------------------------------

int ZEXPORT inflateCopy(z_streamp dest, z_streamp source)
{
  struct inflate_state *state; struct inflate_state *copy; BYTE *window;

  /* check input */
  if (dest == Z_NULL || source == Z_NULL || source->state == Z_NULL || source->zalloc == (alloc_func)0 || source->zfree == (free_func)0)
  {
    return Z_STREAM_ERROR;
  }

  state = (struct inflate_state FAR*)source->state;

  /* allocate space */
  copy = (struct inflate_state FAR*)ZALLOC(source, 1, sizeof(struct inflate_state));

  if (copy == Z_NULL)
  {
    return Z_MEM_ERROR;
  }

  window = Z_NULL;

  if (state->window != Z_NULL)
  {
    window = (BYTE FAR*)ZALLOC(source, 1U << state->wbits, sizeof(BYTE));

    if (window == Z_NULL)
    {
      ZFREE(source, copy); return Z_MEM_ERROR;
    }
  }

  /* copy state */
  *dest =  *source;  *copy =  *state;

  copy->lencode = copy->codes + (state->lencode - state->codes); copy->distcode = copy->codes + (state->distcode - state->codes); copy->next = copy->codes + (state->next - state->codes);

  if (window != Z_NULL)
  {
    zmemcpy(window, state->window, 1U << state->wbits);
  }

  copy->window = window; dest->state = (void*)copy; return Z_OK;
}
