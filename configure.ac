AC_INIT([libbitcoin], [2.5.0], [genjix@riseup.net])
AC_USE_SYSTEM_EXTENSIONS
AC_LANG(C++)
AC_CONFIG_AUX_DIR([build-aux])

# The BOOST_ROOT environment variable is *not* prioritize by the m4 tests.
# As a result there may be dependency conflicts between installed versions
# and a version specified in BOOST_ROOT.
AC_CONFIG_MACRO_DIR([m4])

AM_INIT_AUTOMAKE([subdir-objects -Wall -Werror])
AM_PROG_AR
LT_INIT
AC_PROG_CXX
AC_PROG_LIBTOOL
AC_GNU_SOURCE
AX_CXX_COMPILE_STDCXX_11(noext,mandatory)
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])

# TODO: make testnet a runtime parameter and remove this option.
AC_MSG_CHECKING(--enable-testnet argument)
AC_ARG_ENABLE(testnet,
    [  --enable-testnet        Enable testnet],
    [enable_testnet=$enableval],
    [enable_testnet="no"])
AC_MSG_RESULT($enable_testnet)
if test "$enable_testnet" = "yes"; then
  AC_DEFINE(ENABLE_TESTNET)
fi

# Boost doesn't publish package configs but has m4 tests.
# The current versions produce incorrect error message text.
AX_BOOST_BASE([1.49],,
    [AC_MSG_ERROR([Boost 1.49 or later is required but was not found.])])
AX_BOOST_DATE_TIME
AX_BOOST_FILESYSTEM
AX_BOOST_REGEX
AX_BOOST_SYSTEM

PKG_PROG_PKG_CONFIG

# GMP does not publish a package config, so we do a header and library test.
AC_CHECK_HEADER(gmp.h,,
    [AC_MSG_ERROR([GMP header is required but was not found.])],-)
AC_CHECK_LIB(gmp,__gmpz_init,,
    [AC_MSG_ERROR([GMP library is required but was not found.])])

# TODO: We require secp256k1 to require GMP (over OpenSSL).
PKG_CHECK_MODULES([secp256k1], [libsecp256k1 >= 0.0.1])

# build tests
AC_ARG_WITH([tests], [AS_HELP_STRING([--with-tests],
    [require build with tests. Requires pkg-config [default=no]])],
    [enable_tests_ext=$withval],
    [enable_tests_ext=no])

# conditionally require test dependencies
if test "x$enable_tests_ext" != "xno"; then
    AX_BOOST_UNIT_TEST_FRAMEWORK

    # libcrypto (from OpenSSL) is currently required for test validation only.
    PKG_CHECK_MODULES([crypto], [libcrypto >= 1.0.1])
fi

AM_CONDITIONAL(ENABLE_TESTS, test "x$enable_tests_ext" != "xno")



# Do not add contextual build parameters in configuration.
# http://stackoverflow.com/a/4680578
OPTIMIZATIONS="-fvisibility=internal"
WARNINGS="-Wall -Wextra -Wpedantic -Wno-missing-braces"
SAFEGUARDS="-fvisibility-inlines-hidden -fstack-protector-all"
AM_CXXFLAGS=$WARNINGS" "$SAFEGUARDS" "$OPTIMIZATIONS
AC_SUBST([AM_CXXFLAGS])

AC_ARG_WITH([pkgconfigdir], AS_HELP_STRING([--with-pkgconfigdir=PATH],
    [Path to the pkgconfig directory [[LIBDIR/pkgconfig]]]),
    [pkgconfigdir="$withval"], [pkgconfigdir='${libdir}/pkgconfig'])
AC_SUBST([pkgconfigdir])

AC_CONFIG_FILES([Makefile libbitcoin.pc])
AC_OUTPUT

