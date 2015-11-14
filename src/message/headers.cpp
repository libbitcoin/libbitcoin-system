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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#include <bitcoin/bitcoin/message/headers.hpp>

#include <boost/iostreams/stream.hpp>
#include <bitcoin/bitcoin/utility/container_sink.hpp>
#include <bitcoin/bitcoin/utility/container_source.hpp>
#include <bitcoin/bitcoin/utility/istream_reader.hpp>
#include <bitcoin/bitcoin/utility/ostream_writer.hpp>

namespace libbitcoin {
namespace message {

const std::string message::headers::command = "headers";

headers headers::factory_from_data(const data_chunk& data)
{
    headers instance;
    instance.from_data(data);
    return instance;
}

headers headers::factory_from_data(std::istream& stream)
{
    headers instance;
    instance.from_data(stream);
    return instance;
}

headers headers::factory_from_data(reader& source)
{
    headers instance;
    instance.from_data(source);
    return instance;
}

headers::headers()
{
}

headers::headers(const std::initializer_list<chain::header> elements)
{
    this->elements.insert(this->elements.end(), elements.begin(),
        elements.end());
}

bool headers::is_valid() const
{
    return !elements.empty();
}

void headers::reset()
{
    elements.clear();
}

bool headers::from_data(const data_chunk& data)
{
    data_source istream(data);
    return from_data(istream);
}

bool headers::from_data(std::istream& stream)
{
    istream_reader source(stream);
    return from_data(source);
}

bool headers::from_data(reader& source)
{
    auto result = true;

    reset();

    uint64_t count = source.read_variable_uint_little_endian();
    result = source;

    for (uint64_t i = 0; (i < count) && result; ++i)
    {
        elements.emplace_back();
        result = elements.back().from_data(source, true);
    }

    if (!result)
        reset();

    return result;
}

data_chunk headers::to_data() const
{
    data_chunk data;
    data_sink ostream(data);
    to_data(ostream);
    ostream.flush();
    BITCOIN_ASSERT(data.size() == serialized_size());
    return data;
}

void headers::to_data(std::ostream& stream) const
{
    ostream_writer sink(stream);
    to_data(sink);
}

void headers::to_data(writer& sink) const
{
    sink.write_variable_uint_little_endian(elements.size());
    for (const auto& head : elements)
        head.to_data(sink, true);
}

uint64_t headers::serialized_size() const
{
    uint64_t size = variable_uint_size(elements.size());

    for (const auto& head : elements)
        size += head.serialized_size(true);

    return size;
}

bool operator==(const headers& left, const headers& right)
{
    return left.elements == right.elements;
}

bool operator!=(const headers& left, const headers& right)
{
    return !(left == right);
}

} // namspace message
} // namspace libbitcoin
