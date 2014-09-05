[![Build Status](https://travis-ci.org/libbitcoin/libbitcoin.svg?branch=develop)](https://travis-ci.org/libbitcoin/libbitcoin)

# Libbitcoin

*The Bitcoin Development Library*

**License Overview**

All files in this repository fall under the license specified in [COPYING](COPYING). The project is licensed as [AGPL with a lesser clause](https://wiki.unsystem.net/en/index.php/Libbitcoin/License). It may be used within a proprietary project, but the core library and any changes to it must be published online. Source code for this library must always remain free for everybody to access.

**Supported Platforms**

* [Debian/Ubuntu](#debianubuntu)
* [Macintosh](#macintosh)
* [Windows](#windows)
* [Arch Linux](#arch-linux)
* [Gentoo Linux](#gentoo-linux)

## Debian/Ubuntu

Libbitcoin requires a modern C++ compiler, **GCC 4.7.0** minimum. For this reason **Ubuntu is not supported prior to version 12.04**. To see your compiler version:
```
  $ gcc --version
```
If necessary, upgrade your compiler [as follows](http://bit.ly/1vXaaQL):
```
  $ sudo apt-get install g++-4.8
  $ sudo update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-4.8 50
  $ sudo update-alternatives --config g++
```
Next install the [build system](http://en.wikipedia.org/wiki/GNU_build_system):
```
  $ sudo apt-get install build-essential autoconf automake libtool pkg-config
```
Next install [Boost](http://www.boost.org) (1.50.0 or newer), [GMP](https://gmplib.org/)  (5.0.0 or newer), and [OpenSSL](https://www.openssl.org) (0.9.0 or newer) development packages. Note that we have [deprecated](https://www.peereboom.us/assl/assl/html/openssl.html) OpenSSL and it is currently used only for test validation.
```
  $ sudo apt-get install libboost-all-dev libgmp-dev libssl-dev
```
Finally, execute the [build script](https://raw.githubusercontent.com/evoskuil/libbitcoin/develop/build.sh). This single file will download, build, install and test libbitcoin.
```
  $ ./build.sh
```
Libbitcoin is now installed in `/usr/local/`.

### Notes

You can run the build script from any directory on your system. This will build and install libbitcoin in a new subdirectory named BUILD. If you are missing dependencies the build will terminate and you will be shown what is missing.

It should not normally be executed using sudo. It will immediately prompt you for a super user password if required. This ensures that only the necessary installation steps are executed as a super user, as opposed to the entire build process.

In addition to the packaged dependencies above, the build script clones, builds and installs two repositories, namely:

- [bitcoin/secp256k1](https://github.com/bitcoin/secp256k1)
- [libbitcoin/libbitcoin](https://github.com/libbitcoin/libbitcoin)

### Build Options

Any set of `./configure` options can be passed via the build script, for example:

```
  $ ./build.sh CPPFLAGS=-DDEBUG CFLAGS="-Og -g"
```

### Compiling for Testnet

Currently libbitcoin cannot work with both the **Test Network** (testnet) and mainnet. This restriction will be lifted in a future version. In order to work with testnet in the interim libbitcoin must be recompiled with the testnet option.:
```
  $ ./build.sh --enable-testnet
```
### Packaging Instructions
To build the Debian package execute the following commands:
```
  $ sudo apt-get install libboost-all-dev fakeroot
  $ dpkg-buildpackage -rfakeroot
```

## Macintosh

The OSX installation differs from Linux in the installation of the compiler and packaged dependencies.

To upgrade GCC first set the folloing environment variables:
```
  CC=/usr/local/bin/gcc-4.8
  CXX=/usr/local/bin/g++-4.8
```
Next execute the following commands:
```
  $ brew install boost gmp openssl
  $ brew tap homebrew/versions
  $ brew install gcc48
  $ sudo ln -sf /usr/local/bin/g++-4.8 /usr/bin/g++
```
Finally, invoke the installation script:
```
  $ ./build.sh
```

## Windows

Visual Studio solutions are provided for all libbitcoin libraries and dependencies. Because libbitcoin requires a `C++11` compiler, **Visual Studio 2013** is the most recent supported version and requires a modified pre-release compiler.

* [Visual Studio 2013 Express](http://www.visualstudio.com/en-us/products/visual-studio-express-vs.aspx) is available for [download](http://www.microsoft.com/en-us/download/details.aspx?id=40787) and is free of charge.
* The [November 2013 CTP Compiler](http://blogs.msdn.com/b/vcblog/archive/2013/11/18/announcing-the-visual-c-compiler-november-2013-ctp.aspx) must be [downloaded](http://www.microsoft.com/en-us/download/details.aspx?id=41151) and installed as well.

Dependencies apart from the libbitcoin repos are [available on NuGet](https://www.nuget.org/packages?q=evoskuil). Eventually the libbitcoin libraries and tools will be published as well. The solution files are already configured with references to the NuGet packages and expect a [central NuGet repo](http://docs.nuget.org/docs/release-notes/nuget-2.1). You should be able to restore the packages to the repo once it is properly configured.

Once the dependencies have been downloaded select a **statically-linked build** and build the solution. **The non-static (DLL) build configurations do not compile, as the exports have not yet been completed.**

## Arch Linux

Libbitcoin is available from the [AUR](https://aur.archlinux.org/packages/libbitcoin-leveldb-git).

## Gentoo Linux

Libbitcoin is available in the 'bitcoin' overlay. Use layman to fetch the overlay then emerge as usual.
If you want to use live build, unlock the `'**'` keyword for `net-p2p/libbitcoin` in `/etc/portage/package.accept_keywords`
