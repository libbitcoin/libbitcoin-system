/**
 * Copyright (c) 2011-2025 libbitcoin developers (see AUTHORS)
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
#include <bitcoin/system/chain/outpoint.hpp>

#include <utility>
#include <bitcoin/system/chain/point.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/stream/stream.hpp>

namespace libbitcoin {
namespace system {
namespace chain {

// Constructors.
// ----------------------------------------------------------------------------

// Invalid default used in signature hashing.
outpoint::outpoint() NOEXCEPT
  : outpoint({}, zero, false)
{
}

outpoint::outpoint(chain::point&& point, uint64_t value) NOEXCEPT
  : outpoint(std::move(point), value, true)
{
}

outpoint::outpoint(const chain::point& point, uint64_t value) NOEXCEPT
  : outpoint(point, value, true)
{
}

outpoint::outpoint(const data_slice& data) NOEXCEPT
  : outpoint(stream::in::fast(data))
{
}

// protected
outpoint::outpoint(stream::in::fast&& stream) NOEXCEPT
  : outpoint(read::bytes::fast(stream))
{
}

outpoint::outpoint(stream::in::fast& stream) NOEXCEPT
  : outpoint(read::bytes::fast(stream))
{
}
outpoint::outpoint(std::istream& stream) NOEXCEPT
  : outpoint(read::bytes::istream(stream))
{
}

// protected
outpoint::outpoint(reader&& source) NOEXCEPT
  : outpoint(source)
{
}

// outpoint is not part of block, so doesn't use custom allocator.
outpoint::outpoint(reader& source) NOEXCEPT
  : point_{ source },
    value_{ source.read_8_bytes_little_endian() },
    valid_{ source }
{
}

// protected
outpoint::outpoint(chain::point&& point, uint64_t value, bool valid) NOEXCEPT
  : point_(std::move(point)), value_(value), valid_(valid)
{
}

// protected
outpoint::outpoint(const chain::point& point, uint64_t value,
    bool valid) NOEXCEPT
  : point_(point), value_(value), valid_(valid)
{
}

// Operators.
// ----------------------------------------------------------------------------
// Value does not contribute to identity.

bool outpoint::operator==(const outpoint& other) const NOEXCEPT
{
    return point() == other.point();
}

bool outpoint::operator!=(const outpoint& other) const NOEXCEPT
{
    return !(*this == other);
}

bool operator<(const outpoint& left, const outpoint& right) NOEXCEPT
{
    return left.point() < right.point();
}

// Serialization.
// ----------------------------------------------------------------------------

data_chunk outpoint::to_data() const NOEXCEPT
{
    data_chunk data(serialized_size());
    stream::out::fast ostream(data);
    write::bytes::fast out(ostream);
    to_data(out);
    return data;
}

void outpoint::to_data(std::ostream& stream) const NOEXCEPT
{
    write::bytes::ostream out(stream);
    to_data(out);
}

void outpoint::to_data(writer& sink) const NOEXCEPT
{
    point_.to_data(sink);
    sink.write_8_bytes_little_endian(value_);
}

// Properties.
// ----------------------------------------------------------------------------

bool outpoint::is_valid() const NOEXCEPT
{
    return valid_;
}

const chain::point& outpoint::point() const NOEXCEPT
{
    return point_;
}

uint64_t outpoint::value() const NOEXCEPT
{
    return value_;
}

// Validation.
// ----------------------------------------------------------------------------

bool outpoint::is_null() const NOEXCEPT
{
    return point_.is_null();
}

// JSON value convertors.
// ----------------------------------------------------------------------------

DEFINE_JSON_TO_TAG(outpoint)
{
    return
    {
        value_to<point>(value.at("point")),
        value.at("value").to_number<uint64_t>()
    };
}

DEFINE_JSON_FROM_TAG(outpoint)
{
    value =
    {
        { "point", value_from(instance.point()) },
        { "value", instance.value() }
    };
}

DEFINE_JSON_TO_TAG(outpoint::cptr)
{
    return to_shared(tag_invoke(to_tag<outpoint>{}, value));
}

DEFINE_JSON_FROM_TAG(outpoint::cptr)
{
    tag_invoke(from_tag{}, value, *instance);
}

} // namespace chain
} // namespace system
} // namespace libbitcoin
