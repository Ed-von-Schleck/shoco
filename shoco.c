#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <limits.h>
#include <assert.h>

#if (defined (__BYTE_ORDER__) && (__BYTE_ORDER__ == __ORDER_BIG_ENDIAN__) || __BIG_ENDIAN__)
  #define swap(x) (x)
#else
  #if defined(_MSC_VER)
    #include <stdlib.h>
    #define swap(x) _byteswap_ulong(x)
  #elif defined (__GNUC__)
    #define swap(x) __builtin_bswap32(x)
  #else
    #include <byteswap.h>
    #define swap(x) bswap_32(x)
  #endif
#endif

#if defined(_MSC_VER)
  #define _ALIGNED __declspec(align(16))
#elif defined(__GNUC__)
  #define _ALIGNED __attribute__ ((aligned(16)))
#else
  #define _ALIGNED
#endif

#ifdef __GNUC__
  #define likely(x) __builtin_expect((x),1)
  #define unlikely(x) __builtin_expect((x),0)
#else
  #define likely(x) (x)
  #define unlikely(x) (x)
#endif

#if defined(_M_X64) || defined (_M_AMD64) || defined (__x86_64__)
  #include "emmintrin.h"
  #define HAVE_SSE2
#endif

static inline int decode_header(char val) {
#ifdef __GNUC__
  return (val & 0x80) ? __builtin_clrsb(val << 24) : -1;
#else
  int i = -1;
  while (val & 0x80) {
    val <<= 1;
    ++i;
  }
  return i;
#endif
}


#include "shoco.h"
#define _SHOCO_INTERNAL
#include "shoco_table.h"

union Code {
  uint32_t word;
  char bytes[4];
};



#ifdef HAVE_SSE2
static inline int check_indices(const short * indices, int pack_n) {
  __m128i zero = _mm_setzero_si128();
  __m128i indis = _mm_load_si128 ((__m128i *)indices);
  __m128i masks = _mm_load_si128 ((__m128i *)packs[pack_n].masks);
  __m128i cmp = _mm_cmpgt_epi16 (indis, masks);
  __m128i mmask = _mm_cmpgt_epi16 (masks, zero);
  cmp = _mm_and_si128 (cmp, mmask);
  int result = _mm_movemask_epi8 (cmp);
  return (result == 0);
}
#else
static inline int check_indices(const short indices[MAX_SUCCESSOR_N + 1], int pack_n) {
  for (int i = 0; i < packs[pack_n].bytes_unpacked; ++i)
    if (indices[i] > packs[pack_n].masks[i])
      return 0;
  return 1;
}
#endif

static inline int find_best_encoding(const short indices[MAX_SUCCESSOR_N + 1], int n_consecutive) {
  for (int p = PACK_COUNT - 1; p >= 0; --p)
    if ((n_consecutive >= packs[p].bytes_unpacked) && (check_indices(indices, p)))
      return p;
  return -1;
}

int shoco_compress(const char * const restrict original, char * const restrict out, int bufsize, int strlen) {
  char *o = out;
  const char *in = original;
  short _ALIGNED indices[8] = { 0 };
  int last_chr_index;
  int current_index;
  int successor_index;
  int n_consecutive = 0;
  union Code code;
  strlen = strlen <= 0 ? INT_MAX : strlen;

  while (*in != '\0') {
    if (unlikely(in - original > strlen))
      goto end;

    // find the longest string of known successors
    indices[0] = chrs_reversed[(unsigned char)in[0]];
    last_chr_index = indices[0];
    if (last_chr_index == -1)
      goto last_resort;

    for (n_consecutive = 1; (n_consecutive <= MAX_SUCCESSOR_N) && (in - original + n_consecutive <= strlen); ++n_consecutive) {
      current_index = chrs_reversed[(unsigned char)in[n_consecutive]];
      if (current_index < 0)  // this includes '\0'
        break;

      successor_index = successors_reversed[last_chr_index][current_index];
      if (successor_index == -1)
        break;

      indices[n_consecutive] = successor_index;
      last_chr_index = current_index;
    }
    if (n_consecutive < 2)
      goto last_resort;

    int p = find_best_encoding(indices, n_consecutive);
    if (p >= 0) {
      if (unlikely(o + packs[p].bytes_packed > out + bufsize))
        goto fail;

      code.word = packs[p].word;
      for (int i = 0; i < packs[p].bytes_unpacked; ++i)
        code.word |= indices[i] << packs[p].offsets[i];

      // in the little-endian world, we need to swap what's
      // in the register to match the memory representation
      code.word = swap(code.word);

      // if we'd just copy the, we could write over the end
      // of the output string
      for (int i = 0; i < packs[p].bytes_packed; ++i)
        o[i] = code.bytes[i];

      o += packs[p].bytes_packed;
      in += packs[p].bytes_unpacked;
    } else {
last_resort:
      if (unlikely(*in & 0x80)) {
        if (unlikely(o - out + 2 > bufsize))
          goto fail;
        // non-ascii case
        // put in a sentinel byte
        *o++ = 0xff;
        // and pretend it's just ...
      }
      if (unlikely(o - out + 1 > bufsize))
        goto fail;

      // an ascii byte
      *o++ = *in++;
    }
  }

end:
  if (o  - out < bufsize)
    *o = '\0';
  return o - out;
fail:
  return bufsize + 1;
}

int shoco_decompress(const char * const restrict original, char * const restrict out, int bufsize, int strlen) {
  char *o = out;
  const char *in = original;
  unsigned char current_char;
  int previous_index;
  union Code code;
  int offset;
  int mask;
  int mark;
  strlen = strlen <= 0 ? INT_MAX : strlen;

  while (*in != '\0') {
    if (unlikely(in - original > strlen))
      goto end;

    mark = decode_header(*in);
    if (mark < 0) {
      if (unlikely(o - out + 1 > bufsize))
        goto fail;
      *o++ = *in++;
    } else if (mark < PACK_COUNT) {
      if (unlikely(o - out + packs[mark].bytes_unpacked > bufsize))
        goto fail;

      if (unlikely(in - original + packs[mark].bytes_packed > strlen))
        goto fail;

      code.word = 0;
      for (int i = 0; i < packs[mark].bytes_packed; ++i)
        code.bytes[i] = in[i];

      code.word = swap(code.word);

      offset = packs[mark].offsets[0];
      mask = packs[mark].masks[0];
      previous_index = (code.word >> offset) & mask;
      *o = chrs[previous_index];

      for (int i = 1; i < packs[mark].bytes_unpacked; ++i) {
        offset = packs[mark].offsets[i];
        mask = packs[mark].masks[i];
        current_char = successors[previous_index][(code.word >> offset) & mask];
        o[i] = current_char;
        previous_index = chrs_reversed[current_char];
      }
      o += packs[mark].bytes_unpacked;
      in += packs[mark].bytes_packed;
    } else {
      // non-ascii character
      if (unlikely(o - out  + 1 > bufsize))
        goto fail;
      ++in;
      *o++ = *in++;
    }
  }

end:
  if (o  - out < bufsize)
    *o = '\0';
  return o - out;
fail:
  return bufsize + 1;
}
