/*
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
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
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/math/hash.hpp>
#include <bitcoin/bitcoin/utility/data.hpp>
#include <bitcoin/bitcoin/utility/deserializer.hpp>

namespace libbitcoin {
namespace chain {

class BC_API point
{
public:

    point();

    point(hash_digest hash, uint32_t index);

    point(std::istream& stream);

    point(const data_chunk& value);

    template <typename Iterator, bool SafeCheckLast>
    point(deserializer<Iterator, SafeCheckLast>& deserial);

    template<typename Iterator>
    point(const Iterator begin, const Iterator end);

    hash_digest& hash();

    const hash_digest& hash() const;

    void hash(const hash_digest& hash);

    uint32_t index() const;

    void index(uint32_t index);

    std::string to_string() const;

    bool is_null() const;

    operator const data_chunk() const;

    size_t satoshi_size() const;

    static size_t satoshi_fixed_size();

private:

    template <typename Iterator, bool SafeCheckLast>
    void deserialize(deserializer<Iterator, SafeCheckLast>& deserial);

    hash_digest hash_;
    uint32_t index_;
};

typedef std::vector<point> point_list;

typedef point input_point;
typedef point_list input_point_list;

typedef point output_point;
typedef point_list output_point_list;

BC_API bool operator==(const point& a, const point& b);

BC_API bool operator!=(const point& a, const point& b);

} // end chain
} // end libbitcoin

#include <bitcoin/bitcoin/impl/chain/point.ipp>

#endif
