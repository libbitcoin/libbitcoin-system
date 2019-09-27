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

#include <bitcoin/system/chain/block_filter.hpp>

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
namespace chain {

// HACK: unlinked must match tx slab_map::not_found.
const uint64_t block_filter::validation::unlinked = max_int64;

block_filter block_filter::factory(const data_chunk& data, bool roundtrip)
{
    block_filter instance;
    instance.from_data(data, roundtrip);
    return instance;
}

block_filter block_filter::factory(std::istream& stream, bool roundtrip)
{
    block_filter instance;
    instance.from_data(stream, roundtrip);
    return instance;
}

block_filter block_filter::factory(reader& source, bool roundtrip)
{
    block_filter instance;
    instance.from_data(source, roundtrip);
    return instance;
}

block_filter::block_filter()
  : filter_type_(0u), header_(null_hash), filter_(),
    metadata{}
{
}

block_filter::block_filter(uint8_t filter_type, const hash_digest& header,
    const data_chunk& filter)
  : filter_type_(filter_type),
    header_(header),
    filter_(filter),
    metadata{}
{
}

block_filter::block_filter(uint8_t filter_type, hash_digest&& header,
    data_chunk&& filter)
  : filter_type_(filter_type),
    header_(std::move(header)),
    filter_(std::move(filter)),
    metadata{}
{
}

block_filter::block_filter(const block_filter& other)
  : filter_type_(other.filter_type_),
    header_(other.header_),
    filter_(other.filter_),
    metadata(other.metadata)
{
}

block_filter::block_filter(block_filter&& other)
  : filter_type_(other.filter_type_),
    header_(std::move(other.header_)),
    filter_(std::move(other.filter_)),
    metadata(std::move(other.metadata))
{
}

bool block_filter::is_valid() const
{
    return !filter_.empty();
}

void block_filter::reset()
{
    filter_type_ = 0u;
    header_.fill(0);
    filter_.clear();
    filter_.shrink_to_fit();
}

bool block_filter::from_data(const data_chunk& data, bool roundtrip)
{
    data_source istream(data);
    return from_data(istream, roundtrip);
}

bool block_filter::from_data(std::istream& stream, bool roundtrip)
{
    istream_reader source(stream);
    return from_data(source, roundtrip);
}

bool block_filter::from_data(reader& source, bool roundtrip)
{
    reset();

    if (roundtrip)
    {
        filter_type_ = source.read_byte();
        header_ = source.read_hash();
    }

    const auto count = source.read_size_little_endian();

    // Guard against potential for arbitrary memory allocation.
    if (count > max_block_size)
        source.invalidate();
    else
        filter_ = source.read_bytes(count);

    if (!source)
        reset();

    return source;
}

data_chunk block_filter::to_data(bool roundtrip) const
{
    data_chunk data;
    const auto size = serialized_size(roundtrip);
    data.reserve(size);
    data_sink ostream(data);
    to_data(ostream, roundtrip);
    ostream.flush();
    BITCOIN_ASSERT(data.size() == size);
    return data;
}

void block_filter::to_data(std::ostream& stream, bool roundtrip) const
{
    ostream_writer sink(stream);
    to_data(sink, roundtrip);
}

void block_filter::to_data(writer& sink, bool roundtrip) const
{
    if (roundtrip)
    {
        sink.write_byte(filter_type_);
        sink.write_hash(header_);
    }

    sink.write_size_little_endian(filter_.size());
    sink.write_bytes(filter_);
}

size_t block_filter::serialized_size(bool roundtrip) const
{
    size_t result = hash_size +
        message::variable_uint_size(filter_.size()) + filter_.size();

    if (roundtrip)
        result += sizeof(filter_type_);

    return result;
}

bool block_filter::from_data(reader& source, uint8_t filter_type)
{
    if (!from_data(source, false))
        return false;

    filter_type_ = filter_type;
    return true;
}

uint8_t block_filter::filter_type() const
{
    return filter_type_;
}

void block_filter::set_filter_type(uint8_t value)
{
    filter_type_ = value;
}

hash_digest& block_filter::header()
{
    return header_;
}

const hash_digest& block_filter::header() const
{
    return header_;
}

void block_filter::set_header(const hash_digest& value)
{
    header_ = value;
}

void block_filter::set_header(hash_digest&& value)
{
    header_ = std::move(value);
}

data_chunk& block_filter::filter()
{
    return filter_;
}

const data_chunk& block_filter::filter() const
{
    return filter_;
}

void block_filter::set_filter(const data_chunk& value)
{
    filter_ = value;
}

void block_filter::set_filter(data_chunk&& value)
{
    filter_ = std::move(value);
}

block_filter& block_filter::operator=(block_filter&& other)
{
    filter_type_ = other.filter_type_;
    header_ = std::move(other.header_);
    filter_ = std::move(other.filter_);
    metadata = std::move(other.metadata);
    return *this;
}

bool block_filter::operator==(const block_filter& other) const
{
    return (filter_type_ == other.filter_type_)
        && (header_ == other.header_)
        && (filter_ == other.filter_);
}

bool block_filter::operator!=(const block_filter& other) const
{
    return !(*this == other);
}

} // namespace chain
} // namespace system
} // namespace libbitcoin
