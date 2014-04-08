#include <string.h>
#include <stdio.h>
#include <stdint.h>

#include "shoco.h"
#define _SHOCO_INTERNAL
#include "shoco_table.h"

#if defined(_MSC_VER)
  #include <stdlib.h>
  #define bswap_16(x) _byteswap_ushort(x)
  #define bswap_32(x) _byteswap_ulong(x)
  #define bswap_64(x) _byteswap_uint64(x)
#elif defined (__GNUC__)
  #define bswap_16(x) __builtin_bswap16(x)
  #define bswap_32(x) __builtin_bswap32(x)
  #define bswap_64(x) __builtin_bswap64(x)
#else
  #include <byteswap.h>
#endif

#ifdef __GNUC__
  #define likely(x)       __builtin_expect((x),1)
  #define unlikely(x)     __builtin_expect((x),0)
#else
  #define likely(x) (x)
  #define unlikely(x) (x)
#endif

union Code {
  uint32_t word;
  char bytes[4];
};

static inline int successor_index(unsigned char chr, int chr_index) {
  int i = chrs_reversed[chr];
  return ((chr_index >= 0) && (i >= 0)) ? successors_reversed[chr_index][i] : -1;
}

static inline int check_successors(int * restrict successor_indices, int pack_n) {
  for (int i = 0; i < packs[pack_n].n_successors; ++i)
    if (successor_indices[i] >= 1 << packs[pack_n].successors_bits[i])
      return 0;
  return 1;
}

int shoco_compress(const char * const restrict original, char * const restrict out, int len) {
  int fits = 0;
  char *o = out;
  const char *in = original;
  int successor_indices[MAX_SUCCESSOR_N] = { 0 };
  int lead_chr_index;
  int last_chr_index;
  unsigned char current;
  int n_successors = 0;
  //unsigned int word = 0;
  union Code code;

  while (in[0] != '\0') {
    if (unlikely(o + 1 >= out + len))
      goto end;

    // find the longest string of known successors
    lead_chr_index = chrs_reversed[(unsigned char)in[0]];
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
      if ((n_successors >= packs[p].n_successors)
          && (lead_chr_index < 1 << packs[p].lead_bits)
          && (check_successors(successor_indices, p))) {
        if (o + packs[p].bytes_packed >= out + len)
          goto end;

        code.word = ((1 << packs[p].header_bits) - 2) << packs[p].offsets[0];
        code.word |= lead_chr_index << packs[p].offsets[1];

        for (int i = 0; i < packs[p].n_successors; ++i)
          code.word |= successor_indices[i] << packs[p].offsets[i + 2];

        code.word = bswap_32(code.word);
        for (int i = 0; i < packs[p].bytes_packed; ++i)
          o[i] = code.bytes[i];

        o += packs[p].bytes_packed;
        in += packs[p].bytes_unpacked;
        success = 1;
        break;
      }
    }

    if (!success) {
last_resort:
      if (unlikely(*in & 0x80)) {
        if (unlikely(o + 2 >= out + len))
          goto end;
        // non-ascii case
        *o++ = 0xff;
      }
      // ascii
      *o++ = *in++;
    }
  }

  fits = 1;

end:
  *o++ = '\0';
  return o - out - fits;
}

int shoco_decompress(const char * const restrict original, char * const restrict out, int len) {
  int fits = 0;
  char *o = out;
  const char *in = original;
  unsigned char current_char;
  int previous_index;
  union Code code;
  int offset;
  int mask;

  while (*in != '\0') {
    if (!(*in & 0x80)) {
      if (unlikely(o + 1 >= out + len))
        goto end;
      *o++ = *in++;
      continue;
    }
    int success = 0;
    for (int p = 0; p < PACK_COUNT; ++p) {
      int header = (1 << packs[p].header_bits) - 2;
      int header_mask = (1 << packs[p].header_bits) - 1;
      if ((*in >> (8 - packs[p].header_bits) & header_mask) == header) {
        if (o + packs[p].bytes_unpacked >= out + len)
          goto end;

        code.word = 0;
        for (int i = 0; i < packs[p].bytes_packed; ++i)
          code.bytes[i] = in[i];

        code.word = bswap_32(code.word);

        offset = packs[p].offsets[1];
        mask = (1 << packs[p].lead_bits) - 1;

        previous_index = (code.word >> offset) & mask;
        *o = chrs[previous_index];

        for (int i = 0; i < packs[p].n_successors; ++i) {
          offset = packs[p].offsets[i + 2];
          mask = (1 << packs[p].successors_bits[i]) - 1;
          current_char = successors[previous_index][(code.word >> offset) & mask];
          o[i + 1] = current_char;
          previous_index = chrs_reversed[current_char];
        }
        o += packs[p].bytes_unpacked;
        in += packs[p].bytes_packed;
        success = 1;
        break;
      }
    }
    if (success)
      continue;

    // non-ascii character
    if (o + 1 >= out + len)
      goto end;
    ++in;
    *o++ = *in++;
  }

  fits = 1;

end:
  *o++ = '\0';
  return o - out - fits;
}
