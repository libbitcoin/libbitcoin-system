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
#include <boost/iostreams/stream.hpp>
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

// Default construction creates an invalid point, but populating either the
// index or the hash will make the pont valid.
point::point()
  : point(null_hash, invalid_point)
{
}

point::point(const point& other)
  : point(other.hash, other.index)
{
}

point::point(const hash_digest& hash, uint32_t index)
  : hash(hash),
    index(index)
{
}

point::point(point&& other)
  : point(std::forward<hash_digest>(other.hash), other.index)
{
}

point::point(hash_digest&& hash, uint32_t index)
  : hash(std::forward<hash_digest>(hash)),
    index(index)
{
}

point& point::operator=(point&& other)
{
    hash = std::forward<hash_digest>(other.hash);
    index = other.index;
    return *this;
}

point& point::operator=(const point& other)
{
    hash = other.hash;
    index = other.index;
    return *this;
}

bool point::is_valid() const
{
    return index != point::invalid_point || hash != null_hash;
}

void point::reset()
{
    hash = null_hash;
    index = point::invalid_point;
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

    hash = source.read_hash();
    index = source.read_4_bytes_little_endian();
    const auto result = static_cast<bool>(source);

    if (!result)
        reset();

    return result;
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
    sink.write_hash(hash);
    sink.write_4_bytes_little_endian(index);
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
    value << "\thash = " << encode_hash(hash) << "\n\tindex = " << index;
    return value.str();
}

bool point::is_null() const
{
    return index == null_index && hash == null_hash;
}

// This is used with output_point identification within a set of history rows
// of the same address. Collision will result in miscorrelation of points by
// client callers. This is NOT a bitcoin checksum.
uint64_t point::checksum() const
{
    static constexpr uint64_t divisor = uint64_t{ 1 } << 63;
    static_assert(divisor == 9223372036854775808ull, "Wrong divisor value.");

    // Write index onto a copy of the outpoint hash.
    auto copy = hash;
    auto serial = make_serializer(copy.begin());
    serial.write_4_bytes_little_endian(index);
    const auto hash_value = from_little_endian_unsafe<uint64_t>(copy.begin());

    // x mod 2**n == x & (2**n - 1)
    return hash_value & (divisor - 1);

    // Above usually provides only 32 bits of entropy, so below is preferred.
    // But this is stored in the database. Change requires server API change.
    // return std::hash<point>()(*this);
}

bool point::operator==(const point& other) const
{
    return hash == other.hash && index == other.index;
}

bool point::operator!=(const point& other) const
{
    return !(*this == other);
}

} // namespace chain
} // namespace libbitcoin
