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
#include <bitcoin/system/chain/point.hpp>

#include <cstdint>
#include <utility>
#include <bitcoin/system/assert.hpp>
#include <bitcoin/system/chain/enums/magic_numbers.hpp>
#include <bitcoin/system/constants.hpp>
#include <bitcoin/system/stream/stream.hpp>

namespace libbitcoin {
namespace system {
namespace chain {

// This sentinel is serialized and defined by consensus, not implementation.
const uint32_t point::null_index = no_previous_output;

// Constructors.
//-----------------------------------------------------------------------------

// A default instance is invalid (until modified).
point::point()
  : hash_(null_hash),
    index_(0),
    valid_(false)
{
}

point::point(const hash_digest& hash, uint32_t index)
  : hash_(hash),
    index_(index),
    valid_(true)
{
}

point::point(hash_digest&& hash, uint32_t index)
  : hash_(std::move(hash)),
    index_(index),
    valid_(true)
{
}

// protected
point::point(const hash_digest& hash, uint32_t index, bool valid)
  : hash_(hash),
    index_(index),
    valid_(valid)
{
}

// protected
point::point(hash_digest&& hash, uint32_t index, bool valid)
  : hash_(std::move(hash)),
    index_(index),
    valid_(valid)
{
}

point::point(const point& other)
  : point(other.hash_, other.index_, other.valid_)
{
}

point::point(point&& other)
  : point(std::move(other.hash_), other.index_, other.valid_)
{
}

// Operators.
//-----------------------------------------------------------------------------

point& point::operator=(point&& other)
{
    hash_ = std::move(other.hash_);
    index_ = other.index_;
    return *this;
}

point& point::operator=(const point& other)
{
    hash_ = other.hash_;
    index_ = other.index_;
    return *this;
}

// This arbitrary order is produced to support set uniqueness determinations.
bool point::operator<(const point& other) const
{
    // The index is primary only because its comparisons are simpler.
    return index_ == other.index_ ? hash_ < other.hash_ :
        index_ < other.index_;
}

bool point::operator==(const point& other) const
{
    return (hash_ == other.hash_) && (index_ == other.index_);
}

bool point::operator!=(const point& other) const
{
    return !(*this == other);
}

// Deserialization.
//-----------------------------------------------------------------------------

// static
point point::factory(const data_chunk& data)
{
    point instance;
    instance.from_data(data);
    return instance;
}

// static
point point::factory(std::istream& stream)
{
    point instance;
    instance.from_data(stream);
    return instance;
}

// static
point point::factory(reader& source)
{
    point instance;
    instance.from_data(source);
    return instance;
}

bool point::from_data(const data_chunk& data)
{
    stream::in::copy istream(data);
    return from_data(istream);
}

bool point::from_data(std::istream& stream)
{
    read::bytes::istream source(stream);
    return from_data(source);
}

bool point::from_data(reader& source)
{
    reset();

    valid_ = true;
    hash_ = source.read_hash();
    index_ = source.read_4_bytes_little_endian();

    if (!source)
        reset();

    return source;
}

// protected
void point::reset()
{
    valid_ = false;
    hash_ = null_hash;
    index_ = 0;
}

bool point::is_valid() const
{
    return valid_;
}

// Serialization.
//-----------------------------------------------------------------------------

data_chunk point::to_data() const
{
    data_chunk data(no_fill_byte_allocator);
    data.resize(serialized_size());
    stream::out::copy ostream(data);
    to_data(ostream);
    return data;
}

void point::to_data(std::ostream& stream) const
{
    write::bytes::ostream out(stream);
    to_data(out);
}

void point::to_data(writer& sink) const
{
    DEBUG_ONLY(const auto size = serialized_size();)
    DEBUG_ONLY(const auto start = sink.get_position();)

    sink.write_bytes(hash_);
    sink.write_4_bytes_little_endian(index_);

    BITCOIN_ASSERT(sink.get_position() - start == size);
}

// Properties.
//-----------------------------------------------------------------------------

size_t point::satoshi_fixed_size()
{
    return hash_size + sizeof(uint32_t);
}

size_t point::serialized_size() const
{
    return satoshi_fixed_size();
}

const hash_digest& point::hash() const
{
    return hash_;
}

uint32_t point::index() const
{
    return index_;
}

// Validation.
//-----------------------------------------------------------------------------

bool point::is_null() const
{
    return (index_ == null_index) && (hash_ == null_hash);
}

} // namespace chain
} // namespace system
} // namespace libbitcoin
