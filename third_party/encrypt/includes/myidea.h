#ifndef _MY_IDEA_H
#define _MY_IDEA_H

#define IDEA_INT unsigned int

typedef struct idea_key_st
{
	IDEA_INT data[9][6];
} IDEA_KEY_SCHEDULE;
 
#define idea_mul(r,a,b,ul) \
	ul=(unsigned long)a*b; \
	if (ul != 0) \
	{ \
	r=(ul&0xffff)-(ul>>16); \
	r-=((r)>>16); \
	} \
else \
	r=(-(int)a-b+1); /* assuming a or b is 0 and in range */ 

#define E_IDEA(num) \
	x1&=0xffff; \
	idea_mul(x1,x1,*p,ul); p++; \
	x2+= *(p++); \
	x3+= *(p++); \
	x4&=0xffff; \
	idea_mul(x4,x4,*p,ul); p++; \
	t0=(x1^x3)&0xffff; \
	idea_mul(t0,t0,*p,ul); p++; \
	t1=(t0+(x2^x4))&0xffff; \
	idea_mul(t1,t1,*p,ul); p++; \
	t0+=t1; \
	x1^=t1; \
	x4^=t0; \
	ul=x2^t0; /* do the swap to x3 */ \
	x2=x3^t1; \
	x3=ul;
#define n2s(c,l)	(l =((IDEA_INT)(*((c)++)))<< 8L, \
	l|=((IDEA_INT)(*((c)++)))      )

/* taken directly from the 'paper' I'll have a look at it later */
inline IDEA_INT inverse(unsigned int xin)
{
	long n1,n2,q,r,b1,b2,t;

	if (xin == 0)
		b2=0;
	else
	{
		n1=0x10001;
		n2=xin;
		b2=1;
		b1=0;

		do	{
			r=(n1%n2);
			q=(n1-r)/n2;
			if (r == 0)
			{ if (b2 < 0) b2=0x10001+b2; }
			else
			{
				n1=n2;
				n2=r;
				t=b2;
				b2=b1-q*b2;
				b1=t;
			}
		} while (r != 0);
	}
	return((IDEA_INT)b2);
}

extern void idea_set_encrypt_key(const unsigned char *key, IDEA_KEY_SCHEDULE *ks);
extern void idea_set_decrypt_key(IDEA_KEY_SCHEDULE *ek, IDEA_KEY_SCHEDULE *dk);
extern void idea_encrypt(unsigned long *d, IDEA_KEY_SCHEDULE *key);
typedef void (* f_idea_set_encrypt_key)(const unsigned char *key, IDEA_KEY_SCHEDULE *ks);
typedef void (* f_idea_set_decrypt_key)(IDEA_KEY_SCHEDULE *ek, IDEA_KEY_SCHEDULE *dk);
typedef void (* f_idea_encrypt)(unsigned long *d, IDEA_KEY_SCHEDULE *key);

#endif

