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

# Define common constants.
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


# Initialize the build environment.
#==============================================================================
# Exit this script on the first build error.
#------------------------------------------------------------------------------
set -e

# Configure build parallelism.
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

# Set public link variable (to translate the link type to the Boost build)
link="link=$BOOST_LINK"

# Incorporate the prefix.
#------------------------------------------------------------------------------
if [[ $PREFIX ]]; then

    # Set public with_pkgconfigdir variable (for packages that handle it).
    PKG_CONFIG_DIR="$PREFIX/lib/pkgconfig"
    with_pkgconfigdir="--with-pkgconfigdir=$PKG_CONFIG_DIR"
    
    # Augment PKG_CONFIG_PATH with prefix path. 
    # If all libs support --with-pkgconfigdir we could avoid this variable.
    # Currently all relevant dependencies support it except secp256k1.
    # TODO: patch secp256k1 and disable this.
    export PKG_CONFIG_PATH="$PKG_CONFIG_DIR:$PKG_CONFIG_PATH"

    # Boost m4 discovery searches in the following order:
    # --with-boost=<path>, /usr, /usr/local, /opt, /opt/local, BOOST_ROOT.
    # We use --with-boost to prioritize the --prefix path when we build it.
    # Otherwise the standard paths suffice for Linux, Homebrew and MacPorts.

    # Set public with_boost variable (because Boost has no pkg-config).
    if [[ $BUILD_BOOST ]]; then
        with_boost="--with-boost=$PREFIX"
    fi
    
    # Set public prefix_flags variable (because GMP has no pkg-config).
    if [[ $BUILD_GMP ]]; then    
        prefix_flags="CPPFLAGS=-I$PREFIX/include LDFLAGS=-L$PREFIX/lib"
    fi
    
    # Set public prefix variable (to tell Boost where to build).
    prefix="--prefix=$PREFIX"
fi

# Echo build options.
#------------------------------------------------------------------------------
echo "Building from:"
echo "  $BUILD_DIR"
echo "Published variable options:"
echo "  link: $link"
echo "  prefix: $prefix"
echo "  prefix_flags: $prefix"
echo "  with_boost: $with_boost"
echo "  with_pkgconfigdir: $with_pkgconfigdir"


# Define build options.
#==============================================================================
# Define gmp options.
#------------------------------------------------------------------------------
GMP_OPTIONS=\
"CPPFLAGS=-w "

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
"-q "\
"${prefix} "\
"${link} "

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
"-q "\
"${prefix} "\
"${link} "

# Define secp256k1 options.
#------------------------------------------------------------------------------
SECP256K1_OPTIONS=\
"--with-bignum=gmp "\
"--with-field=gmp "\
"--enable-benchmark=no "\
"--enable-tests=no "\
"--enable-endomorphism=no "\
"${prefix_flags} "

# Define bitcoin options.
#------------------------------------------------------------------------------
BITCOIN_OPTIONS=\
"--enable-silent-rules "\
"${prefix_flags} "\
"${with_boost} "\
"${with_pkgconfigdir} "


# Define operating system settings.
#==============================================================================
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
    local JOBS=$1
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
    local JOBS=$1
    local TARGET=$2

    # Avoid setting -j1 (causes problems on Travis).
    if [[ $JOBS -gt $SEQUENTIAL ]]; then
        make --silent -j$JOBS $TARGET
    else
        make --silent $TARGET
    fi
}

make_tests()
{
    local JOBS=$1

    # Build and run unit tests relative to the primary directory.
    make_silent $JOBS check
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
    local URL=$1
    local ARCHIVE=$2
    local REPO=$3
    local JOBS=$4
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
    if [[ $TRAVIS == "true" ]]; then
        cd ..
        build_from_local "Local $TRAVIS_REPO_SLUG" $JOBS "$@"
        make_tests $JOBS
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
