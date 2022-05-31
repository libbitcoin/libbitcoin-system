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
#ifndef LIBBITCOIN_SYSTEM_CHAIN_POINT_HPP
#define LIBBITCOIN_SYSTEM_CHAIN_POINT_HPP

#include <cstdint>
#include <istream>
#include <memory>
#include <string>
#include <vector>
#include <boost/json.hpp>
#include <bitcoin/system/crypto/crypto.hpp>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/stream/stream.hpp>

namespace libbitcoin {
namespace system {
namespace chain {

class BC_API point
{
public:
    typedef std::shared_ptr<const point> ptr;

    /// This is a sentinel used in .index to indicate no output, e.g. coinbase.
    /// This value is serialized and defined by consensus, not implementation.
    static const uint32_t null_index;

    // Constructors.
    // ------------------------------------------------------------------------

    /// Default point is an invalid null point (null_hash/null_index) object.
    point() noexcept;

    /// Defaults.
    point(point&&) = default;
    point(const point&) = default;
    point& operator=(point&&) = default;
    point& operator=(const point&) = default;
    ~point() = default;

    point(hash_digest&& hash, uint32_t index) noexcept;
    point(const hash_digest& hash, uint32_t index) noexcept;

    point(const data_slice& data) noexcept;
    point(std::istream&& stream) noexcept;
    point(std::istream& stream) noexcept;
    point(reader&& source) noexcept;
    point(reader& source) noexcept;

    // Operators.
    // ------------------------------------------------------------------------

    bool operator==(const point& other) const noexcept;
    bool operator!=(const point& other) const noexcept;

    // Serialization.
    // ------------------------------------------------------------------------

    data_chunk to_data() const noexcept;
    void to_data(std::ostream& stream) const noexcept;
    void to_data(writer& sink) const noexcept;

    // Properties.
    // ------------------------------------------------------------------------

    /// Native properties.
    bool is_valid() const noexcept;
    const hash_digest& hash() const noexcept;
    uint32_t index() const noexcept;

    /// Computed properties.
    bool is_null() const noexcept;
    static size_t serialized_size() noexcept;

protected:
    point(hash_digest&& hash, uint32_t index, bool valid) noexcept;
    point(const hash_digest& hash, uint32_t index, bool valid) noexcept;

private:
    static point from_data(reader& source) noexcept;

    // The index is consensus-serialized as a fixed 4 bytes, however it is
    // effectively bound to 2^17 by the block byte size limit.

    // Point should be stored as shared (adds 16 bytes).
    // copy: 256 + 32 + 1 = 37 bytes (vs. 16 when shared).
    hash_digest hash_;
    uint32_t index_;
    bool valid_;
};

/// Arbitrary compare, for uniqueness sorting.
bool operator<(const point& left, const point& right) noexcept;

typedef std::vector<point> points;

DECLARE_JSON_VALUE_CONVERTORS(point);
DECLARE_JSON_VALUE_CONVERTORS(point::ptr);

} // namespace chain
} // namespace system
} // namespace libbitcoin

namespace std
{
template<>
struct hash<bc::system::chain::point>
{
    size_t operator()(const bc::system::chain::point& value) const noexcept
    {
        return bc::system::hash_combine(
            std::hash<bc::system::hash_digest>{}(value.hash()), value.index());
    }
};
} // namespace std

#endif
