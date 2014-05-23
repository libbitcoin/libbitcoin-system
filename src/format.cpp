/*
 * Copyright (c) 2011-2013 libbitcoin developers (see AUTHORS)
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
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <bitcoin/format.hpp>
#include <bitcoin/primitives.hpp>
#include <bitcoin/utility/assert.hpp>

namespace libbitcoin {

std::ostream& operator<<(std::ostream& stream, const data_chunk& data)
{
    stream << encode_hex(data);
    return stream;
}

std::ostream& operator<<(std::ostream& stream, const hash_digest& hash)
{
    stream << encode_hex(hash);
    return stream;
}

std::ostream& operator<<(std::ostream& stream, const short_hash& hash)
{
    stream << encode_hex(hash);
    return stream;
}

template <typename Point>
std::ostream& concat_point(std::ostream& stream, const Point& point)
{
    stream << point.hash << ":" << point.index;
    return stream;
}
std::ostream& operator<<(std::ostream& stream, const output_point& point)
{
    return concat_point(stream, point);
}

data_chunk decode_hex(std::string hex)
{
    // Trim the fat.
    boost::algorithm::trim(hex);
    data_chunk result(hex.size() / 2);
    for (size_t i = 0; i + 1 < hex.size(); i += 2)
    {
        BITCOIN_ASSERT(hex.size() - i >= 2);
        auto byte_begin = hex.begin() + i;
        auto byte_end = hex.begin() + i + 2;
        // Perform conversion.
        int val = -1;
        std::stringstream converter;
        converter << std::hex << std::string(byte_begin, byte_end);
        converter >> val;
        if (val == -1)
            return data_chunk();
        BITCOIN_ASSERT(val <= 0xff);
        // Set byte.
        result[i / 2] = val;
    }
    return result;
}

template <typename HashType>
HashType decode_hex_digest(const std::string& hex)
{
    data_chunk raw_bytes = decode_hex(hex);
    HashType result;
    if (raw_bytes.size() != result.size())
    {
        // null_hash for hash_digest
        // null_short_hash for short_hash
        result.fill(0);
        return result;
    }
    BITCOIN_ASSERT(raw_bytes.size() == result.size());
    std::copy(raw_bytes.begin(), raw_bytes.end(), result.begin());
    return result;
}

hash_digest decode_hash(const std::string& hex)
{
    return decode_hex_digest<hash_digest>(hex);
}

short_hash decode_short_hash(const std::string& hex)
{
    return decode_hex_digest<short_hash>(hex);
}

std::string satoshi_to_btc(uint64_t value)
{
    uint64_t major = value / coin_price(1);
    std::string result = boost::lexical_cast<std::string>(major);
    BITCOIN_ASSERT(value >= major);
    uint64_t minor = value - (major * coin_price(1));
    BITCOIN_ASSERT(minor < coin_price(1));
    if (minor > 0)
    {
        std::string minor_str = boost::lexical_cast<std::string>(minor);
        std::string padded_minor(8 - minor_str.size(), '0');
        padded_minor += minor_str;
        boost::algorithm::trim_right_if(padded_minor,
            boost::is_any_of("0"));
        result = result + "." + padded_minor;
    }
    return result;
}

} // namespace libbitcoin

