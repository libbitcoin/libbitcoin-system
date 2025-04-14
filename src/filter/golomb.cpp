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

#include <bitcoin/system/filter/golomb.hpp>

#include <algorithm>
#include <utility>
#include <vector>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/hash/hash.hpp>
#include <bitcoin/system/math/math.hpp>
#include <bitcoin/system/stream/stream.hpp>

namespace libbitcoin {
namespace system {

// Golomb-coded set construction
// ----------------------------------------------------------------------------

// protected
void golomb::construct(bitwriter& writer, const data_stack& items, uint8_t bits,
    const siphash_key& entropy, uint64_t target_false_positive_rate) NOEXCEPT
{
    const auto set = hashed_set_construct(items, items.size(),
        target_false_positive_rate, entropy);

    uint64_t previous = 0;
    for (const auto value: set)
    {
        encode(writer, value - previous, bits);
        previous = value;
    };
}

data_chunk golomb::construct(const data_stack& items, uint8_t bits,
    const siphash_key& entropy, uint64_t target_false_positive_rate) NOEXCEPT
{
    data_chunk out{};

    // A vector (push) stream is used because the size is not known a-priori.
    stream::out::data stream(out);
    write::bits::ostream writer(stream);
    construct(writer, items, bits, entropy, target_false_positive_rate);
    writer.flush();
    return out;
}

data_chunk golomb::construct(const data_stack& items, uint8_t bits,
    const half_hash& entropy, uint64_t target_false_positive_rate) NOEXCEPT
{
    return construct(items, bits, to_siphash_key(entropy),
        target_false_positive_rate);
}

// Single element match
// ----------------------------------------------------------------------------

// protected
bool golomb::match_single(bitreader& reader, const data_chunk& target,
    uint64_t set_size, const siphash_key& entropy, uint8_t bits,
    uint64_t target_false_positive_rate) NOEXCEPT
{
    const auto bound = target_false_positive_rate * set_size;
    const auto range = hash_to_range(target, bound, entropy);

    uint64_t previous = 0;
    for (uint64_t index = 0; index < set_size; index++)
    {
        const auto value = previous + decode(reader, bits);

        if (value == range)
            return true;

        if (value > range)
            break;

        previous = value;
    }

    return false;
}

bool golomb::match_single(const data_chunk& compressed_set,
    const data_chunk& target,  uint64_t set_size, const siphash_key& entropy,
    uint8_t bits, uint64_t target_false_positive_rate) NOEXCEPT
{
    stream::in::fast source(compressed_set);
    read::bits::fast reader(source);
    return match_single(reader, target, set_size, entropy, bits,
        target_false_positive_rate);
}

bool golomb::match_single(const data_chunk& compressed_set,
    const data_chunk& target, uint64_t set_size, const half_hash& entropy,
    uint8_t bits, uint64_t target_false_positive_rate) NOEXCEPT
{
    return match_single(compressed_set, target, set_size,
        to_siphash_key(entropy), bits, target_false_positive_rate);
}

// Intersection match
// ----------------------------------------------------------------------------

// protected
bool golomb::match_stack(bitreader& source, const data_stack& targets,
    uint64_t set_size, const siphash_key& entropy, uint8_t bits,
    uint64_t target_false_positive_rate) NOEXCEPT
{
    if (targets.empty())
        return false;

    const auto set = hashed_set_construct(targets, set_size,
        target_false_positive_rate, entropy);

    uint64_t range = 0;
    auto it = set.begin();

    for (uint64_t index = 0; index < set_size && it != set.end(); ++index)
    {
        range += decode(source, bits);

        for (auto value = *it; it != set.end(); value = *(it++))
        {
            if (value == range)
                return true;

            if (value > range)
                break;
        }
    }

    return false;
}

bool golomb::match_stack(const data_chunk& compressed_set,
    const data_stack& targets, uint64_t set_size, const siphash_key& entropy,
    uint8_t bits, uint64_t target_false_positive_rate) NOEXCEPT
{
    stream::in::fast source(compressed_set);
    read::bits::fast reader(source);
    return match_stack(reader, targets, set_size, entropy, bits,
        target_false_positive_rate);
}

bool golomb::match_stack(const data_chunk& compressed_set,
    const data_stack& targets, uint64_t set_size, const half_hash& entropy,
    uint8_t bits, uint64_t target_false_positive_rate) NOEXCEPT
{
    return match_stack(compressed_set, targets, set_size, to_siphash_key(entropy),
        bits, target_false_positive_rate);
}

// private
// ----------------------------------------------------------------------------

void golomb::encode(bitwriter& writer, uint64_t value,
    uint8_t modulo_exponent) NOEXCEPT
{
    const auto quotient = shift_right(value, modulo_exponent);
    for (uint64_t index = 0; index < quotient; ++index)
        writer.write_bit(true);

    writer.write_bit(false);
    writer.write_bits(value, modulo_exponent);
}

uint64_t golomb::decode(bitreader& reader, uint8_t modulo_exponent) NOEXCEPT
{
    uint64_t quotient = 0;
    while (reader.read_bit())
        ++quotient;

    const auto remainder = reader.read_bits(modulo_exponent);
    return shift_left(quotient, modulo_exponent) + remainder;
}

uint64_t golomb::hash_to_range(const data_slice& item, uint64_t bound,
    const siphash_key& key) NOEXCEPT
{
    constexpr auto shift = bits<uint64_t>;
    const auto product = uint128_t(siphash(key, item)) * uint128_t(bound);
    return (product >> shift).convert_to<uint64_t>();
}

std::vector<uint64_t> golomb::hashed_set_construct(const data_stack& items,
    uint64_t set_size, uint64_t target_false_positive_rate,
    const siphash_key& key) NOEXCEPT
{
    if (is_multiply_overflow(target_false_positive_rate, set_size))
        return {};

    std::vector<uint64_t> hashes(items.size());
    const auto bound = target_false_positive_rate * set_size;
    std::transform(items.begin(), items.end(), hashes.begin(),
        [&](const data_chunk& item) NOEXCEPT
        {
            return hash_to_range(item, bound, key);
        });

    return sort(std::move(hashes));
}


} // namespace system
} // namespace libbitcoin
