shoco
=====

_shoco_ compresses short strings to be even smaller. It fast, too, and if your string is ASCII, _shoco_ guarantees that the compressed string will never be bigger than the input string. It comes with sane defaults, but includes the tools to train the compressor for _your_ specific type of input data. The maximum compression rate is 50%, but usually in the range of 30%-35% (although if you have _very_ special data, and trained _shoco_ accordingly, it might even be better).

API
---

```C
int shoco_compress(const char * const in, char * const out, int bufsize, int strlen);
int shoco_decompress(const char * const in, char * const out, int bufsize, int strlen);
```

That's it. If `strlen` is 0, the input char is assumed to be `\0`-terminated. If it's a positive integer, parsing the input will stop after this length, or at a `\0`-char, whatever comes first.

The return value is the number of bytes written. If it is less than `bufsize`, all is well, and a `\0`-terminator is written. If it is exactly `bufsize`, the output is all there, but _not_ `\0`-terminated. It is up to you to decide if that's an error or not. If the buffer is not large enough for the output, the return value will be `bufsize` + 1. You might want to allocate a bigger output buffer.

If you are sure that the input data is plan ASCII, your `out` buffer for `shoco_compress` only needs to be as large as the input string (add 1 byte if you want it to be `\0`-terminated). Otherwise, the output buffer may need to be up to 2x as large as the input, if it's a 1-byte encoding, or even larger for multi-byte encodings like UTF-8.

For the standard values of _shoco_, maximum compression is 50%, so the `out` buffer of `shoco_decompress` needs to be a maximum of twice the size of the compressed string.
