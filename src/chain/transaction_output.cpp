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

transaction_output::transaction_output(const data_chunk& value)
: transaction_output(value.begin(), value.end())
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

transaction_output::operator const data_chunk() const
{
    data_chunk result(satoshi_size());
    auto serial = make_serializer(result.begin());

    serial.write_8_bytes(value_);
    data_chunk raw_script = script_;
    serial.write_variable_uint(raw_script.size());
    serial.write_data(raw_script);

    BITCOIN_ASSERT(std::distance(result.begin(), serial.iterator()) == satoshi_size());

    return result;
}

size_t transaction_output::satoshi_size() const
{
    size_t script_size = script_.satoshi_size();

    return 8 + variable_uint_size(script_size) + script_size;
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
