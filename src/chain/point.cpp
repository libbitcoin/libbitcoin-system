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
#include <bitcoin/bitcoin/chain/point.hpp>
#include <sstream>
#include <boost/iostreams/stream.hpp>
#include <bitcoin/bitcoin/constants.hpp>
#include <bitcoin/bitcoin/formats/base16.hpp>
#include <bitcoin/bitcoin/utility/container_sink.hpp>
#include <bitcoin/bitcoin/utility/container_source.hpp>
#include <bitcoin/bitcoin/utility/istream.hpp>
#include <bitcoin/bitcoin/utility/ostream.hpp>

namespace libbitcoin {
namespace chain {

bool point::is_null() const
{
    return (index == max_index) && (hash == null_hash);
}

point point::factory_from_data(const data_chunk& data)
{
    point instance;
    instance.from_data(data);
    return instance;
}

point point::factory_from_data(std::istream& stream)
{
    point instance;
    instance.from_data(stream);
    return instance;
}

bool point::is_valid() const
{
    return (index != 0) || (hash != null_hash);
}

void point::reset()
{
    hash.fill(0);
    index = 0;
}

bool point::from_data(const data_chunk& data)
{
    boost::iostreams::stream<byte_source<data_chunk>> istream(data);
    return from_data(istream);
}

bool point::from_data(std::istream& stream)
{
    bool result = true;

    reset();

    hash = read_hash(stream);
    index = read_4_bytes(stream);
    result = stream;

    if (!result)
        reset();

    return result;
}

data_chunk point::to_data() const
{
    data_chunk data;
    boost::iostreams::stream<byte_sink<data_chunk>> ostream(data);
    to_data(ostream);
    BOOST_ASSERT(data.size() == satoshi_size());
    return data;
}

void point::to_data(std::ostream& stream) const
{
    write_hash(stream, hash);
    write_4_bytes(stream, index);
}

uint64_t point::satoshi_size() const
{
    return point::satoshi_fixed_size();
}

uint64_t point::satoshi_fixed_size()
{
    return hash_size + 4;
}

std::string point::to_string() const
{
    std::ostringstream ss;

    ss << "\thash = " << encode_hash(hash) << "\n" << "\tindex = " << index;

    return ss.str();
}

bool operator==(const point& a, const point& b)
{
    return a.hash == b.hash && a.index == b.index;
}
bool operator!=(const point& a, const point& b)
{
    return !(a == b);
}

} // end chain
} // end libbitcoin
