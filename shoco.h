#pragma once

#include <stddef.h>

size_t shoco_compress(const char * in, size_t len, char * out, size_t bufsize);
size_t shoco_decompress(const char * in, size_t len, char * out, size_t bufsize);
