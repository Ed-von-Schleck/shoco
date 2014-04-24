## Javascript Version
+++

For showing off, **shoco** ships with a Javascript version (`shoco.js`) that’s generated with [emscripten](https://github.com/kripken/emscripten). If you change the default compression model, you need to re-generate it by typing `make js`. You do need to have emscripten installed. The output is [asm.js](http://asmjs.org/) with a small shim to provide a convenient API:

```js
compressed = shoco.compress(input_string);
output_string = shoco.decompress(compressed);
```

The compressed string is really a [Uint8Array](https://developer.mozilla.org/en-US/docs/Web/API/Uint8Array), since that resembles a C string more closely. The Javascript version is not as furiously fast as the C version because there’s dynamic (heap) memory allocation involved, but I guess there’s no way around it.

`shoco.js` should be usable as a node.js module.
