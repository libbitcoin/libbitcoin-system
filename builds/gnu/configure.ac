###############################################################################
#  Copyright (c) 2014-2026 libbitcoin-system developers (see COPYING).
#
#         GENERATED SOURCE CODE, DO NOT EDIT EXCEPT EXPERIMENTALLY
#
###############################################################################

# Standard declarations.
#==============================================================================
# Requires Automake 1.14 or newer.

# Declare the required version of Autoconf.
AC_PREREQ([2.65])

# Process command-line arguments and perform initialization and verification.
AC_INIT([libbitcoin-system], [4.0.0], [eric@voskuil.org])

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
LT_PREREQ(2.4.2)

# Enable shared libraries if available, and static if they don't conflict.
LT_INIT
AC_SUBST([LIBTOOL_DEPS])

# Determine C++ compiler to use.
AC_PROG_CXX

# Enable sed for substitution.
AC_PROG_SED

# Compute the canonical host-system type variable host, including host_os.
AC_CANONICAL_HOST

# Enable silent rules option.
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])

# Check for pkg-config.
PKG_PROG_PKG_CONFIG
AS_IF([test -n "${PKG_CONFIG}"], [],
    [AC_MSG_ERROR([pkg-config is required but was not found.])])

AS_IF([test "x${enable_static}" != "xno"],
    [AC_SUBST([PKG_CONFIG], ["${PKG_CONFIG} --static"])])

# Declare environment variables that affect the build.
#------------------------------------------------------------------------------
AC_ARG_VAR([CC], "C compiler to use, such as gcc or clang")
AC_ARG_VAR([CXX], "C++ compiler to use, such as g++ or clang++")
AC_ARG_VAR([PKG_CONFIG_PATH], "Additional directories for package discovery.")

repository_root_dir="@abs_top_srcdir@/../.."

# Check for baseline language coverage in the compiler for the C++20 standard.
#------------------------------------------------------------------------------
AX_CXX_COMPILE_STDCXX([20], [noext], [mandatory])

# Process options.
#==============================================================================
AC_MSG_CHECKING([--enable-isystem option])
AC_ARG_ENABLE([isystem],
    AS_HELP_STRING([--enable-isystem],
        [Substitute -isystem for -I in dependencies. @<:@default=no@:>@]),
    [enable_isystem=$enableval],
    [enable_isystem=no])
AC_MSG_RESULT([$enable_isystem])
AM_CONDITIONAL([ENABLE_ISYSTEM], [test "x${enable_isystem}" != "xno"])

AC_MSG_CHECKING([--with-pkgconfigdir option])
AC_ARG_WITH([pkgconfigdir],
    AS_HELP_STRING([--with-pkgconfigdir=DIR],
        [Path to pkgconfig directory. @<:@default=${libdir}/pkgconfig@:>@]),
    [with_pkgconfigdir=$withval],
    [with_pkgconfigdir=${libdir}/pkgconfig])
AC_MSG_RESULT([$with_pkgconfigdir])
AC_SUBST([pkgconfigdir],[${with_pkgconfigdir}])

AC_MSG_CHECKING([--enable-ndebug option])
AC_ARG_ENABLE([ndebug],
    AS_HELP_STRING([--enable-ndebug],
        [Compile with NDEBUG assertion. @<:@default=no@:>@]),
    [enable_ndebug=$enableval],
    [enable_ndebug=no])
AC_MSG_RESULT([$enable_ndebug])
AM_CONDITIONAL([ENABLE_NDEBUG], [test "x${enable_ndebug}" != "xno"])
AS_IF([test "x${enable_ndebug}" != "xno"], [AC_DEFINE([NDEBUG])])

AS_IF([test "x${enable_shared}" != "xno"], [AC_DEFINE([BOOST_ALL_DYN_LINK])])

AC_MSG_CHECKING([--with-tests option])
AC_ARG_WITH([tests],
    AS_HELP_STRING([--with-tests],
        [Compile with unit tests. @<:@default=yes@:>@]),
    [with_tests=$withval],
    [with_tests=yes])
AC_MSG_RESULT([$with_tests])
AM_CONDITIONAL([WITH_TESTS], [test "x${with_tests}" != "xno"])

AC_MSG_CHECKING([--with-examples option])
AC_ARG_WITH([examples],
    AS_HELP_STRING([--with-examples],
        [Compile with examples. @<:@default=yes@:>@]),
    [with_examples=$withval],
    [with_examples=yes])
AC_MSG_RESULT([$with_examples])
AM_CONDITIONAL([WITH_EXAMPLES], [test "x${with_examples}" != "xno"])

AC_MSG_CHECKING([--enable-avx2 option])
AC_ARG_ENABLE([avx2],
    AS_HELP_STRING([--enable-avx2],
        [Use Intel AVX2 intrinsics. @<:@default=no@:>@]),
    [enable_avx2=$enableval],
    [enable_avx2=no])
AC_MSG_RESULT([$enable_avx2])
AM_CONDITIONAL([ENABLE_AVX2], [test "x${enable_avx2}" != "xno"])

AC_MSG_CHECKING([--enable-avx512 option])
AC_ARG_ENABLE([avx512],
    AS_HELP_STRING([--enable-avx512],
        [Use Intel AVX512 intrinsics. @<:@default=no@:>@]),
    [enable_avx512=$enableval],
    [enable_avx512=no])
AC_MSG_RESULT([$enable_avx512])
AM_CONDITIONAL([ENABLE_AVX512], [test "x${enable_avx512}" != "xno"])

AC_MSG_CHECKING([--enable-sse41 option])
AC_ARG_ENABLE([sse41],
    AS_HELP_STRING([--enable-sse41],
        [Use SSE4.1 hardware instructions. @<:@default=no@:>@]),
    [enable_sse41=$enableval],
    [enable_sse41=no])
AC_MSG_RESULT([$enable_sse41])
AM_CONDITIONAL([ENABLE_SSE41], [test "x${enable_sse41}" != "xno"])

AC_MSG_CHECKING([--enable-shani option])
AC_ARG_ENABLE([shani],
    AS_HELP_STRING([--enable-shani],
        [Use Intel/ARM SHA Extensions. @<:@default=no@:>@]),
    [enable_shani=$enableval],
    [enable_shani=no])
AC_MSG_RESULT([$enable_shani])
AM_CONDITIONAL([ENABLE_SHANI], [test "x${enable_shani}" != "xno"])

AC_MSG_CHECKING([--with-ultrafast option])
AC_ARG_WITH([ultrafast],
    AS_HELP_STRING([--with-ultrafast],
        [Use shrec/UltrafastSecp256k1 library. @<:@default=no@:>@]),
    [with_ultrafast=$withval],
    [with_ultrafast=no])
AC_MSG_RESULT([$with_ultrafast])
AM_CONDITIONAL([WITH_ULTRAFAST], [test "x${with_ultrafast}" != "xno"])

AC_MSG_CHECKING([--with-secp256k1 option])
AC_ARG_WITH([secp256k1],
    AS_HELP_STRING([--with-secp256k1],
        [Use bitcoin-core/secp256k1 library. @<:@default=yes@:>@]),
    [with_secp256k1=$withval],
    [with_secp256k1=yes])
AC_MSG_RESULT([$with_secp256k1])
AM_CONDITIONAL([WITH_SECP256K1], [test "x${with_secp256k1}" != "xno"])

# Set flags.
#==============================================================================
AS_IF([test "x${enable_avx2}" != "xno"],
  [
    AX_CHECK_COMPILE_FLAG([-mavx -mavx2],
      [
        CFLAGS="${CFLAGS} -mavx -mavx2";
        CXXFLAGS="${CXXFLAGS} -mavx -mavx2";
        AC_SUBST([avx2], ["-mavx -mavx2"])
      ],[
        AC_MSG_ERROR([-mavx -mavx2 not supported.])
      ],
      [],
      [
        AC_LANG_PROGRAM(
          [[
            #include <stdint.h>
            #include <immintrin.h>
          ]],
          [[
            __m256i a = _mm256_set_epi32(8, 7, 6, 5, 4, 3, 2, 1);
            return _mm256_extract_epi32(_mm256_slli_epi64(a, 2), 5);
          ]])
      ])
  ])

AS_IF([test "x${enable_avx512}" != "xno"],
  [
    AX_CHECK_COMPILE_FLAG([-mavx512f -mavx512bw],
      [
        CFLAGS="${CFLAGS} -mavx512f -mavx512bw";
        CXXFLAGS="${CXXFLAGS} -mavx512f -mavx512bw";
        AC_SUBST([avx512], ["-mavx512f -mavx512bw"])
      ],[
        AC_MSG_ERROR([-mavx512f -mavx512bw not supported.])
      ],
      [],
      [
        AC_LANG_PROGRAM(
          [[
            #include <stdint.h>
            #include <immintrin.h>
          ]],
          [[
            __m512i a = _mm512_set_epi32(16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1);
            return _mm_extract_epi32(_mm512_extracti32x4_epi32(_mm512_slli_epi64(a, 2), 2), 1);
          ]])
      ])
  ])

AS_IF([test "x${enable_shani}" != "xno"],
  [
    AX_CHECK_COMPILE_FLAG([-msse4 -msha],
      [
        CFLAGS="${CFLAGS} -msse4 -msha";
        CXXFLAGS="${CXXFLAGS} -msse4 -msha";
        AC_SUBST([shani], ["-msse4 -msha"])
      ],[
        AC_MSG_ERROR([-msse4 -msha not supported.])
      ],
      [],
      [
        AC_LANG_PROGRAM(
          [[
            #include <stdint.h>
            #include <immintrin.h>
          ]],
          [[
            __m128i a = _mm_set1_epi32(0);
            __m128i b = _mm_set1_epi32(15);
            __m128i k = _mm_set1_epi32(31);
            return _mm_extract_epi32(_mm_sha256msg2_epu32(_mm_sha256msg1_epu32(_mm_sha256rnds2_epu32(a, b, k), b), a), 2);
          ]])
      ])
  ])

AS_IF([test "x${enable_sse41}" != "xno"],
  [
    AX_CHECK_COMPILE_FLAG([-msse4.1],
      [
        CFLAGS="${CFLAGS} -msse4.1";
        CXXFLAGS="${CXXFLAGS} -msse4.1";
        AC_SUBST([sse41], ["-msse4.1"])
      ],[
        AC_MSG_ERROR([-msse4.1 not supported.])
      ],
      [],
      [
        AC_LANG_PROGRAM(
          [[
            #include <stdint.h>
            #include <immintrin.h>
          ]],
          [[
            __m128i a = _mm_set1_epi32(0);
            __m128i b = _mm_set1_epi32(15);
            return _mm_extract_epi32(_mm_add_epi64(a, b), 2);
          ]])
      ])
  ])

AX_CHECK_COMPILE_FLAG([-Wall],
  [
    CFLAGS="${CFLAGS} -Wall";
    CXXFLAGS="${CXXFLAGS} -Wall";
  ])

AX_CHECK_COMPILE_FLAG([-Wextra],
  [
    CFLAGS="${CFLAGS} -Wextra";
    CXXFLAGS="${CXXFLAGS} -Wextra";
  ])

AX_CHECK_COMPILE_FLAG([-Wno-reorder],
  [
    CXXFLAGS="${CXXFLAGS} -Wno-reorder";
  ])

AX_CHECK_COMPILE_FLAG([-Wno-missing-field-initializers],
  [
    CXXFLAGS="${CXXFLAGS} -Wno-missing-field-initializers";
  ])

AX_CHECK_COMPILE_FLAG([-Wno-missing-braces],
  [
    CXXFLAGS="${CXXFLAGS} -Wno-missing-braces";
  ])

AX_CHECK_COMPILE_FLAG([-Wno-comment],
  [
    CXXFLAGS="${CXXFLAGS} -Wno-comment";
  ])

AX_CHECK_COMPILE_FLAG([-Wno-deprecated-copy],
  [
    CXXFLAGS="${CXXFLAGS} -Wno-deprecated-copy";
  ])

AX_CHECK_COMPILE_FLAG([-Wno-ignored-attributes],
  [
    CXXFLAGS="${CXXFLAGS} -Wno-ignored-attributes";
  ])

AS_CASE([${CC}], [*clang*],
  [
    AX_CHECK_COMPILE_FLAG([-Wno-mismatched-tags],
      [
        CXXFLAGS="${CXXFLAGS} -Wno-mismatched-tags";
      ])
  ])

AX_CHECK_COMPILE_FLAG([-Wno-long-long],
  [
    CXXFLAGS="${CXXFLAGS} -Wno-long-long";
  ])

AS_CASE([${CC}], [*gnu*],
  [
    AX_CHECK_COMPILE_FLAG([-fno-var-tracking-assignments],
      [
        CXXFLAGS="${CXXFLAGS} -fno-var-tracking-assignments";
      ])
  ])

AX_CHECK_COMPILE_FLAG([-fstack-protector-all],
  [
    CXXFLAGS="${CXXFLAGS} -fstack-protector-all";
  ])

# Check dependencies.
#==============================================================================
AX_BOOST_BASE([1.86.0],
  [
    AC_SUBST([boost_CPPFLAGS], [${BOOST_CPPFLAGS}])
    AC_SUBST([boost_ISYSTEM_CPPFLAGS], [`echo ${boost_CPPFLAGS} | ${SED} s/^-I/-isystem/g | ${SED} s/' -I'/' -isystem'/g`])
    AC_SUBST([boost_LDFLAGS], [${BOOST_LDFLAGS}])
    AC_MSG_NOTICE([boost_CPPFLAGS : ${boost_CPPFLAGS}])
    AC_MSG_NOTICE([boost_ISYS_CPPFLAGS : ${boost_ISYS_CPPFLAGS}])
    AC_MSG_NOTICE([boost_LDFLAGS : ${boost_LDFLAGS}])
  ],
  [
    AC_MSG_ERROR([Boost 1.86.0 or later is required but was not found.])
  ])

AS_IF([test "x${enable_isystem}" != "xno"],
  [ AC_SUBST([boost_BUILD_CPPFLAGS], [${boost_ISYSTEM_CPPFLAGS}]) ],
  [ AC_SUBST([boost_BUILD_CPPFLAGS], [${boost_CPPFLAGS}]) ])

AC_MSG_NOTICE([boost_BUILD_CPPFLAGS : ${boost_BUILD_CPPFLAGS}])

AX_BOOST_CONTAINER
AC_SUBST([boost_container_LIBS], [${BOOST_CONTAINER_LIB}])
AC_MSG_NOTICE([boost_container_LIBS : ${boost_container_LIBS}])

AX_BOOST_IOSTREAMS
AC_SUBST([boost_iostreams_LIBS], [${BOOST_IOSTREAMS_LIB}])
AC_MSG_NOTICE([boost_iostreams_LIBS : ${boost_iostreams_LIBS}])

AX_BOOST_JSON
AC_SUBST([boost_json_LIBS], [${BOOST_JSON_LIB}])
AC_MSG_NOTICE([boost_json_LIBS : ${boost_json_LIBS}])

AX_BOOST_LOCALE
AC_SUBST([boost_locale_LIBS], [${BOOST_LOCALE_LIB}])
AC_MSG_NOTICE([boost_locale_LIBS : ${boost_locale_LIBS}])

AX_BOOST_PROGRAM_OPTIONS
AC_SUBST([boost_program_options_LIBS], [${BOOST_PROGRAM_OPTIONS_LIB}])
AC_MSG_NOTICE([boost_program_options_LIBS : ${boost_program_options_LIBS}])

AX_BOOST_THREAD
AC_SUBST([boost_thread_LIBS], [${BOOST_THREAD_LIB}])
AC_MSG_NOTICE([boost_thread_LIBS : ${boost_thread_LIBS}])

AX_BOOST_URL
AC_SUBST([boost_url_LIBS], [${BOOST_URL_LIB}])
AC_MSG_NOTICE([boost_url_LIBS : ${boost_url_LIBS}])

AS_IF([(test "x${with_tests}" != "xno")],
  [
    AX_BOOST_UNIT_TEST_FRAMEWORK
    AC_SUBST([boost_unit_test_framework_LIBS], [${BOOST_UNIT_TEST_FRAMEWORK_LIB}])
    AC_MSG_NOTICE([boost_unit_test_framework_LIBS : ${boost_unit_test_framework_LIBS}])
  ])

AS_IF([test "x${with_secp256k1}" != "xno"],
  [
    PKG_CHECK_MODULES(
      [secp256k1],
      [libsecp256k1 >= 0.7.0],
      [],
      [
        AC_MSG_ERROR([libsecp256k1 >= 0.7.0 is required but was not found.])
      ])
    AC_SUBST([secp256k1_PKG], ['libsecp256k1 >= 0.7.0'])
    AC_SUBST([secp256k1_CPPFLAGS], [${secp256k1_CFLAGS}])
    AC_SUBST([secp256k1_ISYS_CPPFLAGS], [`echo ${secp256k1_CPPFLAGS} | ${SED} s/^-I/-isystem/g | ${SED} s/' -I'/' -isystem'/g`])
    AC_MSG_NOTICE([secp256k1_CPPFLAGS : ${secp256k1_CPPFLAGS}])
    AC_MSG_NOTICE([secp256k1_ISYS_CPPFLAGS : ${secp256k1_ISYS_CPPFLAGS}])
    AC_MSG_NOTICE([secp256k1_OTHER_CFLAGS : ${secp256k1_OTHER_CFLAGS}])
    AC_MSG_NOTICE([secp256k1_INCLUDEDIR : ${secp256k1_INCLUDEDIR}])
    AC_MSG_NOTICE([secp256k1_LIBS : ${secp256k1_LIBS}])
    AS_IF([test "x${enable_isystem}" != "xno"],
      [AC_SUBST([secp256k1_BUILD_CPPFLAGS], [${secp256k1_ISYS_CPPFLAGS}])],
      [AC_SUBST([secp256k1_BUILD_CPPFLAGS], [${secp256k1_CPPFLAGS}])])
    AC_MSG_NOTICE([secp256k1_BUILD_CPPFLAGS : ${secp256k1_BUILD_CPPFLAGS}])
  ])

AS_IF([test "x${with_ultrafast}" != "xno"],
  [
    PKG_CHECK_MODULES(
      [UltrafastSecp256k1],
      [secp256k1-fast >= 4.0.0.0],
      [],
      [
        AC_MSG_ERROR([secp256k1-fast >= 4.0.0.0 is required but was not found.])
      ])
    AC_SUBST([UltrafastSecp256k1_PKG], ['secp256k1-fast >= 4.0.0.0'])
    AC_SUBST([UltrafastSecp256k1_CPPFLAGS], [${UltrafastSecp256k1_CFLAGS}])
    AC_SUBST([UltrafastSecp256k1_ISYS_CPPFLAGS], [`echo ${UltrafastSecp256k1_CPPFLAGS} | ${SED} s/^-I/-isystem/g | ${SED} s/' -I'/' -isystem'/g`])
    AC_MSG_NOTICE([UltrafastSecp256k1_CPPFLAGS : ${UltrafastSecp256k1_CPPFLAGS}])
    AC_MSG_NOTICE([UltrafastSecp256k1_ISYS_CPPFLAGS : ${UltrafastSecp256k1_ISYS_CPPFLAGS}])
    AC_MSG_NOTICE([UltrafastSecp256k1_OTHER_CFLAGS : ${UltrafastSecp256k1_OTHER_CFLAGS}])
    AC_MSG_NOTICE([UltrafastSecp256k1_INCLUDEDIR : ${UltrafastSecp256k1_INCLUDEDIR}])
    AC_MSG_NOTICE([UltrafastSecp256k1_LIBS : ${UltrafastSecp256k1_LIBS}])
    AS_IF([test "x${enable_isystem}" != "xno"],
      [AC_SUBST([UltrafastSecp256k1_BUILD_CPPFLAGS], [${UltrafastSecp256k1_ISYS_CPPFLAGS}])],
      [AC_SUBST([UltrafastSecp256k1_BUILD_CPPFLAGS], [${UltrafastSecp256k1_CPPFLAGS}])])
    AC_MSG_NOTICE([UltrafastSecp256k1_BUILD_CPPFLAGS : ${UltrafastSecp256k1_BUILD_CPPFLAGS}])
  ])

AC_CONFIG_FILES([
    Makefile
    libbitcoin-system.pc
  ])

AC_OUTPUT
