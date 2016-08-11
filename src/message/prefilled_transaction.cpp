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
#include <bitcoin/bitcoin/message/prefilled_transaction.hpp>

#include <boost/iostreams/stream.hpp>
#include <bitcoin/bitcoin/message/version.hpp>
#include <bitcoin/bitcoin/utility/container_sink.hpp>
#include <bitcoin/bitcoin/utility/container_source.hpp>
#include <bitcoin/bitcoin/utility/istream_reader.hpp>
#include <bitcoin/bitcoin/utility/ostream_writer.hpp>

namespace libbitcoin {
namespace message {

prefilled_transaction prefilled_transaction::factory_from_data(
    const uint32_t version, const data_chunk& data)
{
    prefilled_transaction instance;
    instance.from_data(version, data);
    return instance;
}

prefilled_transaction prefilled_transaction::factory_from_data(
    const uint32_t version, std::istream& stream)
{
    prefilled_transaction instance;
    instance.from_data(version, stream);
    return instance;
}

prefilled_transaction prefilled_transaction::factory_from_data(
    const uint32_t version, reader& source)
{
    prefilled_transaction instance;
    instance.from_data(version, source);
    return instance;
}

bool prefilled_transaction::is_valid() const
{
    return (index < max_uint16) && transaction.is_valid();
}

void prefilled_transaction::reset()
{
    index = 0;
    transaction.reset();
}

bool prefilled_transaction::from_data(uint32_t version,
    const data_chunk& data)
{
    data_source istream(data);
    return from_data(version, istream);
}

bool prefilled_transaction::from_data(uint32_t version,
    std::istream& stream)
{
    istream_reader source(stream);
    return from_data(version, source);
}

bool prefilled_transaction::from_data(uint32_t version,
    reader& source)
{
    reset();

    index = source.read_variable_uint_little_endian();
    auto result = static_cast<bool>(source);

    if (result)
        result = transaction.from_data(source);

    if (!result)
        reset();

    return result;
}

data_chunk prefilled_transaction::to_data(uint32_t version) const
{
    data_chunk data;
    data_sink ostream(data);
    to_data(version, ostream);
    ostream.flush();
    BITCOIN_ASSERT(data.size() == serialized_size(version));
    return data;
}

void prefilled_transaction::to_data(uint32_t version,
    std::ostream& stream) const
{
    ostream_writer sink(stream);
    to_data(version, sink);
}

void prefilled_transaction::to_data(uint32_t version,
    writer& sink) const
{
    sink.write_variable_uint_little_endian(index);
    transaction.to_data(sink);
}

uint64_t prefilled_transaction::serialized_size(uint32_t version) const
{
    return variable_uint_size(index) + transaction.serialized_size();
}

} // namspace message
} // namspace libbitcoin
