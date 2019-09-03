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

//#include <algorithm>
//#include <utility>
#include <bitcoin/system/math/golomb_coded_sets.hpp>
#include <bitcoin/system/utility/container_source.hpp>
#include <bitcoin/system/utility/container_sink.hpp>
#include <bitcoin/system/utility/istream_bit_reader.hpp>
#include <bitcoin/system/utility/ostream_bit_writer.hpp>
#include <bitcoin/system/utility/ostream_writer.hpp>

namespace libbitcoin {
namespace system {
namespace golomb {

void golomb_encode(bit_writer& sink, uint64_t value, uint8_t modulo_exponent)
{
    uint64_t quotient = value >> modulo_exponent;
    for (uint64_t index = 0; index < quotient; index++)
        sink.write_bit(true);

    sink.write_bit(false);
    sink.write_variable_bits_big_endian(value, modulo_exponent);
}

uint64_t golomb_decode(bit_reader& source, uint8_t modulo_exponent)
{
    uint64_t quotient = 0;
    while(source.read_bit())
        quotient++;

    uint64_t remainder = source.read_variable_bits_big_endian(modulo_exponent);
    return ((quotient << modulo_exponent) + remainder);
}

uint64_t hash_to_range(const data_slice& item, uint64_t bound,
    const numeric_key& key)
{
    uint64_t hash = siphash(key, item);
    return static_cast<uint64_t>(
        (static_cast<uint128_t>(hash) * static_cast<uint128_t>(bound)) >> 64);
}

std::vector<uint64_t> hashed_set_construct(const data_stack& items,
    uint64_t target_false_positive_rate, const numeric_key& key)
{
    std::vector<uint64_t> hashed_items;
    uint64_t bound = static_cast<uint64_t>(
        target_false_positive_rate * items.size());

    for (auto element : items)
        hashed_items.push_back(hash_to_range(element, bound, key));

    return hashed_items;
}

// Golomb-coded set construction
// ----------------------------------------------------------------------------
data_chunk construct(const data_stack& items, uint64_t bit_param,
    const half_hash& entropy, uint64_t target_false_positive_rate)
{
    auto key = to_numeric_key(entropy);
    return construct(items, bit_param, key, target_false_positive_rate);
}

data_chunk construct(const data_stack& items, uint64_t bit_param,
    const numeric_key& entropy, uint64_t target_false_positive_rate)
{
    data_chunk result;
    {
        data_sink stream(result);
        construct(stream, items, bit_param, entropy, target_false_positive_rate);
    }

    return result;
}

void construct(std::ostream& stream, const data_stack& items,
    uint64_t bit_param, const half_hash& entropy,
    uint64_t target_false_positive_rate)
{
    auto key = to_numeric_key(entropy);
    construct(stream, items, bit_param, key, target_false_positive_rate);
}

void construct(std::ostream& stream, const data_stack& items,
    uint64_t bit_param, const numeric_key& entropy,
    uint64_t target_false_positive_rate)
{
    ostream_writer writer(stream);
    construct(writer, items, bit_param, entropy, target_false_positive_rate);
}

void construct(writer& stream, const data_stack& items,
    uint64_t bit_param, const half_hash& entropy,
    uint64_t target_false_positive_rate)
{
    auto key = to_numeric_key(entropy);
    construct(stream, items, bit_param, key, target_false_positive_rate);
}

void construct(writer& stream, const data_stack& items,
    uint64_t bit_param, const numeric_key& entropy,
    uint64_t target_false_positive_rate)
{
    auto hashed_items = hashed_set_construct(items, target_false_positive_rate,
        entropy);

    std::sort(hashed_items.begin(), hashed_items.end(), std::less<uint64_t>());

    {
        // Declared within scope to guarantee flush of buffered writer
        ostream_bit_writer sink(stream);

        uint64_t prev = 0;
        for (auto item : hashed_items)
        {
            uint64_t delta = item - prev;
            golomb_encode(sink, delta, bit_param);
            prev = item;
        }
    }
}

// Single element match
// ----------------------------------------------------------------------------
bool match(const data_chunk& target, const data_chunk& compressed_set,
    uint64_t set_size, const half_hash& entropy, uint64_t bit_param,
    uint64_t target_false_positive_rate)
{
    auto key = to_numeric_key(entropy);
    return match(target, compressed_set, set_size, key, bit_param,
        target_false_positive_rate);
}

bool match(const data_chunk& target, const data_chunk& compressed_set,
    uint64_t set_size, const numeric_key& entropy, uint64_t bit_param,
    uint64_t target_false_positive_rate)
{
    data_source source(compressed_set);
    return match(target, source, set_size, entropy, bit_param,
        target_false_positive_rate);
}

bool match(const data_chunk& target, std::istream& compressed_set,
    uint64_t set_size, const half_hash& entropy, uint64_t bit_param,
    uint64_t target_false_positive_rate)
{
    auto key = to_numeric_key(entropy);
    return match(target, compressed_set, set_size, key, bit_param,
        target_false_positive_rate);
}

bool match(const data_chunk& target, std::istream& compressed_set,
    uint64_t set_size, const numeric_key& entropy, uint64_t bit_param,
    uint64_t target_false_positive_rate)
{
    istream_reader reader(compressed_set);
    return match(target, reader, set_size, entropy, bit_param,
        target_false_positive_rate);
}

bool match(const data_chunk& target, reader& compressed_set,
    uint64_t set_size, const half_hash& entropy, uint64_t bit_param,
    uint64_t target_false_positive_rate)
{
    auto key = to_numeric_key(entropy);
    return match(target, compressed_set, set_size, key, bit_param,
        target_false_positive_rate);
}

bool match(const data_chunk& target, reader& compressed_set,
    uint64_t set_size, const numeric_key& entropy, uint64_t bit_param,
    uint64_t target_false_positive_rate)
{
    uint64_t bound = target_false_positive_rate * set_size;
    uint64_t target_hash = hash_to_range(target, bound, entropy);

    istream_bit_reader source(compressed_set);

    uint64_t prev = 0;

    for (uint64_t index = 0; index < set_size; index++)
    {
        uint64_t delta = golomb_decode(source, bit_param);
        uint64_t item_hash = prev + delta;

        if (item_hash == target_hash)
            return true;

        if (item_hash > target_hash)
            break;

        prev = item_hash;
    }

    return false;
}

// Intersection match
// ----------------------------------------------------------------------------
bool match(const data_stack& targets, const data_chunk& compressed_set,
    uint64_t set_size, const half_hash& entropy, uint64_t bit_param,
    uint64_t target_false_positive_rate)
{
    auto key = to_numeric_key(entropy);
    return match(targets, compressed_set, set_size, key, bit_param,
        target_false_positive_rate);
}

bool match(const data_stack& targets, const data_chunk& compressed_set,
    uint64_t set_size, const numeric_key& entropy, uint64_t bit_param,
    uint64_t target_false_positive_rate)
{
    data_source source(compressed_set);
    return match(targets, source, set_size, entropy, bit_param,
        target_false_positive_rate);
}

bool match(const data_stack& targets, std::istream& compressed_set,
    uint64_t set_size, const half_hash& entropy, uint64_t bit_param,
    uint64_t target_false_positive_rate)
{
    auto key = to_numeric_key(entropy);
    return match(targets, compressed_set, set_size, key, bit_param,
        target_false_positive_rate);
}

bool match(const data_stack& targets, std::istream& compressed_set,
    uint64_t set_size, const numeric_key& entropy, uint64_t bit_param,
    uint64_t target_false_positive_rate)
{
    istream_reader reader(compressed_set);
    return match(targets, reader, set_size, entropy, bit_param,
        target_false_positive_rate);
}

bool match(const data_stack& targets, reader& compressed_set,
    uint64_t set_size, const half_hash& entropy, uint64_t bit_param,
    uint64_t target_false_positive_rate)
{
    auto key = to_numeric_key(entropy);
    return match(targets, compressed_set, set_size, key, bit_param,
        target_false_positive_rate);
}

bool match(const data_stack& targets, reader& compressed_set,
    uint64_t set_size, const numeric_key& entropy, uint64_t bit_param,
    uint64_t target_false_positive_rate)
{
    if (targets.size() == 0)
        return false;

    uint64_t bound = target_false_positive_rate * set_size;

    std::vector<uint64_t> target_hashes;
    for (data_stack::size_type index = 0; index < targets.size(); index++)
        target_hashes.push_back(hash_to_range(targets[index], bound, entropy));

    std::sort(target_hashes.begin(), target_hashes.end(), std::less<uint64_t>());

    istream_bit_reader source(compressed_set);

    uint64_t value = 0;
    uint64_t target_index = 0;

    for (uint64_t index = 0; index < set_size; index++)
    {
        uint64_t delta = golomb_decode(source, bit_param);
        value += delta;

        while (target_index < target_hashes.size())
        {
            if (target_hashes[target_index] == value)
                return true;

            if (target_hashes[target_index] > value)
                break;

            target_index++;
        }

        if (target_index >= target_hashes.size())
            break;
    }

    return false;
}

} // namespace golomb
} // namespace system
} // namespace libbitcoin
