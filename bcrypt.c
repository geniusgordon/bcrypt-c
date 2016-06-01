#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bcrypt.h"
#include "base64.h"
#include "const.h"

unsigned int get_word(unsigned char* text, int bytes, int* offset) {
	unsigned word = 0;
	int i, j;
	j = *offset;
	for (i = 0; i < 4; i++, j++) {
		if (j >= bytes)
			j = 0;
		word = (word << 8) | text[j];
	}
	*offset = j;
	return word;
}

void blowfish_encryptblock(blowfish_context_t *ctx, unsigned int *hi, unsigned int *lo)
{
	int i, temp;

	for(i = 0; i < 16; i++) {
		*hi ^= ctx->pbox[i];
		*lo ^= BLOWFISH_F(*hi);
		temp = *hi, *hi = *lo, *lo = temp;
	}
	temp = *hi, *hi = *lo, *lo = temp;

	*lo ^= ctx->pbox[16];
	*hi ^= ctx->pbox[17];
}

void expand_key(blowfish_context_t* ctx,
  unsigned char* salt, unsigned char* key,
  int saltbytes, int keybytes
) {
	int i, j, k;
	for(i = 0, j = 0; i < 18; i++) {
    ctx->pbox[i] ^= get_word(key, keybytes, &j);
	}
	unsigned int hi = 0, lo = 0;
  for (i = 0, j = 0; i < 18; i += 2) {
    if (saltbytes > 0) {
      hi ^= get_word(salt, saltbytes, &j);
      lo ^= get_word(salt, saltbytes, &j);
    }
    blowfish_encryptblock(ctx, &hi, &lo);
		ctx->pbox[i] = hi;
		ctx->pbox[i + 1] = lo;
  }
	for(i = 0; i < 4; i++) {
		for(k = 0; k < 256; k += 2) {
      if (saltbytes > 0) {
        hi ^= get_word(salt, saltbytes, &j);
        lo ^= get_word(salt, saltbytes, &j);
      }
			blowfish_encryptblock(ctx, &hi, &lo);
			ctx->sbox[i][k] = hi;
			ctx->sbox[i][k + 1] = lo;
		}
	}
}

void eks_blowfish_setup(blowfish_context_t* ctx, int rounds, unsigned char* salt, unsigned char* key) {
	int i;
  int saltbytes = strlen((char*)salt);
  int keybytes = strlen((char*)key) + 1;
	for(i = 0; i < 4; i++)
		memcpy(ctx->sbox[i], ORIG_S[i], 256 * sizeof(int));
	memcpy(ctx->pbox, ORIG_P, 18 * sizeof(int));

  expand_key(ctx, salt, key, saltbytes, keybytes);
  for (i = 0; i < (1<<rounds); i++) {
    expand_key(ctx, (unsigned char*)"", key, 0, keybytes);
    expand_key(ctx, (unsigned char*)"", salt, 0, saltbytes);
  }
}

void encrypt_ecb(blowfish_context_t* ctx, unsigned int* cdata) {
  int i;
  for (i = 0; i < 6; i += 2) {
		blowfish_encryptblock(ctx, cdata+i, cdata+i+1);
  }
}

char* bcrypt(int rounds, unsigned char* salt, unsigned char* input) {
  int i;
  char* output = malloc(61 * sizeof(char));
  unsigned int cdata[16];
  unsigned char ctext[25];
  memcpy(cdata, CIPHER_TEXT, 16 * sizeof(int));

	blowfish_context_t *ctx = (blowfish_context_t *)malloc(sizeof(blowfish_context_t));
  eks_blowfish_setup(ctx, rounds, salt, input);
  for (i = 0; i < 64; i++) {
    encrypt_ecb(ctx, cdata);
  }
  for (i = 0; i < 6; i++) {
    ctext[4 * i + 3] = cdata[i] & 0xff;
    cdata[i] >>= 8;
    ctext[4 * i + 2] = cdata[i] & 0xff;
    cdata[i] >>= 8;
    ctext[4 * i + 1] = cdata[i] & 0xff;
    cdata[i] >>= 8;
    ctext[4 * i + 0] = cdata[i] & 0xff;
  }
  ctext[23] = '\0';
  snprintf(output, 8, "$2a$%2.2u$", rounds);
  memcpy(output + 7, base64_encode(salt), 22);
  memcpy(output + 29, base64_encode(ctext), 31);
  output[61] = '\0';

  return output;
}

int main() {
  /* char* salt = "uMPhwCcS8TyZbsQf.VQRn."; */
  /* char* salt = "UTzaUqMjqurmteIFwp3UlO"; // QQcmw6Hwk1i6yfs9KRb8IywpgRtiyH.'; */
  char* salt = "UXNLX6ygHHacJ/fnajgkNe"; // RhWg8hKdJndGuC0.OPYlympImrO0lTK';
  unsigned char* csalt = base64_decode((unsigned char*)salt);
  char* output = bcrypt(8, csalt, (unsigned char*)"fred");
  printf("%s\n", output);
  return 0;
}


