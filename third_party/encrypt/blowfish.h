
#ifndef _BLOWFISH_H
#define _BLOWFISH_H

#define BF_ENCRYPT	1
#define BF_DECRYPT	0

/*
 * !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 * ! BF_LONG has to be at least 32 bits wide. If it's wider, then !
 * ! BF_LONG_LOG2 has to be defined along.                        !
 * !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 */

#define BF_LONG unsigned long
#define BF_LONG_LOG2 3


#define BF_ROUNDS	16
#define BF_BLOCK	8

typedef struct bf_key_st
{
	BF_LONG P[BF_ROUNDS+2];
	BF_LONG S[4*256];
} BF_KEY;

 
void BF_set_key(BF_KEY *key, int len, const unsigned char *data);

void BF_encrypt(BF_LONG *data,const BF_KEY *key);
void BF_decrypt(BF_LONG *data,const BF_KEY *key);


typedef void (* f_BF_set_key)(BF_KEY *key, int len, const unsigned char *data);
typedef void (* f_BF_encrypt)(BF_LONG *data,const BF_KEY *key);
typedef void (* f_BF_decrypt)(BF_LONG *data,const BF_KEY *key);

#endif

