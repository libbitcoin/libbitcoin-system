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
  : filter_type_(0u), block_hash_(null_hash), filter_()
{
}

compact_filter::compact_filter(uint8_t filter_type,
    const hash_digest& block_hash, const data_chunk& filter)
  : filter_type_(filter_type),
    block_hash_(block_hash),
    filter_(filter)
{
}

compact_filter::compact_filter(uint8_t filter_type, hash_digest&& block_hash,
    data_chunk&& filter)
  : filter_type_(filter_type),
    block_hash_(std::move(block_hash)),
    filter_(std::move(filter))
{
}

compact_filter::compact_filter(const compact_filter& other)
  : compact_filter(other.filter_type_, other.block_hash_, other.filter_)
{
}

compact_filter::compact_filter(compact_filter&& other)
  : compact_filter(other.filter_type_, std::move(other.block_hash_),
      std::move(other.filter_))
{
}

bool compact_filter::is_valid() const
{
    return !filter_.empty();
}

void compact_filter::reset()
{
    filter_type_ = 0u;
    block_hash_.fill(0);
    filter_.clear();
    filter_.shrink_to_fit();
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
    reset();

    filter_type_ = source.read_byte();
    block_hash_ = source.read_hash();

    auto count = source.read_size_little_endian();

    // Guard against potential for arbitrary memory allocation.
    if (count > max_block_size)
        source.invalidate();
    else
        filter_ = source.read_bytes(count);

    if (version < compact_filter::version_minimum)
        source.invalidate();

    if (!source)
        reset();

    return source;
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
    sink.write_byte(filter_type_);
    sink.write_hash(block_hash_);
    sink.write_variable_little_endian(filter_.size());
    sink.write_bytes(filter_);
}

size_t compact_filter::serialized_size(uint32_t ) const
{
    return sizeof(filter_type_) + hash_size +
        variable_uint_size(filter_.size()) + filter_.size();
}

uint8_t compact_filter::filter_type() const
{
    return filter_type_;
}

void compact_filter::set_filter_type(uint8_t value)
{
    filter_type_ = value;
}

hash_digest& compact_filter::block_hash()
{
    return block_hash_;
}

const hash_digest& compact_filter::block_hash() const
{
    return block_hash_;
}

void compact_filter::set_block_hash(const hash_digest& value)
{
    block_hash_ = value;
}

void compact_filter::set_block_hash(hash_digest&& value)
{
    block_hash_ = std::move(value);
}

data_chunk& compact_filter::filter()
{
    return filter_;
}

const data_chunk& compact_filter::filter() const
{
    return filter_;
}

void compact_filter::set_filter(const data_chunk& value)
{
    filter_ = value;
}

void compact_filter::set_filter(data_chunk&& value)
{
    filter_ = std::move(value);
}

compact_filter& compact_filter::operator=(compact_filter&& other)
{
    filter_type_ = other.filter_type_;
    block_hash_ = std::move(other.block_hash_);
    filter_ = std::move(other.filter_);
    return *this;
}

bool compact_filter::operator==(const compact_filter& other) const
{
    return (filter_type_ == other.filter_type_)
        && (block_hash_ == other.block_hash_)
        && (filter_ == other.filter_);
}

bool compact_filter::operator!=(const compact_filter& other) const
{
    return !(*this == other);
}

} // namespace message
} // namespace system
} // namespace libbitcoin
