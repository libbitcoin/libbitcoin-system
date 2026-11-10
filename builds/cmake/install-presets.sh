#!/bin/bash
###############################################################################
#  Copyright (c) 2014-2026 libbitcoin-system developers (see COPYING).
#
#         GENERATED SOURCE CODE, DO NOT EDIT EXCEPT EXPERIMENTALLY
#
###############################################################################
# Script managing the build and installation of libbitcoin-system and its dependencies.
#
# Script options:

# --build-libsecp256k1         Build bitcoin core libsecp256k1 libraries
# --build-preset=<preset>      Specifies preset configuration to build.
# --build-src-dir=<path>       Location for sources.
#                                Default: $(pwd)
# --build-limit=<limit>        Limitation at which the process will terminate.
#                                Values: { none, target, dependencies, external }
#                                Default: none
# --build-full-repositories    Sync full github repositories.
#                                Default: git clone --depth 1 --single-branch
# --build-post-install-clean   Clean dependencies after installation (saves space).
# --build-skip-tests           Skip test compilation and execution.
# --build-skip-test-run        Compile tests without executing them.
# --build-parallel=<int>       Number of jobs to run simultaneously.
#                                Default: supported platforms use nproc/sysctl
# --build-use-local-src        Use existing sources in relevant paths.
# --hardware-optimized         Inform compiler to apply applicable optimizations for the host hardware.
# --noninteractive             Disable any prompt using default.
# --verbose                    Display verbose script output.
# --help, -h                   Display usage, overriding script execution.
#
# Hardware configuration options for arm64:
# --hardware-aes               Use ARM AES and PMULL Extensions.
# --hardware-sha2              Use ARM SHA1/SHA256 Extensions.
# --hardware-sha3              Use ARM SHA3 Extensions for SHA512.
#
# Hardware configuration options for x64:
# --hardware-avx2              Use Intel AVX2 intrinsics.
# --hardware-avx512            Use Intel AVX512 intrinsics.
# --hardware-avx512ifma        Use Intel AVX512 IFMA intrinsics (implies AVX512VL).
# --hardware-avxifma           Use Intel AVX IFMA intrinsics (implies AVX2).
# --hardware-shani             Use Intel SHA Extensions.
# --hardware-sha512            Use Intel SHA512 Extensions (implies AVX2).
# --hardware-sse41             Use SSE4.1 hardware instructions.
# --hardware-aesni             Use Intel AES-NI and PCLMULQDQ intrinsics.
# --hardware-vaes              Use Intel VAES and VPCLMULQDQ intrinsics (implies AES-NI and AVX2).
#
# Recognized pass-through options:
# -Denable-shani=<ON/OFF>      Add Intel SHA Extensions via compilation options.
#                                Default: OFF
# -Denable-sha512=<ON/OFF>     Add Intel SHA512 Extensions (implies AVX2) via compilation options.
#                                Default: OFF
# -Denable-aesni=<ON/OFF>      Add Intel AES-NI and PCLMULQDQ intrinsics via compilation options.
#                                Default: OFF
# -Denable-vaes=<ON/OFF>       Add Intel VAES and VPCLMULQDQ intrinsics (implies AES-NI and AVX2) via compilation options.
#                                Default: OFF
# -Denable-avx512ifma=<ON/OFF> Add Intel AVX512 IFMA intrinsics (implies AVX512VL) via compilation options.
#                                Default: OFF
# -Denable-avx512=<ON/OFF>     Add Intel AVX512 intrinsics via compilation options.
#                                Default: OFF
# -Denable-avxifma=<ON/OFF>    Add Intel AVX IFMA intrinsics (implies AVX2) via compilation options.
#                                Default: OFF
# -Denable-avx2=<ON/OFF>       Add Intel AVX2 intrinsics via compilation options.
#                                Default: OFF
# -Denable-sse41=<ON/OFF>      Add SSE4.1 hardware instructions via compilation options.
#                                Default: OFF
# -Denable-sha2=<ON/OFF>       Check for ARM SHA1/SHA256 Extensions.
#                                Default: OFF
# -Denable-sha3=<ON/OFF>       Check for ARM SHA3 Extensions for SHA512.
#                                Default: OFF
# -Denable-aes=<ON/OFF>        Check for ARM AES and PMULL Extensions.
#                                Default: OFF
# -Dwith-secp256k1=<ON/OFF>    Use native secp256k1 interface, otherwise external library.
#                                Default: ON
# -Dwith-valgrind=<ON/OFF>     Use valgrind memcheck client requests.
#                                Default: OFF
# -Dwith-cuda=<ON/OFF>         Use cuda (gpu) batch signature verification (inert on macOS).
#                                Default: ON
# -Dwith-performance-tests=<ON/OFF> Compile performance tests.
#                                Default: OFF
# -Dwith-slow-tests=<ON/OFF>   Compile slow tests.
#                                Default: OFF
#
# All unrecognized options provided shall be passed as configuration
# options for all dependencies.

OPTS_ENABLE="set -eo pipefail"
OPTS_DISABLE="set +e"

eval "${OPTS_ENABLE}"
trap 'echo "FATAL ERROR: Command failed at line ${LINENO}: ${BASH_COMMAND}" >&2' ERR

SEQUENTIAL=1

if [[ -z ${secp256k1_OWNER} ]]; then
    secp256k1_OWNER="bitcoin-core"
fi
if [[ -z ${secp256k1_TAG} ]]; then
    secp256k1_TAG="v0.8.0"
fi

if [[ -z ${libbitcoin_system_OWNER} ]]; then
    libbitcoin_system_OWNER="libbitcoin"
fi
if [[ -z ${libbitcoin_system_TAG} ]]; then
    libbitcoin_system_TAG="master"
fi

main()
{
    # argument consumption

    for OPTION in "$@"; do
        case ${OPTION} in
            (--build-libsecp256k1)          BUILD_libsecp256k1="yes";;
            (--hardware-aes)                HARDWARE_AES="yes";;
            (--hardware-sha2)               HARDWARE_SHA2="yes";;
            (--hardware-sha3)               HARDWARE_SHA3="yes";;
            (--hardware-avx2)               HARDWARE_AVX2="yes";;
            (--hardware-avx512)             HARDWARE_AVX512="yes";;
            (--hardware-avx512ifma)         HARDWARE_AVX512IFMA="yes";;
            (--hardware-avxifma)            HARDWARE_AVXIFMA="yes";;
            (--hardware-shani)              HARDWARE_SHANI="yes";;
            (--hardware-sha512)             HARDWARE_SHA512="yes";;
            (--hardware-sse41)              HARDWARE_SSE41="yes";;
            (--hardware-aesni)              HARDWARE_AESNI="yes";;
            (--hardware-vaes)               HARDWARE_VAES="yes";;
            (--build-preset=*)              BUILD_PRESET="${OPTION#*=}";;
            (--build-src-dir=*)             BUILD_SRC_DIR="${OPTION#*=}";;
            (--build-limit=*)               BUILD_LIMIT="${OPTION#*=}";;
            (--build-full-repositories)     BUILD_FULL_REPOSITORIES="yes";;
            (--build-post-install-clean)    BUILD_POST_INSTALL_CLEAN="yes";;
            (--build-skip-tests)            BUILD_SKIP_TESTS="yes";;
            (--build-skip-test-run)         BUILD_SKIP_TEST_RUN="yes";;
            (--build-parallel=*)            PARALLEL="${OPTION#*=}";;
            (--build-use-local-src)         BUILD_USE_LOCAL_SRC="yes";;
            (--hardware-optimized)          HARDWARE_OPTIMIZED="yes";;
            (--noninteractive)              NONINTERACTIVE="yes";;
            (--verbose)                     DISPLAY_VERBOSE="yes";;
            (--help|-h)                     DISPLAY_HELP="yes";;
            (-DCMAKE_PREFIX_PATH=*)         CMAKE_PREFIX_PATH="${OPTION#*=}";;
            (-DCMAKE_INSTALL_PREFIX=*)      CMAKE_INSTALL_PREFIX="${OPTION#*=}";;
            (-DCMAKE_INCLUDE_PATH=*)        CMAKE_INCLUDE_PATH="${OPTION#*=}";;
            (-DCMAKE_LIBRARY_PATH=*)        CMAKE_LIBRARY_PATH="${OPTION#*=}";;
        esac
    done

    CONFIGURE_OPTIONS_ORIGINAL=("$@")
    CONFIGURE_OPTIONS=("$@")
    remove_configure_option "--build-libsecp256k1"
    remove_configure_option "--build-preset=*"
    remove_configure_option "--build-src-dir=*"
    remove_configure_option "--build-limit=*"
    remove_configure_option "--build-full-repositories"
    remove_configure_option "--build-post-install-clean"
    remove_configure_option "--build-skip-tests"
    remove_configure_option "--build-skip-test-run"
    remove_configure_option "--build-parallel=*"
    remove_configure_option "--build-use-local-src"
    remove_configure_option "--hardware-optimized"
    remove_configure_option "--noninteractive"
    remove_configure_option "--verbose"
    remove_configure_option "--help"
    remove_configure_option "-h"
    remove_configure_option "--hardware-aes"
    remove_configure_option "--hardware-sha2"
    remove_configure_option "--hardware-sha3"
    remove_configure_option "--hardware-avx2"
    remove_configure_option "--hardware-avx512"
    remove_configure_option "--hardware-avx512ifma"
    remove_configure_option "--hardware-avxifma"
    remove_configure_option "--hardware-shani"
    remove_configure_option "--hardware-sha512"
    remove_configure_option "--hardware-sse41"
    remove_configure_option "--hardware-aesni"
    remove_configure_option "--hardware-vaes"
    CONFIGURE_OPTIONS_CMAKE=("${CONFIGURE_OPTIONS[@]}")
    msg_verbose "*** ARGUMENTS: ${CONFIGURE_OPTIONS_ORIGINAL[*]}"
    msg_verbose "*** SANITIZED: ${CONFIGURE_OPTIONS[*]}"

    if [[ "${DISPLAY_VERBOSE}" == "yes" ]]; then
        display_build_variables
    fi

    # handle help
    if [[ "${DISPLAY_HELP}" == "yes" ]]; then
        help
        return 0
    fi


    OS=$(uname -s)

    # defaults and sanitization: script static options

    # --build-preset
    if [[ -z "${BUILD_PRESET}" ]]; then
        msg_error "No --build-preset specified."
        help
        exit 1
    fi

    if [[ "${BUILD_PRESET}" == "nix-gnu-debug-static" ]]; then
        msg_verbose "BUILD_PRESET value 'nix-gnu-debug-static'."
        BUILD_CONFIG="debug"
        BUILD_LINK="static"
    elif [[ "${BUILD_PRESET}" == "nix-gnu-debug-shared" ]]; then
        msg_verbose "BUILD_PRESET value 'nix-gnu-debug-shared'."
        BUILD_CONFIG="debug"
        BUILD_LINK="dynamic"
    elif [[ "${BUILD_PRESET}" == "nix-gnu-release-static" ]]; then
        msg_verbose "BUILD_PRESET value 'nix-gnu-release-static'."
        BUILD_CONFIG="release"
        BUILD_LINK="static"
    elif [[ "${BUILD_PRESET}" == "nix-gnu-release-shared" ]]; then
        msg_verbose "BUILD_PRESET value 'nix-gnu-release-shared'."
        BUILD_CONFIG="release"
        BUILD_LINK="dynamic"
    elif [[ "${BUILD_PRESET}" == "win-msvc-debug-static" ]]; then
        msg_verbose "BUILD_PRESET value 'win-msvc-debug-static'."
        BUILD_CONFIG="debug"
        BUILD_LINK="static"
    elif [[ "${BUILD_PRESET}" == "win-msvc-release-static" ]]; then
        msg_verbose "BUILD_PRESET value 'win-msvc-release-static'."
        BUILD_CONFIG="release"
        BUILD_LINK="static"
    elif [[ "${BUILD_PRESET}" == "win-msvc-debug-shared" ]]; then
        msg_verbose "BUILD_PRESET value 'win-msvc-debug-shared'."
        BUILD_CONFIG="debug"
        BUILD_LINK="dynamic"
    elif [[ "${BUILD_PRESET}" == "win-msvc-release-shared" ]]; then
        msg_verbose "BUILD_PRESET value 'win-msvc-release-shared'."
        BUILD_CONFIG="release"
        BUILD_LINK="dynamic"
    elif [[ "${BUILD_PRESET}" == "win-clangcl-debug-static" ]]; then
        msg_verbose "BUILD_PRESET value 'win-clangcl-debug-static'."
        BUILD_CONFIG="debug"
        BUILD_LINK="static"
    elif [[ "${BUILD_PRESET}" == "win-clangcl-release-static" ]]; then
        msg_verbose "BUILD_PRESET value 'win-clangcl-release-static'."
        BUILD_CONFIG="release"
        BUILD_LINK="static"
    elif [[ "${BUILD_PRESET}" == "win-clangcl-debug-shared" ]]; then
        msg_verbose "BUILD_PRESET value 'win-clangcl-debug-shared'."
        BUILD_CONFIG="debug"
        BUILD_LINK="dynamic"
    elif [[ "${BUILD_PRESET}" == "win-clangcl-release-shared" ]]; then
        msg_verbose "BUILD_PRESET value 'win-clangcl-release-shared'."
        BUILD_CONFIG="release"
        BUILD_LINK="dynamic"
    else
        msg_error "Unrecognized build-preset value '${BUILD_PRESET}'."
        help
        exit 1
    fi

    # --build-src-dir
    if [[ -z "${BUILD_SRC_DIR}" ]]; then
        BUILD_SRC_DIR="$(pwd)"
        msg_verbose "No build-src-dir specified, using default '${BUILD_SRC_DIR}'."
    fi

    if ! [[ -d "${BUILD_SRC_DIR}" ]]; then
        create_directory "${BUILD_SRC_DIR}"
        msg_verbose "Created --build-src-dir '${BUILD_SRC_DIR}'."
    fi

    push_directory "${BUILD_SRC_DIR}"
    BUILD_SRC_DIR="$(pwd)"
    pop_directory
    msg_verbose "Determined absolute path for --build-src-dir '${BUILD_SRC_DIR}'."
    # Embedded knowlege of CMakePresets: binaryDir

    BUILD_OBJ_DIR="obj/${BUILD_PRESET}"
    BUILD_OBJ_DIR_RELATIVE="yes"
    PREFIX="${BUILD_SRC_DIR}/prefix/${BUILD_PRESET}"

    # --build-obj-dir
    if [[ -z "${BUILD_OBJ_DIR}" ]]; then
        if [[ "${BUILD_OBJ_DIR_RELATIVE}" == "yes" ]]; then
            BUILD_OBJ_DIR="obj"
            msg_verbose "No --build-obj-dir specified, using relative default '${BUILD_OBJ_DIR}'."
        else
            BUILD_OBJ_DIR="$(pwd)/obj"
            msg_verbose "No --build-obj-dir specified, using default '${BUILD_OBJ_DIR}'."
        fi
    fi

    if [[ "${BUILD_OBJ_DIR_RELATIVE}" == "yes" ]]; then
        msg_verbose "Deferring relative path action for --build-obj-dir '${BUILD_OBJ_DIR}'."
    else
        if ! [[ -d "${BUILD_OBJ_DIR}" ]]; then
            create_directory "${BUILD_OBJ_DIR}"
            msg_verbose "Created --build-obj-dir '${BUILD_OBJ_DIR}'."
        fi

        push_directory "${BUILD_OBJ_DIR}"
        BUILD_OBJ_DIR="$(pwd)"
        pop_directory
        msg_verbose "Determined absolute path for --build-obj-dir '${BUILD_OBJ_DIR}'."
    fi

    # --build-limit
    if [[ -z "${BUILD_LIMIT}" ]]; then
        BUILD_LIMIT="none"
        msg_verbose "No --build-limit specified."
    elif [[ "${BUILD_LIMIT}" != "none" ]] &&
        [[ "${BUILD_LIMIT}" != "target" ]] &&
        [[ "${BUILD_LIMIT}" != "dependencies" ]] &&
        [[ "${BUILD_LIMIT}" != "external" ]]; then
        msg_error "Provided --build-limit '${BUILD_LIMIT}' not a valid value."
        help
        exit 1
    else
        msg_verbose "Using provided --build-limit '${BUILD_LIMIT}'"
    fi

    # --build-config
    if [[ -z "${BUILD_CONFIG}" ]]; then
        msg_verbose "No --build-config specified."
    elif [[ "${BUILD_CONFIG}" != "debug" ]] && [[ "${BUILD_CONFIG}" != "release" ]]; then
        msg_error "Provided --build-config '${BUILD_CONFIG}' not a valid value."
        help
        exit 1
    else
        msg_verbose "Using provided --build-config '${BUILD_CONFIG}'"
    fi

    # --build-link
    if [[ -z "${BUILD_LINK}" ]]; then
        msg_verbose "No --build-link specified."
    elif [[ "${BUILD_LINK}" != "dynamic" ]] && [[ "${BUILD_LINK}" != "static" ]]; then
        msg_error "Provided --build-link '${BUILD_LINK}' not a valid value."
        help
        exit 1
    fi

    if [[ "${BUILD_SKIP_TESTS}" == "yes" ]]; then
        CONFIGURE_OPTIONS_CMAKE=( "${CONFIGURE_OPTIONS_CMAKE[@]}" "-Dwith-tests=OFF" )
    fi

    # --prefix
    if [[ -z "${PREFIX}" ]]; then
        # Always set a prefix (required for OSX and lib detection).
        PREFIX="/usr/local"
        msg_verbose "No --prefix specified, defaulting to '${PREFIX}'."
    else
        # Incorporate the custom libdir into each object, for link time resolution
        if [[ "${BUILD_LINK}" == "dynamic" ]]; then
            if [[ -z "${LD_RUN_PATH}" ]]; then
                export LD_RUN_PATH="${PREFIX}/lib"
            else
                export LD_RUN_PATH="${LD_RUN_PATH}:${PREFIX}/lib"
            fi

            if [[ -z "${LD_LIBRARY_PATH}" ]]; then
                export LD_LIBRARY_PATH="${PREFIX}/lib"
            else
                export LD_LIBRARY_PATH="${LD_LIBRARY_PATH}:${PREFIX}/lib"
            fi
        fi
    fi

    if [[ -n "${PREFIX}" ]] && [[ -n "${CMAKE_INSTALL_PREFIX}" ]]; then
        msg_error "Both PREFIX and CMAKE_INSTALL_PREFIX have been defined differently."
        help
        exit 1
    fi

    if [[ -n "${PREFIX}" ]]; then
        CMAKE_INSTALL_PREFIX="${PREFIX}"
        CONFIGURE_OPTIONS_CMAKE=( "${CONFIGURE_OPTIONS_CMAKE[@]/-DCMAKE_INSTALL_PREFIX=*/}" )
        CONFIGURE_OPTIONS_CMAKE=( "${CONFIGURE_OPTIONS_CMAKE[@]}" "-DCMAKE_INSTALL_PREFIX=${PREFIX}" )
    fi

    if [[ -n "${PREFIX}" ]] && [[ -n "${CMAKE_PREFIX_PATH}" ]] &&
       [[ "${PREFIX}" != "${CMAKE_PREFIX_PATH}" ]]; then
        CMAKE_PREFIX_PATH="${PREFIX}:${CMAKE_PREFIX_PATH}"
        CONFIGURE_OPTIONS_CMAKE=( "${CONFIGURE_OPTIONS_CMAKE[@]/-DCMAKE_PREFIX_PATH=*/}" )
        CONFIGURE_OPTIONS_CMAKE=( "${CONFIGURE_OPTIONS_CMAKE[@]}" "-DCMAKE_PREFIX_PATH=${CMAKE_PREFIX_PATH}" )
    elif [[ -n "${PREFIX}" ]]; then
        CMAKE_INSTALL_PREFIX="${PREFIX}"
        CONFIGURE_OPTIONS_CMAKE=( "${CONFIGURE_OPTIONS_CMAKE[@]/-DCMAKE_PREFIX_PATH=*/}" )
        CONFIGURE_OPTIONS_CMAKE=( "${CONFIGURE_OPTIONS_CMAKE[@]}" "-DCMAKE_PREFIX_PATH=${PREFIX}" )
    fi

    if [[ -n "${PREFIX}" ]]; then
        # Set the prefix-based package config directory.
        PREFIX_PKG_CONFIG_PATH="${PREFIX}/lib/pkgconfig"

        # Prioritize prefix package config in PKG_CONFIG_PATH search path.
        if [[ -n "${PKG_CONFIG_PATH}" ]]; then
            export PKG_CONFIG_PATH="${PREFIX_PKG_CONFIG_PATH}:${PKG_CONFIG_PATH}"
        else
            export PKG_CONFIG_PATH="${PREFIX_PKG_CONFIG_PATH}"
        fi

        with_pkgconfigdir="--with-pkgconfigdir=${PREFIX_PKG_CONFIG_PATH}"
    fi

    # --parallel
    if [[ -z "${PARALLEL}" ]]; then
        if [[ ${OS} == Linux ]]; then
            PARALLEL=$(nproc)
        elif [[ ${OS} == Darwin ]] || [[ ${OS} == *BSD ]]; then
            PARALLEL=$(sysctl -n hw.ncpu)
        elif [[ ${OS} == MINGW* ]] || [[ ${OS} == MSYS* ]]; then
            PARALLEL=${NUMBER_OF_PROCESSORS}
        else
            msg_error "Unsupported system: '${OS}'"
            msg_error "  Unable to determine value for --build-parallel"
            msg_error "  Please specify explicitly to continue."
            msg_error ""
            help
            exit 1
        fi
    fi

    # state rationalization of standard build variables

    if [[ ${OS} == *BSD ]]; then
        make() { gmake "$@"; }
    fi

    if [[ -z "${STDLIB}" ]]; then
        if [[ ${OS} == Darwin ]]; then
            STDLIB="c++"
        else
            STDLIB="stdc++"
        fi
    else
        msg_verbose "STDLIB using defined value '${STDLIB}'"
    fi

    if [[ -z "${CC}" ]]; then
        if [[ ${OS} == Darwin ]]; then
            export CC="clang"
        fi
    else
        msg_verbose "CC using defined value '${CC}'"
    fi

    if [[ -z "${CXX}" ]]; then
        if [[ ${OS} == Darwin ]]; then
            export CXX="clang++"
        fi
    else
        msg_verbose "CXX using defined value '${CXX}'"
    fi

    if [[ -z "${MACOSX_DEPLOYMENT_TARGET}" ]]; then
        if [[ ${OS} == Darwin ]]; then
            export MACOSX_DEPLOYMENT_TARGET=$(sw_vers -productVersion)
        fi
    else
        msg_verbose "MACOSX_DEPLOYMENT_TARGET using defined value '${MACOSX_DEPLOYMENT_TARGET}'"
    fi

    # HARDWARE_OPTIMIZED
    if [[ "${HARDWARE_OPTIMIZED}" == "yes" ]]; then
        ARCHTUNE="-march=native -mtune=native"
    fi

    # Platform options: arm64

    if [[ "${HARDWARE_AES}" == "yes" ]]; then
        HARDWARE_PLATFORM_ARM64="yes"
        ARCHBASE="armv8-a"
        ARCHSUFFIX="${ARCHSUFFIX}+aes"
        CONFIGURE_OPTIONS_CMAKE=( "${CONFIGURE_OPTIONS_CMAKE[@]}" "-Denable-aes=ON" )
    fi

    if [[ "${HARDWARE_SHA2}" == "yes" ]]; then
        HARDWARE_PLATFORM_ARM64="yes"
        ARCHBASE="armv8-a"
        ARCHSUFFIX="${ARCHSUFFIX}+sha2"
        CONFIGURE_OPTIONS_CMAKE=( "${CONFIGURE_OPTIONS_CMAKE[@]}" "-Denable-sha2=ON" )
    fi

    if [[ "${HARDWARE_SHA3}" == "yes" ]]; then
        HARDWARE_PLATFORM_ARM64="yes"
        ARCHBASE="armv8.2-a"
        ARCHSUFFIX="${ARCHSUFFIX}+sha3"
        CONFIGURE_OPTIONS_CMAKE=( "${CONFIGURE_OPTIONS_CMAKE[@]}" "-Denable-sha3=ON" )
    fi

    if [[ ${HARDWARE_PLATFORM_ARM64} == "yes" ]]; then
        ARCHTUNE="-march=${ARCHBASE}${ARCHSUFFIX}"
    fi

    # Platform options: x64

    if [[ "${HARDWARE_AVX2}" == "yes" ]]; then
        HARDWARE_PLATFORM_X64="yes"
        CONFIGURE_OPTIONS_CMAKE=( "${CONFIGURE_OPTIONS_CMAKE[@]}" "-Denable-avx2=ON" )
    fi

    if [[ "${HARDWARE_AVX512}" == "yes" ]]; then
        HARDWARE_PLATFORM_X64="yes"
        CONFIGURE_OPTIONS_CMAKE=( "${CONFIGURE_OPTIONS_CMAKE[@]}" "-Denable-avx512=ON" )
    fi

    if [[ "${HARDWARE_AVX512IFMA}" == "yes" ]]; then
        HARDWARE_PLATFORM_X64="yes"
        CONFIGURE_OPTIONS_CMAKE=( "${CONFIGURE_OPTIONS_CMAKE[@]}" "-Denable-avx512ifma=ON" )
    fi

    if [[ "${HARDWARE_AVXIFMA}" == "yes" ]]; then
        HARDWARE_PLATFORM_X64="yes"
        CONFIGURE_OPTIONS_CMAKE=( "${CONFIGURE_OPTIONS_CMAKE[@]}" "-Denable-avxifma=ON" )
    fi

    if [[ "${HARDWARE_SHANI}" == "yes" ]]; then
        HARDWARE_PLATFORM_X64="yes"
        CONFIGURE_OPTIONS_CMAKE=( "${CONFIGURE_OPTIONS_CMAKE[@]}" "-Denable-shani=ON" )
    fi

    if [[ "${HARDWARE_SHA512}" == "yes" ]]; then
        HARDWARE_PLATFORM_X64="yes"
        CONFIGURE_OPTIONS_CMAKE=( "${CONFIGURE_OPTIONS_CMAKE[@]}" "-Denable-sha512=ON" )
    fi

    if [[ "${HARDWARE_SSE41}" == "yes" ]]; then
        HARDWARE_PLATFORM_X64="yes"
        CONFIGURE_OPTIONS_CMAKE=( "${CONFIGURE_OPTIONS_CMAKE[@]}" "-Denable-sse41=ON" )
    fi

    if [[ "${HARDWARE_AESNI}" == "yes" ]]; then
        HARDWARE_PLATFORM_X64="yes"
        CONFIGURE_OPTIONS_CMAKE=( "${CONFIGURE_OPTIONS_CMAKE[@]}" "-Denable-aesni=ON" )
    fi

    if [[ "${HARDWARE_VAES}" == "yes" ]]; then
        HARDWARE_PLATFORM_X64="yes"
        CONFIGURE_OPTIONS_CMAKE=( "${CONFIGURE_OPTIONS_CMAKE[@]}" "-Denable-vaes=ON" )
    fi

    HARDWARE_PLATFORM_COUNT=0
    if [[ ${HARDWARE_PLATFORM_ARM64} == "yes" ]]; then
        HARDWARE_PLATFORM_COUNT=$((HARDWARE_PLATFORM_COUNT + 1))
    fi
    if [[ ${HARDWARE_PLATFORM_X64} == "yes" ]]; then
        HARDWARE_PLATFORM_COUNT=$((HARDWARE_PLATFORM_COUNT + 1))
    fi

    if [ ${HARDWARE_PLATFORM_COUNT} -gt 1 ]; then
        msg_error "Hardware options spanning more than one platform have been specified."
        exit 1
    fi

    if [[ "${HARDWARE_OPTIMIZED}" == "yes" ]] &&
        [ ${HARDWARE_PLATFORM_COUNT} -gt 0 ]; then
        msg_error "Hardware optimization cannot be reliably rationalized with hardware platform options."
        exit 1
    fi


    # Specify cmake build
    if [[ "${BUILD_CONFIG}" == "debug" ]]; then
        CONFIGURE_OPTIONS_CMAKE=( "${CONFIGURE_OPTIONS_CMAKE[@]}" "-DCMAKE_BUILD_TYPE=Debug" )
    elif [[ "${BUILD_CONFIG}" == "release" ]]; then
        CONFIGURE_OPTIONS_CMAKE=( "${CONFIGURE_OPTIONS_CMAKE[@]}" "-DCMAKE_BUILD_TYPE=Release" )
    fi

    # translate BUILD_LINK to appropriate arguments
    if [[ -n "${BUILD_LINK}" ]]; then
        CONFIGURE_OPTIONS_CMAKE=( "${CONFIGURE_OPTIONS_CMAKE[@]/-DBUILD_SHARED_LIBS=*}" )

        if [[ "${BUILD_LINK}" == "dynamic" ]]; then
            CONFIGURE_OPTIONS_CMAKE=( "${CONFIGURE_OPTIONS_CMAKE[@]}" "-DBUILD_SHARED_LIBS=ON" )
        else
            CONFIGURE_OPTIONS_CMAKE=( "${CONFIGURE_OPTIONS_CMAKE[@]}" "-DBUILD_SHARED_LIBS=OFF" )
        fi
    fi

    if [[ ${OS} == Linux && ${CC} == clang* ]]; then
        export LDLIBS="-l${STDLIB} ${LDLIBS}"
        msg_verbose "LDLIBS has been manipulated to encode STDLIB linkage."
    fi

    # defaults and sanitization: generated options


    REMAP=()
    for argument in "${CONFIGURE_OPTIONS_CMAKE[@]}"; do
        if [[ -n "${argument}" ]]; then
            REMAP+=( "${argument}" )
        fi
    done
    CONFIGURE_OPTIONS_CMAKE=( "${REMAP[@]}" )
    unset REMAP

    msg_heading "Configuration"
    display_build_variables

    msg_heading "Toolchain Configuration Parameters"
    display_toolchain_variables

    if [[ "${DISPLAY_VERBOSE}" == "yes" ]]; then
        msg_heading "State"
        display_constants
    fi

    # declarations

    secp256k1_FLAGS=(
        "-w")

    secp256k1_OPTIONS=(
        "-DSECP256K1_BUILD_TESTS=OFF"
        "-DSECP256K1_EXPERIMENTAL=ON"
        "-DSECP256K1_ENABLE_MODULE_ELLSWIFT=ON"
        "-DSECP256K1_ENABLE_MODULE_RECOVERY=ON"
        "-DSECP256K1_ENABLE_MODULE_SCHNORRSIG=ON")

    libbitcoin_system_FLAGS=()

    libbitcoin_system_OPTIONS=()

    if [[ ${BUILD_LIMIT} == "external" ]] ||
       [[ ${BUILD_LIMIT} == "dependencies" ]] ||
       [[ ${BUILD_LIMIT} == "none" ]]; then
        if [[ ${BUILD_libsecp256k1} == "yes" ]]; then
            source_github "${secp256k1_OWNER}" "secp256k1" "${secp256k1_TAG}"
            local SAVE_CPPFLAGS="${CPPFLAGS}"
            export CPPFLAGS="${CPPFLAGS} ${secp256k1_FLAGS[@]}"
            build_cmake "secp256k1" "." "." "${PARALLEL}" "${secp256k1_OPTIONS[@]}" "${CONFIGURE_OPTIONS_CMAKE[@]}"
            install_cmake "secp256k1" "."
            if [[ "${BUILD_POST_INSTALL_CLEAN}" == "yes" ]]; then
                clean_cmake "secp256k1" "."
            fi
            export CPPFLAGS="${SAVE_CPPFLAGS}"
        fi
    fi

    if [[ ${BUILD_LIMIT} == "target" ]] ||
       [[ ${BUILD_LIMIT} == "none" ]]; then
        source_github "${libbitcoin_system_OWNER}" "libbitcoin-system" "${libbitcoin_system_TAG}"
        local SAVE_CPPFLAGS="${CPPFLAGS}"
        export CPPFLAGS="${CPPFLAGS} ${libbitcoin_system_FLAGS[@]}"
        build_preset "libbitcoin-system" "builds/cmake" "." "${PARALLEL}" "${libbitcoin_system_OPTIONS[@]}" "${CONFIGURE_OPTIONS_CMAKE[@]}"
        if ! [[ "${BUILD_SKIP_TESTS}" == "yes" || "${BUILD_SKIP_TEST_RUN}" == "yes" ]]; then
            test_cmake "libbitcoin-system" "." "${PARALLEL}"
        fi
        install_cmake "libbitcoin-system" "."
        if [[ "${BUILD_POST_INSTALL_CLEAN}" == "yes" ]]; then
            clean_cmake "libbitcoin-system" "."
        fi
        export CPPFLAGS="${SAVE_CPPFLAGS}"
    fi

    msg_success "Completed successfully."
}

github_repository_status()
{
    local REPOSITORY="$1"

    push_directory "${REPOSITORY}"
    local DISCOVERED_COMMIT=$(git rev-parse HEAD)
    local DISCOVERED_URL=$(git config --get remote.origin.url)
    pop_directory # REPOSITORY
    msg_warn "  commit : ${DISCOVERED_COMMIT}"
    msg_warn "  url    : ${DISCOVERED_URL}"
}

source_github()
{
    local OWNER="$1"
    local REPOSITORY="$2"
    local TAG="$3"
    shift 3

    msg_heading "Preparing to acquire ${OWNER}/${REPOSITORY}/${TAG}"

    local GIT_CLONE="git clone"
    local CLONE_OPTIONS="--depth 1 --single-branch"

    if [[ "${BUILD_FULL_REPOSITORIES}" == "yes" ]]; then
        CLONE_OPTIONS=""
    fi

    push_directory "${BUILD_SRC_DIR}"

    if [ -d "${REPOSITORY}" ] &&
       [[ "${BUILD_USE_LOCAL_SRC}" == "yes" ]]; then
        msg_warn "Reusing existing '${REPOSITORY}'..."
        github_repository_status "${REPOSITORY}"
        pop_directory # BUILD_SRC_DIR
        return 0
    fi

    if [ -d "${REPOSITORY}" ]; then
        msg_warn "Encountered existing '${REPOSITORY}' directory..."
        if [[ "${NONINTERACTIVE}" == "yes" ]]; then
            CONFIRM="N"
        else
            read -p "Replace '${REPOSITORY}' directory with intended contents? [y/N] " CONFIRM
        fi

        if [[ "$(printf '%s' "${CONFIRM}" | tr '[:upper:]' '[:lower:]')" == "y" ]]; then
            remove_directory_force "${REPOSITORY}"
        else
            msg_error "Aborted installation."
            exit 0
        fi
    fi

    msg "Cloning ${OWNER}/${REPOSITORY}/${TAG}..."

    ${GIT_CLONE} ${CLONE_OPTIONS} --branch "${TAG}" "https://github.com/${OWNER}/${REPOSITORY}"

    github_repository_status "${REPOSITORY}"
    pop_directory # pop BUILD_SRC_DIR
}

build_cmake()
{
    local PROJECT="$1"
    local RELATIVE_SRC_PATH="$2"
    local RELATIVE_OBJ_PATH="$3"
    local JOBS="$4"
    shift 4

    local VERBOSITY_CMAKE=""
    local VERBOSITY_MAKE=""

    if [[ "${DISPLAY_VERBOSE}" == "yes" ]]; then
        VERBOSITY_CMAKE="-DCMAKE_VERBOSE_MAKEFILE=ON"
        VERBOSITY_MAKE="--verbose"
    fi

    msg_heading "Preparing to build ${PROJECT}"


    # directory rationalization
    push_directory "${BUILD_SRC_DIR}/${PROJECT}"
    push_directory "${RELATIVE_SRC_PATH}"
    local BUILD_PATH="$(pwd)"
    pop_directory

    if [[ "${BUILD_OBJ_DIR_RELATIVE}" == "yes" ]]; then
        create_directory_force "${BUILD_OBJ_DIR}"
        push_directory "${BUILD_OBJ_DIR}"
    else
        push_directory "${BUILD_OBJ_DIR}"
        create_directory_force "${PROJECT}"
        pop_directory
        push_directory "${BUILD_OBJ_DIR}/${PROJECT}"
    fi

    if [[ "${RELATIVE_OBJ_PATH}" != "." ]]; then
        create_directory_force "${RELATIVE_OBJ_PATH}"
        push_directory "${RELATIVE_OBJ_PATH}"
    fi

    display_configure_options "$@"

    cmake ${VERBOSITY_CMAKE} -LA "$@" "${BUILD_SRC_DIR}/${PROJECT}/${RELATIVE_SRC_PATH}"

    # make
    if [[ ${JOBS} -gt ${SEQUENTIAL} ]]; then
        cmake --build . -j${JOBS} ${VERBOSITY_MAKE}
    else
        cmake --build . ${VERBOSITY_MAKE}
    fi

    pop_directory # BUILD_OBJ_DIR
    pop_directory # BUILD_SRC_DIR/PROJECT
    if [[ "${RELATIVE_OBJ_PATH}" != "." ]]; then
        pop_directory
    fi

    msg_success "'${PROJECT}' built successfully."
}


build_preset()
{
    local PROJECT="$1"
    local RELATIVE_SRC_PATH="$2"
    local RELATIVE_OBJ_PATH="$3"
    local JOBS="$4"
    shift 4

    local VERBOSITY_CMAKE=""
    local VERBOSITY_MAKE=""

    if [[ "${DISPLAY_VERBOSE}" == "yes" ]]; then
        VERBOSITY_CMAKE="-DCMAKE_VERBOSE_MAKEFILE=ON"
        VERBOSITY_MAKE="--verbose"
    fi

    msg_heading "Preparing to build ${PROJECT}"


    # directory rationalization
    push_directory "${BUILD_SRC_DIR}/${PROJECT}"
    push_directory "${RELATIVE_SRC_PATH}"
    local BUILD_PATH="$(pwd)"
    pop_directory

    if [[ "${BUILD_OBJ_DIR_RELATIVE}" == "yes" ]]; then
        create_directory_force "${BUILD_OBJ_DIR}"
        push_directory "${BUILD_OBJ_DIR}"
    else
        push_directory "${BUILD_OBJ_DIR}"
        create_directory_force "${PROJECT}"
        pop_directory
        push_directory "${BUILD_OBJ_DIR}/${PROJECT}"
    fi

    if [[ "${RELATIVE_OBJ_PATH}" != "." ]]; then
        create_directory_force "${RELATIVE_OBJ_PATH}"
        push_directory "${RELATIVE_OBJ_PATH}"
    fi

    display_configure_options "$@"

    cmake ${VERBOSITY_CMAKE} -LA --preset="${BUILD_PRESET}" "$@" "${BUILD_SRC_DIR}/${PROJECT}/${RELATIVE_SRC_PATH}"

    if [[ ${JOBS} -gt ${SEQUENTIAL} ]]; then
        cmake --build . -j${JOBS} ${VERBOSITY_MAKE}
    else
        cmake --build . ${VERBOSITY_MAKE}
    fi

    pop_directory # BUILD_OBJ_DIR
    pop_directory # BUILD_SRC_DIR/PROJECT
    if [[ "${RELATIVE_OBJ_PATH}" != "." ]]; then
        pop_directory
    fi

    msg_success "'${PROJECT}' built successfully."
}


clean_cmake()
{
    local PROJECT="$1"
    local RELATIVE_OBJ_PATH="$2"
    shift 2

    msg "Preparing to clean ${PROJECT}"

    push_directory "${BUILD_SRC_DIR}/${PROJECT}"

    if [[ "${BUILD_OBJ_DIR_RELATIVE}" == "yes" ]]; then
        push_directory "${BUILD_OBJ_DIR}"
    else
        push_directory "${BUILD_OBJ_DIR}/${PROJECT}"
    fi

    if [[ "${RELATIVE_OBJ_PATH}" != "." ]]; then
        push_directory "${RELATIVE_OBJ_PATH}"
    fi

    disable_exit_on_error

    cmake --build . --target clean

    local RESULT=$?

    if [[ ${RESULT} -ne 0 ]]; then
        msg_error "Encountered error, please see test.log contents above."
        exit ${RESULT}
    fi

    pop_directory # BUILD_OBJ_DIR
    pop_directory # BUILD_SRC_DIR/PROJECT
    if [[ "${RELATIVE_OBJ_PATH}" != "." ]]; then
        pop_directory
    fi

    msg_success "'${PROJECT}' clean complete."
}

install_cmake()
{
    local PROJECT="$1"
    local RELATIVE_OBJ_PATH="$2"
    shift 2

    msg "Preparing to install ${PROJECT}"

    push_directory "${BUILD_SRC_DIR}/${PROJECT}"

    if [[ "${BUILD_OBJ_DIR_RELATIVE}" == "yes" ]]; then
        push_directory "${BUILD_OBJ_DIR}"
    else
        push_directory "${BUILD_OBJ_DIR}/${PROJECT}"
    fi

    if [[ "${RELATIVE_OBJ_PATH}" != "." ]]; then
        push_directory "${RELATIVE_OBJ_PATH}"
    fi

    cmake --install .

    if [[ ${OS} == Linux ]] && [[ "${PREFIX}" == "/usr/local" ]]; then
        ldconfig
    fi

    pop_directory # BUILD_OBJ_DIR
    pop_directory # BUILD_SRC_DIR/PROJECT
    if [[ "${RELATIVE_OBJ_PATH}" != "." ]]; then
        pop_directory
    fi

    msg_success "'${PROJECT}' installation complete."
}

test_cmake()
{
    local PROJECT="$1"
    local RELATIVE_OBJ_PATH="$2"
    local JOBS="$3"
    shift 3

    msg "Preparing to test ${PROJECT}"

    push_directory "${BUILD_SRC_DIR}/${PROJECT}"

    if [[ "${BUILD_OBJ_DIR_RELATIVE}" == "yes" ]]; then
        push_directory "${BUILD_OBJ_DIR}"
    else
        push_directory "${BUILD_OBJ_DIR}/${PROJECT}"
    fi

    if [[ "${RELATIVE_OBJ_PATH}" != "." ]]; then
        push_directory "${RELATIVE_OBJ_PATH}"
    fi

    disable_exit_on_error

    ctest --test-dir .

    local RESULT=$?

    if [[ -e "Testing/Temporary/LastTest.log" ]]; then
        msg_warn "begin error log: Testing/Temporary/LastTest.log"
        cat "Testing/Temporary/LastTest.log"
        msg_warn "  end error log: Testing/Temporary/LastTest.log"
    fi

    if [[ -e "test.log" ]]; then
        msg_warn "begin error log: test.log"
        cat "test.log"
        msg_warn "  end error log: test.log"
    fi

    if [[ ${RESULT} -ne 0 ]]; then
        msg_error "Encountered error, please see test.log contents above."
        exit ${RESULT}
    fi

    enable_exit_on_error

    pop_directory # BUILD_OBJ_DIR
    pop_directory # BUILD_SRC_DIR/PROJECT
    if [[ "${RELATIVE_OBJ_PATH}" != "." ]]; then
        pop_directory
    fi

    msg_success "'${PROJECT}' test complete."
}

display_build_variables()
{
    msg "BUILD_libsecp256k1              : ${BUILD_libsecp256k1}"
    msg "BUILD_PRESET                    : ${BUILD_PRESET}"
    msg "BUILD_LIMIT                     : ${BUILD_LIMIT}"
    msg "BUILD_SRC_DIR                   : ${BUILD_SRC_DIR}"
    msg "BUILD_OBJ_DIR                   : ${BUILD_OBJ_DIR}"
    msg "BUILD_OBJ_DIR_RELATIVE          : ${BUILD_OBJ_DIR_RELATIVE}"
    msg "BUILD_CONFIG                    : ${BUILD_CONFIG}"
    msg "BUILD_LINK                      : ${BUILD_LINK}"
    msg "BUILD_FULL_REPOSITORIES         : ${BUILD_FULL_REPOSITORIES}"
    msg "BUILD_POST_INSTALL_CLEAN        : ${BUILD_POST_INSTALL_CLEAN}"
    msg "BUILD_USE_LOCAL_SRC             : ${BUILD_USE_LOCAL_SRC}"
    msg "BUILD_SKIP_TESTS                : ${BUILD_SKIP_TESTS}"
    msg "BUILD_SKIP_TEST_RUN             : ${BUILD_SKIP_TEST_RUN}"
    msg "PARALLEL                        : ${PARALLEL}"
    msg "PREFIX                          : ${PREFIX}"
    msg "DISPLAY_VERBOSE                 : ${DISPLAY_VERBOSE}"
    msg "CONFIGURE_OPTIONS               : ${CONFIGURE_OPTIONS[*]}"
if [[ "${DISPLAY_VERBOSE}" == "yes" ]]; then
    msg "CONFIGURE_OPTIONS_ORIGINAL      : ${CONFIGURE_OPTIONS_ORIGINAL[*]}"
fi
}

display_toolchain_variables()
{

    msg "CC                              : ${CC}"
    msg "CFLAGS                          : ${CFLAGS}"
    msg "CXX                             : ${CXX}"
    msg "CXXFLAGS                        : ${CXXFLAGS}"
    msg "LDFLAGS                         : ${LDFLAGS}"
    msg "LDADD                           : ${LDADD}"
    msg "LD_RUN_PATH                     : ${LD_RUN_PATH}"
    msg "LD_LIBRARY_PATH                 : ${LD_LIBRARY_PATH}"
    msg "PKG_CONFIG_PATH                 : ${PKG_CONFIG_PATH}"
    msg "LDLIBS                          : ${LDLIBS}"
    msg "CMAKE_PREFIX_PATH               : ${CMAKE_PREFIX_PATH}"
    msg "CMAKE_INSTALL_PREFIX            : ${CMAKE_INSTALL_PREFIX}"
    msg "CMAKE_INCLUDE_PATH              : ${CMAKE_INCLUDE_PATH}"
    msg "CMAKE_LIBRARY_PATH              : ${CMAKE_LIBRARY_PATH}"
}

display_constants()
{

    msg "secp256k1_OWNER                 : ${secp256k1_OWNER}"
    msg "secp256k1_TAG                   : ${secp256k1_TAG}"

    msg "libbitcoin_system_OWNER         : ${libbitcoin_system_OWNER}"
    msg "libbitcoin_system_TAG           : ${libbitcoin_system_TAG}"
}

help()
{
    msg "Script managing the build and installation of libbitcoin-system and its dependencies."
    msg ""
    msg "Script options:"

    msg "--build-libsecp256k1         Build bitcoin core libsecp256k1 libraries"
    msg "--build-preset=<preset>      Specifies preset configuration to build."
    msg "--build-src-dir=<path>       Location for sources."
    msg "                               Default: $(pwd)"
    msg "--build-limit=<limit>        Limitation at which the process will terminate."
    msg "                               Values: { none, target, dependencies, external }"
    msg "                               Default: none"
    msg "--build-full-repositories    Sync full github repositories."
    msg "                               Default: git clone --depth 1 --single-branch"
    msg "--build-post-install-clean   Clean dependencies after installation (saves space)."
    msg "--build-skip-tests           Skip test compilation and execution."
    msg "--build-skip-test-run        Compile tests without executing them."
    msg "--build-parallel=<int>       Number of jobs to run simultaneously."
    msg "                               Default: supported platforms use nproc/sysctl"
    msg "--build-use-local-src        Use existing sources in relevant paths."
    msg "--hardware-optimized         Inform compiler to apply applicable optimizations for the host hardware."
    msg "--noninteractive             Disable any prompt using default."
    msg "--verbose                    Display verbose script output."
    msg "--help, -h                   Display usage, overriding script execution."
    msg ""
    msg "Hardware configuration options for arm64:"
    msg "--hardware-aes               Use ARM AES and PMULL Extensions."
    msg "--hardware-sha2              Use ARM SHA1/SHA256 Extensions."
    msg "--hardware-sha3              Use ARM SHA3 Extensions for SHA512."
    msg ""
    msg "Hardware configuration options for x64:"
    msg "--hardware-avx2              Use Intel AVX2 intrinsics."
    msg "--hardware-avx512            Use Intel AVX512 intrinsics."
    msg "--hardware-avx512ifma        Use Intel AVX512 IFMA intrinsics (implies AVX512VL)."
    msg "--hardware-avxifma           Use Intel AVX IFMA intrinsics (implies AVX2)."
    msg "--hardware-shani             Use Intel SHA Extensions."
    msg "--hardware-sha512            Use Intel SHA512 Extensions (implies AVX2)."
    msg "--hardware-sse41             Use SSE4.1 hardware instructions."
    msg "--hardware-aesni             Use Intel AES-NI and PCLMULQDQ intrinsics."
    msg "--hardware-vaes              Use Intel VAES and VPCLMULQDQ intrinsics (implies AES-NI and AVX2)."
    msg ""
    msg "Recognized pass-through options:"
    msg "-Denable-shani=<ON/OFF>      Add Intel SHA Extensions via compilation options."
    msg "                               Default: OFF"
    msg "-Denable-sha512=<ON/OFF>     Add Intel SHA512 Extensions (implies AVX2) via compilation options."
    msg "                               Default: OFF"
    msg "-Denable-aesni=<ON/OFF>      Add Intel AES-NI and PCLMULQDQ intrinsics via compilation options."
    msg "                               Default: OFF"
    msg "-Denable-vaes=<ON/OFF>       Add Intel VAES and VPCLMULQDQ intrinsics (implies AES-NI and AVX2) via compilation options."
    msg "                               Default: OFF"
    msg "-Denable-avx512ifma=<ON/OFF> Add Intel AVX512 IFMA intrinsics (implies AVX512VL) via compilation options."
    msg "                               Default: OFF"
    msg "-Denable-avx512=<ON/OFF>     Add Intel AVX512 intrinsics via compilation options."
    msg "                               Default: OFF"
    msg "-Denable-avxifma=<ON/OFF>    Add Intel AVX IFMA intrinsics (implies AVX2) via compilation options."
    msg "                               Default: OFF"
    msg "-Denable-avx2=<ON/OFF>       Add Intel AVX2 intrinsics via compilation options."
    msg "                               Default: OFF"
    msg "-Denable-sse41=<ON/OFF>      Add SSE4.1 hardware instructions via compilation options."
    msg "                               Default: OFF"
    msg "-Denable-sha2=<ON/OFF>       Check for ARM SHA1/SHA256 Extensions."
    msg "                               Default: OFF"
    msg "-Denable-sha3=<ON/OFF>       Check for ARM SHA3 Extensions for SHA512."
    msg "                               Default: OFF"
    msg "-Denable-aes=<ON/OFF>        Check for ARM AES and PMULL Extensions."
    msg "                               Default: OFF"
    msg "-Dwith-secp256k1=<ON/OFF>    Use native secp256k1 interface, otherwise external library."
    msg "                               Default: ON"
    msg "-Dwith-valgrind=<ON/OFF>     Use valgrind memcheck client requests."
    msg "                               Default: OFF"
    msg "-Dwith-cuda=<ON/OFF>         Use cuda (gpu) batch signature verification (inert on macOS)."
    msg "                               Default: ON"
    msg "-Dwith-performance-tests=<ON/OFF> Compile performance tests."
    msg "                               Default: OFF"
    msg "-Dwith-slow-tests=<ON/OFF>   Compile slow tests."
    msg "                               Default: OFF"
    msg ""
    msg "All unrecognized options provided shall be passed as configuration"
    msg "options for all dependencies."
}

display_configure_options()
{
    msg "configure options:"
    for OPTION in "$@"; do
        if [[ -n ${OPTION} ]]; then
            msg "${OPTION}"
        fi
    done
}

remove_configure_option()
{
    local PATTERN="$1"
    local RETAINED=()
    local OPTION

    for OPTION in "${CONFIGURE_OPTIONS[@]}"; do
        if [[ ${OPTION} != ${PATTERN} ]]; then
            RETAINED=("${RETAINED[@]}" "${OPTION}")
        fi
    done

    CONFIGURE_OPTIONS=("${RETAINED[@]}")
}


enable_exit_on_error()
{
    eval "${OPTS_ENABLE}"
}

disable_exit_on_error()
{
    eval "${OPTS_DISABLE}"
}

create_directory()
{
    local DIRECTORY="$1"
    local MODE="$2"

    if [[ -d "${DIRECTORY}" ]]; then
        if [[ ${MODE} == "-f" ]]; then
            msg_warn "Reinitializing '${DIRECTORY}'..."
            rm -rf "${DIRECTORY}"
            mkdir -p "${DIRECTORY}"
        else
            msg_warn "Reusing existing '${DIRECTORY}'..."
        fi
    else
        msg "Initializing '${DIRECTORY}'..."
        mkdir -p "${DIRECTORY}"
    fi
}

create_directory_force()
{
    create_directory "$@" -f
}

pop_directory()
{
    msg_verbose "*** move  pre: '$(pwd)'"
    popd >/dev/null
    msg_verbose "*** move post: '$(pwd)'"
}

push_directory()
{
    msg_verbose "*** move  pre: '$(pwd)'"
    local DIRECTORY="$1"
    pushd "${DIRECTORY}" >/dev/null
    msg_verbose "*** move post: '$(pwd)'"
}

remove_directory_force()
{
    msg_verbose "*** removing: '$@'"
    rm -rf "$@"
}

COLOR_CYAN='\e[0;96m'
COLOR_GREEN='\e[0;92m'
COLOR_RED='\e[0;91m'
COLOR_YELLOW='\e[0;93m'
COLOR_RESET='\e[0m'

msg_heading()
{
    printf "\n********************** %b **********************\n" "$@"
}

msg_verbose()
{
    if [[ "${DISPLAY_VERBOSE}" == "yes" ]]; then
        printf "${COLOR_CYAN}%b${COLOR_RESET}\n" "$@"
    fi
}

msg()
{
    printf "%b\n" "$@"
}

msg_success()
{
    printf "${COLOR_GREEN}%b${COLOR_RESET}\n" "$@"
}

msg_warn()
{
    printf "${COLOR_YELLOW}%b${COLOR_RESET}\n" "$@"
}

msg_error()
{
    >&2 printf "${COLOR_RED}%b${COLOR_RESET}\n" "$@"
}

main "$@"
