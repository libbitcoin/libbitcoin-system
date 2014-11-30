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

#include <bitcoin/bitcoin/utility/assert.hpp>

namespace libbitcoin {

size_t binary_type::blocks_size(const size_t bitsize)
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
binary_type::binary_type(size_t size, const data_slice& blocks)
{
    // Copy blocks
    blocks_.resize(blocks.size());
    std::copy(blocks.begin(), blocks.end(), blocks_.begin());
    // Pad with 00 for safety.
    while (blocks_.size() * bits_per_block < size)
        blocks_.push_back(0x00);
    resize(size);
}

void binary_type::resize(size_t size)
{
    size_ = size;
    blocks_.resize(blocks_size(size_), 0);
}

bool binary_type::operator[](size_t index) const
{
    BITCOIN_ASSERT(index < size_);
    const size_t block_index = index / bits_per_block;
    const uint8_t block = blocks_[block_index];
    const size_t offset = index - (block_index * bits_per_block);
    const uint8_t bitmask = 1 << (bits_per_block - offset - 1);
    return (block & bitmask) > 0;
}

const data_chunk& binary_type::blocks() const
{
    return blocks_;
}

size_t binary_type::size() const
{
    return size_;
}

bool operator==(
    const binary_type& prefix_a, const binary_type& prefix_b)
{
    for (size_t i = 0; i < prefix_a.size() && i < prefix_b.size(); ++i)
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
    size_t bit_iter = binary_type::bits_per_block;

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
    for (size_t i = 0; i < prefix.size(); ++i)
        if (prefix[i])
            stream << '1';
        else
            stream << '0';
    return stream;
}

} // namespace libbitcoin

