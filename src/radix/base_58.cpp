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
#include <bitcoin/system/radix/base_58.hpp>

#include <algorithm>
#include <bitcoin/system/define.hpp>

// base58
// Base 58 is an ascii data encoding with a domain of 58 symbols (characters).
// 58 is not a power of 2 so base58 is not a bit mapping.

namespace libbitcoin {
namespace system {

const std::string base58_chars =
    "123456789ABCDEFGHJKLMNPQRSTUVWXYZabcdefghijkmnopqrstuvwxyz";

bool is_base58(char character) NOEXCEPT
{
    // The base58 character list is lexically sorted.
    return std::binary_search(base58_chars.begin(), base58_chars.end(),
        character);
}

bool is_base58(const std::string& text) NOEXCEPT
{
    const auto test = [](const char character) NOEXCEPT
    {
        return is_base58(character);
    };

    return std::all_of(text.begin(), text.end(), test);
}

template <typename Data>
static auto find_first_nonzero(const Data& data) NOEXCEPT ->
    decltype(data.cbegin())
{
    auto first_nonzero = data.cbegin();
    while (first_nonzero != data.end() && is_zero(*first_nonzero))
        ++first_nonzero;

    return first_nonzero;
}

size_t count_leading_zeros(const data_slice& unencoded) NOEXCEPT
{
    // Skip and count leading '1's.
    size_t leading_zeros = 0;
    for (const auto byte: unencoded)
    {
        if (!is_zero(byte))
            break;

        ++leading_zeros;
    }

    return leading_zeros;
}

void pack_value(data_chunk& indexes, size_t carry) NOEXCEPT
{
    // Apply "b58 = b58 * 256 + ch".
    for (auto it = indexes.rbegin(); it != indexes.rend(); ++it)
    {
        carry += 256_size * (*it);
        *it = carry % 58_size;
        carry /= 58_size;
    }

    BC_ASSERT(is_zero(carry));
}

std::string encode_base58(const data_slice& unencoded) NOEXCEPT
{
    const auto leading_zeros = count_leading_zeros(unencoded);

    // size = log(256) / log(58), rounded up.
    const auto number_nonzero = unencoded.size() - leading_zeros;
    const auto indexes_size = add1(number_nonzero * 138_size / 100_size);

    // Allocate enough space in big-endian base58 representation.
    data_chunk indexes(indexes_size);

    // Process the bytes.
    for (size_t index = leading_zeros; index < unencoded.size(); ++index)
        pack_value(indexes, unencoded[index]);

    // Skip leading zeros in base58 result.
    auto it = find_first_nonzero(indexes);

    // Translate the result into a string.
    std::string encoded;
    const size_t estimated_size = leading_zeros + (indexes.end() - it);
    encoded.reserve(estimated_size);
    encoded.assign(leading_zeros, '1');

    // Set actual main bytes.
    for (; it != indexes.end(); ++it)
    {
        BC_PUSH_WARNING(NO_ARRAY_INDEXING)
        BC_PUSH_WARNING(NO_DYNAMIC_ARRAY_INDEXING)
        encoded += base58_chars[*it];
        BC_POP_WARNING()
        BC_POP_WARNING()
    }

    return encoded;
}

size_t count_leading_zeros(const std::string& encoded) NOEXCEPT
{
    // Skip and count leading '1's.
    auto leading_zeros = zero;
    for (const auto digit: encoded)
    {
        BC_PUSH_WARNING(NO_ARRAY_INDEXING)
        if (digit != base58_chars[0])
        BC_POP_WARNING()
        {
            break;
        }

        ++leading_zeros;
    }

    return leading_zeros;
}

void unpack_char(data_chunk& data, size_t carry) NOEXCEPT
{
    for (auto it = data.rbegin(); it != data.rend(); it++)
    {
        carry += 58_size * (*it);
        *it = carry % 256_size;
        carry /= 256_size;
    }

    BC_ASSERT(is_zero(carry));
}

bool decode_base58(data_chunk& out, const std::string& in) NOEXCEPT
{
    out.clear();

    // Trim spaces and newlines around the string.
    const auto leading_zeros = count_leading_zeros(in);

    // log(58) / log(256), rounded up.
    const size_t data_size = add1(in.size() * 733_size / 1000_size);

    // Allocate enough space in big-endian base256 representation.
    data_chunk data(data_size);

    // Process the characters.
    for (auto it = in.begin() + leading_zeros; it != in.end(); ++it)
    {
        const auto carry = base58_chars.find(*it);
        if (carry == std::string::npos)
            return false;

        unpack_char(data, carry);
    }

    // Skip leading zeros in data.
    const auto first_nonzero = find_first_nonzero(data);

    // Copy result into output vector.
    const size_t estimated_size = leading_zeros + (data.end() - first_nonzero);
    out.reserve(estimated_size);
    out.assign(leading_zeros, 0x00_u8);
    out.insert(out.end(), first_nonzero, data.cend());
    return true;
}

} // namespace system
} // namespace libbitcoin
