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

// Sponsored in part by Digital Contract Design, LLC

#include <bitcoin/system/hash/siphash.hpp>

#include <span>
#include <tuple>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/endian/endian.hpp>
#include <bitcoin/system/intrinsics/intrinsics.hpp>
#include <bitcoin/system/math/math.hpp>

// This would be circular a /hash include (must stay in cpp).
#include <bitcoin/system/stream/stream.hpp>

namespace libbitcoin {
namespace system {

BC_PUSH_WARNING(NO_USE_OF_SPAN)
BC_PUSH_WARNING(NO_ARRAY_INDEXING)
BC_PUSH_WARNING(NO_DYNAMIC_ARRAY_INDEXING)

constexpr auto word_bits = bits<uint64_t>;
constexpr uint64_t siphash_magic_0 = 0x736f6d6570736575;
constexpr uint64_t siphash_magic_1 = 0x646f72616e646f6d;
constexpr uint64_t siphash_magic_2 = 0x6c7967656e657261;
constexpr uint64_t siphash_magic_3 = 0x7465646279746573;
constexpr uint64_t finalization = 0x00000000000000ff;
constexpr uint64_t max_encoded_byte_count = (1 << byte_bits);

// local
constexpr uint64_t to_length(size_t bytes) NOEXCEPT
{
    constexpr auto eight = sizeof(uint64_t);
    return (bytes % max_encoded_byte_count) << to_bits(sub1(eight));
}

// local
constexpr siphash_words initialize(const siphash_key& key) NOEXCEPT
{
    return
    {
        siphash_magic_0 ^ std::get<0>(key),
        siphash_magic_1 ^ std::get<1>(key),
        siphash_magic_2 ^ std::get<0>(key),
        siphash_magic_3 ^ std::get<1>(key)
    };
}

// Rounds (integral or extended words).
// ----------------------------------------------------------------------------

// local
template <typename Word>
INLINE constexpr void sip_round(Word& v0, Word& v1, Word& v2,
    Word& v3) NOEXCEPT
{
    v0 = f::add<word_bits>(v0, v1);
    v2 = f::add<word_bits>(v2, v3);
    v1 = f::rol<13, word_bits>(v1);
    v3 = f::rol<16, word_bits>(v3);
    v1 = f::xor_(v1, v0);
    v3 = f::xor_(v3, v2);

    v0 = f::rol<32, word_bits>(v0);

    v2 = f::add<word_bits>(v2, v1);
    v0 = f::add<word_bits>(v0, v3);
    v1 = f::rol<17, word_bits>(v1);
    v3 = f::rol<21, word_bits>(v3);
    v1 = f::xor_(v1, v2);
    v3 = f::xor_(v3, v0);

    v2 = f::rol<32, word_bits>(v2);
}

// local
template <typename Word>
INLINE constexpr void compression_round(Word& v0, Word& v1, Word& v2,
    Word& v3, Word word) NOEXCEPT
{
    v3 = f::xor_(v3, word);
    sip_round(v0, v1, v2, v3);
    sip_round(v0, v1, v2, v3);
    v0 = f::xor_(v0, word);
}

// local
template <typename Word>
INLINE constexpr Word finalize(Word& v0, Word& v1, Word& v2, Word& v3,
    Word last, Word final) NOEXCEPT
{
    compression_round(v0, v1, v2, v3, last);

    v2 = f::xor_(v2, final);
    sip_round(v0, v1, v2, v3);
    sip_round(v0, v1, v2, v3);
    sip_round(v0, v1, v2, v3);
    sip_round(v0, v1, v2, v3);

    return f::xor_(f::xor_(v0, v1), f::xor_(v2, v3));
}

// Lanes (one row per 64 bit lane).
// ----------------------------------------------------------------------------

// local
template <typename xWord>
INLINE xWord load(const std::span<const uint64_t>& column,
    size_t row) NOEXCEPT
{
    constexpr auto lanes = capacity<xWord, uint64_t>;
    const auto group = column.subspan(row, lanes);
    const auto& words = unsafe_array_cast<uint64_t, lanes>(group.data());
    const auto little = f::load<uint64_t>(array_cast<xWord>(words).front());
    return native_from_little_end<uint64_t>(little);
}

// local
template <typename xWord>
INLINE void store(const std::span<uint64_t>& out, size_t row,
    xWord value) NOEXCEPT
{
    constexpr auto lanes = capacity<xWord, uint64_t>;
    const auto group = out.subspan(row, lanes);
    auto& words = unsafe_array_cast<uint64_t, lanes>(group.data());
    f::store<uint64_t>(array_cast<xWord>(words).front(), value);
}

// Hashes whole groups of rows, advancing row past them.
// local
template <typename xWord>
INLINE void hash_lanes(size_t& row, const std::span<uint64_t>& out,
    const siphash_words& state, const siphash_columns& columns) NOEXCEPT
{
    if constexpr (have<xWord>)
    {
        constexpr auto lanes = capacity<xWord, uint64_t>;
        if ((out.size() - row) < lanes)
            return;

        const auto s0 = f::broadcast<xWord>(state[0]);
        const auto s1 = f::broadcast<xWord>(state[1]);
        const auto s2 = f::broadcast<xWord>(state[2]);
        const auto s3 = f::broadcast<xWord>(state[3]);
        const auto final = f::broadcast<xWord>(finalization);
        const auto last = f::broadcast<xWord>(
            to_length(sizeof(siphash_words)));

        do
        {
            auto v0 = s0, v1 = s1, v2 = s2, v3 = s3;
            compression_round(v0, v1, v2, v3, load<xWord>(columns[0], row));
            compression_round(v0, v1, v2, v3, load<xWord>(columns[1], row));
            compression_round(v0, v1, v2, v3, load<xWord>(columns[2], row));
            compression_round(v0, v1, v2, v3, load<xWord>(columns[3], row));
            store<xWord>(out, row, finalize(v0, v1, v2, v3, last, final));
            row += lanes;
        }
        while ((out.size() - row) >= lanes);
    }
}

// Public.
// ----------------------------------------------------------------------------

uint64_t siphash(const siphash_key& key, const data_slice& message) NOEXCEPT
{
    auto [v0, v1, v2, v3] = initialize(key);

    constexpr auto eight = sizeof(uint64_t);
    const auto bytes = message.size();
    stream::in::fast stream(message);
    read::bytes::fast source(stream);

    for (size_t index = eight; index <= bytes; index += eight)
        compression_round(v0, v1, v2, v3, source.read_8_bytes_little_endian());

    // Read zero to seven remainder bytes (zero padded, fails stream).
    BC_ASSERT(source);
    const auto last = source.read_8_bytes_little_endian();
    BC_ASSERT(!source);

    return finalize(v0, v1, v2, v3, last ^ to_length(bytes), finalization);
}

uint64_t siphash(const siphash_key& key, const siphash_words& message) NOEXCEPT
{
    auto [v0, v1, v2, v3] = initialize(key);

    for (const auto word: message)
        compression_round(v0, v1, v2, v3, word);

    return finalize(v0, v1, v2, v3, to_length(sizeof(siphash_words)),
        finalization);
}

void siphash(const std::span<uint64_t>& out, const siphash_key& key,
    const siphash_columns& columns) NOEXCEPT
{
    size_t row{};
    const auto state = initialize(key);
    hash_lanes<xint512_t>(row, out, state, columns);
    hash_lanes<xint256_t>(row, out, state, columns);
    hash_lanes<xint128_t>(row, out, state, columns);

    for (; row < out.size(); ++row)
        out[row] = siphash(key, siphash_words
        {
            native_from_little_end(columns[0][row]),
            native_from_little_end(columns[1][row]),
            native_from_little_end(columns[2][row]),
            native_from_little_end(columns[3][row])
        });
}

uint64_t siphash(const half_hash& hash, const data_slice& message) NOEXCEPT
{
    return siphash(to_siphash_key(hash), message);
}

BC_POP_WARNING()
BC_POP_WARNING()
BC_POP_WARNING()

} // namespace system
} // namespace libbitcoin
