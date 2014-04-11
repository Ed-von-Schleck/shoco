#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "shoco.h"

static const char USAGE[] = "compresses or decompresses your (presumably short) data.\n"
"usage: shoco {c(ompress),d(ecompress)}\n";

typedef enum {
  JOB_COMPRESS,
  JOB_DECOMPRESS,
} Job;


int main(int argc, char **argv) {
  Job job; 
  if (argc < 2) {
    puts(USAGE);
    return 1;
  }
  if (argv[1][0] == 'c')
    job = JOB_COMPRESS;
  else if (argv[1][0] == 'd')
    job = JOB_COMPRESS;
  else {
    puts(USAGE);
    return 1;
  }

  char buffer[BUFSIZ];
  char *input = NULL;
  char *tmp = NULL;
  int bytes_read = 0;

  while(feof(stdin) == 0) {
    int n = fread(buffer, 1, BUFSIZ, stdin);
    input = realloc(input, bytes_read + n);
    if (input) {
      memmove(&input[bytes_read], buffer, n);
      bytes_read += n;
    } else {
      puts("Memory error!");
      return 1;
    }
  }

  if (job == JOB_COMPRESS) {
    char compressed[bytes_read];
    int len = shoco_compress(input, compressed, bytes_read);
    fwrite(compressed, len, 1, stdout);
  }
  return 0;
}
