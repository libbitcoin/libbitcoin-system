/**
 * Copyright (c) 2011-2026 libbitcoin developers (see AUTHORS)
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
#ifndef LIBBITCOIN_SYSTEM_CHAIN_OUTPOINT_HPP
#define LIBBITCOIN_SYSTEM_CHAIN_OUTPOINT_HPP

#include <memory>
#include <unordered_set>
#include <bitcoin/system/chain/point.hpp>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/hash/hash.hpp>
#include <bitcoin/system/stream/stream.hpp>

namespace libbitcoin {
namespace system {
namespace chain {

class BC_API outpoint
{
public:
    DEFAULT_COPY_MOVE_DESTRUCT(outpoint);

    typedef std::shared_ptr<const outpoint> cptr;

    static constexpr size_t serialized_size() NOEXCEPT
    {
        return point::serialized_size() + sizeof(uint64_t);
    }

    /// Constructors.
    /// -----------------------------------------------------------------------

    /// Default outpoint is an invalid null outpoint (null_hash/null_index/0).
    outpoint() NOEXCEPT;

    outpoint(chain::point&& point, uint64_t value) NOEXCEPT;
    outpoint(const chain::point& point, uint64_t value) NOEXCEPT;

    outpoint(const data_slice& data) NOEXCEPT;
    outpoint(stream::in::fast& stream) NOEXCEPT;
    outpoint(std::istream& stream) NOEXCEPT;
    outpoint(reader& source) NOEXCEPT;

    /// Operators.
    /// -----------------------------------------------------------------------

    bool operator==(const outpoint& other) const NOEXCEPT;
    bool operator!=(const outpoint& other) const NOEXCEPT;

    /// Serialization.
    /// -----------------------------------------------------------------------

    data_chunk to_data() const NOEXCEPT;
    void to_data(std::ostream& stream) const NOEXCEPT;
    void to_data(writer& sink) const NOEXCEPT;

    /// Properties.
    /// -----------------------------------------------------------------------

    /// Native properties.
    bool is_valid() const NOEXCEPT;
    const chain::point& point() const NOEXCEPT;
    uint64_t value() const NOEXCEPT;

    /// Computed properties.
    bool is_null() const NOEXCEPT;

protected:
    outpoint(stream::in::fast&& stream) NOEXCEPT;
    outpoint(reader&& source) NOEXCEPT;
    outpoint(chain::point&& point, uint64_t value, bool valid) NOEXCEPT;
    outpoint(const chain::point& point, uint64_t value, bool valid) NOEXCEPT;

private:
    chain::point point_;
    uint64_t value_;

    // Cache.
    bool valid_;
};

/// Arbitrary compare, for uniqueness sorting.
bool operator<(const outpoint& left, const outpoint& right) NOEXCEPT;

typedef std_vector<outpoint> outpoints;

DECLARE_JSON_TAG_INVOKE(outpoint);
DECLARE_JSON_TAG_INVOKE(outpoint::cptr);

} // namespace chain
} // namespace system
} // namespace libbitcoin

namespace std
{
template<>
struct hash<bc::system::chain::outpoint>
{
    size_t operator()(const bc::system::chain::outpoint& value) const NOEXCEPT
    {
        // Value does not contribute to identity.
        return std::hash<bc::system::chain::point>{}(value.point());
    }
};
} // namespace std

#endif
