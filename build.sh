#!/bin/bash
#
# Script to build and install Bitcoin Explorer and unpackaged dependencies.
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

# This script will build boost in this relative directory.
BUILD_DIRECTORY="BUILD"

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

github_build()
{
    # This function parameters.
    ACCOUNT=$1
    REPO=$2
    BRANCH=$3

    # Shift the first three parameters out of @.
    shift 3

    # Do the standard stuff.
    FORK=$ACCOUNT"/"$REPO
    echo
    echo "******************* install" $FORK"/"$BRANCH "**********************"
    echo 
    rm -rf $REPO
    git clone --branch $BRANCH --single-branch "https://github.com/"$FORK
    cd $REPO
    ./autogen.sh
    ./configure "$@"
    make
    sudo make install
    sudo ldconfig
    cd ..
}

build_explorer()
{
    # Initialize git repository at the root of the build directory.
    git init
    git config user.name anonymous

    # Download, build and install all unpackaged dependencies.
    # This script args are passed to configure of each build.
    github_build bitcoin secp256k1 master "$@" $SECP256K1_OPTIONS
    
    # Build only develop, and the above dependencies as necessary.
    github_build libbitcoin libbitcoin develop "$@"
    
    # Build and run unit tests.
    cd libbitcoin/test
    ./make.sh
    ./test_libbitcoin $BOOST_UNIT_TEST_PARAMETERS
    cd ../..
}

# Exit this script on the first error (any statement returns non-true value).
set -e

# Warn that this script will do something destructive.
echo "This script will erase and build in the subdirectory: "$BUILD_DIRECTORY

# Create and move to the build directory.
# Cache credentials for subsequent sudo calls and clean the build directory.
sudo rm -rf $BUILD_DIRECTORY
mkdir $BUILD_DIRECTORY
cd $BUILD_DIRECTORY

# Build explorer.
build_explorer "$@"

