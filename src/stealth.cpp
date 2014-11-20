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

data_chunk value_to_blocks(uint32_t value)
{
    auto array = to_big_endian(value);
    return data_chunk(array.begin(), array.end());
}

stealth_prefix::stealth_prefix()
{
}
stealth_prefix::stealth_prefix(const std::string& bitstring)
{
    size_ = bitstring.size();
    uint8_t byte = 0;
    size_t bit_iter = bits_per_block - 1;
    for (const char repr: bitstring)
    {
        BITCOIN_ASSERT(repr == '0' || repr == '1');
        const uint8_t bitmask = 1 << bit_iter;
        if (repr == '1')
            byte |= bitmask;
        if (bit_iter == 0)
        {
            blocks_.push_back(byte);
            byte = 0;
            bit_iter = bits_per_block - 1;
            continue;
        }
        --bit_iter;
    }
    if (bit_iter != 0)
        blocks_.push_back(byte);
}
stealth_prefix::stealth_prefix(size_t size, uint32_t value)
  : stealth_prefix(size, value_to_blocks(value))
{
}
stealth_prefix::stealth_prefix(size_t size, const data_chunk& blocks)
  : blocks_(blocks)
{
    // Pad with 00 for safety.
    while (blocks_.size() * bits_per_block < size)
        blocks_.push_back(0x00);
    resize(size);
}

void stealth_prefix::resize(size_t size)
{
    size_ = size;
    if (size == 0)
    {
        blocks_.clear();
        return;
    }
    const size_t num_blocks = (size - 1) / bits_per_block + 1;
    blocks_.resize(num_blocks);

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

uint32_t stealth_prefix::uint32() const
{
    if (blocks_.empty())
        return 0;
    uint32_t value = from_big_endian<uint32_t>(blocks_.begin());
    // Zero remaining bits
    for (size_t i = size_; i < 32; ++i)
    {
        const uint32_t bitmask = 1 << (32 - i - 1);
        value &= ~bitmask;
    }
    return value;
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
    return prefix_a.blocks_ == prefix_b.blocks_;
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

bool stealth_match(const stealth_prefix& prefix, const uint8_t* raw_bitfield)
{
    size_t current_bit = 0;
    for (size_t i = 0; i < prefix.size(); ++i)
    {
        if (i > 0 && i % byte_bits == 0)
        {
            // Move to next byte.
            ++raw_bitfield;
            current_bit = 0;
        }
        BITCOIN_ASSERT(current_bit < byte_bits);
        const uint32_t bitmask = 1 << (byte_bits - current_bit - 1);
        bool value = (*raw_bitfield & bitmask) > 0;
        if (value != prefix[i])
            return false;
        ++current_bit;
    }
    return true;
}

stealth_bitfield calculate_stealth_bitfield(const data_chunk& stealth_data)
{
    constexpr size_t bitfield_size = sizeof(stealth_bitfield);
    // Calculate stealth bitfield
    const hash_digest index = bitcoin_hash(stealth_data);
    auto deserial = make_deserializer(
        index.begin(), index.begin() + bitfield_size);
    auto bitfield = deserial.read_little_endian<stealth_bitfield>();
    return bitfield;
}

} // namespace libbitcoin

