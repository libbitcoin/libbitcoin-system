/**
 * Copyright (c) 2011-2023 libbitcoin developers (see AUTHORS)
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

#include <algorithm>
#include <iostream>
#include <utility>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/hash/hash.hpp>
#include <bitcoin/system/radix/radix.hpp>
#include <bitcoin/system/serial/serial.hpp>
#include <bitcoin/system/stream/stream.hpp>

namespace libbitcoin {
namespace system {
namespace chain {

bool checkpoint::is_at(const list& checkpoints, size_t height) NOEXCEPT
{
    return std::any_of(checkpoints.begin(), checkpoints.end(),
        [&](const auto& item) NOEXCEPT
        {
            return height == item.height();
        });
}

bool checkpoint::is_under(const list& checkpoints, size_t height) NOEXCEPT
{
    // False if empty, optimal if checkpoints sorted by increasing height.
    return std::any_of(checkpoints.rbegin(), checkpoints.rend(),
        [&](const auto& item) NOEXCEPT
        {
            return height <= item.height();
        });
}

bool checkpoint::is_conflict(const list& checkpoints, const hash_digest& hash,
    size_t height) NOEXCEPT
{
    //  Conflict if height matches and hash does not.
    const auto it = std::find_if(checkpoints.begin(), checkpoints.end(),
        [&](const auto& item) NOEXCEPT
        {
            return height == item.height();
        });

    return it != checkpoints.end() && it->hash() != hash;
}

// Constructors.
// ----------------------------------------------------------------------------

checkpoint::checkpoint() NOEXCEPT
  : checkpoint({}, zero, false)
{
}

checkpoint::checkpoint(hash_digest&& hash, size_t height) NOEXCEPT
  : checkpoint(std::move(hash), height, true)
{
}

checkpoint::checkpoint(const hash_digest& hash, size_t height) NOEXCEPT
  : checkpoint(hash, height, true)
{
}

checkpoint::checkpoint(const std::string& hash, size_t height) NOEXCEPT
  : checkpoint(from_string(hash, height))
{
}

// protected
checkpoint::checkpoint(hash_digest&& hash, size_t height, bool valid) NOEXCEPT
  : hash_(std::move(hash)), height_(height), valid_(valid)
{
}

// protected
checkpoint::checkpoint(const hash_digest& hash, size_t height,
    bool valid) NOEXCEPT
  : hash_(hash), height_(height), valid_(valid)
{
}

// private
checkpoint checkpoint::from_string(const std::string& hash,
    size_t height) NOEXCEPT
{
    hash_digest digest;
    if (!decode_hash(digest, hash))
        return {};

    return { std::move(digest), height, true };
}

// functional equality
bool checkpoint::equals(const hash_digest& hash, size_t height) const NOEXCEPT
{
    return this->hash() == hash && this->height() == height;
}

// Operators.
// ----------------------------------------------------------------------------

bool operator<(const checkpoint& left, const checkpoint& right) NOEXCEPT
{
    return left.height() < right.height();
}

bool operator==(const checkpoint& left, const checkpoint& right) NOEXCEPT
{
    return left.equals(right.hash(), right.height());
}

bool operator!=(const checkpoint& left, const checkpoint& right) NOEXCEPT
{
    return !(left == right);
}

// Deserialization.
// ----------------------------------------------------------------------------

// TODO: add from_string.
// TODO: add get_line/put_line to reader and eliminate stream_result.
std::istream& operator>>(std::istream& stream, checkpoint& out) THROWS
{
    std::string value;
    stream >> value;

    hash_digest hash;
    size_t height(zero);

    // std::string avoids boolean override.
    const auto tokens = split(value, std::string{ ":" });

    if (tokens.size() != two ||
        !decode_hash(hash, tokens.front()) ||
        !deserialize(height, tokens.back()))
        throw istream_exception(value);

    out = { hash, height };
    return stream;
}

bool checkpoint::is_valid() const NOEXCEPT
{
    return valid_;
}

// Serialization.
// ----------------------------------------------------------------------------

std::ostream& operator<<(std::ostream& stream, const checkpoint& in) NOEXCEPT
{
    stream << in.to_string();
    return stream;
}

std::string checkpoint::to_string() const NOEXCEPT
{
    return encode_hash(hash_) + ":" + serialize(height_);
}

// Properties.
// ----------------------------------------------------------------------------

size_t checkpoint::height() const NOEXCEPT
{
    return height_;
}

const hash_digest& checkpoint::hash() const NOEXCEPT
{
    return hash_;
}

// JSON value convertors.
// ----------------------------------------------------------------------------

namespace json = boost::json;

checkpoint tag_invoke(json::value_to_tag<checkpoint>,
    const json::value& value) NOEXCEPT
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
    const checkpoint& checkpoint) NOEXCEPT
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
