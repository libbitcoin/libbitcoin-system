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
# --<enable/disable>-avx2     Use Intel AVX2 intrinsics.
#                               Default: --disable-avx2
# --<enable/disable>-avx512   Use Intel AVX512 intrinsics.
#                               Default: --disable-avx512
# --<enable/disable>-sse41    Use SSE4.1 hardware instructions.
#                               Default: --disable-sse41
# --<enable/disable>-shani    Use Intel/ARM SHA Extensions.
#                               Default: --disable-shani
# --build-boost               Build Boost libraries
# --build-secp256k1           Build libsecp256k1 libraries
# --build-src-dir=<path>      Location for sources.
#                               Default: $(pwd)
# --build-obj-dir=<path>      Location for intermediate objects.
#                               Default: obj
# --build-obj-dir-relative    Interpret build-obj-dir as relative to project sources.
# --build-config=<mode>       Specifies the build configuration.
#                               Values: { debug, release }
#                               Toolchain default behavior will occur if no value specified.
# --build-link=<mode>         Specifies the link mode.
#                               Values: { dynamic, static }
#                               Toolchain default behavior will occur if no value specified.
# --build-full-repositories   Sync full github repositories.
#                               Default: git clone --depth 1 --single-branch
# --build-post-install-clean  Clean dependencies after installation (saves space).
# --build-skip-tests          Skip test compilation and execution.
# --build-parallel=<int>      Number of jobs to run simultaneously.
#                               Default: supported platforms use nproc/sysctl
# --build-use-local-src       Use existing sources in relevant paths.
# --prefix=<path>             Installation destination.
#                               Default: /usr/local
# --verbose                   Display verbose script output.
# --help, -h                  Display usage, overriding script execution.
#
# All unrecognized options provided shall be passed as configuration
# options for all dependencies.

OPTS_ENABLE="set -eo pipefail"
OPTS_DISABLE="set +e"

eval "${OPTS_ENABLE}"
trap 'echo "FATAL ERROR: Command failed at line ${LINENO}: ${BASH_COMMAND}" >&2' ERR

SEQUENTIAL=1

if [[ -z ${boost_URLBASE} ]]; then
    boost_URLBASE="https://archives.boost.io/release/1.86.0/source/"
fi
if [[ -z ${boost_FILENAME} ]]; then
    boost_FILENAME="boost_1_86_0.tar.bz2"
fi

if [[ -z ${secp256k1_OWNER} ]]; then
    secp256k1_OWNER="bitcoin-core"
fi
if [[ -z ${secp256k1_TAG} ]]; then
    secp256k1_TAG="v0.7.0"
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
            (--build-boost)             BUILD_boost="yes";;
            (--build-secp256k1)         BUILD_secp256k1="yes";;
            (--build-src-dir=*)         BUILD_SRC_DIR="${OPTION#*=}";;
            (--build-obj-dir=*)         BUILD_OBJ_DIR="${OPTION#*=}";;
            (--build-obj-dir-relative)  BUILD_OBJ_DIR_RELATIVE="yes";;
            (--build-config=*)          BUILD_CONFIG="${OPTION#*=}";;
            (--build-link=*)            BUILD_LINK="${OPTION#*=}";;
            (--build-full-repositories) BUILD_FULL_REPOSITORIES="yes";;
            (--build-post-install-clean)BUILD_POST_INSTALL_CLEAN="yes";;
            (--build-skip-tests)        BUILD_SKIP_TESTS="yes";;
            (--build-parallel=*)        PARALLEL="${OPTION#*=}";;
            (--build-use-local-src)     BUILD_USE_LOCAL_SRC="yes";;
            (--prefix=*)                PREFIX="${OPTION#*=}";;
            (--verbose)                 DISPLAY_VERBOSE="yes";;
            (--help|-h)                 DISPLAY_HELP="yes";;
        esac
    done

    CONFIGURE_OPTIONS_ORIGINAL=("$@")
    CONFIGURE_OPTIONS=("$@")
    CONFIGURE_OPTIONS=("${CONFIGURE_OPTIONS[@]/--build-boost/}")
    CONFIGURE_OPTIONS=("${CONFIGURE_OPTIONS[@]/--build-secp256k1/}")
    CONFIGURE_OPTIONS=("${CONFIGURE_OPTIONS[@]/--build-src-dir=*/}")
    CONFIGURE_OPTIONS=("${CONFIGURE_OPTIONS[@]/--build-obj-dir=*/}")
    CONFIGURE_OPTIONS=("${CONFIGURE_OPTIONS[@]/--build-obj-dir-relative/}")
    CONFIGURE_OPTIONS=("${CONFIGURE_OPTIONS[@]/--build-config=*/}")
    CONFIGURE_OPTIONS=("${CONFIGURE_OPTIONS[@]/--build-link=*/}")
    CONFIGURE_OPTIONS=("${CONFIGURE_OPTIONS[@]/--build-full-repositories/}")
    CONFIGURE_OPTIONS=("${CONFIGURE_OPTIONS[@]/--build-post-install-clean/}")
    CONFIGURE_OPTIONS=("${CONFIGURE_OPTIONS[@]/--build-skip-tests/}")
    CONFIGURE_OPTIONS=("${CONFIGURE_OPTIONS[@]/--build-parallel=*/}")
    CONFIGURE_OPTIONS=("${CONFIGURE_OPTIONS[@]/--build-use-local-src/}")
    CONFIGURE_OPTIONS=("${CONFIGURE_OPTIONS[@]/--prefix=*/}")
    CONFIGURE_OPTIONS=("${CONFIGURE_OPTIONS[@]/--verbose/}")
    CONFIGURE_OPTIONS=("${CONFIGURE_OPTIONS[@]/--help/}")
    CONFIGURE_OPTIONS=("${CONFIGURE_OPTIONS[@]/-h/}")
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

    # --prefix
    if [[ -z "${PREFIX}" ]]; then
        # Always set a prefix (required for OSX and lib detection).
        PREFIX="/usr/local"
        msg_verbose "No --prefix specified, defaulting to '${PREFIX}'."
    else
        # Incorporate the custom libdir into each object, for link time resolution
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

    if [[ -n "${PREFIX}" ]]; then
        CONFIGURE_OPTIONS=( "${CONFIGURE_OPTIONS[@]}" "--prefix=${PREFIX}" )
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
        elif [[ ${OS} == Darwin ]] || [[ ${OS} == OpenBSD ]]; then
            PARALLEL=$(sysctl -n hw.ncpu)
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

    if [[ ${OS} == OpenBSD ]]; then
        make() { gmake "$@"; }
    fi

    if [[ -z "${STDLIB}" ]]; then
        if [[ ${OS} == Darwin ]]; then
            STDLIB="c++"
        elif [[ ${OS} == OpenBSD ]]; then
            STDLIB="estdc++"
        else
            STDLIB="stdc++"
        fi
    else
        define_message_verbose "STDLIB using defined value '${STDLIB}'"
    fi

    if [[ -z "${CC}" ]]; then
        if [[ ${OS} == Darwin ]]; then
            export CC="clang"
        elif [[ ${OS} == OpenBSD ]]; then
            export CC="egcc"
        fi
    else
        msg_verbose "CC using defined value '${CC}'"
    fi

    if [[ -z "${CXX}" ]]; then
        if [[ ${OS} == Darwin ]]; then
            export CXX="clang++"
        elif [[ ${OS} == OpenBSD ]]; then
            export CXX="eg++"
        fi
    else
        msg_verbose "CXX using defined value '${CXX}'"
    fi

    # translate BUILD_CONFIG to ZFLAGS
    if [[ -n "${BUILD_CONFIG}" ]]; then
        msg_verbose "*** Build config specified, calculating flags..."

        if [[ "${BUILD_CONFIG}" == "debug" ]]; then
            BUILD_FLAGS="-Og -g"
        elif [[ "${BUILD_CONFIG}" == "release" ]]; then
            BUILD_FLAGS="-O3"
        fi

        if [[ -z "${CFLAGS}" ]]; then
            export CFLAGS="${BUILD_FLAGS}"
            msg_verbose "Exporting CFLAGS '${CFLAGS}'"
        else
            msg_verbose "CFLAGS initially '${CFLAGS}'"
            export CFLAGS="${CFLAGS} ${BUILD_FLAGS}"
            msg_verbose "CFLAGS modified to '${CFLAGS}'"
        fi

        if [[ -z "${CXXFLAGS}" ]]; then
            export CXXFLAGS="${BUILD_FLAGS}"
            msg_verbose "Exporting CXXFLAGS '${CXXFLAGS}'"
        else
            msg_verbose "CXXFLAGS initially '${CXXFLAGS}'"
            export CXXFLAGS="${CXXFLAGS} ${BUILD_FLAGS}"
            msg_verbose "CXXFLAGS modified to '${CXXFLAGS}'"
        fi
    fi

    # Specify or remove --enable-ndebug for gnu toolchain on release
    if [[ "${BUILD_CONFIG}" == "debug" ]]; then
        CONFIGURE_OPTIONS=("${CONFIGURE_OPTIONS[@]/--enable-ndebug/}")
    elif [[ "${BUILD_CONFIG}" == "release" ]]; then
        CONFIGURE_OPTIONS=( "${CONFIGURE_OPTIONS[@]}" "--enable-ndebug" )
    fi

    # translate BUILD_LINK to appropriate arguments
    if [[ -n "${BUILD_LINK}" ]]; then
        CONFIGURE_OPTIONS=( "${CONFIGURE_OPTIONS[@]/--disable-shared=*}" )
        CONFIGURE_OPTIONS=( "${CONFIGURE_OPTIONS[@]/--enable-shared=*}" )
        CONFIGURE_OPTIONS=( "${CONFIGURE_OPTIONS[@]/--disable-static=*}" )
        CONFIGURE_OPTIONS=( "${CONFIGURE_OPTIONS[@]/--enable-static=*}" )

        if [[ "${BUILD_LINK}" == "dynamic" ]]; then
            CONFIGURE_OPTIONS=( "${CONFIGURE_OPTIONS[@]}" "--enable-shared" "--disable-static" )
        else
            CONFIGURE_OPTIONS=( "${CONFIGURE_OPTIONS[@]}" "--disable-shared" "--enable-static" )
        fi
    fi

    if [[ ${OS} == Linux && ${CC} == clang* ]] || [[ ${OS} == OpenBSD ]]; then
        export LDLIBS="-l${STDLIB} ${LDLIBS}"
        msg_verbose "LDLIBS has been manipulated to encode STDLIB linkage."
    fi

    # defaults and sanitization: generated options

    if [[ -n "${BUILD_boost}" ]]; then
        export BOOST_ROOT="${PREFIX}"
        with_boost="--with-boost=${PREFIX}"
    fi

    REMAP=()
    for argument in "${CONFIGURE_OPTIONS[@]}"; do
        if [[ -n "${argument}" ]]; then
            REMAP+=( "${argument}" )
        fi
    done
    CONFIGURE_OPTIONS=( "${REMAP[@]}" )
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

    boost_FLAGS=(
        "-Wno-enum-constexpr-conversion")

    boost_OPTIONS=(
        "--with-iostreams"
        "--with-locale"
        "--with-program_options"
        "--with-regex"
        "--with-thread"
        "--with-url"
        "--with-test")

    secp256k1_FLAGS=(
        "-w")

    secp256k1_OPTIONS=(
        "--disable-tests"
        "--enable-experimental"
        "--enable-module-recovery"
        "--enable-module-schnorrsig")

    libbitcoin_system_FLAGS=()

    libbitcoin_system_OPTIONS=(
        "${with_boost}"
        "${with_pkgconfigdir}")

    if [[ ${BUILD_boost} == "yes" ]]; then
        source_archive "boost" "${boost_FILENAME}" "${boost_URLBASE}" "bzip2"
        local SAVE_CPPFLAGS="${CPPFLAGS}"
        export CPPFLAGS="${CPPFLAGS} ${boost_FLAGS[@]}"
        build_boost "boost" "${boost_OPTIONS[@]}"
        export CPPFLAGS="${SAVE_CPPFLAGS}"
    fi

    if [[ ${BUILD_secp256k1} == "yes" ]]; then
        source_github "${secp256k1_OWNER}" "secp256k1" "${secp256k1_TAG}"
        local SAVE_CPPFLAGS="${CPPFLAGS}"
        export CPPFLAGS="${CPPFLAGS} ${secp256k1_FLAGS[@]}"
        build_gnu "secp256k1" "." "${PARALLEL}" "${secp256k1_OPTIONS[@]}" "${CONFIGURE_OPTIONS[@]}"
        install_make "secp256k1"
        if [[ "${BUILD_POST_INSTALL_CLEAN}" == "yes" ]]; then
            clean_make "secp256k1"
        fi
        export CPPFLAGS="${SAVE_CPPFLAGS}"
    fi

    source_github "${libbitcoin_system_OWNER}" "libbitcoin-system" "${libbitcoin_system_TAG}"
    local SAVE_CPPFLAGS="${CPPFLAGS}"
    export CPPFLAGS="${CPPFLAGS} ${libbitcoin_system_FLAGS[@]}"
    build_gnu "libbitcoin-system" "." "${PARALLEL}" "${libbitcoin_system_OPTIONS[@]}" "${CONFIGURE_OPTIONS[@]}"
    if ! [[ "${BUILD_SKIP_TESTS}" == "yes" ]]; then
        test_make "libbitcoin-system" "check" "${PARALLEL}"
    fi
    install_make "libbitcoin-system"
    if [[ "${BUILD_POST_INSTALL_CLEAN}" == "yes" ]]; then
        clean_make "libbitcoin-system"
    fi
    export CPPFLAGS="${SAVE_CPPFLAGS}"

    msg_success "Completed successfully."
}

source_archive()
{
    local PROJECT="$1"
    local FILENAME="$2"
    local URL_BASE="$3"
    local COMPRESSION="$4"
    shift 4

    msg_heading "Preparing to acquire ${PROJECT}"

    push_directory "${BUILD_SRC_DIR}"

    if [ -d "${PROJECT}" ] &&
       [[ "${BUILD_USE_LOCAL_SRC}" == "yes" ]]; then
        msg_warn "Reusing existing '${PROJECT}' directory..."
        return 0
    fi

    if [ -d "${PROJECT}" ]; then
        msg_warn "Encountered existing '${PROJECT}' directory, removing..."
        remove_directory_force "${PROJECT}"
    fi

    msg "Retrieving ${PROJECT}..."

    create_directory "${PROJECT}"
    push_directory "${PROJECT}"

    local TAR="tar"
    local WGET="wget --quiet"

    # retrieve file
    ${WGET} --output-document "${FILENAME}" "${URL_BASE}${FILENAME}"
    # ${WGET} --output-document "${FILENAME}" "${URL_BASE}${FILENAME}"

    # extract to expected path
    ${TAR} --extract --file "${FILENAME}" --${COMPRESSION} --strip-components=1
    # ${TAR} --extract --file "${FILENAME}" "--${COMPRESSION}" --strip-components=1

    # pop ${PROJECT}
    pop_directory

    # pop ${BUILD_SRC_DIR}
    pop_directory

    msg_success "Completed download and extraction successfully."
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
        pop_directory # BUILD_SRC_DIR
        return 0
    fi

    if [ -d "${REPOSITORY}" ]; then
        msg_warn "Encountered existing '${REPOSITORY}' directory, removing..."
        remove_directory_force "${REPOSITORY}"
    fi

    msg "Cloning ${OWNER}/${REPOSITORY}/${TAG}..."

    ${GIT_CLONE} ${CLONE_OPTIONS} --branch "${TAG}" "https://github.com/${OWNER}/${REPOSITORY}"

    # pop BUILD_SRC_DIR
    pop_directory
}

install_make()
{
    local PROJECT="$1"
    shift

    msg "Preparing to install ${PROJECT}"

    push_directory "${BUILD_SRC_DIR}/${PROJECT}"

    if [[ "${BUILD_OBJ_DIR_RELATIVE}" == "yes" ]]; then
        push_directory "${BUILD_OBJ_DIR}"
    else
        push_directory "${BUILD_OBJ_DIR}/${PROJECT}"
    fi

    make install

    if [[ ${OS} == Linux ]] && [[ "${PREFIX}" == "/usr/local" ]]; then
        ldconfig
    fi

    pop_directory # BUILD_OBJ_DIR
    pop_directory # BUILD_SRC_DIR/PROJECT

    msg_success "'${PROJECT}' installation complete."
}

test_make()
{
    local PROJECT="$1"
    local TARGET="$2"
    local JOBS="$3"
    shift 3

    msg "Preparing to test ${PROJECT}"

    push_directory "${BUILD_SRC_DIR}/${PROJECT}"

    if [[ "${BUILD_OBJ_DIR_RELATIVE}" == "yes" ]]; then
        push_directory "${BUILD_OBJ_DIR}"
    else
        push_directory "${BUILD_OBJ_DIR}/${PROJECT}"
    fi

    disable_exit_on_error

    if [[ ${JOBS} -gt ${SEQUENTIAL} ]]; then
        make -j${JOBS} ${TARGET} VERBOSE=1
    else
        make ${TARGET} VERBOSE=1
    fi

    local RESULT=$?

    if [[ -e "test-suite.log" ]]; then
        msg_warn "begin error log: test-suite.log"
        cat "test-suite.log"
        msg_warn "  end error log: test-suite.log"
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

    msg_success "'${PROJECT}' test complete."
}

clean_make()
{
    local PROJECT="$1"
    shift 1

    msg "Preparing to clean ${PROJECT}"

    push_directory "${BUILD_SRC_DIR}/${PROJECT}"

    if [[ "${BUILD_OBJ_DIR_RELATIVE}" == "yes" ]]; then
        push_directory "${BUILD_OBJ_DIR}"
    else
        push_directory "${BUILD_OBJ_DIR}/${PROJECT}"
    fi

    disable_exit_on_error

    make clean

    local RESULT=$?

    if [[ ${RESULT} -ne 0 ]]; then
        msg_error "Encountered error, please see test.log contents above."
        exit ${RESULT}
    fi

    pop_directory # BUILD_OBJ_DIR
    pop_directory # BUILD_SRC_DIR/PROJECT

    msg_success "'${PROJECT}' clean complete."
}

build_boost()
{
    local PROJECT="$1"
    shift

    msg_heading "Preparing to build ${PROJECT}"

    push_directory "${BUILD_SRC_DIR}/${PROJECT}"

    local SAVE_IFS="${IFS}"
    IFS=' '

    # Compute configuration
    if [[ "${BUILD_LINK}" == "dynamic" ]]; then
        BOOST_LINK="shared"
    elif [[ "${BUILD_LINK}" == "static" ]]; then
        BOOST_LINK="static"
    else
        BOOST_LINK="static,shared"
    fi

    if [[ -n ${CC} ]]; then
        BOOST_TOOLSET="toolset=${CC}"
    fi

    if [[ (${OS} == Linux && ${CC} == clang*) || (${OS} == OpenBSD) ]]; then
        STDLIB_FLAG="-stdlib=lib${STDLIB}"
        BOOST_CXXFLAGS="cxxflags=${STDLIB_FLAG}"
        BOOST_LINKFLAGS="linkflags=${STDLIB_FLAG}"
    fi

    guessed_toolset=`${BUILD_SRC_DIR}/${PROJECT}/tools/build/src/engine/build.sh --guess-toolset`
    CXXFLAGS="-w" ${BUILD_SRC_DIR}/${PROJECT}/tools/build/src/engine/build.sh ${guessed_toolset} --cxxflags="-w"
    cp "${BUILD_SRC_DIR}/${PROJECT}/tools/build/src/engine/b2" .

    if [[ -n "${BOOST_CXXFLAGS}" ]]; then
        BOOST_CXXFLAGS="${BOOST_CXXFLAGS} ${boost_FLAGS[@]}"
    else
        BOOST_CXXFLAGS="cxxflags=${boost_FLAGS[@]}"
    fi

    # Prebuild status report
    msg "${PROJECT} configuration."
    msg "--------------------------------------------------------------------"
    msg "variant               : release"
    msg "threading             : multi"
    msg "toolset               : ${CC}"
    msg "boost cxxflags        : ${BOOST_CXXFLAGS}"
    msg "boost linkflags       : ${BOOST_LINKFLAGS}"
    msg "link                  : ${BOOST_LINK}"
    msg "-sNO_BZIP2            : 1"
    msg "-sNO_ZSTD             : 1"
    msg "-j                    : ${PARALLEL}"
    msg "-d0                   : [suppress informational messages]"
    msg "-q                    : [stop at the first error]"
    msg "--reconfigure         : [ignore cached configuration]"
    msg "--prefix              : ${PREFIX}"
    msg "BOOST_OPTIONS         : $*"
    msg "cxxflags (ignored)    : ${CXXFLAGS}"
    msg "--------------------------------------------------------------------"

    # Begin build
    ./bootstrap.sh --with-bjam=./b2 --prefix=${PREFIX}

    ./b2 install \
        "cxxstd=20" \
        "variant=release" \
        "threading=multi" \
        "${BOOST_TOOLSET}" \
        "${BOOST_CXXFLAGS}" \
        "${BOOST_LINKFLAGS}" \
        "link=${BOOST_LINK}" \
        "warnings=off" \
        "-sNO_BZIP2=1" \
        "-sNO_ZSTD=1" \
        "-j ${PARALLEL}" \
        "-d0" \
        "-q" \
        "--reconfigure" \
        "--prefix=${PREFIX}" \
        "$@"

    IFS="${SAVE_IFS}"

    pop_directory # BUILD_SRC_DIR
}

build_gnu()
{
    local PROJECT="$1"
    local RELATIVE_PATH="$2"
    local JOBS="$3"
    shift 3

    local VERBOSITY_GNU=""
    local VERBOSITY_MAKE=""

    if [[ "${DISPLAY_VERBOSE}" == "yes" ]]; then
        VERBOSITY_GNU="--verbose"
        VERBOSITY_MAKE="VERBOSE=1"
    fi

    msg_heading "Preparing to build ${PROJECT}"


    # directory rationalization
    push_directory "${BUILD_SRC_DIR}/${PROJECT}"
    push_directory "${RELATIVE_PATH}"
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

    # configuration
    push_directory "${BUILD_PATH}"
    autoreconf ${VERBOSITY_GNU} -i
    pop_directory # BUILD_PATH

    display_configure_options "$@"

    "${BUILD_PATH}/configure" ${VERBOSITY_GNU} "$@"

    # make
    if [[ ${JOBS} -gt ${SEQUENTIAL} ]]; then
        make -j${JOBS} ${VERBOSITY_MAKE}
    else
        make
    fi

    pop_directory # BUILD_OBJ_DIR
    pop_directory # BUILD_SRC_DIR/PROJECT
    msg_success "'${PROJECT}' built successfully."
}

display_build_variables()
{
    msg "BUILD_boost                     : ${BUILD_boost}"
    msg "BUILD_secp256k1                 : ${BUILD_secp256k1}"
    msg "BUILD_SRC_DIR                   : ${BUILD_SRC_DIR}"
    msg "BUILD_OBJ_DIR                   : ${BUILD_OBJ_DIR}"
    msg "BUILD_OBJ_DIR_RELATIVE          : ${BUILD_OBJ_DIR_RELATIVE}"
    msg "BUILD_CONFIG                    : ${BUILD_CONFIG}"
    msg "BUILD_LINK                      : ${BUILD_LINK}"
    msg "BUILD_FULL_REPOSITORIES         : ${BUILD_FULL_REPOSITORIES}"
    msg "BUILD_POST_INSTALL_CLEAN        : ${BUILD_POST_INSTALL_CLEAN}"
    msg "BUILD_USE_LOCAL_SRC             : ${BUILD_USE_LOCAL_SRC}"
    msg "BUILD_SKIP_TESTS                : ${BUILD_SKIP_TESTS}"
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
    msg "LD_RUN_PATH                     : ${LD_RUN_PATH}"
    msg "LD_LIBRARY_PATH                 : ${LD_LIBRARY_PATH}"
    msg "PKG_CONFIG_PATH                 : ${PKG_CONFIG_PATH}"
    msg "LDLIBS                          : ${LDLIBS}"
    msg "BOOST_ROOT                      : ${BOOST_ROOT}"
}

display_constants()
{
    msg "boost_URLBASE                   : ${boost_URLBASE}"
    msg "boost_FILENAME                  : ${boost_FILENAME}"

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
    msg "--<enable/disable>-avx2     Use Intel AVX2 intrinsics."
    msg "                              Default: --disable-avx2"
    msg "--<enable/disable>-avx512   Use Intel AVX512 intrinsics."
    msg "                              Default: --disable-avx512"
    msg "--<enable/disable>-sse41    Use SSE4.1 hardware instructions."
    msg "                              Default: --disable-sse41"
    msg "--<enable/disable>-shani    Use Intel/ARM SHA Extensions."
    msg "                              Default: --disable-shani"
    msg "--build-boost               Build Boost libraries"
    msg "--build-secp256k1           Build libsecp256k1 libraries"
    msg "--build-src-dir=<path>      Location for sources."
    msg "                              Default: $(pwd)"
    msg "--build-obj-dir=<path>      Location for intermediate objects."
    msg "                              Default: obj"
    msg "--build-obj-dir-relative    Interpret build-obj-dir as relative to project sources."
    msg "--build-config=<mode>       Specifies the build configuration."
    msg "                              Values: { debug, release }"
    msg "                              Toolchain default behavior will occur if no value specified."
    msg "--build-link=<mode>         Specifies the link mode."
    msg "                              Values: { dynamic, static }"
    msg "                              Toolchain default behavior will occur if no value specified."
    msg "--build-full-repositories   Sync full github repositories."
    msg "                              Default: git clone --depth 1 --single-branch"
    msg "--build-post-install-clean  Clean dependencies after installation (saves space)."
    msg "--build-skip-tests          Skip test compilation and execution."
    msg "--build-parallel=<int>      Number of jobs to run simultaneously."
    msg "                              Default: supported platforms use nproc/sysctl"
    msg "--build-use-local-src       Use existing sources in relevant paths."
    msg "--prefix=<path>             Installation destination."
    msg "                              Default: /usr/local"
    msg "--verbose                   Display verbose script output."
    msg "--help, -h                  Display usage, overriding script execution."
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
