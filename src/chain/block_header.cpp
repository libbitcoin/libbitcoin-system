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
#include <bitcoin/bitcoin/chain/block_header.hpp>
#include <boost/iostreams/stream.hpp>
#include <bitcoin/bitcoin/utility/data_source.hpp>
#include <bitcoin/bitcoin/utility/istream.hpp>

namespace libbitcoin {
namespace chain {

void block_header::reset()
{
    version = 0;
    previous_block_hash.fill(0);
    merkle.fill(0);
    timestamp = 0;
    bits = 0;
    nonce = 0;
}

bool block_header::from_data(const data_chunk& data)
{
    byte_source<data_chunk> source(data);
    boost::iostreams::stream<byte_source<data_chunk>> istream(source);
    return from_data(istream);
}

bool block_header::from_data(std::istream& stream)
{
    bool result = true;

    reset();

    version = read_4_bytes(stream);
    previous_block_hash = read_hash(stream);
    merkle = read_hash(stream);
    timestamp = read_4_bytes(stream);
    bits = read_4_bytes(stream);
    nonce = read_4_bytes(stream);

    result = stream;

    if (!result)
        reset();

    return result;
}

data_chunk block_header::to_data() const
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
            == block_header::satoshi_fixed_size());

    return result;
}

uint64_t block_header::satoshi_size() const
{
    return block_header::satoshi_fixed_size();
}

uint64_t block_header::satoshi_fixed_size()
{
    return 80;
}

bool operator==(const block_header& block_a,
    const block_header& block_b)
{
    return (block_a.version == block_b.version)
        && (block_a.previous_block_hash == block_b.previous_block_hash)
        && (block_a.merkle == block_b.merkle)
        && (block_a.timestamp == block_b.timestamp)
        && (block_a.bits == block_b.bits)
        && (block_a.nonce == block_b.nonce);
}

hash_digest block_header::hash() const
{
    return bitcoin_hash(to_data());
}

} // end chain
} // end libbitcoin
