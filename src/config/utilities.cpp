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
#include <bitcoin/system/config/utilities.hpp>

#include <charconv>
#include <regex>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/serial/serial.hpp>

namespace libbitcoin {
namespace system {
namespace config {

// Empty converts to zero.
template <typename Integer>
inline bool to_integer(Integer& out, const std::string& in) NOEXCEPT
{
    if (in.empty())
    {
        out = Integer{};
        return true;
    }

    const auto last = std::next(in.data(), in.size());
    const auto [end, ec] = std::from_chars(in.data(), last, out);
    return ec == std::errc{} && end == last;
}

// For calling consistency.
inline bool to_string(std::string& to, std::string&& from) NOEXCEPT
{
    to = std::move(from);
    return true;
}

inline bool make_address(ip::address& out, const std::string& host) NOEXCEPT
{
    return ip::address::from_string(out, trim_copy(host, { "[", "]" }));
}

// regex parsers.
// ----------------------------------------------------------------------------

// en.wikipedia.org/wiki/List_of_URI_schemes
// Schemes of p2p network and our zeromq endpoints.
#define SCHEME "(tcp|udp|http|https|inproc):\\/\\/"
#define IPV4   "([0-9.]+)"
#define IPV6   "\\[([0-9a-f:]+)\\]"
#define HOST   "([^:?/\\\\]+)"
#define PORT   ":([1-9][0-9]{0,4})"
#define CIDR   "\\/([1-9][0-9]{0,2})"

// sregex_iterator doesn't provide `at()`.
BC_PUSH_WARNING(NO_ARRAY_INDEXING)
BC_PUSH_WARNING(NO_THROW_IN_NOEXCEPT)

// Excludes ipv4 mapped/compat and unbracketed ipv6.
static bool parse_host(ip::address& ip, const std::string& value) NOEXCEPT
{
    static const std::regex regular
    {
        "^(" IPV4 "|" IPV6 ")$"
    };

    std::sregex_iterator token{ value.cbegin(), value.cend(), regular };
    std::sregex_iterator end{};
    return token != end
        && make_address(ip, (*token)[1].str());
}

// Excludes ipv4 mapped/compat to ipv6.
bool parse_authority(ip::address& ip, uint16_t& port, uint8_t& cidr,
    const std::string& value) NOEXCEPT
{
    constexpr uint8_t maximum_cidr_ip4 = 32;
    constexpr uint8_t maximum_cidr_ip6 = 128;
    static const std::regex regular
    {
        "^(" IPV4 "|" IPV6 ")(" PORT ")?(" CIDR ")?$"
    };

    std::sregex_iterator token{ value.cbegin(), value.cend(), regular };
    std::sregex_iterator end{};
    return token != end
        && make_address(ip, (*token)[1].str())
        && to_integer(port, (*token)[5].str())
        && to_integer(cidr, (*token)[7].str())
        && ((ip.is_v4() && cidr <= maximum_cidr_ip4) ||
            (ip.is_v6() && cidr <= maximum_cidr_ip6));
}

// Excludes ipv4 mapped/compat to ipv6.
bool parse_url(std::string& scheme, std::string& host, uint16_t& port,
    const std::string& value) NOEXCEPT
{
    static const std::regex regular
    {
        "^(" SCHEME ")?(" IPV4 "|" IPV6 "|" HOST ")(" PORT ")?$"
    };

    std::sregex_iterator token{ value.cbegin(), value.cend(), regular };
    std::sregex_iterator end{};
    return token != end
        && to_string(scheme, (*token)[2].str())
        && to_string(host,   (*token)[3].str())
        && to_integer(port,  (*token)[8].str());
}

// Excludes ipv4 mapped/compat to ipv6.
bool parse_endpoint(std::string& host, uint16_t& port,
    const std::string& value) NOEXCEPT
{
    static const std::regex regular
    {
        "^(" IPV4 "|" IPV6 "|" HOST ")(" PORT ")?$"
    };

    std::sregex_iterator token{ value.cbegin(), value.cend(), regular };
    std::sregex_iterator end{};
    return token != end
        && to_string(host, (*token)[1].str())
        && to_integer(port, (*token)[6].str());
}

BC_POP_WARNING()
BC_POP_WARNING()

// Host conversions.
// ----------------------------------------------------------------------------

// Rejects ipv6 mapped/compat to ipv4 and unbracketed ipv6.
ip::address from_host(const std::string& host) THROWS
{
    ip::address out{};
    if (!parse_host(out, host))
        throw istream_exception{ host };

    return out;
}

} // namespace config
} // namespace system
} // namespace libbitcoin
