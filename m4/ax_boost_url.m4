#
# SYNOPSIS
#
#   AX_BOOST_URL
#
# DESCRIPTION
#
#   Test for the Boost.URL library from the Boost C++ Libraries. This macro
#   checks whether the Boost.URL library is available and usable. It requires
#   the AX_BOOST_BASE macro to be called beforehand to set up Boost paths.
#
#   This macro sets:
#     - HAVE_BOOST_URL to "yes" or "no" in the configure script.
#     - BOOST_URL_LIB to the linker flags for Boost.URL, if found.
#
#   If Boost.URL is not found, the macro can either error out (if required)
#   or continue with a warning, depending on the --with-boost-url option.
#
# USAGE
#
#   AX_BOOST_BASE([1.74])  # Minimum Boost version with Boost.URL
#   AX_BOOST_URL
#
# LICENSE
#
#   Copyright (c) 2025
#
#   Copying and distribution of this file, with or without modification, are
#   permitted in any medium without royalty provided the copyright notice
#   and this notice are preserved. This file is offered as-is, without any
#   warranty.

AC_DEFUN([AX_BOOST_URL],
[
AC_PREREQ([2.69])
AC_REQUIRE([AX_BOOST_BASE])

AC_ARG_WITH([boost-url],
  [AS_HELP_STRING([--with-boost-url@<:@=special-lib@:>@],
    [use the Boost.URL library from boost - it is the first found or the one in special-lib])],
  [],
  [with_boost_url=yes])

AS_IF([test "x$with_boost_url" != xno],
[
  # Save original flags
  ax_boost_url_save_LIBS="$LIBS"
  ax_boost_url_save_CPPFLAGS="$CPPFLAGS"
  ax_boost_url_save_LDFLAGS="$LDFLAGS"

  # Initialize variables
  BOOST_URL_LIB=""
  HAVE_BOOST_URL=no

  # Use Boost include and library paths from AX_BOOST_BASE
  CPPFLAGS="$CPPFLAGS $BOOST_CPPFLAGS"
  LDFLAGS="$LDFLAGS $BOOST_LDFLAGS"

  # Check for Boost.URL header
  AC_CHECK_HEADER([boost/url.hpp],
    [HAVE_BOOST_URL=yes],
    [HAVE_BOOST_URL=no])

  AS_IF([test "x$HAVE_BOOST_URL" = xyes],
  [
    # Try to find the Boost.URL library
    # Boost.URL is header-only in newer versions, but we check for linking
    # to ensure compatibility with older setups or custom builds
    AC_MSG_CHECKING([whether the Boost.URL library links correctly])
    LIBS="$LIBS -lboost_url"
    AC_LINK_IFELSE(
      [AC_LANG_PROGRAM([#include <boost/url.hpp>],
        [boost::urls::url u; u.set_scheme("https");])],
      [BOOST_URL_LIB="-lboost_url"
       AC_MSG_RESULT([yes])],
      [AC_MSG_RESULT([no])
       HAVE_BOOST_URL=no])
  ])

  # Handle the result
  AS_IF([test "x$HAVE_BOOST_URL" = xyes],
  [
    AC_DEFINE([HAVE_BOOST_URL], [1], [Define if Boost.URL is available])
    AC_SUBST([BOOST_URL_LIB])
  ],
  [
    if test "x$with_boost_url" = xyes; then
      AC_MSG_ERROR([Boost.URL library not found. Try specifying --with-boost-url=/path/to/boost.])
    else
      AC_MSG_WARN([Boost.URL library not found; continuing without it.])
    fi
  ])

  # Restore original flags
  LIBS="$ax_boost_url_save_LIBS"
  CPPFLAGS="$ax_boost_url_save_CPPFLAGS"
  LDFLAGS="$ax_boost_url_save_LDFLAGS"
],
[
  AC_MSG_NOTICE([Boost.URL support disabled by --without-boost-url])
])

]) # AX_BOOST_URL
