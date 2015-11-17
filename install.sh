#!/bin/bash
###############################################################################
#  Copyright (c) 2014-2015 libbitcoin developers (see COPYING).
#
#         GENERATED SOURCE CODE, DO NOT EDIT EXCEPT EXPERIMENTALLY
#
###############################################################################
# Script to build and install libbitcoin.
#
# Script options:
# --build-icu              Builds ICU libraries.
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
# Values (e.g. yes|no) in the '--disable-<linkage>' options are not supported.
# All command line options are passed to 'configure' of each repo, with
# the exception of the --build-<item> options, which are for the script only.
# Depending on the caller's permission to the --prefix or --build-dir
# directory, the script may need to be sudo'd.

# Define common constants.
#==============================================================================
# The default build directory.
#------------------------------------------------------------------------------
BUILD_DIR="build-libbitcoin"

# ICU archive.
#------------------------------------------------------------------------------
ICU_URL="http://download.icu-project.org/files/icu4c/55.1/icu4c-55_1-src.tgz"
ICU_ARCHIVE="icu4c-55_1-src.tgz"
ICU_STANDARD=\
"CXXFLAGS=-std=c++0x"

# Boost archive for gcc.
#------------------------------------------------------------------------------
BOOST_URL_GCC="http://github.com/libbitcoin/libbitcoin-build/blob/master/mirror/boost_1_55_0.tar.bz2?raw=true"
BOOST_ARCHIVE_GCC="boost_1_55_0.tar.bz2"
BOOST_STANDARD_GCC=\
"threading=multi "\
"variant=release "\
"-d0 "\
"-q"

# Boost archive for clang.
#------------------------------------------------------------------------------
BOOST_URL_CLANG="http://github.com/libbitcoin/libbitcoin-build/blob/master/mirror/boost_1_54_0.tar.bz2?raw=true"
BOOST_ARCHIVE_CLANG="boost_1_54_0.tar.bz2"
BOOST_STANDARD_CLANG=\
"threading=multi "\
"variant=release "\
"-d0 "\
"-q"


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
    PARALLEL=`sysctl -n hw.ncpu`
else
    echo "Unsupported system: $OS"
    exit 1
fi
echo "Make jobs: $PARALLEL"
echo "Make for system: $OS"

# Define operating system specific settings.
#------------------------------------------------------------------------------
if [[ $OS == Darwin ]]; then
    # Always require clang, common lib linking will otherwise fail.
    export CC="clang"
    export CXX="clang++"
    LIBC="libc++"
    
    # Always initialize prefix on OSX so default is useful.
    PREFIX="/usr/local"
else
    LIBC="libstdc++"
fi
echo "Make with cc: $CC"
echo "Make with cxx: $CXX"

# Define compiler specific settings.
#------------------------------------------------------------------------------
COMPILER="GCC"
if [[ $CXX == "clang++" ]]; then
    BOOST_TOOLS="toolset=clang cxxflags=-stdlib=$LIBC linkflags=-stdlib=$LIBC"  
    COMPILER="CLANG"
fi

# Parse command line options that are handled by this script.
#------------------------------------------------------------------------------
for OPTION in "$@"; do
    case $OPTION in
        # Custom build options (in the form of --build-<option>).
        (--build-icu)      BUILD_ICU="yes";;
        (--build-boost)    BUILD_BOOST="yes";;
        (--build-dir=*)    BUILD_DIR="${OPTION#*=}";;
        
        # Standard build options.
        (--prefix=*)       PREFIX="${OPTION#*=}";;
        (--disable-shared) DISABLE_SHARED="yes";;
        (--disable-static) DISABLE_STATIC="yes";;
        (--with-icu)       WITH_ICU="yes";;
    esac
done
echo "Build directory: $BUILD_DIR"
echo "Prefix directory: $PREFIX"

# Warn on configurations that imply static/prefix isolation.
#------------------------------------------------------------------------------
if [[ $BUILD_ICU == yes ]]; then
    if [[ !($PREFIX)]]; then    
        echo "Warning: --prefix recommended when building ICU."
    fi
    if [[ !($DISABLE_SHARED) ]]; then
        echo "Warning: --disable-shared recommended when building ICU."
    fi
fi
if [[ $BUILD_BOOST == yes ]]; then
    if [[ !($PREFIX)]]; then    
        echo "Warning: --prefix recommended when building boost."
    fi
    if [[ !($DISABLE_SHARED) ]]; then
        echo "Warning: --disable-shared recommended when building boost."
    fi
fi

# Purge custom options so they don't go to configure.
#------------------------------------------------------------------------------
CONFIGURE_OPTIONS=( "$@" )
CUSTOM_OPTIONS=( "--build-icu" "--build-boost" "--build-dir=$BUILD_DIR" )
for CUSTOM_OPTION in "${CUSTOM_OPTIONS[@]}"; do
    CONFIGURE_OPTIONS=( "${CONFIGURE_OPTIONS[@]/$CUSTOM_OPTION}" )
done

# Set link variables.
#------------------------------------------------------------------------------
if [[ $DISABLE_STATIC == yes ]]; then
    BOOST_LINK="link=shared"
    ICU_LINK="--enable-shared --disable-static"
elif [[ $DISABLE_SHARED == yes ]]; then
    BOOST_LINK="link=static"
    ICU_LINK="--disable-shared --enable-static"
else
    BOOST_LINK="link=static,shared"
    ICU_LINK="--enable-shared --enable-static"
fi

# Incorporate the prefix.
#------------------------------------------------------------------------------
if [[ $PREFIX ]]; then
    # Set the prefix-based package config directory.
    PREFIX_PKG_CONFIG_DIR="$PREFIX/lib/pkgconfig"

    # Augment PKG_CONFIG_PATH search path with our prefix.
    export PKG_CONFIG_PATH="$PKG_CONFIG_PATH:$PREFIX_PKG_CONFIG_DIR"
    
    # Set public prefix variable.
    prefix="--prefix=$PREFIX"
    
    # Set a package config save path that can be passed via our builds.
    with_pkgconfigdir="--with-pkgconfigdir=$PREFIX_PKG_CONFIG_DIR"
    
    if [[ $BUILD_BOOST ]]; then
        # Boost has no pkg-config, m4 searches in the following order:
        # --with-boost=<path>, /usr, /usr/local, /opt, /opt/local, $BOOST_ROOT.
        # We use --with-boost to prioritize the --prefix path when we build it.
        # Otherwise standard paths suffice for Linux, Homebrew and MacPorts.
        with_boost="--with-boost=$PREFIX" 
    fi
fi

# Echo published dynamic build options.
#------------------------------------------------------------------------------
echo "  prefix: ${prefix}"
echo "  with_boost: ${with_boost}"
echo "  with_pkgconfigdir: ${with_pkgconfigdir}"


# Define build options.
#==============================================================================
# Define icu options.
#------------------------------------------------------------------------------
ICU_OPTIONS=\
"--enable-draft "\
"--enable-tools "\
"--disable-extras "\
"--disable-icuio "\
"--disable-layout "\
"--disable-layoutex "\
"--disable-tests "\
"--disable-samples "

# Define boost options for gcc.
#------------------------------------------------------------------------------
BOOST_OPTIONS_GCC=\
"--with-chrono "\
"--with-date_time "\
"--with-filesystem "\
"--with-iostreams "\
"--with-locale "\
"--with-program_options "\
"--with-regex "\
"--with-system "\
"--with-thread "\
"--with-test "

# Define boost options for clang.
#------------------------------------------------------------------------------
BOOST_OPTIONS_CLANG=\
"--with-chrono "\
"--with-date_time "\
"--with-filesystem "\
"--with-iostreams "\
"--with-locale "\
"--with-program_options "\
"--with-regex "\
"--with-system "\
"--with-thread "\
"--with-test "

# Define secp256k1 options.
#------------------------------------------------------------------------------
SECP256K1_OPTIONS=\
"--disable-tests "

# Define bitcoin options.
#------------------------------------------------------------------------------
BITCOIN_OPTIONS=\
"${with_boost} "\
"${with_pkgconfigdir} "


# Define utility functions.
#==============================================================================
circumvent_boost_icu_detection()
{
    # Boost expects a directory structure for ICU which is incorrect.
    # Boost ICU discovery fails when using prefix, can't fix with -sICU_LINK,
    # so we rewrite the two 'has_icu_test.cpp' files to always return success.
    
    if [[ $WITH_ICU ]]; then
        local SUCCESS="int main() { return 0; }"
        local REGEX_TEST="libs/regex/build/has_icu_test.cpp"
        local LOCALE_TEST="libs/locale/build/has_icu_test.cpp"
        
        echo $SUCCESS > $REGEX_TEST
        echo $SUCCESS > $LOCALE_TEST

        echo "hack: ICU detection modified, will always indicate found."
    fi
}

configure_options()
{
    echo "configure: $@"
    ./configure "$@"
}

configure_links()
{
    # Configure dynamic linker run-time bindings.
    if [[ ($OS == Linux) && !($PREFIX) ]]; then
        ldconfig
    fi
}

create_directory()
{
    local DIRECTORY="$1"

    rm -rf "$DIRECTORY"
    mkdir "$DIRECTORY"
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

initialize_boost_icu()
{
    if [[ $WITH_ICU ]]; then
        # Restrict other local options when compiling boost with icu.
        BOOST_ICU_ONLY="boost.locale.iconv=off boost.locale.posix=off"
        
        # Extract ICU prefix directory from package config variable.
        local ICU_PREFIX=`pkg-config icu-i18n --variable=prefix`
        BOOST_ICU_PATH="-sICU_PATH=$ICU_PREFIX"
        BOOTSTRAP_WITH_ICU="--with-icu=$ICU_PREFIX"

        # Extract ICU libs from package config variables and augment with -ldl.
        local ICU_LIBS="`pkg-config icu-i18n --libs` -ldl"
        BOOST_ICU_LINK="-sICU_LINK=$ICU_LIBS"
    fi
}

initialize_icu_packages()
{
    if [[ $OS == Darwin && !($BUILD_ICU) ]]; then
        # Update PKG_CONFIG_PATH for ICU package installations on OSX.
        # OSX provides libicucore.dylib with no pkgconfig and doesn't support
        # renaming or important features, so we can't use that.
        HOMEBREW_ICU_PKG_CONFIG="/usr/local/opt/icu4c/lib/pkgconfig"
        MACPORTS_ICU_PKG_CONFIG="/opt/local/lib/pkgconfig"
        
        if [[ -d "$HOMEBREW_ICU_PKG_CONFIG" ]]; then
            export PKG_CONFIG_PATH="$PKG_CONFIG_PATH:$HOMEBREW_ICU_PKG_CONFIG"
        elif [[ -d "$MACPORTS_ICU_PKG_CONFIG" ]]; then
            export PKG_CONFIG_PATH="$PKG_CONFIG_PATH:$MACPORTS_ICU_PKG_CONFIG"
        fi
    fi
}

initialize_options()
{
    if [[ !($BOOST_OPTIONS) ]]; then
        # Select compiler-conditional generated configuration parameters.
        if [[ $COMPILER == CLANG ]]; then
            BOOST_URL=$BOOST_URL_CLANG
            BOOST_ARCHIVE=$BOOST_ARCHIVE_CLANG
            BOOST_STANDARD=$BOOST_STANDARD_CLANG
            BOOST_OPTIONS=$BOOST_OPTIONS_CLANG
        else
            BOOST_URL=$BOOST_URL_GCC
            BOOST_ARCHIVE=$BOOST_ARCHIVE_GCC
            BOOST_STANDARD=$BOOST_STANDARD_GCC
            BOOST_OPTIONS=$BOOST_OPTIONS_GCC
        fi
    fi
}

make_current_directory()
{
    local JOBS=$1
    shift 1

    ./autogen.sh
    configure_options "$@"
    make_jobs $JOBS
    make install
    configure_links
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
    # VERBOSE=1 ensures test-suite.log output sent to console (gcc).
    if ! make_jobs $JOBS check VERBOSE=1; then
        if [ -e "test-suite.log" ]; then
            echo "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<"
            echo "cat test-suite.log"
            echo "------------------------------"
            cat "test-suite.log"
            echo ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>"
        fi
        exit 1
    fi
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
build_from_tarball_icu()
{
    local URL=$1
    local ARCHIVE=$2
    local REPO=$3
    local JOBS=$4
    shift 4

    if [[ !($BUILD_ICU) ]]; then
        initialize_icu_packages
        display_message "ICU build not enabled"
        return
    fi

    display_message "Download $ARCHIVE"

    create_directory $REPO
    push_directory $REPO

    # Extract the source locally.
    wget --output-document $ARCHIVE $URL
    tar --extract --file $ARCHIVE --strip-components=1
    push_directory "source"

    # Build and install.
    # ICU is a typical GNU build except that it fails on unknown options.
    configure_options $ICU_LINK $ICU_STANDARD ${prefix} "$@"
    make_jobs $JOBS --silent
    make install
    configure_links

    pop_directory
    pop_directory
}

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
    
    # Circumvent Boost ICU detection bug.
    circumvent_boost_icu_detection

    initialize_boost_icu
    
    # Build and install.
    BOOSTSTRAP_OPTIONS="${prefix} $BOOTSTRAP_WITH_ICU"
    B2_OPTIONS="install --reconfigure -j $JOBS ${prefix} $BOOST_LINK $BOOST_TOOLS $BOOST_STANDARD $BOOST_ICU_PATH $BOOST_ICU_LINK $BOOST_ICU_ONLY $@"
    
    echo "bootstrap: $BOOSTSTRAP_OPTIONS"
    echo "b2: $B2_OPTIONS"
    echo
    
    ./bootstrap.sh $BOOSTSTRAP_OPTIONS
    ./b2 $B2_OPTIONS
    
    # Boost has no pkg-config, m4 searches in the following order:
    # --with-boost=<path>, /usr, /usr/local, /opt, /opt/local, $BOOST_ROOT.
    # We use --with-boost to prioritize the --prefix path when we build it.
    # Otherwise standard paths suffice for Linux, Homebrew and MacPorts.
    with_boost="--with-boost=$PREFIX"

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
    build_from_tarball_icu $ICU_URL $ICU_ARCHIVE icu $PARALLEL $ICU_OPTIONS
    build_from_tarball_boost $BOOST_URL $BOOST_ARCHIVE boost $PARALLEL $BOOST_OPTIONS
    build_from_github libbitcoin secp256k1 version3 $PARALLEL "$@" $SECP256K1_OPTIONS
    build_from_travis libbitcoin libbitcoin sync $PARALLEL "$@" $BITCOIN_OPTIONS
}


# Build the primary library and all dependencies.
#==============================================================================
create_directory "$BUILD_DIR"
push_directory "$BUILD_DIR"
initialize_git
initialize_options
time build_all "${CONFIGURE_OPTIONS[@]}"
pop_directory
