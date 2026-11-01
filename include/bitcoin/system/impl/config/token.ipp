/**
 * Copyright (c) 2011-2026 libbitcoin developers
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
#ifndef LIBBITCOIN_SYSTEM_IMPL_CONFIG_TOKEN_IPP
#define LIBBITCOIN_SYSTEM_IMPL_CONFIG_TOKEN_IPP

#include <sstream>
#include <bitcoin/system/define.hpp>

namespace libbitcoin {
namespace system {
namespace config {

TEMPLATE
constexpr std::string_view CLASS::to_word(type value) NOEXCEPT
{
    auto index = zero;
    for (const auto& word: words)
    {
        if (static_cast<type>(index) == value)
            return word;

        ++index;
    }

    return {};
}

TEMPLATE
constexpr bool CLASS::from_word(type& out, std::string_view word) NOEXCEPT
{
    if (word.empty())
        return false;

    auto index = zero;
    for (const auto& candidate: words)
    {
        if (candidate == word)
        {
            out = static_cast<type>(index);
            return true;
        }

        ++index;
    }

    return false;
}

TEMPLATE
std::string CLASS::names() NOEXCEPT
{
    BC_PUSH_WARNING(NO_THROW_IN_NOEXCEPT)
    std::string out{};
    for (const auto& word: words)
    {
        if (word.empty())
            continue;

        if (!out.empty())
            out += '|';

        out += word;
    }

    return out;
    BC_POP_WARNING()
}

TEMPLATE
CLASS::token() NOEXCEPT
  : value_{}
{
}

TEMPLATE
CLASS::token(type value) NOEXCEPT
  : value_(value)
{
}

TEMPLATE
CLASS::token(const std::string& value) THROWS
  : token()
{
    std::istringstream(value) >> *this;
}

TEMPLATE
CLASS::operator type() const NOEXCEPT
{
    return value_;
}

} // namespace config
} // namespace system
} // namespace libbitcoin

#endif
