#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "base64.h"

char* CODE = "./ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
int INDEX[128] = {
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
  54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 0, 0, 0, 0, 0, 0,
  0, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16,
  17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 0, 0, 0, 0, 0,
  0, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42,
  43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 0, 0, 0, 0, 0
};

unsigned char* base64_encode(unsigned char* in) {
  int len = strlen((char*)in);
  unsigned char* out = malloc(sizeof(unsigned char) * len / 3 * 5);

  int in_offset = 0;
  int out_offset = 0;
  while (in_offset < len) {
    char tmp;
    tmp = (in[in_offset] >> 2) & 0x3f;
    out[out_offset] = CODE[tmp];
    in_offset++;
    out_offset++;

    tmp = (in[in_offset-1] & 0x3) << 4;
    if (in_offset == len) {
      out[out_offset] = CODE[tmp];
      out_offset++;
      break;
    }
    tmp |= in[in_offset] >> 4;
    out[out_offset] = CODE[tmp];
    in_offset++;
    out_offset++;

    tmp = (in[in_offset-1] & 0xf) << 2;
    if (in_offset == len) {
      out[out_offset] = CODE[tmp];
      out_offset++;
      break;
    }
    tmp |= in[in_offset] >> 6;
    out[out_offset] = CODE[tmp];
    out_offset++;

    tmp = in[in_offset] & 0x3f;
    out[out_offset] = CODE[tmp];
    in_offset++;
    out_offset++;
  }
  while (in_offset % 3 != 0) {
    out[out_offset] = '=';
    in_offset++;
    out_offset++;
  }
  out[out_offset] = '\0';
  return out;
}

unsigned char* base64_decode(unsigned char* in) {
  int len = strlen((char*)in);
  unsigned char* out = malloc(sizeof(unsigned char) * len);
  int in_offset = 0;
  int out_offset = 0;

  int i;
  int padding = 0;
  for (i = len-1; i >= 0; i--) {
    if (in[i] == '=') {
      padding++;
    }
  }

  while (in_offset < len - 1) {
    char c1, c2, c3, c4;
    c1 = INDEX[in[in_offset]];
    c2 = INDEX[in[in_offset + 1]];
    out[out_offset] = (c1 << 2) | ((c2 & 0x30) >> 4);
    in_offset++;
    out_offset++;

    if (in_offset + padding == len - 1) {
      break;
    }

    c3 = INDEX[in[in_offset + 1]];
    out[out_offset] = ((c2 & 0x0f) << 4) | ((c3 & 0x3c) >> 2);
    in_offset++;
    out_offset++;

    if (in_offset + padding == len - 1) {
      break;
    }

    c4 = INDEX[in[in_offset + 1]];
    out[out_offset] = ((c3 & 0x03) << 6) | c4;
    in_offset += 2;
    out_offset++;
  }
  out[out_offset] = '\0';
  return out;
}

/* int main(int argc, char const *argv[]) { */
/*   int i; */
/*   for (size_t i = 0; i < 64; i++) { */
/*     printf("%d: %c %d\n", i, CODE[i], INDEX[CODE[i]]); */
/*   } */
/*   return 0; */
/* } */

