/**
 * Copyright (c) 2011-2018 libbitcoin developers (see AUTHORS)
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
#include <bitcoin/bitcoin/formats/base_10.hpp>

#include <iomanip>
#include <sstream>
#include <boost/algorithm/string.hpp>
#include <bitcoin/bitcoin/constants.hpp>

namespace libbitcoin {

/**
 * Returns true if a character is one of `[0-9]`.
 *
 * The C standard library function `isdigit` depends on the current locale,
 * and doesn't necessarily correspond to the valid base10 encoding digits.
 * The Microsoft `isdigit` includes superscript characters like '²'
 * in some locales, for example.
 */
static bool is_digit(const char c)
{
    return '0' <= c && c <= '9';
}

template <char C>
bool char_is(const char c)
{
    return c == C;
}

bool decode_base10(uint64_t& out, const std::string& amount,
    uint8_t decimal_places, bool strict)
{
    std::string value(amount);

    // Get rid of the decimal point:
    auto point = std::find(value.begin(), value.end(), '.');
    if (point != value.end())
        point = value.erase(point);

    // Only digits should remain:
    if (!std::all_of(value.begin(), value.end(), is_digit))
        return false;

    // Add digits to the end if there are too few:
    auto actual_places = value.end() - point;
    if (actual_places < decimal_places)
        value.append(decimal_places - actual_places, '0');

    // Remove digits from the end if there are too many:
    bool round = false;
    if (actual_places > decimal_places)
    {
        auto end = point + decimal_places;
        round = !std::all_of(end, value.end(), char_is<'0'>);
        value.erase(end, value.end());
    }
    if (strict && round)
        return false;

    // Convert to an integer:
    std::istringstream stream(value);
    uint64_t number = 0;
    if (value.size() && !(stream >> number))
        return false;

    // Round and return:
    if (round && number == max_uint64)
        return false;
    out = number + round;
    return true;
}

std::string encode_base10(uint64_t amount, uint8_t decimal_places)
{
    std::ostringstream stream;
    stream << std::setfill('0') << std::setw(1 + decimal_places) << amount;

    auto string = stream.str();
    string.insert(string.size() - decimal_places, 1, '.');
    boost::algorithm::trim_right_if(string, char_is<'0'>);
    boost::algorithm::trim_right_if(string, char_is<'.'>);
    return string;
}

} // namespace libbitcoin
