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

m4_define([_GIT_COMMIT_HASH],
    m4_dquote(m4_esyscmd_s([git rev-parse HEAD 2>/dev/null || echo unknown])))

m4_define([_GIT_COMMIT_HEIGHT],
    m4_esyscmd_s([git rev-list --count HEAD 2>/dev/null || echo 0]))

m4_define([_git_dirty],
    m4_esyscmd_s([if git status --porcelain 2>/dev/null | grep -q . ; then echo 1; else echo 0; fi]))

m4_define([_GIT_DIRTY], m4_ifnblank(_git_dirty, [_git_dirty], [0]))

m4_define([_GIT_ORIGIN_URL],
    m4_dquote(m4_esyscmd_s([git ls-remote --get-url 2>/dev/null | grep -e '://' -e '@' || echo unknown])))

m4_define([_LIBBITCOIN_VERSION_MAJOR], [4])
m4_define([_LIBBITCOIN_VERSION_MINOR], [0])
m4_define([_LIBBITCOIN_VERSION_PATCH], [_GIT_COMMIT_HEIGHT])

m4_define([_LIBBITCOIN_VERSION],
    [_LIBBITCOIN_VERSION_MAJOR._LIBBITCOIN_VERSION_MINOR._LIBBITCOIN_VERSION_PATCH])

# Process command-line arguments and perform initialization and verification.
AC_INIT([libbitcoin-system],
    [_LIBBITCOIN_VERSION],
    [eric@voskuil.org])

AC_SUBST([GIT_COMMIT_HASH], [_GIT_COMMIT_HASH])
AC_MSG_NOTICE([GIT_COMMIT_HASH : ${GIT_COMMIT_HASH}])
AC_SUBST([GIT_COMMIT_HEIGHT], [_GIT_COMMIT_HEIGHT])
AC_MSG_NOTICE([GIT_COMMIT_HEIGHT : ${GIT_COMMIT_HEIGHT}])
AC_SUBST([GIT_DIRTY], [_GIT_DIRTY])
AC_MSG_NOTICE([GIT_DIRTY : ${GIT_DIRTY}])
AC_SUBST([GIT_ORIGIN_URL], [_GIT_ORIGIN_URL])
AC_MSG_NOTICE([GIT_ORIGIN_URL : ${GIT_ORIGIN_URL}])

AC_SUBST([LIBBITCOIN_VERSION_MAJOR], [_LIBBITCOIN_VERSION_MAJOR])
AC_MSG_NOTICE([LIBBITCOIN_VERSION_MAJOR : ${LIBBITCOIN_VERSION_MAJOR}])
AC_SUBST([LIBBITCOIN_VERSION_MINOR], [_LIBBITCOIN_VERSION_MINOR])
AC_MSG_NOTICE([LIBBITCOIN_VERSION_MINOR : ${LIBBITCOIN_VERSION_MINOR}])
AC_SUBST([LIBBITCOIN_VERSION_PATCH], [_LIBBITCOIN_VERSION_PATCH])
AC_MSG_NOTICE([LIBBITCOIN_VERSION_PATCH : ${LIBBITCOIN_VERSION_PATCH}])

# Specify the temporary directory for build tools.
AC_CONFIG_AUX_DIR([build-aux])

# Specify the directory of additional local Autoconf macros.
AC_CONFIG_MACRO_DIR([m4])

# Compute the canonical host-system type variable host, including host_os.
AC_CANONICAL_BUILD
AC_CANONICAL_HOST

# Run macros for operation of generated Makefiles, enable non-recursive make.
# Unless [foreign] is specified standard GNU files will be required,
# specifically: AUTHORS, COPYING, INSTALL, NEWS, README and ChangeLog.
AM_INIT_AUTOMAKE([subdir-objects])

# Do compilation tests.
AC_LANG(C++)

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

repository_root_dir=`cd "${srcdir}/../.." && pwd`

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
        [Compile with NDEBUG (no debug assertions). @<:@default=yes@:>@]),
    [enable_ndebug=$enableval],
    [enable_ndebug=yes])
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

AC_MSG_CHECKING([--enable-shani option])
AC_ARG_ENABLE([shani],
    AS_HELP_STRING([--enable-shani],
        [Use Intel SHA Extensions. @<:@default=no@:>@]),
    [enable_shani=$enableval],
    [enable_shani=no])
AC_MSG_RESULT([$enable_shani])
AM_CONDITIONAL([ENABLE_SHANI], [test "x${enable_shani}" != "xno"])

AC_MSG_CHECKING([--enable-sha512 option])
AC_ARG_ENABLE([sha512],
    AS_HELP_STRING([--enable-sha512],
        [Use Intel SHA512 Extensions (implies AVX2). @<:@default=no@:>@]),
    [enable_sha512=$enableval],
    [enable_sha512=no])
AC_MSG_RESULT([$enable_sha512])
AM_CONDITIONAL([ENABLE_SHA512], [test "x${enable_sha512}" != "xno"])

AC_MSG_CHECKING([--enable-aesni option])
AC_ARG_ENABLE([aesni],
    AS_HELP_STRING([--enable-aesni],
        [Use Intel AES-NI and PCLMULQDQ intrinsics. @<:@default=no@:>@]),
    [enable_aesni=$enableval],
    [enable_aesni=no])
AC_MSG_RESULT([$enable_aesni])
AM_CONDITIONAL([ENABLE_AESNI], [test "x${enable_aesni}" != "xno"])

AC_MSG_CHECKING([--enable-vaes option])
AC_ARG_ENABLE([vaes],
    AS_HELP_STRING([--enable-vaes],
        [Use Intel VAES and VPCLMULQDQ intrinsics (implies AES-NI and AVX2). @<:@default=no@:>@]),
    [enable_vaes=$enableval],
    [enable_vaes=no])
AC_MSG_RESULT([$enable_vaes])
AM_CONDITIONAL([ENABLE_VAES], [test "x${enable_vaes}" != "xno"])

AC_MSG_CHECKING([--enable-avx512ifma option])
AC_ARG_ENABLE([avx512ifma],
    AS_HELP_STRING([--enable-avx512ifma],
        [Use Intel AVX512 IFMA intrinsics (implies AVX512VL). @<:@default=no@:>@]),
    [enable_avx512ifma=$enableval],
    [enable_avx512ifma=no])
AC_MSG_RESULT([$enable_avx512ifma])
AM_CONDITIONAL([ENABLE_AVX512IFMA], [test "x${enable_avx512ifma}" != "xno"])

AC_MSG_CHECKING([--enable-avx512 option])
AC_ARG_ENABLE([avx512],
    AS_HELP_STRING([--enable-avx512],
        [Use Intel AVX512 intrinsics. @<:@default=no@:>@]),
    [enable_avx512=$enableval],
    [enable_avx512=no])
AC_MSG_RESULT([$enable_avx512])
AM_CONDITIONAL([ENABLE_AVX512], [test "x${enable_avx512}" != "xno"])

AC_MSG_CHECKING([--enable-avxifma option])
AC_ARG_ENABLE([avxifma],
    AS_HELP_STRING([--enable-avxifma],
        [Use Intel AVX IFMA intrinsics (implies AVX2). @<:@default=no@:>@]),
    [enable_avxifma=$enableval],
    [enable_avxifma=no])
AC_MSG_RESULT([$enable_avxifma])
AM_CONDITIONAL([ENABLE_AVXIFMA], [test "x${enable_avxifma}" != "xno"])

AC_MSG_CHECKING([--enable-avx2 option])
AC_ARG_ENABLE([avx2],
    AS_HELP_STRING([--enable-avx2],
        [Use Intel AVX2 intrinsics. @<:@default=no@:>@]),
    [enable_avx2=$enableval],
    [enable_avx2=no])
AC_MSG_RESULT([$enable_avx2])
AM_CONDITIONAL([ENABLE_AVX2], [test "x${enable_avx2}" != "xno"])

AC_MSG_CHECKING([--enable-sse41 option])
AC_ARG_ENABLE([sse41],
    AS_HELP_STRING([--enable-sse41],
        [Use SSE4.1 hardware instructions. @<:@default=no@:>@]),
    [enable_sse41=$enableval],
    [enable_sse41=no])
AC_MSG_RESULT([$enable_sse41])
AM_CONDITIONAL([ENABLE_SSE41], [test "x${enable_sse41}" != "xno"])

AC_MSG_CHECKING([--enable-sha option])
AC_ARG_ENABLE([sha],
    AS_HELP_STRING([--enable-sha],
        [Use ARM SHA1/SHA256 Extensions. @<:@default=no@:>@]),
    [enable_sha=$enableval],
    [enable_sha=no])
AC_MSG_RESULT([$enable_sha])
AM_CONDITIONAL([ENABLE_SHA], [test "x${enable_sha}" != "xno"])

AC_MSG_CHECKING([--enable-sha3 option])
AC_ARG_ENABLE([sha3],
    AS_HELP_STRING([--enable-sha3],
        [Use ARM SHA3 Extensions for SHA512. @<:@default=no@:>@]),
    [enable_sha3=$enableval],
    [enable_sha3=no])
AC_MSG_RESULT([$enable_sha3])
AM_CONDITIONAL([ENABLE_SHA3], [test "x${enable_sha3}" != "xno"])

AC_MSG_CHECKING([--enable-aes option])
AC_ARG_ENABLE([aes],
    AS_HELP_STRING([--enable-aes],
        [Use ARM AES and PMULL Extensions. @<:@default=no@:>@]),
    [enable_aes=$enableval],
    [enable_aes=no])
AC_MSG_RESULT([$enable_aes])
AM_CONDITIONAL([ENABLE_AES], [test "x${enable_aes}" != "xno"])

AC_MSG_CHECKING([--with-secp256k1 option])
AC_ARG_WITH([secp256k1],
    AS_HELP_STRING([--with-secp256k1],
        [Use bitcoin-core/secp256k1 library. @<:@default=no@:>@]),
    [with_secp256k1=$withval],
    [with_secp256k1=no])
AC_MSG_RESULT([$with_secp256k1])
AM_CONDITIONAL([WITH_SECP256K1], [test "x${with_secp256k1}" != "xno"])

AC_MSG_CHECKING([--with-valgrind option])
AC_ARG_WITH([valgrind],
    AS_HELP_STRING([--with-valgrind],
        [Use valgrind memcheck client requests. @<:@default=no@:>@]),
    [with_valgrind=$withval],
    [with_valgrind=no])
AC_MSG_RESULT([$with_valgrind])
AM_CONDITIONAL([WITH_VALGRIND], [test "x${with_valgrind}" != "xno"])

AC_MSG_CHECKING([--enable-cuda option])
AC_ARG_ENABLE([cuda],
    AS_HELP_STRING([--enable-cuda],
        [Use cuda (gpu) batch signature verification (inert on macOS). @<:@default=yes@:>@]),
    [enable_cuda=$enableval],
    [enable_cuda=yes])
AC_MSG_RESULT([$enable_cuda])
AM_CONDITIONAL([ENABLE_CUDA], [test "x${enable_cuda}" != "xno"])

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
    AX_CHECK_COMPILE_FLAG([-mavx512f -mavx512bw -mavx512vl],
      [
        CFLAGS="${CFLAGS} -mavx512f -mavx512bw -mavx512vl";
        CXXFLAGS="${CXXFLAGS} -mavx512f -mavx512bw -mavx512vl";
        AC_SUBST([avx512], ["-mavx512f -mavx512bw -mavx512vl"])
      ],[
        AC_MSG_ERROR([-mavx512f -mavx512bw -mavx512vl not supported.])
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
            __m256i b = _mm256_set1_epi32(3);
            return _mm_extract_epi32(_mm512_extracti32x4_epi32(_mm512_slli_epi64(a, 2), 2), 1) + _mm256_extract_epi32(_mm256_ror_epi32(b, 1), 2);
          ]])
      ])
  ])

AS_IF([test "x${enable_avx512ifma}" != "xno"],
  [
    AX_CHECK_COMPILE_FLAG([-mavx512ifma -mavx512vl],
      [
        CFLAGS="${CFLAGS} -mavx512ifma -mavx512vl";
        CXXFLAGS="${CXXFLAGS} -mavx512ifma -mavx512vl";
        AC_SUBST([avx512ifma], ["-mavx512ifma -mavx512vl"])
      ],[
        AC_MSG_ERROR([-mavx512ifma -mavx512vl not supported.])
      ],
      [],
      [
        AC_LANG_PROGRAM(
          [[
            #include <stdint.h>
            #include <immintrin.h>
          ]],
          [[
            __m512i a = _mm512_set1_epi64(3);
            __m256i b = _mm256_set1_epi64x(5);
            return _mm_extract_epi32(_mm512_extracti32x4_epi32(_mm512_madd52lo_epu64(a, a, a), 1), 0) + _mm256_extract_epi32(_mm256_madd52hi_epu64(b, b, b), 2);
          ]])
      ])
  ])

AS_IF([test "x${enable_avxifma}" != "xno"],
  [
    AX_CHECK_COMPILE_FLAG([-mavxifma],
      [
        CFLAGS="${CFLAGS} -mavxifma";
        CXXFLAGS="${CXXFLAGS} -mavxifma";
        AC_SUBST([avxifma], ["-mavxifma"])
      ],[
        AC_MSG_ERROR([-mavxifma not supported.])
      ],
      [],
      [
        AC_LANG_PROGRAM(
          [[
            #include <stdint.h>
            #include <immintrin.h>
          ]],
          [[
            __m256i a = _mm256_set1_epi64x(3);
            __m128i b = _mm_set1_epi64x(5);
            return _mm256_extract_epi32(_mm256_madd52lo_avx_epu64(a, a, a), 2) + _mm_extract_epi32(_mm_madd52hi_avx_epu64(b, b, b), 2);
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

AS_IF([test "x${enable_sha512}" != "xno"],
  [
    AX_CHECK_COMPILE_FLAG([-mavx2 -msha512],
      [
        CFLAGS="${CFLAGS} -mavx2 -msha512";
        CXXFLAGS="${CXXFLAGS} -mavx2 -msha512";
        AC_SUBST([sha512], ["-mavx2 -msha512"])
      ],[
        AC_MSG_ERROR([-mavx2 -msha512 not supported.])
      ],
      [],
      [
        AC_LANG_PROGRAM(
          [[
            #include <stdint.h>
            #include <immintrin.h>
          ]],
          [[
            __m256i a = _mm256_set1_epi64x(0);
            __m128i k = _mm_set1_epi64x(15);
            return _mm256_extract_epi32(_mm256_sha512msg2_epi64(_mm256_sha512msg1_epi64(_mm256_sha512rnds2_epi64(a, a, k), k), a), 2);
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

AS_IF([test "x${enable_aesni}" != "xno"],
  [
    AX_CHECK_COMPILE_FLAG([-msse4.1 -maes -mpclmul],
      [
        CFLAGS="${CFLAGS} -msse4.1 -maes -mpclmul";
        CXXFLAGS="${CXXFLAGS} -msse4.1 -maes -mpclmul";
        AC_SUBST([aesni], ["-msse4.1 -maes -mpclmul"])
      ],[
        AC_MSG_ERROR([-msse4.1 -maes -mpclmul not supported.])
      ],
      [],
      [
        AC_LANG_PROGRAM(
          [[
            #include <stdint.h>
            #include <immintrin.h>
          ]],
          [[
            __m128i a = _mm_set1_epi32(1);
            __m128i b = _mm_set1_epi32(3);
            return _mm_cvtsi128_si32(_mm_clmulepi64_si128(_mm_aesenc_si128(a, b), b, 0x00));
          ]])
      ])
  ])

AS_IF([test "x${enable_vaes}" != "xno"],
  [
    AX_CHECK_COMPILE_FLAG([-mavx2 -maes -mpclmul -mvaes -mvpclmulqdq],
      [
        CFLAGS="${CFLAGS} -mavx2 -maes -mpclmul -mvaes -mvpclmulqdq";
        CXXFLAGS="${CXXFLAGS} -mavx2 -maes -mpclmul -mvaes -mvpclmulqdq";
        AC_SUBST([vaes], ["-mavx2 -maes -mpclmul -mvaes -mvpclmulqdq"])
      ],[
        AC_MSG_ERROR([-mavx2 -maes -mpclmul -mvaes -mvpclmulqdq not supported.])
      ],
      [],
      [
        AC_LANG_PROGRAM(
          [[
            #include <stdint.h>
            #include <immintrin.h>
          ]],
          [[
            __m256i a = _mm256_set1_epi32(1);
            __m256i b = _mm256_set1_epi32(3);
            return _mm256_extract_epi32(_mm256_clmulepi64_epi128(_mm256_aesenc_epi128(a, b), b, 0x00), 1);
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

march_base=""
march_features=""
AS_IF([test "x${enable_sha}" != "xno"],
  [
    march_base="armv8-a"
    march_features="${march_features}+sha2"
  ])
AS_IF([test "x${enable_aes}" != "xno"],
  [
    march_base="armv8-a"
    march_features="${march_features}+aes"
  ])
AS_IF([test "x${enable_sha3}" != "xno"],
  [
    march_base="armv8.2-a"
    march_features="${march_features}+sha3"
  ])

AS_IF([test "x${march_features}" != "x"],
  [
    march="-march=${march_base}${march_features}"
    march_CXXFLAGS="${CXXFLAGS}"
    CXXFLAGS="${CXXFLAGS} ${march}"

    AS_IF([test "x${enable_sha}" != "xno"],
      [
        AC_MSG_CHECKING([whether ${march} supports sha])
        AC_COMPILE_IFELSE(
          [
            AC_LANG_PROGRAM(
              [[
                #include <arm_neon.h>
              ]],
              [[
                uint32x4_t a = vdupq_n_u32(0);
                return vgetq_lane_u32(vsha256hq_u32(a, a, a), 0);
              ]])
          ],
          [AC_MSG_RESULT([yes])],
          [AC_MSG_ERROR([${march} does not support sha.])])
      ])

    AS_IF([test "x${enable_aes}" != "xno"],
      [
        AC_MSG_CHECKING([whether ${march} supports aes])
        AC_COMPILE_IFELSE(
          [
            AC_LANG_PROGRAM(
              [[
                #include <arm_neon.h>
              ]],
              [[
                uint8x16_t a = vdupq_n_u8(0);
                return vgetq_lane_u8(vaeseq_u8(a, a), 0);
              ]])
          ],
          [AC_MSG_RESULT([yes])],
          [AC_MSG_ERROR([${march} does not support aes.])])
      ])

    AS_IF([test "x${enable_sha3}" != "xno"],
      [
        AC_MSG_CHECKING([whether ${march} supports sha3])
        AC_COMPILE_IFELSE(
          [
            AC_LANG_PROGRAM(
              [[
                #include <arm_neon.h>
              ]],
              [[
                uint64x2_t a = vdupq_n_u64(0);
                return vgetq_lane_u64(vsha512hq_u64(a, a, a), 0);
              ]])
          ],
          [AC_MSG_RESULT([yes])],
          [AC_MSG_ERROR([${march} does not support sha3.])])
      ])

    CXXFLAGS="${march_CXXFLAGS}"
    CFLAGS="${CFLAGS} ${march}"
    CXXFLAGS="${CXXFLAGS} ${march}"
    AC_SUBST([march], ["${march}"])
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

AS_IF([test "x${with_secp256k1}" != "xno"],
  [
    PKG_CHECK_MODULES(
      [secp256k1],
      [libsecp256k1 >= 0.8.0],
      [],
      [
        AC_MSG_ERROR([libsecp256k1 >= 0.8.0 is required but was not found.])
      ])
    AC_SUBST([secp256k1_PKG], ['libsecp256k1 >= 0.8.0'])
    AC_SUBST([secp256k1_VERSION], [`${PKG_CONFIG} --modversion libsecp256k1`])
    AC_SUBST([secp256k1_CPPFLAGS], [${secp256k1_CFLAGS}])
    AC_SUBST([secp256k1_ISYS_CPPFLAGS], [`echo ${secp256k1_CPPFLAGS} | ${SED} s/^-I/-isystem/g | ${SED} s/' -I'/' -isystem'/g`])
    AC_MSG_NOTICE([secp256k1_VERSION : ${secp256k1_VERSION}])
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

# Set preprocessor defines.
#==============================================================================
AS_IF([test "x${with_secp256k1}" != "xno"], [
        AC_DEFINE([WITH_SECP256K1])
    ])

AS_IF([test "x${with_secp256k1}" != "xno"], [
        AC_DEFINE_UNQUOTED([SECP256K1_VERSION], [${secp256k1_VERSION}])
    ])

AS_IF([test "x${with_valgrind}" != "xno"], [
        AC_DEFINE([WITH_VALGRIND])
    ])

AS_IF([test "x${enable_cuda}" != "xno"], [
        AC_DEFINE([ENABLE_CUDA])
    ])

AC_CONFIG_FILES([
    Makefile
    libbitcoin-system.pc
    include/bitcoin/system/version.hpp:${repository_root_dir}/include/bitcoin/system/version.hpp.in
  ])

AC_OUTPUT
