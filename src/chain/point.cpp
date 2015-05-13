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
#include <bitcoin/bitcoin/constants.hpp>
#include <bitcoin/bitcoin/formats/base16.hpp>
#include <bitcoin/bitcoin/utility/data_stream_host.hpp>
#include <bitcoin/bitcoin/utility/istream.hpp>
#include <bitcoin/bitcoin/utility/serializer.hpp>

namespace libbitcoin {
namespace chain {

bool point::is_null() const
{
    return (index == max_index) && (hash == null_hash);
}

void point::reset()
{
    hash.fill(0);
    index = 0;
}

bool point::from_data(const data_chunk& data)
{
    data_chunk_stream_host host(data);
    return from_data(host.stream);
}

bool point::from_data(std::istream& stream)
{
    bool result = true;

    reset();

    hash = read_hash(stream);
    index = read_4_bytes(stream);
    result = !stream.fail();

    if (!result)
        reset();

    return result;
}

data_chunk point::to_data() const
{
    data_chunk result(satoshi_size());
    auto serial = make_serializer(result.begin());

    serial.write_hash(hash);
    serial.write_4_bytes(index);

    BITCOIN_ASSERT(
        std::distance(result.begin(), serial.iterator())
            == point::satoshi_fixed_size());

    return result;
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
