#include <string.h>
#include <stdio.h>
#include <stdint.h>

#include "smateco.h"
#define _SMATECO_INTERNAL
#include "successor_table.h"

#if defined(_MSC_VER)
  #include <stdlib.h>
  #define bswap_16(x) _byteswap_ushort(x)
  #define bswap_32(x) _byteswap_ulong(x)
  #define bswap_64(x) _byteswap_uint64(x)
#else
  #include <byteswap.h>
#endif

#ifdef __GNUC__
#define likely(x)       __builtin_expect((x),1)
#define unlikely(x)     __builtin_expect((x),0)
#endif

#define PACK_COUNT 2
#define MAX_SUCCESSOR_N 12

typedef struct Pack {
  unsigned int bytes_packed;
  unsigned int bytes_unpacked;
  unsigned int header;
  unsigned int header_bits;
  unsigned int lead_bits;
  unsigned const int n_successors;
  unsigned const int *successors_bits;
} Pack;

static const int successors_1[] = { 2 };
static const int successors_2[] = { 3, 3, 3 };

static const Pack packs[PACK_COUNT] = {
  { 1, 2, 0x02, 2, 4, 1, successors_1 },
  { 2, 4, 0x06, 3, 4, 3, successors_2 },
};

static int successor_index(char chr, int chr_index) {
  int i = chrs_reversed[chr];
  return ((chr_index >= 0) && (i >= 0)) ? successors_reversed[chr_index][i] : -1;
}

static int check_successors(int * restrict successor_indices, int pack_n) {
  for (int i = 0; i < packs[pack_n].n_successors; ++i)
    if (successor_indices[i] >= 1 << packs[pack_n].successors_bits[i])
      return 0;
  return 1;
}

int smateco_compress(const char * const restrict original, char * const restrict out, int len) {
  int fits = 0;
  char *o = out;
  const char *in = original;
  int successor_indices[13];
  int lead_chr_index;
  int last_chr_index;
  char current;
  int n_successors = 0;
  unsigned int word = 0;
  int offset;

  while (in[0] != '\0') {
    if (unlikely(o + 1 >= out + len))
      goto end;

    // check for non-ascii character
    if (unlikely(in[0] & 0x80)) {
      *o++ = 0xf8;
      *o++ = *in++;
      continue;
    }

    // find the longest string of known successors
    lead_chr_index = chrs_reversed[in[0]];
    last_chr_index = lead_chr_index;
    if (lead_chr_index == -1)
      goto last_resort;

    for (n_successors = 0; n_successors < MAX_SUCCESSOR_N; ++n_successors) {
      current = in[n_successors + 1];
      if (current == '\0')
        break;

      successor_indices[n_successors] = successor_index(current, last_chr_index);
      if (successor_indices[n_successors] == -1)
        break;

      last_chr_index = chrs_reversed[current];
    }

    int success = 0;
    for (int p = PACK_COUNT - 1; p >= 0; --p) {
      if ((n_successors >= packs[p].n_successors) && (check_successors(successor_indices, p))) {
        if (o + packs[p].bytes_packed >= out + len)
          goto end;

        offset = sizeof(word) * 8 - packs[p].header_bits;
        word = packs[p].header << offset;
        offset -= packs[p].lead_bits;
        word |= lead_chr_index << offset;

        for (int i = 0; i < packs[p].n_successors != 0; ++i) {
          offset -= packs[p].successors_bits[i];
          word |= successor_indices[i] << offset;
        }
        word = bswap_32(word);
        memcpy(o, &word, packs[p].bytes_packed);

        o += packs[p].bytes_packed;
        in += packs[p].bytes_unpacked;
        success = 1;
        break;
      }
    }

    if (!success) {
last_resort:
      *o++ = *in++;
    }
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
    if (!(*in & 0x80)) {
      if (o + 1 >= out + len)
        goto end;
      *o++ = *in++;
    } else if (!(*in & 0x40)) {
      if (o + 3 >= out + len)
        goto end;
      previous_index = (in[0] >> 2) & 0x0f;
      *o++ = chrs[previous_index];
      *o++ = successors[previous_index][in[0] & 0x03];
      ++in;
    } else if (!(*in & 0x20)) {
      int p = 1;
      if (o + 6 >= out + len)
        goto end;

      uint16_t *tmp = (uint16_t *) in;
      *tmp = bswap_16(*tmp);

      int offset = sizeof(*tmp) * 8 - packs[p].header_bits - packs[p].lead_bits;
      int mask = (1 << packs[p].lead_bits) - 1;

      previous_index = (*tmp >> offset) & mask;
      *o = chrs[previous_index];

      for (int i = 0; i < packs[p].n_successors != 0; ++i) {
        offset -= packs[p].successors_bits[i];
        mask = (1 << packs[p].successors_bits[i]) - 1;
        current_char = successors[previous_index][(*tmp >> offset) & mask];
        o[i + 1] = current_char;
        previous_index = chrs_reversed[current_char];
      }
      o += packs[p].bytes_unpacked;
      in += packs[p].bytes_packed;
    } else if (!(*in & 0x10)) {
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
