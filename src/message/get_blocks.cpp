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
#include <bitcoin/bitcoin/utility/data_stream_host.hpp>
#include <bitcoin/bitcoin/utility/istream.hpp>

namespace libbitcoin {
namespace message {

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

void get_blocks::reset()
{
    start_hashes_.clear();
    hash_stop_.fill(0);
}

bool get_blocks::from_data(const data_chunk& data)
{
    data_chunk_stream_host host(data);
    return from_data(host.stream);
}

bool get_blocks::from_data(std::istream& stream)
{
    bool result = true;

    reset();

    // Discard protocol version because it is stupid
    read_4_bytes(stream);

    // Note: changed to uint64_t to preclude possible loss of data.
    uint64_t count = read_variable_uint(stream);
    result = !stream.fail();

    for (uint64_t i = 0; (i < count) && result; ++i)
    {
        start_hashes_.push_back(read_hash(stream));
        result = !stream.fail();
    }

    if (result)
    {
        hash_stop_ = read_hash(stream);
        result = !stream.fail();
    }

    if (!result)
        reset();

    return result;
}

data_chunk get_blocks::to_data() const
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

uint64_t get_blocks::satoshi_size() const
{
    return 36 +
        variable_uint_size(start_hashes_.size()) +
        hash_size * start_hashes_.size();
}

} // end message
} // end libbitcoin
