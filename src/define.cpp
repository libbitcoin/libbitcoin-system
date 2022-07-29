/**
 * Copyright (c) 2011-2022 libbitcoin developers (see AUTHORS)
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

// This is a maintainer file to force rebuild of /system only.

// define.hpp is the common include for /system.
// All non-system headers include define.hpp.
// System inclusions are chained as follows.

// version        : <generated>
// have           : version
// warnings       : have
// boost          : warnings
// exceptions     : boost
// types          : exceptions
// constants      : types
// literals       : constants
// assembly       : literals
// intrinsics     : assembly
// funclets       : intrinsics
// typelets       : funclets
// constraints    : typelets
// define         : constraints

// Other directory common includes are not internally chained.
// Each headers include only its required common headers.

// /error         : define
// /unicode       : define
// /math          : define
// /data          : /math /unicode
// /endian        : /data
// /words         : /data
// /radix         : /words
// /serial        : /radix
// /crypto        : /radix {TODO: /endian -> /math}
// /stream        : /crypto /endian /error
// /chain         : /stream [/settings]
// settings       : /chain
// /machine       : /chain
// /config        : /chain
// /wallet        : /chain

// All /crypto/external can be made cpp/constexpr except the lax
// parser, which wraps libsecp256k1 (as with elliptic_curve).
// TODO: convert and move to /system/hash/ directory along with hash.hpp.

DEFINED("Version " LIBBITCOIN_SYSTEM_VERSION)

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
#ifdef HAVE_ITANIUM
DEFINED("HAVE_ITANIUM")
#endif
#ifdef HAVE_ARM32
DEFINED("HAVE_ARM32")
#endif
#ifdef HAVE_ARM64
DEFINED("HAVE_ARM64")
#endif

#ifdef HAVE_XCPU
DEFINED("HAVE_XCPU")
#endif
#ifdef HAVE_XGETBV
DEFINED("HAVE_XGETBV")
#endif
#ifdef HAVE_XCPUID
DEFINED("HAVE_XCPUID")
#endif
#ifdef HAVE_XCPUIDEX
DEFINED("HAVE_XCPUIDEX")
#endif
#ifdef HAVE_XCPUID_COUNT
DEFINED("HAVE_XCPUID_COUNT")
#endif
#ifdef HAVE_XASSEMBLY
DEFINED("HAVE_XASSEMBLY")
#endif
#ifdef HAVE_NEON
DEFINED("HAVE_NEON")
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

#ifdef HAVE_RANGES
DEFINED("HAVE_RANGES")
#endif
#ifdef HAVE_CONSTEVAL
DEFINED("HAVE_CONSTEVAL")
#endif
#ifdef HAVE_STRING_CONSTEXPR
DEFINED("HAVE_STRING_CONSTEXPR")
#endif
#ifdef HAVE_VECTOR_CONSTEXPR
DEFINED("HAVE_VECTOR_CONSTEXPR")
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
#ifdef HAVE_SLOW_TESTS
DEFINED("HAVE_SLOW_TESTS")
#endif

// Force compilation of this unit.
namespace libbitcoin {}