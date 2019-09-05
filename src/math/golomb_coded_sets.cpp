/**
 * Copyright (c) 2011-2019 libbitcoin developers (see AUTHORS)
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

#include <algorithm>
#include <cstdint>
#include <functional>
#include <iostream>
#include <iterator>
#include <vector>
#include <bitcoin/system/constants.hpp>
#include <bitcoin/system/math/golomb_coded_sets.hpp>
#include <bitcoin/system/utility/container_source.hpp>
#include <bitcoin/system/utility/container_sink.hpp>
#include <bitcoin/system/utility/istream_bit_reader.hpp>
#include <bitcoin/system/utility/ostream_bit_writer.hpp>
#include <bitcoin/system/utility/ostream_writer.hpp>

namespace libbitcoin {
namespace system {
namespace golomb {

static void golomb_encode(bit_writer& sink, uint64_t value,
    uint8_t modulo_exponent)
{
    const uint64_t quotient = value >> modulo_exponent;
    for (uint64_t index = 0; index < quotient; index++)
        sink.write_bit(true);

    sink.write_bit(false);
    sink.write_variable_bits_big_endian(value, modulo_exponent);
}

static uint64_t golomb_decode(bit_reader& source, uint8_t modulo_exponent)
{
    uint64_t quotient = 0;
    while(source.read_bit())
        quotient++;

    uint64_t remainder = source.read_variable_bits_big_endian(modulo_exponent);
    return ((quotient << modulo_exponent) + remainder);
}

static uint64_t hash_to_range(const data_slice& item, uint64_t bound,
    const siphash_key& key)
{
    static const auto range_shift = sizeof(uint64_t) * bc::byte_bits;

    return static_cast<uint64_t>((
        static_cast<uint128_t>(siphash(key, item)) *
        static_cast<uint128_t>(bound)) >> range_shift);
}

static std::vector<uint64_t> hashed_set_construct(const data_stack& items,
    size_t set_size, uint64_t target_false_positive_rate, const siphash_key& key)
{
    auto bound = static_cast<uint64_t>(target_false_positive_rate * set_size);

    std::vector<uint64_t> hashes;
    hashes.reserve(items.size());

    std::transform(items.begin(), items.end(), std::back_inserter(hashes),
        [&](const data_chunk& item){ return hash_to_range(item, bound, key); });

    std::sort(hashes.begin(), hashes.end(), std::less<uint64_t>());
    return hashes;
}

// Golomb-coded set construction
// ----------------------------------------------------------------------------

data_chunk construct(const data_stack& items, uint8_t bits,
    const half_hash& entropy, uint64_t target_false_positive_rate)
{
    return construct(items, bits, to_siphash_key(entropy),
        target_false_positive_rate);
}

data_chunk construct(const data_stack& items, uint8_t bits,
    const siphash_key& entropy, uint64_t target_false_positive_rate)
{
    data_chunk result;
    data_sink stream(result);
    construct(stream, items, bits, entropy, target_false_positive_rate);
    return result;
}

void construct(std::ostream& stream, const data_stack& items, uint8_t bits,
    const half_hash& entropy, uint64_t target_false_positive_rate)
{
    construct(stream, items, bits, to_siphash_key(entropy),
        target_false_positive_rate);
}

void construct(std::ostream& stream, const data_stack& items, uint8_t bits,
    const siphash_key& entropy, uint64_t target_false_positive_rate)
{
    ostream_writer writer(stream);
    construct(writer, items, bits, entropy, target_false_positive_rate);
}

void construct(writer& stream, const data_stack& items, uint8_t bits,
    const half_hash& entropy, uint64_t target_false_positive_rate)
{
    construct(stream, items, bits, to_siphash_key(entropy),
        target_false_positive_rate);
}

void construct(writer& stream, const data_stack& items, uint8_t bits,
    const siphash_key& entropy, uint64_t target_false_positive_rate)
{
    const auto set = hashed_set_construct(items, items.size(),
        target_false_positive_rate, entropy);

    uint64_t previous = 0;
    ostream_bit_writer sink(stream);

    std::for_each(set.begin(), set.end(), [&](const uint64_t& value)
    {
        golomb_encode(sink, value - previous, bits);
        previous = value;
    });
}

// Single element match
// ----------------------------------------------------------------------------

bool match(const data_chunk& target, const data_chunk& compressed_set,
    uint64_t set_size, const half_hash& entropy, uint8_t bits,
    uint64_t target_false_positive_rate)
{
    return match(target, compressed_set, set_size, to_siphash_key(entropy),
        bits, target_false_positive_rate);
}

bool match(const data_chunk& target, const data_chunk& compressed_set,
    uint64_t set_size, const siphash_key& entropy, uint8_t bits,
    uint64_t target_false_positive_rate)
{
    data_source source(compressed_set);
    return match(target, source, set_size, entropy, bits,
        target_false_positive_rate);
}

bool match(const data_chunk& target, std::istream& compressed_set,
    uint64_t set_size, const half_hash& entropy, uint8_t bits,
    uint64_t target_false_positive_rate)
{
    return match(target, compressed_set, set_size, to_siphash_key(entropy),
        bits, target_false_positive_rate);
}

bool match(const data_chunk& target, std::istream& compressed_set,
    uint64_t set_size, const siphash_key& entropy, uint8_t bits,
    uint64_t target_false_positive_rate)
{
    istream_reader reader(compressed_set);
    return match(target, reader, set_size, entropy, bits,
        target_false_positive_rate);
}

bool match(const data_chunk& target, reader& compressed_set,
    uint64_t set_size, const half_hash& entropy, uint8_t bits,
    uint64_t target_false_positive_rate)
{
    return match(target, compressed_set, set_size, to_siphash_key(entropy),
        bits, target_false_positive_rate);
}

bool match(const data_chunk& target, reader& compressed_set,
    uint64_t set_size, const siphash_key& entropy, uint8_t bits,
    uint64_t target_false_positive_rate)
{
    const auto bound = target_false_positive_rate * set_size;
    const auto range = hash_to_range(target, bound, entropy);
    istream_bit_reader source(compressed_set);
    uint64_t previous = 0;

    for (uint64_t index = 0; index < set_size; index++)
    {
        const auto value = previous + golomb_decode(source, bits);

        if (value == range)
            return true;

        if (value > range)
            break;

        previous = value;
    }

    return false;
}

// Intersection match
// ----------------------------------------------------------------------------

bool match(const data_stack& targets, const data_chunk& compressed_set,
    uint64_t set_size, const half_hash& entropy, uint8_t bits,
    uint64_t target_false_positive_rate)
{
    return match(targets, compressed_set, set_size, to_siphash_key(entropy),
        bits, target_false_positive_rate);
}

bool match(const data_stack& targets, const data_chunk& compressed_set,
    uint64_t set_size, const siphash_key& entropy, uint8_t bits,
    uint64_t target_false_positive_rate)
{
    data_source source(compressed_set);
    return match(targets, source, set_size, entropy, bits,
        target_false_positive_rate);
}

bool match(const data_stack& targets, std::istream& compressed_set,
    uint64_t set_size, const half_hash& entropy, uint8_t bits,
    uint64_t target_false_positive_rate)
{
    return match(targets, compressed_set, set_size, to_siphash_key(entropy),
        bits, target_false_positive_rate);
}

bool match(const data_stack& targets, std::istream& compressed_set,
    uint64_t set_size, const siphash_key& entropy, uint8_t bits,
    uint64_t target_false_positive_rate)
{
    istream_reader reader(compressed_set);
    return match(targets, reader, set_size, entropy, bits,
        target_false_positive_rate);
}

bool match(const data_stack& targets, reader& compressed_set,
    uint64_t set_size, const half_hash& entropy, uint8_t bits,
    uint64_t target_false_positive_rate)
{
    return match(targets, compressed_set, set_size, to_siphash_key(entropy),
        bits, target_false_positive_rate);
}

bool match(const data_stack& targets, reader& compressed_set,
    uint64_t set_size, const siphash_key& entropy, uint8_t bits,
    uint64_t target_false_positive_rate)
{
    if (targets.empty())
        return false;

    const auto set = hashed_set_construct(targets, set_size,
        target_false_positive_rate, entropy);

    uint64_t range = 0;
    auto it = set.begin();
    istream_bit_reader source(compressed_set);

    for (uint64_t index = 0; index < set_size && it != set.end(); index++)
    {
        range += golomb_decode(source, bits);

        for (auto value = *it; it != set.end(); value = *(++it))
        {
            if (value == range)
                return true;

            if (value > range)
                break;
        }
    }

    return false;
}

} // namespace golomb
} // namespace system
} // namespace libbitcoin
