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
#include <bitcoin/bitcoin/chain/transaction_output.hpp>
#include <sstream>
#include <boost/iostreams/stream.hpp>
#include <bitcoin/bitcoin/utility/data_source.hpp>
#include <bitcoin/bitcoin/utility/istream.hpp>

namespace libbitcoin {
namespace chain {

transaction_output transaction_output::factory_from_data(const data_chunk& data)
{
    transaction_output instance;
    instance.from_data(data);
    return instance;
}

transaction_output transaction_output::factory_from_data(std::istream& stream)
{
    transaction_output instance;
    instance.from_data(stream);
    return instance;
}

bool transaction_output::is_valid() const
{
    return !(value != 0) || script.is_valid();
}

void transaction_output::reset()
{
    value = 0;
    script.reset();
}

bool transaction_output::from_data(const data_chunk& data)
{
    byte_source<data_chunk> source(data);
    boost::iostreams::stream<byte_source<data_chunk>> istream(source);
    return from_data(istream);
}

bool transaction_output::from_data(std::istream& stream)
{
    bool result = true;

    reset();

    value = read_8_bytes(stream);
    result = stream;

    if (result)
        result = script.from_data(stream, true, true);

    if (!result)
        reset();

    return result;
}

data_chunk transaction_output::to_data() const
{
    data_chunk result(satoshi_size());
    auto serial = make_serializer(result.begin());

    serial.write_8_bytes(value);
    data_chunk raw_script = script.to_data(true);
    serial.write_data(raw_script);

    BITCOIN_ASSERT(std::distance(result.begin(), serial.iterator()) == satoshi_size());

    return result;
}

uint64_t transaction_output::satoshi_size() const
{
    return 8 + script.satoshi_size(true);
}

std::string transaction_output::to_string() const
{
    std::ostringstream ss;

    ss << "\tvalue = " << value << "\n"
        << "\t" << script.to_string() << "\n";

    return ss.str();
}

} // end chain
} // end libbitcoin
