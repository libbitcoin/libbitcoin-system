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
#ifndef LIBBITCOIN_SYSTEM_HASH_SHA_SHA_HPP
#define LIBBITCOIN_SYSTEM_HASH_SHA_SHA_HPP

#include <bitcoin/system/define.hpp>

// TODO: implement 5.3.6 SHA-512/t initial vector derivation.
// TODO: add SHA-256/224, 512/384, 512/224, 512/256 constants/types.

namespace libbitcoin {
namespace system {
namespace sha {

struct shak_t {};
struct shah_t {};

template <size_t Strength, size_t Rounds,
    bool_if<Strength == 160 || Strength == 256 || Strength == 512> = true,
    bool_if<Rounds == 64 || Rounds == 80> = true>
struct k
{
    using T = shak_t;
    static constexpr auto strength = Strength;
    static constexpr auto rounds = Rounds;
    static constexpr auto columns = 20_size;
    static constexpr auto rows = strength / columns;
    static constexpr auto size = (strength == 160 ? 256_size : strength);
    using constants_t = std_array<
        iif<strength == 512, uint64_t, uint32_t>, rounds>;
};

template <typename Constants, size_t Digest = Constants::strength,
    if_not_greater<Digest, Constants::size> = true,
    if_same<typename Constants::T, shak_t> = true>
struct h
{
    using T = shah_t;
    using K = Constants;
    static constexpr auto digest       = Digest;
    static constexpr auto size         = K::size;
    static constexpr auto rounds       = K::rounds;
    static constexpr auto strength     = K::strength;
    static constexpr auto word_bits    = bytes<size>;
    static constexpr auto word_bytes   = bytes<word_bits>;
    static constexpr auto block_words  = bytes<size> / to_half(word_bytes);
    static constexpr auto chunk_words  = to_half(block_words);
    static constexpr auto state_words  = Digest == 160 ? 5 : 8;
    using word_t = unsigned_type<word_bytes>;
    using state_t = std_array<word_t, state_words>;
};

} // namespace sha
} // namespace system
} // namespace libbitcoin

#endif
