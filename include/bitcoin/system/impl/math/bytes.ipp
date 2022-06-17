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
#ifndef LIBBITCOIN_SYSTEM_MATH_BYTES_IPP
#define LIBBITCOIN_SYSTEM_MATH_BYTES_IPP

#include <algorithm>
#include <array>
#include <array>
#include <cstdint>
#include <ranges>
#include <bitcoin/system/constraints.hpp>
#include <bitcoin/system/math/safe.hpp>

////#ifdef _MSC_VER
////    #include <stdlib.h>
////    #define bswap_08(x) (x)
////    #define bswap_16(x) _byteswap_ushort(x)
////    #define bswap_32(x) _byteswap_ulong(x)
////    #define bswap_64(x) _byteswap_uint64(x)
////#endif

namespace libbitcoin {
namespace system {

// C++23: std::byteswap.
template <typename Integer,
    if_integral_integer<Integer>,
    if_unique_object_representations<Integer>>
constexpr Integer byteswap(Integer value) noexcept
{
    auto out = std::bit_cast<std::array<std::byte, sizeof(Integer)>>(value);
    std::ranges::reverse(out);
    return std::bit_cast<Integer>(out);
}

////// Intrinsics (non-constexpr).
////template <typename Integer,
////    if_integral_integer<Integer>,
////    if_unique_object_representations<Integer>,
////    if_not_greater<sizeof(Integer), sizeof(uint64_t)>>
////inline Integer byte_swap_intrinsic(Integer value) noexcept
////{
////#ifdef _MSC_VER
////    constexpr auto uvalue = to_unsigned(value);
////    if constexpr (sizeof<Integer> == sizeof(uint8_t))
////        return possible_sign_cast<Integer>(bswap_08(uvalue));
////    if constexpr (sizeof<Integer> == sizeof(uint16_t)
////        return possible_sign_cast<Integer>(bswap_16(uvalue));
////    if constexpr (sizeof<Integer> == sizeof(uint32_t)
////        return possible_sign_cast<Integer>(bswap_32(uvalue));
////    if constexpr (sizeof<Integer> == sizeof(uint64_t)
////        return possible_sign_cast<Integer>(bswap_64(uvalue));
////#else
////    static_assert(false), "not implemented");
////#endif // _MSC_VER
////}

} // namespace system
} // namespace libbitcoin

#endif
