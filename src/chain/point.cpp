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

// This is a sentinel used internally to signal invalidity of the point.
///////////////////////////////////////////////////////////////////////////////
// Setting a flag only on deserialization failure is insufficient, since
// default construction yields success, which is not the intended result
// when returning a failure instance. Apart from restructuring interfaces
// the only alternative would be a method to explicitly set invalidity,
// so we've opted for this. This is not a consensus value and is currently
// outside of the domain of valid values due to the block size limit.
///////////////////////////////////////////////////////////////////////////////
const uint32_t point::invalid_point = null_index - 1u;

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

point::point()
  : hash_(null_hash), index_(invalid_point)
{
}

point::point(const hash_digest& hash, uint32_t index)
  : hash_(hash), index_(index)
{
}

point::point(hash_digest&& hash, uint32_t index)
  : hash_(std::move(hash)), index_(index)
{
}

point::point(const point& other)
  : point(other.hash_, other.index_)
{
}

point::point(point&& other)
  : point(std::move(other.hash_), other.index_)
{
}

bool point::is_valid() const
{
    return (index_ != point::invalid_point) || (hash_ != null_hash);
}

void point::reset()
{
    hash_ = null_hash;
    index_ = point::invalid_point;
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

    hash_ = source.read_hash();
    index_ = source.read_4_bytes_little_endian();

    if (!source)
        reset();

    return source;
}

data_chunk point::to_data() const
{
    data_chunk data;
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

uint64_t point::serialized_size() const
{
    return point::satoshi_fixed_size();
}

uint64_t point::satoshi_fixed_size()
{
    return hash_size + sizeof(uint32_t);
}

point_iterator point::begin() const
{
    return point_iterator(*this);
}

point_iterator point::end() const
{
    return point_iterator(*this, true);
}

std::string point::to_string() const
{
    std::ostringstream value;
    value << "\thash = " << encode_hash(hash_) << "\n\tindex = " << index_;
    return value.str();
}

bool point::is_null() const
{
    return (index_ == null_index) && (hash_ == null_hash);
}

// This is used with output_point identification within a set of history rows
// of the same address. Collision will result in miscorrelation of points by
// client callers. This is NOT a bitcoin checksum.
uint64_t point::checksum() const
{
    const auto hash64 = from_little_endian_unsafe<uint64_t>(hash_.begin());
    const auto index_hash = (hash64 & 0xffffffff00000000) | index_;

    // x mod 2**n == x & (2**n - 1)
    static constexpr uint64_t divisor = uint64_t{ 1 } << 63;
    static_assert(divisor == 9223372036854775808ull, "Wrong divisor value.");
    return index_hash & (divisor - 1);

    // Above usually provides only 32 bits of entropy, so below is preferred.
    // But this is stored in the database. Change requires server API change.
    // return std::hash<point>()(*this);
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
    hash_ = value;
}

void point::set_hash(hash_digest&& value)
{
    hash_ = std::move(value);
}

uint32_t point::index() const
{
    return index_;
}

void point::set_index(uint32_t value)
{
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
