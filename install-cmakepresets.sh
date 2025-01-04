#!/bin/bash
###############################################################################
#  Copyright (c) 2014-2025 libbitcoin-system developers (see COPYING).
#
#         GENERATED SOURCE CODE, DO NOT EDIT EXCEPT EXPERIMENTALLY
#
###############################################################################
# Script to build and install libbitcoin-system.
#
# Script options:
# --with-icu               Compile with International Components for Unicode.
#                            Since the addition of BIP-39 and later BIP-38
#                            support, libbitcoin conditionally incorporates ICU
#                            to provide BIP-38 and BIP-39 passphrase
#                            normalization features. Currently
#                            libbitcoin-explorer is the only other library that
#                            accesses this feature, so if you do not intend to
#                            use passphrase normalization this dependency can
#                            be avoided.
# --build-icu              Builds ICU libraries.
# --build-boost            Builds Boost libraries.
# --build-dir=<path>       Location of downloaded and intermediate files.
# --preset=<label>         CMakePreset label.
# --prefix=<absolute-path> Library install location (defaults to /usr/local).
# --disable-shared         Disables shared library builds.
# --disable-static         Disables static library builds.
# --verbose                Display verbose output (defaults to quiet on called tooling).
# --help                   Display usage, overriding script execution.
#
# Verified on Ubuntu 14.04, requires gcc-4.8 or newer.
# Verified on OSX 10.10, using MacPorts and Homebrew repositories, requires
# Apple LLVM version 6.0 (clang-600.0.54) (based on LLVM 3.5svn) or newer.
# This script does not like spaces in the --prefix or --build-dir, sorry.
# Values (e.g. yes|no) in the '--disable-<linkage>' options are not supported.
# All command line options are passed to 'configure' of each repo, with
# the exception of the --build-<item> options, which are for the script only.
# Depending on the caller's permission to the --prefix or --build-dir
# directory, the script may need to be sudo'd.

# Define constants.
#==============================================================================

# Declare associative array for computed presets.
#------------------------------------------------------------------------------
declare -A REPO_PRESET

# Sentinel for comparison of sequential build.
#------------------------------------------------------------------------------
SEQUENTIAL=1

# Git clone parameters.
#------------------------------------------------------------------------------
if [[ $GIT_CLONE_PARAMS ]]; then
    display_message "Using shell-defined GIT_CLONE_PARAMS value."
else
    GIT_CLONE_PARAMS="--depth 1 --single-branch"
fi

# The default build directory.
#------------------------------------------------------------------------------
BUILD_DIR="build-libbitcoin-system"

PRESUMED_CI_PROJECT_PATH=$(pwd)

# ICU archive.
#------------------------------------------------------------------------------
ICU_URL="https://github.com/unicode-org/icu/releases/download/release-55-2/icu4c-55_2-src.tgz"
ICU_ARCHIVE="icu4c-55_2-src.tgz"

# Boost archive.
#------------------------------------------------------------------------------
BOOST_URL="https://archives.boost.io/release/1.78.0/source/boost_1_78_0.tar.bz2"
BOOST_ARCHIVE="boost_1_78_0.tar.bz2"


# Define utility functions.
#==============================================================================
configure_links()
{
    # Configure dynamic linker run-time bindings when installing to system.
    if [[ ($OS == Linux) && ($PREFIX == "/usr/local") ]]; then
        ldconfig
    fi
}

configure_options()
{
    display_message "configure options:"
    for OPTION in "$@"; do
        if [[ $OPTION ]]; then
            display_message "$OPTION"
        fi
    done

    ./configure "$@"
}

create_directory()
{
    local DIRECTORY="$1"

    rm -rf "$DIRECTORY"
    mkdir "$DIRECTORY"
}

display_heading_message()
{
    printf "\n********************** %s **********************\n" "$@"
}

display_message()
{
    printf "%s\n" "$@"
}

display_error()
{
    >&2 printf "%s\n" "$@"
}

initialize_git()
{
    display_heading_message "Initialize git"

    # Initialize git repository at the root of the current directory.
    git init
    git config user.name anonymous
}

# make_project_directory project_name jobs [configure_options]
make_project_directory()
{
    local PROJ_NAME=$1
    local JOBS=$2
    local TEST=$3
    shift 3

    push_directory "$PROJ_NAME"
    local PROJ_CONFIG_DIR
    PROJ_CONFIG_DIR=$(pwd)

    ./autogen.sh

    configure_options "$@"
    make_jobs "$JOBS"

    if [[ $TEST == true ]]; then
        make_tests "$JOBS"
    fi

    make install
    configure_links
    pop_directory
}

# make_jobs jobs [make_options]
make_jobs()
{
    local JOBS=$1
    shift 1

    VERBOSITY=""
    if [[ DISPLAY_VERBOSE ]]; then
        VERBOSITY="VERBOSE=1"
    fi

    SEQUENTIAL=1
    # Avoid setting -j1 (causes problems on single threaded systems [TRAVIS]).
    if [[ $JOBS > $SEQUENTIAL ]]; then
        make -j"$JOBS" "$@" $VERBOSITY
    else
        make "$@" $VERBOSITY
    fi
}

# make_tests jobs
make_tests()
{
    local JOBS=$1

    disable_exit_on_error

    # Build and run unit tests relative to the primary directory.
    # VERBOSE=1 ensures test runner output sent to console (gcc).
    make_jobs "$JOBS" check "VERBOSE=1"
    local RESULT=$?

    # Test runners emit to the test.log file.
    if [[ -e "test.log" ]]; then
        cat "test.log"
    fi

    if [[ $RESULT -ne 0 ]]; then
        exit $RESULT
    fi

    enable_exit_on_error
}

pop_directory()
{
    popd >/dev/null
}

push_directory()
{
    local DIRECTORY="$1"

    pushd "$DIRECTORY" >/dev/null
}

enable_exit_on_error()
{
    set -e
}

disable_exit_on_error()
{
    set +e
}

display_help()
{
    display_message "Usage: ./install.sh [OPTION]..."
    display_message "Manage the installation of libbitcoin-system."
    display_message "Script options:"
    display_message "  --with-icu               Compile with International Components for Unicode."
    display_message "                             Since the addition of BIP-39 and later BIP-38 "
    display_message "                             support, libbitcoin conditionally incorporates ICU "
    display_message "                             to provide BIP-38 and BIP-39 passphrase "
    display_message "                             normalization features. Currently "
    display_message "                             libbitcoin-explorer is the only other library that "
    display_message "                             accesses this feature, so if you do not intend to "
    display_message "                             use passphrase normalization this dependency can "
    display_message "                             be avoided."
    display_message "  --build-icu              Build ICU libraries."
    display_message "  --build-boost            Build Boost libraries."
    display_message "  --build-secp256k1        Build libsecp256k1 libraries."
    display_message "  --build-dir=<path>       Location of downloaded and intermediate files."
    display_message "  --preset=<label>         CMakePreset label."
    display_message "  --prefix=<absolute-path> Library install location (defaults to /usr/local)."
    display_message "  --disable-shared         Disables shared library builds."
    display_message "  --disable-static         Disables static library builds."
    display_message "  --help                   Display usage, overriding script execution."
    display_message ""
    display_message "All unrecognized options provided shall be passed as configuration options for "
    display_message "all dependencies."
}

# Define environment initialization functions
#==============================================================================
parse_command_line_options()
{
    for OPTION in "$@"; do
        case $OPTION in
            # Standard script options.
            (--help)                DISPLAY_HELP="yes";;
            (--verbose)             DISPLAY_VERBOSE="yes";;

            # Standard build options.
            (--prefix=*)            PREFIX="${OPTION#*=}";;
            (--disable-shared)      DISABLE_SHARED="yes";;
            (--disable-static)      DISABLE_STATIC="yes";;

            # Common project options.
            (--with-icu)            WITH_ICU="yes";;

            # Custom build options.
            (--build-icu)                              BUILD_ICU="yes";;
            (--build-boost)                            BUILD_BOOST="yes";;
            (--build-secp256k1)                        BUILD_SECP256K1="yes";;

            # Unique script options.
            (--build-dir=*)         BUILD_DIR="${OPTION#*=}";;
            (--preset=*)            PRESET_ID="${OPTION#*=}";;

            # Handle ndebug declarations due to disabled argument passthrough
            (--enable-ndebug)       ENABLE_NDEBUG="yes";;
            (--disable-ndebug)      DISABLE_NDEBUG="yes";;

        esac
    done
}

handle_help_line_option()
{
    if [[ $DISPLAY_HELP ]]; then
        display_help
        exit 0
    fi
}

set_operating_system()
{
    OS=$(uname -s)
}

configure_build_parallelism()
{
    if [[ $PARALLEL ]]; then
        display_message "Using shell-defined PARALLEL value."
    elif [[ $OS == Linux ]]; then
        PARALLEL=$(nproc)
    elif [[ ($OS == Darwin) || ($OS == OpenBSD) ]]; then
        PARALLEL=$(sysctl -n hw.ncpu)
    else
        display_error "Unsupported system: $OS"
        display_error "  Explicit shell-definition of PARALLEL will avoid system detection."
        display_error ""
        display_help
        exit 1
    fi
}

set_os_specific_compiler_settings()
{
    if [[ $OS == Darwin ]]; then
        export CC="clang"
        export CXX="clang++"
        STDLIB="c++"
    elif [[ $OS == OpenBSD ]]; then
        make() { gmake "$@"; }
        export CC="egcc"
        export CXX="eg++"
        STDLIB="estdc++"
    else # Linux
        STDLIB="stdc++"
    fi
}

link_to_standard_library()
{
    if [[ ($OS == Linux && $CC == clang*) || ($OS == OpenBSD) ]]; then
        export LDLIBS="-l$STDLIB $LDLIBS"
        export CXXFLAGS="-stdlib=lib$STDLIB $CXXFLAGS"
    fi
}

normalize_static_and_shared_options()
{
    if [[ $DISABLE_SHARED ]]; then
        CONFIGURE_OPTIONS=("$@" "--enable-static")
    elif [[ $DISABLE_STATIC ]]; then
        CONFIGURE_OPTIONS=("$@" "--enable-shared")
    else
        CONFIGURE_OPTIONS=("$@" "--enable-shared")
        CONFIGURE_OPTIONS=("$@" "--enable-static")
    fi
}

handle_custom_options()
{
    if [[
        ($PRESET_ID != "nix-base") &&
        ($PRESET_ID != "gnu-debug") &&
        ($PRESET_ID != "gnu-release") &&
        ($PRESET_ID != "static") &&
        ($PRESET_ID != "shared") &&
        ($PRESET_ID != "nix-gnu-debug-static") &&
        ($PRESET_ID != "nix-gnu-debug-shared") &&
        ($PRESET_ID != "nix-gnu-release-static") &&
        ($PRESET_ID != "nix-gnu-release-shared")]]; then
        display_error "Unsupported preset: $PRESET_ID"
        display_error "Supported values are:"
        display_error "  nix-base"
        display_error "  gnu-debug"
        display_error "  gnu-release"
        display_error "  static"
        display_error "  shared"
        display_error "  nix-gnu-debug-static"
        display_error "  nix-gnu-debug-shared"
        display_error "  nix-gnu-release-static"
        display_error "  nix-gnu-release-shared"
        display_error ""
        display_help
        exit 1
    fi

    BASE_PRESET_ID="$PRESET_ID"
    REPO_PRESET[libbitcoin-system]="$PRESET_ID"
    display_message "REPO_PRESET[libbitcoin-system]=${REPO_PRESET[libbitcoin-system]}"

    CUMULATIVE_FILTERED_ARGS=""
    CUMULATIVE_FILTERED_ARGS_CMAKE=""

    if [[ $ENABLE_NDEBUG && $DISABLE_NDEBUG ]]; then
        display_error "--enable-ndebug and --disable-ndebug are mutually exclusive options."
        display_error ""
        exit 1
    elif [[ $DISABLE_NDEBUG ]]; then
        CUMULATIVE_FILTERED_ARGS="--disable-ndebug"
        CUMULATIVE_FILTERED_ARGS_CMAKE="-Denable-ndebug=no"
    else
        CUMULATIVE_FILTERED_ARGS="--enable-ndebug"
        CUMULATIVE_FILTERED_ARGS_CMAKE="-Denable-ndebug=yes"
    fi

    # Process link declaration
    if [[ $DISABLE_SHARED ]]; then
        CUMULATIVE_FILTERED_ARGS+=" --enable-static --disable-shared"
        CUMULATIVE_FILTERED_ARGS_CMAKE+=" -DBUILD_SHARED_LIBS=FALSE"
    elif [[ $DISABLE_STATIC ]]; then
        CUMULATIVE_FILTERED_ARGS+=" --disable-static --enable-shared"
        CUMULATIVE_FILTERED_ARGS_CMAKE+=" -DBUILD_SHARED_LIBS=TRUE"
    fi

    # Process prefix
    if [[ ($PREFIX) ]]; then
        CUMULATIVE_FILTERED_ARGS+=" --prefix="${PREFIX}""
        CUMULATIVE_FILTERED_ARGS_CMAKE+=" -DCMAKE_PREFIX_PATH="${PREFIX}" -DCMAKE_INSTALL_PREFIX="${PREFIX}""

        if [ -z $CMAKE_INCLUDE_PATH ]; then
            export CMAKE_INCLUDE_PATH="${PREFIX}/include"
        else
            export CMAKE_INCLUDE_PATH="${PREFIX}/include:${CMAKE_INCLUDE_PATH}"
        fi

        if [ -z $CMAKE_LIBRARY_PATH ]; then
            export CMAKE_LIBRARY_PATH="${PREFIX}/lib"
        else
            export CMAKE_LIBRARY_PATH="${PREFIX}/lib:${CMAKE_LIBRARY_PATH}"
        fi
    fi

    # Process ICU
    if [[ $WITH_ICU ]]; then
        CUMULATIVE_FILTERED_ARGS+=" --with-icu"
        CUMULATIVE_FILTERED_ARGS_CMAKE+=" -Dwith-icu=yes"
    fi
}

remove_build_options()
{
    # Purge custom build options so they don't break configure.
    CONFIGURE_OPTIONS=("${CONFIGURE_OPTIONS[@]/--build-*/}")
}

remove_install_options()
{
    # Purge installer handled options other than --build-.
    CONFIGURE_OPTIONS=("${CONFIGURE_OPTIONS[@]/--with-*/}")
    CONFIGURE_OPTIONS=("${CONFIGURE_OPTIONS[@]/--without-*/}")
    CONFIGURE_OPTIONS=("${CONFIGURE_OPTIONS[@]/--enable-*/}")
    CONFIGURE_OPTIONS=("${CONFIGURE_OPTIONS[@]/--disable-*/}")
    CONFIGURE_OPTIONS=("${CONFIGURE_OPTIONS[@]/--prefix=*/}")
    CONFIGURE_OPTIONS=("${CONFIGURE_OPTIONS[@]/--preset=*/}")
}

set_prefix()
{
    # Always set a prefix (required on OSX and for lib detection).
    if [[ ! ($PREFIX) ]]; then
        PREFIX="/usr/local"
        CONFIGURE_OPTIONS=( "${CONFIGURE_OPTIONS[@]}" "--prefix=$PREFIX")
    else
        # Incorporate the custom libdir into each object, for link time resolution.
        if [[ ! ($LD_RUN_PATH) ]]; then
            export LD_RUN_PATH="$LD_RUN_PATH:$PREFIX/lib"
        else
            export LD_RUN_PATH="$PREFIX/lib"
        fi

        if [[ ! ($LD_LIBRARY_PATH) ]]; then
            export LD_LIBRARY_PATH="$LD_LIBRARY_PATH:$PREFIX/lib"
        else
            export LD_LIBRARY_PATH="$PREFIX/lib"
        fi
    fi
}

set_pkgconfigdir()
{
    # Set the prefix-based package config directory.
    PREFIX_PKG_CONFIG_DIR="$PREFIX/lib/pkgconfig"

    # Prioritize prefix package config in PKG_CONFIG_PATH search path.
    export PKG_CONFIG_PATH="$PREFIX_PKG_CONFIG_DIR:$PKG_CONFIG_PATH"

    # Set a package config save path that can be passed via our builds.
    with_pkgconfigdir="-Dpkgconfigdir=$PREFIX_PKG_CONFIG_DIR"
}

set_with_boost_prefix()
{
    if [[ $BUILD_BOOST ]]; then
        # Boost detection via FindBoost.cmake provides for path hint via
	# $BOOT_ROOT environment variable only.
        export BOOST_ROOT="$PREFIX"
    fi
}

display_configuration()
{
    display_message "libbitcoin-system installer configuration."
    display_message "--------------------------------------------------------------------"
    display_message "OS                    : $OS"
    display_message "PARALLEL              : $PARALLEL"
    display_message "CC                    : $CC"
    display_message "CXX                   : $CXX"
    display_message "CPPFLAGS              : $CPPFLAGS"
    display_message "CFLAGS                : $CFLAGS"
    display_message "CXXFLAGS              : $CXXFLAGS"
    display_message "LDFLAGS               : $LDFLAGS"
    display_message "LDLIBS                : $LDLIBS"
    display_message "WITH_ICU              : $WITH_ICU"
    display_message "BUILD_ICU             : $BUILD_ICU"
    display_message "BUILD_BOOST           : $BUILD_BOOST"
    display_message "BUILD_SECP256K1       : $BUILD_SECP256K1"
    display_message "BOOST_ROOT            : $BOOST_ROOT"
    display_message "BUILD_DIR                      : $BUILD_DIR"
    display_message "PRESET_ID                      : $PRESET_ID"
    display_message "CUMULATIVE_FILTERED_ARGS       : $CUMULATIVE_FILTERED_ARGS"
    display_message "CUMULATIVE_FILTERED_ARGS_CMAKE : $CUMULATIVE_FILTERED_ARGS_CMAKE"
    display_message "PREFIX                : $PREFIX"
    display_message "DISABLE_SHARED        : $DISABLE_SHARED"
    display_message "DISABLE_STATIC        : $DISABLE_STATIC"
    display_message "with_boost            : ${with_boost}"
    display_message "with_pkgconfigdir     : ${with_pkgconfigdir}"
    display_message "--------------------------------------------------------------------"
}


# Define build functions.
#==============================================================================
# Because PKG_CONFIG_PATH doesn't get updated by Homebrew or MacPorts.
initialize_icu_packages()
{
    if [[ ($OS == Darwin) ]]; then
        # Update PKG_CONFIG_PATH for ICU package installations on OSX.
        # OSX provides libicucore.dylib with no pkgconfig and doesn't support
        # renaming or important features, so we can't use that.
        local HOMEBREW_ICU_PKG_CONFIG="/usr/local/opt/icu4c/lib/pkgconfig"
        local MACPORTS_ICU_PKG_CONFIG="/opt/local/lib/pkgconfig"

        if [[ -d "$HOMEBREW_ICU_PKG_CONFIG" ]]; then
            export PKG_CONFIG_PATH="$PKG_CONFIG_PATH:$HOMEBREW_ICU_PKG_CONFIG"
        elif [[ -d "$MACPORTS_ICU_PKG_CONFIG" ]]; then
            export PKG_CONFIG_PATH="$PKG_CONFIG_PATH:$MACPORTS_ICU_PKG_CONFIG"
        fi
    fi
}

extract_from_tarball()
{
    local TARGET_DIR=$1
    local URL=$2
    local ARCHIVE=$3
    local COMPRESSION=$4

    create_directory "$TARGET_DIR"
    push_directory "$TARGET_DIR"

    # Extract the source locally.
    WGET="wget --quiet"
    TAR="tar"

    if [[ $DISPLAY_VERBOSE ]]; then
        WGET="wget --verbose"
        TAR="tar --verbose"
    fi

    $WGET --output-document "$ARCHIVE" "$URL"
    $TAR --extract --file "$ARCHIVE" "--$COMPRESSION" --strip-components=1

    display_message "Completed download and extraction successfully."
    pop_directory
}

unpack_from_tarball()
{
    local ARCHIVE=$1
    local URL=$2
    local COMPRESSION=$3
    local BUILD=$4

    display_heading_message "Preparing to acquire $ARCHIVE"

    if [[ ! ($BUILD) ]]; then
        display_message "Skipping unpack of $ARCHIVE..."
        return
    fi

    local TARGET_DIR="build-$ARCHIVE"

    if [[ -d "$TARGET_DIR" ]]; then
        if [[ true ]]; then
            display_message "Re-downloading $ARCHIVE..."
            rm -rf "$TARGET_DIR"
            extract_from_tarball "$TARGET_DIR" "$URL" "$ARCHIVE" "$COMPRESSION"
        else
            display_message "Reusing existing archive $ARCHIVE..."
        fi
    else
        display_message "Downloading $ARCHIVE..."
        extract_from_tarball "$TARGET_DIR" "$URL" "$ARCHIVE" "$COMPRESSION"
    fi
}

# Standard build from tarball.
build_from_tarball()
{
    local ARCHIVE=$1
    local PUSH_DIR=$2
    local JOBS=$3
    local BUILD=$4
    local OPTIONS=$5
    shift 5

    local SAVE_LDFLAGS="$LDFLAGS"
    local SAVE_CPPFLAGS="$CPPFLAGS"

    # For some platforms we need to set ICU pkg-config path.
    # TODO: clean this up?
    if [[ ! ($BUILD) ]]; then
        if [[ $ARCHIVE == "$ICU_ARCHIVE" ]]; then
            display_heading_message "Rationalize ICU detection."
            initialize_icu_packages
        fi
        return
    fi

    display_heading_message "Preparing to build $ARCHIVE"

    # Because ICU tools don't know how to locate internal dependencies.
    if [[ ($ARCHIVE == "$ICU_ARCHIVE") ]]; then
        export LDFLAGS="-L$PREFIX/lib $LDFLAGS"
    fi

    local TARGET="build-$ARCHIVE"
    push_directory "$TARGET"
    push_directory "$PUSH_DIR"

    # Join generated and command line options.
    local CONFIGURATION=("${OPTIONS[@]}" "$@")

    if [[ $ARCHIVE == "$MBEDTLS_ARCHIVE" ]]; then
        make -j "$JOBS" lib
        make DESTDIR=$PREFIX install
    else
        configure_options "${CONFIGURATION[@]}"
        make_jobs "$JOBS" --silent
        make install
    fi

    configure_links

    pop_directory
    pop_directory

    # Restore flags to prevent side effects.
    export LDFLAGS=$SAVE_LDFLAGS
    export CPPFLAGS=$SAVE_CPPFLAGS
}

clone_from_github()
{
    local FORK=$1
    local BRANCH=$2

    # Clone the repository locally.
    git clone $GIT_CLONE_PARAMS --branch "$BRANCH" "https://github.com/$FORK"
}

create_from_github()
{
    push_directory "$BUILD_SRC_DIR"

    local ACCOUNT=$1
    local REPO=$2
    local BRANCH=$3
    local BUILD=$4
	shift 4

    if [[ ! ($BUILD) || ($BUILD == "no") ]]; then
        return
    fi

    FORK="$ACCOUNT/$REPO"

    display_heading_message "Preparing to acquire $FORK/$BRANCH"

    if [[ -d "$REPO" ]]; then
        if [[ true ]]; then
            display_message "Re-cloning $FORK/$BRANCH..."
            rm -rf "$REPO"
            clone_from_github "$FORK" "$BRANCH"
        else
            display_message "Reusing existing clone of $FORK, Branch may not match $BRANCH..."
        fi
    else
        display_message "Cloning $FORK/$BRANCH..."
        clone_from_github "$FORK" "$BRANCH"
    fi

    pop_directory
}

# Standard build from github.
build_from_github()
{
    local REPO=$1
    local JOBS=$2
    local TEST=$3
    local BUILD=$4
    local OPTIONS=$5
    shift 5

    if [[ ! ($BUILD) || ($BUILD == "no") ]]; then
        return
    fi

    # Join generated and command line options.
    local CONFIGURATION=("${OPTIONS[@]}" "$@")

    display_heading_message "Preparing to build $REPO"

    # Build the local repository clone.
    make_project_directory "$REPO" "$JOBS" "$TEST" "${CONFIGURATION[@]}"
}

cmake_tests()
{
    local JOBS=$1

    disable_exit_on_error

    # Build and run unit tests relative to the primary directory.
    # VERBOSE=1 ensures test runner output sent to console (gcc).
    CTEST_OUTPUT_ON_FAILURE=ON make -j"$JOBS" test "VERBOSE=1"
    local RESULT=$?

    if [[ $RESULT -ne 0 ]]; then
        exit $RESULT
    fi

    enable_exit_on_error
}

cmake_project_directory()
{
    local PROJ_NAME=$1
    local PRESET=$2
    local JOBS=$3
    local TEST=$4
    shift 4

    push_directory "$PROJ_NAME"
    local PROJ_CONFIG_DIR
    PROJ_CONFIG_DIR=$(pwd)

    push_directory "builds/cmake"
    display_message "Preparing cmake --preset=$PRESET $@"
    cmake -LA --preset=$PRESET $@
    popd

    push_directory "obj/$PRESET"
    make_jobs "$JOBS"

    if [[ $TEST == true ]]; then
        cmake_tests "$JOBS"
    fi

    make install
    configure_links
    pop_directory
    pop_directory
}

build_from_github_cmake()
{
    local REPO=$1
    local PRESET=$2
    local JOBS=$3
    local TEST=$4
    local BUILD=$5
    local OPTIONS=$6
    shift 6

    if [[ ! ($BUILD) || ($BUILD == "no") ]]; then
        return
    fi

    # Join generated and command line options.
    local CONFIGURATION=("${OPTIONS[@]}" "$@")

    display_heading_message "Preparing to build $REPO"

    # Build the local repository clone.
    cmake_project_directory "$REPO" "$PRESET" "$JOBS" "$TEST" "${CONFIGURATION[@]}"
}

# Because boost ICU static lib detection assumes in incorrect ICU path.
circumvent_boost_icu_detection()
{
    # Boost expects a directory structure for ICU which is incorrect.
    # Boost ICU discovery fails when using prefix, can't fix with -sICU_LINK,
    # so we rewrite the two 'has_icu_test.cpp' files to always return success.

    local SUCCESS="int main() { return 0; }"
    local REGEX_TEST="libs/regex/build/has_icu_test.cpp"
    local LOCALE_TEST="libs/locale/build/has_icu_test.cpp"

    printf "%s" "$SUCCESS" > $REGEX_TEST
    printf "%s" "$SUCCESS" > $LOCALE_TEST

    # display_message "Hack: ICU detection modified, will always indicate found."
}

# Because boost doesn't support autoconfig and doesn't like empty settings.
initialize_boost_configuration()
{
    if [[ $DISABLE_STATIC ]]; then
        BOOST_LINK="shared"
    elif [[ $DISABLE_SHARED ]]; then
        BOOST_LINK="static"
    else
        BOOST_LINK="static,shared"
    fi

    if [[ $CC ]]; then
        BOOST_TOOLSET="toolset=$CC"
    fi

    if [[ ($OS == Linux && $CC == clang*) || ($OS == OpenBSD) ]]; then
        STDLIB_FLAG="-stdlib=lib$STDLIB"
        BOOST_CXXFLAGS="cxxflags=$STDLIB_FLAG"
        BOOST_LINKFLAGS="linkflags=$STDLIB_FLAG"
    fi
}

# Because boost doesn't use pkg-config.
# The hacks below are still required as of boost 1.72.0.
initialize_boost_icu_configuration()
{
    BOOST_ICU_ICONV="on"
    BOOST_ICU_POSIX="on"

    if [[ $WITH_ICU ]]; then
        # Restrict other locale options when compiling boost with icu.
        BOOST_ICU_ICONV="off"
        BOOST_ICU_POSIX="off"

        # Work around boost ICU static lib discovery bug.
        circumvent_boost_icu_detection

        # Extract ICU prefix directory from package config variable.
        ICU_PREFIX=$(pkg-config icu-i18n --variable=prefix)

        # Extract ICU libs from package config variables and augment with -ldl.
        ICU_LIBS="$(pkg-config icu-i18n --libs) -ldl"

        # This is a hack for boost m4 scripts that fail with ICU dependency.
        export BOOST_ICU_LIBS=("${ICU_LIBS[@]}")
    fi
}

# Because boost doesn't use autoconfig.
build_from_tarball_boost()
{
    local SAVE_IFS="$IFS"
    IFS=' '

    local ARCHIVE=$1
    local JOBS=$2
    local BUILD=$3
    shift 3

    if [[ ! ($BUILD) ]]; then
        return
    fi

    display_heading_message "Preparing to build $ARCHIVE"

    local TARGET="build-$ARCHIVE"

    push_directory "$TARGET"

    initialize_boost_configuration
    initialize_boost_icu_configuration

    guessed_toolset=`./tools/build/src/engine/build.sh --guess-toolset`
    CXXFLAGS="-w" ./tools/build/src/engine/build.sh ${guessed_toolset} --cxxflags="-w"
    cp tools/build/src/engine/b2 .
    if [[ (x"$BOOST_CXXFLAGS" == "x") ]]; then
        BOOST_CXXFLAGS="cxxflags=${BOOST_FLAGS[@]}"
    else
        BOOST_CXXFLAGS="$BOOST_CXXFLAGS ${BOOST_FLAGS[@]}"
    fi

    display_message "Libbitcoin boost configuration."
    display_message "--------------------------------------------------------------------"
    display_message "variant               : release"
    display_message "threading             : multi"
    display_message "toolset               : $CC"
    display_message "boost cxxflags        : $BOOST_CXXFLAGS"
    display_message "boost linkflags       : $BOOST_LINKFLAGS"
    display_message "link                  : $BOOST_LINK"
    display_message "boost.locale.iconv    : $BOOST_ICU_ICONV"
    display_message "boost.locale.posix    : $BOOST_ICU_POSIX"
    display_message "-sNO_BZIP2            : 1"
    display_message "-sNO_ZSTD             : 1"
    display_message "-sICU_PATH            : $ICU_PREFIX"
  # display_message "-sICU_LINK            : " "${ICU_LIBS[*]}"
    display_message "-j                    : $JOBS"
    display_message "-d0                   : [supress informational messages]"
    display_message "-q                    : [stop at the first error]"
    display_message "--reconfigure         : [ignore cached configuration]"
    display_message "--prefix              : $PREFIX"
    display_message "BOOST_OPTIONS         : $*"
    display_message "cxxflags (ignored)    : $CXXFLAGS"
    display_message "--------------------------------------------------------------------"

    ./bootstrap.sh \
        "--with-bjam=./b2" \
        "--prefix=$PREFIX" \
        "--with-icu=$ICU_PREFIX"

    # boost_regex:
    # As of boost 1.72.0 the ICU_LINK symbol is no longer supported and
    # produces a hard stop if WITH_ICU is also defined. Removal is sufficient.
    # github.com/libbitcoin/libbitcoin-system/issues/1192
    # "-sICU_LINK=${ICU_LIBS[*]}"

    ./b2 install \
        "cxxstd=11" \
        "variant=release" \
        "threading=multi" \
        "$BOOST_TOOLSET" \
        "$BOOST_CXXFLAGS" \
        "$BOOST_LINKFLAGS" \
        "link=$BOOST_LINK" \
	"warnings=off" \
        "boost.locale.iconv=$BOOST_ICU_ICONV" \
        "boost.locale.posix=$BOOST_ICU_POSIX" \
        "-sNO_BZIP2=1" \
        "-sNO_ZSTD=1" \
        "-sICU_PATH=$ICU_PREFIX" \
        "-j $JOBS" \
        "-d0" \
        "-q" \
        "--reconfigure" \
        "--prefix=$PREFIX" \
        "$@"

    pop_directory

    IFS="$SAVE_IFS"
}


# The master build function.
#==============================================================================
build_all()
{
    unpack_from_tarball "$ICU_ARCHIVE" "$ICU_URL" gzip "$BUILD_ICU"
    local SAVE_CPPFLAGS="$CPPFLAGS"
    export CPPFLAGS="$CPPFLAGS ${ICU_FLAGS[@]}"
    build_from_tarball "$ICU_ARCHIVE" source "$PARALLEL" "$BUILD_ICU" "${ICU_OPTIONS[@]}" $CUMULATIVE_FILTERED_ARGS
    export CPPFLAGS=$SAVE_CPPFLAGS
    unpack_from_tarball "$BOOST_ARCHIVE" "$BOOST_URL" bzip2 "$BUILD_BOOST"
    local SAVE_CPPFLAGS="$CPPFLAGS"
    export CPPFLAGS="$CPPFLAGS ${BOOST_FLAGS[@]}"
    build_from_tarball_boost "$BOOST_ARCHIVE" "$PARALLEL" "$BUILD_BOOST" "${BOOST_OPTIONS[@]}"
    export CPPFLAGS=$SAVE_CPPFLAGS
    create_from_github bitcoin-core secp256k1 v0.5.1 "$BUILD_SECP256K1"
    local SAVE_CPPFLAGS="$CPPFLAGS"
    export CPPFLAGS="$CPPFLAGS ${SECP256K1_FLAGS[@]}"
    build_from_github secp256k1 "$PARALLEL" false "$BUILD_SECP256K1" "${SECP256K1_OPTIONS[@]}" $CUMULATIVE_FILTERED_ARGS
    export CPPFLAGS=$SAVE_CPPFLAGS
    local SAVE_CPPFLAGS="$CPPFLAGS"
    export CPPFLAGS="$CPPFLAGS ${BITCOIN_SYSTEM_FLAGS[@]}"
    if [[ ! ($CI == true) ]]; then
        create_from_github libbitcoin libbitcoin-system master "yes"
        display_message "libbitcoin-system PRESET ${REPO_PRESET[libbitcoin-system]}"
        build_from_github_cmake libbitcoin-system ${REPO_PRESET[libbitcoin-system]} "$PARALLEL" true                 "yes" "${BITCOIN_SYSTEM_OPTIONS[@]}" $CUMULATIVE_FILTERED_ARGS_CMAKE "$@"
    else
        push_directory "$PRESUMED_CI_PROJECT_PATH"
        push_directory ".."
        display_message "libbitcoin-system PRESET ${REPO_PRESET[libbitcoin-system]}"
        build_from_github_cmake libbitcoin-system ${REPO_PRESET[libbitcoin-system]} "$PARALLEL" true "yes" "${BITCOIN_SYSTEM_OPTIONS[@]}" $CUMULATIVE_FILTERED_ARGS_CMAKE "$@"
        pop_directory
        pop_directory
    fi
    export CPPFLAGS=$SAVE_CPPFLAGS
}


# Initialize the build environment.
#==============================================================================
enable_exit_on_error
parse_command_line_options "$@"
handle_help_line_option
handle_custom_options
set_operating_system
configure_build_parallelism
set_os_specific_compiler_settings "$@"
link_to_standard_library
normalize_static_and_shared_options "$@"
remove_build_options
set_prefix
set_pkgconfigdir
set_with_boost_prefix

remove_install_options

# Define build flags.
#==============================================================================
# Define icu flags.
#------------------------------------------------------------------------------
ICU_FLAGS=(
"-w")

# Define boost flags.
#------------------------------------------------------------------------------
BOOST_FLAGS=(
"-Wno-enum-constexpr-conversion")

# Define secp256k1 flags.
#------------------------------------------------------------------------------
SECP256K1_FLAGS=(
"-w")


# Define build options.
#==============================================================================
# Define icu options.
#------------------------------------------------------------------------------
ICU_OPTIONS=(
"--enable-draft" \
"--enable-rpath" \
"--enable-tools" \
"--disable-extras" \
"--disable-icuio" \
"--disable-layout" \
"--disable-layoutex" \
"--disable-tests" \
"--disable-samples")

# Define boost options.
#------------------------------------------------------------------------------
BOOST_OPTIONS=(
"--with-chrono" \
"--with-iostreams" \
"--with-json" \
"--with-locale" \
"--with-program_options" \
"--with-system" \
"--with-thread" \
"--with-test")

# Define secp256k1 options.
#------------------------------------------------------------------------------
SECP256K1_OPTIONS=(
"--disable-tests" \
"--enable-experimental" \
"--enable-module-recovery" \
"--enable-module-schnorrsig")

# Define bitcoin-system options.
#------------------------------------------------------------------------------
BITCOIN_SYSTEM_OPTIONS=(
"${with_boost}" \
"${with_pkgconfigdir}")


# Build the primary library and all dependencies.
#==============================================================================
display_configuration

if [[ ! ($CI == true) ]]; then
    create_directory "$BUILD_DIR"
    push_directory "$BUILD_DIR"
else
    push_directory "$BUILD_DIR"
fi

initialize_git
time build_all "${CONFIGURE_OPTIONS[@]}"
pop_directory
