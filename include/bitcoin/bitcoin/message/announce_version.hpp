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
#ifndef LIBBITCOIN_MESSAGE_ANNOUNCE_VERSION_HPP
#define LIBBITCOIN_MESSAGE_ANNOUNCE_VERSION_HPP

#include <cstdint>
#include <istream>
#include <string>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/message/network_address.hpp>
#include <bitcoin/bitcoin/utility/serializer.hpp>

namespace libbitcoin {
namespace message {

class BC_API announce_version
{
public:

    static const std::string satoshi_command;

    uint32_t version() const;

    void version(uint32_t value);

    uint64_t services() const;

    void services(uint64_t value);

    uint64_t timestamp() const;

    void timestamp(uint64_t value);

    network_address& address_me();

    const network_address& address_me() const;

    void address_me(const network_address& value);

    network_address& address_you();

    const network_address& address_you() const;

    void address_you(const network_address& value);

    uint64_t nonce() const;

    void nonce(uint64_t value);

    std::string& user_agent();

    const std::string& user_agent() const;

    void user_agent(const std::string& value);

    uint32_t start_height() const;

    void start_height(uint32_t value);

    bool from_data(const data_chunk& data);

    bool from_data(std::istream& stream);

    data_chunk to_data() const;

    void reset();

    size_t satoshi_size() const;

private:

    uint32_t version_;
    uint64_t services_;
    uint64_t timestamp_;
    network_address address_me_;
    network_address address_you_;
    uint64_t nonce_;
    std::string user_agent_;
    uint32_t start_height_;
};

} // end message
} // end libbitcoin

#endif
