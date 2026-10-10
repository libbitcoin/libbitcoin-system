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
#include <bitcoin/system/wallet/addresses/uri.hpp>

#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/math/math.hpp>
#include <bitcoin/system/radix/radix.hpp>
#include <bitcoin/system/unicode/unicode.hpp>

namespace libbitcoin {
namespace system {
namespace wallet {

constexpr auto npos = std::string_view::npos;

// RFC 3986 character classes.
// ----------------------------------------------------------------------------

static constexpr bool is_alpha(char character) NOEXCEPT
{
    return is_between(character, 'a', 'z') || is_between(character, 'A', 'Z');
}

static constexpr bool is_digit(char character) NOEXCEPT
{
    return is_between(character, '0', '9');
}

static constexpr bool is_hex(char character) NOEXCEPT
{
    return is_base16(character);
}

static constexpr bool is_unreserved(char character) NOEXCEPT
{
    return is_alpha(character) || is_digit(character) || character == '-' ||
        character == '.' || character == '_' || character == '~';
}

static constexpr bool is_sub_delimiter(char character) NOEXCEPT
{
    switch (character)
    {
        case '!': case '$': case '&': case '\'': case '(': case ')':
        case '*': case '+': case ',': case ';': case '=':
            return true;
        default:
            return false;
    }
}

static constexpr bool is_scheme_character(char character) NOEXCEPT
{
    return is_alpha(character) || is_digit(character) || character == '+' ||
        character == '-' || character == '.';
}

static constexpr bool is_userinfo_character(char character) NOEXCEPT
{
    return is_unreserved(character) || is_sub_delimiter(character) ||
        character == ':';
}

static constexpr bool is_host_character(char character) NOEXCEPT
{
    return is_unreserved(character) || is_sub_delimiter(character);
}

static constexpr bool is_path_character(char character) NOEXCEPT
{
    return is_unreserved(character) || is_sub_delimiter(character) ||
        character == ':' || character == '@' || character == '/';
}

static constexpr bool is_query_character(char character) NOEXCEPT
{
    return is_path_character(character) || character == '?';
}

static constexpr bool is_fragment_character(char character) NOEXCEPT
{
    return is_query_character(character);
}

static constexpr bool is_parameter_character(char character) NOEXCEPT
{
    return is_query_character(character) && character != '&' &&
        character != '=' && character != '+';
}

// Percent encoding.
// ----------------------------------------------------------------------------

template <typename Allowed>
static bool is_encoded(const std::string_view& text, Allowed allowed) NOEXCEPT
{
    for (auto it = text.cbegin(); it != text.cend(); ++it)
    {
        if (*it == '%')
        {
            if (std::distance(it, text.cend()) <= 2 ||
                !is_hex(*++it) || !is_hex(*++it))
                return false;
        }
        else if (!allowed(*it))
        {
            return false;
        }
    }

    return true;
}

template <typename Allowed>
static std::string encode(const std::string_view& text, Allowed allowed) NOEXCEPT
{
    constexpr std::string_view digits{ "0123456789ABCDEF" };

    std::string out{};
    out.reserve(text.size());
    for (const auto character: text)
    {
        if (allowed(character))
        {
            out.push_back(character);
        }
        else
        {
            const auto byte = possible_sign_cast<uint8_t>(character);
            out.push_back('%');
            out.push_back(digits[byte / 16u]);
            out.push_back(digits[byte % 16u]);
        }
    }

    return out;
}

// Input must be valid (is_encoded).
static std::string decode(const std::string_view& text,
    bool plus_is_space=false) NOEXCEPT
{
    std::string out{};
    out.reserve(text.size());
    for (auto it = text.cbegin(); it != text.cend(); ++it)
    {
        if (*it == '%' && std::distance(it, text.cend()) > 2)
        {
            const char octet[]{ *++it, *++it, '\0' };
            out.push_back(possible_sign_cast<char>(encode_octet(octet)));
        }
        else
        {
            out.push_back(plus_is_space && *it == '+' ? ' ' : *it);
        }
    }

    return out;
}

// Component grammar.
// ----------------------------------------------------------------------------

static bool is_scheme(const std::string_view& text) NOEXCEPT
{
    return !text.empty() && is_alpha(text.front()) &&
        std::all_of(std::next(text.cbegin()), text.cend(), is_scheme_character);
}

static bool is_decimal_octet(const std::string_view& text) NOEXCEPT
{
    if (text.empty() || text.size() > 3 ||
        !std::all_of(text.cbegin(), text.cend(), is_digit))
        return false;

    if (text.size() > 1 && text.front() == '0')
        return false;

    return text.size() < 3 || text <= "255";
}

static bool is_ipv4(std::string_view text) NOEXCEPT
{
    for (size_t octets{ 1 }; ; ++octets)
    {
        const auto dot = text.find('.');
        if (!is_decimal_octet(text.substr(0, dot)))
            return false;

        if (dot == npos)
            return octets == 4u;

        text.remove_prefix(add1(dot));
    }
}

// Counts the 16 bit groups of a colon-separated list (ipv4 tail counts two).
static bool count_groups(size_t& count, std::string_view text,
    bool ipv4_tail) NOEXCEPT
{
    count = zero;
    if (text.empty())
        return true;

    while (true)
    {
        const auto colon = text.find(':');
        const auto group = text.substr(0, colon);
        if (colon == npos && ipv4_tail && group.find('.') != npos)
        {
            count += 2u;
            return is_ipv4(group);
        }

        if (group.empty() || group.size() > 4 ||
            !std::all_of(group.cbegin(), group.cend(), is_hex))
            return false;

        ++count;
        if (colon == npos)
            return true;

        text.remove_prefix(add1(colon));
    }
}

static bool is_ipv6(const std::string_view& text) NOEXCEPT
{
    size_t head{}, tail{};
    const auto elision = text.find("::");
    if (elision == npos)
        return count_groups(head, text, true) && head == 8u;

    const auto right = text.substr(elision + 2u);
    return right.find("::") == npos &&
        count_groups(head, text.substr(0, elision), false) &&
        count_groups(tail, right, true) && head + tail < 8u;
}

static bool is_ipvfuture(const std::string_view& text) NOEXCEPT
{
    if (text.empty() || (text.front() != 'v' && text.front() != 'V'))
        return false;

    const auto dot = text.find('.');
    if (dot == npos || dot < 2u)
        return false;

    const auto version = text.substr(1, sub1(dot));
    const auto address = text.substr(add1(dot));
    return !address.empty() &&
        std::all_of(version.cbegin(), version.cend(), is_hex) &&
        std::all_of(address.cbegin(), address.cend(), is_userinfo_character);
}

static bool is_authority(std::string_view text) NOEXCEPT
{
    const auto at = text.find('@');
    if (at != npos)
    {
        if (!is_encoded(text.substr(0, at), is_userinfo_character))
            return false;

        text.remove_prefix(add1(at));
    }

    std::string_view port{};
    if (text.starts_with('['))
    {
        const auto close = text.find(']');
        if (close == npos)
            return false;

        const auto literal = text.substr(1, sub1(close));
        if (!is_ipv6(literal) && !is_ipvfuture(literal))
            return false;

        const auto rest = text.substr(add1(close));
        if (!rest.empty())
        {
            if (!rest.starts_with(':'))
                return false;

            port = rest.substr(1);
        }
    }
    else
    {
        const auto colon = text.rfind(':');
        if (colon != npos)
        {
            port = text.substr(add1(colon));
            text = text.substr(0, colon);
        }

        if (!is_encoded(text, is_host_character))
            return false;
    }

    return std::all_of(port.cbegin(), port.cend(), is_digit);
}

static std::string escape_colons(const std::string_view& text) NOEXCEPT
{
    std::string out{};
    for (const auto character: text)
    {
        if (character == ':')
            out += "%3A";
        else
            out.push_back(character);
    }

    return out;
}

// uri
// ----------------------------------------------------------------------------

bool uri::decode(const std::string& encoded) NOEXCEPT
{
    *this = {};
    if (encoded.empty())
        return false;

    uri out{};
    std::string_view text{ encoded };
    const auto delimiter = text.find_first_of(":/?#");
    if (delimiter != npos && text.at(delimiter) == ':')
    {
        const auto scheme = text.substr(0, delimiter);
        if (!is_scheme(scheme))
            return false;

        out.scheme_ = scheme;
        text.remove_prefix(add1(delimiter));
    }

    if (text.starts_with("//"))
    {
        text.remove_prefix(2);
        const auto authority = text.substr(0, text.find_first_of("/?#"));
        if (!is_authority(authority))
            return false;

        out.authority_ = authority;
        out.has_authority_ = true;
        text.remove_prefix(authority.size());
    }

    const auto path = text.substr(0, text.find_first_of("?#"));
    if (!is_encoded(path, is_path_character))
        return false;

    out.path_ = path;
    text.remove_prefix(path.size());

    if (text.starts_with('?'))
    {
        text.remove_prefix(1);
        const auto query = text.substr(0, text.find('#'));
        if (!is_encoded(query, is_query_character))
            return false;

        out.query_ = query;
        out.has_query_ = true;
        text.remove_prefix(query.size());
    }

    if (text.starts_with('#'))
    {
        text.remove_prefix(1);
        if (!is_encoded(text, is_fragment_character))
            return false;

        out.fragment_ = text;
        out.has_fragment_ = true;
    }

    *this = std::move(out);
    return true;
}

std::string uri::encoded() const NOEXCEPT
{
    std::string out{};
    if (has_scheme())
        out += scheme_ + ":";

    if (has_authority_)
    {
        out += "//" + authority_;
        if (!path_.empty() && !path_.starts_with('/'))
            out += "/";
    }
    else if (path_.starts_with("//"))
    {
        out += "/.";
    }

    if (!has_scheme() && !has_authority_)
    {
        const auto slash = path_.find('/');
        out += escape_colons(std::string_view{ path_ }.substr(0, slash));
        if (slash != npos)
            out += path_.substr(slash);
    }
    else
    {
        out += path_;
    }

    if (has_query_)
        out += "?" + query_;

    if (has_fragment_)
        out += "#" + fragment_;

    return out;
}

std::string uri::scheme() const NOEXCEPT
{
    return ascii_to_lower(scheme_);
}

bool uri::has_scheme() const NOEXCEPT
{
    return !scheme_.empty();
}

bool uri::set_scheme(const std::string& scheme) NOEXCEPT
{
    if (!scheme.empty() && !is_scheme(scheme))
        return false;

    scheme_ = scheme;
    return true;
}

std::string uri::authority() const NOEXCEPT
{
    return authority_;
}

bool uri::has_authority() const NOEXCEPT
{
    return has_authority_;
}

bool uri::set_authority(const std::string& authority) NOEXCEPT
{
    if (!is_authority(authority))
        return false;

    authority_ = authority;
    has_authority_ = true;
    return true;
}

void uri::remove_authority() NOEXCEPT
{
    authority_.clear();
    has_authority_ = false;
}

std::string uri::path() const NOEXCEPT
{
    return wallet::decode(path_);
}

bool uri::set_path(const std::string& path) NOEXCEPT
{
    path_ = encode(path, is_path_character);
    return true;
}

std::string uri::query() const NOEXCEPT
{
    return wallet::decode(query_);
}

bool uri::has_query() const NOEXCEPT
{
    return has_query_;
}

bool uri::set_query(const std::string& query) NOEXCEPT
{
    query_ = encode(query, is_query_character);
    has_query_ = true;
    return true;
}

void uri::remove_query() NOEXCEPT
{
    query_.clear();
    has_query_ = false;
}

std::string uri::fragment() const NOEXCEPT
{
    return wallet::decode(fragment_);
}

bool uri::has_fragment() const NOEXCEPT
{
    return has_fragment_;
}

bool uri::set_fragment(const std::string& fragment) NOEXCEPT
{
    fragment_ = encode(fragment, is_fragment_character);
    has_fragment_ = true;
    return true;
}

void uri::remove_fragment() NOEXCEPT
{
    fragment_.clear();
    has_fragment_ = false;
}

uri::query_map uri::decode_query() const NOEXCEPT
{
    query_map out{};
    if (!has_query_)
        return out;

    // Last value wins for duplicate keys.
    for (const auto& parameter: split(query_, "&", false, false))
    {
        const auto equals = parameter.find('=');
        const auto key = std::string_view{ parameter }.substr(0, equals);
        const auto value = equals == npos ? std::string_view{} :
            std::string_view{ parameter }.substr(add1(equals));

        out[wallet::decode(key, true)] = wallet::decode(value, true);
    }

    return out;
}

void uri::encode_query(const query_map& map) NOEXCEPT
{
    if (map.empty())
    {
        remove_query();
        return;
    }

    string_list parameters{};
    parameters.reserve(map.size());
    for (const auto& parameter: map)
        parameters.push_back(encode(parameter.first, is_parameter_character) +
            "=" + encode(parameter.second, is_parameter_character));

    query_ = join(parameters, "&");
    has_query_ = true;
}

} // namespace wallet
} // namespace system
} // namespace libbitcoin
