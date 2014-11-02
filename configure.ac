# Declare the required version of autoconf.
AC_PREREQ([2.65])

# Process command-line arguments and perform initialization and verification.
AC_INIT([libbitcoin], [2.5.0], [genjix@riseup.net])

# Do compilation tests.
AC_LANG(C++)

# Specify the temporary directory for build tools.
AC_CONFIG_AUX_DIR([build-aux])

# Specify the directory of additional local Autoconf macros.
AC_CONFIG_MACRO_DIR([m4])

# Run macros for operation of generated Makefiles, enable non-recursive make.
AM_INIT_AUTOMAKE([subdir-objects])

# Enable C and Posix extensions that may be disabled on certain platforms.
AC_USE_SYSTEM_EXTENSIONS

# Enable the archiver.
AM_PROG_AR

# Initialize libtool.
LT_INIT

# Determine C++ compiler to use.
AC_PROG_CXX

# Enable shared libraries if available, and static if they don't conflict.
AC_PROG_LIBTOOL

# Declare using the GNU C library (glibc).
AC_GNU_SOURCE

# Compute the canonical host-system type variable host, including host_os.
AC_CANONICAL_HOST

# Check for baseline language coverage in the compiler for the C++11 standard.
AX_CXX_COMPILE_STDCXX_11([noext], [mandatory])

# Enable silent rules option.
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])

# Declare environment variables that affect the build.
#------------------------------------------------------------------------------
AC_ARG_VAR([CC], "C compiler to use, such as gcc or clang")
AC_ARG_VAR([CXX], "C++ compiler to use, such as g++ or clang++")
AC_ARG_VAR([PKG_CONFIG_PATH], "Additional directories for package discovery.")

# Support --with-pkgconfigdir option (after verifying package config install).
#------------------------------------------------------------------------------
PKG_PROG_PKG_CONFIG
AC_MSG_CHECKING(--with-pkgconfigdir option)
AC_ARG_WITH([pkgconfigdir], 
    AS_HELP_STRING([--with-pkgconfigdir=PATH], 
        [Path to pkgconfig directory.]),
    [pkgconfigdir=$withval], 
    [pkgconfigdir=${libdir}/pkgconfig])
AC_MSG_RESULT($pkgconfigdir)
AC_SUBST([pkgconfigdir])

# Support --enable-debug option
#------------------------------------------------------------------------------
AC_MSG_CHECKING(--enable-debug option)
AC_ARG_ENABLE([debug],
    AS_HELP_STRING([--enable-debug], 
        [Compile with debug assertions.]),
    [enable_debug=$enableval],
    [enable_debug=no])
AC_MSG_RESULT($enable_debug)
AS_IF([test x$enable_debug != xno], AC_DEFINE([DEBUG]))

# Support --enable-testnet option
#------------------------------------------------------------------------------
AC_MSG_CHECKING(--enable-testnet option)
AC_ARG_ENABLE([testnet],
    AS_HELP_STRING([--enable-testnet], 
        [Compile for testnet.]),
    [enable_testnet=$enableval],
    [enable_testnet=no])
AC_MSG_RESULT($enable_testnet)
AS_IF([test x$enable_testnet != xno], AC_DEFINE([ENABLE_TESTNET]))

# Support --with-tests option
#------------------------------------------------------------------------------
AC_MSG_CHECKING(--with-tests option)
AC_ARG_WITH([tests],
    AS_HELP_STRING([--with-tests], 
        [Compile with unit tests.]),
    [with_tests=$withval],
    [with_tests=no])
AC_MSG_RESULT($with_tests)

# Require boost min version.
#------------------------------------------------------------------------------
# Boost does not publish package configs but has m4 tests.
# The BOOST_ROOT environment variable is *not* prioritize by the m4 tests.
# As a result there may be dependency conflicts between installed versions
# and a version specified in BOOST_ROOT.
# These scripts may produce incorrect error message text.
AX_BOOST_BASE([1.49],,
    [AC_MSG_ERROR([Boost 1.49 or later is required but was not found.])])
AX_BOOST_DATE_TIME
AX_BOOST_FILESYSTEM
AX_BOOST_REGEX
AX_BOOST_SYSTEM

# Require GMP.
#------------------------------------------------------------------------------
# GMP does not publish a package config.
AC_CHECK_HEADER([gmp.h],
    [],
    [AC_MSG_ERROR([GMP header is required but was not found.])],-)
AC_CHECK_LIB([gmp],[__gmpz_init],
    [AC_SUBST([gmp_LIBS], [-lgmp])],
    [AC_MSG_ERROR([GMP library is required but was not found.])])

# Require secp256k1 package min version.
#------------------------------------------------------------------------------
PKG_CHECK_MODULES([secp256k1], [libsecp256k1 >= 0.0.1])

# Require test dependencies if $with_tests is set.
#------------------------------------------------------------------------------
AS_IF([test x$with_tests != xno], 
    [
        AX_BOOST_UNIT_TEST_FRAMEWORK
        PKG_CHECK_MODULES([crypto], [libcrypto >= 1.0.1])
    ])
AM_CONDITIONAL([WITH_TESTS], [test x$with_tests != xno])
AM_COND_IF([WITH_TESTS], [AC_MSG_NOTICE([WITH_TESTS defined])])

# Require librt if on linux.
#------------------------------------------------------------------------------
AS_CASE([${host_os}], [*linux*],
    AC_CHECK_LIB([rt], [clock_gettime],
        [AC_SUBST([rt_LIBS], [-lrt]) clock_gettime_defined=yes],
        [AC_MSG_ERROR([rt library is required but was not found.])]))
AM_CONDITIONAL([USE_LIBRT], [test x$clock_gettime_defined = xyes])
AM_COND_IF([USE_LIBRT], [AC_MSG_NOTICE([USE_LIBRT defined])])

# Set warning levels.
#------------------------------------------------------------------------------
AX_CHECK_COMPILE_FLAG([-Wall],
    [CXXFLAGS="$CXXFLAGS -Wall"])

AX_CHECK_COMPILE_FLAG([-Wextra],
    [CXXFLAGS="$CXXFLAGS -Wextra"])

AX_CHECK_COMPILE_FLAG([-Wno-missing-braces],
    [CXXFLAGS="$CXXFLAGS -Wno-missing-braces"])

AX_CHECK_COMPILE_FLAG([-Wpedantic],
    [CXXFLAGS="$CXXFLAGS -Wpedantic"],
    AX_CHECK_COMPILE_FLAG([-pedantic],
        [CXXFLAGS="$CXXFLAGS -pedantic"]))

# Set compiler flags.
#------------------------------------------------------------------------------
AX_CHECK_COMPILE_FLAG([-fvisibility-inlines-hidden],
    [CXXFLAGS="$CXXFLAGS -fvisibility-inlines-hidden"])

AX_CHECK_COMPILE_FLAG([-fvisibility=internal],
    [CXXFLAGS="$CXXFLAGS -fvisibility=internal"],
    AX_CHECK_COMPILE_FLAG([-fvisibility=hidden],
        [CXXFLAGS="$CXXFLAGS -fvisibility=hidden"]))

AX_CHECK_COMPILE_FLAG([-fstack-protector],
    [AX_CHECK_LINK_FLAG([-fstack-protector], 
        [CXXFLAGS="$CXXFLAGS -fstack-protector"])])

AX_CHECK_COMPILE_FLAG([-fstack-protector-all],
    [AX_CHECK_LINK_FLAG([-fstack-protector-all],
        [CXXFLAGS="$CXXFLAGS -fstack-protector-all"])])

# Generate output files.
#------------------------------------------------------------------------------
AC_CONFIG_FILES([Makefile libbitcoin.pc])
AC_OUTPUT
