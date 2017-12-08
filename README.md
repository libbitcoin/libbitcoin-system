[![Build Status](https://travis-ci.org/libbitcoin/libbitcoin.svg?branch=master)](https://travis-ci.org/libbitcoin/libbitcoin)

[![Coverage Status](https://coveralls.io/repos/libbitcoin/libbitcoin/badge.svg)](https://coveralls.io/r/libbitcoin/libbitcoin)

# Libbitcoin

*The Bitcoin Development Library*

[Documentation](https://github.com/libbitcoin/libbitcoin/wiki) is available on the wiki.

**License Overview**

All files in this repository fall under the license specified in [COPYING](COPYING). The project is licensed as [AGPL with a lesser clause](https://www.gnu.org/licenses/agpl-3.0.en.html). It may be used within a proprietary project, but the core library and any changes to it must be published online. Source code for this library must always remain free for everybody to access.

**About Libbitcoin**

The libbitcoin toolkit is a set of cross platform C++ libraries for building bitcoin applications. The toolkit consists of several libraries, most of which depend on the foundational [libbitcoin](https://github.com/libbitcoin/libbitcoin) library. Each library's repository can be cloned and built using common [automake](http://www.gnu.org/software/automake) 1.14+ instructions. There are no packages yet in distribution however each library includes an installation script (described below) which is regularly verified in the automated build.

## Installation

On Linux and macOS libbitcoin is built using Autotools as follows.
```sh
$ ./autogen.sh
$ ./configure
$ make
$ sudo make install
$ sudo ldconfig
```
A minimal libbitcoin build requires boost and libsecp256k1. The [libbitcoin/secp256k1](https://github.com/libbitcoin/secp256k1) repository is forked from [bitcoin-core/secp256k1](https://github.com/bitcoin-core/secp256k1) in order to control for changes and to incorporate the necessary Visual Studio build. The original repository can be used directly but recent changes to the public interface may cause build breaks. The `--enable-module-recovery` switch is required.

Detailed instructions are provided below.

  * [Debian/Ubuntu](#debianubuntu)
  * [Macintosh](#macintosh)
  * [Windows](#windows)
  * [CMake](#cmake)

### Debian/Ubuntu

Libbitcoin requires a C++11 compiler, currently minimum [GCC 4.8.0](https://gcc.gnu.org/projects/cxx0x.html) or Clang based on [LLVM 3.5](http://llvm.org/releases/3.5.0/docs/ReleaseNotes.html).

To see your GCC version:
```sh
$ g++ --version
```
```
g++ (Ubuntu 4.8.2-19ubuntu1) 4.8.2
Copyright (C) 2013 Free Software Foundation, Inc.
This is free software; see the source for copying conditions.  There is NO
warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
```
If necessary, upgrade your compiler as follows:
```sh
$ sudo apt-get install g++-4.8
$ sudo update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-4.8 50
$ sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-4.8 50
$ sudo update-alternatives --install /usr/bin/gcov gcov /usr/bin/gcov-4.8 50
```
Next install the [build system](http://wikipedia.org/wiki/GNU_build_system) (Automake minimum 1.14) and git:
```sh
$ sudo apt-get install build-essential autoconf automake libtool pkg-config git
```
Next install the [Boost](http://www.boost.org) (minimum 1.57.0) development package:
```sh
$ sudo apt-get install libboost-all-dev
```
Next download the [install script](https://github.com/libbitcoin/libbitcoin/blob/version3/install.sh) and enable execution:
```sh
$ wget https://raw.githubusercontent.com/libbitcoin/libbitcoin/version3/install.sh
$ chmod +x install.sh
```
Finally install libbitcoin with default [build options](#build-notes-for-linux--macos):
```sh
$ sudo ./install.sh
```

Libbitcoin is now installed in `/usr/local/`.

### Macintosh

The macOS installation differs from Linux in the installation of the compiler and packaged dependencies. Libbitcoin supports both [Homebrew](http://brew.sh) and [MacPorts](https://www.macports.org) package managers. Both require Apple's [Xcode](https://developer.apple.com/xcode) command line tools. Neither requires Xcode as the tools may be installed independently.

Libbitcoin compiles with Clang on macOS and requires C++11 support. Installation has been verified using Clang based on [LLVM 3.5](http://llvm.org/releases/3.5.0/docs/ReleaseNotes.html). This version or newer should be installed as part of the Xcode command line tools.

To see your Clang/LLVM  version:
```sh
$ clang++ --version
```
You may encounter a prompt to install the Xcode command line developer tools, in which case accept the prompt.
```
Apple LLVM version 6.0 (clang-600.0.54) (based on LLVM 3.5svn)
Target: x86_64-apple-darwin14.0.0
Thread model: posix
```
If required update your version of the command line tools as follows:
```sh
$ xcode-select --install
```

#### Using Homebrew

First install [Homebrew](https://brew.sh). 

Next install the [build system](http://wikipedia.org/wiki/GNU_build_system) (Automake minimum 1.14) and [wget](http://www.gnu.org/software/wget):
```sh
$ brew install autoconf automake libtool pkgconfig wget
```
Next install the [Boost](http://www.boost.org) (1.57.0 or newer) development package:
```sh
$ brew install boost
```
Next download the [install script](https://github.com/libbitcoin/libbitcoin/blob/version3/install.sh) and enable execution:
```sh
$ wget https://raw.githubusercontent.com/libbitcoin/libbitcoin/version3/install.sh
$ chmod +x install.sh
```
Finally install libbitcoin with default [build options](#build-notes-for-linux--macos):
```sh
$ ./install.sh
```

Libbitcoin is now installed in `/usr/local/`.

##### Installing from Formula

Instead of building, libbitcoin can be installed from a formula:
```sh
$ brew install libbitcoin
```

#### Using MacPorts

First install [MacPorts](https://www.macports.org/install.php).

Next install the [build system](http://wikipedia.org/wiki/GNU_build_system) (Automake minimum 1.14) and [wget](http://www.gnu.org/software/wget):
```sh
$ sudo port install autoconf automake libtool pkgconfig wget
```
Next install the [Boost](http://www.boost.org) (1.57.0 or newer) development package. The `-` options remove MacPort defaults that are not Boost defaults:
```sh
$ sudo port install boost -no_single -no_static -python27
```
Next download the [install script](https://github.com/libbitcoin/libbitcoin/blob/version3/install.sh) and enable execution:
```sh
$ wget https://raw.githubusercontent.com/libbitcoin/libbitcoin/version3/install.sh
$ chmod +x install.sh
```
Finally install libbitcoin with default [build options](#build-notes-for-linux--macos):
```sh
$ ./install.sh
```

Libbitcoin is now installed in `/usr/local/`.

### Build Notes for Linux / macOS
The [install script](https://github.com/libbitcoin/libbitcoin/blob/version3/install.sh) itself is commented so that the manual build steps for each dependency can be inferred by a developer.

You can run the install script from any directory on your system. By default this will build libbitcoin in a subdirectory named `build-libbitcoin` and install it to `/usr/local/`. The install script requires `sudo` only if you do not have access to the installation location, which you can change using the `--prefix` option on the installer command line.

The build script clones, builds and installs two unpackaged repositories, namely:

- [libbitcoin/secp256k1](https://github.com/libbitcoin/secp256k1)
- [libbitcoin/libbitcoin](https://github.com/libbitcoin/libbitcoin)

The script builds from the head of their `version4` and `version3` branches respectively. The `master` branch is a staging area for changes. The version branches are considered release quality.

#### Build Options

Any set of `./configure` options can be passed via the build script, for example:
```sh
$ ./install.sh CFLAGS="-Og -g" --prefix=/home/me/myprefix
```

#### Compiling with ICU (International Components for Unicode)

Since the addition of [BIP-39](https://github.com/bitcoin/bips/blob/master/bip-0039.mediawiki) and later [BIP-38](https://github.com/bitcoin/bips/blob/master/bip-0038.mediawiki) support, libbitcoin conditionally incorporates [ICU](http://site.icu-project.org). To use the BIP-38 and BIP-39 passphrase normalization features libbitcoin must be compiled with the `--with-icu` option. Currently [libbitcoin-explorer](https://github.com/libbitcoin/libbitcoin-explorer) is the only other library that accesses this feature, so if you do not intend to use passphrase normalization this dependency can be avoided.
```sh
$ ./install.sh --with-icu
```

#### Compiling with QR Code Support

Since the addition of [qrcode](https://github.com/evoskuil/libbitcoin/blob/master/src/wallet/qrcode.cpp) support, libbitcoin conditionally incorporates `qrencode`. This requires compiling with the `--with-qrencode` option. Currently [libbitcoin-explorer](https://github.com/libbitcoin/libbitcoin-explorer) is the only other library that accesses this feature, so if you do not intend to use qrcode this dependency can be avoided.
```sh
$ ./install.sh --with-qrencode
```

Since the addition of [png](https://github.com/evoskuil/libbitcoin/blob/master/src/utility/png.cpp) support, libbitcoin conditionally incorporates `libpng` (which in turn requires `zlib`). This requires compiling with the `--with-png` option. Currently [libbitcoin-explorer](https://github.com/libbitcoin/libbitcoin-explorer) is the only other library that accesses this feature, so if you do not intend to use png this dependency can be avoided.
```sh
$ ./install.sh --with-png
```

#### Building ICU, ZLib, PNG, QREncode and/or Boost

The installer can download and install any or all of these dependencies. ICU is a large package that is not typically preinstalled at a sufficient level. Using these builds ensures compiler and configuration compatibility across all of the build components. It is recommended to use a prefix directory when building these components.
```sh
$ ./install.sh --with-icu --with-png --with-qrencode --build-icu --build-zlib --build-png --build-qrencode --build-boost --prefix=/home/me/myprefix
```

### Windows

Visual Studio solutions are maintained for all libbitcoin libraries. NuGet packages exist for dependencies with the exceptions of the optional ZLib, PNG, and QREncode (required for QR code functionality). ICU is integrated into Windows and therefore not required as an additional dependency when using ICU features.

> The libbitcoin execution environment supports `Windows XP Service Pack 2` and newer.

### CMake

CMake has been tested on Visual Studio 2017 and CentOS Linux release 7.4.1708

`BOOST_ROOT` needs to be set to base directory of your boost version
`SECP256K1_ROOT` may be set if the library is in its default `/usr/local` location

Windows example:
For Windows SECP256K1_ROOT must be set to base dir of compiled library
```sh
cd libbitcoin
mkdir build
cd build
cmake.exe -G "Visual Studio 15 2017 Win64" -DBOOST_ROOT=/c/builds/boost_1_64_0 -DSECP256K1_ROOT=/c/builds/secp256k1/ ..
```
From there you will find a libbitcoin.sln generated from which you can build.

Linux example:
```sh
cd libbitcoin
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release -DBOOST_ROOT=/root/builds/boost_1_65_1/ ..
make
```

#### Upgrade Compiler

Libbitcoin requires a C++11 compiler, which means **Visual Studio 2013** minimum. Additionally a pre-release compiler must be installed as an update to Visual Studio. Download and install the following tools as necessary. Both are available free of charge:

* [Visual Studio 2013 Express](https://my.visualstudio.com/Downloads?q=visual%20studio%202013&wt.mc_id=o~msft~vscom~older-downloads)
* [November 2013 CTP Compiler](http://www.microsoft.com/en-us/download/details.aspx?id=41151)
* See also: [CTP Compiler installation issue](http://stackoverflow.com/a/34548651/1172329)

#### Create Local NuGet Repository

Dependencies apart from the libbitcoin libraries are available as [NuGet packages](https://www.nuget.org/packages?q=evoskuil). The libbitcoin solution files are configured with references to these packages. To avoid redundancies these references expect a [NuGet.config](http://docs.nuget.org/docs/release-notes/nuget-2.1) in a central location.

The required set of NuGet packages can be viewed using the [NuGet package manager](http://docs.nuget.org/docs/start-here/managing-nuget-packages-using-the-dialog) from the libbitcoin solution. The NuGet package manager will automatically download missing packages, either from the build scripts or after prompting you in the Visual Studio environment. For your reference these are the required packages:

* Packages maintained by [sergey.shandar](http://www.nuget.org/profiles/sergey.shandar)
 * [boost](http://www.nuget.org/packages/boost)
 * [boost\_chrono-vc120](http://www.nuget.org/packages/boost_chrono-vc120)
 * [boost\_date\_time-vc120](http://www.nuget.org/packages/boost_date_time-vc120)
 * [boost\_filesystem-vc120](http://www.nuget.org/packages/boost_filesystem-vc120)
 * [boost\_iostreams-vc120](http://www.nuget.org/packages/boost_iostreams-vc120)
 * [boost\_locale-vc120](http://www.nuget.org/packages/boost_locale-vc120)
 * [boost\_log-vc120](http://www.nuget.org/packages/boost_log-vc120)
 * [boost\_program\_options-vc120](http://www.nuget.org/packages/boost_program_options-vc120)
 * [boost\_regex-vc120](http://www.nuget.org/packages/boost_regex-vc120)
 * [boost\_system-vc120](http://www.nuget.org/packages/boost_system-vc120)
 * [boost\_thread-vc120](http://www.nuget.org/packages/boost_thread-vc120)
 * [boost\_unit\_test\_framework-vc120](http://www.nuget.org/packages/boost_unit_test_framework-vc120)
* Packages maintained by [evoskuil](http://www.nuget.org/profiles/evoskuil)
 * [secp256k1\_vc120](http://www.nuget.org/packages/secp256k1_vc120)

#### Build Libbitcoin Projects

After cloning the the repository the libbitcoin build can be performed manually (from within Visual Studio) or using the `buildall.bat` script provided in the `builds\msvc\build\` subdirectory. The scripts automatically download the required NuGet packages.

> Tip: The `buildall.bat` script builds *all* valid configurations. The build time can be significantly reduced by disabling all but the desired configuration in `buildbase.bat`.

> The libbitcoin dynamic (DLL) build configurations do not compile, as the exports have not yet been fully implemented. These are currently disabled in the build scripts but you will encounter numerous errors if you build then manually.

#### Optional: Building secp256k1

The secp256k1 package above is maintained using the same [Visual Studio template](https://github.com/evoskuil/visual-studio-template) as all libbitcoin libraries. If so desired it can be built locally, in the same manner as libbitcoin.

* [libbitcoin/secp256k1/version4](https://github.com/libbitcoin/secp256k1/tree/version4/builds/msvc)

This change is properly accomplished by disabling the "NuGet Dependencies" in the Visual Studio properties user interface and then importing `secp256k1.import.props`, which references `secp256k1.import.xml`.
