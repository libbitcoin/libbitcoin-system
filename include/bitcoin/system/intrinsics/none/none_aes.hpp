/**
 * Copyright (c) 2011-2026 libbitcoin developers
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
#ifndef LIBBITCOIN_SYSTEM_INTRINSICS_NONE_AES_HPP
#define LIBBITCOIN_SYSTEM_INTRINSICS_NONE_AES_HPP

#include <bitcoin/system/define.hpp>
#include <bitcoin/system/intrinsics/types.hpp>

#if !defined(HAVE_AES)

namespace libbitcoin {
namespace system {
namespace aes {

INLINE xint128_t inverse(xint128_t) NOEXCEPT
{
    return {};
}

template <typename xWord>
INLINE xWord replicate(xint128_t) NOEXCEPT
{
    return {};
}

template <size_t Rounds, typename xWord, size_t Count>
INLINE void encrypt(std_array<xWord, Count>&,
    const std_array<xWord, add1(Rounds)>&) NOEXCEPT
{
}

template <size_t Rounds, size_t Count>
INLINE void decrypt(std_array<xint128_t, Count>&,
    const std_array<xint128_t, add1(Rounds)>&) NOEXCEPT
{
}

INLINE xint128_t sum(xint128_t, xint128_t) NOEXCEPT
{
    return {};
}

INLINE xint128_t reflect(xint128_t) NOEXCEPT
{
    return {};
}

INLINE void multiply(xint128_t&, xint128_t&, xint128_t, xint128_t) NOEXCEPT
{
}

INLINE xint128_t reduce(xint128_t, xint128_t) NOEXCEPT
{
    return {};
}

} // namespace aes
} // namespace system
} // namespace libbitcoin

#endif // HAVE_AES

#endif
