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
// ----------------------------------------------------------------------------

// Valid default used in signature hashing.
point::point()
  : point(null_hash, point::null_index, true)
{
}

point::point(point&& other)
  : point(std::move(other.hash_), other.index_, other.valid_)
{
}

point::point(const point& other)
  : point(other.hash_, other.index_, other.valid_)
{
}

point::point(hash_digest&& hash, uint32_t index)
  : point(std::move(hash), index, true)
{
}

point::point(const hash_digest& hash, uint32_t index)
  : point(hash, index, true)
{
}

point::point(const data_slice& data)
  : point(stream::in::copy(data))
{
}

point::point(std::istream& stream)
  : point(read::bytes::istream(stream))
{
}

point::point(reader& source)
  : point()
{
    // Above default construct presumed cheaper than factory populated move.
    from_data(source);
}

// protected
point::point(const hash_digest& hash, uint32_t index, bool valid)
  : hash_(hash), index_(index), valid_(valid)
{
}

// protected
point::point(hash_digest&& hash, uint32_t index, bool valid)
  : hash_(std::move(hash)), index_(index), valid_(valid)
{
}

// Operators.
// ----------------------------------------------------------------------------

point& point::operator=(point&& other)
{
    hash_ = std::move(other.hash_);
    index_ = other.index_;
    valid_ = other.valid_;
    return *this;
}

point& point::operator=(const point& other)
{
    hash_ = other.hash_;
    index_ = other.index_;
    valid_ = other.valid_;
    return *this;
}

bool point::operator==(const point& other) const
{
    return (hash_ == other.hash_)
        && (index_ == other.index_);
}

bool point::operator!=(const point& other) const
{
    return !(*this == other);
}

bool operator<(const point& left, const point& right)
{
    // Arbitrary compare, for uniqueness sorting.
    return left.index() == right.index() ?
        left.hash() < right.hash() : left.index() < right.index();
}

// Deserialization.
// ----------------------------------------------------------------------------

bool point::from_data(const data_slice& data)
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
    ////reset();

    hash_ = source.read_hash();
    index_ = source.read_4_bytes_little_endian();

    if (!source)
        reset();

    valid_ = source;
    return valid_;
}

// protected
void point::reset()
{
    hash_.fill(0);
    index_ = 0;
    valid_ = false;
}

bool point::is_valid() const
{
    return valid_;
}

// Serialization.
// ----------------------------------------------------------------------------

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
    DEBUG_ONLY(const auto bytes = serialized_size();)
    DEBUG_ONLY(const auto start = sink.get_position();)

    sink.write_bytes(hash_);
    sink.write_4_bytes_little_endian(index_);

    BITCOIN_ASSERT(sink && sink.get_position() - start == bytes);
}

// Properties.
// ----------------------------------------------------------------------------

const hash_digest& point::hash() const
{
    return hash_;
}

uint32_t point::index() const
{
    return index_;
}

size_t point::serialized_size()
{
    return hash_size + sizeof(uint32_t);
}

// Validation.
// ----------------------------------------------------------------------------

bool point::is_null() const
{
    return (index_ == null_index) && (hash_ == null_hash);
}

} // namespace chain
} // namespace system
} // namespace libbitcoin
