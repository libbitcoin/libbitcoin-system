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
#include <bitcoin/bitcoin/message/get_blocks.hpp>
#include <bitcoin/bitcoin/constants.hpp>
#include <bitcoin/bitcoin/utility/istream.hpp>

namespace libbitcoin {
namespace message {

get_blocks::get_blocks()
    : start_hashes_(), hash_stop_()
{
}

get_blocks::get_blocks(const block_locator& start_hashes,
    const hash_digest& hash_stop)
    : start_hashes_(start_hashes), hash_stop_(hash_stop)
{
}

get_blocks::get_blocks(std::istream& stream)
{
    // Discard protocol version because it is stupid
    read_4_bytes(stream);

    // Note: changed to uint64_t to preclude possible loss of data.
    uint64_t count = read_variable_uint(stream);

    for (uint64_t i = 0; (i < count) && !stream.fail(); ++i)
    {
        hash_digest start_hash = read_hash(stream);
        start_hashes_.push_back(start_hash);
    }

    hash_stop_ = read_hash(stream);

    if (stream.fail())
        throw std::ios_base::failure("get_blocks");
}

//get_blocks::get_blocks(const data_chunk& value)
//: get_blocks(value.begin(), value.end())
//{
//}

block_locator& get_blocks::start_hashes()
{
    return start_hashes_;
}

const block_locator& get_blocks::start_hashes() const
{
    return start_hashes_;
}

void get_blocks::start_hashes(const block_locator& value)
{
    start_hashes_ = value;
}

hash_digest& get_blocks::hash_stop()
{
    return hash_stop_;
}

const hash_digest& get_blocks::hash_stop() const
{
    return hash_stop_;
}

void get_blocks::hash_stop(const hash_digest& value)
{
    hash_stop_ = value;
}

get_blocks::operator const data_chunk() const
{
    data_chunk result(satoshi_size());
    auto serial = make_serializer(result.begin());

    serial.write_4_bytes(protocol_version);

    serial.write_variable_uint(start_hashes_.size());

    for (hash_digest start_hash: start_hashes_)
    {
        serial.write_hash(start_hash);
    }

    serial.write_hash(hash_stop_);

    return result;
}

size_t get_blocks::satoshi_size() const
{
    return 36 +
        variable_uint_size(start_hashes_.size()) +
        hash_size * start_hashes_.size();
}

} // end message
} // end libbitcoin
