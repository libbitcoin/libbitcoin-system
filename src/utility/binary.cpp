/*
 * Copyright (c) 2011-2013 libbitcoin developers (see AUTHORS)
 *
 * This file is part of libbitcoin.
 *
 * libbitcoin is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License with
 * additional permissions to the one published by the Free Software
 * Foundation, either version 3 of the License, or (at your option)
 * any later version. For more information see LICENSE.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#include <bitcoin/bitcoin/utility/binary.hpp>

#include <sstream>
#include <bitcoin/bitcoin/utility/assert.hpp>
#include <iostream>

namespace libbitcoin {

binary_type::size_type binary_type::blocks_size(const size_type bitsize)
{
    if (bitsize == 0)
        return 0;
    return (bitsize - 1) / bits_per_block + 1;
}

binary_type::binary_type()
{
}
binary_type::binary_type(const std::string& bitstring)
{
    std::stringstream(bitstring) >> *this;
}
binary_type::binary_type(size_type size, data_slice blocks)
{
    // Copy blocks
    blocks_.resize(blocks.size());
    std::copy(blocks.begin(), blocks.end(), blocks_.begin());
    // Pad with 00 for safety.
    while (blocks_.size() * bits_per_block < size)
        blocks_.push_back(0x00);
    resize(size);
}

void binary_type::resize(size_type size)
{
    size_ = size;
    blocks_.resize(blocks_size(size_), 0);

    size_type offset = size_ % bits_per_block;

    if (offset > 0)
    {
        uint8_t mask = 0xFF << (bits_per_block - offset);
        blocks_[blocks_.size() - 1] = blocks_[blocks_.size() - 1] & mask;
    }
}

bool binary_type::operator[](size_type index) const
{
    BITCOIN_ASSERT(index < size_);
    const size_type block_index = index / bits_per_block;
    const uint8_t block = blocks_[block_index];
    const size_type offset = index - (block_index * bits_per_block);
    const uint8_t bitmask = 1 << (bits_per_block - offset - 1);
    return (block & bitmask) > 0;
}

const data_chunk& binary_type::blocks() const
{
    return blocks_;
}

binary_type::size_type binary_type::size() const
{
    return size_;
}

void binary_type::append(const binary_type& post)
{
    const size_type block_offset = size_ / bits_per_block;
    const size_type offset = size_ % bits_per_block;

    // overkill for byte alignment
    binary_type duplicate(post.size(), post.blocks());
    duplicate.shift_right(offset);

    resize(size_ + post.size());

    data_chunk post_shift_blocks = duplicate.blocks();

    for (size_type i = 0; i < post_shift_blocks.size(); i++)
    {
        blocks_[block_offset + i] = blocks_[block_offset + i] | post_shift_blocks[i];
    }
}

void binary_type::prepend(const binary_type& prior)
{
    shift_right(prior.size());

    data_chunk prior_blocks = prior.blocks();

    for (size_type i = 0; i < prior_blocks.size(); i++)
    {
        blocks_[i] = blocks_[i] | prior_blocks[i];
    }
}

void binary_type::shift_left(size_type distance)
{
    const size_type initial_size = size_;
    const size_type initial_block_count = blocks_.size();

    size_type destination_size = 0;

    if (distance < initial_size)
        destination_size = initial_size - distance;

    const size_type block_offset = distance / bits_per_block;
    const size_type offset = distance % bits_per_block;

    // shift
    for (size_type i = 0; i < initial_block_count; i++)
    {
        uint8_t trailing_bits = 0x00;

        if ((offset != 0) && ((block_offset + i + 1) < initial_block_count))
        {
            trailing_bits = blocks_[block_offset + i + 1] >> (bits_per_block - offset);
        }

        uint8_t leading_bits = blocks_[block_offset + i] << offset;

        blocks_[i] = leading_bits | trailing_bits;
    }

    resize(destination_size);
}

void binary_type::shift_right(size_type distance)
{
    const size_type initial_size = size_;
    const size_type initial_block_count = blocks_.size();

    const size_type offset = distance % bits_per_block;
    const size_type offset_blocks = distance / bits_per_block;
    const size_type destination_size = initial_size + distance;

    for (size_type i = 0; i < offset_blocks; i++)
    {
        blocks_.insert(blocks_.begin(), 0x00);
    }

    size_ = initial_size + (offset_blocks * bits_per_block);

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
    {
        blocks_[blocks_.size() - 1] = previous << (bits_per_block - offset);
    }
}

binary_type binary_type::get_substring(size_type start, size_type length) const
{
    if (start > size_)
    {
        start = size_;
    }

    if ((start + length) > size_)
    {
        length = size_ - start;
    }

    binary_type result(size_, blocks_);

    result.shift_left(start);

    result.resize(length);

    return result;
}

bool operator==(
    const binary_type& prefix_a, const binary_type& prefix_b)
{
    for (binary_type::size_type i = 0; i < prefix_a.size() && i < prefix_b.size(); ++i)
        if (prefix_a[i] != prefix_b[i])
            return false;
    return true;
}
bool operator!=(
    const binary_type& prefix_a, const binary_type& prefix_b)
{
    return !(prefix_a == prefix_b);
}

std::istream& operator>>(
    std::istream& stream, binary_type& prefix)
{
    std::string bitstring;
    stream >> bitstring;

    prefix.size_ = 0;
    prefix.blocks_.clear();

    uint8_t block = 0;
    binary_type::size_type bit_iter = binary_type::bits_per_block;

    for (const char repr : bitstring)
    {
        if (repr != '0' && repr != '1')
        {
            prefix.blocks_.clear();
            return stream;
        }

        // Set bit to 1
        if (repr == '1')
        {
            const uint8_t bitmask = 1 << (bit_iter - 1);
            block |= bitmask;
        }

        // Next bit
        --bit_iter;

        if (bit_iter == 0)
        {
            // Move to the next block.
            prefix.blocks_.push_back(block);
            block = 0;
            bit_iter = binary_type::bits_per_block;
        }
    }

    // Block wasn't finished but push it back.
    if (bit_iter != binary_type::bits_per_block)
        prefix.blocks_.push_back(block);

    prefix.size_ = bitstring.size();
    return stream;
}
std::ostream& operator<<(
    std::ostream& stream, const binary_type& prefix)
{
    for (binary_type::size_type i = 0; i < prefix.size(); ++i)
        if (prefix[i])
            stream << '1';
        else
            stream << '0';
    return stream;
}

} // namespace libbitcoin

