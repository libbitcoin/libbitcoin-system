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
#include <sstream>
#include <boost/algorithm/string.hpp>
#include <bitcoin/bitcoin/constants.hpp>

namespace libbitcoin {

static bool is_digit(const char c)
{
    return '0' <= c && c <= '9';
}

template<char C>
bool char_is(const char c)
{
    return c == C;
}

bool parse_amount(uint64_t& out, std::string amount,
    bool strict, uint8_t decimal_places)
{
    // Get rid of the decimal point:
    auto point = std::find(amount.begin(), amount.end(), '.');
    if (point != amount.end())
        point = amount.erase(point);

    // Only digits should remain:
    if (!std::all_of(amount.begin(), amount.end(), is_digit))
        return false;

    // Add digits to the end if there are too few:
    auto actual_places = amount.end() - point;
    if (actual_places < decimal_places)
        amount.append(decimal_places - actual_places, '0');

    // Remove digits from the end if there are too many:
    bool round = false;
    if (actual_places > decimal_places)
    {
        auto end = point + decimal_places;
        round = !std::all_of(end, amount.end(), char_is<'0'>);
        amount.erase(end, amount.end());
    }
    if (strict && round)
        return false;

    // Convert to an integer:
    std::istringstream stream(amount);
    uint64_t value = 0;
    if (!(stream >> value))
        return false;

    // Round and return:
    if (round && value == max_uint64)
        return false;
    out = value + round;
    return true;
}

std::string format_amount(uint64_t amount, uint8_t decimal_places)
{
    std::ostringstream stream;
    stream << std::setfill('0') << std::setw(1 + decimal_places) << amount;

    auto string = stream.str();
    string.insert(string.size() - decimal_places, 1, '.');
    boost::algorithm::trim_right_if(string, char_is<'0'>);
    boost::algorithm::trim_right_if(string, char_is<'.'>);
    return string;
}

bool btc_to_satoshi(uint64_t& satoshi, const std::string& btc)
{
    return parse_amount(satoshi, btc);
}

std::string satoshi_to_btc(uint64_t satoshi)
{
    return format_amount(satoshi);
}

} // namespace libbitcoin
