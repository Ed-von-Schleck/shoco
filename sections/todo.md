## Things Still To Do
+++

**shoco** is stable, and it works well – but I’d have only tested it with gcc/clang on x86_64 Linux. Feedback on how it runs on other OSes, compilers and architectures would be highly appreciated! If it fails, it’s a bug (and given the size of the project, it should be easy to fix). Other than that, there’s a few issues that could stand some improvements:

* There should be more tests, because there’s _never_ enough tests. Ever. Patches are very welcome!
* Tests should include model generation. As that involves re-compilation, these should probably written as a Makefile, or in bash or Python (maybe using `ctypes` to call the **shoco**-functions directly).
* The Python script for model generation should see some clean-up, as well as documentation. Also it should utilize all cpu cores (presumably via the `multiprocess`-module). This is a good task for new contributers!
* Again for model generation: Investigate why **pypy** isn’t as fast as should be expected ([jitviewer](https://bitbucket.org/pypy/jitviewer/) might be of help here).
* Make a real **node.js** module.
* The current SSE2 optimization is probably not optimal. Anyone who loves to tinker with these kinds of micro-optimizations is invited to try his or her hand here.
* Publishing/packaging it as a real library probably doesn’t make much sense, as the model is compiled-in, but maybe we should be making it easier to use **shoco** as a git submodule (even if it’s just about adding documentation), or finding other ways to avoid the copy&paste installation.
