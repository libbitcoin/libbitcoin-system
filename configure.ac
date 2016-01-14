###############################################################################
#  Copyright (c) 2014-2015 libbitcoin developers (see COPYING).
#
#         GENERATED SOURCE CODE, DO NOT EDIT EXCEPT EXPERIMENTALLY
#
###############################################################################

# Standard declarations.
#==============================================================================
# Declare the required version of autoconf.
AC_PREREQ([2.65])

# Process command-line arguments and perform initialization and verification.
AC_INIT([libbitcoin], [2.10.1], [genjix@riseup.net])

# Do compilation tests.
AC_LANG(C++)

# Specify the temporary directory for build tools.
AC_CONFIG_AUX_DIR([build-aux])

# Specify the directory of additional local Autoconf macros.
AC_CONFIG_MACRO_DIR([m4])

# Run macros for operation of generated Makefiles, enable non-recursive make.
# Unless [foreign] is specified standard GNU files will be required,
# specifically: AUTHORS, COPYING, INSTALL, NEWS, README and ChangeLog.
AM_INIT_AUTOMAKE([subdir-objects])

# Enable C and POSIX extensions that may be disabled on certain platforms.
AC_USE_SYSTEM_EXTENSIONS

# Enable the archiver.
AM_PROG_AR

# Initialize libtool.
LT_INIT

# Determine C++ compiler to use.
AC_PROG_CXX

# Enable shared libraries if available, and static if they don't conflict.
AC_PROG_LIBTOOL

# Compute the canonical host-system type variable host, including host_os.
AC_CANONICAL_HOST

# Enable silent rules option.
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])

# Check for pkg-config.
PKG_PROG_PKG_CONFIG

# Declare environment variables that affect the build.
#------------------------------------------------------------------------------
AC_ARG_VAR([CC], "C compiler to use, such as gcc or clang")
AC_ARG_VAR([CXX], "C++ compiler to use, such as g++ or clang++")
AC_ARG_VAR([PKG_CONFIG_PATH], "Additional directories for package discovery.")

# Check for baseline language coverage in the compiler for the C++11 standard.
#------------------------------------------------------------------------------
AX_CXX_COMPILE_STDCXX_11([noext], [mandatory])


# Process options.
#==============================================================================
# Implement --with-pkgconfigdir and output ${pkgconfigdir}.
#------------------------------------------------------------------------------
AC_MSG_CHECKING([--with-pkgconfigdir option])
AC_ARG_WITH([pkgconfigdir],
    AS_HELP_STRING([--with-pkgconfigdir=DIR],
        [Path to pkgconfig directory. @<:@default=${libdir}/pkgconfig@:>@]),
    [pkgconfigdir=$withval],
    [pkgconfigdir=${libdir}/pkgconfig])
AC_MSG_RESULT([$pkgconfigdir])
AC_SUBST([pkgconfigdir])

# Implement --with-tests and declare WITH_TESTS.
#------------------------------------------------------------------------------
AC_MSG_CHECKING([--with-tests option])
AC_ARG_WITH([tests],
    AS_HELP_STRING([--with-tests],
        [Compile with unit tests. @<:@default=yes@:>@]),
    [with_tests=$withval],
    [with_tests=yes])
AC_MSG_RESULT([$with_tests])
AM_CONDITIONAL([WITH_TESTS], [test x$with_tests != xno])

# Implement --with-examples and declare WITH_EXAMPLES.
#------------------------------------------------------------------------------
AC_MSG_CHECKING([--with-examples option])
AC_ARG_WITH([examples],
    AS_HELP_STRING([--with-examples],
        [Compile with examples. @<:@default=yes@:>@]),
    [with_examples=$withval],
    [with_examples=yes])
AC_MSG_RESULT([$with_examples])
AM_CONDITIONAL([WITH_EXAMPLES], [test x$with_examples != xno])

# Implement --with-icu and define BOOST_HAS_ICU and output ${icu}.
#------------------------------------------------------------------------------
AC_MSG_CHECKING([--with-icu option])
AC_ARG_WITH([icu],
    AS_HELP_STRING([--with-icu],
        [Compile with International Components for Unicode. @<:@default=no@:>@]),
    [with_icu=$withval],
    [with_icu=no])
AC_MSG_RESULT([$with_icu])
AS_CASE([${with_icu}], [yes], AC_DEFINE([BOOST_HAS_ICU]))
AS_CASE([${with_icu}], [yes], AC_SUBST([icu], [-DWITH_ICU]))

# Implement --enable-ndebug and define NDEBUG.
#------------------------------------------------------------------------------
AC_MSG_CHECKING([--enable-ndebug option])
AC_ARG_ENABLE([ndebug],
    AS_HELP_STRING([--enable-ndebug],
        [Compile without debug assertions. @<:@default=yes@:>@]),
    [enable_ndebug=$enableval],
    [enable_ndebug=yes])
AC_MSG_RESULT([$enable_ndebug])
AS_CASE([${enable_ndebug}], [yes], AC_DEFINE([NDEBUG]))

# Implement --enable-testnet and define ENABLE_TESTNET and output ${testnet}.
#------------------------------------------------------------------------------
AC_MSG_CHECKING([--enable-testnet option])
AC_ARG_ENABLE([testnet],
    AS_HELP_STRING([--enable-testnet],
        [Compile for testnet (deprecated). @<:@default=no@:>@]),
    [enable_testnet=$enableval],
    [enable_testnet=no])
AC_MSG_RESULT([$enable_testnet])
AS_CASE([${enable_testnet}], [yes], AC_DEFINE([ENABLE_TESTNET]))
AS_CASE([${enable_testnet}], [yes], AC_SUBST([testnet], [-DENABLE_TESTNET]))

# Inherit --enable-shared and define BOOST_TEST_DYN_LINK.
#------------------------------------------------------------------------------
AS_CASE([${enable_shared}], [yes], AC_DEFINE([BOOST_TEST_DYN_LINK]))


# Check dependencies.
#==============================================================================
# Require Boost of at least version 1.55.0 if in gcc and output ${boost_CPPFLAGS/LDFLAGS}.
#------------------------------------------------------------------------------
AS_CASE([${CC}], [*gcc*],
    [AX_BOOST_BASE([1.55.0],
        [AC_SUBST([boost_CPPFLAGS], [${BOOST_CPPFLAGS}])
         AC_SUBST([boost_LDFLAGS], [${BOOST_LDFLAGS}])
         AC_MSG_NOTICE([boost_CPPFLAGS : ${boost_CPPFLAGS}])
         AC_MSG_NOTICE([boost_LDFLAGS : ${boost_LDFLAGS}])],
        [AC_MSG_ERROR([Boost 1.55.0 or later is required but was not found.])])])

# Require Boost of at least version 1.54.0 if in clang and output ${boost_CPPFLAGS/LDFLAGS}.
#------------------------------------------------------------------------------
AS_CASE([${CC}], [*clang*],
    [AX_BOOST_BASE([1.54.0],
        [AC_SUBST([boost_CPPFLAGS], [${BOOST_CPPFLAGS}])
         AC_SUBST([boost_LDFLAGS], [${BOOST_LDFLAGS}])
         AC_MSG_NOTICE([boost_CPPFLAGS : ${boost_CPPFLAGS}])
         AC_MSG_NOTICE([boost_LDFLAGS : ${boost_LDFLAGS}])],
        [AC_MSG_ERROR([Boost 1.54.0 or later is required but was not found.])])])

AX_BOOST_CHRONO
AC_SUBST([boost_chrono_LIBS], [${BOOST_CHRONO_LIB}])
AC_MSG_NOTICE([boost_chrono_LIBS : ${boost_chrono_LIBS}])

AX_BOOST_DATE_TIME
AC_SUBST([boost_date_time_LIBS], [${BOOST_DATE_TIME_LIB}])
AC_MSG_NOTICE([boost_date_time_LIBS : ${boost_date_time_LIBS}])

AX_BOOST_FILESYSTEM
AC_SUBST([boost_filesystem_LIBS], [${BOOST_FILESYSTEM_LIB}])
AC_MSG_NOTICE([boost_filesystem_LIBS : ${boost_filesystem_LIBS}])

AX_BOOST_LOCALE
AC_SUBST([boost_locale_LIBS], [${BOOST_LOCALE_LIB}])
AC_MSG_NOTICE([boost_locale_LIBS : ${boost_locale_LIBS}])

AX_BOOST_PROGRAM_OPTIONS
AC_SUBST([boost_program_options_LIBS], [${BOOST_PROGRAM_OPTIONS_LIB}])
AC_MSG_NOTICE([boost_program_options_LIBS : ${boost_program_options_LIBS}])

AX_BOOST_REGEX
AC_SUBST([boost_regex_LIBS], [${BOOST_REGEX_LIB}])
AC_MSG_NOTICE([boost_regex_LIBS : ${boost_regex_LIBS}])

AX_BOOST_SYSTEM
AC_SUBST([boost_system_LIBS], [${BOOST_SYSTEM_LIB}])
AC_MSG_NOTICE([boost_system_LIBS : ${boost_system_LIBS}])

AX_BOOST_THREAD
AC_SUBST([boost_thread_LIBS], [${BOOST_THREAD_LIB}])
AC_MSG_NOTICE([boost_thread_LIBS : ${boost_thread_LIBS}])

AS_CASE([${with_tests}], [yes],
    [AX_BOOST_UNIT_TEST_FRAMEWORK
     AC_SUBST([boost_unit_test_framework_LIBS], [${BOOST_UNIT_TEST_FRAMEWORK_LIB}])
     AC_MSG_NOTICE([boost_unit_test_framework_LIBS : ${boost_unit_test_framework_LIBS}])],
    [AC_SUBST([boost_unit_test_framework_LIBS], [])])

# Require pthread and output ${pthread_CPPFLAGS/LIBS}.
#------------------------------------------------------------------------------
AX_PTHREAD(
    [AC_SUBST([pthread_CPPFLAGS], [${PTHREAD_CPPFLAGS}])
     AC_SUBST([pthread_LIBS], [${PTHREAD_LIBS}])
     AC_MSG_NOTICE([pthread_CPPFLAGS : ${pthread_CPPFLAGS}])
     AC_MSG_NOTICE([pthread_LIBS : ${pthread_LIBS}])],
    [AC_MSG_ERROR([pthread library is required but was not found.])])
# Hard wired -lpthread because AX_PTHREAD does not do it.
AC_SUBST([pthread_LIBS], [-lpthread])

# Require rt if on linux and output ${rt_LIBS}.
#------------------------------------------------------------------------------
AS_CASE([${host_os}], [*linux*],
    [AC_CHECK_LIB([rt], [clock_gettime],
        [AC_SUBST([rt_LIBS], [-lrt])
         AC_MSG_NOTICE([rt_LIBS : ${rt_LIBS}])], 
        [AC_MSG_ERROR([rt library is required but was not found.])])],
    [AC_SUBST([rt_LIBS], [])])

# Require icu-i18n of at least version 51.2 and output ${icu_i18n_CPPFLAGS/LIBS/PKG}.
#------------------------------------------------------------------------------
AS_CASE([${with_icu}], [yes],
    [PKG_CHECK_MODULES([icu_i18n], [icu-i18n >= 51.2])
     AC_SUBST([icu_i18n_PKG], ['icu-i18n >= 51.2'])
     AC_SUBST([icu_i18n_CPPFLAGS], [${icu_i18n_CFLAGS}])
     AC_MSG_NOTICE([icu_i18n_CPPFLAGS : ${icu_i18n_CPPFLAGS}])
     AC_MSG_NOTICE([icu_i18n_LIBS : ${icu_i18n_LIBS}])],
    [AC_SUBST([icu_i18n_PKG], [])])

# Require dl if on linux and output ${dl_LIBS}.
#------------------------------------------------------------------------------
AS_CASE([${host_os}], [*linux*],
    [AC_CHECK_LIB([dl], [dlopen],
        [AC_SUBST([dl_LIBS], [-ldl])
         AC_MSG_NOTICE([dl_LIBS : ${dl_LIBS}])], 
        [AC_MSG_ERROR([dl library is required but was not found.])])],
    [AC_SUBST([dl_LIBS], [])])

# Require secp256k1 of at least version 0.0.1 and output ${secp256k1_CPPFLAGS/LIBS/PKG}.
#------------------------------------------------------------------------------
PKG_CHECK_MODULES([secp256k1], [libsecp256k1 >= 0.0.1])
AC_SUBST([secp256k1_PKG], ['libsecp256k1 >= 0.0.1'])
AC_SUBST([secp256k1_CPPFLAGS], [${secp256k1_CFLAGS}])
AC_MSG_NOTICE([secp256k1_CPPFLAGS : ${secp256k1_CPPFLAGS}])
AC_MSG_NOTICE([secp256k1_LIBS : ${secp256k1_LIBS}])


# Set flags.
#==============================================================================
# Require c++11 for all c++ products.
#------------------------------------------------------------------------------
AS_CASE([${CC}], [*],
    [AX_CHECK_COMPILE_FLAG([-std=c++11],
        [CXXFLAGS="$CXXFLAGS -std=c++11"])])

# Clean up boost 1.54 headers. Enabled in clang only.
#------------------------------------------------------------------------------
AS_CASE([${CC}], [*clang*],
    [AX_CHECK_COMPILE_FLAG([-Wno-deprecated-register],
        [CXXFLAGS="$CXXFLAGS -Wno-deprecated-register"])])

# Clean up boost 1.55 headers. Enabled in gcc only.
#------------------------------------------------------------------------------
AS_CASE([${CC}], [*gcc*],
    [AX_CHECK_COMPILE_FLAG([-Wno-deprecated-declarations],
        [CXXFLAGS="$CXXFLAGS -Wno-deprecated-declarations"])])

# Warn on all stuff.
#------------------------------------------------------------------------------
AS_CASE([${CC}], [*],
    [AX_CHECK_COMPILE_FLAG([-Wall],
        [CFLAGS="$CFLAGS -Wall"])])

# Warn on extra stuff.
#------------------------------------------------------------------------------
AS_CASE([${CC}], [*],
    [AX_CHECK_COMPILE_FLAG([-Wextra],
        [CFLAGS="$CFLAGS -Wextra"])])

# Be really annoying.
#------------------------------------------------------------------------------
AS_CASE([${CC}], [*],
    [AX_CHECK_COMPILE_FLAG([-Wpedantic],
        [CFLAGS="$CFLAGS -Wpedantic"],
        [AX_CHECK_COMPILE_FLAG([-pedantic],
            [CFLAGS="$CFLAGS -pedantic"])])])

# Allow use of C99 'long long' type.
#------------------------------------------------------------------------------
AS_CASE([${CC}], [*],
    [AX_CHECK_COMPILE_FLAG([-Wno-long-long],
        [CFLAGS="$CFLAGS -Wno-long-long"],
        [AX_CHECK_COMPILE_FLAG([-pedantic],
            [CFLAGS="$CFLAGS -pedantic"])])])

# Conform to style.
#------------------------------------------------------------------------------
AS_CASE([${CC}], [*],
    [AX_CHECK_COMPILE_FLAG([-Wno-missing-braces],
        [CXXFLAGS="$CXXFLAGS -Wno-missing-braces"])])

# Conflict in stdlib under clang. Enabled in clang only.
#------------------------------------------------------------------------------
AS_CASE([${CC}], [*clang*],
    [AX_CHECK_COMPILE_FLAG([-Wno-mismatched-tags],
        [CXXFLAGS="$CXXFLAGS -Wno-mismatched-tags"])])

# Protect stack.
#------------------------------------------------------------------------------
AS_CASE([${CC}], [*],
    [AX_CHECK_LINK_FLAG([-fstack-protector],
        [LDFLAGS="$LDFLAGS -fstack-protector"])])

# Protect stack comprehensively.
#------------------------------------------------------------------------------
AS_CASE([${CC}], [*],
    [AX_CHECK_LINK_FLAG([-fstack-protector-all],
        [LDFLAGS="$LDFLAGS -fstack-protector-all"])])

# Hide internal functions from external libs. Enabled in gcc only.
#------------------------------------------------------------------------------
AS_CASE([${CC}], [*gcc*],
    [AX_CHECK_COMPILE_FLAG([-fvisibility-hidden],
        [CXXFLAGS="$CXXFLAGS -fvisibility-hidden"])])

# Hide inlines from external libs. Enabled in gcc only.
#------------------------------------------------------------------------------
AS_CASE([${CC}], [*gcc*],
    [AX_CHECK_COMPILE_FLAG([-fvisibility-inlines-hidden],
        [CXXFLAGS="$CXXFLAGS -fvisibility-inlines-hidden"])])

# Limit delays and warnings. Enabled in gcc only.
#------------------------------------------------------------------------------
AS_CASE([${CC}], [*gcc*],
    [AX_CHECK_COMPILE_FLAG([-fno-var-tracking-assignments],
        [CXXFLAGS="$CXXFLAGS -fno-var-tracking-assignments"])])


# Process outputs into templates.
#==============================================================================
AC_CONFIG_FILES([Makefile libbitcoin.pc])
AC_OUTPUT
