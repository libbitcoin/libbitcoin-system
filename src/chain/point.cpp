/**
 * Copyright (c) 2011-2017 libbitcoin developers (see AUTHORS)
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
#include <bitcoin/bitcoin/chain/point.hpp>

#include <cstdint>
#include <sstream>
#include <utility>
#include <bitcoin/bitcoin/constants.hpp>
#include <bitcoin/bitcoin/formats/base_16.hpp>
#include <bitcoin/bitcoin/utility/container_sink.hpp>
#include <bitcoin/bitcoin/utility/container_source.hpp>
#include <bitcoin/bitcoin/utility/istream_reader.hpp>
#include <bitcoin/bitcoin/utility/ostream_writer.hpp>
#include <bitcoin/bitcoin/utility/serializer.hpp>

namespace libbitcoin {
namespace chain {

// This sentinel is serialized and defined by consensus, not implementation.
const uint32_t point::null_index = no_previous_output;

point point::factory_from_data(const data_chunk& data)
{
    point instance;
    instance.from_data(data);
    return instance;
}

point point::factory_from_data(std::istream& stream)
{
    point instance;
    instance.from_data(stream);
    return instance;
}

point point::factory_from_data(reader& source)
{
    point instance;
    instance.from_data(source);
    return instance;
}

// A default instance is invalid (until modified).
point::point()
  : hash_(null_hash), index_(0), valid_(false)
{
}

point::point(const hash_digest& hash, uint32_t index)
  : hash_(hash), index_(index), valid_(true)
{
}

point::point(hash_digest&& hash, uint32_t index)
  : hash_(std::move(hash)), index_(index), valid_(true)
{
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

point::point(const point& other)
  : point(other.hash_, other.index_, other.valid_)
{
}

point::point(point&& other)
  : point(std::move(other.hash_), other.index_, other.valid_)
{
}

bool point::is_valid() const
{
    return valid_ || (hash_ != null_hash) || (index_ != 0);
}

void point::reset()
{
    valid_ = false;
    hash_ = null_hash;
    index_ = 0;
}

bool point::from_data(const data_chunk& data)
{
    data_source istream(data);
    return from_data(istream);
}

bool point::from_data(std::istream& stream)
{
    istream_reader source(stream);
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

data_chunk point::to_data() const
{
    data_chunk data;
    data.reserve(serialized_size());
    data_sink ostream(data);
    to_data(ostream);
    ostream.flush();
    BITCOIN_ASSERT(data.size() == serialized_size());
    return data;
}

void point::to_data(std::ostream& stream) const
{
    ostream_writer sink(stream);
    to_data(sink);
}

void point::to_data(writer& sink) const
{
    sink.write_hash(hash_);
    sink.write_4_bytes_little_endian(index_);
}

size_t point::serialized_size() const
{
    return point::satoshi_fixed_size();
}

size_t point::satoshi_fixed_size()
{
    return hash_size + sizeof(uint32_t);
}

point_iterator point::begin() const
{
    return point_iterator(*this);
}

point_iterator point::end() const
{
    return point_iterator(*this, static_cast<unsigned>(satoshi_fixed_size()));
}

bool point::is_null() const
{
    return (index_ == null_index) && (hash_ == null_hash);
}

// This is used with output_point identification within a set of history rows
// of the same address. Collision will result in miscorrelation of points by
// client callers. This is stored in database. This is NOT a bitcoin checksum.
uint64_t point::checksum() const
{
    // Use an offset to the middle of the hash to avoid coincidental mining
    // of values into the front or back of tx hash (not a security feature).
    // Use most possible bits of tx hash to make intentional collision hard.
    return from_little_endian_unsafe<uint64_t>(hash_.begin() + 12) |
        static_cast<uint64_t>(index_);
}

hash_digest& point::hash()
{
    return hash_;
}

const hash_digest& point::hash() const
{
    return hash_;
}

void point::set_hash(const hash_digest& value)
{
    // This is no longer a default instance, so valid.
    valid_ = true;
    hash_ = value;
}

void point::set_hash(hash_digest&& value)
{
    // This is no longer a default instance, so valid.
    valid_ = true;
    hash_ = std::move(value);
}

uint32_t point::index() const
{
    return index_;
}

void point::set_index(uint32_t value)
{
    // This is no longer a default instance, so valid.
    valid_ = true;
    index_ = value;
}

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

bool point::operator==(const point& other) const
{
    return (hash_ == other.hash_) && (index_ == other.index_);
}

bool point::operator!=(const point& other) const
{
    return !(*this == other);
}

} // namespace chain
} // namespace libbitcoin
