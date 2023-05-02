###############################################################################
#  Copyright (c) 2014-2020 libbitcoin-system developers (see COPYING).
#
#         GENERATED SOURCE CODE, DO NOT EDIT EXCEPT EXPERIMENTALLY
#
###############################################################################

# libbitcoin-system project configuration.
#------------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.5 FATAL_ERROR)

project(libbitcoin-system LANGUAGES C CXX)

enable_testing()

list( APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/modules" )
include(CheckIncludeFiles)
include(CheckSymbolExists)
include(CheckCXXCompilerFlag)
include(CheckCXXSourceCompiles)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

if (MSVC)
  set( CANONICAL_LIB_NAME libbitcoin-system )
else ()
  set( CANONICAL_LIB_NAME bitcoin-system )
  find_package( PkgConfig REQUIRED )

  set( prefix "${CMAKE_PREFIX_PATH}" )
  set( exec_prefix "\${prefix}" )
  set( libdir "\${exec_prefix}/lib" )
  set( includedir "\${exec_prefix}/include" )

  set( PACKAGE_VERSION "4.0.0" )
  set( VERSION "${PACKAGE_VERSION}" )
endif ()

set( CMAKE_CXX_STANDARD 20 )
set( CMAKE_CXX_STANDARD_REQUIRED ON )

# Add compiler options
#------------------------------------------------------------------------------
# Warn on all stuff.
check_cxx_compiler_flag( "-Wall" HAS_FLAG_WALL )
if ( HAS_FLAG_WALL )
    add_compile_options( "-Wall" )
else()
    message( FATAL_ERROR "Compiler does not support -Wall" )
endif()

# Warn on extra stuff.
check_cxx_compiler_flag( "-Wextra" HAS_FLAG_WEXTRA )
if ( HAS_FLAG_WEXTRA )
    add_compile_options( "-Wextra" )
else()
    message( FATAL_ERROR "Compiler does not support -Wextra" )
endif()

# Disallow warning on style order of declarations.
check_cxx_compiler_flag( "-Wno-reorder" HAS_FLAG_WNO-REORDER )
if ( HAS_FLAG_WNO-REORDER )
    add_compile_options( "-Wno-reorder" )
else()
    message( FATAL_ERROR "Compiler does not support -Wno-reorder" )
endif()

# Suppress warning for incomplete field initialization.
check_cxx_compiler_flag( "-Wno-missing-field-initializers" HAS_FLAG_WNO-MISSING-FIELD-INITIALIZERS )
if ( HAS_FLAG_WNO-MISSING-FIELD-INITIALIZERS )
    add_compile_options( "-Wno-missing-field-initializers" )
else()
    message( FATAL_ERROR "Compiler does not support -Wno-missing-field-initializers" )
endif()

# Conform to style.
check_cxx_compiler_flag( "-Wno-missing-braces" HAS_FLAG_WNO-MISSING-BRACES )
if ( HAS_FLAG_WNO-MISSING-BRACES )
    add_compile_options( "-Wno-missing-braces" )
else()
    message( FATAL_ERROR "Compiler does not support -Wno-missing-braces" )
endif()

# Ignore comments within comments or commenting of backslash extended lines.
check_cxx_compiler_flag( "-Wno-comment" HAS_FLAG_WNO-COMMENT )
if ( HAS_FLAG_WNO-COMMENT )
    add_compile_options( "-Wno-comment" )
else()
    message( FATAL_ERROR "Compiler does not support -Wno-comment" )
endif()

# Suppress warning for copy of implicitly generated copy constructor.
check_cxx_compiler_flag( "-Wno-deprecated-copy" HAS_FLAG_WNO-DEPRECATED-COPY )
if ( HAS_FLAG_WNO-DEPRECATED-COPY )
    add_compile_options( "-Wno-deprecated-copy" )
else()
    message( FATAL_ERROR "Compiler does not support -Wno-deprecated-copy" )
endif()

# Allow use of C99 'long long' type.
check_cxx_compiler_flag( "-Wno-long-long" HAS_FLAG_WNO-LONG-LONG )
if ( HAS_FLAG_WNO-LONG-LONG )
    add_compile_options( "-Wno-long-long" )
else()
    message( FATAL_ERROR "Compiler does not support -Wno-long-long" )
endif()

# Conflict in stdlib under clang.
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    check_cxx_compiler_flag( "-Wno-mismatched-tags" HAS_FLAG_WNO-MISMATCHED-TAGS )
    if ( HAS_FLAG_WNO-MISMATCHED-TAGS )
        add_compile_options( "-Wno-mismatched-tags" )
    else()
        message( FATAL_ERROR "Compiler does not support -Wno-mismatched-tags" )
    endif()
endif()

# Limit delays and warnings.
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    check_cxx_compiler_flag( "-fno-var-tracking-assignments" HAS_FLAG_FNO-VAR-TRACKING-ASSIGNMENTS )
    if ( HAS_FLAG_FNO-VAR-TRACKING-ASSIGNMENTS )
        add_compile_options( "-fno-var-tracking-assignments" )
    else()
        message( FATAL_ERROR "Compiler does not support -fno-var-tracking-assignments" )
    endif()
endif()

# Implement -Dpkgconfigdir and output ${pkgconfigdir}.
#------------------------------------------------------------------------------
set( pkgconfigdir "${libdir}/pkgconfig" CACHE PATH "Path to pkgconfig directory." )

# Implement -Dwith-tests and declare with-tests.
#------------------------------------------------------------------------------
set( with-tests "yes" CACHE BOOL "Compile with unit tests." )

# Implement -Dwith-examples and declare with-examples.
#------------------------------------------------------------------------------
set( with-examples "yes" CACHE BOOL "Compile with examples." )

# Implement -Dwith-icu and define BOOST_HAS_ICU and output ${icu}.
#------------------------------------------------------------------------------
set( with-icu "no" CACHE BOOL "Compile with International Components for Unicode." )

if (with-icu)
    add_definitions( -DBOOST_HAS_ICU )
endif()

if (with-icu)
    set( icu "-DWITH_ICU" )
endif()

# Implement -Denable-avx2.
#------------------------------------------------------------------------------
set( enable-avx2 "no" CACHE BOOL "Compile with avx2 intrinsics (specifically -mavx -mavx2)." )

# Implement -Denable-avx512.
#------------------------------------------------------------------------------
set( enable-avx512 "no" CACHE BOOL "Compile with avx512 intrinsics (specifically -mavx512bw -mavx512f)." )

# Implement -Denable-sse4.
#------------------------------------------------------------------------------
set( enable-sse4 "no" CACHE BOOL "Compile with sse4 intrinsics (specifically -msse4.1)." )

# Implement -Denable-shani.
#------------------------------------------------------------------------------
set( enable-shani "no" CACHE BOOL "Compile with sha native intrinsics (specifically -msse4 -msha)" )

# Implement -Denable-ndebug and define NDEBUG.
#------------------------------------------------------------------------------
set( enable-ndebug "yes" CACHE BOOL "Compile without debug assertions." )

if (enable-ndebug)
    add_definitions( -DNDEBUG )
endif()

# Inherit -Denable-shared and define BOOST_ALL_DYN_LINK.
#------------------------------------------------------------------------------
if (BUILD_SHARED_LIBS)
    add_definitions( -DBOOST_ALL_DYN_LINK )
endif()

if (enable-avx2)
    check_cxx_compiler_flag("-mavx -mavx2" HAS_FLAGS_AVX2)

    if (HAS_FLAGS_AVX2)
        add_compile_options( "-mavx" )
        add_compile_options( "-mavx2" )
        set( CMAKE_REQUIRED_FLAGS_PREV "${CMAKE_REQUIRED_FLAGS}" )
        set( CMAKE_REQUIRED_FLAGS "${CMAKE_REQUIRED_FLAGS} -mavx -mavx2" )
    endif()

    check_cxx_source_compiles("
        #include <stdint.h>
        #include <immintrin.h>
        int main() {
            __m256i a = _mm256_set_epi32(8, 7, 6, 5, 4, 3, 2, 1);
            return _mm256_extract_epi32(_mm256_slli_epi64(a, 2), 5);
        }" HAVE_AVX2)

    if (HAS_FLAGS_AVX2)
        set( CMAKE_REQUIRED_FLAGS "${CMAKE_REQUIRED_FLAGS_PREV}" )
    endif()

    if ( HAVE_AVX2 )
        add_compile_definitions( HAVE_AVX2 )
    else()
        message( FATAL_ERROR "Failed to enable HAVE_AVX2" )
    endif()
endif()

if (enable-avx512)
    check_cxx_compiler_flag("-mavx512f -mavx512bw" HAS_FLAGS_AVX512)

    if (HAS_FLAGS_AVX512)
        add_compile_options( "-mavx512f" )
        add_compile_options( "-mavx512bw" )
        set( CMAKE_REQUIRED_FLAGS_PREV "${CMAKE_REQUIRED_FLAGS}" )
        set( CMAKE_REQUIRED_FLAGS "${CMAKE_REQUIRED_FLAGS} -mavx512f -mavx512bw" )
    endif()

    check_cxx_source_compiles("
        #include <stdint.h>
        #include <immintrin.h>
        int main() {
            __m512i a = _mm512_set_epi32(16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1);
            return _mm_extract_epi32(_mm512_extracti32x4_epi32(_mm512_slli_epi64(a, 2), 2), 1);
        }" HAVE_AVX512)

    if (HAS_FLAGS_AVX512)
        set( CMAKE_REQUIRED_FLAGS "${CMAKE_REQUIRED_FLAGS_PREV}" )
    endif()

    if ( HAVE_AVX512 )
        add_compile_definitions( HAVE_AVX512 )
    else()
        message( FATAL_ERROR "Failed to enable HAVE_AVX512" )
    endif()
endif()

if (enable-shani)
    check_cxx_compiler_flag("-msse4 -msha" HAS_FLAGS_SHANI)

    if (HAS_FLAGS_SHANI)
        add_compile_options( "-msse4" )
        add_compile_options( "-msha" )
        set( CMAKE_REQUIRED_FLAGS_PREV "${CMAKE_REQUIRED_FLAGS}" )
        set( CMAKE_REQUIRED_FLAGS "${CMAKE_REQUIRED_FLAGS} -msse4 -msha" )
    endif()

    check_cxx_source_compiles("
        #include <stdint.h>
        #include <immintrin.h>
        int main() {
            __m128i a = _mm_set1_epi32(0);
            __m128i b = _mm_set1_epi32(15);
            __m128i k = _mm_set1_epi32(31);
            return _mm_extract_epi32(_mm_sha256msg2_epu32(_mm_sha256msg1_epu32(_mm_sha256rnds2_epu32(a, b, k), b), a), 2);
        }" HAVE_SHANI)

    if (HAS_FLAGS_SHANI)
        set( CMAKE_REQUIRED_FLAGS "${CMAKE_REQUIRED_FLAGS_PREV}" )
    endif()

    if ( HAVE_SHANI )
        add_compile_definitions( HAVE_SHANI )
    else()
        message( FATAL_ERROR "Failed to enable HAVE_SHANI" )
    endif()
endif()

if (enable-sse4)
    check_cxx_compiler_flag("-msse4.1" HAS_FLAGS_SSE4)

    if (HAS_FLAGS_SSE4)
        add_compile_options( "-msse4.1" )
        set( CMAKE_REQUIRED_FLAGS_PREV "${CMAKE_REQUIRED_FLAGS}" )
        set( CMAKE_REQUIRED_FLAGS "${CMAKE_REQUIRED_FLAGS} -msse4.1" )
    endif()

    check_cxx_source_compiles("
        #include <stdint.h>
        #include <immintrin.h>
        int main() {
            __m128i a = _mm_set1_epi32(0);
            __m128i b = _mm_set1_epi32(15);
            return _mm_extract_epi32(_mm_add_epi64(a, b), 2);
        }" HAVE_SSE4)

    if (HAS_FLAGS_SSE4)
        set( CMAKE_REQUIRED_FLAGS "${CMAKE_REQUIRED_FLAGS_PREV}" )
    endif()

    if ( HAVE_SSE4 )
        add_compile_definitions( HAVE_SSE4 )
    else()
        message( FATAL_ERROR "Failed to enable HAVE_SSE4" )
    endif()
endif()

if (BUILD_SHARED_LIBS)
    set( Boost_USE_STATIC_LIBS "off" )
else()
    set( Boost_USE_STATIC_LIBS "on" )
endif()

# Find boost
#------------------------------------------------------------------------------
find_package( Boost 1.76.0 REQUIRED COMPONENTS
    chrono
    container
    iostreams
    json
    locale
    program_options
    system
    thread
    unit_test_framework )

set( boost_chrono_LIBS "-lboost_chrono" )
set( boost_container_LIBS "-lboost_container" )
set( boost_iostreams_LIBS "-lboost_iostreams" )
set( boost_json_LIBS "-lboost_json" )
set( boost_locale_LIBS "-lboost_locale" )
set( boost_program_options_LIBS "-lboost_program_options" )
set( boost_system_LIBS "-lboost_system" )
set( boost_thread_LIBS "-lboost_thread" )
set( boost_unit_test_framework_LIBS "-lboost_unit_test_framework" )

if (enable-ndebug)
    set( Boost_LIBRARY_DIR "${Boost_LIBRARY_DIR_DEBUG}" )
else ()
    set( Boost_LIBRARY_DIR "${Boost_LIBRARY_DIR_RELEASE}" )
endif()

set( boost_CPPFLAGS "-I${Boost_INCLUDE_DIR}" )
set( boost_LDFLAGS "-L${Boost_LIBRARY_DIR}" )

set( CMAKE_THREAD_PREFER_PTHREAD ON )
set( THREADS_PREFER_PTHREAD_FLAG ON )

# Find threads
#------------------------------------------------------------------------------
if (NOT ANDROID)
    find_package( Threads REQUIRED )
endif()

if ( Threads_FOUND AND NOT ANDROID )
    # TODO: find proper detection - documentation on FindThreads appears incorrect.
    set( pthread_CPPFLAGS "" )
    set( pthread_LIBRARIES "-lpthread" )
    set( pthread_LIBS "-lpthread" )
endif()

# Find rt
#------------------------------------------------------------------------------
if ((${CMAKE_SYSTEM_NAME} MATCHES "Linux") AND NOT ANDROID)
    find_package( Rt REQUIRED )
endif()

# Find icu-i18n
#------------------------------------------------------------------------------
if (with-icu)
    find_package( Icu-I18N 55.2 REQUIRED )
endif()

# Find dl
#------------------------------------------------------------------------------
if ((${CMAKE_SYSTEM_NAME} MATCHES "Linux"))
    find_package( Dl REQUIRED )
endif()

# Find secp256k1
#------------------------------------------------------------------------------
find_package( Secp256K1 0.1.0.20 REQUIRED )

# Define project common includes directories
#------------------------------------------------------------------------------
if (BUILD_SHARED_LIBS)
    include_directories( SYSTEM
        ${Boost_INCLUDE_DIR}
        ${pthread_INCLUDE_DIRS}
        ${rt_INCLUDE_DIRS}
        ${icu_i18n_INCLUDE_DIRS}
        ${dl_INCLUDE_DIRS}
        ${secp256k1_INCLUDE_DIRS} )
else()
    include_directories( SYSTEM
        ${Boost_INCLUDE_DIR}
        ${pthread_INCLUDE_DIRS}
        ${rt_INCLUDE_DIRS}
        ${icu_i18n_STATIC_INCLUDE_DIRS}
        ${dl_INCLUDE_DIRS}
        ${secp256k1_STATIC_INCLUDE_DIRS} )
endif()

# Define project common library directories
#------------------------------------------------------------------------------
if (BUILD_SHARED_LIBS)
    link_directories(
        ${Boost_LIBRARY_DIRS}
        ${pthread_LIBRARY_DIRS}
        ${rt_LIBRARY_DIRS}
        ${icu_i18n_LIBRARY_DIRS}
        ${dl_LIBRARY_DIRS}
        ${secp256k1_LIBRARY_DIRS} )
else()
    link_directories(
        ${Boost_LIBRARY_DIRS}
        ${pthread_LIBRARY_DIRS}
        ${rt_LIBRARY_DIRS}
        ${icu_i18n_STATIC_LIBRARY_DIRS}
        ${dl_LIBRARY_DIRS}
        ${secp256k1_STATIC_LIBRARY_DIRS} )
endif()

# Define project common libraries/linker flags.
#------------------------------------------------------------------------------
if (BUILD_SHARED_LIBS)
    link_libraries(
        "-fstack-protector"
        "-fstack-protector-all"
        ${Boost_CHRONO_LIBRARY}
        ${Boost_IOSTREAMS_LIBRARY}
        ${Boost_JSON_LIBRARY}
        ${Boost_LOCALE_LIBRARY}
        ${Boost_PROGRAM_OPTIONS_LIBRARY}
        ${Boost_SYSTEM_LIBRARY}
        ${Boost_THREAD_LIBRARY}
        ${pthread_LIBRARIES}
        ${rt_LIBRARIES}
        ${icu_i18n_LIBRARIES}
        ${dl_LIBRARIES}
        ${secp256k1_LIBRARIES} )
else()
    link_libraries(
        "-fstack-protector"
        "-fstack-protector-all"
        ${Boost_CHRONO_LIBRARY}
        ${Boost_IOSTREAMS_LIBRARY}
        ${Boost_JSON_LIBRARY}
        ${Boost_LOCALE_LIBRARY}
        ${Boost_PROGRAM_OPTIONS_LIBRARY}
        ${Boost_SYSTEM_LIBRARY}
        ${Boost_THREAD_LIBRARY}
        ${pthread_LIBRARIES}
        ${rt_LIBRARIES}
        ${icu_i18n_STATIC_LIBRARIES}
        ${dl_LIBRARIES}
        ${secp256k1_STATIC_LIBRARIES} )
endif()

add_definitions(
    ${icu} )

# Define ${CANONICAL_LIB_NAME} project.
#------------------------------------------------------------------------------
add_library( ${CANONICAL_LIB_NAME}
    "../../src/define.cpp"
    "../../src/settings.cpp"
    "../../src/chain/block.cpp"
    "../../src/chain/chain_state.cpp"
    "../../src/chain/checkpoint.cpp"
    "../../src/chain/context.cpp"
    "../../src/chain/header.cpp"
    "../../src/chain/input.cpp"
    "../../src/chain/operation.cpp"
    "../../src/chain/output.cpp"
    "../../src/chain/point.cpp"
    "../../src/chain/script.cpp"
    "../../src/chain/transaction.cpp"
    "../../src/chain/witness.cpp"
    "../../src/chain/enums/opcode.cpp"
    "../../src/config/base16.cpp"
    "../../src/config/base2.cpp"
    "../../src/config/base32.cpp"
    "../../src/config/base58.cpp"
    "../../src/config/base64.cpp"
    "../../src/config/base85.cpp"
    "../../src/config/block.cpp"
    "../../src/config/hash160.cpp"
    "../../src/config/hash256.cpp"
    "../../src/config/header.cpp"
    "../../src/config/input.cpp"
    "../../src/config/output.cpp"
    "../../src/config/parameter.cpp"
    "../../src/config/parser.cpp"
    "../../src/config/point.cpp"
    "../../src/config/printer.cpp"
    "../../src/config/script.cpp"
    "../../src/config/transaction.cpp"
    "../../src/crypto/aes256.cpp"
    "../../src/crypto/der_parser.cpp"
    "../../src/crypto/ec_context.cpp"
    "../../src/crypto/ec_context.hpp"
    "../../src/crypto/golomb_coding.cpp"
    "../../src/crypto/pseudo_random.cpp"
    "../../src/crypto/ring_signature.cpp"
    "../../src/crypto/secp256k1.cpp"
    "../../src/data/data_chunk.cpp"
    "../../src/data/string.cpp"
    "../../src/endian/endian.cpp"
    "../../src/error/block_error_t.cpp"
    "../../src/error/error_t.cpp"
    "../../src/error/op_error_t.cpp"
    "../../src/error/script_error_t.cpp"
    "../../src/error/transaction_error_t.cpp"
    "../../src/hash/accumulator.cpp"
    "../../src/hash/checksum.cpp"
    "../../src/hash/siphash.cpp"
    "../../src/hash/vectorization/sha256_1_native.cpp"
    "../../src/hash/vectorization/sha256_2_shani.cpp"
    "../../src/hash/vectorization/sha256_4_neon.cpp"
    "../../src/hash/vectorization/sha256_4_sse4.cpp"
    "../../src/hash/vectorization/sha256_4_sse41.cpp"
    "../../src/hash/vectorization/sha256_8_avx2.cpp"
    "../../src/math/math.cpp"
    "../../src/radix/base_10.cpp"
    "../../src/radix/base_2048.cpp"
    "../../src/radix/base_32.cpp"
    "../../src/radix/base_58.cpp"
    "../../src/radix/base_64.cpp"
    "../../src/radix/base_85.cpp"
    "../../src/serial/props.cpp"
    "../../src/stream/binary.cpp"
    "../../src/unicode/ascii.cpp"
    "../../src/unicode/code_points.cpp"
    "../../src/unicode/conversion.cpp"
    "../../src/unicode/normalization.cpp"
    "../../src/unicode/utf8_everywhere/console_streambuf.cpp"
    "../../src/unicode/utf8_everywhere/environment.cpp"
    "../../src/unicode/utf8_everywhere/ifstream.cpp"
    "../../src/unicode/utf8_everywhere/ofstream.cpp"
    "../../src/unicode/utf8_everywhere/paths.cpp"
    "../../src/unicode/utf8_everywhere/stdio.cpp"
    "../../src/unicode/utf8_everywhere/unicode_istream.cpp"
    "../../src/unicode/utf8_everywhere/unicode_ostream.cpp"
    "../../src/unicode/utf8_everywhere/unicode_streambuf.cpp"
    "../../src/wallet/context.cpp"
    "../../src/wallet/message.cpp"
    "../../src/wallet/neutrino_filter.cpp"
    "../../src/wallet/point_value.cpp"
    "../../src/wallet/points_value.cpp"
    "../../src/wallet/addresses/bitcoin_uri.cpp"
    "../../src/wallet/addresses/payment_address.cpp"
    "../../src/wallet/addresses/qr_code.cpp"
    "../../src/wallet/addresses/stealth_address.cpp"
    "../../src/wallet/addresses/stealth_receiver.cpp"
    "../../src/wallet/addresses/stealth_sender.cpp"
    "../../src/wallet/addresses/tiff.cpp"
    "../../src/wallet/addresses/uri.cpp"
    "../../src/wallet/addresses/witness_address.cpp"
    "../../src/wallet/addresses/qrencode/bitstream.c"
    "../../src/wallet/addresses/qrencode/bitstream.h"
    "../../src/wallet/addresses/qrencode/mask.c"
    "../../src/wallet/addresses/qrencode/mask.h"
    "../../src/wallet/addresses/qrencode/mmask.c"
    "../../src/wallet/addresses/qrencode/mmask.h"
    "../../src/wallet/addresses/qrencode/mqrspec.c"
    "../../src/wallet/addresses/qrencode/mqrspec.h"
    "../../src/wallet/addresses/qrencode/qrencode.c"
    "../../src/wallet/addresses/qrencode/qrencode.h"
    "../../src/wallet/addresses/qrencode/qrencode_inner.h"
    "../../src/wallet/addresses/qrencode/qrinput.c"
    "../../src/wallet/addresses/qrencode/qrinput.h"
    "../../src/wallet/addresses/qrencode/qrspec.c"
    "../../src/wallet/addresses/qrencode/qrspec.h"
    "../../src/wallet/addresses/qrencode/rsecc.c"
    "../../src/wallet/addresses/qrencode/rsecc.h"
    "../../src/wallet/addresses/qrencode/split.c"
    "../../src/wallet/addresses/qrencode/split.h"
    "../../src/wallet/keys/ec_point.cpp"
    "../../src/wallet/keys/ec_private.cpp"
    "../../src/wallet/keys/ec_public.cpp"
    "../../src/wallet/keys/ec_scalar.cpp"
    "../../src/wallet/keys/ek_private.cpp"
    "../../src/wallet/keys/ek_public.cpp"
    "../../src/wallet/keys/ek_token.cpp"
    "../../src/wallet/keys/encrypted_keys.cpp"
    "../../src/wallet/keys/hd_private.cpp"
    "../../src/wallet/keys/hd_public.cpp"
    "../../src/wallet/keys/mini_keys.cpp"
    "../../src/wallet/keys/stealth.cpp"
    "../../src/wallet/keys/parse_encrypted_keys/parse_encrypted_key.hpp"
    "../../src/wallet/keys/parse_encrypted_keys/parse_encrypted_key.ipp"
    "../../src/wallet/keys/parse_encrypted_keys/parse_encrypted_prefix.hpp"
    "../../src/wallet/keys/parse_encrypted_keys/parse_encrypted_prefix.ipp"
    "../../src/wallet/keys/parse_encrypted_keys/parse_encrypted_private.cpp"
    "../../src/wallet/keys/parse_encrypted_keys/parse_encrypted_private.hpp"
    "../../src/wallet/keys/parse_encrypted_keys/parse_encrypted_public.cpp"
    "../../src/wallet/keys/parse_encrypted_keys/parse_encrypted_public.hpp"
    "../../src/wallet/keys/parse_encrypted_keys/parse_encrypted_token.cpp"
    "../../src/wallet/keys/parse_encrypted_keys/parse_encrypted_token.hpp"
    "../../src/wallet/mnemonics/electrum.cpp"
    "../../src/wallet/mnemonics/electrum_v1.cpp"
    "../../src/wallet/mnemonics/mnemonic.cpp"
    "../../src/words/languages.cpp"
    "../../src/words/catalogs/electrum.cpp"
    "../../src/words/catalogs/electrum_v1.cpp"
    "../../src/words/catalogs/mnemonic.cpp" )

# ${CANONICAL_LIB_NAME} project specific include directories.
#------------------------------------------------------------------------------
if (BUILD_SHARED_LIBS)
    target_include_directories( ${CANONICAL_LIB_NAME} PRIVATE
        "../../include"
        ${Boost_INCLUDE_DIR}
        ${pthread_INCLUDE_DIRS}
        ${rt_INCLUDE_DIRS}
        ${icu_i18n_INCLUDE_DIRS}
        ${dl_INCLUDE_DIRS}
        ${secp256k1_INCLUDE_DIRS} )
else()
    target_include_directories( ${CANONICAL_LIB_NAME} PRIVATE
        "../../include"
        ${Boost_INCLUDE_DIR}
        ${pthread_INCLUDE_DIRS}
        ${rt_INCLUDE_DIRS}
        ${icu_i18n_STATIC_INCLUDE_DIRS}
        ${dl_INCLUDE_DIRS}
        ${secp256k1_STATIC_INCLUDE_DIRS} )
endif()

target_include_directories( ${CANONICAL_LIB_NAME} PUBLIC
"../../include" )

# ${CANONICAL_LIB_NAME} project specific libraries/linker flags.
#------------------------------------------------------------------------------
if (BUILD_SHARED_LIBS)
    target_link_libraries( ${CANONICAL_LIB_NAME}
        ${Boost_CHRONO_LIBRARY}
        ${Boost_IOSTREAMS_LIBRARY}
        ${Boost_JSON_LIBRARY}
        ${Boost_LOCALE_LIBRARY}
        ${Boost_PROGRAM_OPTIONS_LIBRARY}
        ${Boost_SYSTEM_LIBRARY}
        ${Boost_THREAD_LIBRARY}
        ${pthread_LIBRARIES}
        ${rt_LIBRARIES}
        ${icu_i18n_LIBRARIES}
        ${dl_LIBRARIES}
        ${secp256k1_LIBRARIES} )
else()
    target_link_libraries( ${CANONICAL_LIB_NAME}
        ${Boost_CHRONO_LIBRARY}
        ${Boost_IOSTREAMS_LIBRARY}
        ${Boost_JSON_LIBRARY}
        ${Boost_LOCALE_LIBRARY}
        ${Boost_PROGRAM_OPTIONS_LIBRARY}
        ${Boost_SYSTEM_LIBRARY}
        ${Boost_THREAD_LIBRARY}
        ${pthread_LIBRARIES}
        ${rt_LIBRARIES}
        ${icu_i18n_STATIC_LIBRARIES}
        ${dl_LIBRARIES}
        ${secp256k1_STATIC_LIBRARIES} )
endif()

# Define libbitcoin-system-examples project.
#------------------------------------------------------------------------------
if (with-examples)
    add_executable( libbitcoin-system-examples
        "../../examples/main.cpp" )

#     libbitcoin-system-examples project specific include directories.
#------------------------------------------------------------------------------
    target_include_directories( libbitcoin-system-examples PRIVATE
        "../../include" )

#     libbitcoin-system-examples project specific libraries/linker flags.
#------------------------------------------------------------------------------
    target_link_libraries( libbitcoin-system-examples
        ${CANONICAL_LIB_NAME} )

endif()

# Define libbitcoin-system-test project.
#------------------------------------------------------------------------------
if (with-tests)
    add_executable( libbitcoin-system-test
        "../../test/constants.cpp"
        "../../test/constraints.cpp"
        "../../test/funclets.cpp"
        "../../test/hacks.cpp"
        "../../test/literals.cpp"
        "../../test/main.cpp"
        "../../test/settings.cpp"
        "../../test/test.cpp"
        "../../test/test.hpp"
        "../../test/typelets.cpp"
        "../../test/types.cpp"
        "../../test/chain/block.cpp"
        "../../test/chain/chain_state.cpp"
        "../../test/chain/checkpoint.cpp"
        "../../test/chain/compact.cpp"
        "../../test/chain/context.cpp"
        "../../test/chain/header.cpp"
        "../../test/chain/input.cpp"
        "../../test/chain/operation.cpp"
        "../../test/chain/output.cpp"
        "../../test/chain/point.cpp"
        "../../test/chain/satoshi_words.cpp"
        "../../test/chain/script.cpp"
        "../../test/chain/script.hpp"
        "../../test/chain/stripper.cpp"
        "../../test/chain/transaction.cpp"
        "../../test/chain/witness.cpp"
        "../../test/chain/enums/opcode.cpp"
        "../../test/config/base16.cpp"
        "../../test/config/base2.cpp"
        "../../test/config/base32.cpp"
        "../../test/config/base58.cpp"
        "../../test/config/base64.cpp"
        "../../test/config/base85.cpp"
        "../../test/config/block.cpp"
        "../../test/config/hash256.cpp"
        "../../test/config/parameter.cpp"
        "../../test/config/printer.cpp"
        "../../test/crypto/aes256.cpp"
        "../../test/crypto/elliptic_curve.cpp"
        "../../test/crypto/pseudo_random.cpp"
        "../../test/crypto/ring_signature.cpp"
        "../../test/data/array_cast.cpp"
        "../../test/data/byte_cast.cpp"
        "../../test/data/collection.cpp"
        "../../test/data/data_array.cpp"
        "../../test/data/data_chunk.cpp"
        "../../test/data/data_reference.cpp"
        "../../test/data/data_slab.cpp"
        "../../test/data/data_slice.cpp"
        "../../test/data/exclusive_slice.cpp"
        "../../test/data/external_ptr.cpp"
        "../../test/data/integer.cpp"
        "../../test/data/iterable.cpp"
        "../../test/data/memory.cpp"
        "../../test/data/no_fill_allocator.cpp"
        "../../test/data/string.cpp"
        "../../test/endian/batch.cpp"
        "../../test/endian/integers.cpp"
        "../../test/endian/integrals.cpp"
        "../../test/endian/minimal.cpp"
        "../../test/endian/nominal.cpp"
        "../../test/endian/stream.cpp"
        "../../test/endian/swaps.cpp"
        "../../test/endian/uintx_t.cpp"
        "../../test/endian/unsafe.cpp"
        "../../test/error/block_error_t.cpp"
        "../../test/error/error.cpp"
        "../../test/error/error_t.cpp"
        "../../test/error/op_error_t.cpp"
        "../../test/error/script_error_t.cpp"
        "../../test/error/transaction_error_t.cpp"
        "../../test/hash/accumulator.cpp"
        "../../test/hash/checksum.cpp"
        "../../test/hash/functions.cpp"
        "../../test/hash/hash.hpp"
        "../../test/hash/hmac.cpp"
        "../../test/hash/pbkd.cpp"
        "../../test/hash/scrypt.cpp"
        "../../test/hash/siphash.cpp"
        "../../test/hash/siphash.hpp"
        "../../test/hash/performance/performance.cpp"
        "../../test/hash/performance/performance.hpp"
        "../../test/hash/performance/baseline/byteswap.h"
        "../../test/hash/performance/baseline/common.h"
        "../../test/hash/performance/baseline/endian.h"
        "../../test/hash/performance/baseline/rmd160.cpp"
        "../../test/hash/performance/baseline/rmd160.h"
        "../../test/hash/performance/baseline/sha256.cpp"
        "../../test/hash/performance/baseline/sha256.h"
        "../../test/hash/rmd/algorithm.cpp"
        "../../test/hash/rmd/analysis.cpp"
        "../../test/hash/sha/algorithm.cpp"
        "../../test/hash/sha/analysis.cpp"
        "../../test/hash/sha/sha160.cpp"
        "../../test/hash/sha/sha256.cpp"
        "../../test/hash/sha/sha512.cpp"
        "../../test/hash/sha/vectorization.cpp"
        "../../test/hash/sha/clone/algorithm.hpp"
        "../../test/hash/sha/clone/algorithm.ipp"
        "../../test/hash/sha/clone/algorithm_compression.ipp"
        "../../test/hash/sha/clone/algorithm_vectorization.ipp"
        "../../test/intrinsics/haves.cpp"
        "../../test/intrinsics/intrinsics.cpp"
        "../../test/intrinsics/xcpu/cpuid.cpp"
        "../../test/intrinsics/xcpu/functional.cpp"
        "../../test/machine/interpreter.cpp"
        "../../test/machine/number.cpp"
        "../../test/machine/program.cpp"
        "../../test/math/addition.cpp"
        "../../test/math/bits.cpp"
        "../../test/math/bytes.cpp"
        "../../test/math/byteswap.cpp"
        "../../test/math/cast.cpp"
        "../../test/math/division.cpp"
        "../../test/math/limits.cpp"
        "../../test/math/logarithm.cpp"
        "../../test/math/overflow.cpp"
        "../../test/math/power.cpp"
        "../../test/math/rotate.cpp"
        "../../test/math/sign.cpp"
        "../../test/radix/base_10.cpp"
        "../../test/radix/base_16.cpp"
        "../../test/radix/base_2048.cpp"
        "../../test/radix/base_2n.cpp"
        "../../test/radix/base_32.cpp"
        "../../test/radix/base_58.cpp"
        "../../test/radix/base_64.cpp"
        "../../test/radix/base_85.cpp"
        "../../test/serial/deserialize.cpp"
        "../../test/serial/props.cpp"
        "../../test/serial/serialize.cpp"
        "../../test/stream/binary.cpp"
        "../../test/stream/device.cpp"
        "../../test/stream/stream.cpp"
        "../../test/stream/streamers.cpp"
        "../../test/stream/devices/copy_sink.cpp"
        "../../test/stream/devices/copy_source.cpp"
        "../../test/stream/devices/flip_sink.cpp"
        "../../test/stream/devices/push_sink.cpp"
        "../../test/stream/simple/iostream.cpp"
        "../../test/stream/simple/istream.cpp"
        "../../test/stream/simple/ostream.cpp"
        "../../test/stream/streamers/bit_flipper.cpp"
        "../../test/stream/streamers/bit_reader.cpp"
        "../../test/stream/streamers/bit_writer.cpp"
        "../../test/stream/streamers/byte_flipper.cpp"
        "../../test/stream/streamers/byte_reader.cpp"
        "../../test/stream/streamers/byte_writer.cpp"
        "../../test/stream/streamers/sha256_writer.cpp"
        "../../test/stream/streamers/sha256x2_writer.cpp"
        "../../test/unicode/ascii.cpp"
        "../../test/unicode/code_points.cpp"
        "../../test/unicode/conversion.cpp"
        "../../test/unicode/normalization.cpp"
        "../../test/unicode/utf8_everywhere/environment.cpp"
        "../../test/unicode/utf8_everywhere/ifstream.cpp"
        "../../test/unicode/utf8_everywhere/ofstream.cpp"
        "../../test/unicode/utf8_everywhere/unicode_istream.cpp"
        "../../test/unicode/utf8_everywhere/unicode_ostream.cpp"
        "../../test/wallet/context.cpp"
        "../../test/wallet/message.cpp"
        "../../test/wallet/neutrino_filter.cpp"
        "../../test/wallet/point_value.cpp"
        "../../test/wallet/points_value.cpp"
        "../../test/wallet/addresses/bitcoin_uri.cpp"
        "../../test/wallet/addresses/checked.cpp"
        "../../test/wallet/addresses/payment_address.cpp"
        "../../test/wallet/addresses/qr_code.cpp"
        "../../test/wallet/addresses/stealth_address.cpp"
        "../../test/wallet/addresses/stealth_receiver.cpp"
        "../../test/wallet/addresses/stealth_sender.cpp"
        "../../test/wallet/addresses/tiff.cpp"
        "../../test/wallet/addresses/uri.cpp"
        "../../test/wallet/addresses/uri_reader.cpp"
        "../../test/wallet/addresses/witness_address.cpp"
        "../../test/wallet/keys/ec_point.cpp"
        "../../test/wallet/keys/ec_private.cpp"
        "../../test/wallet/keys/ec_public.cpp"
        "../../test/wallet/keys/ec_scalar.cpp"
        "../../test/wallet/keys/encrypted_keys.cpp"
        "../../test/wallet/keys/hd_private.cpp"
        "../../test/wallet/keys/hd_public.cpp"
        "../../test/wallet/keys/mini_keys.cpp"
        "../../test/wallet/keys/stealth.cpp"
        "../../test/wallet/mnemonics/electrum.cpp"
        "../../test/wallet/mnemonics/electrum.hpp"
        "../../test/wallet/mnemonics/electrum_v1.cpp"
        "../../test/wallet/mnemonics/electrum_v1.hpp"
        "../../test/wallet/mnemonics/mnemonic.cpp"
        "../../test/wallet/mnemonics/mnemonic.hpp"
        "../../test/words/dictionaries.cpp"
        "../../test/words/dictionaries.hpp"
        "../../test/words/dictionary.cpp"
        "../../test/words/dictionary.hpp"
        "../../test/words/languages.cpp"
        "../../test/words/languages.hpp"
        "../../test/words/catalogs/electrum.cpp"
        "../../test/words/catalogs/electrum.hpp"
        "../../test/words/catalogs/electrum_v1.cpp"
        "../../test/words/catalogs/electrum_v1.hpp"
        "../../test/words/catalogs/mnemonic.cpp"
        "../../test/words/catalogs/mnemonic.hpp" )

    add_test( NAME libbitcoin-system-test COMMAND libbitcoin-system-test
            --run_test=*
            --show_progress=no
            --detect_memory_leak=0
            --report_level=no
            --build_info=yes )

#     libbitcoin-system-test project specific include directories.
#------------------------------------------------------------------------------
    target_include_directories( libbitcoin-system-test PRIVATE
        "../../include" )

#     libbitcoin-system-test project specific libraries/linker flags.
#------------------------------------------------------------------------------
    target_link_libraries( libbitcoin-system-test
        ${CANONICAL_LIB_NAME}
        ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY} )

endif()

# Manage pkgconfig installation.
#------------------------------------------------------------------------------
configure_file(
  "../../libbitcoin-system.pc.in"
  "libbitcoin-system.pc" @ONLY )

install( FILES
    "${CMAKE_CURRENT_BINARY_DIR}/libbitcoin-system.pc"
    DESTINATION "${pkgconfigdir}" )

# Manage installation of docs.
#------------------------------------------------------------------------------
install( FILES
    "../../AUTHORS"
    "../../COPYING"
    "../../ChangeLog"
    "../../INSTALL"
    "../../NEWS"
    "../../README"
    DESTINATION share/doc/libbitcoin-system )

# Manage ${CANONICAL_LIB_NAME} installation.
#------------------------------------------------------------------------------
install( TARGETS ${CANONICAL_LIB_NAME}
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    PUBLIC_HEADER DESTINATION include )

# Manage include installation.
#------------------------------------------------------------------------------
install( DIRECTORY "../../include/bitcoin"
    DESTINATION include )

