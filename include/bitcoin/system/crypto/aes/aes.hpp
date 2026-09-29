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
#ifndef LIBBITCOIN_SYSTEM_CRYPTO_AES_AES_HPP
#define LIBBITCOIN_SYSTEM_CRYPTO_AES_AES_HPP

#include <bitcoin/system/define.hpp>

namespace libbitcoin {
namespace system {
namespace aes {

struct aesk_t{};

template <size_t Strength,
    bool_if<Strength == 128 || Strength == 256> = true>
struct k
{
    using T = aesk_t;
    static constexpr auto strength   = Strength;
    static constexpr auto key_words  = strength / bits<uint32_t>;
    static constexpr auto rounds     = key_words + 6_size;
    static constexpr auto round_keys = add1(rounds);
};

using k128 = k<128>;
using k256 = k<256>;

} // namespace aes
} // namespace system
} // namespace libbitcoin

#endif
