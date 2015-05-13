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

transaction_output::transaction_output()
    : value_(0), script_()
{
}

transaction_output::transaction_output(uint64_t value,
    const chain::script& script)
    : value_(value), script_(script)
{
}

chain::script& transaction_output::script()
{
    return script_;
}

const chain::script& transaction_output::script() const
{
    return script_;
}

void transaction_output::script(const chain::script& script)
{
    script_ = script;
}

uint64_t transaction_output::value() const
{
    return value_;
}

void transaction_output::value(const uint64_t value)
{
    value_ = value;
}

void transaction_output::reset()
{
    value_ = 0;
    script_.reset();
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

    value_ = read_8_bytes(stream);
    result = !stream.fail();

    if (result)
        result = script_.from_data(stream, true);

    if (!result)
        reset();

    return result;
}

data_chunk transaction_output::to_data() const
{
    data_chunk result(satoshi_size());
    auto serial = make_serializer(result.begin());

    serial.write_8_bytes(value_);
    data_chunk raw_script = script_.to_data(true);
    serial.write_data(raw_script);

    BITCOIN_ASSERT(std::distance(result.begin(), serial.iterator()) == satoshi_size());

    return result;
}

uint64_t transaction_output::satoshi_size() const
{
    return 8 + script_.satoshi_size(true);
}

std::string transaction_output::to_string() const
{
    std::ostringstream ss;

    ss << "\tvalue = " << value_ << "\n"
        << "\t" << script_.to_string() << "\n";

    return ss.str();
}

} // end chain
} // end libbitcoin
