[![Build Status](https://travis-ci.org/libbitcoin/libbitcoin.svg?branch=master)](https://travis-ci.org/libbitcoin/libbitcoin)

# Libbitcoin

*The Bitcoin Development Library*

**License Overview**

All files in this repository fall under the license specified in [COPYING](COPYING). The project is licensed as [AGPL with a lesser clause](https://wiki.unsystem.net/en/index.php/Libbitcoin/License). It may be used within a proprietary project, but the core library and any changes to it must be published online. Source code for this library must always remain free for everybody to access.

**About Libbitcoin**

The libbitcoin toolkit is a set of cross platform C++ libraries for building bitcoin applications. The toolkit consists of several libraries, most of which depend on the foundational [libbitcoin](https://github.com/libbitcoin/libbitcoin) library. Each library's repository can be cloned and built using common [automake](http://www.gnu.org/software/automake) instructions. There are no packages yet in distribution however each library includes an installation script (described below) which is regularly verified in the automated build.

**Table of Contents**

* Installation
  * [Debian/Ubuntu](#debianubuntu)
  * [Macintosh](#macintosh)
  * [Windows](#windows)
  * [Arch Linux](#arch-linux)
  * [Gentoo Linux](#gentoo-linux)

## Installation

### Debian/Ubuntu

Libbitcoin requires a C++11 compiler, currently [GCC 4.8.0](https://gcc.gnu.org/projects/cxx0x.html) minimum.

> For this reason Ubuntu is not supported prior to version 12.04.

To see your GCC version:

```sh
  $ gcc --version
```
If necessary, upgrade your compiler [as follows](http://bit.ly/1vXaaQL):
```sh
  $ sudo apt-get install g++-4.8
  $ sudo update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-4.8 50
  $ sudo update-alternatives --config g++
```
Next install the [build system](http://en.wikipedia.org/wiki/GNU_build_system):
```sh
  $ sudo apt-get install build-essential autoconf automake libtool pkg-config
```
Next install [Boost](http://www.boost.org) (1.49.0 or newer) and [GMP](https://gmplib.org/)  (5.0.0 or newer).
```sh
  $ sudo apt-get install libboost-all-dev libgmp-dev
```
Finally, execute the [install script](https://raw.githubusercontent.com/evoskuil/libbitcoin/develop/install.sh). This single file will download, build, install and test libbitcoin.
```sh
  $ ./install.sh
```
Libbitcoin is now installed in `/usr/local/`.

#### Notes

If you intend to inspect and/or modify source code you should [git clone](http://git-scm.com/docs/git-clone) libbitcoin and each unpackaged dependency and build them manually. The install script itself is commented so that the manual build steps for each dependency can be inferred by a developer.

You can run the install script from any directory on your system. This will build libbitcoin in a subdirectory named `libbitcoin_build` and install it to `/usr/local/`. When the build completes successfully the `libbitcoin_build` directory is deleted.

The install script should not normally be executed using sudo. Instead it will immediately prompt you for a super user password if required. This ensures that only the necessary installation steps are executed as a super user, as opposed to the entire build process.

The build script clones, builds and installs two unpackaged repositories, namely:

- [libbitcoin/secp256k1](https://github.com/libbitcoin/secp256k1)
- [libbitcoin/libbitcoin](https://github.com/libbitcoin/libbitcoin)

#### Build Options

Any set of `./configure` options can be passed via the build script, for example:

```sh
  $ ./install.sh CPPFLAGS=-DDEBUG CFLAGS="-Og -g"
```

#### Compiling for Testnet

Currently libbitcoin cannot work with both [testnet](https://en.bitcoin.it/wiki/Testnet) and mainnet. This restriction will be lifted in a future version. In order to work with testnet in the interim libbitcoin must be recompiled with the testnet option:
```sh
  $ ./install.sh --enable-testnet
```
#### Packaging Instructions

To build the Debian package execute the following commands:
```sh
  $ sudo apt-get install libboost-all-dev fakeroot
  $ dpkg-buildpackage -rfakeroot
```

### Macintosh

The OSX installation differs from Linux in the installation of the compiler and packaged dependencies.

To upgrade GCC first set the following environment variables:
```sh
  CC=/usr/local/bin/gcc-4.8
  CXX=/usr/local/bin/g++-4.8
```
Next execute the following commands:
```sh
  $ brew install boost gmp
  $ brew tap homebrew/versions
  $ brew install gcc48
  $ sudo ln -sf /usr/local/bin/g++-4.8 /usr/bin/g++
```
Finally, invoke the install script:
```sh
  $ ./install.sh
```

### Windows

Visual Studio solutions are maintained for all libbitcoin libraries and dependencies.

> The libbitcoin execution environment supports `Windows XP Service Pack 2` and newer.

#### Upgrade Compiler

Libbitcoin requires a C++11 compiler, which means **Visual Studio 2013** minimum. Additionally a pre-release compiler must be installed as an update to Visual Studio. Download and install the following tools as necessary. Both are available free of charge:

* [Visual Studio 2013 Express](http://www.microsoft.com/en-us/download/details.aspx?id=43733)
* [November 2013 CTP Compiler](http://www.microsoft.com/en-us/download/details.aspx?id=41151)

#### Create Local NuGet Repository

Dependencies apart from the libbitcoin libraries are available as [NuGet packages](https://www.nuget.org/packages?q=evoskuil). The libbitcoin solution files are configured with references to these packages. To avoid redundancies these references expect a [NuGet.config](http://docs.nuget.org/docs/release-notes/nuget-2.1) in a central location.

The required set of NuGet packages can be viewed using the [NuGet package manager](http://docs.nuget.org/docs/start-here/managing-nuget-packages-using-the-dialog) from the libbitcoin solution. The NuGet package manager will automatically download missing packages, either from the build scripts or after prompting you in the Visual Studio environment. For your reference these are the required packages:

* Packages maintained by [sergey.shandar](http://www.nuget.org/profiles/sergey.shandar)
 * [boost](http://www.nuget.org/packages/boost)
 * [boost\_date\_time-vc120](http://www.nuget.org/packages/boost_date_time-vc120)
 * [boost\_filesystem](http://www.nuget.org/packages/boost_filesystem)
 * [boost\_filesystem-vc120](http://www.nuget.org/packages/boost_filesystem-vc120)
 * [boost\_regex-vc120](http://www.nuget.org/packages/boost_regex-vc120)
 * [boost\_system-vc120](http://www.nuget.org/packages/boost_system-vc120)
 * [boost\_unit\_test\_framework-vc120](http://www.nuget.org/packages/boost_unit_test_framework-vc120)
* Packages maintained by [evoskuil](http://www.nuget.org/profiles/evoskuil)
 * [libgmp\_vc120](http://www.nuget.org/packages/libgmp_vc120)
 * [secp256k1\_gmp\_vc120](http://www.nuget.org/packages/secp256k1_gmp_vc120)

> The GMP for Windows project is called [MPIR](http://www.mpir.org) and has binary compatibility with GMP.

#### Build Libbitcoin Projects

After cloning the the repository the libbitcoin build can be performed manually (from within Visual Studio) or using the `buildall.bat` script provided in the `builds\msvc\build\` subdirectory. The scripts automatically download the required NuGet packages.

> Tip: The `buildall.bat` script builds *all* valid configurations. The build time can be significantly reduced by disabling all but the desired configuration in `buildbase.bat`.

> The libbitcoin dynamic (DLL) build configurations do not compile, as the exports have not yet been fully implemented. These are currently disabled in the build scripts but you will encounter numerous errors if you build then manually.

#### Optional: Build Everything

The non-boost packages above are all sourced from GitHub repositories maintained using the same [Visual Studio template](https://github.com/evoskuil/visual-studio-template) as the libbitcoin libraries. If so desired each of these can be built locally, in the same manner as the libbitcoin libraries above. This allows you to avoid using the pre-built NuGet packages. The repositories for each dependency are as follows:

* [evoskuil/mpir](https://github.com/evoskuil/mpir)
* [evoskuil/secp256k1](https://github.com/evoskuil/secp256k1)

This change is properly accomplished by disabling the "NuGet Dependencies" in the Visual Studio properties user interface and then importing the `.import.props` file(s) for the corresponding dependencies.

> TODO: Update libbitcoin with the .import.props files in a disabled configuration. This will allow this transition to be made entirely in the Visual Studio user interface. Then clarify the above explanation.

### Arch Linux

Libbitcoin is available from the [AUR](https://aur.archlinux.org/packages/libbitcoin-git).

### Gentoo Linux

Libbitcoin is available in the 'bitcoin' overlay. Use layman to fetch the overlay then emerge as usual.
If you want to use live build, unlock the `'**'` keyword for `net-p2p/libbitcoin` in `/etc/portage/package.accept_keywords`
