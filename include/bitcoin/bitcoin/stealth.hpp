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
#ifndef LIBBITCOIN_STEALTH_HPP
#define LIBBITCOIN_STEALTH_HPP

#include <boost/dynamic_bitset.hpp>
#include <bitcoin/bitcoin/types.hpp>

namespace libbitcoin {

typedef uint32_t stealth_bitfield;

class BC_API stealth_prefix
{
public:
    typedef boost::dynamic_bitset<uint8_t> bitset_type;
    static const int bits_per_block = bitset_type::bits_per_block;

    stealth_prefix();
    stealth_prefix(const std::string& repr);
    stealth_prefix(size_t number_bits, uint32_t value);
    stealth_prefix(size_t number_bits, data_chunk data);

    template <typename BlockIter>
    void init_from_block_range(BlockIter first, BlockIter last)
    {
        bitset_.init_from_block_range(first, last);
    }

    void resize(size_t num_bits);

    bool operator[](size_t i) const;

    uint32_t to_ulong() const;

    size_t size() const;
    size_t num_blocks() const;

private:
    friend bool operator==(
        const stealth_prefix& prefix_a, const stealth_prefix& prefix_b);
    friend std::ostream& operator<<(
        std::ostream& stream, const stealth_prefix& prefix);

    bitset_type bitset_;
};

BC_API bool operator==(
    const stealth_prefix& prefix_a, const stealth_prefix& prefix_b);
BC_API std::ostream& operator<<(
    std::ostream& stream, const stealth_prefix& prefix);

BC_API bool stealth_match(
    const stealth_prefix& prefix, const uint8_t* raw_bitfield);

BC_API stealth_bitfield calculate_stealth_bitfield(
    const data_chunk& stealth_data);

} // namespace libbitcoin

#endif

