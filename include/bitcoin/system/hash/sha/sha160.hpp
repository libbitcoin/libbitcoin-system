/**
 * Copyright (c) 2011-2023 libbitcoin developers (see AUTHORS)
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
#ifndef LIBBITCOIN_SYSTEM_HASH_SHA_SHA160_HPP
#define LIBBITCOIN_SYSTEM_HASH_SHA_SHA160_HPP

#include <bitcoin/system/define.hpp>
#include <bitcoin/system/hash/sha/sha.hpp>

namespace libbitcoin {
namespace system {
namespace sha {

struct k160
  : public k<160, 80>
{
    // added constants (K)
    static constexpr constants_t get
    {
        // rounds 0..19
        0x5a827999, 0x5a827999, 0x5a827999, 0x5a827999,
        0x5a827999, 0x5a827999, 0x5a827999, 0x5a827999,
        0x5a827999, 0x5a827999, 0x5a827999, 0x5a827999,
        0x5a827999, 0x5a827999, 0x5a827999, 0x5a827999,
        0x5a827999, 0x5a827999, 0x5a827999, 0x5a827999,

        // rounds 20..39
        0x6ed9eba1, 0x6ed9eba1, 0x6ed9eba1, 0x6ed9eba1,
        0x6ed9eba1, 0x6ed9eba1, 0x6ed9eba1, 0x6ed9eba1,
        0x6ed9eba1, 0x6ed9eba1, 0x6ed9eba1, 0x6ed9eba1,
        0x6ed9eba1, 0x6ed9eba1, 0x6ed9eba1, 0x6ed9eba1,
        0x6ed9eba1, 0x6ed9eba1, 0x6ed9eba1, 0x6ed9eba1,

        // rounds 40..59
        0x8f1bbcdc, 0x8f1bbcdc, 0x8f1bbcdc, 0x8f1bbcdc,
        0x8f1bbcdc, 0x8f1bbcdc, 0x8f1bbcdc, 0x8f1bbcdc,
        0x8f1bbcdc, 0x8f1bbcdc, 0x8f1bbcdc, 0x8f1bbcdc,
        0x8f1bbcdc, 0x8f1bbcdc, 0x8f1bbcdc, 0x8f1bbcdc,
        0x8f1bbcdc, 0x8f1bbcdc, 0x8f1bbcdc, 0x8f1bbcdc,

        // rounds 60..79
        0xca62c1d6, 0xca62c1d6, 0xca62c1d6, 0xca62c1d6,
        0xca62c1d6, 0xca62c1d6, 0xca62c1d6, 0xca62c1d6,
        0xca62c1d6, 0xca62c1d6, 0xca62c1d6, 0xca62c1d6,
        0xca62c1d6, 0xca62c1d6, 0xca62c1d6, 0xca62c1d6,
        0xca62c1d6, 0xca62c1d6, 0xca62c1d6, 0xca62c1d6
    };
};

struct h160
  : public h<k160>
{
    using state_t = typename h<k160>::state_t;

    // initial value (H)
    static constexpr state_t get
    {
        0x67452301,
        0xefcdab89,
        0x98badcfe,
        0x10325476,
        0xc3d2e1f0
    };
};

} // namespace sha
} // namespace system
} // namespace libbitcoin

#endif
