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
#ifndef LIBBITCOIN__CONFIG_ENDPOINT_HPP
#define LIBBITCOIN__CONFIG_ENDPOINT_HPP

#include <cstdint>
#include <iostream>
#include <string>
#include <vector>
#include <boost/asio.hpp>
#include <bitcoin/config_authority.hpp>
#include <bitcoin/utility_address.hpp>
#include <bitcoin/utility_endpoint.hpp>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/config/endpoint.hpp>
#include <bitcoin/bitcoin/formats/base_16.hpp>

namespace libbitcoin {
//namespace config {
namespace api {

/**
 * Serialization helper for a network endpoint in URI format.
 * This is a container for a {scheme, host, port} tuple.
 */
class BC_API config_endpoint
{
public:
    /**
     * A list of endpoints.
     * This must provide operator<< for ostream in order to be used as a
     * boost::program_options default_value.
     */
//    typedef std::vector<endpoint> list;

    /**
     * Default constructor.
     */
    config_endpoint();

    /**
     * Copy constructor.
     * @param[in]  other  The object to copy into self on construct.
     */
    config_endpoint(const config_endpoint& other);

    /**
     * Initialization constructor.
     * The scheme and port may be undefined, in which case the port is reported
     * as zero and the scheme is reported as an empty string.
     * @param[in]  value  The initial value of the [scheme://]host[:port] form.
     */
    config_endpoint(const std::string& value);

    /**
     * Initialization constructor.
     * @param[in]  authority  The value to initialize with.
     */
    config_endpoint(const config_authority& authority);

    /**
     * Initialization constructor.
     * @param[in]  host  The host name or ip address to initialize with.
     * @param[in]  port  The port to initialize with.
     */
    config_endpoint(const std::string& host, uint16_t port);

    /**
     * Initialization constructor.
     * @param[in]  scheme  The scheme to initialize with.
     * @param[in]  host    The host name or ip address to initialize with.
     * @param[in]  port    The port to initialize with.
     */
    config_endpoint(const std::string& scheme, const std::string& host, uint16_t port);

    /**
     * Initialization constructor.
     * @param[in]  endpoint  The endpoint addresss to initialize with.
     */
    config_endpoint(const libbitcoin::api::utility_endpoint& host);

    /**
     * Initialization constructor.
     * @param[in]  ip    The boost ip addresss to initialize with.
     * @param[in]  port  The port to initialize with.
     */
    config_endpoint(const libbitcoin::api::utility_address& ip, uint16_t port);

    /**
     * Getter.
     * @return True if the endpoint is initialized.
     */
//    operator bool() const;
    bool toBoolean() const;

    /**
     * Getter.
     * @return The scheme of the endpoint or empty string.
     */
    const std::string& scheme() const;

    /**
     * Getter.
     * @return The host name or ip address of the endpoint.
     */
    const std::string& host() const;

    /**
     * Getter.
     * @return The tcp port of the endpoint.
     */
    uint16_t port() const;

    /**
     * Get the endpoint as a string.
     * An empty scheme and/or empty port is omitted.
     * @return The endpoint in the [scheme://]host[:port] form.
     */
    std::string to_string() const;

    /**
     * Return a new endpoint that replaces host instances of "*" with
     * "localhost".  This is intended for clients that wish to connect
     * to a service that has been configured to bind to all
     * interfaces.
     * @return The endpoint in the [scheme://]host[:port] form.
     */
    config_endpoint to_local() const;

    /**
     * Override the equality operator.
     * @param[in]  other  The other object with which to compare.
     */
//    bool operator==(const endpoint& other) const;
    bool eq(const config_endpoint& other) const;

    /**
     * Define stream in. Throws if input is invalid.
     * @param[in]   input     The input stream to read the value from.
     * @param[out]  argument  The object to receive the read value.
     * @return                The input stream reference.
     */
//    friend std::istream& operator>>(std::istream& input, endpoint& argument);

    /**
     * Define stream out.
     * @param[in]   output    The output stream to write the value to.
     * @param[out]  argument  The object from which to obtain the value.
     * @return                The output stream reference.
     */
//    friend std::ostream& operator<<(std::ostream& output, const endpoint& argument);

	config::endpoint getValue() {
		return value;
	}

	void setValue(config::endpoint value) {
		this->value = value;
	}
private:
	config::endpoint value;

//private:
//    std::string scheme_;
//    std::string host_;
//    uint16_t port_;
};

} // namespace api
//} // namespace config
} // namespace libbitcoin

#endif
