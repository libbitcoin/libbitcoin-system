/**
 * Copyright (c) 2011-2025 libbitcoin developers (see AUTHORS)
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
#include <bitcoin/system/wallet/addresses/uri.hpp>

#include <iomanip>
#include <iterator>
#include <ranges>
#include <sstream>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/radix/radix.hpp>
#include <bitcoin/system/unicode/unicode.hpp>

namespace libbitcoin {
namespace system {
namespace wallet {

BC_PUSH_WARNING(NO_THROW_IN_NOEXCEPT)

// These character classification functions correspond to RFC 3986.
// They avoid C standard library character classification functions,
// since those give different answers based on the current locale.

constexpr char path_character[]
{
    '-',
    '.',
    '_',
    '~',
    '!',
    '$',
    '&',
    '\'',
    '(',
    ')',
    '*',
    '+',
    ',',
    ';',
    '=',
    ':',
    '@'
};

constexpr bool is_path_character(const char point) NOEXCEPT
{
    return is_ascii_alphanumeric(point) || std::ranges::any_of(path_character,
        [=](auto path) NOEXCEPT
        {
            return path == point;
        });
}

constexpr bool is_path(const char point) NOEXCEPT
{
    return is_path_character(point) || point == '/';
}

constexpr bool is_scheme(const char point) NOEXCEPT
{
    return is_ascii_alphanumeric(point) ||
        point == '+' || point == '-' || point == '.';
}

constexpr bool is_query(const char point) NOEXCEPT
{
    return is_path_character(point) || point == '/' || point == '?';
}

constexpr bool is_query_character(const char point) NOEXCEPT
{
    return is_query(point) && point != '&' && point != '=';
}

// Verifies that all RFC 3986 escape sequences in a string are valid, and that
// all characters belong to the given class.
static bool validate(const std::string& in,
    bool (*is_valid)(const char)) NOEXCEPT
{
    for (auto it = in.begin(); it != in.end();)
    {
        if (*it == '%')
        {
            // If not octet.
            if (!((std::distance(it, in.end()) > 2) &&
                is_base16(it[1]) && is_base16(it[2])))
            {
                return false;
            }

            std::advance(it, 3);
        }
        else
        {
            if (!is_valid(*it))
                return false;

            std::advance(it, 1);
        }
    }

    return true;
}

// Decodes all RFC 3986 escape sequences in a string.
static std::string unescape(const std::string& in) NOEXCEPT
{
    // Do the conversion:
    std::string out;
    out.reserve(in.size());

    for (auto it = in.begin(); it != in.end();)
    {
        // If % and is octet.
        if ((*it == '%') && (std::distance(it, in.end()) > 2) &&
            is_base16(it[1]) && is_base16(it[2]))
        {
            out.push_back(encode_octet({ it[1], it[2], '\0' }));
            std::advance(it, 3);
        }
        else
        {
            out.push_back(*it);
            std::advance(it, 1);
        }
    }

    return out;
}

// URI encodes a string (i.e. percent encoding).
// is_valid a function returning true for acceptable characters.
static std::string escape(const std::string& in,
    bool (*is_valid)(char)) NOEXCEPT
{
    std::ostringstream stream;
    stream << std::hex << std::uppercase << std::setfill('0');
    for (const auto character: in)
    {
        if (is_valid(character))
            stream << character;
        else
            stream << '%' << std::setw(2) << +character;
    }

    return stream.str();
}

bool uri::decode(const std::string& encoded, bool strict) NOEXCEPT
{
    auto it = encoded.begin();

    // Store the scheme:
    auto start = it;
    while (it != encoded.end() && *it != ':')
        ++it;

    scheme_ = std::string(start, it);
    if (scheme_.empty() || !is_ascii_alpha(scheme_[0]))
        return false;

    if (!std::all_of(scheme_.begin(), scheme_.end(), is_scheme))
        return false;

    // Consume ':':
    if (it == encoded.end())
        return false;

    ++it;

    // Consume "//":
    authority_.clear();
    has_authority_ = false;
    if (std::distance(it, encoded.end()) > 1 && it[0] == '/' && it[1] == '/')
    {
        has_authority_ = true;
        it += 2;

        // Store authority part:
        start = it;
        while (it != encoded.end() && *it != '#' && *it != '?' && *it != '/')
            ++it;

        authority_ = std::string(start, it);
        if (strict && !validate(authority_, is_path_character))
            return false;
    }

    // Store the path part:
    start = it;
    while (it != encoded.end() && *it != '#' && *it != '?')
        ++it;

    path_ = std::string(start, it);
    if (strict && !validate(path_, is_path))
        return false;

    // Consume '?':
    has_query_ = false;
    if (it != encoded.end() && *it != '#')
    {
        has_query_ = true;
        ++it;
    }

    // Store the query part:
    start = it;
    while (it != encoded.end() && *it != '#')
        ++it;

    query_ = std::string(start, it);
    if (strict && !validate(query_, is_query))
        return false;

    // Consume '#':
    has_fragment_ = false;
    if (encoded.end() != it)
    {
        has_fragment_ = true;
        ++it;
    }

    // Store the fragment part:
    fragment_ = std::string(it, encoded.end());
    return !strict || validate(fragment_, is_query);
}

std::string uri::encoded() const NOEXCEPT
{
    std::ostringstream out;
    out << scheme_ << ':';
    if (has_authority_)
        out << "//" << authority_;

    out << path_;
    if (has_query_)
        out << '?' << query_;

    if (has_fragment_)
        out << '#' << fragment_;

    return out.str();
}

// Scheme accessors:

std::string uri::scheme() const NOEXCEPT
{
    return ascii_to_lower(scheme_);
}

void uri::set_scheme(const std::string& scheme) NOEXCEPT
{
    scheme_ = scheme;
}

// Authority accessors:

std::string uri::authority() const NOEXCEPT
{
    return unescape(authority_);
}

bool uri::has_authority() const NOEXCEPT
{
    return has_authority_;
}

void uri::set_authority(const std::string& authority) NOEXCEPT
{
    has_authority_ = true;
    authority_ = escape(authority, is_path_character);
}

void uri::remove_authority() NOEXCEPT
{
    has_authority_ = false;
}

// Path accessors:

std::string uri::path() const NOEXCEPT
{
    return unescape(path_);
}

void uri::set_path(const std::string& path) NOEXCEPT
{
    path_ = escape(path, is_path);
}

// Query accessors:

std::string uri::query() const NOEXCEPT
{
    return unescape(query_);
}

bool uri::has_query() const NOEXCEPT
{
    return has_query_;
}

void uri::set_query(const std::string& query) NOEXCEPT
{
    has_query_ = true;
    query_ = escape(query, is_query);
}

void uri::remove_query() NOEXCEPT
{
    has_query_ = false;
}

// Fragment accessors:

std::string uri::fragment() const NOEXCEPT
{
    return unescape(fragment_);
}

bool uri::has_fragment() const NOEXCEPT
{
    return has_fragment_;
}

void uri::set_fragment(const std::string& fragment) NOEXCEPT
{
    has_fragment_ = true;
    fragment_ = escape(fragment, is_query);
}

void uri::remove_fragment() NOEXCEPT
{
    has_fragment_ = false;
}

// Query interpretation:

uri::query_map uri::decode_query() const NOEXCEPT
{
    query_map out;
    for (auto it = query_.begin(); it != query_.end();)
    {
        // Read the key:
        auto begin = it;
        while (it != query_.end() && *it != '&' && *it != '=')
            ++it;

        auto key = unescape(std::string(begin, it));

        // Consume '=':
        if (it != query_.end() && *it != '&')
            ++it;

        // Read the value:
        begin = it;
        while (it != query_.end() && *it != '&')
            ++it;

        out[key] = unescape(std::string(begin, it));

        // Consume '&':
        if (query_.end() != it)
            ++it;
    }

    return out;
}

void uri::encode_query(const query_map& map) NOEXCEPT
{
    auto first = true;
    std::ostringstream query;
    for (const auto& term: map)
    {
        if (!first)
            query << '&';

        first = false;
        query << escape(term.first, is_query_character);
        if (!term.second.empty())
            query << '=' << escape(term.second, is_query_character);
    }

    has_query_ = !map.empty();
    query_ = query.str();
}

BC_POP_WARNING()

} // namespace wallet
} // namespace system
} // namespace libbitcoin
