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
#include <bitcoin/bitcoin/message/fee_filter.hpp>

#include <boost/iostreams/stream.hpp>
#include <bitcoin/bitcoin/message/version.hpp>
#include <bitcoin/bitcoin/utility/container_sink.hpp>
#include <bitcoin/bitcoin/utility/container_source.hpp>
#include <bitcoin/bitcoin/utility/istream_reader.hpp>
#include <bitcoin/bitcoin/utility/ostream_writer.hpp>

namespace libbitcoin {
namespace message {

const std::string fee_filter::command = "feefilter";
const uint32_t fee_filter::version_minimum = version::level::bip133;
const uint32_t fee_filter::version_maximum = version::level::bip133;

fee_filter fee_filter::factory_from_data(uint32_t version, const data_chunk& data)
{
    fee_filter instance;
    instance.from_data(version, data);
    return instance;
}

fee_filter fee_filter::factory_from_data(uint32_t version, std::istream& stream)
{
    fee_filter instance;
    instance.from_data(version, stream);
    return instance;
}

fee_filter fee_filter::factory_from_data(uint32_t version, reader& source)
{
    fee_filter instance;
    instance.from_data(version, source);
    return instance;
}

uint64_t fee_filter::satoshi_fixed_size(uint32_t version)
{
    return sizeof(minimum_fee);
}

fee_filter::fee_filter()
  : minimum_fee(0), valid_(false)
{
}

fee_filter::fee_filter(uint64_t minimum)
  : minimum_fee(minimum), valid_(true)
{
}

bool fee_filter::from_data(uint32_t version, const data_chunk& data)
{
    data_source istream(data);
    return from_data(version, istream);
}

bool fee_filter::from_data(uint32_t version, std::istream& stream)
{
    istream_reader source(stream);
    return from_data(version, source);
}

bool fee_filter::from_data(uint32_t version, reader& source)
{
    reset();

    valid_ = !(version < fee_filter::version_minimum);
    minimum_fee = source.read_8_bytes_little_endian();
    valid_ &= static_cast<bool>(source);

    if (!valid_)
        reset();

    return valid_;
}

data_chunk fee_filter::to_data(uint32_t version) const
{
    data_chunk data;
    data_sink ostream(data);
    to_data(version, ostream);
    ostream.flush();
    BITCOIN_ASSERT(data.size() == serialized_size(version));
    return data;
}

void fee_filter::to_data(uint32_t version, std::ostream& stream) const
{
    ostream_writer sink(stream);
    to_data(version, sink);
}

void fee_filter::to_data(uint32_t version, writer& sink) const
{
    sink.write_8_bytes_little_endian(minimum_fee);
}

bool fee_filter::is_valid() const
{
    return valid_;
}

void fee_filter::reset()
{
    valid_ = false;
    minimum_fee = 0;
}

uint64_t fee_filter::serialized_size(uint32_t version) const
{
    return satoshi_fixed_size(version);
}

bool fee_filter::operator==(const fee_filter& other) const
{
    return (minimum_fee == other.minimum_fee);
}

bool fee_filter::operator!=(const fee_filter& other) const
{
    return !(*this == other);
}

} // namspace message
} // namspace libbitcoin
