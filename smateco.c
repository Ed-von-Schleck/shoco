#include <string.h>
#include <stdint.h>

#if defined(_MSC_VER)
  #include <stdlib.h>
  #define bswap_16(x) _byteswap_ushort(x)
  #define bswap_32(x) _byteswap_ulong(x)
  #define bswap_64(x) _byteswap_uint64(x)
#else
  #include <byteswap.h>
#endif

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
  int n_successors;

  while (in[0] != '\0') {
    if (o + 1 >= out + len)
      goto end;

    // check for non-ascii character
    if (in[0] & 0x80) {
      *o++ = 0xf8;
      *o++ = *in++;
      continue;
    }

    // find the longest string of known successors
    chr_indices[0] = chrs_reversed[in[0]];
    if (chr_indices[0] == -1)
      goto last_resort;

    for (n_successors = 0; n_successors < 12; ++n_successors) {
      current = in[n_successors + 1];
      if (current == '\0')
        break;

      successor_indices[n_successors] = successor_index(current, chr_indices[n_successors]);
      if (successor_indices[n_successors] == -1)
        break;

      chr_indices[n_successors + 1] = chrs_reversed[current];
    }
    if (n_successors > 9) {
      if (o + 6 >= out + len)
        goto end;
      uint32_t *tmp = (uint32_t *) o;
      *tmp = 0xf0000000
        | chr_indices[0] << 23
        | successor_indices[0] << 20
        | successor_indices[1] << 17
        | successor_indices[2] << 14
        | successor_indices[3] << 11
        | successor_indices[4] << 8
        | successor_indices[5] << 5
        | successor_indices[6] << 2
        | successor_indices[7] >> 1;

      *tmp = bswap_32(*tmp);
      o += 4;
      *o++ = successor_indices[7] << 7 | successor_indices[8] << 4 | successor_indices[9] << 1;
      in += 11;
      continue;
    }
    if (n_successors > 4) {
      if (o + 4 >= out + len)
        goto end;
      uint32_t *tmp = (uint32_t *) o;
      *tmp = 0xe0000000
        | chr_indices[0] << 23
        | successor_indices[0] << 20
        | successor_indices[1] << 17
        | successor_indices[2] << 14
        | successor_indices[3] << 11
        | successor_indices[4] << 8;
      *tmp = bswap_32(*tmp);
      o += 3;
      in += 6;
      continue;
    }
    if (n_successors > 2) {
      if (o + 3 >= out + len)
        goto end;
      uint16_t *tmp = (uint16_t *) o;
      *tmp = 0xc000
        | chr_indices[0] << 9
        | successor_indices[0] << 6
        | successor_indices[1] << 3
        | successor_indices[2];
      *tmp = bswap_16(*tmp);
      o += 2;
      in += 4;
      continue;
    }
    if (n_successors > 0) {
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

static int get_3_bits(const char *in, int offset) {
  if (offset < 0) {
    int overflow = (offset + 3);
    offset += 8;
    int mask1 = 0x7 >> overflow;
    int mask2 = 0x7 ^ mask1;
    return ((in[0] << (8 - offset) & mask2) | ((in[1] >> offset) & mask1));
  } else {
    return (in[0] >> offset) & 0x7;
  }
}

static int decode (const char *in, char *out, int bit_offset, int byte_offset, int n, int previous_index) {
  char *o = out;
  char current_char;
  for (int i = 0; i < n; ++i) {
    bit_offset -= 3;
    current_char = successors[previous_index][get_3_bits(in + byte_offset, bit_offset)];
    *o++ = current_char;
    previous_index = chrs_reversed[current_char];
    if (bit_offset < 0) {
      bit_offset += 8;
      ++byte_offset;
    }
  }
  return o - out;
}


int smateco_decompress(const char * const restrict original, char * const restrict out, int len) {
  int fits = 0;
  char *o = out;
  const char *in = original;
  char current_char;
  int previous_index;

  while (*in != '\0') {
    if (!(*in & 0x80)) {
      if (o + 1 >= out + len)
        goto end;
      *o++ = *in++;
    } else if (!(*in & 0x40)) {
      if (o + 3 >= out + len)
        goto end;
      previous_index = get_3_bits(in, 3);
      *o++ = chrs[previous_index];
      *o++ = successors[previous_index][in[0] & 0x07];
      ++in;
    } else if (!(*in & 0x20)) {
      if (o + 5 >= out + len)
        goto end;
      previous_index = (in[0] >> 1) & 0x0f;
      *o++ = chrs[previous_index];
      o += decode(in, o, 1, 0, 3, previous_index);
      in += 2;
    } else if (!(*in & 0x10)) {

      // one 4-bit and five 3-bit chars
      if (o + 7 >= out + len)
        goto end;
      previous_index = ((in[0] & 0x07) << 1) | ((in[1] >> 7) & 0x1);
      *o++ = chrs[previous_index];
      o += decode(in, o, 7, 1, 5, previous_index);
      in += 3;
    } else if (!(*in & 0x08)) {
      // one 4-bit and ten 3-bit chars
      if (o + 12 >= out + len)
        goto end;
      previous_index = ((in[0] & 0x07) << 1) | ((in[1] >> 7) & 0x1);
      *o++ = chrs[previous_index];
      o += decode(in, o, 7, 1, 10, previous_index);
      in += 5;
    } else {
      // non-ascii character
      if (o + 1 >= out + len)
        goto end;
      ++in;
      *o++ = *in++;
    }
  }

  fits = 1;

end:
  *o++ = '\0';
  return o - out - fits;
}
