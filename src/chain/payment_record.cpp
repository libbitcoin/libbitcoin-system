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

// HACK: must match tx slab_map::not_found.
static const uint64_t undetermined_link = max_uint64;

// Constructors.
//-----------------------------------------------------------------------------

payment_record::payment_record()
  : output_(false),
    height_(0),
    point_{},
    data_(0),
    link_(undetermined_link)
{
}

payment_record::payment_record(chain::payment_record&& other)
  : output_(other.output_),
    height_(other.height_),
    point_(std::move(other.point_)),
    data_(other.data_),
    link_(other.link_)
{
}

payment_record::payment_record(const chain::payment_record& other)
  : output_(other.output_),
    height_(other.height_),
    point_(other.point_),
    data_(other.data_),
    link_(other.link_)
{
}

payment_record::payment_record(uint64_t link, uint32_t index, uint64_t data,
    bool output)
  : output_(output),
    height_(0),
    point_{ null_hash, index },
    data_(data),
    link_(link)
    
{
}

// Operators.
//-----------------------------------------------------------------------------

payment_record& payment_record::operator=(payment_record&& other)
{
    output_ = other.output_;
    height_ = other.height_;
    point_ = std::move(other.point_);
    data_ = other.data_;
    link_ = other.link_;
    return *this;
}

payment_record& payment_record::operator=(const payment_record& other)
{
    output_ = other.output_;
    height_ = other.height_;
    point_ = other.point_;
    data_ = other.data_;
    link_ = other.link_;
    return *this;
}

bool payment_record::operator==(const payment_record& other) const
{
    return (output_ == other.output_)
        && (height_ == other.height_)
        && (point_ == other.point_)
        && (data_ == other.data_)
        && (link_ == other.link_);
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

bool payment_record::from_data(reader& source, bool wire)
{
    const auto output = source.read_byte();

    if (output != 1 && output != 0)
        source.invalidate();

    output_ = output == 1;
    point_.from_data(source, wire);

    if (wire)
        height_ = source.read_4_bytes_little_endian();
    else
        link_ = source.read_8_bytes_little_endian();

    data_ = source.read_8_bytes_little_endian();

    if (!source)
        reset();

    return source;
}

// protected
void payment_record::reset()
{
    output_ = false;
    height_ = 0;
    point_.reset();
    data_ = 0;
    link_ = undetermined_link;
}

bool payment_record::is_valid() const
{
    return point_.is_valid();
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

// Wire assumes height and point.hash population.
void payment_record::to_data(writer& sink, bool wire) const
{
    sink.write_byte(output_ ? 1 : 0);
    point_.to_data(sink, wire);

    if (wire)
        sink.write_4_bytes_little_endian(static_cast<uint32_t>(height_));
    else
        sink.write_8_bytes_little_endian(link_);

    sink.write_8_bytes_little_endian(data_);
}

// Properties (size, accessors).
//-----------------------------------------------------------------------------

// TODO: should be further generalized into point with link metadata.
// Non-wire point does not currently include the tx offset (hacked in).
// Non-wire payment_record doesn't include height (retrieved from tx store).
size_t payment_record::satoshi_fixed_size(bool wire)
{
    return
        sizeof(uint8_t) +
        chain::point::satoshi_fixed_size(wire) +
        (wire ? sizeof(uint32_t) : sizeof(uint64_t)) +
        sizeof(uint64_t);
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
    return output_;
}

const chain::point& payment_record::point() const
{
    return point_;
}

uint64_t payment_record::data() const
{
    return data_;
}

uint64_t payment_record::link() const
{
    return link_;
}

// Set after non-wire deserializaton (distinct store).
void payment_record::set_height(size_t height)
{
    BITCOIN_ASSERT_MSG(height <= max_size_t, "height overflow");
    height_ = height;
}

// Set after non-wire deserializaton (distinct store).
void payment_record::set_hash(hash_digest&& hash)
{
    point_.set_hash(std::move(hash));
}

} // namespace chain
} // namespace libbitcoin
