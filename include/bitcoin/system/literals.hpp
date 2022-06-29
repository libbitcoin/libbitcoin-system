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
#ifndef LIBBITCOIN_SYSTEM_LITERALS_HPP
#define LIBBITCOIN_SYSTEM_LITERALS_HPP

#include <cstddef>
#include <cstdint>
#include <exception>
#include <limits>
#include <bitcoin/system/define.hpp>

namespace libbitcoin {

// TODO: make these consteval and remove non-constexpr evaluation test cases.

/// User-defined literals.
/// en.cppreference.com/w/cpp/language/user_literal

/// The only integer type allowed for customization.
using unsigned64 = unsigned long long int;

BC_PUSH_WARNING(NO_CASTS_FOR_ARITHMETIC_CONVERSION)

CONSTEVAL int8_t operator "" _i8(unsigned64 value) noexcept
{
    if ((value & 0xffffffffffffff00) != 0) std::terminate();

    // Since the native "one byte" literal is signed, it does not require
    // domain coercion to properly overflow.
    return static_cast<int8_t>(value);
}

CONSTEVAL int16_t operator "" _i16(unsigned64 value) noexcept
{
    if ((value & 0xffffffffffff0000) != 0) std::terminate();

    // Since the native "two byte" literal is signed, it does not require 
    // domain coercion to properly overflow.
    return static_cast<int16_t>(value);
}

CONSTEVAL int32_t operator "" _i32(unsigned64 value) noexcept
{
    if ((value & 0xffffffff00000000) != 0) std::terminate();

    // Since the native "four byte" literal is either signed (high bit not
    // set), or unsigned (high bit set), it must be coerced back to unsigned
    // 32 bit from unsigned 64 bit before casting it to an unsigned 32 bit.
    // This ensures that the value will overflow as expected.
    const auto sized = static_cast<uint32_t>(value);
    return static_cast<int32_t>(sized);
}

CONSTEVAL int64_t operator "" _i64(unsigned64 value) noexcept
{
    // Domains aligned, will overflow as expected.
    return static_cast<int64_t>(value);
}

CONSTEVAL uint8_t operator "" _u8(unsigned64 value) noexcept
{
    if ((value & 0xffffffffffffff00) != 0) std::terminate();
    return static_cast<uint8_t>(value);
}

CONSTEVAL uint16_t operator "" _u16(unsigned64 value) noexcept
{
    if ((value & 0xffffffffffff0000) != 0) std::terminate();
    return static_cast<uint16_t>(value);
}

CONSTEVAL uint32_t operator "" _u32(unsigned64 value) noexcept
{
    if ((value & 0xffffffff00000000) != 0) std::terminate();
    return static_cast<uint32_t>(value);
}

CONSTEVAL uint64_t operator "" _u64(unsigned64 value) noexcept
{
    return static_cast<uint64_t>(value);
}

CONSTEVAL size_t operator "" _size(unsigned64 value) noexcept
{
    if (value > std::numeric_limits<size_t>::max()) std::terminate();
    return static_cast<size_t>(value);
}

/// Aliases (for vertial alignment).

CONSTEVAL int8_t operator "" _i08(unsigned64 value) noexcept
{
    return operator "" _i8(value);
}

CONSTEVAL uint8_t operator "" _u08(unsigned64 value) noexcept
{
    return operator "" _u8(value);
}

CONSTEVAL size_t operator "" _siz(unsigned64 value) noexcept
{
    return operator "" _size(value);
}

BC_POP_WARNING()

} // namespace libbitcoin

#endif
