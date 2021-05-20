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
#include <bitcoin/system/data/binary.hpp>

#include <iostream>
#include <sstream>
#include <string>
#include <bitcoin/system/assert.hpp>
#include <bitcoin/system/constants.hpp>
#include <bitcoin/system/math/limits.hpp>
#include <bitcoin/system/serialization/endian.hpp>

namespace libbitcoin {
namespace system {

binary::size_type binary::blocks_size(size_type bit_size)
{
    return bit_size == 0 ? 0 : (bit_size - 1) / bits_per_block + 1;
}

bool binary::is_base2(const std::string& text)
{
    for (const auto& character: text)
        if (character != '0' && character != '1')
            return false;

    return true;
}

binary::binary()
  : final_block_excess_(0)
{
}

binary::binary(const binary& other)
  : blocks_(other.blocks_), final_block_excess_(other.final_block_excess_)
{
}

binary::binary(const std::string& bit_string)
  : binary()
{
    std::stringstream(bit_string) >> *this;
}

binary::binary(size_type size, uint32_t number)
  : binary(size, to_little_endian(number))
{
}

binary::binary(size_type size, const data_slice& blocks)
  : binary()
{
    // Copy blocks
    blocks_.resize(blocks.size());

    if (blocks_.empty())
        return;

    std::copy_n(blocks.begin(), blocks.size(), blocks_.begin());

    // Pad with 00 for safety.
    while (blocks_.size() * bits_per_block < size)
        blocks_.push_back(0x00);

    resize(size);
}

void binary::resize(size_type size)
{
    final_block_excess_ = 0;
    blocks_.resize(blocks_size(size), 0);
    const size_type offset = size % bits_per_block;

    if (offset > 0)
    {
        // This subtraction is guarded above.
        final_block_excess_ = static_cast<uint8_t>(bits_per_block - offset);
        const auto last = safe_subtract(blocks_.size(), size_t(1));
        uint8_t mask = 0xFF << final_block_excess_;
        blocks_[last] &= mask;
    }
}

bool binary::operator[](size_type index) const
{
    BITCOIN_ASSERT(index < size());
    const size_type block_index = index / bits_per_block;
    const uint8_t block = blocks_[block_index];
    const size_type offset = index - (block_index * bits_per_block);
    const uint8_t bitmask = 1 << (bits_per_block - offset - 1);
    return (block & bitmask) > 0;
}

const data_chunk& binary::blocks() const
{
    return blocks_;
}

std::string binary::encoded() const
{
    std::stringstream bits;
    bits << *this;
    return bits.str();
}

binary::size_type binary::size() const
{
    const size_type base_bit_size = blocks_.size() * bits_per_block;
    return safe_subtract(base_bit_size,
        static_cast<size_type>(final_block_excess_));
}

void binary::append(const binary& post)
{
    const size_type block_offset = size() / bits_per_block;
    const size_type offset = size() % bits_per_block;

    // overkill for byte alignment
    binary duplicate(post.size(), post.blocks());
    duplicate.shift_right(offset);
    resize(size() + post.size());
    data_chunk post_shift_blocks = duplicate.blocks();

    for (size_type i = 0; i < post_shift_blocks.size(); i++)
        blocks_[block_offset + i] = blocks_[block_offset + i] |
            post_shift_blocks[i];
}

void binary::prepend(const binary& prior)
{
    shift_right(prior.size());
    data_chunk prior_blocks = prior.blocks();

    for (size_type i = 0; i < prior_blocks.size(); i++)
        blocks_[i] = blocks_[i] | prior_blocks[i];
}

void binary::shift_left(size_type distance)
{
    const size_type initial_size = size();
    const size_type initial_block_count = blocks_.size();
    size_type destination_size = 0;

    if (distance < initial_size)
        destination_size = initial_size - distance;

    const size_type block_offset = distance / bits_per_block;
    const size_type offset = distance % bits_per_block;

    // shift
    for (size_type i = 0; i < initial_block_count; i++)
    {
        uint8_t leading_bits = 0x00;
        uint8_t trailing_bits = 0x00;

        if ((offset != 0) && ((block_offset + i + 1) < initial_block_count))
            trailing_bits = blocks_[block_offset + i + 1] >>
                (bits_per_block - offset);

        if ((block_offset + i) < initial_block_count)
            leading_bits = blocks_[block_offset + i] << offset;

        blocks_[i] = leading_bits | trailing_bits;
    }

    resize(destination_size);
}

void binary::shift_right(size_type distance)
{
    const size_type initial_size = size();
    const size_type initial_block_count = blocks_.size();
    const size_type offset = distance % bits_per_block;
    const size_type offset_blocks = distance / bits_per_block;
    const size_type destination_size = initial_size + distance;

    for (size_type i = 0; i < offset_blocks; i++)
        blocks_.insert(blocks_.begin(), 0x00);

    uint8_t previous = 0x00;

    for (size_type i = 0; i < initial_block_count; i++)
    {
        uint8_t current = blocks_[offset_blocks + i];
        uint8_t leading_bits = previous << (bits_per_block - offset);
        uint8_t trailing_bits = current >> offset;
        blocks_[offset_blocks + i] = leading_bits | trailing_bits;
        previous = current;
    }

    resize(destination_size);

    if (offset_blocks + initial_block_count < blocks_.size())
        blocks_[blocks_.size() - 1] = previous << (bits_per_block - offset);
}

binary binary::substring(size_type start, size_type length) const
{
    size_type current_size = size();

    if (start > current_size)
        start = current_size;

    if ((length == max_size_t) || ((start + length) > current_size))
        length = current_size - start;

    binary result(current_size, blocks_);
    result.shift_left(start);
    result.resize(length);
    return result;
}

bool binary::is_prefix_of(uint32_t field) const
{
    return is_prefix_of(to_little_endian(field));
}

bool binary::is_prefix_of(const binary& field) const
{
    return is_prefix_of(field.blocks());
}

bool binary::is_prefix_of(const data_slice& field) const
{
    const binary truncated_prefix(size(), field);
    return *this == truncated_prefix;
}

bool binary::operator<(const binary& other) const
{
    return encoded() < other.encoded();
}

bool binary::operator==(const binary& other) const
{
    if (size() != other.size())
        return false;

    const auto self = *this;

    for (binary::size_type i = 0; i < size(); ++i)
        if (self[i] != other[i])
            return false;

    return true;
}

bool binary::operator!=(const binary& other) const
{
    return !(*this == other);
}

binary& binary::operator=(const binary& other)
{
    blocks_ = other.blocks_;
    final_block_excess_ = other.final_block_excess_;
    return *this;
}

std::istream& operator>>(std::istream& in, binary& to)
{
    std::string bitstring;
    in >> bitstring;

    to.resize(0);
    uint8_t block = 0;
    auto bit_iterator = binary::bits_per_block;

    for (const char representation: bitstring)
    {
        if (representation != '0' && representation != '1')
        {
            to.blocks_.clear();
            return in;
        }

        // Set bit to 1
        if (representation == '1')
        {
            const uint8_t bitmask = 1 << (bit_iterator - 1);
            block |= bitmask;
        }

        // Next bit
        --bit_iterator;

        if (bit_iterator == 0)
        {
            // Move to the next block.
            to.blocks_.push_back(block);
            block = 0;
            bit_iterator = binary::bits_per_block;
        }
    }

    // Block wasn't finished but push it back.
    if (bit_iterator != binary::bits_per_block)
        to.blocks_.push_back(block);

    to.resize(bitstring.size());
    return in;
}

std::ostream& operator<<(std::ostream& out, const binary& of)
{
    for (binary::size_type i = 0; i < of.size(); ++i)
        out << (of[i] ? '1' : '0');

    return out;
}

} // namespace system
} // namespace libbitcoin
