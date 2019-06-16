[![Build Status](https://travis-ci.org/libbitcoin/libbitcoin-system.svg?branch=master)](https://travis-ci.org/libbitcoin/libbitcoin-system)

[![Coverage Status](https://coveralls.io/repos/libbitcoin/libbitcoin/badge.svg)](https://coveralls.io/r/libbitcoin/libbitcoin)

# Libbitcoin

*The Bitcoin Development Library*

[Documentation](https://github.com/libbitcoin/libbitcoin/wiki) is available on the wiki.

**License Overview**

All files in this repository fall under the license specified in [COPYING](COPYING). The project is licensed as [AGPL with a lesser clause](https://www.gnu.org/licenses/agpl-3.0.en.html). It may be used within a proprietary project, but the core library and any changes to it must be published online. Source code for this library must always remain free for everybody to access.

**About Libbitcoin**

The libbitcoin toolkit is a set of cross platform C++ libraries for building bitcoin applications. The toolkit consists of several libraries, most of which depend on the foundational [libbitcoin](https://github.com/libbitcoin/libbitcoin) library. Each library's repository can be cloned and built using common [automake](http://www.gnu.org/software/automake) 1.14+ instructions. There are no packages yet in distribution however each library includes an installation script (described below) which is regularly verified in the automated build.

## Installation

The master branch is a staging area for the next major release and should be used only by libbitcoin developers. The current release branch is version3. Detailed installation instructions are provided below.

  * [Debian/Ubuntu](#debianubuntu)
  * [Macintosh](#macintosh)
  * [Windows](#windows)

### Autotools (advanced users)

On Linux and macOS libbitcoin is built using Autotools as follows.
```sh
$ ./autogen.sh
$ ./configure
$ make
$ sudo make install
$ sudo ldconfig
```
A minimal libbitcoin build requires boost and libsecp256k1. The [libbitcoin/secp256k1](https://github.com/libbitcoin/secp256k1) repository is forked from [bitcoin-core/secp256k1](https://github.com/bitcoin-core/secp256k1) in order to control for changes and to incorporate the necessary Visual Studio build. The original repository can be used directly but recent changes to the public interface may cause build breaks. The `--enable-module-recovery` switch is required.

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
Next download the [install script](https://github.com/libbitcoin/libbitcoin/blob/version3/install.sh) and enable execution:
```sh
$ wget https://raw.githubusercontent.com/libbitcoin/libbitcoin/version3/install.sh
$ chmod +x install.sh
```
Finally install libbitcoin with recommended [build options](#build-notes-for-linux--macos):
```sh
$ ./install.sh --prefix=/home/me/myprefix --build-boost --disable-shared
```

Libbitcoin is now installed in `/home/me/myprefix/`.

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
Next download the [install script](https://github.com/libbitcoin/libbitcoin/blob/version3/install.sh) and enable execution:
```sh
$ wget https://raw.githubusercontent.com/libbitcoin/libbitcoin/version3/install.sh
$ chmod +x install.sh
```
Finally install libbitcoin with recommended [build options](#build-notes-for-linux--macos):
```sh
$ ./install.sh --prefix=/home/me/myprefix --build-boost --disable-shared
```

Libbitcoin is now installed in `/home/me/myprefix/`.

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
Next download the [install script](https://github.com/libbitcoin/libbitcoin/blob/version3/install.sh) and enable execution:
```sh
$ wget https://raw.githubusercontent.com/libbitcoin/libbitcoin/version3/install.sh
$ chmod +x install.sh
```
Finally install libbitcoin with default [build options](#build-notes-for-linux--macos):
```sh
$ ./install.sh --prefix=/home/me/myprefix --build-boost --disable-shared
```

Libbitcoin is now installed in `/home/me/myprefix/`.

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

Since the addition of [BIP-39](https://github.com/bitcoin/bips/blob/master/bip-0039.mediawiki) and later [BIP-38](https://github.com/bitcoin/bips/blob/master/bip-0038.mediawiki) and [Electrum](https://electrum.org) mnemnoic support, libbitcoin conditionally incorporates [ICU](http://site.icu-project.org). To use passphrase normalization for these features libbitcoin must be compiled with the `--with-icu` option. Currently [libbitcoin-explorer](https://github.com/libbitcoin/libbitcoin-explorer) is the only other library that accesses this feature, so if you do not intend to use passphrase normalization this dependency can be avoided.
```sh
$ ./install.sh --with-icu --build-icu --build-boost --disable-shared
```

#### Compiling with QR Code Support

Since the addition of [qrcode](https://github.com/evoskuil/libbitcoin/blob/master/src/wallet/qrcode.cpp) support, libbitcoin conditionally incorporates `qrencode`. This requires compiling with the `--with-qrencode` option. Currently [libbitcoin-explorer](https://github.com/libbitcoin/libbitcoin-explorer) is the only other library that accesses this feature, so if you do not intend to use qrcode this dependency can be avoided.
```sh
$ ./install.sh --with-qrencode --build-qrencode --build-boost --disable-shared
```

Since the addition of [png](https://github.com/evoskuil/libbitcoin/blob/master/src/utility/png.cpp) support, libbitcoin conditionally incorporates `libpng` (which in turn requires `zlib`). This requires compiling with the `--with-png` option. Currently [libbitcoin-explorer](https://github.com/libbitcoin/libbitcoin-explorer) is the only other library that accesses this feature, so if you do not intend to use png this dependency can be avoided.
```sh
$ ./install.sh --with-png --build-png --build-boost --disable-shared
```

#### Building ICU, ZLib, PNG, QREncode and/or Boost

The installer can download and install any or all of these dependencies. ICU is a large package that is not typically preinstalled at a sufficient level. Using these builds ensures compiler and configuration compatibility across all of the build components. It is recommended to use a prefix directory when building these components.
```sh
$ ./install.sh --prefix=/home/me/myprefix --with-icu --with-png --with-qrencode --build-icu --build-zlib --build-png --build-qrencode --build-boost --disable-shared
```

### Windows

Visual Studio solutions are maintained for all libbitcoin libraries. NuGet packages exist for dependencies with the exceptions of the optional ZLib, PNG, and QREncode (required for QR code functionality). ICU is integrated into Windows and therefore not required as an additional dependency when using ICU features.

> The libbitcoin execution environment supports `Windows XP Service Pack 2` and newer.

#### Supported Compilers

Libbitcoin requires a C++11 compiler, which means Visual Studio 2013 (with a pre-release compiler update) or later. Download and install one of the following free tools as necessary:

* [Visual Studio 2017 Express](https://www.visualstudio.com/downloads)
* [Visual Studio 2015 Express](https://www.visualstudio.com/vs/older-downloads)
* [Visual Studio 2013 Express](https://www.visualstudio.com/vs/older-downloads)
  * [November 2013 CTP Compiler for Visual Studio 2013](http://www.microsoft.com/en-us/download/details.aspx?id=41151)
  * [November 2013 CTP Compiler installation issue](http://stackoverflow.com/a/34548651/1172329)

#### NuGet Repository

Dependencies apart from the libbitcoin libraries are available as [NuGet packages](https://www.nuget.org):

* Packages maintained by [sergey.shandar](http://www.nuget.org/profiles/sergey.shandar)
  * [boost](http://www.nuget.org/packages/boost)
  * [boost\_atomic](http://www.nuget.org/packages/boost_atomic-vc120)
  * [boost\_chrono](http://www.nuget.org/packages/boost_chrono-vc120)
  * [boost\_date\_time](http://www.nuget.org/packages/boost_date_time-vc120)
  * [boost\_filesystem](http://www.nuget.org/packages/boost_filesystem-vc120)
  * [boost\_iostreams](http://www.nuget.org/packages/boost_iostreams-vc120)
  * [boost\_locale](http://www.nuget.org/packages/boost_locale-vc120)
  * [boost\_log](http://www.nuget.org/packages/boost_log-vc120)
  * [boost\_log_setup](http://www.nuget.org/packages/boost_log_setup-vc120)
  * [boost\_program\_options](http://www.nuget.org/packages/boost_program_options-vc120)
  * [boost\_regex](http://www.nuget.org/packages/boost_regex-vc120)
  * [boost\_system](http://www.nuget.org/packages/boost_system-vc120)
  * [boost\_thread](http://www.nuget.org/packages/boost_thread-vc120)
  * [boost\_unit\_test\_framework](http://www.nuget.org/packages/boost_unit_test_framework-vc120)
* Packages maintained by [evoskuil](http://www.nuget.org/profiles/evoskuil)
  * [secp256k1](http://www.nuget.org/packages/secp256k1_vc120)
  * [libzmq](http://www.nuget.org/packages/libzmq_vc120) [required for client-server repositories only]

The packages can be viewed using the [NuGet package manager](http://docs.nuget.org/docs/start-here/managing-nuget-packages-using-the-dialog) from the libbitcoin solution. The package manager will prompt for download of any missing packages.
  
The libbitcoin solution files are configured with references to these packages. The location of the NuGet repository is controlled by the [nuget.config](https://github.com/libbitcoin/libbitcoin/blob/master/builds/msvc/nuget.config) file `repositoryPath` setting **and** the `NuGetPackageRoot` element of **each** [\[project\].props](https://github.com/libbitcoin/libbitcoin/blob/master/builds/msvc/vs2017/libbitcoin/libbitcoin.props) file.

#### Build Libbitcoin Projects

After cloning the the repository the libbitcoin build can be performed from within Visual Studio or using the `build_all.bat` script provided in the `builds\msvc\build\` subdirectory. The script automatically downloads all required NuGet packages.

> Tip: The `build_all.bat` script builds *all* valid configurations for *all* compilers. The build time can be significantly reduced by disabling all but the desired configuration in `build_base.bat` and `build_all.bat`.

The libbitcoin dynamic (DLL) build configurations do not compile, as the exports have not yet been fully implemented. These are currently disabled in the build scripts but you will encounter numerous errors if you build then manually.

#### Optional: Building External Dependencies

The secp256k1 and libzmq package above are maintained using the same [Visual Studio template](https://github.com/evoskuil/visual-studio-template) as all libbitcoin libraries. If so desired these can be built locally, in the same manner as libbitcoin.

* [libbitcoin/secp256k1](https://github.com/libbitcoin/secp256k1/tree/version5/builds/msvc)
* [zeromq/libzmq](https://github.com/zeromq/libzmq/tree/master/builds/msvc)

This change is properly accomplished by disabling the "NuGet Dependencies" in the Visual Studio properties user interface and then importing `secp256k1.import.props`, which references `secp256k1.import.xml` and `libzmq.import.props`, which references `libzmq.import.xml`.

See [boost documentation](http://www.boost.org/doc/libs/1_57_0/more/getting_started/windows.html) for building boost libraries for Visual C++.
