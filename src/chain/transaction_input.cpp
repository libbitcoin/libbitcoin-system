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
#include <bitcoin/bitcoin/chain/transaction_input.hpp>
#include <sstream>
#include <boost/iostreams/stream.hpp>
#include <bitcoin/bitcoin/constants.hpp>
#include <bitcoin/bitcoin/utility/container_sink.hpp>
#include <bitcoin/bitcoin/utility/container_source.hpp>
#include <bitcoin/bitcoin/utility/istream_reader.hpp>
#include <bitcoin/bitcoin/utility/ostream_writer.hpp>

namespace libbitcoin {
namespace chain {

transaction_input transaction_input::factory_from_data(const data_chunk& data)
{
    transaction_input instance;
    instance.from_data(data);
    return instance;
}

transaction_input transaction_input::factory_from_data(std::istream& stream)
{
    transaction_input instance;
    instance.from_data(stream);
    return instance;
}

transaction_input transaction_input::factory_from_data(reader& source)
{
    transaction_input instance;
    instance.from_data(source);
    return instance;
}

bool transaction_input::is_valid() const
{
    return (sequence != 0) ||
        previous_output.is_valid() ||
        script.is_valid();
}

void transaction_input::reset()
{
    previous_output.reset();
    script.reset();
    sequence = 0;
}

bool transaction_input::from_data(const data_chunk& data)
{
    boost::iostreams::stream<byte_source<data_chunk>> istream(data);
    return from_data(istream);
}

bool transaction_input::from_data(std::istream& stream)
{
    istream_reader source(stream);
    return from_data(source);
}

bool transaction_input::from_data(reader& source)
{
    bool result = true;

    reset();

    result = previous_output.from_data(source);

    if (result)
    {
        script::parse_mode mode = script::parse_mode::strict;

        if (previous_output.is_null())
            mode = script::parse_mode::raw_data;

        result = script.from_data(source, true, mode);
    }

    if (result)
    {
        sequence = source.read_4_bytes_little_endian();
        result = source;
    }

    if (!result)
        reset();

    return result;
}

data_chunk transaction_input::to_data() const
{
    data_chunk data;
    boost::iostreams::stream<byte_sink<data_chunk>> ostream(data);
    to_data(ostream);
    ostream.flush();
    BITCOIN_ASSERT(data.size() == satoshi_size());
    return data;
}

void transaction_input::to_data(std::ostream& stream) const
{
    ostream_writer sink(stream);
    to_data(sink);
}

void transaction_input::to_data(writer& sink) const
{
    previous_output.to_data(sink);
    script.to_data(sink, true);
    sink.write_4_bytes_little_endian(sequence);
}

uint64_t transaction_input::satoshi_size() const
{
    uint64_t script_size = script.satoshi_size(true);
    return 4 + previous_output.satoshi_size() + script_size;
}

std::string transaction_input::to_string() const
{
    std::ostringstream ss;

    ss << previous_output.to_string() << "\n"
        << "\t" << script.to_string() << "\n"
        << "\tsequence = " << sequence << "\n";

    return ss.str();
}

bool transaction_input::is_final() const
{
    return (sequence == max_sequence);
}

} // end chain
} // end libbitcoin
