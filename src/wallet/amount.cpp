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
#include <bitcoin/bitcoin/wallet/amount.hpp>

#include <iomanip>
#include <cstdint>
#include <sstream>
#include <boost/algorithm/string.hpp>
#include <bitcoin/bitcoin/define.hpp>

namespace libbitcoin {

static bool is_digit(const char c)
{
    return '0' <= c && c <= '9';
}

template <typename Int>
Int multiply_clamp(Int a, Int b, Int max)
{
    if (a > max / b)
        return max;
    else
        return a * b;
}

template <typename Int>
Int add_clamp(Int a, Int b, Int max)
{
    if (a > max - b)
        return max;
    else
        return a + b;
}

uint64_t parse_amount(const std::string& amount, uint8_t decmial_places)
{
    auto i = amount.begin();
    uint64_t value = 0;
    unsigned places = 0;
    const uint64_t invalid_amount = MAX_UINT64;

    while (amount.end() != i && is_digit(*i))
    {
        auto x10 = multiply_clamp<uint64_t>(value, 10, invalid_amount);
        value = add_clamp<uint64_t>(x10, (*i - '0'), invalid_amount);
        ++i;
    }
    if (amount.end() != i && '.' == *i)
    {
        ++i;
        while (amount.end() != i && is_digit(*i))
        {
            if (places < decmial_places)
            {
                auto x10 = multiply_clamp<uint64_t>(value, 10, invalid_amount);
                value = add_clamp<uint64_t>(x10, (*i - '0'), invalid_amount);
            }
            else if (places == decmial_places && '5' <= *i)
                value = add_clamp<uint64_t>(value, 1, invalid_amount);
            ++places;
            ++i;
        }
    }
    while (places < decmial_places)
    {
        value = multiply_clamp<uint64_t>(value, 10, invalid_amount);
        ++places;
    }
    if (amount.end() != i)
        return invalid_amount;
    return value;
}

std::string format_amount(uint64_t amount, uint8_t decimal_places)
{
    // Get the integer and fractional parts:
    uint64_t factor = 1;
    for (unsigned i = 0; i < decimal_places; ++i)
        factor *= 10;
    uint64_t int_part = amount / factor;
    uint64_t decimal_part = amount % factor;
    // Format as a fixed-point number:
    std::ostringstream stream;
    stream << int_part << '.';
    stream << std::setw(decimal_places) << std::setfill('0') << decimal_part;
    // Trim trailing zeros:
    auto string = stream.str();
    boost::algorithm::trim_right_if(string, [](char c){ return '0' == c; });
    boost::algorithm::trim_right_if(string, [](char c){ return '.' == c; });
    return string;
}

} // namespace libbitcoin
