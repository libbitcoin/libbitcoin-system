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
    typedef std::shared_ptr<point> ptr;

    /// This is a sentinel used in .index to indicate no output, e.g. coinbase.
    /// This value is serialized and defined by consensus, not implementation.
    static const uint32_t null_index;

    // Constructors.
    // ------------------------------------------------------------------------

    /// Default point is a valid (null point) object.
    point();

    point(point&& other);
    point(const point& other);

    point(hash_digest&& hash, uint32_t index);
    point(const hash_digest& hash, uint32_t index);

    point(const data_slice& data);
    point(std::istream& stream);
    point(reader& source);

    // Operators.
    // ------------------------------------------------------------------------

    point& operator=(point&& other);
    point& operator=(const point& other);

    bool operator==(const point& other) const;
    bool operator!=(const point& other) const;

    // Deserialization.
    // ------------------------------------------------------------------------

    bool from_data(const data_slice& data);
    bool from_data(std::istream& stream);
    bool from_data(reader& source);

    /// Deserialization result.
    bool is_valid() const;

    // Serialization.
    // ------------------------------------------------------------------------

    data_chunk to_data() const;
    void to_data(std::ostream& stream) const;
    void to_data(writer& sink) const;

    // Properties.
    // ------------------------------------------------------------------------

    /// Native properties.
    const hash_digest& hash() const;
    uint32_t index() const;

    /// Computed properties.
    bool is_null() const;
    static size_t serialized_size();

protected:
    // So input may reset its member.
    friend class input;

    point(hash_digest&& hash, uint32_t index, bool valid);
    point(const hash_digest& hash, uint32_t index, bool valid);

    void reset();

private:
    // The index is consensus-serialized as a fixed 4 bytes, however it is
    // effectively bound to 2^17 by the block byte size limit.
    hash_digest hash_;
    uint32_t index_;
    bool valid_;
};

/// Arbitrary compare, for uniqueness sorting.
bool operator<(const point& left, const point& right);

typedef std::vector<point> points;

} // namespace chain
} // namespace system
} // namespace libbitcoin

#endif
