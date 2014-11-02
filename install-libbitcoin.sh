#!/bin/bash
#
# Script to build and install libbitcoin and unpackaged dependencies.
#

# The source repository for the primary build (when not running in Travis).
BUILD_ACCOUNT="libbitcoin"
BUILD_REPO="libbitcoin"
BUILD_BRANCH="develop"

# This script will build using this relative directory.
# This is meant to be temporary, just to facilitate the install.
BUILD_DIRECTORY="libbitcoin-build"

HOMEBREW_BOOST_ROOT_PATH=\
"/usr/local/opt/boost"

HOMEBREW_PKG_CONFIG_PATHS=\
"/usr/local/opt/gmp/lib/pkgconfig:"\
"/usr/local/opt/openssl/lib/pkgconfig"

# https://github.com/bitcoin/secp256k1
SECP256K1_OPTIONS=\
"--with-bignum=gmp "\
"--with-field=gmp "\
"--enable-benchmark=no "\
"--enable-tests=no "\
"--enable-endomorphism=no"

# Enable test compile in the primary build.
TEST_OPTIONS=\
"--with-tests=yes"

# Set SEQUENTIAL (always 1), PARALLEL (number of concurrent jobs) and OS.
SEQUENTIAL=1
PARALLEL=2
OS=$(uname -s)
if [[ $TRAVIS = "true" ]]; then
    PARALLEL=$SEQUENTIAL
elif [[ $OS = "Linux" ]]; then
    PARALLEL=$(nproc)
elif [[ $OS = "Darwin" ]]; then
    PARALLEL=2
    PREFIX="/usr/local"
else
    echo "Unsupported system: $OS"
    exit 1
fi

# Set PREFIX from --prefix=path option.
for i in "$@"; do
    case $i in
        (--prefix=*) PREFIX="${i#*=}";;
    esac
done

# Set PKG_CONFIG_PATH, BOOST_ROOT, CC and CXX
if [[ $OS == "Darwin" ]]; then
    export CC=clang
    export CXX=clang++
    export BOOST_ROOT=$HOMEBREW_BOOST_ROOT_PATH
    export PKG_CONFIG_PATH="$PKG_CONFIG_PATH:$HOMEBREW_PKG_CONFIG_PATHS"
fi
if [[ $PREFIX ]]; then
    export PKG_CONFIG_PATH="$PKG_CONFIG_PATH:$PREFIX/lib/pkgconfig"
fi

# Give user feedback on build configuration.
echo "Install prefix: $PREFIX"
echo "Allocated jobs: $PARALLEL"
echo "Making for system: $OS"
echo "Temp directory: $BUILD_DIRECTORY"
echo "Homebrew boost path: $BOOST_ROOT"
echo "Package config path: $PKG_CONFIG_PATH"
    
display_message()
{
    MESSAGE=$1
    echo
    echo "********************** $MESSAGE **********************"
    echo
}

automake_current_directory()
{
    JOBS=$1
    shift 1

    ./autogen.sh
    ./configure --enable-silent-rules "$@"

    # Avoid setting -j1.
    if [[ $JOBS -gt $SEQUENTIAL ]]; then
        make --silent -j$JOBS
    else
        make --silent
    fi

    make install

    # If sudo is required the caller should sudo the script.
    # Use ldconfig only in case of non --prefix installation on Linux.    
    if [[ ($OS == "Linux") && !($PREFIX)]]; then
        ldconfig
    fi
}

build_from_github()
{
    ACCOUNT=$1
    REPO=$2
    BRANCH=$3
    JOBS=$4
    shift 4

    # Show the user what repo we are building.
    FORK="$ACCOUNT/$REPO"
    display_message "Download $FORK/$BRANCH"
    
    # Clone the repo locally.
    rm -rf $REPO
    git clone --branch $BRANCH --single-branch "https://github.com/$FORK"

    # Build the local repo clone.
    pushd $REPO
    automake_current_directory $JOBS "$@"
    popd
}

build_primary()
{
    JOBS=$1
    shift 1

    if [[ $TRAVIS == "true" ]]; then
        # If the environment is Travis drop out of build directory.
        cd ..
        display_message "Local $TRAVIS_REPO_SLUG"
        automake_current_directory $JOBS "$@"
        build_tests
    else
        # Otherwise we pull the primary repo down for the single file install.
        build_from_github $BUILD_ACCOUNT $BUILD_REPO $BUILD_BRANCH $JOBS "$@"

        # Build the tests and drop out of build directory.
        pushd $BUILD_REPO
        build_tests $JOBS
        popd
        cd ..
    fi
}

build_tests()
{
    JOBS=$1

    # Build and run unit tests relative to the primary directory.
    if [[ $JOBS -gt $SEQUENTIAL ]]; then
        make check -j$JOBS
    else
        make check
    fi
}

create_build_directory()
{
    # By default will require privilege, use --prefix=<directory> to avoid.
    rm -rf $BUILD_DIRECTORY
    mkdir $BUILD_DIRECTORY
    cd $BUILD_DIRECTORY

    # Initialize git repository at the root of the build directory.
    git init
    git config user.name anonymous
}

build_library()
{
    # Create and move to a temporary build directory.
    create_build_directory

    # Download, build and install all unpackaged dependencies.
    build_from_github evoskuil secp256k1 osx-patch $SEQUENTIAL "$@" $SECP256K1_OPTIONS

    # The primary build is not downloaded if we are running in Travis.
    build_primary $PARALLEL "$@" $TEST_OPTIONS

    # If the build succeeded clean up the build directory.
    delete_build_directory
}

delete_build_directory()
{
    # This precludes use of a 'make uninstall' however that would need to be 
    # applied to dependencies as well. Typically each time a git pull occurs 
    # into a build directory the uninstall is potentially invalidated. This 
    # approach allows us to perform a complete uninstall across all versions.
    rm -rf $BUILD_DIRECTORY
}

# Exit this script on the first error (any statement returns non-true value).
set -e

# Build the primary library and its unpackaged dependencies.
build_library "$@"

