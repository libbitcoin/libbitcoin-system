/**
 * Copyright (c) 2011-2025 libbitcoin developers (see AUTHORS)
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

#ifndef LIBBITCOIN_SYSTEM_FILTER_GOLOMB_HPP
#define LIBBITCOIN_SYSTEM_FILTER_GOLOMB_HPP

#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/hash/hash.hpp>
#include <bitcoin/system/stream/stream.hpp>

namespace libbitcoin {
namespace system {

class BC_API golomb
{
public:

    /// Golomb-coded set construction
    /// -----------------------------------------------------------------------

    static void construct(bitwriter& writer, const data_stack& items,
        uint8_t bits, const siphash_key& entropy,
        uint64_t target_false_positive_rate) NOEXCEPT;

    static data_chunk construct(const data_stack& items,
        uint8_t bits, const siphash_key& entropy,
        uint64_t target_false_positive_rate) NOEXCEPT;

    static data_chunk construct(const data_stack& items,
        uint8_t bits, const half_hash& entropy,
        uint64_t target_false_positive_rate) NOEXCEPT;

    /// Single element match
    /// -----------------------------------------------------------------------

    static bool match_single(bitreader& reader,
        const data_chunk& target, uint64_t set_size,
        const siphash_key& entropy, uint8_t bits,
        uint64_t target_false_positive_rate) NOEXCEPT;

    static bool match_single(const data_chunk& compressed_set,
        const data_chunk& target, uint64_t set_size,
        const half_hash& entropy, uint8_t bits,
        uint64_t target_false_positive_rate) NOEXCEPT;

    static bool match_single(const data_chunk& compressed_set,
        const data_chunk& target, uint64_t set_size,
        const siphash_key& entropy, uint8_t bits,
        uint64_t target_false_positive_rate) NOEXCEPT;

    /// Intersection match
    /// -----------------------------------------------------------------------

    static bool match_stack(bitreader& reader,
        const data_stack& targets, uint64_t set_size,
        const siphash_key& entropy, uint8_t bits,
        uint64_t target_false_positive_rate) NOEXCEPT;

    static bool match_stack(const data_chunk& compressed_set,
        const data_stack& targets, uint64_t set_size,
        const half_hash& entropy, uint8_t bits,
        uint64_t target_false_positive_rate) NOEXCEPT;

    static bool match_stack(const data_chunk& compressed_set,
        const data_stack& targets, uint64_t set_size,
        const siphash_key& entropy, uint8_t bits,
        uint64_t target_false_positive_rate) NOEXCEPT;

private:
    static void encode(bitwriter& writer, uint64_t value,
        uint8_t modulo_exponent) NOEXCEPT;
    static uint64_t decode(bitreader& reader,
        uint8_t modulo_exponent) NOEXCEPT;
    static uint64_t hash_to_range(const data_slice& item,
        uint64_t bound, const siphash_key& key) NOEXCEPT;
    static std::vector<uint64_t> hashed_set_construct(const data_stack& items,
        uint64_t set_size, uint64_t target_false_positive_rate,
        const siphash_key& key) NOEXCEPT;
};

} // namespace system
} // namespace libbitcoin

#endif
