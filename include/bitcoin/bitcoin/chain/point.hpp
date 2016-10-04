/**
 * Copyright (c) 2011-2015 libbitcoin developers (see AUTHORS)
 *
 * This file is part of libbitcoin.
 *
 * libbitcoin is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License with
 * additional permissions to the one published by the Free Software
 * Foundation, either version 3 of the License, or (at your option)
 * any later version. For more information see LICENSE.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef LIBBITCOIN_CHAIN_POINT_HPP
#define LIBBITCOIN_CHAIN_POINT_HPP

#include <cstdint>
#include <istream>
#include <string>
#include <vector>
#include <boost/functional/hash.hpp>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/chain/point_iterator.hpp>
#include <bitcoin/bitcoin/math/hash.hpp>
#include <bitcoin/bitcoin/utility/data.hpp>
#include <bitcoin/bitcoin/utility/reader.hpp>
#include <bitcoin/bitcoin/utility/writer.hpp>

namespace libbitcoin {
namespace chain {

class BC_API point
{
public:
    /// This is a sentinel used in .index to indicate no output, e.g. coinbase.
    /// This value is serialized and defined by consensus, not implementation.
    static const uint32_t null_index;

    /// This is a sentinel used internally to signal invalidity of the point.
    static const uint32_t invalid_point;

    typedef std::vector<point> list;
    typedef std::vector<uint32_t> indexes;

    static point factory_from_data(const data_chunk& data);
    static point factory_from_data(std::istream& stream);
    static point factory_from_data(reader& source);
    static uint64_t satoshi_fixed_size();

    point();
    point(const hash_digest& hash, uint32_t index);
    point(hash_digest&& hash, uint32_t index);
    point(const point& other);
    point(point&& other);

    hash_digest& hash();
    const hash_digest& hash() const;
    void set_hash(const hash_digest& value);
    void set_hash(hash_digest&& value);

    uint32_t index() const;
    void set_index(uint32_t value);

    bool from_data(const data_chunk& data);
    bool from_data(std::istream& stream);
    bool from_data(reader& source);
    data_chunk to_data() const;
    void to_data(std::ostream& stream) const;
    void to_data(writer& sink) const;
    std::string to_string() const;

    void reset();
    bool is_valid() const;
    bool is_null() const;

    uint64_t checksum() const;
    uint64_t serialized_size() const;

    /// This class is move assignable and copy assignable.
    point& operator=(point&& other);
    point& operator=(const point& other);

    bool operator==(const point& other) const;
    bool operator!=(const point& other) const;

private:
    hash_digest hash_;
    uint32_t index_;
};

typedef point input_point;

} // namespace chain
} // namespace libbitcoin

namespace std
{

// Extend std namespace with our hash wrapper, used as database hash.
template <>
struct hash<bc::chain::point>
{
    // Changes to this function invalidate existing database files.
    size_t operator()(const bc::chain::point& point) const
    {
        size_t seed = 0;
        boost::hash_combine(seed, point.hash());
        boost::hash_combine(seed, point.index());
        return seed;
    }
};

// Extend std namespace with the size of our point, used as database key size.
template <>
struct tuple_size<bc::chain::point>
{
    static const size_t value = std::tuple_size<bc::hash_digest>::value +
        sizeof(uint32_t);

    operator std::size_t() const
    {
        return value;
    }
};

} // namespace std

#endif
