#include "md4.h"
#include "md5.h"
#include "skey.h"

#include <stdlib.h>
#include <string.h>

/*
 * Crunch a key:
 * concatenate the seed and the password, run through MD4 and
 * collapse to 64 bits. This is defined as the user's starting key.
 */
int md4_keycrunch(char *result, char *seed, char *passwd)
{
	char *buf;
	MD4_CTX md;
	unsigned long results[4];
	unsigned int buflen;
	
	buflen = strlen(seed) + strlen(passwd);
	if((buf = (char *)malloc(buflen+1)) == NULL)
		return -1;
	strcpy(buf, seed);
	strcat(buf, passwd);

	/* Crunch the key through MD4 */
	MD4Init(&md);
	MD4Update(&md, (unsigned char *)buf, buflen);
	MD4Final((unsigned char *)results, &md);
	free(buf);

	results[0] ^= results[2];
	results[1] ^= results[3];

	memcpy(result, (char *)results,8);

	return 0;
}

int md5_keycrunch(char *result, char *seed, char *passwd)
{
	char *buf;
	MD5_CTX md;
	unsigned long results[4];
	unsigned int buflen;
	
	buflen = strlen(seed) + strlen(passwd);
	if((buf = (char *)malloc(buflen+1)) == NULL)
		return -1;
	strcpy(buf, seed);
	strcat(buf, passwd);

	/* Crunch the key through MD5 */
	MD5Init(&md);
	MD5Update(&md, (unsigned char *)buf, buflen);
	MD5Final((unsigned char *)results, &md);
	free(buf);

	results[0] ^= results[2];
	results[1] ^= results[3];

	memcpy(result, (char *)results,8);

	return 0;
}

/* The one-way function f(). Takes 8 bytes and returns 8 bytes in place */
void md4_f(char *x)
{
	MD4_CTX md;
	unsigned long results[4];

	MD4Init(&md);
	MD4Update(&md,(unsigned char *)x,8);
	MD4Final((unsigned char *)results,&md);
	/* Fold 128 to 64 bits */
	results[0] ^= results[2];
	results[1] ^= results[3];

	memcpy(x,(char *)results,8);
}

void md5_f(char *x)
{
	MD5_CTX md;
	unsigned long results[4];

	MD5Init(&md);
	MD5Update(&md,(unsigned char *)x,8);
	MD5Final((unsigned char *)results,&md);
	/* Fold 128 to 64 bits */
	results[0] ^= results[2];
	results[1] ^= results[3];

	memcpy(x,(char *)results,8);
}
