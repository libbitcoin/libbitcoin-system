/*
 * Copyright (c) 2011-2014 libbitcoin developers (see AUTHORS)
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
#include <bitcoin/bitcoin/utility/base16.hpp>

#include <iomanip>
#include <sstream>
#include <boost/algorithm/string.hpp>
#include <bitcoin/bitcoin/constants.hpp>
#include <bitcoin/bitcoin/utility/assert.hpp>

namespace libbitcoin {

bool is_base16(const char c)
{
    return ('0' <= c && c <= '9') ||
        ('A' <= c && c <= 'F') ||
        ('a' <= c && c <= 'f');
}

std::string encode_base16(data_slice data)
{
    std::stringstream ss;
    ss << std::hex;
    for (int val: data)
        ss << std::setw(2) << std::setfill('0') << val;
    return ss.str();
}

bool decode_base16(data_chunk& out, const std::string& in)
{
    auto size = in.size();

    // This prevents a last odd character from being ignored.
    if (size % 2 != 0)
        return false;

    data_chunk result(in.size() / 2);
    for (size_t i = 0; i + 1 < size; i += 2)
    {
        BITCOIN_ASSERT(size - i >= 2);
        auto byte_begin = in.begin() + i;
        auto byte_end = in.begin() + i + 2;
        // Perform conversion.
        int val = -1;
        std::stringstream converter;
        converter << std::hex << std::string(byte_begin, byte_end);
        converter >> val;
        if (val == -1)
            return false;
        BITCOIN_ASSERT(val <= 0xff);
        // Set byte.
        result[i / 2] = val;
    }
    out = result;
    return true;
}

hash_digest decode_hash(const std::string& in)
{
    data_chunk data;
    if (!decode_base16(data, in))
        return null_hash;

    hash_digest result;
    if (data.size() != result.size())
        return null_hash;

    std::copy(data.begin(), data.end(), result.begin());
    return result;
}

std::string encode_hex(data_slice in)
{
    return encode_base16(in);
}

data_chunk decode_hex(std::string in)
{
    // Trim the fat:
    boost::algorithm::trim(in);

    data_chunk out;
    decode_base16(out, in);
    return out;
}

} // namespace libbitcoin

