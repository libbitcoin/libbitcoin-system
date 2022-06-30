/**
 * Copyright (c) 2011-2022 libbitcoin developers (see AUTHORS)
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
#include <memory>
#include <utility>
#include <bitcoin/system/boost.hpp>
#include <bitcoin/system/chain/enums/magic_numbers.hpp>
#include <bitcoin/system/constants.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/math/math.hpp>
#include <bitcoin/system/stream/stream.hpp>

namespace libbitcoin {
namespace system {
namespace chain {

// This sentinel is serialized and defined by consensus, not implementation.
const uint32_t point::null_index = no_previous_output;

// Constructors.
// ----------------------------------------------------------------------------

// Invalid default used in signature hashing.
point::point() NOEXCEPT
  : point(null_hash, point::null_index, false)
{
}

point::point(hash_digest&& hash, uint32_t index) NOEXCEPT
  : point(std::move(hash), index, true)
{
}

point::point(const hash_digest& hash, uint32_t index) NOEXCEPT
  : point(hash, index, true)
{
}

point::point(const data_slice& data) NOEXCEPT
    BC_PUSH_WARNING(NO_THROW_IN_NOEXCEPT)
  : point(stream::in::copy(data))
    BC_POP_WARNING()
{
}

point::point(std::istream&& stream) NOEXCEPT
  : point(read::bytes::istream(stream))
{
}

point::point(std::istream& stream) NOEXCEPT
  : point(read::bytes::istream(stream))
{
}

point::point(reader&& source) NOEXCEPT
  : point(from_data(source))
{
}

point::point(reader& source) NOEXCEPT
  : point(from_data(source))
{
}

// protected
point::point(hash_digest&& hash, uint32_t index, bool valid) NOEXCEPT
  : hash_(std::move(hash)), index_(index), valid_(valid)
{
}

// protected
point::point(const hash_digest& hash, uint32_t index, bool valid) NOEXCEPT
  : hash_(hash), index_(index), valid_(valid)
{
}

// Operators.
// ----------------------------------------------------------------------------

bool point::operator==(const point& other) const NOEXCEPT
{
    return (hash_ == other.hash_)
        && (index_ == other.index_);
}

bool point::operator!=(const point& other) const NOEXCEPT
{
    return !(*this == other);
}

bool operator<(const point& left, const point& right) NOEXCEPT
{
    // Arbitrary compare, for uniqueness sorting.
    return left.index() == right.index() ?
        left.hash() < right.hash() : left.index() < right.index();
}

// Deserialization.
// ----------------------------------------------------------------------------

// static/private
point point::from_data(reader& source) NOEXCEPT
{
    return
    {
        source.read_hash(),
        source.read_4_bytes_little_endian(),
        source
    };
}

// Serialization.
// ----------------------------------------------------------------------------

data_chunk point::to_data() const NOEXCEPT
{
    data_chunk data(serialized_size(), no_fill_byte_allocator);

    BC_PUSH_WARNING(NO_THROW_IN_NOEXCEPT)
    stream::out::copy ostream(data);
    BC_POP_WARNING()

    to_data(ostream);
    return data;
}

void point::to_data(std::ostream& stream) const NOEXCEPT
{
    write::bytes::ostream out(stream);
    to_data(out);
}

void point::to_data(writer& sink) const NOEXCEPT
{
    sink.write_bytes(hash_);
    sink.write_4_bytes_little_endian(index_);
}

// Properties.
// ----------------------------------------------------------------------------

bool point::is_valid() const NOEXCEPT
{
    return valid_;
}

const hash_digest& point::hash() const NOEXCEPT
{
    return hash_;
}

uint32_t point::index() const NOEXCEPT
{
    return index_;
}

// Validation.
// ----------------------------------------------------------------------------

bool point::is_null() const NOEXCEPT
{
    return (index_ == null_index) && (hash_ == null_hash);
}

// JSON value convertors.
// ----------------------------------------------------------------------------

namespace json = boost::json;

// boost/json will soon have NOEXCEPT: github.com/boostorg/json/pull/636
BC_PUSH_WARNING(NO_THROW_IN_NOEXCEPT)

point tag_invoke(json::value_to_tag<point>, const json::value& value) NOEXCEPT
{
    hash_digest hash;
    if (!decode_hash(hash, value.at("hash").get_string().c_str()))
        return {};

    return
    {
        hash,
        value.at("index").to_number<uint32_t>()
    };
}

void tag_invoke(json::value_from_tag, json::value& value,
    const point& point) NOEXCEPT
{
    value =
    {
        { "hash", encode_hash(point.hash()) },
        { "index", point.index() }
    };
}

BC_POP_WARNING()

point::cptr tag_invoke(json::value_to_tag<point::cptr>,
    const json::value& value) NOEXCEPT
{
    return to_shared(tag_invoke(json::value_to_tag<point>{}, value));
}

// Shared pointer overload is required for navigation.
BC_PUSH_WARNING(SMART_PTR_NOT_NEEDED)
BC_PUSH_WARNING(NO_VALUE_OR_CONST_REF_SHARED_PTR)

void tag_invoke(json::value_from_tag tag, json::value& value,
    const point::cptr& output) NOEXCEPT
{
    tag_invoke(tag, value, *output);
}

BC_POP_WARNING()
BC_POP_WARNING()

} // namespace chain
} // namespace system
} // namespace libbitcoin
