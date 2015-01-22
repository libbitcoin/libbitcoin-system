#!/bin/bash
###############################################################################
# Copyright (c) 2011-2014 libbitcoin developers (see COPYING).
#
#         GENERATED SOURCE CODE, DO NOT EDIT EXCEPT EXPERIMENTALLY
#
###############################################################################
# Script to build and install libbitcoin.
#
# Script options:
# --build-boost            Builds Boost libraries.
# --build-dir=<path>       Location of downloaded and intermediate files.
# --prefix=<absolute-path> Library install location (defaults to /usr/local).
# --disable-shared         Disables shared library builds.
# --disable-static         Disables static library builds.
#
# Verified on Ubuntu 14.04, requires gcc-4.8 or newer.
# Verified on OSX 10.10, using MacPorts and Homebrew repositories, requires
# Apple LLVM version 6.0 (clang-600.0.54) (based on LLVM 3.5svn) or newer.
# This script does not like spaces in the --prefix or --build-dir, sorry.
# Values (e.g. yes|no) in the boolean options are not supported by the script.
# All command line options are passed to 'configure' of each repo, with
# the exception of the --build-<item> options, which are for the script only.
# Depending on the caller's permission to the --prefix or --build-dir
# directory, the script may need to be sudo'd.

# Define common constants.
#==============================================================================
# The default build directory.
#------------------------------------------------------------------------------
BUILD_DIR="build-libbitcoin"

# Boost archives for gcc.
#------------------------------------------------------------------------------
BOOST_URL_GCC="http://sourceforge.net/projects/boost/files/boost/1.49.0/boost_1_49_0.tar.bz2/download"
BOOST_ARCHIVE_GCC="boost_1_49_0.tar.bz2"

# Boost archives for clang.
#------------------------------------------------------------------------------
BOOST_URL_CLANG="http://sourceforge.net/projects/boost/files/boost/1.54.0/boost_1_54_0.tar.bz2/download"
BOOST_ARCHIVE_CLANG="boost_1_54_0.tar.bz2"

# GMP archives.
#------------------------------------------------------------------------------

# Initialize the build environment.
#==============================================================================
# Exit this script on the first build error.
#------------------------------------------------------------------------------
set -e

# Configure build parallelism.
#------------------------------------------------------------------------------
SEQUENTIAL=1
OS=`uname -s`
if [[ $TRAVIS == true ]]; then
    PARALLEL=$SEQUENTIAL
elif [[ $OS == Linux ]]; then
    PARALLEL=`nproc`
elif [[ $OS == Darwin ]]; then
    PARALLEL=2 #TODO
else
    echo "Unsupported system: $OS"
    exit 1
fi
echo "Make jobs: $PARALLEL"
echo "Make for system: $OS"

# Define operating system settings.
#------------------------------------------------------------------------------
if [[ $OS == Darwin ]]; then

    # Always require CLang, common lib linking will otherwise fail.
    export CC="clang"
    export CXX="clang++"
    
    # Always initialize prefix on OSX so default is consistent.
    PREFIX="/usr/local"
fi
echo "Make with cc: $CC"
echo "Make with cxx: $CXX"

# Parse command line options that are handled by this script.
#------------------------------------------------------------------------------
for OPTION in "$@"; do
    case $OPTION in
        (--prefix=*) PREFIX="${OPTION#*=}";;
        (--build-dir=*) BUILD_DIR="${OPTION#*=}";;

        (--build-gmp) BUILD_GMP="yes";;
        (--build-boost) BUILD_BOOST="yes";;
        
        (--disable-shared) DISABLE_SHARED="yes";;
        (--disable-static) DISABLE_STATIC="yes";;
    esac
done
echo "Build directory: $BUILD_DIR"
echo "Prefix directory: $PREFIX"

# Purge our custom options so they don't go to configure.
#------------------------------------------------------------------------------
CONFIGURE_OPTIONS=( "$@" )
CUSTOM_OPTIONS=( "--build-dir=$BUILD_DIR" "--build-boost" "--build-gmp" )
for CUSTOM_OPTION in "${CUSTOM_OPTIONS[@]}"; do
    CONFIGURE_OPTIONS=( "${CONFIGURE_OPTIONS[@]/$CUSTOM_OPTION}" )
done

# Set public boost_link variable (to translate configuration to Boost build).
#------------------------------------------------------------------------------
if [[ $DISABLE_STATIC == yes ]]; then
    boost_link="link=shared"
elif [[ $DISABLE_SHARED == yes ]]; then
    boost_link="link=static"
else
    boost_link="link=static,shared"
fi

# Set public boost_stdlib variable (to translate configuration to Boost build).
#------------------------------------------------------------------------------
if [[ $OS == Darwin ]]; then
    boost_stdlib="libc++"
else
    boost_stdlib="libstdc++"
fi

# Incorporate the prefix.
#------------------------------------------------------------------------------
if [[ $PREFIX ]]; then

    # Set public with_pkgconfigdir variable (for packages that handle it).
    # Currently all relevant dependencies support it except secp256k1.
    PKG_CONFIG_DIR="$PREFIX/lib/pkgconfig"
    with_pkgconfigdir="--with-pkgconfigdir=$PKG_CONFIG_DIR"
    
    # Augment PKG_CONFIG_PATH with prefix path. 
    # This allows package config to locate packages installed to the prefix.
    # TODO: determine why this is necessary when setting with_pkgconfigdir.
    export PKG_CONFIG_PATH="$PKG_CONFIG_DIR:$PKG_CONFIG_PATH"

    # Boost m4 discovery searches in the following order:
    # --with-boost=<path>, /usr, /usr/local, /opt, /opt/local, BOOST_ROOT.
    # We use --with-boost to prioritize the --prefix path when we build it.
    # Otherwise the standard paths suffice for Linux, Homebrew and MacPorts.

    # Set public with_boost variable (because Boost has no pkg-config).
    if [[ $BUILD_BOOST == yes ]]; then
        with_boost="--with-boost=$PREFIX"
    fi
    
    # Set public gmp_flags variable (because GMP has no pkg-config).
    if [[ $BUILD_GMP == yes ]]; then
        gmp_flags="CPPFLAGS=-I$PREFIX/include LDFLAGS=-L$PREFIX/lib"
    fi
    
    # Set public prefix variable (to tell Boost where to build).
    prefix="--prefix=$PREFIX"
fi

# Echo published dynamic build options.
#------------------------------------------------------------------------------
echo "Published dynamic options:"
echo "  boost_link: $boost_link"
echo "  boost_stdlib: $boost_stdlib"
echo "  prefix: $prefix"
echo "  gmp_flags: $gmp_flags"
echo "  with_boost: $with_boost"
echo "  with_pkgconfigdir: $with_pkgconfigdir"


# Define build options.
#==============================================================================
# Define boost options for gcc.
#------------------------------------------------------------------------------
BOOST_OPTIONS_GCC=\
"threading=single "\
"variant=release "\
"--disable-icu "\
"--with-date_time "\
"--with-filesystem "\
"--with-program_options "\
"--with-regex "\
"--with-system "\
"--with-test "\
"-d0 "\
"-q "\
"${prefix} "\
"${boost_link} "

# Define boost options for clang.
#------------------------------------------------------------------------------
BOOST_OPTIONS_CLANG=\
"toolset=clang "\
"cxxflags=-stdlib=${boost_stdlib} "\
"linkflags=-stdlib=${boost_stdlib} "\
"threading=single "\
"variant=release "\
"--disable-icu "\
"--with-date_time "\
"--with-filesystem "\
"--with-program_options "\
"--with-regex "\
"--with-system "\
"--with-test "\
"-d0 "\
"-q "\
"${prefix} "\
"${boost_link} "

# Define secp256k1 options.
#------------------------------------------------------------------------------
SECP256K1_OPTIONS=\
"--enable-tests=no "\
"--with-bignum=no "

# Define bitcoin options.
#------------------------------------------------------------------------------
BITCOIN_OPTIONS=\
"${with_boost} "\
"${with_pkgconfigdir} "


# Define compiler settings.
#==============================================================================
if [[ $CXX == "clang++" ]]; then
    BOOST_URL="$BOOST_URL_CLANG"
    BOOST_ARCHIVE="$BOOST_ARCHIVE_CLANG"
    BOOST_OPTIONS="$BOOST_OPTIONS_CLANG"
else # g++
    BOOST_URL="$BOOST_URL_GCC"
    BOOST_ARCHIVE="$BOOST_ARCHIVE_GCC"
    BOOST_OPTIONS="$BOOST_OPTIONS_GCC"
fi


# Define utility functions.
#==============================================================================
configure_options()
{
    echo "configure: $@"
    ./configure "$@"
}

create_directory()
{
    local DIRECTORY="$1"

    rm -rf "$DIRECTORY"
    mkdir "$DIRECTORY"
}

display_linkage()
{
    local LIBRARY="$1"
    
    # Display shared library links.
    if [[ $OS == Darwin ]]; then
        otool -L "$LIBRARY"
    else
        ldd --verbose "$LIBRARY"
    fi
}

display_message()
{
    MESSAGE="$1"
    echo
    echo "********************** $MESSAGE **********************"
    echo
}

initialize_git()
{
    # Initialize git repository at the root of the current directory.
    git init
    git config user.name anonymous
}

make_current_directory()
{
    local JOBS=$1
    shift 1

    ./autogen.sh
    configure_options "$@"
    make_jobs $JOBS
    make install

    # Use ldconfig only in case of non --prefix installation on Linux.    
    if [[ ($OS == Linux) && !($PREFIX)]]; then
        ldconfig
    fi
}

make_jobs()
{
    local JOBS=$1
    local TARGET=$2

    # Avoid setting -j1 (causes problems on Travis).
    if [[ $JOBS > $SEQUENTIAL ]]; then
        make -j$JOBS $TARGET
    else
        make $TARGET
    fi
}

make_tests()
{
    local JOBS=$1

    # Build and run unit tests relative to the primary directory.
    # VERBOSE=1 ensures test-suite.log output sent to console.
    make_jobs $JOBS check VERBOSE=1
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


# Build functions.
#==============================================================================
build_from_tarball_boost()
{
    local URL=$1
    local ARCHIVE=$2
    local REPO=$3
    local JOBS=$4
    shift 4

    if [[ $BUILD_BOOST != yes ]]; then
        display_message "Boost build not enabled"
        return
    fi
    
    display_message "Download $ARCHIVE"

    create_directory $REPO
    push_directory $REPO

    # Extract the source locally.
    wget --output-document $ARCHIVE $URL
    tar --extract --file $ARCHIVE --bzip2 --strip-components=1

    echo "configure: $@"
    echo

    # Build and install (note that "$@" is not from script args).
    ./bootstrap.sh
    ./b2 install -j $JOBS "$@"

    pop_directory
}

build_from_tarball_gmp()
{
    local URL=$1
    local ARCHIVE=$2
    local REPO=$3
    local JOBS=$4
    shift 4

    if [[ $BUILD_GMP != yes ]]; then
        display_message "GMP build not enabled"
        return
    fi

    display_message "Download $ARCHIVE"
    
    create_directory $REPO
    push_directory $REPO
    
    # Extract the source locally.
    wget --output-document $ARCHIVE $URL
    tar --extract --file $ARCHIVE --bzip2 --strip-components=1

    # Build the local sources.
    # GMP does not provide autogen.sh or package config.
    configure_options "$@"

    # GMP does not honor noise reduction.
    echo "Making all..."
    make_jobs $JOBS >/dev/null
    echo "Installing all..."
    make install >/dev/null

    pop_directory
}

build_from_github()
{
    local ACCOUNT=$1
    local REPO=$2
    local BRANCH=$3
    local JOBS=$4
    shift 4

    FORK="$ACCOUNT/$REPO"
    display_message "Download $FORK/$BRANCH"
    
    # Clone the repository locally.
    git clone --branch $BRANCH --single-branch "https://github.com/$FORK"

    # Build the local repository clone.
    push_directory $REPO
    make_current_directory $JOBS "$@"
    pop_directory
}

build_from_local()
{
    local MESSAGE="$1"
    local JOBS=$2
    shift 2

    display_message "$MESSAGE"

    # Build the current directory.
    make_current_directory $JOBS "$@"
}

build_from_travis()
{
    local ACCOUNT=$1
    local REPO=$2
    local BRANCH=$3
    local JOBS=$4
    shift 4

    # The primary build is not downloaded if we are running in Travis.
    if [[ $TRAVIS == true ]]; then
        # TODO: enable so build-dir in travis can be absolute or multi-segment.
        # push_directory "$TRAVIS_BUILD_DIR"
        push_directory ".."
        build_from_local "Local $TRAVIS_REPO_SLUG" $JOBS "$@"
        make_tests $JOBS
        pop_directory
    else
        build_from_github $ACCOUNT $REPO $BRANCH $JOBS "$@"
        push_directory $REPO
        make_tests $JOBS
        pop_directory
    fi
}


# The master build function.
#==============================================================================
build_all()
{
    build_from_tarball_boost $BOOST_URL $BOOST_ARCHIVE boost $PARALLEL $BOOST_OPTIONS
    build_from_github libbitcoin secp256k1 version2 $PARALLEL "$@" $SECP256K1_OPTIONS
    build_from_travis libbitcoin libbitcoin version2 $PARALLEL "$@" $BITCOIN_OPTIONS
}


# Build the primary library and all dependencies.
#==============================================================================
create_directory "$BUILD_DIR"
push_directory "$BUILD_DIR"
initialize_git   
time build_all "${CONFIGURE_OPTIONS[@]}"
pop_directory
