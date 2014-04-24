### API
+++

Here is all of it:

```C
size_t shoco_compress(const char * const in, size_t len, char * const out, size_t bufsize);
size_t shoco_decompress(const char * const in, size_t len, char * const out, size_t bufsize);
```

If the `len` argument for `shoco_compress` is 0, the input char is assumed to be null-terminated. If it’s a positive integer, parsing the input will stop after this length, or at a null-char, whatever comes first. `shoco_decompress` however will need a positive integer for `len` (most likely you should pass the return value of `shoco_compress`).

The return value is the number of bytes written. If it is less than `bufsize`, all is well. In case of decompression, a null-terminator is written. If the return value is exactly `bufsize`, the output is all there, but _not_ null-terminated. It is up to you to decide if that’s an error or not. If the buffer is not large enough for the output, the return value will be `bufsize` + 1. You might want to allocate a bigger output buffer. The compressed string will never be null-terminated.

If you are sure that the input data is plain ASCII, your `out` buffer for `shoco_compress` only needs to be as large as the input string. Otherwise, the output buffer may need to be up to 2x as large as the input, if it’s a 1-byte encoding, or even larger for multi-byte or variable-width encodings like UTF-8.

For the standard values of _shoco_, maximum compression is 50%, so the `out` buffer for `shoco_decompress` needs to be a maximum of twice the size of the compressed string.

