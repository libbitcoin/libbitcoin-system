[![Continuous Integration Build](https://github.com/libbitcoin/libbitcoin-system/actions/workflows/ci.yml/badge.svg?branch=master&event=push)](https://github.com/libbitcoin/libbitcoin-system/actions/workflows/ci.yml)

[![Coverage Status](https://img.shields.io/coveralls/github/libbitcoin/libbitcoin-system/master)](https://coveralls.io/github/libbitcoin/libbitcoin-system?branch=master)

# libbitcoin-system

*The Bitcoin Development Library*

[Documentation](https://github.com/libbitcoin/libbitcoin/wiki) is available on the wiki.

**License Overview**

All files in this repository fall under the license specified in [COPYING](COPYING). The project is licensed as [AGPL with a lesser clause](https://www.gnu.org/licenses/agpl-3.0.en.html). It may be used within a proprietary project, but the core library and any changes to it must be published online. Source code for this library must always remain free for everybody to access.

**About libbitcoin**

The libbitcoin toolkit is a set of cross platform C++ libraries for building Bitcoin applications. The toolkit consists of several libraries, most of which depend on this foundational library. Each library's repository can be cloned and built separately.

The master branch is current, pending the v4 release.

## Contents
- [Requirements](#requirements)
- [Build from Source](#build-from-source)
  - [GNU Autotools Build](#gnu-autotools-build)
  - [CMake Build](#cmake-build)
  - [CMake Presets Build](#cmake-presets-build)
  - [macOS](#macos)
  - [Windows](#windows)
- [secp256k1](#secp256k1)
- [Hardware](#hardware)

---

## Requirements

| Requirement | Notes |
|-------------|-------|
| C++20 compiler | Verified in CI with GCC 13–16, Clang 18–19, Apple Clang and MSVC (Visual Studio 2026) |
| Boost ≥ 1.86 | container, json, program_options, url, and unit_test_framework for tests |
| Autoconf, Automake, Libtool, pkg-config | GNU Autotools builds |
| CMake ≥ 3.30 | CMake builds |
| git | Used by the installation scripts to clone dependencies |

On Ubuntu 24.04 LTS:

```bash
# For GNU Autotools builds:
sudo apt install build-essential git autoconf automake libtool pkg-config

# Additionally for CMake builds:
sudo apt install cmake
```

---

## Build from Source

libbitcoin-system ships with **three installation scripts**, each targeting a different toolchain:

| Script | Toolchain | Best for |
|--------|-----------|----------|
| `builds/gnu/install-gnu.sh` | GNU Autotools (make) | Linux, macOS — traditional |
| `builds/cmake/install-cmake.sh` | CMake | Linux, macOS — flexible |
| `builds/cmake/install-presets.sh` | CMake with Presets | Linux — simplified, named configurations |

Each script can download and build its dependencies (Boost and, optionally, a secp256k1 library) before building and installing libbitcoin-system. Unrecognized options are passed through as configuration options to all builds.

### GNU Autotools Build

Uses `make` and Autotools (`autoconf`, `automake`, `libtool`). Well-tested and the reference build for CI.

```bash
git clone https://github.com/libbitcoin/libbitcoin-system
cd libbitcoin-system

./builds/gnu/install-gnu.sh \
  --prefix=$HOME/libbitcoin \
  --build-boost \
  --build-config=release \
  --build-link=static \
  --build-post-install-clean
```

On success the headers and library are installed under `$HOME/libbitcoin`, along with `lib/pkgconfig/libbitcoin-system.pc`.

**Key options:**

| Option | Values | Description |
|--------|--------|-------------|
| `--prefix=<path>` | absolute path | Installation destination (default `/usr/local`) |
| `--build-config=<mode>` | `release`, `debug` | Build configuration |
| `--build-link=<mode>` | `static`, `dynamic` | Link mode |
| `--build-boost` | — | Build Boost from source |
| `--build-secp256k1` | — | Build bitcoin-core/secp256k1 from source (with `--with-secp256k1`) |
| `--build-use-local-src` | — | Reuse already-present source directories instead of cloning from GitHub |
| `--build-skip-tests` | — | Skip test compilation and execution |
| `--build-parallel=<n>` | integer | Number of parallel compile jobs |
| `--build-post-install-clean` | — | Remove dependency build artifacts after installation |

See all options:

```bash
./builds/gnu/install-gnu.sh --help
```

### CMake Build

Uses CMake instead of Autotools. Configuration options use CMake's `-D` syntax.

```bash
git clone https://github.com/libbitcoin/libbitcoin-system
cd libbitcoin-system

./builds/cmake/install-cmake.sh \
  --prefix=$HOME/libbitcoin \
  --build-boost \
  --build-config=release \
  --build-link=static \
  --build-post-install-clean
```

The script options are those of the Autotools script. The configuration options are:

| Option | Default | Description |
|--------|---------|-------------|
| `-Dwith-tests=<ON/OFF>` | `ON` | Compile with unit tests |
| `-Dwith-examples=<ON/OFF>` | `ON` | Compile with examples |
| `-Dwith-secp256k1=<ON/OFF>` | `OFF` | Use bitcoin-core/secp256k1 |
| `-Denable-<extension>=<ON/OFF>` | `OFF` | See [CPU Extensions](#cpu-extensions) |

See all options:

```bash
./builds/cmake/install-cmake.sh --help
```

### CMake Presets Build

Named presets combine the toolchain, build type and link mode into a single `--build-preset` parameter. The install prefix and build directory are set relative to the source tree.

| Preset | Config | Link |
|--------|--------|------|
| `nix-gnu-release-static` | release | static |
| `nix-gnu-release-shared` | release | dynamic |
| `nix-gnu-debug-static` | debug | static |
| `nix-gnu-debug-shared` | debug | dynamic |

```bash
./builds/cmake/install-presets.sh \
  --build-preset=nix-gnu-release-static \
  --build-boost
```

See all options:

```bash
./builds/cmake/install-presets.sh --help
```

### macOS

The Autotools and CMake scripts work on macOS with Apple Clang. Install the prerequisites with [Homebrew](https://brew.sh):

```bash
brew install autoconf automake libtool pkg-config cmake
```

Then follow the [GNU Autotools Build](#gnu-autotools-build) or [CMake Build](#cmake-build) instructions above.

### Windows

Visual Studio 2026 (v145 toolset) solution and project files are in `builds/msvc/vs2026/`. Dependencies are NuGet packages, restored automatically when the solution is built. Only Boost is required; the secp256k1 library is an optional replacement for the internal implementation (see [secp256k1](#secp256k1)):

| Package | Version | Required |
|---------|---------|----------|
| `libbitcoin-boost` (with container, json, program_options, url and unit_test_framework) | 1.91.0 | yes |
| `secp256k1_vc145` | 0.8.0 | no |

Build from within Visual Studio, or from a command prompt with `builds\msvc\build-msvc.cmd`, which restores the NuGet packages and builds with MSBuild. The script runs `msbuild` and `nuget` from `PATH` (a Developer Command Prompt provides `msbuild`; `nuget.exe` must be downloaded). To use other executables, set the `MSBUILD_EXE` and `NUGET_EXE` environment variables before running it. Packages are restored to `.nuget\packages` under the source directory unless `NUGET_PKG_PATH` is set.

The script requires `--build-config`, set to a solution configuration (`StaticRelease` or `StaticDebug`). The platform defaults to `x64` unless set with `--build-platform` (`x64`, `Win32` or `ARM64`). It also accepts the same `--enable-<extension>` options as the other scripts.

```
builds\msvc\build-msvc.cmd --build-config StaticRelease
```

See `builds\msvc\build-msvc.cmd --help` for all options.

---

## secp256k1

The secp256k1 elliptic curve implementation is internal, and can be replaced by any library that provides the libsecp256k1 interface. The build supports bitcoin-core/secp256k1:

| Library | GNU | CMake | Notes |
|---------|-----|-------|-------|
| [bitcoin-core/secp256k1](https://github.com/bitcoin-core/secp256k1) | `--with-secp256k1` | `-Dwith-secp256k1=ON` | Add `--build-secp256k1` to build it from source |

`WITH_SECP256K1` selects the libsecp256k1 interface, so any other implementation of it can be linked in place of bitcoin-core/secp256k1.

---

## Hardware

libbitcoin can be compiled with optional CPU acceleration for hashing and cryptography. All are disabled by default.

| Extension | Flag (GNU) | Flag (CMake) | Description |
|-----------|-----------|--------------|-------------|
| SHA-NI | `--enable-shani` | `-Denable-shani=ON` | SHA hardware instructions (Intel/ARM) |
| SSE4.1 | `--enable-sse41` | `-Denable-sse41=ON` | SIMD integer ops |
| AVX2 | `--enable-avx2` | `-Denable-avx2=ON` | 256-bit SIMD |
| AVX-512 | `--enable-avx512` | `-Denable-avx512=ON` | 512-bit SIMD |
| AVX-512 IFMA | `--enable-avx512ifma` | `-Denable-avx512ifma=ON` | 52-bit multiply-add (implies AVX-512 VL) |
| AVX IFMA | `--enable-avxifma` | `-Denable-avxifma=ON` | 52-bit multiply-add (implies AVX2) |
| SHA512 | `--enable-sha512` | `-Denable-sha512=ON` | Intel SHA512 instructions (implies AVX2) |
| AES-NI | `--enable-aesni` | `-Denable-aesni=ON` | AES and carry-less multiply |
| VAES | `--enable-vaes` | `-Denable-vaes=ON` | Vector AES and carry-less multiply (implies AES-NI and AVX2) |
| ARM Crypto | `--enable-crypto` | `-Denable-crypto=ON` | SHA and AES instructions (ARM) |
| ARM SHA3 | `--enable-sha3` | `-Denable-sha3=ON` | ARM SHA3 instructions for SHA512 (implies Crypto) |

> **Important:** These hardware options are not portable. The platform must provide the hardware or the process will terminate.

Example:

```bash
./builds/gnu/install-gnu.sh \
  --prefix=$HOME/libbitcoin \
  --build-boost \
  --build-config=release \
  --build-link=static \
  --enable-shani \
  --enable-sse41 \
  --enable-avx2
```
