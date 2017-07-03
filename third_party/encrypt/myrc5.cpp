#include "myrc5.h"

void RC5_32_set_key(RC5_32_KEY *key, int len, const unsigned char *data,
		    int rounds)
	{
	RC5_32_INT L[64],l,ll,A,B,*S,k;
	int i,j,m,c,t,ii,jj;

	if (	(rounds != RC5_16_ROUNDS) &&
		(rounds != RC5_12_ROUNDS) &&
		(rounds != RC5_8_ROUNDS))
		rounds=RC5_16_ROUNDS;

	key->rounds=rounds;
	S= &(key->data[0]);
	j=0;
	for (i=0; i<=(len-8); i+=8)
		{
		c2l(data,l);
		L[j++]=l;
		c2l(data,l);
		L[j++]=l;
		}
	ii=len-i;
	if (ii)
		{
		k=len&0x07;
		c2ln(data,l,ll,k);
		L[j+0]=l;
		L[j+1]=ll;
		}

	c=(len+3)/4;
	t=(rounds+1)*2;
	S[0]=RC5_32_P;
	for (i=1; i<t; i++)
		S[i]=(S[i-1]+RC5_32_Q)&RC5_32_MASK;

	j=(t>c)?t:c;
	j*=3;
	ii=jj=0;
	A=B=0;
	for (i=0; i<j; i++)
		{
		k=(S[ii]+A+B)&RC5_32_MASK;
		A=S[ii]=ROTATE_l32(k,3);
		m=(int)(A+B);
		k=(L[jj]+A+B)&RC5_32_MASK;
		B=L[jj]=ROTATE_l32(k,m);
		if (++ii >= t) ii=0;
		if (++jj >= c) jj=0;
		}
	}

void RC5_32_encrypt(RC5_32_INT *d, RC5_32_KEY *key)
	{
		RC5_32_INT a,b,*s;

		s=key->data;

		a=d[0]+s[0];
		b=d[1]+s[1];
		E_RC5_32(a,b,s, 2);
		E_RC5_32(a,b,s, 4);
		E_RC5_32(a,b,s, 6);
		E_RC5_32(a,b,s, 8);
		E_RC5_32(a,b,s,10);
		E_RC5_32(a,b,s,12);
		E_RC5_32(a,b,s,14);
		E_RC5_32(a,b,s,16);
		if (key->rounds == 12)
		{
			E_RC5_32(a,b,s,18);
			E_RC5_32(a,b,s,20);
			E_RC5_32(a,b,s,22);
			E_RC5_32(a,b,s,24);
		}
		else if (key->rounds == 16)
		{
			/* Do a full expansion to avoid a jump */
			E_RC5_32(a,b,s,18);
			E_RC5_32(a,b,s,20);
			E_RC5_32(a,b,s,22);
			E_RC5_32(a,b,s,24);
			E_RC5_32(a,b,s,26);
			E_RC5_32(a,b,s,28);
			E_RC5_32(a,b,s,30);
			E_RC5_32(a,b,s,32);
		}
		d[0]=a;
		d[1]=b;
	}

void RC5_32_decrypt(RC5_32_INT *d, RC5_32_KEY *key)
	{
		RC5_32_INT a,b,*s;

		s=key->data;

		a=d[0];
		b=d[1];
		if (key->rounds == 16) 
		{
			D_RC5_32(a,b,s,32);
			D_RC5_32(a,b,s,30);
			D_RC5_32(a,b,s,28);
			D_RC5_32(a,b,s,26);
			/* Do a full expansion to avoid a jump */
			D_RC5_32(a,b,s,24);
			D_RC5_32(a,b,s,22);
			D_RC5_32(a,b,s,20);
			D_RC5_32(a,b,s,18);
		}
		else if (key->rounds == 12)
		{
			D_RC5_32(a,b,s,24);
			D_RC5_32(a,b,s,22);
			D_RC5_32(a,b,s,20);
			D_RC5_32(a,b,s,18);
		}
		D_RC5_32(a,b,s,16);
		D_RC5_32(a,b,s,14);
		D_RC5_32(a,b,s,12);
		D_RC5_32(a,b,s,10);
		D_RC5_32(a,b,s, 8);
		D_RC5_32(a,b,s, 6);
		D_RC5_32(a,b,s, 4);
		D_RC5_32(a,b,s, 2);
		d[0]=a-s[0];
		d[1]=b-s[1];
	}

