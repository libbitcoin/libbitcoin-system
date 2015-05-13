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
#include <bitcoin/bitcoin/utility/istream.hpp>
#include <bitcoin/bitcoin/utility/data_stream_host.hpp>

namespace libbitcoin {
namespace chain {

void transaction_output::reset()
{
    value = 0;
    script.reset();
}

bool transaction_output::from_data(const data_chunk& data)
{
    data_chunk_stream_host host(data);
    return from_data(host.stream);
}

bool transaction_output::from_data(std::istream& stream)
{
    bool result = true;

    reset();

    value = read_8_bytes(stream);
    result = !stream.fail();

    if (result)
        result = script.from_data(stream, true);

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
