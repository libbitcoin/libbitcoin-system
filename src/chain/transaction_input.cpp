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
#include <bitcoin/bitcoin/constants.hpp>
#include <bitcoin/bitcoin/utility/istream.hpp>

namespace libbitcoin {
namespace chain {

transaction_input::transaction_input()
    : previous_output_(), script_(), sequence_(0)
{
}

transaction_input::transaction_input(const output_point& previous_output,
    const chain::script& script, const uint32_t sequence)
    : previous_output_(previous_output), script_(script), sequence_(sequence)
{
}

transaction_input::transaction_input(std::istream& stream)
    : previous_output_(stream), script_(stream), sequence_(read_4_bytes(stream))
{
    if (stream.fail())
        throw std::ios_base::failure("transaction_input");
}

//transaction_input::transaction_input(const data_chunk& value)
//: transaction_input(value.begin(), value.end())
//{
//}

output_point& transaction_input::previous_output()
{
    return previous_output_;
}

const output_point& transaction_input::previous_output() const
{
    return previous_output_;
}

void transaction_input::previous_output(const output_point& previous)
{
    previous_output_ = previous;
}

chain::script& transaction_input::script()
{
    return script_;
}

const chain::script& transaction_input::script() const
{
    return script_;
}

void transaction_input::script(const chain::script& script)
{
    script_ = script;
}

uint32_t transaction_input::sequence() const
{
    return sequence_;
}

void transaction_input::sequence(const uint32_t sequence)
{
    sequence_ = sequence;
}

transaction_input::operator const data_chunk() const
{
    data_chunk result(satoshi_size());
    auto serial = make_serializer(result.begin());

    data_chunk raw_prevout = previous_output_;
    serial.write_data(raw_prevout);
    data_chunk raw_script = script_;
    serial.write_variable_uint(raw_script.size());
    serial.write_data(raw_script);
    serial.write_4_bytes(sequence_);

    BITCOIN_ASSERT(std::distance(result.begin(), serial.iterator()) == satoshi_size());

    return result;
}

size_t transaction_input::satoshi_size() const
{
    size_t script_size = script_.satoshi_size();

    return 4 + previous_output_.satoshi_size()
        + variable_uint_size(script_size) + script_size;
}

std::string transaction_input::to_string() const
{
    std::ostringstream ss;

    ss << previous_output_.to_string() << "\n"
        << "\t" << script_.to_string() << "\n"
        << "\tsequence = " << sequence_ << "\n";

    return ss.str();
}

bool transaction_input::is_final() const
{
    return (sequence_ == max_sequence);
}

} // end chain
} // end libbitcoin
