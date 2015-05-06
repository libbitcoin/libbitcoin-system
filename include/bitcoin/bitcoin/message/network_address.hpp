/*
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
#ifndef LIBBITCOIN_MESSAGE_NETWORK_ADDRESS_HPP
#define LIBBITCOIN_MESSAGE_NETWORK_ADDRESS_HPP

#include <cstdint>
#include <istream>
#include <vector>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/utility/data.hpp>
#include <bitcoin/bitcoin/utility/deserializer.hpp>
#include <bitcoin/bitcoin/utility/serializer.hpp>

namespace libbitcoin {
namespace message {

typedef byte_array<16> ip_address;

class BC_API network_address
{
public:

    network_address();

    network_address(const uint32_t timestamp, const uint64_t services,
        const ip_address& ip, const uint16_t port);

    network_address(std::istream& stream);

    uint32_t timestamp() const;

    void timestamp(uint32_t value);

    uint64_t services() const;

    void services(uint64_t value);

    ip_address& ip();

    const ip_address& ip() const;

    void ip(const ip_address& value);

    uint16_t port() const;

    void port(uint16_t value);

    data_chunk to_data() const;

    size_t satoshi_size() const;

    static size_t satoshi_fixed_size();

private:

    uint32_t timestamp_;
    uint64_t services_;
    ip_address ip_;
    uint16_t port_;
};

typedef std::vector<network_address> network_address_list;

} // end message
} // end libbitcoin

#endif
