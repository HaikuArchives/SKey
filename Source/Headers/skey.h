#ifndef _SKEYLIB_H
#	define _SKEYLIB_H

/* Server-side data structure for reading keys file during login */
struct skey {
	FILE *keyfile;
	char buf[256];
	char *logname;
	int n;
	char *seed;
	char *val;
	long	recstart; /*needed so reread of buffer is efficient*/


};

/* Client-side structure for scanning data stream for challenge */
struct mc {
	char buf[256];
	int skip;
	int cnt;
};

void md4_f(char *x);
int md4_keycrunch(char *result, char *seed, char *passwd);
void md5_f(char *x);
int md5_keycrunch(char *result, char *seed, char *passwd);
extern "C" char *btoe(char *engout, char *c);
char *put8(char *out, char *s);
int etob(char *out, char *e);
void rip(char *buf);

#endif
