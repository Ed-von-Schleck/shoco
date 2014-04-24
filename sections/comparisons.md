## Comparisons With Other Compressors
+++

### smaz

There’s another good small string compressor out there: [**smaz**](https://github.com/antirez/**smaz**). **smaz** seems to be dictionary based, while **shoco** is an entropy encoder. As a result, **smaz** will often do better than **shoco** when compressing common english terms. However, **shoco** typically beats **smaz** for more obscure input, as long as it’s ASCII. Smaz may enlarge your string for uncommon words (like numbers), **shoco** will never do that for ASCII strings.

Performance-wise, **shoco** is typically faster by at least a factor of 2. As an example, compressing and decompressing all words in `/usr/dict/share/words` with **smaz** takes around 0.325s on my computer and compresses on average by 28%, while **shoco** has a compression average of 33% (with the standard table; an optimized table will be even better) and takes around 0.140s. **shoco** is _especially_ fast at decompression.

**shoco** can be trained with user data, while **smaz**’s dictionary is built-in. That said, the maximum compression rate of **smaz** is hard to reach for **shoco**, so depending on your input type, you might fare better with **smaz** (there’s no way around it: You have to measure it yourself).

### gzip, xz

As mentioned, **shoco**’s compression ratio can’t (and doesn’t want to) compete with gzip et al. for strings larger than a few bytes. But for very small strings, it will always be better than standard compressors.

The performance of **shoco** should always be several times faster than about any standard compression tool. For testing purposes, there’s a binary (unsurprisingly called `shoco`) included that compresses and decompresses single files. The following timings were made with this command line tool. The data is `/usr/share/dict/words` (size: 4,953,680), compressing it as a whole (not a strong point of **shoco**):

compressor | compression time | decompression time | compressed size
-----------|------------------|--------------------|----------------
shoco      | 0.070s           | 0.010s             | 3,393,975
gzip       | 0.470s           | 0.048s             | 1,476,083
xz         | 3.300s           | 0.148s             | 1,229,980

This demonstates quite clearly that **shoco**’s compression rate sucks, but also that it’s _very_ fast.
