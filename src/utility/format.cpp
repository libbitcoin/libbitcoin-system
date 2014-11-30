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
#include <bitcoin/bitcoin/utility/format.hpp>

#include <iomanip>
#include <sstream>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <bitcoin/bitcoin/constants.hpp>
#include <bitcoin/bitcoin/utility/assert.hpp>

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

std::ostream& operator<<(std::ostream& stream, const point_type& point)
{
    stream << point.hash << ":" << point.index;
    return stream;
}

std::string encode_hex(data_slice data)
{
    std::stringstream ss;
    ss << std::hex;
    for (int val: data)
        ss << std::setw(2) << std::setfill('0') << val;
    return ss.str();
}

data_chunk decode_hex(std::string hex)
{
    // Trim the fat.
    boost::algorithm::trim(hex);
    auto size = hex.size();

    // This prevents a last odd character from being ignored.
    if (size % 2 != 0)
        return data_chunk();

    data_chunk result(hex.size() / 2);
    for (size_t i = 0; i + 1 < size; i += 2)
    {
        BITCOIN_ASSERT(size - i >= 2);
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

static std::string pad_left_8_zeroes_trim_right(const std::string& value)
{
    BITCOIN_ASSERT(value.size() <= 8);
    std::string result(8 - value.size(), '0');
    result += value;
    boost::algorithm::trim_right_if(result, boost::is_any_of("0"));
    return result;
}

static std::string pad_right_8_zeroes_trim_left(const std::string& value)
{
    if (value.size() > 8)
        throw boost::bad_lexical_cast();
    auto result = value + std::string(8 - value.size(), '0');
    boost::algorithm::trim_left_if(result, boost::is_any_of("0"));
    return result;
}

// There are a number of places in libbitcoin where overflowable operations,
// either signed or unsigned, may causes errors. See discussion for more info:
// www.securecoding.cert.org/confluence/display/seccode/
// INT32-C.+Ensure+that+operations+on+signed+integers+do+not+result+in+overflow
static inline bool sum_will_overflow(uint64_t addend1, uint64_t addend2)
{
    return addend1 > (max_uint64 - addend2);
}

bool btc_to_satoshi(uint64_t& satoshi, const std::string& btc)
{
    std::vector<std::string> parts;
    boost::split(parts, btc, boost::is_any_of("."));
    if (parts.size() > 2)
        return false;

    uint64_t minor = 0;
    uint64_t major = 0;
    try
    {
        if (parts.front().size() > 0)
            major = boost::lexical_cast<int64_t>(parts.front()) * coin_price(1);
        if (parts.size() == 2 && parts.back().size() > 0)
            minor = boost::lexical_cast<uint64_t>(
                pad_right_8_zeroes_trim_left(parts.back()));
    }
    catch (boost::bad_lexical_cast&)
    {
        return false;
    }

    if (sum_will_overflow(major, minor))
        return false;

    satoshi = major + minor;
    return true;
}

std::string satoshi_to_btc(uint64_t satoshi)
{
    uint64_t major = satoshi / coin_price(1);
    BITCOIN_ASSERT(satoshi >= major);
    uint64_t minor = satoshi - (major * coin_price(1));
    BITCOIN_ASSERT(minor < coin_price(1));
    const auto result = boost::lexical_cast<std::string>(major);
    if (minor > 0)
    {
        const auto minor_str = boost::lexical_cast<std::string>(minor);
        return result + "." + pad_left_8_zeroes_trim_right(minor_str);
    }
    return result;
}

} // namespace libbitcoin

