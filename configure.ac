
# Define the package version numbers
m4_define([SHOCO_MAJOR], 0)
m4_define([SHOCO_MINOR], 0)
m4_define([SHOCO_PATCH], 0)

# Define the libtool version numbers; check the Autobook, Section 11.4.
m4_define([SHOCO_CURRENT], 0)
m4_define([SHOCO_REVISION], 0)
m4_define([SHOCO_AGE], 0)

# Initialize autoconf & automake.
AC_PREREQ(2.61)
AC_INIT([shoco], [SHOCO_MAJOR.SHOCO_MINOR.SHOCO_PATCH],
        [], [shoco],
        [https://github.com/Ed-von-Schleck/shoco])

AC_DEFINE([PACKAGE_DESCRIPTION], ["shoco is a compressor for small text strings"], [Description])
AC_SUBST([PACKAGE_DESCRIPTION], ["shoco is a compressor for small text strings"])

AC_CONFIG_AUX_DIR([build])
AC_CONFIG_HEADERS([config.h])
#AC_CONFIG_SRCDIR([src])
AC_CONFIG_MACRO_DIR([m4])

AM_INIT_AUTOMAKE([foreign subdir-objects -Wall])

AX_CFLAGS_WARN_ALL

# Define macro variables for the package version numbers.
AC_DEFINE(SHOCO_VERSION_MAJOR, SHOCO_MAJOR, [Define the major version number.])
AC_DEFINE(SHOCO_VERSION_MINOR, SHOCO_MINOR, [Define the minor version number.])
AC_DEFINE(SHOCO_VERSION_PATCH, SHOCO_PATCH, [Define the patch version number.])
AC_DEFINE(SHOCO_VERSION_STRING, "SHOCO_MAJOR.SHOCO_MINOR.SHOCO_PATCH", [Define the version string.])

# Define substitutions for the libtool version numbers.
SHOCO_LT_CURRENT=SHOCO_CURRENT
SHOCO_LT_REVISION=SHOCO_REVISION
SHOCO_LT_AGE=SHOCO_AGE
AC_SUBST(SHOCO_LT_CURRENT)
AC_SUBST(SHOCO_LT_REVISION)
AC_SUBST(SHOCO_LT_AGE)

# Checks for programs
m4_ifdef([AM_PROG_AR], [AM_PROG_AR])
AC_PROG_CC
AC_PROG_CC_STDC
AC_PROG_CC_C99
AC_PROG_INSTALL
AC_PROG_LIBTOOL
AC_PROG_LN_S
AM_CONDITIONAL([GCC], [test x$GCC = xyes])

# Checks for header files
AC_HEADER_STDC

# Checks for typedefs, structures, and compiler characteristics
AC_C_CONST
AC_C_INLINE
AC_C_STRINGIZE
AC_TYPE_SIZE_T

# Define Makefiles.
AC_CONFIG_FILES([
  shoco.pc
  Makefile
])

# Generate the "configure" script.
AC_OUTPUT
