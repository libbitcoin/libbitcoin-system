/*
 * Copyright (c) 2011-2015 libbitcoin developers (see AUTHORS)
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
#include <bitcoin/bitcoin/primitives/blockchain/block_header.hpp>

namespace libbitcoin {

block_header_type::block_header_type()
{
}

block_header_type::block_header_type(const data_chunk& value)
    : block_header_type(value.begin(), value.end())
{
}

block_header_type::operator const data_chunk() const
{
    data_chunk result(satoshi_size());
    auto serial = make_serializer(result.begin());

    serial.write_4_bytes(version);
    serial.write_hash(previous_block_hash);
    serial.write_hash(merkle);
    serial.write_4_bytes(timestamp);
    serial.write_4_bytes(bits);
    serial.write_4_bytes(nonce);

    BITCOIN_ASSERT(
        std::distance(result.begin(), serial.iterator())
            == block_header_type::satoshi_fixed_size());

    return result;
}

size_t block_header_type::satoshi_size() const
{
    return block_header_type::satoshi_fixed_size();
}

size_t block_header_type::satoshi_fixed_size()
{
    return 80;
}

bool operator==(const block_header_type& block_a,
    const block_header_type& block_b)
{
    return block_a.version == block_b.version
        && block_a.previous_block_hash == block_b.previous_block_hash
        && block_a.merkle == block_b.merkle
        && block_a.timestamp == block_b.timestamp
        && block_a.bits == block_b.bits && block_a.nonce == block_b.nonce;
}

}
