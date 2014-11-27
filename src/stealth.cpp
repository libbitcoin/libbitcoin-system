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
#include <bitcoin/bitcoin/constants.hpp>
#include <bitcoin/bitcoin/stealth.hpp>
#include <bitcoin/bitcoin/utility/assert.hpp>
#include <bitcoin/bitcoin/utility/hash.hpp>
#include <bitcoin/bitcoin/utility/serializer.hpp>

namespace libbitcoin {

stealth_prefix::stealth_prefix()
{
}
stealth_prefix::stealth_prefix(const std::string& bitstring)
{
    std::stringstream(bitstring) >> *this;
}
stealth_prefix::stealth_prefix(size_t size, const data_slice& blocks)
{
    // Copy blocks
    blocks_.resize(blocks.size());
    std::copy(blocks.begin(), blocks.end(), blocks_.begin());
    // Pad with 00 for safety.
    while (blocks_.size() * bits_per_block < size)
        blocks_.push_back(0x00);
    resize(size);
}

void stealth_prefix::resize(size_t size)
{
    size_ = size;
    blocks_.resize(stealth_blocks_size(size_), 0);
}

bool stealth_prefix::operator[](size_t i) const
{
    BITCOIN_ASSERT(i < size_);
    const size_t block_index = i / bits_per_block;
    const uint8_t block = blocks_[block_index];
    const size_t offset = i - (block_index * bits_per_block);
    const uint8_t bitmask = 1 << (bits_per_block - offset - 1);
    return (block & bitmask) > 0;
}

const data_chunk& stealth_prefix::blocks() const
{
    return blocks_;
}

size_t stealth_prefix::size() const
{
    return size_;
}

bool operator==(
    const stealth_prefix& prefix_a, const stealth_prefix& prefix_b)
{
    for (size_t i = 0; i < prefix_a.size() && i < prefix_b.size(); ++i)
        if (prefix_a[i] != prefix_b[i])
            return false;
    return true;
}
bool operator!=(
    const stealth_prefix& prefix_a, const stealth_prefix& prefix_b)
{
    return !(prefix_a == prefix_b);
}

std::istream& operator>>(
    std::istream& stream, stealth_prefix& prefix)
{
    std::string bitstring;
    stream >> bitstring;

    prefix.size_ = 0;
    prefix.blocks_.clear();

    uint8_t block = 0;
    size_t bit_iter = stealth_prefix::bits_per_block;

    for (const char repr: bitstring)
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
            bit_iter = stealth_prefix::bits_per_block;
        }
    }

    // Block wasn't finished but push it back.
    if (bit_iter != stealth_prefix::bits_per_block)
        prefix.blocks_.push_back(block);

    prefix.size_ = bitstring.size();
    return stream;
}

std::ostream& operator<<(
    std::ostream& stream, const stealth_prefix& prefix)
{
    for (size_t i = 0; i < prefix.size(); ++i)
        if (prefix[i])
            stream << '1';
        else
            stream << '0';
    return stream;
}

size_t stealth_blocks_size(const size_t bitsize)
{
    if (bitsize == 0)
        return 0;
    return (bitsize - 1) / stealth_prefix::bits_per_block + 1;
}

stealth_prefix calculate_stealth_prefix(
    const script_type& stealth_script)
{
    const data_chunk stealth_data = save_script(stealth_script);
    const hash_digest index = bitcoin_hash(stealth_data);
    const size_t bitsize = stealth_prefix::bits_per_block * sizeof(uint32_t);
    return stealth_prefix(bitsize, index);
}

} // namespace libbitcoin

