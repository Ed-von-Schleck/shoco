#include <stdio.h>
#include <string.h>

#include "smateco.h"

static void print(char *in, char *out, int inlen, int complen) {
  int equal = strcmp(in, out) == 0;
  int ratio = ((double)(inlen - complen) / (double)inlen) * 100;
  printf("in: '%s', working: %d, ratio: %d%\n", in, equal, ratio);
}

static void print_debug(char *in, char *out, int inlen, int complen) {
  int ratio = ((double)(inlen - complen) / (double)inlen) * 100;
  printf("in: '%s', out: %s, ratio: %d%\n", in, out, ratio);
}
 
static void inout(char *in) {
  int inlen, complen;
  char buf[256];
  char out[256];

  inlen = strlen(in);
  complen = smateco_compress(in, buf, 256);
  smateco_decompress(buf, out, 256);
  print(in, out, inlen, complen);
}

int main() {
  inout("test");
  inout("xxxx");
  inout("Hallo Welt!");
  inout("lore ipsum");
  inout("the end is near");
  inout("Hello World!");
  inout("this is the end.");
  inout("This is a rather long sentence, but let's see how it compresses.");
  inout("http://google.com");
}
