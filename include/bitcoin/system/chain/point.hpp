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
#ifndef LIBBITCOIN_SYSTEM_CHAIN_POINT_HPP
#define LIBBITCOIN_SYSTEM_CHAIN_POINT_HPP

#include <functional>
#include <istream>
#include <memory>
#include <vector>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/hash/hash.hpp>
#include <bitcoin/system/stream/stream.hpp>

namespace libbitcoin {
namespace system {
namespace chain {

class BC_API point
{
public:
    DEFAULT_COPY_MOVE_DESTRUCT(point);

    typedef std::shared_ptr<const point> cptr;

    /// This is a sentinel used in .index to indicate no output, e.g. coinbase.
    /// This value is serialized and defined by consensus, not implementation.
    static const uint32_t null_index;

    static constexpr size_t serialized_size() NOEXCEPT
    {
        return hash_size + sizeof(uint32_t);
    }

    /// Constructors.
    /// -----------------------------------------------------------------------

    /// Default point is an invalid null point (null_hash/null_index) object.
    point() NOEXCEPT;

    point(hash_digest&& hash, uint32_t index) NOEXCEPT;
    point(const hash_digest& hash, uint32_t index) NOEXCEPT;

    point(const data_slice& data) NOEXCEPT;
    ////point(stream::in::fast&& stream) NOEXCEPT;
    point(stream::in::fast& stream) NOEXCEPT;
    point(std::istream&& stream) NOEXCEPT;
    point(std::istream& stream) NOEXCEPT;
    point(reader&& source) NOEXCEPT;
    point(reader& source) NOEXCEPT;

    /// Operators.
    /// -----------------------------------------------------------------------

    bool operator==(const point& other) const NOEXCEPT;
    bool operator!=(const point& other) const NOEXCEPT;

    /// Serialization.
    /// -----------------------------------------------------------------------

    data_chunk to_data() const NOEXCEPT;
    void to_data(std::ostream& stream) const NOEXCEPT;
    void to_data(writer& sink) const NOEXCEPT;

    /// Properties.
    /// -----------------------------------------------------------------------

    /// Native properties.
    bool is_valid() const NOEXCEPT;
    const hash_digest& hash() const NOEXCEPT;
    uint32_t index() const NOEXCEPT;

    /// Computed properties.
    bool is_null() const NOEXCEPT;

protected:
    point(hash_digest&& hash, uint32_t index, bool valid) NOEXCEPT;
    point(const hash_digest& hash, uint32_t index, bool valid) NOEXCEPT;

private:
    static point from_data(reader& source) NOEXCEPT;

    // The index is consensus-serialized as a fixed 4 bytes, however it is
    // effectively bound to 2^17 by the block byte size limit.

    // Point should be stored as shared (adds 16 bytes).
    // copy: 256 + 32 + 1 = 37 bytes (vs. 16 when shared).
    hash_digest hash_;
    uint32_t index_;

    // Cache.
    bool valid_;
};

/// Arbitrary compare, for uniqueness sorting.
bool operator<(const point& left, const point& right) NOEXCEPT;

typedef std::vector<point> points;

DECLARE_JSON_VALUE_CONVERTORS(point);
DECLARE_JSON_VALUE_CONVERTORS(point::cptr);

} // namespace chain
} // namespace system
} // namespace libbitcoin

namespace std
{
template<>
struct hash<bc::system::chain::point>
{
    size_t operator()(const bc::system::chain::point& value) const NOEXCEPT
    {
        return bc::system::hash_combine(value.index(),
            bc::system::unique_hash_t<>{}(value.hash()));
    }
};

template<>
struct hash<std::reference_wrapper<const bc::system::chain::point>>
{
    using wrapped = std::reference_wrapper<const bc::system::chain::point>;
    std::size_t operator()(const wrapped& point) const NOEXCEPT
    {
        return std::hash<bc::system::chain::point>{}(point.get());
    }
};

inline bool operator==(
    const std::reference_wrapper<const bc::system::chain::point>& left,
    const std::reference_wrapper<const bc::system::chain::point>& right) NOEXCEPT
{
    return left.get() == right.get();
}

} // namespace std

#endif
