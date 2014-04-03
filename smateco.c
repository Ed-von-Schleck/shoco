#include <string.h>
#include <stdio.h>

extern const char chrs[16];
extern const char successors[16][8];
extern const int chrs_reversed[128];

static int successor_index(char chr, int chr_index) __attribute__ ((const));
static int successor_index(char chr, int chr_index) {
  char *chr_ptr = memchr(successors[chr_index], chr, 8);
  return chr_ptr != NULL ? chr_ptr - successors[chr_index] : -1;
}

int smateco_compress(const char * const restrict original, char * const restrict out, int len) {
  int fits = 0;
  char *o = out;
  const char *in = original;
  int chr_indices[13];
  int successor_indices[13];
  char current;
  int i;

  while (in[0] != '\0') {
    if (o + 1 >= out + len)
      goto end;

    // check for non-ascii character
    if (in[0] & 0x80) {
      *o++ = 0xe0;
      *o++ = *in++;
      continue;
    }

    // find the longest string of known successors
    chr_indices[0] = chrs_reversed[in[0]];
    if (chr_indices[0] == -1)
      goto last_resort;

    for (i = 0; i < 12; ++i) {
      current = in[i + 1];
      if (current == '\0')
        break;

      successor_indices[i] = successor_index(current, chr_indices[i]);
      if (successor_indices[i] == -1)
        break;

      chr_indices[i + 1] = chrs_reversed[current];
    }
    if (i > 9) {
      if (o + 6 >= out + len)
        goto end;
      *o++ = 0xf8 | chr_indices[0] >> 1;
      *o++ = chr_indices[0] << 7 | successor_indices[0] << 4 | successor_indices[1] << 1 | successor_indices[2] >> 2;
      *o++ = successor_indices[2] << 6 | successor_indices[3] << 3 | successor_indices[4];
      *o++ = successor_indices[5] << 4 | successor_indices[6] << 1 | successor_indices[7] >> 2;
      *o++ = successor_indices[7] << 6 | successor_indices[8] << 3 | successor_indices[9];
      in += 11;
      continue;
    }
    if (i > 4) {
      if (o + 4 >= out + len)
        goto end;
      *o++ = 0xf0 | chr_indices[0] >> 1;
      *o++ = chr_indices[0] << 7 | successor_indices[0] << 4 | successor_indices[1] << 1 | successor_indices[2] >> 2;
      *o++ = successor_indices[2] << 6 | successor_indices[3] << 3 | successor_indices[4];
      in += 6;
      continue;
    }
    if (i > 2) {
      if (o + 3 >= out + len)
        goto end;
      *o++ = 0xc0 | chr_indices[0] << 1 | successor_indices[0] >> 2;
      *o++ = successor_indices[0] << 6 | successor_indices[1] << 3 | successor_indices[2];
      /*
      uint16_t *tmp = (uint16_t *) o;
      *tmp = 0xc000  | chr_indices[0] << 9 |  successor_indices[0] << 6 | successor_indices[1] << 3 | successor_indices[2];
      *tmp = (*tmp >> 8) | (*tmp << 8);
      o += 2;
      */
      //*tmp = 0xc000;
      in += 4;
      continue;
    }
    if (i > 0) {
      if (chr_indices[0] > 7)
        goto last_resort;
      *o++ = 0x80 | chr_indices[0] << 3 | successor_indices[0];
      in += 2;
      continue;
    }
last_resort:
    *o++ = *in++;
  }

  fits = 1;

end:
  *o++ = '\0';
  return o - out - fits;
}


int smateco_decompress(const char * const restrict original, char * const restrict out, int len) {
  int fits = 0;
  char *o = out;
  const char *in = original;
  char current_char;
  int previous_index;

  while (*in != '\0') {
    //if ((*in & 0x7f) == *in) {
    if (!(*in & 0x80)) {
      if (o + 1 >= out + len)
        goto end;
      *o++ = *in++;
      continue;
    //} else if ((*in & 0xbf) == *in) {
    } else if (!(*in & 0x40)) {
      if (o + 3 >= out + len)
        goto end;
      previous_index = (in[0] >> 3) & 0x07;
      *o++ = chrs[previous_index];

      current_char = successors[previous_index][in[0] & 0x07];
      previous_index = chrs_reversed[current_char];
      *o++ = current_char;

      ++in;
    //} else if ((*in & 0xdf) == *in) {
    } else if (!(*in & 0x20)) {
      if (o + 5 >= out + len)
        goto end;
      previous_index = (in[0] >> 1) & 0x0f;
      *o++ = chrs[previous_index];

      current_char = successors[previous_index][((in[0] << 2) & 0x04) | ((in[1] >> 6) & 0x03)];
      previous_index = chrs_reversed[current_char];
      *o++ = current_char;

      current_char = successors[previous_index][(in[1] >> 3) & 0x07];
      previous_index = chrs_reversed[current_char];
      *o++ = current_char;

      current_char = successors[previous_index][in[1] & 0x07];
      previous_index = chrs_reversed[current_char];
      *o++ = current_char;

      in += 2;
    } else if (!(*in & 0x10)) {
      // non-ascii character
      if (o + 1 >= out + len)
        goto end;
      ++in;
      *o++ = *in++;

    } else if (!(*in & 0x08)) {
      // one 4-bit and five 3-bit chars
      if (o + 7 >= out + len)
        goto end;
      previous_index = ((in[0] & 0x07) << 1) | ((in[1] >> 7) & 0x1);
      *o++ = chrs[previous_index];

      current_char = successors[previous_index][(in[1] >> 4) & 0x07];
      *o++ = current_char;
      previous_index = chrs_reversed[current_char];

      current_char = successors[previous_index][(in[1] >> 1) & 0x07];
      *o++ = current_char;
      previous_index = chrs_reversed[current_char];

      current_char = successors[previous_index][((in[1] << 2) & 0x04) | ((in[2] >> 6) & 0x03)];
      *o++ = current_char;
      previous_index = chrs_reversed[current_char];

      current_char = successors[previous_index][(in[2] >> 3) & 0x07];
      *o++ = current_char;
      previous_index = chrs_reversed[current_char];

      current_char = successors[previous_index][in[2] & 0x07];
      *o++ = current_char;

      in += 3;
    } else {
      // one 4-bit and ten 3-bit chars
      if (o + 12 >= out + len)
        goto end;
      previous_index = ((in[0] & 0x07) << 1) | ((in[1] >> 7) & 0x1);
      *o++ = chrs[previous_index];

      current_char = successors[previous_index][(in[1] >> 4) & 0x07];
      previous_index = chrs_reversed[current_char];
      *o++ = current_char;

      current_char = successors[previous_index][(in[1] >> 1) & 0x07];
      previous_index = chrs_reversed[current_char];
      *o++ = current_char;

      current_char = successors[previous_index][((in[1] << 2) & 0x04) | ((in[2] >> 6) & 0x03)];
      previous_index = chrs_reversed[current_char];
      *o++ = current_char;

      current_char = successors[previous_index][(in[2] >> 3) & 0x07];
      previous_index = chrs_reversed[current_char];
      *o++ = current_char;

      current_char = successors[previous_index][in[2] & 0x07];
      previous_index = chrs_reversed[current_char];
      *o++ = current_char;


      current_char = successors[previous_index][(in[3] >> 4) & 0x07];
      previous_index = chrs_reversed[current_char];
      *o++ = current_char;

      current_char = successors[previous_index][(in[3] >> 1) & 0x07];
      previous_index = chrs_reversed[current_char];
      *o++ = current_char;

      current_char = successors[previous_index][((in[3] << 2) & 0x04) | ((in[4] >> 6) & 0x03)];
      previous_index = chrs_reversed[current_char];
      *o++ = current_char;

      current_char = successors[previous_index][(in[4] >> 3) & 0x07];
      previous_index = chrs_reversed[current_char];
      *o++ = current_char;

      current_char = successors[previous_index][in[4] & 0x07];
      previous_index = chrs_reversed[current_char];
      *o++ = current_char;

      in += 5;
    }
  }

  fits = 1;

end:
  *o++ = '\0';
  return o - out - fits;
}
