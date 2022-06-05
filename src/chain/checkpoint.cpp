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
#include <bitcoin/system/chain/checkpoint.hpp>

#include <cstddef>
#include <iostream>
#include <string>
#include <utility>
#include <bitcoin/system/constants.hpp>
#include <bitcoin/system/crypto/crypto.hpp>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/radix/radix.hpp>
#include <bitcoin/system/serial/serial.hpp>
#include <bitcoin/system/stream/stream.hpp>

namespace libbitcoin {
namespace system {
namespace chain {

// Constructors.
// ----------------------------------------------------------------------------

checkpoint::checkpoint() noexcept
  : checkpoint({}, zero, false)
{
}

checkpoint::checkpoint(hash_digest&& hash, size_t height) noexcept
  : checkpoint(std::move(hash), height, true)
{
}

checkpoint::checkpoint(const hash_digest& hash, size_t height) noexcept
  : checkpoint(hash, height, true)
{
}

checkpoint::checkpoint(const std::string& hash, size_t height) noexcept
  : checkpoint(from_string(hash, height))
{
}

// protected
checkpoint::checkpoint(hash_digest&& hash, size_t height, bool valid) noexcept
  : hash_(std::move(hash)), height_(height), valid_(valid)
{
}

// protected
checkpoint::checkpoint(const hash_digest& hash, size_t height,
    bool valid) noexcept
  : hash_(hash), height_(height), valid_(valid)
{
}

// private
checkpoint checkpoint::from_string(const std::string& hash,
    size_t height) noexcept
{
    hash_digest digest;
    if (!decode_hash(digest, hash))
        return {};

    return { std::move(digest), height, true };
}

// Operators.
// ----------------------------------------------------------------------------

bool operator<(const checkpoint& left, const checkpoint& right) noexcept
{
    return left.height() < right.height();
}

bool operator==(const checkpoint& left, const checkpoint& right) noexcept
{
    return left.hash() == right.hash() && left.height() == right.height();
}

bool operator!=(const checkpoint& left, const checkpoint& right) noexcept
{
    return !(left == right);
}

// Deserialization.
// ----------------------------------------------------------------------------

// TODO: add from_string.
// TODO: add get_line/put_line to reader and eliminate stream_result.
std::istream& operator>>(std::istream& stream, checkpoint& out) noexcept
{
    std::string value;
    stream >> value;

    hash_digest hash;
    size_t height(zero);
    const auto tokens = split(value, ":");

    if (tokens.size() == two &&
        decode_hash(hash, tokens.front()) &&
        deserialize(height, tokens.back()))
    {
        out = { hash, height };
        return stream_result(stream, true);
    }

    out = {};
    return stream_result(stream, false);
}

bool checkpoint::is_valid() const noexcept
{
    return valid_;
}

// Serialization.
// ----------------------------------------------------------------------------

std::ostream& operator<<(std::ostream& stream, const checkpoint& in) noexcept
{
    stream << in.to_string();
    return stream;
}

std::string checkpoint::to_string() const noexcept
{
    return encode_hash(hash_) + ":" + serialize(height_);
}

// Properties.
// ----------------------------------------------------------------------------

size_t checkpoint::height() const noexcept
{
    return height_;
}

const hash_digest& checkpoint::hash() const noexcept
{
    return hash_;
}

// JSON value convertors.
// ----------------------------------------------------------------------------

namespace json = boost::json;

checkpoint tag_invoke(json::value_to_tag<checkpoint>,
    const json::value& value) noexcept
{
    hash_digest hash;
    if (!decode_hash(hash, value.at("hash").get_string().c_str()))
        return {};

    return
    {
        hash,
        value.at("height").to_number<size_t>()
    };
}

void tag_invoke(json::value_from_tag, json::value& value,
    const checkpoint& checkpoint) noexcept
{
    value =
    {
        { "hash", encode_hash(checkpoint.hash()) },
        { "height", checkpoint.height() }
    };
}

} // namespace chain
} // namespace system
} // namespace libbitcoin
