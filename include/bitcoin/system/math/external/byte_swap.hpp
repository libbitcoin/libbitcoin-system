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
#ifndef LIBBITCOIN_SYSTEM_MATH_EXTERNAL_BYTE_SWAP_HPP
#define LIBBITCOIN_SYSTEM_MATH_EXTERNAL_BYTE_SWAP_HPP

#include <bitcoin/system/define.hpp>

// Shift left is undefined for negative value.
// Shift right is implementation-defined for negative value.
// So these are unsigned though caller may cast from/to signed.

constexpr uint16_t byte_swap16_native(uint16_t value) NOEXCEPT
{
    BC_PUSH_WARNING(NO_CASTS_FOR_ARITHMETIC_CONVERSION)
    return static_cast<uint16_t>((value << 8u) | (value >> 8u));
    BC_POP_WARNING()
}

constexpr uint32_t byte_swap32_native(uint32_t value) NOEXCEPT
{
    return
        ((value << 24) |
        ((value <<  8) & 0x00ff0000ul) |
        ((value >>  8) & 0x0000ff00ul) |
         (value >> 24));
}

constexpr uint64_t byte_swap64_native(uint64_t value) NOEXCEPT
{
    return
        ((value << 56) |
        ((value << 40) & 0xff000000000000ull) |
        ((value << 24) & 0x00ff0000000000ull) |
        ((value <<  8) & 0x0000ff00000000ull) |
        ((value >>  8) & 0x000000ff000000ull) |
        ((value >> 24) & 0x00000000ff0000ull) |
        ((value >> 40) & 0x0000000000ff00ull) |
         (value >> 56));
}

#if defined(HAVE_MSC)
    // docs.microsoft.com/en-us/cpp/c-runtime-library/reference/
    // byteswap-uint64-byteswap-ulong-byteswap-ushort?view=msvc-170
    #include <stdlib.h>
    #define byte_swap16(value) _byteswap_ushort(value)
    #define byte_swap32(value) _byteswap_ulong(value)
    #define byte_swap64(value) _byteswap_uint64(value)
#elif defined(HAVE_LINUX)
    // man7.org/linux/man-pages/man3/bswap.3.html
    #include <byteswap.h>
    #define byte_swap16(value) bswap_16(value)
    #define byte_swap32(value) bswap_32(value)
    #define byte_swap64(value) bswap_64(value)
#elif defined(HAVE_APPLE)
    // opensource.apple.com/source/xnu/xnu-344.2/libkern/libkern/
    // OSByteOrder.h.auto.html
    #include <libkern/OSByteOrder.h>
    #define byte_swap16(value) OSSwapInt16(value)
    #define byte_swap32(value) OSSwapInt32(value)
    #define byte_swap64(value) OSSwapInt64(value)
#elif defined(HAVE_FREEBSD)
    #include <sys/endian.h>
    // www.freebsd.org/cgi/man.cgi?query=bswap64
    #define byte_swap16(value) bswap16(value)
    #define byte_swap32(value) bswap32(value)
    #define byte_swap64(value) bswap64(value)
#elif defined(HAVE_OPENBSD)
    // man.openbsd.org/swap64
    #include <sys/types.h>
    #define byte_swap16(value) swap16(value)
    #define byte_swap32(value) swap32(value)
    #define byte_swap64(value) swap64(value)
#elif defined(HAVE_NETBSD)
    // man.netbsd.org/bswap
    #include <sys/types.h>
    #include <machine/bswap.h>
    #define byte_swap16(value) bswap16(value)
    #define byte_swap32(value) bswap32(value)
    #define byte_swap64(value) bswap64(value)
#else
    // Native implementation.
    #define byte_swap16(value) byte_swap16_native(value)
    #define byte_swap32(value) byte_swap32_native(value)
    #define byte_swap64(value) byte_swap64_native(value)
#endif

#endif
