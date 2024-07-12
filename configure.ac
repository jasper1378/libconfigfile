# Prelude.
AC_PREREQ([2.72])
AC_INIT([libconfigfile], [0.0])
AC_CONFIG_SRCDIR([src/parser.cpp])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIR([m4])
AM_INIT_AUTOMAKE([foreign tar-ustar -Wall -Werror])
LT_PREREQ([2.5.0.14-9a4a-dirty])
LT_INIT([])
AC_LANG([C++])

# Checks for programs.
AC_PROG_AR
AC_PROG_AWK
AC_PROG_CC
AC_PROG_CPP
AC_PROG_CXX
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET
AC_PROG_RANLIB
AM_PROG_AR

# Checks for libraries.

# Checks for header files.
AC_CHECK_HEADER_STDBOOL

# Checks for typedefs, structures, and compiler characteristics.
AC_C_INLINE
AC_TYPE_INT64_T
AC_TYPE_SIZE_T
AC_TYPE_UINT8_T
AC_CHECK_TYPES([ptrdiff_t])

# Check for library functions.

# Output files.
AC_CONFIG_FILES([Makefile deps/Makefile src/Makefile include/libconfigfile/Makefile])
AC_CONFIG_SUBDIRS([deps/bits-and-bytes])
AC_OUTPUT
