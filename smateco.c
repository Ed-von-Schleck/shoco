#include <string.h>
#include <stdio.h>

static const char letters[16] = {
  'e', 't', 'a', 'o', 'i', 'n', 's', 'h',
  'r', 'd', 'u', 'l', 'c', ' ', ',', '.'
};
static const char bigrams[16][2] = {
  {'t', 'h'},
  {'h', 'e'},
  {'i', 'n'},
  {'e', 'r'},
  {'a', 'n'},
  {'r', 'e'},
  {'n', 'd'},
  {'a', 't'},

  {'o', 'n'},
  {'n', 't'},
  {'h', 'a'},
  {'e', 's'},
  {'s', 't'},
  {'e', 'n'},
  {'e', 'd'},
  {'t', 'o'}
};

static int bigram_index(char chr1, char chr2) {
  for (int i = 0; i < 16; ++i)
    if ((bigrams[i][0] == chr1) && (bigrams[i][1] == chr2))
      return i;
  return -1;
}

static int letter_index(char chr) {
  char *letter_ptr = memchr(letters, chr, 16);
  return letter_ptr != NULL ? letter_ptr - letters : -1;
}

typedef struct {
  char higher;
  char lower;
} twochars;

static twochars try_triplet(char *o, int prefix, int t1, int t2, int t3) {
  twochars tmp = { 0 };
  if ((t1 != -1) && (t2 != -1) && (t3 != -1)) {
    tmp.higher = prefix | t1;
    tmp.lower = t2 << 4 | t3;
  }
  return tmp;
}

int smateco_compress(const char *in, char *const out, int len) {
  int overflow = 1;
  char *o = out;
  twochars tmp;

  while (in[0] != '\0') {
    if (in[1] != '\0') {
      int i1 = letter_index(in[0]);
      int i2 = letter_index(in[1]);
      // see if we can pack two letters into the char
      if ((i1 != -1) && (i2 != -1) && (i1 < 8) && (i2 < 8)) {
        if (o + 1 >= out + len)
          goto out;
        // prefix with b'10'
        *o++ = 0x80 | i1 << 3 | i2; 
        in += 2;
        continue;
      }
      if (in[2] != '\0') {
        int i3 = letter_index(in[2]);
        if (in[3] != '\0') {
          int i4 = letter_index(in[3]);
          // check if a bigram exists (and create encoded two-byte
          // representation).
          tmp = try_triplet(o, 0xc0, bigram_index(in[0], in[1]), i3, i4);
          if (!tmp.higher) {
            tmp = try_triplet(o, 0xd0, i1, bigram_index(in[1], in[2]), i4);
            if (!tmp.higher) {
              tmp = try_triplet(o, 0xe0, i1, i2, bigram_index(in[2], in[3]));
            }
          }

          if (tmp.higher) {
            if (o + 3 >= out + len)
              goto out;
            *o++ = tmp.higher;
            *o++ = tmp.lower;
            in += 4;
            continue;
          }
        }
        // if no bigram is found or string ends too early,
        // we see if we can pack three letters in two chars
        if ((i1 != -1) && (i2 != -1) && (i3 != -1)) {
          if (o + 2 >= out + len)
            goto out;
          // prefix with b'1111'
          *o++ = 0xf0 | i1;
          *o++ = i2 << 4 | i3;
          in += 3;
          continue;
        }
      }
    }
    // if all else fails, copy the char
    // verbatim.
    if (o + 1 >= out + len)
      goto out;
    *o++ = in[0];
    ++in;
  }
  overflow = 0;

out:
  *o = '\0';
  if (overflow)
    return len + 1;
  else
    return o - out;

}

int smateco_decompress(const char *in, char *const out, int len) {
  int overflow = 1;
  char *o = out;

  while (*in != '\0') {
    if (!(*in & 0x80)) {
      if (o + 1 >= out + len)
        goto out;
      *o++ = *in;
      ++in;
    } else if (((*in >> 6) & 1) == 0) {
        if (o + 2 >= out + len)
          goto out;
        *o++ = letters[(*in & 0x38) >> 3];
        *o++ = letters[(*in & 0x7)];
        ++in;
    } else {
      switch ((*in >> 4) & 0xf) {
        case 0x0c: {
          if (o + 4 >= out + len)
            goto out;
          *o++ = bigrams[(*in & 0x0f)][0];
          *o++ = bigrams[(*in & 0x0f)][1];
          ++in;
          *o++ = letters[(*in & 0xf0) >> 4];
          *o++ = letters[(*in & 0x0f)];
          ++in;
          break;
        }
        case 0x0d: {
          if (o + 4 >= out + len)
            goto out;
          *o++ = letters[(*in & 0x0f)];
          ++in;
          *o++ = bigrams[(*in & 0xf0) >> 4][0];
          *o++ = bigrams[(*in & 0xf0) >> 4][1];
          *o++ = letters[(*in & 0x0f)];
          ++in;
          break;

        }
        case 0x0e: {
          if (o + 4 >= out + len)
            goto out;
          *o++ = letters[(*in & 0x0f)];
          ++in;
          *o++ = letters[(*in & 0xf0) >> 4];
          *o++ = bigrams[(*in & 0x0f)][0];
          *o++ = bigrams[(*in & 0x0f)][1];
          ++in;
          break;

        }
        case 0x0f: {
          if (o + 3 >= out + len)
            goto out;
          *o++ = letters[(*in & 0x0f)];
          ++in;
          *o++ = letters[(*in & 0xf0) >> 4];
          *o++ = letters[(*in & 0x0f)];
          ++in;
          break;
        }
      }
    }
  }
  overflow = 0;
out:
  *o = '\0';
  if (overflow)
    return len + 1;
  else
    return o - out;
}
