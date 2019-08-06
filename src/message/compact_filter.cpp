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

// Sponsored in part by Digital Contract Design, LLC

#include <bitcoin/system/message/compact_filter.hpp>

#include <initializer_list>
#include <bitcoin/system/math/limits.hpp>
#include <bitcoin/system/message/messages.hpp>
#include <bitcoin/system/message/version.hpp>
#include <bitcoin/system/utility/container_sink.hpp>
#include <bitcoin/system/utility/container_source.hpp>
#include <bitcoin/system/utility/istream_reader.hpp>
#include <bitcoin/system/utility/ostream_writer.hpp>

namespace libbitcoin {
namespace system {
namespace message {

const std::string compact_filter::command = "cfilter";
const uint32_t compact_filter::version_minimum = version::level::bip157;
const uint32_t compact_filter::version_maximum = version::level::bip157;

compact_filter compact_filter::factory(uint32_t version, const data_chunk& data)
{
    compact_filter instance;
    instance.from_data(version, data);
    return instance;
}

compact_filter compact_filter::factory(uint32_t version, std::istream& stream)
{
    compact_filter instance;
    instance.from_data(version, stream);
    return instance;
}

compact_filter compact_filter::factory(uint32_t version, reader& source)
{
    compact_filter instance;
    instance.from_data(version, source);
    return instance;
}

compact_filter::compact_filter()
  : chain::compact_filter()
{
}

compact_filter::compact_filter(uint8_t filter_type,
    const hash_digest& block_hash, const data_chunk& filter)
  : chain::compact_filter(filter_type, block_hash, filter)
{
}

compact_filter::compact_filter(uint8_t filter_type, hash_digest&& block_hash,
    data_chunk&& filter)
  : chain::compact_filter(filter_type , std::move(block_hash),
      std::move(filter))
{
}

compact_filter::compact_filter(const compact_filter& other)
  : chain::compact_filter(other)
{
}

compact_filter::compact_filter(const chain::compact_filter& other)
  : chain::compact_filter(other)
{
}

compact_filter::compact_filter(compact_filter&& other)
  : chain::compact_filter(std::move(other))
{
}

compact_filter::compact_filter(chain::compact_filter&& other)
  : chain::compact_filter(std::move(other))
{
}

bool compact_filter::from_data(uint32_t version, const data_chunk& data)
{
    data_source istream(data);
    return from_data(version, istream);
}

bool compact_filter::from_data(uint32_t version, std::istream& stream)
{
    istream_reader source(stream);
    return from_data(version, source);
}

bool compact_filter::from_data(uint32_t version, reader& source)
{
    if (version < compact_filter::version_minimum)
        source.invalidate();

    return chain::compact_filter::from_data(source);
}

data_chunk compact_filter::to_data(uint32_t version) const
{
    data_chunk data;
    const auto size = serialized_size(version);
    data.reserve(size);
    data_sink ostream(data);
    to_data(version, ostream);
    ostream.flush();
    BITCOIN_ASSERT(data.size() == size);
    return data;
}

void compact_filter::to_data(uint32_t version, std::ostream& stream) const
{
    ostream_writer sink(stream);
    to_data(version, sink);
}

void compact_filter::to_data(uint32_t , writer& sink) const
{
    chain::compact_filter::to_data(sink);
}

size_t compact_filter::serialized_size(uint32_t ) const
{
    return chain::compact_filter::serialized_size();
}

compact_filter& compact_filter::operator=(chain::compact_filter&& other)
{
    chain::compact_filter::operator=(std::move(other));
    return *this;
}

compact_filter& compact_filter::operator=(compact_filter&& other)
{
    chain::compact_filter::operator=(std::move(other));
    return *this;
}

bool compact_filter::operator==(const chain::compact_filter& other) const
{
    return chain::compact_filter::operator==(other);
}

bool compact_filter::operator!=(const chain::compact_filter& other) const
{
    return !(*this == other);
}

bool compact_filter::operator==(const compact_filter& other) const
{
    return chain::compact_filter::operator==(other);
}

bool compact_filter::operator!=(const compact_filter& other) const
{
    return !(*this == other);
}

} // namespace message
} // namespace system
} // namespace libbitcoin
