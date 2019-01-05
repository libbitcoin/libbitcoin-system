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
#ifndef LIBBITCOIN_SYSTEM_STRING_IPP
#define LIBBITCOIN_SYSTEM_STRING_IPP

#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string/trim.hpp>

namespace libbitcoin {
namespace system {

template <typename Value>
bool deserialize(Value& out_value, const std::string& text, bool trim)
{
    try
    {
        if (trim)
            out_value = boost::lexical_cast<Value>(boost::trim_copy(text));
        else
            out_value = boost::lexical_cast<Value>(text);
    }
    catch (const boost::bad_lexical_cast&)
    {
        return false;
    }

    return true;
}

template <typename Value>
bool deserialize(std::vector<Value>& out_collection, const std::string& text,
    bool trim)
{
    // This had problems with the inclusion of the ideographic (CJK) space
    // (0xe3, 0x80, 0x80). Need to infuse the local in bc::system::split().
    const auto tokens = split(text, " \n\r\t");
    for (const auto& token: tokens)
    {
        Value value;
        if (!deserialize(value, token, trim))
            return false;

        out_collection.push_back(value);
    }

    return true;
}

template <typename Value>
std::string serialize(const Value& out_value, const std::string& fallback)
{
    std::stringstream stream;
    stream << out_value;
    const auto& text = stream.str();
    return text.empty() ? fallback : text;
}

} // namespace system
} // namespace libbitcoin

#endif
