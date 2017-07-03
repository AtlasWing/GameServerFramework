#ifndef _MY_RC5_H
#define _MY_RC5_H
#include <stdlib.h>

#define RC5_ENCRYPT	1
#define RC5_DECRYPT	0

/* 32 bit.  For Alpha, things may get weird */
#define RC5_32_INT unsigned int
 
#define RC5_32_BLOCK		8
#define RC5_32_KEY_LENGTH	16 /* This is a default, max is 255 */

/* This are the only values supported.  Tweak the code if you want more
* The most supported modes will be
* RC5-32/12/16
* RC5-32/16/8
*/
#define RC5_8_ROUNDS	8
#define RC5_12_ROUNDS	12
#define RC5_16_ROUNDS	16

typedef struct rc5_key_st
{
	/* Number of rounds */
	int rounds;
	RC5_32_INT data[2*(RC5_16_ROUNDS+1)];
} RC5_32_KEY;

#define c2ln(c,l1,l2,n)	{ \
	c+=n; \
	l1=l2=0; \
	switch (n) { \
			case 8: l2 =((RC5_32_INT)(*(--(c))))<<24L; \
			case 7: l2|=((RC5_32_INT)(*(--(c))))<<16L; \
			case 6: l2|=((RC5_32_INT)(*(--(c))))<< 8L; \
			case 5: l2|=((RC5_32_INT)(*(--(c))));     \
			case 4: l1 =((RC5_32_INT)(*(--(c))))<<24L; \
			case 3: l1|=((RC5_32_INT)(*(--(c))))<<16L; \
			case 2: l1|=((RC5_32_INT)(*(--(c))))<< 8L; \
			case 1: l1|=((RC5_32_INT)(*(--(c))));     \
	} \
}
#undef c2l
#define c2l(c,l)	(l =((RC5_32_INT)(*((c)++)))    , \
	l|=((RC5_32_INT)(*((c)++)))<< 8L, \
	l|=((RC5_32_INT)(*((c)++)))<<16L, \
	l|=((RC5_32_INT)(*((c)++)))<<24L)

#define RC5_32_MASK	0xffffffffL

#define RC5_16_P	0xB7E1
#define RC5_16_Q	0x9E37
#define RC5_32_P	0xB7E15163L
#define RC5_32_Q	0x9E3779B9L
#define RC5_64_P	0xB7E151628AED2A6BLL
#define RC5_64_Q	0x9E3779B97F4A7C15LL

#define ROTATE_l32(a,n)     (((a)<<(int)(n))|((a)>>(32-(int)(n))))
#define ROTATE_r32(a,n)     (((a)>>(int)(n))|((a)<<(32-(int)(n))))
/*
#define ROTATE_l32(a,n)     _lrotl(a,n)
#define ROTATE_r32(a,n)     _lrotr(a,n)
*/

#define E_RC5_32(a,b,s,n) \
	a^=b; \
	a=ROTATE_l32(a,b); \
	a+=s[n]; \
	a&=RC5_32_MASK; \
	b^=a; \
	b=ROTATE_l32(b,a); \
	b+=s[n+1]; \
	b&=RC5_32_MASK;

#define D_RC5_32(a,b,s,n) \
	b-=s[n+1]; \
	b&=RC5_32_MASK; \
	b=ROTATE_r32(b,a); \
	b^=a; \
	a-=s[n]; \
	a&=RC5_32_MASK; \
	a=ROTATE_r32(a,b); \
	a^=b;

extern void RC5_32_set_key(RC5_32_KEY *key, int len, const unsigned char *data,	int rounds);
extern void RC5_32_encrypt(RC5_32_INT *d, RC5_32_KEY *key);
extern void RC5_32_decrypt(RC5_32_INT *d, RC5_32_KEY *key);
typedef void (* f_RC5_32_decrypt)(RC5_32_INT *d, RC5_32_KEY *key);
typedef void (* f_RC5_32_encrypt)(RC5_32_INT *d, RC5_32_KEY *key);
typedef void (* f_RC5_32_set_key)(RC5_32_KEY *key, int len, const unsigned char *data,
		    int rounds);
#endif
