#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "shoco.h"

#define BUFFER_SIZE 1024

static double ratio(int original, int compressed) {
  return ((double)(original - compressed) / (double)original);
}

static int percent(double ratio) {
  return ratio * 100;
}

static void print(char *in, char *out, double ratio) {
  printf("'%s', '%s', (%d%%)\n", in, out, percent(ratio));
}

int main() {
  char buffer[BUFFER_SIZE] = { 0 };
  char comp[BUFFER_SIZE] = { 0 };
  char out[BUFFER_SIZE] = { 0 };
  int count = 0;
  double ratios = 0;
  double rat = 0;
  int inlen, complen;

  while (fgets(buffer, BUFFER_SIZE, stdin) != NULL) {
    *strchr(buffer, '\n') = '\0';
    inlen = strlen(buffer);
    complen = shoco_compress(buffer, comp, BUFFER_SIZE);
    shoco_decompress(comp, out, BUFFER_SIZE);
    rat = ratio(inlen, complen);
    if (complen != 0) {
      ++count;
      ratios += rat;
    }
    if (0) {
      print(buffer, out, rat);
    }
    assert(strcmp(buffer, out) == 0);
  }
  printf("Number of compressed strings: %d, average compression ratio: %d%%\n", count, percent(ratios / count));
  return 0;
}
