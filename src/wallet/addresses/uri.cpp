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

#include <algorithm>
#include <ranges>
#include <bitcoin/system/unicode/unicode.hpp>

namespace libbitcoin {
namespace system {
namespace wallet {

using namespace boost::urls;

bool uri::decode(const std::string& encoded) NOEXCEPT
{
    if (!encoded.empty())
    {
        try
        {
            // throw std::length_error only for s.size() > url_view::max_size.
            if (const auto parts = parse_uri_reference(encoded))
            {
                url_ = { parts.value() };
                return true;
            }
        }
        catch (...)
        {
        }
    }

    url_.clear();
    return false;
}

std::string uri::encoded() const NOEXCEPT
{
    // String allocation exception only.
    BC_PUSH_WARNING(NO_THROW_IN_NOEXCEPT)
    return url_.buffer();
    BC_POP_WARNING()
}

std::string uri::scheme() const NOEXCEPT
{
    // String allocation exception only.
    BC_PUSH_WARNING(NO_THROW_IN_NOEXCEPT)
    return system::ascii_to_lower(url_.scheme());
    BC_POP_WARNING()
}

bool uri::has_scheme() const NOEXCEPT
{
    return url_.has_scheme();
}

bool uri::set_scheme(const std::string& scheme) NOEXCEPT
{
    try
    {
        url_.set_scheme(scheme);
        return true;
    }
    catch (...)
    {
        return false;
    }
}

std::string uri::authority() const NOEXCEPT
{
    // String allocation exception only.
    BC_PUSH_WARNING(NO_THROW_IN_NOEXCEPT)
    return url_.authority().buffer();
    BC_POP_WARNING()
}

bool uri::has_authority() const NOEXCEPT
{
    return url_.has_authority();
}

bool uri::set_authority(const std::string& authority) NOEXCEPT
{
    try
    {
        url_.set_encoded_authority(authority);
        return true;
    }
    catch (...)
    {
        return false;
    }
}

void uri::remove_authority() NOEXCEPT
{
    // Documented to throw nothing, but not noexcept.
    // original.boost.org/doc/libs/1_86_0/libs/url/doc/html/url/ref/
    // boost__urls__url/remove_authority.html
    BC_PUSH_WARNING(NO_THROW_IN_NOEXCEPT)
    url_.remove_authority();
    BC_POP_WARNING()
}

std::string uri::path() const NOEXCEPT
{
    // String allocation exception only.
    BC_PUSH_WARNING(NO_THROW_IN_NOEXCEPT)
    return url_.path();
    BC_POP_WARNING()
}

bool uri::set_path(const std::string& path) NOEXCEPT
{
    try
    {
        url_.set_path(path);
        return true;
    }
    catch (...)
    {
        return false;
    }
}

std::string uri::query() const NOEXCEPT
{
    // String allocation exception only.
    BC_PUSH_WARNING(NO_THROW_IN_NOEXCEPT)
    return url_.query();
    BC_POP_WARNING()
}

bool uri::has_query() const NOEXCEPT
{
    return url_.has_query();
}

bool uri::set_query(const std::string& query) NOEXCEPT
{
    try
    {
        url_.set_query(query);
        return true;
    }
    catch (...)
    {
        return false;
    }
}

void uri::remove_query() NOEXCEPT
{
    url_.remove_query();
}

std::string uri::fragment() const NOEXCEPT
{
    // String allocation exception only.
    BC_PUSH_WARNING(NO_THROW_IN_NOEXCEPT)
    return url_.fragment();
    BC_POP_WARNING()
}

bool uri::has_fragment() const NOEXCEPT
{
    return url_.has_fragment();
}

bool uri::set_fragment(const std::string& fragment) NOEXCEPT
{
    try
    {
        url_.set_fragment(fragment);
        return true;
    }
    catch (...)
    {
        return false;
    }
}

void uri::remove_fragment() NOEXCEPT
{
    url_.remove_fragment();
}

uri::query_map uri::decode_query() const NOEXCEPT
{
    query_map out{};

    // Allocation exceptions only.
    // Last value wins for duplicate keys.
    BC_PUSH_WARNING(NO_THROW_IN_NOEXCEPT)
    for (const auto& parameter: url_.params())
        out[parameter.key] = parameter.value;
    BC_POP_WARNING()
    return out;
}

void uri::encode_query(const query_map& map) NOEXCEPT
{
    url_.params().clear();

    // Allocation exceptions only.
    BC_PUSH_WARNING(NO_THROW_IN_NOEXCEPT)
    for (const auto& parameter: map)
        url_.params().append({ parameter.first, parameter.second });
    BC_POP_WARNING()
}

} // namespace wallet
} // namespace system
} // namespace libbitcoin
