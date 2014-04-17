shoco
=====

_shoco_ compresses short strings to be even smaller. It is very fast, but the most remarkable property is:  If your string is ASCII, _shoco_ guarantees that the compressed string will never be bigger than the input string. In fact, an ASCII string is proper input for the decompressor (and of course, it decompresses to the exact same string). _shoco_ comes with sane defaults, but includes the tools to train the compressor for _your_ specific type of input data. The maximum compression rate is 50%, but usually in the range of 30%-35% (although if you have _very_ special data, and trained _shoco_ accordingly, it might even be better).

_shoco_'s focus lies less on maximum compression (on might want to device an arithmetic coder or something similar to achieve this), but on speed and simplicity of code. Even when compressing and decompressing millions of strings, you should not experience a noticable impact as compared to using plain strings.

Installation
------------

Simply copy `shoco.c`, `shoco.h` and `shoco_table.h` over to your project. Then, include `shoco.h` to use it.

The standard table is trained for optimal compression of common english words. If that isn't your typical input, you might want to copy one of the other table files from the `tables` directory and rename it to `shoco_table.h`. If none of the tables work well for you, you can use the `generate_successor_table.py` script to generate a table file that's better suited to your input (provided you have good training data). For details on table generation, see below.

API
---

```C
size_t shoco_compress(const char * const in, size_t len, char * const out, size_t bufsize);
size_t shoco_decompress(const char * const in, size_t len, char * const out, size_t bufsize);
```

That's it. If the `len` argument for `shoco_compress` is 0, the input char is assumed to be `\0`-terminated. If it's a positive integer, parsing the input will stop after this length, or at a `\0`-char, whatever comes first. `shoco_decompress` however will need a positive integer for `len` (most likely you should pass the return value of `shoco_compress`).

The return value is the number of bytes written. If it is less than `bufsize`, all is well. In case of decompression, a `\0`-terminator is written. If the return value is exactly `bufsize`, the output is all there, but _not_ `\0`-terminated. It is up to you to decide if that's an error or not. If the buffer is not large enough for the output, the return value will be `bufsize` + 1. You might want to allocate a bigger output buffer. The compressed string will never be `\0`-terminated.

If you are sure that the input data is plain ASCII, your `out` buffer for `shoco_compress` only needs to be as large as the input string. Otherwise, the output buffer may need to be up to 2x as large as the input, if it's a 1-byte encoding, or even larger for multi-byte encodings like UTF-8.

For the standard values of _shoco_, maximum compression is 50%, so the `out` buffer for `shoco_decompress` needs to be a maximum of twice the size of the compressed string.

Generating Tables
-----------------

It's easy to generate tables suited for your kind of data: _shoco_ comes with a script that takes your training data (one or more files, or stdin if none are provided), and outputs a header file suitable as a replacement for the included `shoco_table.h`. An example that trains against a dictionary (btw., not the best kind of training data, because it's dominated by uncommon words):

```
$ ./generate_successor_table.py /usr/share/dict/words -o shoco_table.h
```

There are options on how to chunk and strip the input data – for example, if we want to train _shoco_ with the words in this readme, but without punctuation and whitespace, we could do

```
./generate_successor_table.py README.md --split=whitespace --strip=punctuation
```

Since we haven't specified an output file, the resulting table file is printed on stdout.

`generate_successor_table.py --help` prints a friendly help message:

```
usage: generate_successor_table.py [-h] [-o OUTPUT]
                                   [--split {newline,whitespace,none}]
                                   [--strip {whitespace,punctuation,none}]
                                   [--max-leading-char-bits MAX_LEADING_CHAR_BITS]
                                   [--max-successor-bits MAX_SUCCESSOR_BITS]
                                   [--encoding-types {1,2,3}]
                                   [--optimize-encoding]
                                   [file [file ...]]

Generate a succession table for 'shoco'.

positional arguments:
  file                  The training data file(s). If no input file is
                        specified, the input is read from STDIN.

optional arguments:
  -h, --help            show this help message and exit
  -o OUTPUT, --output OUTPUT
                        Output file for the resulting succession table.
  --split {newline,whitespace,none}
                        Split the input into chunks at this separator.
                        Default: newline
  --strip {whitespace,punctuation,none}
                        Remove leading and trailing characters from each
                        chunk. Default: whitespace

table and encoding generation arguments:
  Higher values may provide for better compression ratios, but will make
  compression/decompression slower. Likewise, lower numbers make
  compression/decompression faster, but will likely make hurt the
  compression ratio. The default values are mostly a good compromise.

  --max-leading-char-bits MAX_LEADING_CHAR_BITS
                        The maximum amount of bits that may be used for
                        representing a leading character. Default: 5
  --max-successor-bits MAX_SUCCESSOR_BITS
                        The maximum amount of bits that may be used for
                        representing a successor character. Default: 4
  --encoding-types {1,2,3}
                        The number of different encoding schemes. If your
                        input strings are very short, consider lower values.
                        Default: 3
  --optimize-encoding   Find the optimal packing structure for the training
                        data. This rarely leads to different results than the
                        default values, and it is *slow*. Use it for very
                        unusual input strings, or when you use non-default
                        table generation arguments.
```

Since generating tables can be slow if your input data is large, and _especially_ so if you use the `--otimize-encoding` option, using [pypy](http://pypy.org/) can significantly speed up the process. 

Comparison With Other Compressors
---------------------------------

### smaz ###

There's another good small string compressor out there: [smaz](https://github.com/antirez/smaz). Smaz seems to be dictionary based, while _shoco_ is an entropy encoder. As a result, smaz will often do better than _shoco_ when compressing common english terms. However, _shoco_ typically beats smaz for more obscure input, as long as it's ASCII. Smaz may enlarge your string for uncommon words (like numbers), _shoco_ will never do that for ASCII strings.

Performance-wise, _shoco_ is typically faster by at least a factor of 2. As an example, compressing and decompressing all words in `/usr/dict/share/words` with _smaz_ takes around 0.325s on my computer and compresses on average by 28%, while _shoco_ has a compression average of 33% (with the standard table; an optimized table will be even better) and takes around 0.140s. _shoco_ is _especially_ fast at decompression.

_shoco_ can be trained with user data, while _smaz_'s dictionary is built-in. That said, the maximum compression rate of _smaz_ is hard to reach for _shoco_, so depending on your input type, you might fare better with _smaz_ (there's no way around it: You have to measure it yourself).

### gzip, xz ###

_shoco_'s compression ratio can't (and doesn't want to) compete with gzip et al. for string sizes bigger than, say, a hundred bytes or so. But for strings up to that size, _shoco_ is a good contender, and for very very small strings, it will always be better than standard compressors.

The performance of _shoco_ should always be several times faster than about any standard compression tool. For testing purposes, there's a binary called `shoco` included that compresses and decompresses single files. The following timings were made with this command line tool. The data is `/usr/share/dict/words` (size: 4,953,680), compressing it as a whole (not a strong point of _shoco_):

                   | shoco     | gzip      | xz
-------------------|-----------|-----------|-------
compression time   | 0.070s    | 0.470s    | 3.300s
decompression time | 0.010s    | 0.048s    | 0.148s
compressed size    | 3,393,975 | 1,476,083 | 1,229,980

This demonstates quite clearly that _shoco_'s compression rate sucks, but also that it's _very_ fast.

Use Cases
----------

As of now, there are no known uses of _shoco_ in real-life projects. If you do use _shoco_, I would love to hear about it! Possible use cases might include i18n tools like gettext (Strings appearing in GUIs tend to be rather short, and should compress quite well), or transfering short messages over a slow network (Twitter?), especially if the cpus on either side are too undepowered to run full-blown compressors (like embedded devices sometimes are).

To Do
-----

_shoco_ works for me – but I'd have only tested it on x86_64 Linux. Feedback on how it runs on other systems, especially Windows, would be highly appreciated! If it fails, it's a bug (and given the size of the project, it should be easy to fix). Other than that, there's a few issues that could stand some improvements:

* There should be more tests, because there's _never_ enough tests. Ever. Patches are very welcome!
* Tests should include table generation. As that involves re-compilation, these should probably written as a Makefile, or in bash or Python (maybe using `ctypes` to call the _shoco_-functions directly).
* The Python script for table generation should see some clean-up, as well as documentation. Also it should utilize all cpu cores (presumably via the `multiprocess`-module). This is a good task for new contributers!
* Again for table generation: Investigate why _pypy_ isn't as fast as should be expected.

Saying Thanks
-------------

If you use _shoco_, or like it for whatever reason, I'd really love to hear from you! Also, please consider supporting me financially via
[git tip](https://www.gittip.com/Ed-von-Schleck/) or [flattr](https://flattr.com/submit/auto?user_id=Christian.Schramm&url=https://github.com/Ed-von-Schleck/&title=shoco&language=C&tags=github&category=software).

License
-------

_shoco_ is published under the MIT license; see the LICENSE file for details.

Authors
-------

_shoco_ is written by Christian Schramm.
