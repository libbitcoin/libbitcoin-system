/**
 * Copyright (c) 2011-2015 libbitcoin developers (see AUTHORS)
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
#ifndef LIBBITCOIN_AUTHORITY_HPP
#define LIBBITCOIN_AUTHORITY_HPP

#include <cstdint>
#include <iostream>
#include <string>
#include <vector>
#include <boost/asio.hpp>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/primitives.hpp>

namespace libbitcoin {
namespace config {

/**
 * Serialization helper for a network authority.
 * This is a container for a {ip address, port} tuple.
 */
class BC_API authority
{
public:
    /**
     * A list of authorities.
     */
    typedef std::vector<authority> list;

    /**
     * Default constructor.
     */
    authority();
    
    /**
     * Copy constructor.
     * @param[in]  other  The object to copy into self on construct.
     */
    authority(const authority& other);
    
    /**
     * Initialization constructor.
     * The port is optional and will be set to zero if not provided.
     * @param[in]  value  The initial value in one of two forms:
     *                    a.b.c.d[:port]
     *                    0000::0000::0000::0000::0000::ffff::0a00::0001[:port]
     */
    authority(const std::string& value);
    
    /**
     * Initialization constructor.
     * @param[in]  net  The value to initialize with.
     */
    authority(const network_address_type& net);
    
    /**
     * Initialization constructor.
     * @param[in]  ip    The ip addresss to initialize with.
     * @param[in]  port  The port to initialize with.
     */
    authority(const ip_address_type& ip, uint16_t port);
    
    /**
     * Initialization constructor.
     * @param[in]  endpoint  The boost endpoint addresss to initialize with.
     */
    authority(const boost::asio::ip::tcp::endpoint& endpoint);
    
    /**
     * Initialization constructor.
     * @param[in]  ip    The boost ip addresss to initialize with.
     * @param[in]  port  The port to initialize with.
     */
    authority(const boost::asio::ip::address& ip, uint16_t port);

    /**
     * Getter.
     * @return The ip address of the authority.
     */
    const ip_address_type& ip() const;

    /**
     * Getter.
     * @return The tcp port of the authority.
     */
    uint16_t port() const;
    
    /**
     * Get the ip address of the authority as a string.
     * The form of the return is determined by the type of address.
     * @return The ip address of the authority in one of two forms:
     *         a.b.c.d
     *         0000::0000::0000::0000::0000::ffff::0a00::0001
     */
    std::string to_hostname() const;
    
    /**
     * Get the authority as a string.
     * The form of the return is determined by the type of address.
     * @return The authority in one of two forms:
     *         a.b.c.d[:port]
     *         0000::0000::0000::0000::0000::ffff::0a00::0001[:port]
     */
    std::string to_string() const;

    /**
     * Convert to bitcoin network address type.
     * @return  The authority converted to a network address.
     */
    network_address_type to_network_address() const;

    /**
     * Override the equality operator.
     * @param[in]  other  The other object with which to compare.
     */
    bool operator==(const authority& other) const;

    /**
     * Define stream in. Throws if input is invalid.
     * @param[in]   input     The input stream to read the value from.
     * @param[out]  argument  The object to receive the read value.
     * @return                The input stream reference.
     */
    friend std::istream& operator>>(std::istream& input,
        authority& argument);

    /**
     * Define stream out.
     * @param[in]   output    The output stream to write the value to.
     * @param[out]  argument  The object from which to obtain the value.
     * @return                The output stream reference.
     */
    friend std::ostream& operator<<(std::ostream& output,
        const authority& argument);

private:
    boost::asio::ip::address ip_;
    uint16_t port_;
};

} // namespace config
} // namespace libbitcoin

#endif
