## Generating Compression Models
+++

Maybe your typical input isn’t english words. Maybe it’s german or french – or whole sentences. Or file system paths. Or URLs. While the standard compression model of **shoco** should work for all of these, it might be worthwile to train **shoco** for this specific type of input data.

Fortunately, that’s really easy: **shoco** includes a python script called `generate_compression_model.py` that takes one or more text files and ouputs a header file ready for **shoco** to use. Here’s an example that trains **shoco** with a dictionary (btw., not the best kind of training data, because it’s dominated by uncommon words):

```bash
$ ./generate_successor_table.py /usr/share/dict/words -o shoco_model.h
```

There are options on how to chunk and strip the input data – for example, if we want to train **shoco** with the words in a readme file, but without punctuation and whitespace, we could do

```bash
$ ./generate_successor_table.py --split=whitespace --strip=punctuation README.md
```

Since we haven’t specified an output file, the resulting table file is printed on stdout.

This is most likely all you’ll need to generate a good model, but if you are adventurous, you might want to play around with all the options of the script: Type `generate_compression_model.py --help` to get a friendly help message. We won’t dive into the details here, though – just one word of warning: Generating tables can be slow if your input data is large, and _especially_ so if you use the `--optimize-encoding` option. Using [pypy](http://pypy.org/) can significantly speed up the process. 

