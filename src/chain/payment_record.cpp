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
#include <bitcoin/bitcoin/chain/payment_record.hpp>

#include <cstddef>
#include <cstdint>
#include <istream>
#include <utility>
#include <bitcoin/bitcoin/constants.hpp>
#include <bitcoin/bitcoin/chain/input_point.hpp>
#include <bitcoin/bitcoin/chain/output_point.hpp>
#include <bitcoin/bitcoin/chain/point.hpp>
#include <bitcoin/bitcoin/utility/data.hpp>
#include <bitcoin/bitcoin/utility/container_sink.hpp>
#include <bitcoin/bitcoin/utility/container_source.hpp>
#include <bitcoin/bitcoin/utility/istream_reader.hpp>
#include <bitcoin/bitcoin/utility/ostream_writer.hpp>

namespace libbitcoin {
namespace chain {

// [ height:4 ]
// [ kind:1 ]
// [ data:8 ]
// [ point:[hash:32][index:2] ]

// Constructors.
//-----------------------------------------------------------------------------

payment_record::payment_record()
  : height_(0), kind_(point_kind::invalid), point_{}, data_(0)
{
}

payment_record::payment_record(chain::payment_record&& other)
  : height_(static_cast<uint32_t>(other.height_)),
    kind_(other.kind_),
    point_(std::move(other.point_)),
    data_(other.data_)
{
}

payment_record::payment_record(const chain::payment_record& other)
  : height_(static_cast<uint32_t>(other.height_)),
    kind_(other.kind_),
    point_(other.point_),
    data_(other.data_)
{
}

payment_record::payment_record(size_t height, chain::input_point&& point,
    uint64_t checksum)
  : height_(static_cast<uint32_t>(height)),
    kind_(point_kind::input),
    point_(std::move(point)),
    data_(checksum)
    
{
    // This class is used internal to the store, parameters presumed valid.
    BITCOIN_ASSERT(height <= max_uint32);
}

payment_record::payment_record(size_t height, const chain::input_point& point,
    uint64_t checksum)
  : height_(static_cast<uint32_t>(height)),
    kind_(point_kind::input),
    point_(point),
    data_(checksum)
{
    // This class is used internal to the store, parameters presumed valid.
    BITCOIN_ASSERT(height <= max_uint32);
}

payment_record::payment_record(size_t height, chain::output_point&& point,
    uint64_t value)
  : height_(static_cast<uint32_t>(height)),
    kind_(point_kind::output),
    point_(std::move(point)),
    data_(value)
    
{
    // This class is used internal to the store, parameters presumed valid.
    BITCOIN_ASSERT(height <= max_uint32);
}

payment_record::payment_record(size_t height, const chain::output_point& point,
    uint64_t value)
  : height_(static_cast<uint32_t>(height)),
    kind_(point_kind::output),
    point_(point),
    data_(value)
{
    // This class is used internal to the store, parameters presumed valid.
    BITCOIN_ASSERT(height <= max_uint32);
}

// Operators.
//-----------------------------------------------------------------------------

payment_record& payment_record::operator=(payment_record&& other)
{
    height_ = other.height_;
    kind_ = other.kind_;
    point_ = std::move(other.point_);
    data_ = other.data_;
    return *this;
}

payment_record& payment_record::operator=(const payment_record& other)
{
    height_ = other.height_;
    kind_ = other.kind_;
    point_ = other.point_;
    data_ = other.data_;
    return *this;
}

bool payment_record::operator==(const payment_record& other) const
{
    return (height_ == other.height_)
        && (kind_ == other.kind_)
        && (point_ == other.point_)
        && (data_ == other.data_);
}

bool payment_record::operator!=(const payment_record& other) const
{
    return !(*this == other);
}

// Deserialization.
//-----------------------------------------------------------------------------

payment_record payment_record::factory(const data_chunk& data, bool wire)
{
    payment_record instance;
    instance.from_data(data, wire);
    return instance;
}

payment_record payment_record::factory(std::istream& stream, bool wire)
{
    payment_record instance;
    instance.from_data(stream, wire);
    return instance;
}

payment_record payment_record::factory(reader& source, bool wire)
{
    payment_record instance;
    instance.from_data(source, wire);
    return instance;
}

bool payment_record::from_data(const data_chunk& data, bool wire)
{
    data_source istream(data);
    return from_data(istream, wire);
}

bool payment_record::from_data(std::istream& stream, bool wire)
{
    istream_reader source(stream);
    return from_data(source, wire);
}

// private/static
payment_record::point_kind payment_record::to_kind(uint8_t value)
{
    switch (value)
    {
        case 0:
            return point_kind::output;
        case 1:
            return point_kind::input;
        default:
            return point_kind::invalid;
    }
}

bool payment_record::from_data(reader& source, bool wire)
{
    if (!wire)
        return from_data(source, size_t(0));

    kind_ = to_kind(source.read_byte());
    point_.from_data(source, wire);
    height_ = source.read_4_bytes_little_endian();
    data_ = source.read_8_bytes_little_endian();

    if (kind_ == point_kind::invalid)
        source.invalidate();

    if (!source)
        reset();

    return source;
}

// Optimize reads by short-circuiting what is unnecessary.
// Invalid returns are conflated with skipped, but are store only.
bool payment_record::from_data(reader& source, size_t start_height)
{
    height_ = source.read_4_bytes_little_endian();

    if (height_ < start_height)
    {
        reset();
        source.skip(serialized_size() - sizeof(uint32_t));
        return false;
    }

    kind_ = to_kind(source.read_byte());
    point_.from_data(source, false);
    data_ = source.read_8_bytes_little_endian();

    if (kind_ == point_kind::invalid)
        source.invalidate();

    if (!source)
        reset();

    return source;
}

// protected
void payment_record::reset()
{
    height_ = 0;
    kind_ = point_kind::invalid;
    point_.reset();
    data_ = 0;
}

bool payment_record::is_valid() const
{
    return kind_ != point_kind::invalid && point_.is_valid();
}

// Serialization.
//-----------------------------------------------------------------------------

data_chunk payment_record::to_data(bool wire) const
{
    data_chunk data;
    const auto size = serialized_size(wire);
    data.reserve(size);
    data_sink ostream(data);
    to_data(ostream, wire);
    ostream.flush();
    BITCOIN_ASSERT(data.size() == size);
    return data;
}

void payment_record::to_data(std::ostream& stream, bool wire) const
{
    ostream_writer sink(stream);
    to_data(sink, wire);
}

void payment_record::to_data(writer& sink, bool wire) const
{
    if (wire)
    {
        // Client-server v3 protocol preserved.
        sink.write_byte(static_cast<uint8_t>(kind_));
        point_.to_data(sink, wire);
        sink.write_4_bytes_little_endian(height_);
        sink.write_8_bytes_little_endian(data_);
    }
    else
    {
        // Store serialization optimized in v4.
        sink.write_4_bytes_little_endian(height_);
        sink.write_byte(static_cast<uint8_t>(kind_));
        point_.to_data(sink, wire);
        sink.write_8_bytes_little_endian(data_);
    }
}

// Properties (size, accessors).
//-----------------------------------------------------------------------------

size_t payment_record::satoshi_fixed_size(bool wire)
{
    return sizeof(uint8_t) + chain::point::satoshi_fixed_size(wire) + 
        sizeof(uint32_t) + sizeof(uint64_t);
}

size_t payment_record::serialized_size(bool wire) const
{
    return satoshi_fixed_size(wire);
}

size_t payment_record::height() const
{
    return height_;
}

bool payment_record::is_output() const
{
    return kind_ == point_kind::output;
}

bool payment_record::is_input() const
{
    return kind_ == point_kind::input;
}

const chain::point& payment_record::point() const
{
    return point_;
}

uint64_t payment_record::data() const
{
    return data_;
}

} // namespace chain
} // namespace libbitcoin
