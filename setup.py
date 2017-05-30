from setuptools import setup, Extension
import os

if os.name == 'posix':
    cflags = ['-std=c99']
else:
    cflags = None

pyshoco = Extension(
    'pyshoco',
    sources=['pyshoco.c', 'shoco.c'],
    include_dirs=['.'],
    extra_compile_args=cflags)

setup(
    name='pyshoco',
    version='1.3.4',
    description='Shoco Python wrapper module',
    ext_modules=[pyshoco],
    author='R. Rowe',
    author_email='mega.gamer05@gmail.com',
    url='https://pypi.python.org/pypi/pyshoco/')
