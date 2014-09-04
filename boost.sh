#!/bin/bash
#
# Script to build and install selected Boost features.
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

build_boost()
{
    echo '*************************** install boost **************************'

    # Establish boost build parameters.
    # Boost program_options.option_descriptions.canonical_display_name (1.50)
    # Boost thread (although not currently used) with gcc7/8 (1.49)
    # For more information see: https://svn.boost.org/trac/boost/ticket/6165
    BOOST_LIBS="chrono,date_time,filesystem,program_options,regex,serialization,system,test"
    BOOST_URL="http://sourceforge.net/projects/boost/files/boost/1.50.0/boost_1_50_0.tar.bz2/download"
    BOOST_ARCHIVE="boost_1_50_0.tar.bz2"

    # Download archive.
    wget --output-document $BOOST_ARCHIVE $BOOST_URL

    # Extract archive into new the current directory.
    tar --extract --file $BOOST_ARCHIVE --bzip2 --strip-components=1

    # Prepare the install with the specified boost libraries.
    ./bootstrap.sh --with-libraries=$BOOST_LIBS
    
    # Build and install, disabling ICU for build consistency across platforms.
    sudo ./b2 -d0 -q --disable-icu install
}

# Establish shared build parameters.
BUILD_DIRECTORY="BOOST"

# Exit this script on the first error (any statement returns non-true value).
set -e

# Warn that this script will do something destructive.
echo "This script will erase and build in the subdirectory: "$BUILD_DIRECTORY

# Create and move to the build directory.
# Cache credentials for subsequent sudo calls and clean the build directory.
sudo rm -rf $BUILD_DIRECTORY
mkdir $BUILD_DIRECTORY
cd $BUILD_DIRECTORY

# Build boost in the current directory.
build_boost

# Export the standard boost directory variable so dependants can find boost.
export BOOST_ROOT=$BOOST_DIRECTORY

