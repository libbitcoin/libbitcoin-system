#!/bin/bash
#
# Script to build and install Libbitcoin and unpackaged dependencies.
#
# This build script is based on a few ideas.
# -----------------------------------------------------------------------------
#  0. Start Clean: make reliable by never building from cached results.
#  1. Fail Fast: make all errors fail the build so that none are overlooked.
#  3. Self-Contain: build everything that needs to be built from one file.
#  2. Validate Dependencies: detect dependency breaks before they ship.
#  3. Validate Deployment: this file is both deployment and verification build.
#  3. Be Declarative: make behavior obvious by not using conditional statements.
#  4. Be Explicit: not everyone speaks the same code or human languages.
#  5. Use Least Privilege: don't require sudo for the entire script.
#  6. Do Not Repeat Yourself: do not repeat yourself.

# This script will build libbitcoin using this relative directory.
# This is meant to be temporary, just to facilitate the install.

if [ "$TRAVIS" = "true" ]; then
    PARALLEL="1"

    echo "Detected travis install, setting to non-parallel: $PARALLEL"
else
    NPROC=$(nproc)
    PARALLEL="$NPROC"

    echo "Detected cores for parallel make: $PARALLEL"
fi

SEQUENTIAL="1"

BUILD_DIRECTORY="libbitcoin_build"

# The source repository for the primary build (when not running in Travis).
BUILD_ACCOUNT="libbitcoin"
BUILD_REPO="libbitcoin"
BUILD_BRANCH="develop"

# https://github.com/bitcoin/secp256k1
SECP256K1_OPTIONS=\
"--with-bignum=gmp "\
"--with-field=gmp "\
"--enable-benchmark=no "\
"--enable-tests=no "\
"--enable-endomorphism=no"

# http://bit.ly/1pKbuFP
BOOST_UNIT_TEST_PARAMETERS=\
"--run_test=* "\
"--random=1 "\
"--show_progress=yes "\
"--result_code=no "\
"--detect_memory_leak=0 "\
"--report_level=no "\
"--build_info=yes"

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
    ./configure "$@"

    if [ "$JOBS" -gt "$SEQUENTIAL" ]; then
        make "-j$JOBS"
    else
        make 
    fi

    sudo make install
    sudo ldconfig
}

build_from_github()
{
    ACCOUNT=$1
    REPO=$2
    BRANCH=$3
    JOBS=$4

    # Shift the first three parameters out of @.
    shift 4

    # Show the user what repo we are building.
    FORK="$ACCOUNT/$REPO"
    display_message "Download $FORK/$BRANCH"
    
    # Clone the repo locally.
    rm -rf $REPO
    git clone --branch $BRANCH --single-branch "https://github.com/$FORK"

    # Build the local repo clone.
    pushd $REPO
    automake_current_directory "$JOBS" "$@"
    popd
}

build_primary()
{
    JOBS=$1
    shift 1

    if [ "$TRAVIS" = "true" ]; then
        # If the environment is Travis drop out of build directory.
        cd ..
        display_message "Local $TRAVIS_REPO_SLUG"
	    automake_current_directory "$JOBS" "$@"
        build_tests
    else
        # Otherwise we pull the primary repo down for the single file install.
        build_from_github $BUILD_ACCOUNT $BUILD_REPO $BUILD_BRANCH "$JOBS" "$@"

        # Build the tests and drop out of build directory.
        pushd $BUILD_REPO
        build_tests "$JOBS"
        popd
        cd ..
    fi
}

build_tests()
{
    JOBS=$1

    # Build and run unit tests relative to the primary directory.
    if [ "$JOBS" -gt "$SEQUENTIAL" ]; then
        TEST_FLAGS="$BOOST_UNIT_TEST_PARAMETERS" make check "-j$JOBS"
    else
        TEST_FLAGS="$BOOST_UNIT_TEST_PARAMETERS" make check
    fi
}

clean_usr_local()
{
    # Remove previous usr/local libbitcoin installs (not all dependencies).
    # Only installations conforming to the directory structure are cleaned.

    # Includes
    sudo rm --force /usr/local/include/bitcoin/bitcoin.hpp
    sudo rm --force --recursive /usr/local/include/bitcoin/bitcoin

    # Archives
    sudo rm --force /usr/local/lib/libbitcoin.a
    sudo rm --force /usr/local/lib/libbitcoin.la
    sudo rm --force /usr/local/lib/libbitcoin.so
    sudo rm --force /usr/local/lib/libbitcoin.so.*
}

create_build_directory()
{
    # Notify that this script will do something destructive.
    echo "This script will erase and build in: $BUILD_DIRECTORY"

    # Cache credentials for subsequent sudo calls.
    sudo rm --force --recursive $BUILD_DIRECTORY
    mkdir $BUILD_DIRECTORY
    cd $BUILD_DIRECTORY

    # Initialize git repository at the root of the build directory.
    git init
    git config user.name anonymous
}

build_library()
{
    # Purge previous installations.
    clean_usr_local

    # Create and move to a temporary build directory.
    create_build_directory

    # Download, build and install all unpackaged dependencies.
    build_from_github bitcoin secp256k1 master "$SEQUENTIAL" "$@" $SECP256K1_OPTIONS

    # The primary build is not downloaded if we are running in Travis.
    build_primary "$PARALLEL" "$@"

    # If the build succeeded clean up the build directory.
    delete_build_directory
}

delete_build_directory()
{
    # This precludes use of a 'make uninstall' however that would need to be 
    # applied to dependencies as well. Typically each time a git pull occurs 
    # into a build directory the uninstall is potentially invalidated. This 
    # approach allows us to perform a complete uninstall across all versions.
    sudo rm --force --recursive $BUILD_DIRECTORY
}

# Exit this script on the first error (any statement returns non-true value).
set -e

# Build the primary library and its unpackaged dependencies.
build_library "$@"

