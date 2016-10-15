/**
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#include <bitcoin/bitcoin/chain/output.hpp>

#include <cstdint>
#include <sstream>
#include <bitcoin/bitcoin/constants.hpp>
#include <bitcoin/bitcoin/utility/container_sink.hpp>
#include <bitcoin/bitcoin/utility/container_source.hpp>
#include <bitcoin/bitcoin/utility/istream_reader.hpp>
#include <bitcoin/bitcoin/utility/ostream_writer.hpp>

namespace libbitcoin {
namespace chain {

// This is a consensus critical value that must be set on reset.
const uint64_t output::not_found = sighash_null_value;

output output::factory_from_data(const data_chunk& data)
{
    output instance;
    instance.from_data(data);
    return instance;
}

output output::factory_from_data(std::istream& stream)
{
    output instance;
    instance.from_data(stream);
    return instance;
}

output output::factory_from_data(reader& source)
{
    output instance;
    instance.from_data(source);
    return instance;
}

output::output()
  : value_(not_found), script_()
{
}

output::output(uint64_t value, const chain::script& script)
  : value_(value), script_(script)
{
}

output::output(uint64_t value, chain::script&& script)
  : value_(value), script_(std::move(script))
{
}

output::output(const output& other)
  : output(other.value_, other.script_)
{
}

output::output(output&& other)
  : output(other.value_, std::move(other.script_))
{
}

// Empty scripts are valid, validation relies on not_found only.
bool output::is_valid() const
{
    return value_ != output::not_found;
}

void output::reset()
{
    value_ = output::not_found;
    script_.reset();
}

bool output::from_data(const data_chunk& data)
{
    data_source istream(data);
    return from_data(istream);
}

bool output::from_data(std::istream& stream)
{
    istream_reader source(stream);
    return from_data(source);
}

bool output::from_data(reader& source)
{
    reset();

    value_ = source.read_8_bytes_little_endian();

    // Always parse non-coinbase input/output scripts as fallback.
    script_.from_data(source, true, script::parse_mode::raw_data_fallback);

    if (!source)
        reset();

    return source;
}

data_chunk output::to_data() const
{
    data_chunk data;
    data_sink ostream(data);
    to_data(ostream);
    ostream.flush();
    BITCOIN_ASSERT(data.size() == serialized_size());
    return data;
}

void output::to_data(std::ostream& stream) const
{
    ostream_writer sink(stream);
    to_data(sink);
}

void output::to_data(writer& sink) const
{
    sink.write_8_bytes_little_endian(value_);
    script_.to_data(sink, true);
}

uint64_t output::serialized_size() const
{
    return 8 + script_.serialized_size(true);
}

size_t output::signature_operations() const
{
    return script_.sigops(false);
}

std::string output::to_string(uint32_t flags) const
{
    std::ostringstream ss;

    ss << "\tvalue = " << value_ << "\n"
        << "\t" << script_.to_string(flags) << "\n";

    return ss.str();
}

uint64_t output::value() const
{
    return value_;
}

void output::set_value(uint64_t value)
{
    value_ = value;
}

chain::script& output::script()
{
    return script_;
}

const chain::script& output::script() const
{
    return script_;
}

void output::set_script(const chain::script& value)
{
    script_ = value;
}

void output::set_script(chain::script&& value)
{
    script_ = std::move(value);
}

output& output::operator=(output&& other)
{
    value_ = other.value_;
    script_ = std::move(other.script_);
    return *this;
}

output& output::operator=(const output& other)
{
    value_ = other.value_;
    script_ = other.script_;
    return *this;
}

bool output::operator==(const output& other) const
{
    return (value_ == other.value_) && (script_ == other.script_);
}

bool output::operator!=(const output& other) const
{
    return !(*this == other);
}

} // namespace chain
} // namespace libbitcoin
