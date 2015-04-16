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
#include <bitcoin/bitcoin/chain/point.hpp>

#include <sstream>
#include <bitcoin/bitcoin/constants.hpp>
#include <bitcoin/bitcoin/formats/base16.hpp>
#include <bitcoin/bitcoin/utility/serializer.hpp>

namespace libbitcoin {
namespace chain {

point::point()
//    :hash_(null_hash), index_(std::numeric_limits<uint32_t>::max())
{
}

point::point(hash_digest hash, uint32_t index)
    : hash_(hash), index_(index)
{
}

point::point(const data_chunk& value)
    : point(value.begin(), value.end())
{
}

const hash_digest& point::hash() const
{
    return hash_;
}

uint32_t point::index() const
{
    return index_;
}

bool point::set_hash(const hash_digest& hash)
{
    hash_ = hash;
    return true;
}

bool point::set_index(uint32_t index)
{
    index_ = index;
    return true;
}

std::string point::to_string() const
{
    std::ostringstream ss;

    ss << "\thash = " << encode_hash(hash_) << "\n" << "\tindex = " << index_;

    return ss.str();
}

bool point::is_null() const
{
    return (index_ == max_index) && (hash_ == null_hash);
}

point::operator const data_chunk() const
{
    data_chunk result(satoshi_size());
    auto serial = make_serializer(result.begin());

    serial.write_hash(hash_);
    serial.write_4_bytes(index_);

    BITCOIN_ASSERT(
        std::distance(result.begin(), serial.iterator())
            == point::satoshi_fixed_size());

    return result;
}

size_t point::satoshi_size() const
{
    return point::satoshi_fixed_size();
}

size_t point::satoshi_fixed_size()
{
    return hash_size + 4;
}

bool operator==(const point& a, const point& b)
{
    return a.hash() == b.hash() && a.index() == b.index();
}
bool operator!=(const point& a, const point& b)
{
    return !(a == b);
}

} // end chain
} // end libbitcoin
