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
#include <bitcoin/bitcoin/utility/data_stream_host.hpp>
#include <bitcoin/bitcoin/utility/istream.hpp>

namespace libbitcoin {
namespace chain {

block_header::block_header()
{
}

block_header::block_header(uint32_t version, hash_digest previous_block_hash,
    hash_digest merkle, uint32_t timestamp, uint32_t bits, uint32_t nonce)
    : version_(version), previous_block_hash_(previous_block_hash),
      merkle_(merkle), timestamp_(timestamp), bits_(bits), nonce_(nonce)
{
}

uint32_t block_header::version() const
{
    return version_;
}

void block_header::version(uint32_t version)
{
    version_ = version;
}

hash_digest& block_header::previous_block_hash()
{
    return previous_block_hash_;
}

const hash_digest& block_header::previous_block_hash() const
{
    return previous_block_hash_;
}

void block_header::previous_block_hash(const hash_digest& hash)
{
    previous_block_hash_ = hash;
}

hash_digest& block_header::merkle()
{
    return merkle_;
}

const hash_digest& block_header::merkle() const
{
    return merkle_;
}

void block_header::merkle(const hash_digest& merkle)
{
    merkle_ = merkle;
}

uint32_t block_header::timestamp() const
{
    return timestamp_;
}

void block_header::timestamp(uint32_t timestamp)
{
    timestamp_ = timestamp;
}

uint32_t block_header::bits() const
{
    return bits_;
}

void block_header::bits(uint32_t bits)
{
    bits_ = bits;
}

uint32_t block_header::nonce() const
{
    return nonce_;
}

void block_header::nonce(uint32_t nonce)
{
    nonce_ = nonce;
}

void block_header::reset()
{
    version_ = 0;
    previous_block_hash_.fill(0);
    merkle_.fill(0);
    timestamp_ = 0;
    bits_ = 0;
    nonce_ = 0;
}

bool block_header::from_data(const data_chunk& data)
{
    data_chunk_stream_host host(data);
    return from_data(host.stream);
}

bool block_header::from_data(std::istream& stream)
{
    bool result = true;

    reset();

    version_ = read_4_bytes(stream);
    previous_block_hash_ = read_hash(stream);
    merkle_ = read_hash(stream);
    timestamp_ = read_4_bytes(stream);
    bits_ = read_4_bytes(stream);
    nonce_ = read_4_bytes(stream);

    result = !stream.fail();

    if (!result)
        reset();

    return result;
}

data_chunk block_header::to_data() const
{
    data_chunk result(satoshi_size());
    auto serial = make_serializer(result.begin());

    serial.write_4_bytes(version_);
    serial.write_hash(previous_block_hash_);
    serial.write_hash(merkle_);
    serial.write_4_bytes(timestamp_);
    serial.write_4_bytes(bits_);
    serial.write_4_bytes(nonce_);

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
    const hash_digest hash_a = block_a.previous_block_hash();
    const hash_digest hash_b = block_b.previous_block_hash();
    const hash_digest merkle_a = block_a.merkle();
    const hash_digest merkle_b = block_b.merkle();

    return (block_a.version() == block_b.version())
        && (hash_a == hash_b)
        && (merkle_a == merkle_b)
        && (block_a.timestamp() == block_b.timestamp())
        && (block_a.bits() == block_b.bits())
        && (block_a.nonce() == block_b.nonce());
}

hash_digest block_header::hash() const
{
    return bitcoin_hash(to_data());
}

} // end chain
} // end libbitcoin
