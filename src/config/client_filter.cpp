/**
 * Copyright (c) 2011-2019 libbitcoin developers (see AUTHORS)
 *
 * This file is part of libbitcoin.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifdef MOVE_TO_NETWORK

// Sponsored in part by Digital Contract Design, LLC

#include <bitcoin/system/config/client_filter.hpp>

#include <sstream>
#include <string>
#include <bitcoin/system/config/base16.hpp>
#include <bitcoin/system/exceptions.hpp>
#include <bitcoin/system/stream/stream.hpp>

// TODO: replace use of messages::client_filter, moving to network.
// TODO: maybe just move this to libbitcoin-explorer.

namespace libbitcoin {
namespace system {
namespace config {

client_filter::client_filter()
  : value_()
{
}

client_filter::client_filter(const std::string& hexcode)
  : value_()
{
    std::stringstream(hexcode) >> *this;
}

client_filter::client_filter(const messages::client_filter& value)
  : value_(value)
{
}

client_filter::client_filter(const client_filter& other)
  : client_filter(other.value_)
{
}

client_filter& client_filter::operator=(const client_filter& other)
{
    value_ = messages::client_filter(other.value_);
    return *this;
}

client_filter& client_filter::operator=(messages::client_filter&& other)
{
    value_ = std::move(other);
    return *this;
}

bool client_filter::operator==(const client_filter& other) const
{
    // No equality operator on message types.
    return value_.id == other.value_.id
        && value_.filter_type == other.value_.filter_type
        && value_.filter == other.value_.filter;
}

client_filter::operator const messages::client_filter&() const
{
    return value_;
}

std::string client_filter::to_string() const
{
    std::stringstream value;
    value << *this;
    return value.str();
}

std::istream& operator>>(std::istream& input, client_filter& argument)
{
    using namespace messages;
    constexpr auto version = version::maximum;

    std::string hexcode;
    input >> hexcode;

    // No data deserializer on message types.
    data_chunk data;
    if (!decode_base16(data, hexcode))
        throw istream_exception(hexcode);

    read::bytes::copy source(data);
    argument.value_ = messages::client_filter::deserialize(version, source);

    if (!source)
        throw istream_exception(hexcode);

    return input;
}

std::ostream& operator<<(std::ostream& output, const client_filter& argument)
{
    using namespace messages;
    constexpr auto version = version::maximum;

    // No data serializer on message types.
    data_chunk data(argument.value_.size(version));
    write::bytes::copy sink(data);

    argument.value_.serialize(version, sink);

    output << base16(data);
    return output;
}

} // namespace config
} // namespace system
} // namespace libbitcoin

#endif // MOVE_TO_NETWORK