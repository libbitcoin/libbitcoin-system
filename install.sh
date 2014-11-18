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
# --build-gmp              Builds GMP library.
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

# Define common values.
#==============================================================================
# The default build directory.
#------------------------------------------------------------------------------
BUILD_DIR="libbitcoin-build"

# Boost archives for linux.
#------------------------------------------------------------------------------
BOOST_URL_LINUX="http://sourceforge.net/projects/boost/files/boost/1.49.0/boost_1_49_0.tar.bz2/download"
BOOST_ARCHIVE_LINUX="boost_1_49_0.tar.bz2"

# Boost archives for darwin.
#------------------------------------------------------------------------------
BOOST_URL_DARWIN="http://sourceforge.net/projects/boost/files/boost/1.54.0/boost_1_54_0.tar.bz2/download"
BOOST_ARCHIVE_DARWIN="boost_1_54_0.tar.bz2"

# GMP archives.
#------------------------------------------------------------------------------
GMP_URL="https://ftp.gnu.org/gnu/gmp/gmp-6.0.0a.tar.bz2"
GMP_ARCHIVE="gmp-6.0.0a.tar.bz2"


# Define build options.
#==============================================================================
# Define boost options for linux.
#------------------------------------------------------------------------------
BOOST_OPTIONS_LINUX=\
"threading=single "\
"variant=release "\
"--disable-icu "\
"--with-date_time "\
"--with-filesystem "\
"--with-regex "\
"--with-system "\
"--with-test "\
"-d0 "\
"-q "

# Define boost options for darwin.
#------------------------------------------------------------------------------
BOOST_OPTIONS_DARWIN=\
"toolset=clang "\
"cxxflags=-stdlib=libc++ "\
"linkflags=-stdlib=libc++ "\
"threading=single "\
"variant=release "\
"--disable-icu "\
"--with-date_time "\
"--with-filesystem "\
"--with-regex "\
"--with-system "\
"--with-test "\
"-d0 "\
"-q "

# Define gmp options.
#------------------------------------------------------------------------------
GMP_OPTIONS=\
"CPPFLAGS=-w "

# Define secp256k1 options.
#------------------------------------------------------------------------------
SECP256K1_OPTIONS=\
"--with-bignum=gmp "\
"--with-field=gmp "\
"--enable-benchmark=no "\
"--enable-tests=no "\
"--enable-endomorphism=no "

# Define bitcoin options.
#------------------------------------------------------------------------------
BITCOIN_OPTIONS=\
"--enable-silent-rules "

# Define option to build without compiling tests.
#------------------------------------------------------------------------------
WITHOUT_TESTS=\
"--without-tests"


# Initialize values conditioned on build environment.
#==============================================================================
# Exit this script on the first build error.
#------------------------------------------------------------------------------
set -e

# Initialize build parallelism.
#------------------------------------------------------------------------------
SEQUENTIAL=1
OS=`uname -s`
if [[ $TRAVIS = "true" ]]; then
    PARALLEL=$SEQUENTIAL
elif [[ $OS = "Linux" ]]; then
    PARALLEL=`nproc`
elif [[ $OS = "Darwin" ]]; then
    PARALLEL=2 #TODO
else
    echo "Unsupported system: $OS"
    exit 1
fi
echo "Making for system: $OS"
echo "Allocated jobs: $PARALLEL"

# Configure OSX settings.
#------------------------------------------------------------------------------
if [[ $OS == "Darwin" ]]; then

    # Always require CLang, common lib linking will otherwise fail.
    export CC="clang"
    export CXX="clang++"
    
    # Always initialize prefix on OSX so default is consistent.
    PREFIX="/usr/local"
    
    BOOST_URL="$BOOST_URL_DARWIN"
    BOOST_ARCHIVE="$BOOST_ARCHIVE_DARWIN"
    BOOST_OPTIONS="$BOOST_OPTIONS_DARWIN"
else
    BOOST_URL="$BOOST_URL_LINUX"
    BOOST_ARCHIVE="$BOOST_ARCHIVE_LINUX"
    BOOST_OPTIONS="$BOOST_OPTIONS_LINUX"
fi


# Initialize values conditioned on command line arguments.
#==============================================================================
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

# Map standard libtool options to Boost link option.
#------------------------------------------------------------------------------
BOOST_LINK="static,shared"
if [[ $DISABLE_STATIC ]]; then
    BOOST_LINK="shared"
elif [[ $DISABLE_SHARED ]]; then
    BOOST_LINK="static"
fi
BOOST_OPTIONS="link=$BOOST_LINK $BOOST_OPTIONS"

# Incorporate the prefix.
#------------------------------------------------------------------------------
if [[ $PREFIX ]]; then

    # Add the prefix to the Boost build options (for Boost output).
    BOOST_OPTIONS="$BOOST_OPTIONS --prefix=$PREFIX"

    # Augment PKG_CONFIG_PATH with prefix path. 
    # If all libs support --with-pkgconfigdir we could avoid this variable.
    # Currently all dependencies with dependencies support it except secp256k1.
    export PKG_CONFIG_PATH="$PREFIX/lib/pkgconfig:$PKG_CONFIG_PATH"

    # Boost m4 discovery searches in the following order:
    # --with-boost=<path>, /usr, /usr/local, /opt, /opt/local, BOOST_ROOT.
    # We use --with-boost to prioritize the --prefix path when we build it.
    # Otherwise the standard paths suffice for Linux, Homebrew and MacPorts.
    
    # Set Boost discovery in the case of an internal build (no pkg-config).
    if [[ $BUILD_BOOST ]]; then
        WITH_BOOST="--with-boost=$PREFIX"
    fi
    
    # Set GMP discovery in the case of an internal build (no pkg-config).
    if [[ $BUILD_GMP ]]; then    
        WITH_GMP="CPPFLAGS=-I$PREFIX/include LDFLAGS=-L$PREFIX/lib"
    fi
    
    # Set Boost and GMP discovery information into dependent builds.
    SECP256K1_OPTIONS="$SECP256K1_OPTIONS $WITH_GMP"
    BITCOIN_OPTIONS="$BITCOIN_OPTIONS $WITH_GMP $WITH_BOOST"
fi


# Utility functions.
#==============================================================================
configure_options()
{
    echo "configure: $@"
    ./configure "$@"
}

create_directory()
{
    DIRECTORY="$1"

    rm -rf "$DIRECTORY"
    mkdir "$DIRECTORY"
}

display_linkage()
{
    LIBRARY="$1"
    
    # Display shared library links.
    if [[ $OS == "Darwin" ]]; then
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
    JOBS=$1
    shift 1

    ./autogen.sh
    configure_options "$@"
    make_silent $JOBS
    make install

    # Use ldconfig only in case of non --prefix installation on Linux.    
    if [[ ($OS == "Linux") && !($PREFIX)]]; then
        ldconfig
    fi
}

make_silent()
{
    JOBS=$1
    TARGET=$2
    
    # Avoid setting -j1 (causes problems on Travis).
    if [[ $JOBS -gt $SEQUENTIAL ]]; then
        make --silent -j$JOBS $TARGET
    else
        make --silent
    fi
}

make_tests()
{
    JOBS=$1

    # Build and run unit tests relative to the primary directory.
    make_silent $JOBS check
}

pop_directory()
{
    popd >/dev/null
}

push_directory()
{
    DIRECTORY="$1"
    
    pushd "$DIRECTORY" >/dev/null
}


# Build functions.
#==============================================================================
build_from_tarball_boost()
{
    URL=$1
    ARCHIVE=$2
    REPO=$3
    JOBS=$4
    shift 4

    if [[ !($BUILD_BOOST) ]]; then
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
    URL=$1
    ARCHIVE=$2
    REPO=$3
    JOBS=$4
    shift 4

    if [[ !($BUILD_GMP) ]]; then
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
    make_silent $JOBS >/dev/null
    echo "Installing all..."
    make install >/dev/null

    pop_directory
}

build_from_github()
{
    ACCOUNT=$1
    REPO=$2
    BRANCH=$3
    JOBS=$4
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
    MESSAGE="$1"
    JOBS=$2
    shift 2

    display_message "$MESSAGE"

    # Build the current directory.
    make_current_directory $JOBS "$@"
}

build_from_travis()
{
    ACCOUNT=$1
    REPO=$2
    BRANCH=$3
    JOBS=$4
    shift 4

    # The primary build is not downloaded if we are running in Travis.
    if [[ $TRAVIS == "true" ]]; then
        cd ..
        build_from_local "Local $TRAVIS_REPO_SLUG" $JOBS "$@"
        make_tests
        cd "$BUILD_DIR"
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
    build_from_tarball_gmp $GMP_URL $GMP_ARCHIVE gmp $PARALLEL "$@" $GMP_OPTIONS
    build_from_tarball_boost $BOOST_URL $BOOST_ARCHIVE boost $PARALLEL $BOOST_OPTIONS
    build_from_github bitcoin secp256k1 master $PARALLEL "$@" $SECP256K1_OPTIONS
    build_from_travis libbitcoin libbitcoin version2 $PARALLEL "$@" $BITCOIN_OPTIONS
}


# Build the primary library and all dependencies.
#==============================================================================
create_directory "$BUILD_DIR"
push_directory "$BUILD_DIR"
initialize_git   
time build_all "${CONFIGURE_OPTIONS[@]}"
pop_directory    
