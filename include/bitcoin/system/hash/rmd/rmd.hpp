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
#ifndef LIBBITCOIN_SYSTEM_HASH_RMD_RMD_HPP
#define LIBBITCOIN_SYSTEM_HASH_RMD_RMD_HPP

#include <bitcoin/system/define.hpp>

namespace libbitcoin {
namespace system {
namespace rmd {

struct rmdk_t {};
struct rmdh_t {};

template <size_t Strength,
    bool_if<Strength == 128 || Strength == 160> = true>
struct k
{
    using T = rmdk_t;
    static constexpr auto strength = Strength;
    static constexpr auto rounds = strength;
    static constexpr auto columns = 16_size;
    static constexpr auto rows = strength / columns;
    static constexpr auto size = 256_size;
    using constants_t = std_array<uint32_t, rows>;
    using schedule_t = std_array<uint32_t, rounds>;
};

template <typename Constants, size_t Digest = Constants::strength,
    if_same<typename Constants::T, rmdk_t> = true>
struct h
{
    using T = rmdh_t;
    using K = Constants;
    static constexpr auto digest       = Digest;
    static constexpr auto size         = K::size;
    static constexpr auto rounds       = K::rounds;
    static constexpr auto word_bits    = bytes<size>;
    static constexpr auto word_bytes   = bytes<word_bits>;
    static constexpr auto block_words  = bytes<size> / to_half(word_bytes);
    static constexpr auto chunk_words  = to_half(block_words);
    static constexpr auto state_words  = Digest == 128 ? 4 : 5;
    using word_t = unsigned_type<word_bytes>;
    using state_t = std_array<word_t, state_words>;
};

} // namespace rmd
} // namespace system
} // namespace libbitcoin

#endif
