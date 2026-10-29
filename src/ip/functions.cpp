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
#include <bitcoin/system/ip/functions.hpp>

#include <bitcoin/system/define.hpp>
#include <bitcoin/system/endian/endian.hpp>
#include <bitcoin/system/ip/address.hpp>
#include <bitcoin/system/ip/ipv4.hpp>
#include <bitcoin/system/ip/ipv6.hpp>
#include <bitcoin/system/math/math.hpp>

namespace libbitcoin {
namespace system {
namespace ip {

constexpr auto maximum_cidr_ip4 = bits<uint32_t>;
constexpr auto maximum_cidr_ip6 = to_bits(ipv6::size);

// Member if subnet addresses contain host.
// ----------------------------------------------------------------------------

// Hosts exclude the network and broadcast addresses, unless cidr is 32.
static bool is_member(const ipv4& ip, const ipv4& net, uint8_t cidr) NOEXCEPT
{
    if (cidr > maximum_cidr_ip4)
        return false;

    if (cidr == maximum_cidr_ip4)
        return ip == net;

    const auto host = from_big_endian(ip.to_bytes());
    const auto mask = unmask_left<uint32_t>(cidr);
    const auto network = bit_and(from_big_endian(net.to_bytes()), mask);
    const auto broadcast = bit_or(network, bit_not(mask));
    return bit_and(host, mask) == network && host != network &&
        host != broadcast;
}

// Hosts include all addresses of the network, cidr zero is not a network.
static bool is_member(const ipv6& ip, const ipv6& net, uint8_t cidr) NOEXCEPT
{
    if (is_zero(cidr) || cidr > maximum_cidr_ip6)
        return false;

    const auto& host = ip.to_bytes();
    const auto& network = net.to_bytes();
    for (size_t index = 0; index < ipv6::size; ++index)
    {
        const auto prefix = to_bits(index);
        const auto masked = cidr > prefix ? cidr - prefix : zero;
        const auto mask = unmask_left<uint8_t>(std::min<size_t>(masked, byte_bits));

        BC_PUSH_WARNING(NO_ARRAY_INDEXING)
        if (bit_and(host[index], mask) != bit_and(network[index], mask))
            return false;
        BC_POP_WARNING()
    }

    return true;
}

// This assumes host and/or subnet v4 address(s) unmapped.
bool is_member(const address& ip, const address& subnet, uint8_t cidr) NOEXCEPT
{
    if (ip.is_v4() && subnet.is_v4())
        return is_member(ip.to_v4(), subnet.to_v4(), cidr);

    if (ip.is_v6() && subnet.is_v6())
        return is_member(ip.to_v6(), subnet.to_v6(), cidr);

    return false;
}

// Mapping.
// ----------------------------------------------------------------------------

// Convert IPv6-mapped to IPV4 (ensures consistent internal matching).
// Reduce 4 encodings (IPv6, IPv6-mapped, IPv6-compat, IPv4) to 2 (IPv6, IPv4).
address denormalize(const address& ip) NOEXCEPT
{
    if (ip.is_v6() && ip.to_v6().is_v4_mapped())
        return { ip.to_v6().to_v4() };

    return ip;
}

// Convert IPv4 to IPv6-mapped (p2p message encoding).
ipv6 normalize(const address& ip) NOEXCEPT
{
    return ip.is_v6() ? ip.to_v6() : ipv6::mapped(ip.to_v4());
}

// Text.
// ----------------------------------------------------------------------------

// Serialize to host denormal form (unmapped) without ipv6 backets.
std::string to_host(const address& ip) NOEXCEPT
{
    return denormalize(ip).to_string();
}

// Serialize to host denormal form (unmapped) and with ipv6 backets.
std::string to_literal(const address& ip) NOEXCEPT
{
    BC_PUSH_WARNING(NO_THROW_IN_NOEXCEPT)
    const auto host = to_host(ip);
    return (host.find(":") == std::string::npos) ? host : ("[" + host + "]");
    BC_POP_WARNING()
}

} // namespace ip
} // namespace system
} // namespace libbitcoin
