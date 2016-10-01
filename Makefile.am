
EXTRA_DIST = README.md LICENSE generate_compressor_model.py Makefile.orig

ACLOCAL_AMFLAGS = -I m4 --install

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = shoco.pc

bootstrap: maintainer-clean
	./bootstrap

maintainer-clean-local:
	-rm -f aclocal.m4 config.h.in configure config/*
	-find ${builddir} -name Makefile.in -exec rm -f '{}' ';'

# data
shoco_model.h: models/words_en.h
	$(MAKE) -f Makefile.orig shoco_model.h

# lib
include_HEADERS = shoco.h
lib_LTLIBRARIES = libshoco.la
libshoco_la_SOURCES = shoco.h shoco.c shoco_model.h shoco_table.h
libshoco_la_LDFLAGS = \
	-no-undefined \
	-version-info $(SHOCO_LT_CURRENT):$(SHOCO_LT_REVISION):$(SHOCO_LT_AGE)

# bin
bin_PROGRAMS = shoco
shoco_SOURCES = shoco-bin.c
shoco_LDADD = $(top_builddir)/libshoco.la

# test
check_PROGRAMS = test_input tests
test_input_SOURCES = shoco.h test_input.c
test_input_LDADD = $(top_builddir)/libshoco.la
tests_SOURCES = shoco.h tests.c
tests_LDADD = $(top_builddir)/libshoco.la

TESTS = tests

.PHONY: bootstrap
