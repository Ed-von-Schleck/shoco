#include "stdio.h"
#include "assert.h"
#include "string.h"

#include "shoco.h"

static const char LARGE_STR[] = "This is a large string that won't possibly fit into a small buffer";
static const char NON_ASCII_STR[] = "Übergrößenträger";

int main() {
  char buf_1[1];
  char buf_2[2];
  char buf_4[4];
  char buf_large[4096];
  int ret;

  // test compression
  ret = shoco_compress(LARGE_STR, buf_2, 2, 0);
  assert(ret == 3); // bufsize + 1 if buffer too small

  ret = shoco_compress(LARGE_STR, buf_large, 4096, 0);
  assert(ret <= strlen(LARGE_STR));

  ret = shoco_compress("a", buf_1, 1, 0);
  assert(ret == 1); // bufsize if null byte didn't fit

  buf_2[1] = 'x';
  ret = shoco_compress("a", buf_2, 2, 0);
  assert(ret == 1); // compressed string length without null byte
  assert(buf_2[1] == '\0'); // null byte written

  ret = shoco_compress("a", buf_4, 4, 0);
  assert(ret == 1);

  ret = shoco_compress("test", buf_4, 4, 0);
  assert(ret <= 4);

  buf_4[1] = 'x';
  ret = shoco_compress("test", buf_4, 4, 1); // buffer large enough, but strlen said "just compress first char"
  assert(ret == 1);
  assert(buf_4[1] == '\0'); // null byte written

  buf_4[1] = 'x';
  ret = shoco_compress("test", buf_4, 1, 1);
  assert(ret == 1);
  assert(buf_4[1] != '\0'); // no null byte written

  buf_4[1] = 'x';
  ret = shoco_compress("a", buf_4, 4, 1);
  assert(ret == 1);
  assert(buf_4[1] == '\0');

  buf_4[1] = 'x';
  ret = shoco_compress("a", buf_4, 4, 2);
  assert(ret == 1);
  assert(buf_4[1] == '\0');

  ret = shoco_compress("ä", buf_1, 1, 0); // this assumes that 'ä' is not in the frequent chars table
  assert(ret == 2);

  
  //test decompression
  char compressed_large[4096];
  int large_len = strlen(LARGE_STR);
  shoco_compress(LARGE_STR, compressed_large, 4096, 0);

  buf_large[large_len] = 'x';
  ret = shoco_decompress(compressed_large, buf_large, 4096, 0);
  assert(ret == large_len);
  assert(strcmp(buf_large, LARGE_STR) == 0);
  assert(buf_large[large_len] == '\0'); // null byte written
  
  ret = shoco_decompress(compressed_large, buf_2, 2, 0);
  assert(ret == 3); // ret = bufsize + 1, because buffer too small

  buf_large[large_len] = 'x';
  ret = shoco_decompress(compressed_large, buf_large, large_len, 0);
  assert(ret == large_len);
  assert(buf_large[large_len] != '\0'); // no null byte written

  ret = shoco_decompress(compressed_large, buf_large, 4096, 5);
  assert((ret < large_len) || (ret == 4097)); // either fail (bufsize + 1) or it happened to work


  char compressed_non_ascii[256];
  int non_ascii_len = strlen(NON_ASCII_STR);
  shoco_compress(NON_ASCII_STR, compressed_non_ascii, 256, 0);

  buf_large[non_ascii_len] = 'x';
  ret = shoco_decompress(compressed_non_ascii, buf_large, 4096, 0);
  assert(ret == non_ascii_len);
  assert(strcmp(buf_large, NON_ASCII_STR) == 0);
  assert(buf_large[non_ascii_len] == '\0'); // null byte written

  puts("All tests passed.");
  return 0;
}
