#ifndef _BCRYPT_H_
#define _BCRYPT_H_

#define BLOWFISH_F(x) \
	(((ctx->sbox[0][x >> 24] + ctx->sbox[1][(x >> 16) & 0xFF]) \
	^ ctx->sbox[2][(x >> 8) & 0xFF]) + ctx->sbox[3][x & 0xFF])

typedef struct blowfish_context_t_ {
	unsigned int pbox[256];
	unsigned int sbox[4][256];
} blowfish_context_t;

unsigned int get_word(unsigned char* text, int bytes, int* offset);
void blowfish_encryptblock (blowfish_context_t *ctx, unsigned int  *hi,  unsigned int *lo);

#endif

