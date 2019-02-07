/**
 * Copyright (c) 2011-2017 libbitcoin developers (see AUTHORS)
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
#ifndef LIBBITCOIN__CONFIG_AUTHORITY_HPP
#define LIBBITCOIN__CONFIG_AUTHORITY_HPP

#include <cstdint>
#include <iostream>
#include <string>
#include <vector>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/config/authority.hpp>
#include <bitcoin/message_ip_address.hpp>
#include <bitcoin/message_network_address.hpp>
#include <bitcoin/utility_address.hpp>
#include <bitcoin/utility_ipv6.hpp>
#include <bitcoin/utility_endpoint.hpp>

namespace libbitcoin {
namespace config {
namespace api {

/**
 * Serialization helper for a network authority.
 * This is a container for a {ip address, port} tuple.
 */
class BC_API config_authority : public authority
{
public:
    /**
     * A list of authorities.
     * This must provide operator<< for ostream in order to be used as a
     * boost::program_options default_value.
     */
//    typedef std::vector<authority> list;

    /**
     * Default constructor.
     */
    config_authority();

    /**
     * Copy constructor.
     * @param[in]  other  The object to copy into self on construct.
     */
    config_authority(const config_authority& other);

    /**
     * Initialization constructor.
     * Deserialize a IPv4 or IPv6 address-based hostname[:port].
     * The port is optional and will be set to zero if not provided.
     * @param[in]  authority  The initial value in one of two forms:
     *                        [2001:db8::2]:port or 1.2.240.1:port
     */
    config_authority(const std::string& authority);

    /**
     * Initialization constructor.
     * @param[in]  net  The network address (ip and port) to initialize with.
     */
    config_authority(const message::api::message_network_address& address);

    /**
     * Initialization constructor.
     * @param[in]  ip    The ip addresss to initialize with.
     * @param[in]  port  The port to initialize with.
     */
    config_authority(const message::api::message_ip_address& ip, uint16_t port);

    /**
     * Initialization constructor.
     * @param[in]  host  The host to initialize with in one of three forms:
     *                   [2001:db8::2] or 2001:db8::2 or 1.2.240.1
     * @param[in]  port  The port to initialize with.
     */
    config_authority(const std::string& host, uint16_t port);

    /**
     * Initialization constructor.
     * @param[in]  ip    The boost ip addresss to initialize with.
     * @param[in]  port  The port to initialize with.
     */
    config_authority(const libbitcoin::api::utility_address& ip, uint16_t port);

    /**
     * Initialization constructor.
     * @param[in]  endpoint  The boost endpoint address to initialize with.
     */
    config_authority(const libbitcoin::api::utility_endpoint& endpoint);

    /**
     * Getter.
     * @return True if the port is non-zero.
     */
    //operator bool() const;
    bool toBoolean() const;

    /**
     * Getter.
     * @return The ip address of the authority.
     */
    libbitcoin::api::utility_ipv6 asio_ip() const;

    /**
     * Getter.
     * @return The ip address of the authority.
     */
    message::api::message_ip_address ip() const;

    /**
     * Getter.
     * @return The tcp port of the authority.
     */
    uint16_t port() const;

    /**
     * Get the hostname of the authority as a string.
     * The form of the return is determined by the type of address.
     * @return The hostname in one of two forms: 2001:db8::2 or 1.2.240.1
     */
    std::string to_hostname() const;

    /**
     * Get the authority as a string.
     * The form of the return is determined by the type of address.
     * The port is optional and not included if zero-valued.
     * @return The authority in one of two forms:
     *         [2001:db8::2]:port or 1.2.240.1:port
     */
    std::string to_string() const;

    /**
     * Convert to bitcoin network address type.
     * @return  The authority converted to a network address.
     */
    message::api::message_network_address to_network_address() const;

    /**
     * Override the equality operator.
     * @param[in]  other  The other object with which to compare.
     */
//    bool operator==(const authority& other) const;
    bool eq(const config_authority& other) const;

    /**
     * Override the inequality operator.
     * @param[in]  other  The other object with which to compare.
     */
//    bool operator!=(const authority& other) const;
//    bool ne(const config_authority& other) const;

    /**
     * Define stream in. Throws if input is invalid.
     * @param[in]   input     The input stream to read the value from.
     * @param[out]  argument  The object to receive the read value.
     * @return                The input stream reference.
     */
//    friend std::istream& operator>>(std::istream& input,
//        authority& argument);

    /**
     * Define stream out.
     * @param[in]   output    The output stream to write the value to.
     * @param[out]  argument  The object from which to obtain the value.
     * @return                The output stream reference.
     */
//    friend std::ostream& operator<<(std::ostream& output,
//        const authority& argument);

//private:
//    asio::ipv6 ip_;
//    uint16_t port_;
};

} // namespace api
} // namespace config
} // namespace libbitcoin

#endif
