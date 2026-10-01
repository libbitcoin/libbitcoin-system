/**
 * Copyright (c) 2011-2026 libbitcoin developers
 *
 * This file is part of libbitcoin.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef LIBBITCOIN_SYSTEM_HAVE_HPP
#define LIBBITCOIN_SYSTEM_HAVE_HPP

#include <bitcoin/system/version.hpp>

///////////////////////////////////////////////////////////////////////////////
// Maintainers: update corresponding diagnostic HAVE emissions in define.cpp.
///////////////////////////////////////////////////////////////////////////////

/// Plaform: architecture, compiler, and standard libraries.
/// ---------------------------------------------------------------------------

#if defined(__linux__)
    #define HAVE_LINUX
#elif defined(__APPLE__)
    #define HAVE_APPLE
#elif defined(__FreeBSD__)
    #define HAVE_FREEBSD
#elif defined(__OpenBSD__)
    #define HAVE_OPENBSD
#elif defined(__NetBSD__)
    #define HAVE_NETBSD
#elif defined(__CYGWIN__)
    #define HAVE_CYGWIN
#endif

// Others excluded presently due to lack of verified support.
#if defined(HAVE_LINUX) || defined(HAVE_APPLE)
    #define HAVE_POSIX
#endif

/// stackoverflow.com/questions/38499462/how-to-tell-clang-to-stop-pretending-
/// to-be-other-compilers
#if defined(__clang__)
    #define HAVE_CLANG
#endif
#if defined(__APPLE__) && defined(HAVE_CLANG) && defined(__apple_build_version__)
    #define HAVE_XCODE
#endif
#if defined(__GNUC__) && !defined(HAVE_CLANG)
    #define HAVE_GNUC
#endif
#if defined(_MSC_VER) && !defined(HAVE_CLANG)
    #define HAVE_MSC
#endif

/// Determines linker defines (Windows vs. Unix/Linux).
#if defined(HAVE_CLANG) || defined(HAVE_GNUC)
    #define HAVE_NX_LIBS
#elif defined(_MSC_VER) || defined(__CYGWIN__)
    #define HAVE_WINDOWS_LIBS
#endif

/// GNU/MSC defines for targeted CPU architecture.
/// sourceforge.net/p/predef/wiki/Architectures
/// docs.microsoft.com/en-us/cpp/preprocessor/predefined-macros
#if defined(__i386__) || defined(_M_IX86)
    #define HAVE_X32
    #define HAVE_XCPU
#elif defined(__amd64__) || defined(_M_AMD64)
    #define HAVE_X64
    #define HAVE_XCPU
#elif defined(__arm__) || defined(_M_ARM)
    #define HAVE_ARM32
    #define HAVE_ARM
#elif defined(__aarch64__) || defined(_M_ARM64)
    #define HAVE_ARM64
    #define HAVE_ARM
#endif

/// WITH_ build symbols.
/// ---------------------------------------------------------------------------

/// vc++: There are no flags for SHANI/CRYPTO, so use custom WITH_SHA option.
#if defined(HAVE_MSC) && defined(WITH_SHA)
    #if defined(HAVE_XCPU)
        #define __SHA__
    #elif defined(HAVE_ARM)
        #define __ARM_FEATURE_CRYPTO
    #endif
#endif

/// vc++: There are no flags for SHA512/SHA3, so use custom WITH_SHA512 option.
#if defined(HAVE_MSC) && defined(WITH_SHA512)
    #if defined(HAVE_XCPU)
        #define __SHA512__
    #elif defined(HAVE_ARM)
        #define __ARM_FEATURE_SHA512
    #endif
#endif

/// vc++: There are no flags for AVX512IFMA, AVXIFMA, AES-NI and VAES, so use
/// custom WITH_AVX512IFMA, WITH_AVXIFMA, WITH_AESNI and WITH_VAES options.
#if defined(HAVE_MSC) && defined(HAVE_XCPU)
    #if defined(WITH_AVX512IFMA)
        #define __AVX512IFMA__
    #endif
    #if defined(WITH_AVXIFMA)
        #define __AVXIFMA__
    #endif
    #if defined(WITH_AESNI)
        #define __AES__
        #define __PCLMUL__
    #endif
    #if defined(WITH_VAES)
        #define __VAES__
        #define __VPCLMULQDQ__
    #endif
#endif

// Custom options to use extended SVE variable width.
#if defined(__ARM_FEATURE_SVE)
    #if defined(WITH_512)
        #define HAVE_512
    #endif
    #if defined(WITH_256)
        #define HAVE_256
    #endif
#endif

// bitcoin-core/secp256k1 (otherwise local secp256k1)
#if defined(WITH_SECP256K1)
    #define HAVE_SECP256K1
#endif

// shrec/UltrafastSecp256k1 (batch verification)
#if defined(WITH_ULTRAFAST)
    #define HAVE_ULTRAFAST
#endif

#if defined(HAVE_SECP256K1) && defined(HAVE_ULTRAFAST)
    #error secp256k1 and ultrafast are mutually exclusive.
#endif

/// ENABLE_ build symbols.
/// ---------------------------------------------------------------------------

// Compilation of device kernels to ptx (developer).
#if defined(ENABLE_PTX)
    #define HAVE_PTX
#endif

/// Platform features derived.
/// ---------------------------------------------------------------------------

/// vc++: There are no flags for SSE41 so use AVX as it implies at least sse41.
#if defined(HAVE_MSC) && defined(__AVX__) && !defined(__SSE4_1__)
    #define __SSE4_1__
#endif

/// vc++: AVX512 (/arch:AVX512) implies AVX512VL.
#if defined(HAVE_MSC) && defined(__AVX512BW__) && !defined(__AVX512VL__)
    #define __AVX512VL__
#endif

/// vc++: ARM implies NEON, SVE not supported, CRYPTO requires custom option.
#if defined(HAVE_MSC) && defined(HAVE_ARM) && !defined(__ARM_NEON)
    #define __ARM_NEON
#endif

/// Guard assumption of hierarchy.
#if defined(__AVX512IFMA__) && !defined(__AVX512VL__)
    #define __AVX512VL__
#endif
#if defined(__AVX512VL__) && !defined(__AVX512F__)
    #define __AVX512F__
#endif
#if defined(__AVX512BW__) && !defined(__AVX512F__)
    #define __AVX512F__
#endif
#if defined(__AVX512F__) && !defined(__AVX2__)
    #define __AVX2__
#endif
#if defined(__AVXIFMA__) && !defined(__AVX2__)
    #define __AVX2__
#endif
#if defined(__SHA512__) && !defined(__AVX2__)
    #define __AVX2__
#endif
#if defined(__VAES__) && !defined(__AES__)
    #define __AES__
#endif
#if defined(__VPCLMULQDQ__) && !defined(__PCLMUL__)
    #define __PCLMUL__
#endif
#if defined(__VAES__) && !defined(__AVX2__)
    #define __AVX2__
#endif
#if defined(__AES__) && !defined(__SSE4_1__)
    #define __SSE4_1__
#endif
#if defined(__SHA__) && !defined(__SSE4_1__)
    #define __SSE4_1__
#endif
#if defined(__AVX2__) && !defined(__AVX__)
    #define __AVX__
#endif
#if defined(__AVX__) && !defined(__SSE4_1__)
    #define __SSE4_1__
#endif
#if defined(__ARM_FEATURE_CRYPTO) && !defined(__ARM_NEON)
    #define __ARM_NEON
#endif
#if defined(__ARM_FEATURE_SHA512) && !defined(__ARM_NEON)
    #define __ARM_NEON
#endif
#if defined(__ARM_FEATURE_SVE) && !defined(__ARM_NEON)
    #define __ARM_NEON
#endif

/// Map standard XCPU defines for intrinsics usage.
/// vc++: There are no native flags for SHA and SSE41.
#if defined(HAVE_XCPU)
    // -msha
    // vc++: SHANI not independently configurable (requires custom option).
    #if defined(__SHA__)
        #define HAVE_SHANI
        #define HAVE_SHA
    #endif
    // -mavx2 -msha512
    // vc++: SHA512 not independently configurable (requires custom option).
    #if defined(__SHA512__)
        #define HAVE_SHANI512
        #define HAVE_SHA512
    #endif
    // -mavx512bw -mavx512vl
    // vc++: Advanced Vector Extensions 512 (X86/X64) (/arch:AVX512)
    // AVX512VL is required because without it compilers widen 256 bit rotates
    // to 512 bit, which lowers the clock of cpus that throttle avx512.
    #if defined(__AVX512BW__) && defined(__AVX512VL__)
        #define HAVE_AVX512
        #define HAVE_512
    #endif
    // -mavx2
    // vc++: Advanced Vector Extensions 2 (X86/X64) (/arch:AVX2)
    #if defined(__AVX2__)
        #define HAVE_AVX2
        #define HAVE_256
    #endif
    // -msse4.1
    // vc++: Use Advanced Vector Extensions (X86/X64) (/arch:AVX).
    #if defined(__SSE4_1__)
        #define HAVE_SSE41
        #define HAVE_128
    #endif
    // -mavx512ifma -mavx512vl
    // vc++: AVX512IFMA not independently configurable (requires custom option).
    #if defined(__AVX512IFMA__)
        #define HAVE_AVX512IFMA
    #endif
    // -mavxifma
    // vc++: AVXIFMA not independently configurable (requires custom option).
    #if defined(__AVXIFMA__)
        #define HAVE_AVXIFMA
    #endif
    // -maes -mpclmul
    // vc++: AES-NI not independently configurable (requires custom option).
    #if defined(__AES__) && defined(__PCLMUL__)
        #define HAVE_AESNI
        #define HAVE_AES
    #endif
    // -mvaes -mvpclmulqdq
    // vc++: VAES not independently configurable (requires custom option).
    #if defined(__VAES__) && defined(__VPCLMULQDQ__)
        #define HAVE_VAES
    #endif

    // 52 bit fused multiply-add by lane width.
    #if defined(HAVE_AVX512IFMA) || defined(HAVE_AVXIFMA)
        #define HAVE_IFMA_256
        #define HAVE_IFMA_128
    #endif
    #if defined(HAVE_AVX512IFMA) && defined(HAVE_AVX512)
        #define HAVE_IFMA_512
    #endif
#endif

/// Map standard ARM defines for intrinsics usage. 
/// SVE maximum width is not detectable (requires custom options).
#if defined(HAVE_ARM)
    // -march=armv8-a+crypto
    // Requires 64 bit build.
    #if defined(__ARM_FEATURE_CRYPTO)
        #define HAVE_CRYPTO
        #define HAVE_SHA
        #define HAVE_AES
    #endif
    // -march=armv8.2-a+crypto+sha3
    // Requires 64 bit build.
    #if defined(__ARM_FEATURE_SHA512)
        #define HAVE_SHA3
        #define HAVE_SHA512
    #endif
    // -march=armv8-a+sve
    // Requires 64 bit build.
    #if defined(__ARM_FEATURE_SVE)
        #define HAVE_SVE
    #endif
    // -march=armv8-a (64 bit)
    // -march=armv7-a -mfpu=neon -mfloat-abi=hard (32 bit)
    #if defined(__ARM_NEON)
        #define HAVE_NEON
        #define HAVE_128
    #endif
#endif

/// XCPU architecture intrinsics _xgetbv, _cpuid, __cpuidex/__cpuid_count.
/// Always enabled on XCPU, as the binary is not portable to other CPUs.
#if defined(HAVE_XCPU)
    // TODO: CLANG/GCC compile test XCPU and always set -mxsave.
    // TODO: these these defines can be enabled unconditionally.
    #if defined(HAVE_CLANG)
        // Clang13: __cpuid_count/_cpuid/_xgetbv.
        ////#define HAVE_XGETBV
        ////#define HAVE_XCPUID_COUNT
    #endif
    #if defined(HAVE_GNUC)
        // GCC11: __cpuidex/_cpuid/_xgetbv.
        ////#define HAVE_XGETBV
        ////#define HAVE_XCPUIDEX
    #endif
    #if defined(HAVE_MSC)
        // Always available without platform test or build configuration.
        // docs.microsoft.com/en-us/cpp/intrinsics/cpuid-cpuidex
        // docs.microsoft.com/en-us/cpp/intrinsics/x86-intrinsics-list
        #define HAVE_XGETBV
        #define HAVE_XCPUIDEX
    #endif
#endif

/// XCPU architecture inline assembly.
#if defined(HAVE_XCPU) && !(defined(HAVE_MSC) && defined(HAVE_X32))
    #define HAVE_XCPU_ASSEMBLY
#endif

/// ARM architecture inline assembly.
#if defined(HAVE_ARM) && !(defined(HAVE_MSC) && defined(HAVE_X32))
    #define HAVE_ARM_ASSEMBLY
#endif

#if defined(__FAST_MATH__) || defined(_M_FP_FAST)
    #define HAVE_FAST_MATH
#endif

/// MSC predefined constant for Visual Studio version (exclusive).
/// ---------------------------------------------------------------------------

#if defined(HAVE_MSC)
    #if   _MSC_VER >= 1930
        #define HAVE_VS2022
    #elif _MSC_VER >= 1920
        #define HAVE_VS2019
    #elif _MSC_VER >= 1910
        #define HAVE_VS2017
    #elif _MSC_VER >= 1900
        #define HAVE_VS2015
    #elif _MSC_VER >= 1800
        #define HAVE_VS2013
    #endif
#endif

/// C/C++ language and support by platform.
/// ---------------------------------------------------------------------------

/// ISO predefined constant for C++ version (inclusive).
#if __cplusplus >= 199711L
    #define HAVE_CPP03
#endif
#if __cplusplus >= 201103L
    #define HAVE_CPP11
#endif
#if __cplusplus >= 201402L
    #define HAVE_CPP14
#endif
#if __cplusplus >= 201703L
    #define HAVE_CPP17
#endif
#if __cplusplus >= 202002L
    #define HAVE_CPP20
#endif

/// These are manually configured here.
/// ---------------------------------------------------------------------------

/// Disable to suppress pragma messages.
#define HAVE_MESSAGES

/// Disable to unsuppress warnings.
#define HAVE_SUPPRESSION

/// Disable to emit all suppressed warnings.
#define HAVE_WARNINGS

// Deprecated is noisy, turn on to find dependencies.
////#define HAVE_DEPRECATED

#endif
