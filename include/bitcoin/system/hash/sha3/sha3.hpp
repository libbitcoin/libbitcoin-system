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
#ifndef LIBBITCOIN_SYSTEM_HASH_SHA3_SHA3_HPP
#define LIBBITCOIN_SYSTEM_HASH_SHA3_SHA3_HPP

#include <bitcoin/system/define.hpp>
#include <bitcoin/system/math/math.hpp>

namespace libbitcoin {
namespace system {
namespace sha3 {

struct sha3k_t{};
struct sha3h_t{};

/// Padding domain separation byte (FIPS 202 SHA3 vs. original Keccak).
constexpr uint8_t sha3_domain = 0x06;
constexpr uint8_t keccak_domain = 0x01;

/// Keccak-f[1600] parameters, capacity is twice the strength.
template <size_t Strength,
    bool_if<Strength == 224 || Strength == 256 ||
        Strength == 384 || Strength == 512> = true>
struct k
{
    using T = sha3k_t;
    static constexpr auto strength = Strength;
    static constexpr auto rounds = 24_size;
    static constexpr auto width = 1600_size;
    static constexpr auto capacity = 2 * strength;
    static constexpr auto rate = width - capacity;
    using constants_t = std_array<uint64_t, rounds>;

    // round constants (RC)
    static constexpr constants_t get
    {
        0x0000000000000001, 0x0000000000008082,
        0x800000000000808a, 0x8000000080008000,
        0x000000000000808b, 0x0000000080000001,
        0x8000000080008081, 0x8000000000008009,
        0x000000000000008a, 0x0000000000000088,
        0x0000000080008009, 0x000000008000000a,
        0x000000008000808b, 0x800000000000008b,
        0x8000000000008089, 0x8000000000008003,
        0x8000000000008002, 0x8000000000000080,
        0x000000000000800a, 0x800000008000000a,
        0x8000000080008081, 0x8000000000008080,
        0x0000000080000001, 0x8000000080008008
    };
};

template <typename Constants, size_t Digest = Constants::strength,
    uint8_t Domain = sha3_domain,
    if_not_greater<Digest, Constants::strength> = true,
    if_same<typename Constants::T, sha3k_t> = true>
struct h
{
    using T = sha3h_t;
    using K = Constants;
    static constexpr auto digest       = Digest;
    static constexpr auto domain       = Domain;
    static constexpr auto rounds       = K::rounds;
    static constexpr auto strength     = K::strength;
    static constexpr auto word_bits    = 64_size;
    static constexpr auto word_bytes   = bytes<word_bits>;
    static constexpr auto block_words  = K::rate / word_bits;
    static constexpr auto state_words  = K::width / word_bits;
    using word_t = uint64_t;
    using state_t = std_array<word_t, state_words>;

    // initial value (H), the sponge state is initially zero.
    static constexpr state_t get{};
};

using k224 = k<224>;
using k256 = k<256>;
using k384 = k<384>;
using k512 = k<512>;

template <size_t Digest = 224, uint8_t Domain = sha3_domain>
struct h224
  : public h<k224, Digest, Domain>
{
};

template <size_t Digest = 256, uint8_t Domain = sha3_domain>
struct h256
  : public h<k256, Digest, Domain>
{
};

template <size_t Digest = 384, uint8_t Domain = sha3_domain>
struct h384
  : public h<k384, Digest, Domain>
{
};

template <size_t Digest = 512, uint8_t Domain = sha3_domain>
struct h512
  : public h<k512, Digest, Domain>
{
};

} // namespace sha3
} // namespace system
} // namespace libbitcoin

#endif
