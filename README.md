[![Build Status](https://travis-ci.org/libbitcoin/libbitcoin.svg?branch=develop)](https://travis-ci.org/libbitcoin/libbitcoin)
# Libbitcoin

All files in this repository fall under the license specified in 
[COPYING](COPYING). The project is licensed as [AGPL with a lesser clause](https://wiki.unsystem.net/en/index.php/Libbitcoin/License).
It may be used within a proprietary project, but the core library and any
changes to it must be published online. Source code for this library must 
always remain free for everybody to access.

## Debian/Ubuntu Instructions

Libbitcoin requires a modern C++ compiler, **GCC 4.7.0** minimum. For this
reason **Ubuntu is not supported prior to version 12.04**. To see your 
compiler version:
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
Next install 
[Boost](http://www.boost.org) (1.50.0 or newer), 
[GMP](https://gmplib.org/)  (5.0.0 or newer), and
[OpenSSL](https://www.openssl.org) (0.9.0 or newer) packages. Note that we have 
[deprecated](https://www.peereboom.us/assl/assl/html/openssl.html)
OpenSSL and it is currently used only for test validation.
```
  $ sudo apt-get install libboost-all-dev libgmp-dev libssl-dev
```
Next download, build and install the Bitcoin elliptic curve library
[Secp256k1](https://github.com/bitcoin/secp256k1) (0.1.0 or newer).
The below configuration explicitly links GMP, but the default (OpenSSL) 
works as well.
```
  $ git clone https://github.com/bitcoin/secp256k1.git
  $ cd secp256k1
  
  $ ./autogen.sh
  $ ./configure --with-bignum=gmp --with-field=gmp
  $ make
  $ make install
  $ ldconfig
  
  $ cd ..
```
Finally, build and install libbitcoin:
```
  $ ./autogen.sh
  $ ./configure
  $ make
  $ sudo make install
  $ sudo ldconfig
```
Libbitcoin is now installed in /usr/local/.

### Configure Options

Configure (`./configure`) accepts a `--prefix` switch. This is useful if you
prefer to install libbitcoin to a local directory instead of globally. To 
install to a local directory substitute the following line in the instructions 
above, replacing the specified directory with your own.
```
  $ ./configure --prefix /home/genjix/usr/
```
If you set the prefix you must add these environment variables to the end 
of your `~/.bashrc` to point software to your local installation. Otherwise
software will not find the libbitcoin install.
```
  export LD_LIBRARY_PATH=~/usr/lib/
  export PKG_CONFIG_PATH=~/usr/lib/pkgconfig/
```
Configure also accepts the following flags:
```
  --enable-testnet     (deprecated, make all reference to testnet)
  CPPFLAGS=-DDEBUG     (verbose output)
```
### Compile/Build Flags

The `pkg-config` tool can be invoked to get the neccessary compile and build 
flags for creating software using libbitcoin.
```
  $ pkg-config --cflags --libs libbitcoin
```
### Examples

Start with the examples/ to see a few sample programs:
```
  $ cd examples/
  $ make
```
Now you can play around:
```
  $ ./satoshiwords
```
### Building the Debian package
```
  $ sudo apt-get install libboost-all-dev fakeroot
  $ dpkg-buildpackage -rfakeroot
```
## Arch Linux Instructions

Libbitcoin is available from the 
[AUR](https://aur.archlinux.org/packages/libbitcoin-leveldb-git/).

## Gentoo Linux Instructions

Libbitcoin is available in the 'bitcoin' overlay. Use layman to fetch the 
overlay then emerge as usual.

If you want to use live build, unlock the `'**'` keyword for 
`net-p2p/libbitcoin` in `/etc/portage/package.accept_keywords`

## Mac OSX Instructions

To build on **Mac OSX**, you will need to download and compile your own boost
using **gcc** (4.7.0 or later) before building libbitcoin. Then set the following
environment variables (matching your gcc version) before running the build 
process.
```
  CC=/usr/local/bin/gcc-4.7
  CXX=/usr/local/bin/g++-4.7
```
You will need **secp256k1** as a library dependency, if you don't have it already 
in your system this is how you get, build and install it from source code.
```
  $ git clone https://github.com/bitcoin/secp256k1
  $ cd secp256k1

  $ ./autogen.sh
  $ ./configure
  $ make
  $ sudo make install
```
For **Mountain Lion**, you need to follow these brew commands:
```
  $ brew install boost openssl
  $ brew tap homebrew/versions
  $ brew install gcc48
```
This can be reverted later if you prefer, but keeping the new g++ is a good idea.
```  
  $ sudo ln -sf /usr/local/bin/g++-4.8 /usr/bin/g++

  $ ./autogen.sh
  $ ./configure
  $ make
  $ sudo make install
```
## Windows Instructions
Visual Studio solutions are provided for all libbitcoin libraries and dependencies.
Because libbitcoin requires a `C++11` compiler, **Visual Studio 2013** is the most 
recent supported version and requires a modified pre-release compiler.

* [Visual Studio 2013 Express](http://www.visualstudio.com/en-us/products/visual-studio-express-vs.aspx)
is available for [download](http://www.microsoft.com/en-us/download/details.aspx?id=40787)
and is free of charge.
* The [November 2013 CTP Compiler](http://blogs.msdn.com/b/vcblog/archive/2013/11/18/announcing-the-visual-c-compiler-november-2013-ctp.aspx) must be [downloaded](http://www.microsoft.com/en-us/download/details.aspx?id=41151) 
and installed as well.

Dependencies apart from the libbitcoin repo are [available on NuGet](https://www.nuget.org/packages?q=evoskuil).
Eventually the libbitcoin libraries and tools will be published as well. The 
solution files are already configured with references to the NuGet packages
and expect a [central NuGet repo](http://docs.nuget.org/docs/release-notes/nuget-2.1).
You should be able to restore the packages to the repo once it is properly configured.

Once the dependencies have been downloaded select a **statically-linked build** and
build the solution. 
**The DLL build configurations do not compile, as the exports have not yet been completed.**
