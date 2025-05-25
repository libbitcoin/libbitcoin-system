/**
 * Copyright (c) 2011-2025 libbitcoin developers (see AUTHORS)
 *
 * This file is part of libbitcoin.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include <bitcoin/system/define.hpp>

// Required in source (cpp) I replaced for boost header only.
#include <boost/json/src.hpp>

// This is a maintainer file to force rebuild of /system only.

// define.hpp is the common include for /system.
// All non-system headers include define.hpp.
// System inclusions are chained as follows.

// version        : <generated>
// execution      : 
// have           : version
// preprocessor   : have, execution
// warnings       : preprocessor
// boost          : warnings
// exceptions     : boost
// arena          : exceptions
// allocator      : arena
// types          : allocator
// constants      : types
// literals       : constants
// funclets       : literals
// typelets       : funclets
// constraints    : typelets
// define         : constraints
// forks          :

// Other directory common includes are not internally chained.
// Each header includes only its required common headers.

// /error         : define
// /unicode       : define
// /intrinsics    : define
// /math          : /intrinsics
// /data          : /math /unicode
// /endian        : /data
// /words         : /data
// /radix         : /words
// /serial        : /radix
// /hash          : /radix /endian
// /crypto        : /hash
// /stream        : /crypto /endian /error
// /filter        : /stream
// /chain         : /stream forks [forward: settings]
// /machine       : /chain
// /config        : /chain
// /wallet        : /chain filter
// settings       : /chain forks

// When a symbol is unexplainably undefined, its defining include is probably
// creating the circularity.

DEFINED(LIBBITCOIN_SYSTEM_VERSION)

#ifdef HAVE_LINUX
DEFINED("HAVE_LINUX")
#endif
#ifdef HAVE_APPLE
DEFINED("HAVE_APPLE")
#endif
#ifdef HAVE_FREEBSD
DEFINED("HAVE_FREEBSD")
#endif
#ifdef HAVE_OPENBSD
DEFINED("HAVE_OPENBSD")
#endif
#ifdef HAVE_NETBSD
DEFINED("HAVE_NETBSD")
#endif
#ifdef HAVE_CYGWIN
DEFINED("HAVE_CYGWIN")
#endif

#ifdef HAVE_CLANG
DEFINED("HAVE_CLANG")
#endif
#ifdef HAVE_XCODE
DEFINED("HAVE_XCODE")
#endif
#ifdef HAVE_GNUC
DEFINED("HAVE_GNUC")
#endif
#ifdef HAVE_MSC
DEFINED("HAVE_MSC")
#endif

#ifdef HAVE_NX_LIBS
DEFINED("HAVE_NX_LIBS")
#endif
#ifdef HAVE_WINDOWS_LIBS
DEFINED("HAVE_WINDOWS_LIBS")
#endif

#ifdef HAVE_X32
DEFINED("HAVE_X32")
#endif
#ifdef HAVE_X64
DEFINED("HAVE_X64")
#endif
#ifdef HAVE_XCPU
DEFINED("HAVE_XCPU")
#endif

#ifdef HAVE_ARM32
DEFINED("HAVE_ARM32")
#endif
#ifdef HAVE_ARM64
DEFINED("HAVE_ARM64")
#endif
#ifdef HAVE_ARM
DEFINED("HAVE_ARM")
#endif

#ifdef HAVE_XGETBV
DEFINED("HAVE_XGETBV")
#endif
#ifdef HAVE_XCPUID_COUNT
DEFINED("HAVE_XCPUID_COUNT")
#endif
#ifdef HAVE_XCPUIDEX
DEFINED("HAVE_XCPUIDEX")
#endif

#ifdef HAVE_XCPU_ASSEMBLY
DEFINED("HAVE_XCPU_ASSEMBLY")
#endif
#ifdef HAVE_ARM_ASSEMBLY
DEFINED("HAVE_ARM_ASSEMBLY")
#endif

#ifdef HAVE_SSE41
DEFINED("HAVE_SSE41")
#endif
#ifdef HAVE_AVX2
DEFINED("HAVE_AVX2")
#endif
#ifdef HAVE_AVX512
DEFINED("HAVE_AVX512")
#endif
#ifdef HAVE_SHANI
DEFINED("HAVE_SHANI")
#endif

#ifdef HAVE_NEON
DEFINED("HAVE_NEON")
#endif
#ifdef HAVE_SVE
DEFINED("HAVE_SVE")
#endif
#ifdef HAVE_CRC32
DEFINED("HAVE_CRC32")
#endif
#ifdef HAVE_CRYPTO
DEFINED("HAVE_CRYPTO")
#endif

#ifdef HAVE_VS2013
DEFINED("HAVE_VS2013")
#endif
#ifdef HAVE_VS2015
DEFINED("HAVE_VS2015")
#endif
#ifdef HAVE_VS2017
DEFINED("HAVE_VS2017")
#endif
#ifdef HAVE_VS2019
DEFINED("HAVE_VS2019")
#endif
#ifdef HAVE_VS2022
DEFINED("HAVE_VS2022")
#endif

#ifdef HAVE_CPP03
DEFINED("HAVE_CPP03")
#endif
#ifdef HAVE_CPP11
DEFINED("HAVE_CPP11")
#endif
#ifdef HAVE_CPP14
DEFINED("HAVE_CPP14")
#endif
#ifdef HAVE_CPP17
DEFINED("HAVE_CPP17")
#endif
#ifdef HAVE_CPP20
DEFINED("HAVE_CPP20")
#endif

#ifdef HAVE_CONSTEVAL
DEFINED("HAVE_CONSTEVAL")
#endif

#ifdef HAVE_EXECUTION
DEFINED("HAVE_EXECUTION")
#endif

#ifdef HAVE_ICU
DEFINED("HAVE_ICU")
#endif

// These messages are suppressed without this.
#ifdef HAVE_MESSAGES
DEFINED("HAVE_MESSAGES")
#endif
#ifdef HAVE_SUPPRESSION
DEFINED("HAVE_SUPPRESSION")
#endif
#ifdef HAVE_NOEXCEPT
DEFINED("HAVE_NOEXCEPT")
#endif
#ifdef HAVE_DEPRECATED
DEFINED("HAVE_DEPRECATED")
#endif

// Force compilation of this unit.
namespace libbitcoin {}