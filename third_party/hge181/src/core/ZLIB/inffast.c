/* inffast.c -- fast decoding
 * Copyright (C) 1995-2003 Mark Adler
 * For conditions of distribution and use, see copyright notice in zlib.h
 */

#include "zutil.h"
#include "inftrees.h"
#include "inflate.h"
#include "inffast.h"


#define PUP(a) *(a)++

/*
Decode literal, length, and distance codes and write out the resulting
literal and match bytes until either not enough input or output is
available, an end-of-block is encountered, or a data error is encountered.
When large enough input and output buffers are supplied to inflate(), for
example, a 16K input buffer and a 64K output buffer, more than 95% of the
inflate execution time is spent in this routine.

Entry assumptions:

state->mode == LEN
strm->avail_in >= 6
strm->avail_out >= 258
start >= strm->avail_out
state->bits < 8

On return, state->mode is one of:

LEN -- ran out of enough output space or enough available input
TYPE -- reached end of block code, inflate() to interpret next block
BAD -- error in block data

Notes:

- The maximum input bits used by a length/distance pair is 15 bits for the
length code, 5 bits for the length extra, 15 bits for the distance code,
and 13 bits for the distance extra.  This totals 48 bits, or six bytes.
Therefore if strm->avail_in >= 6, then there is enough input to avoid
checking for available input while decoding.

- The maximum bytes that a single length/distance pair can output is 258
bytes, which is the maximum length that can be coded.  inflate_fast()
requires strm->avail_out >= 258 for each loop to avoid checking for
output space.
 */

void inflate_fast(z_streamp strm, DWORD start)
{
  struct inflate_state *state = (struct inflate_state*)strm->state;

  BYTE *in = strm->next_in;
  BYTE *last = in + (strm->avail_in - 5);
  BYTE *dest = strm->next_out;
  BYTE *beg = dest - (start - strm->avail_out);
  BYTE *end = dest + (strm->avail_out - 257);
  BYTE *window = window = state->window;
  BYTE *from;

  DWORD wsize = state->wsize;
  DWORD whave = state->whave;
  DWORD write = state->write;
  DWORD hold = state->hold;
  DWORD bits = state->bits;
  DWORD lmask = (1U << state->lenbits) - 1;
  DWORD dmask = (1U << state->distbits) - 1;

  DWORD op;
  DWORD len;
  DWORD dist;

  code const *lcode = state->lencode;
  code const *dcode = state->distcode;
  code this;

  /* decode literals and length/distances until end-of-block or not enough
  input data or output space */

  do
  {
    if (bits < 15)
    {
      hold += (DWORD)(PUP(in)) << bits;
      bits += 8;
      hold += (DWORD)(PUP(in)) << bits;
      bits += 8;
    }

    this = lcode[hold & lmask];

 dolen:

    op = (DWORD)(this.bits);
    hold >>= op;
    bits -= op;
    op = (DWORD)(this.op);

    if (!op)
    {
      PUP(dest) = (BYTE)(this.val);
    }
    else if (op &16)
    {
      len = (DWORD)(this.val);
      op &= 15; /* number of extra bits */

      if (op)
      {
        if (bits < op)
        {
          hold += (DWORD)(PUP(in)) << bits;
          bits += 8;
        }

        len += (DWORD)hold &((1U << op) - 1);
        hold >>= op;
        bits -= op;
      }

      if (bits < 15)
      {
        hold += (DWORD)(PUP(in)) << bits;
        bits += 8;
        hold += (DWORD)(PUP(in)) << bits;
        bits += 8;
      }

      this = dcode[hold &dmask];
      dodist: op = (DWORD)(this.bits);
      hold >>= op;
      bits -= op;
      op = (DWORD)(this.op);

      if (op & 16)
      {
        /* distance base */
        dist = (DWORD)(this.val);
        op &= 15; /* number of extra bits */

        if (bits < op)
        {
          hold += (DWORD)(PUP(in)) << bits;
          bits += 8;

          if (bits < op)
          {
            hold += (DWORD)(PUP(in)) << bits;
            bits += 8;
          }
        }

        dist += (DWORD)hold &((1U << op) - 1);
        hold >>= op;
        bits -= op;
        op = (DWORD)(dest - beg);

        if (dist > op)
        {
          op = dist - op;

          if (op > whave)
          {
            state->mode = BAD;
            break;
          }

          from = window;

          if (!write) /* very common case */
          {
            from += wsize - op;

            if (op < len) /* some from window */
            {
              len -= op;
              do
              {
                PUP(dest) = PUP(from);
              }
              while (--op);
              from = dest - dist; /* rest from output */
            }
          }
          else if (write < op) /* wrap around window */
          {
            from += wsize + write - op;
            op -= write;
            
            if (op < len)   /* some from end of window */
            {
              len -= op;
              
              do
              {
                PUP(dest) = PUP(from);
              }
              while (--op);
              
              from = window;
              
              if (write < len)  /* some from start of window */
              {
                op = write;
                len -= op;
                
                do
                {
                  PUP(dest) = PUP(from);
                }
                while (--op);
                
                from = dest - dist; /* rest from output */
              }
            }
          }
          else
          {
            from += write - op;
            
            if (op < len)
            {
              len -= op;
              
              do
              {
                PUP(dest) = PUP(from);
              }
              while (--op);
              
              from = dest - dist; /* rest from output */
            }
          }
          
		  memcpy(dest, from, len);
		  dest += len;
		  from += len;
          len = 0;          
        }
        else
        {
          from = dest - dist; /* copy direct from output */
		  
#ifdef __GNUC__
//          asm("movl $dest, %edi");
//          asm("movl $from, %esi");
//          asm("movl $len, %ecx");
//          asm("movl %edi, %eax");
//          asm("addl %ecx, %eax");
//          asm("movl %eax, $dest");
//          asm("repne movsb");
          memcpy(dest, from, len);
          dest += len;
#else
          _asm mov edi, dword ptr dest
          _asm mov esi, dword ptr from
          _asm mov ecx, dword ptr len
          _asm mov eax, edi
          _asm add eax, ecx
          _asm mov dword ptr dest, eax
          _asm repne movsb     
#endif
        }
      }
      else if (!(op & 64))
      {
        this = dcode[this.val + (hold &((1 << op) - 1))];
        goto dodist;
      }
      else
      {
        state->mode = BAD;
        break;
      }
    }
    else if (!(op & 64))
    {
      this = lcode[this.val + (hold &((1 << op) - 1))];
      goto dolen;
    }
    else if (op & 32)
    {
      state->mode = TYPE;
      break;
    }
    else
    {
      state->mode = BAD;
      break;
    }
  }
  while (in < last && dest < end);

  /* return unused bytes (on entry, bits < 8, so in won't go too far back) */

  len = bits >> 3;
  in -= len;
  bits -= len << 3;
  hold &= (1U << bits) - 1;

  /* update state and return */
  strm->next_in = in;
  strm->next_out = dest;
  strm->avail_in = (DWORD)(in < last ? 5+(last - in): 5-(in - last));
  strm->avail_out = (DWORD)(dest < end ? 257+(end - dest): 257-(dest - end));
  state->hold = hold;
  state->bits = bits;
}
