from distutils.core import setup, Extension

pyshoco = Extension('pyshoco', sources = ['pyshoco.c'], libraries = ['shoco'], library_dirs = ['.'])

setup(name = 'pyshoco', version = '1.0', description = 'Shoco Python wrapper module', ext_modules = [pyshoco])