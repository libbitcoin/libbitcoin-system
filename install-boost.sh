#!/bin/bash
#
# Script to build and install selected Boost features.
#

build_boost()
{
    echo '*************************** install boost **************************'

    # Establish boost build parameters.
    # Boost thread (not currently used) with gcc7/8 requires (1.49)
    # For more information see: https://svn.boost.org/trac/boost/ticket/6165
    BOOST_LIBS="date_time,filesystem,regex,system,test"
    BOOST_URL="http://sourceforge.net/projects/boost/files/boost/1.49.0/boost_1_49_0.tar.bz2/download"
    BOOST_ARCHIVE="boost_1_49_0.tar.bz2"

    # Download archive.
    wget --output-document $BOOST_ARCHIVE $BOOST_URL

    # Extract archive into new the current directory.
    tar --extract --file $BOOST_ARCHIVE --bzip2 --strip-components=1

    # Prepare the install with the specified boost libraries.
    ./bootstrap.sh --with-libraries=$BOOST_LIBS
    
    # Build and install, disabling ICU for build consistency across platforms.
    ./b2 -d0 -q --disable-icu install
}

# Establish shared build parameters.
BUILD_DIRECTORY="BOOST"

# Exit this script on the first error (any statement returns non-true value).
set -e

# Warn that this script will do something destructive.
echo "This script will erase and build in the subdirectory: "$BUILD_DIRECTORY

# Create and move to the build directory.
rm -rf $BUILD_DIRECTORY
mkdir $BUILD_DIRECTORY
cd $BUILD_DIRECTORY

# Build boost in the current directory.
build_boost

# Export the standard boost directory variable so dependants can find boost.
export BOOST_ROOT=$BOOST_DIRECTORY

