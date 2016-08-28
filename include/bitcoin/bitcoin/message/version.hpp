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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef LIBBITCOIN_MESSAGE_ANNOUNCE_VERSION_HPP
#define LIBBITCOIN_MESSAGE_ANNOUNCE_VERSION_HPP

#include <cstdint>
#include <istream>
#include <memory>
#include <string>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/constants.hpp>
#include <bitcoin/bitcoin/message/network_address.hpp>
#include <bitcoin/bitcoin/utility/reader.hpp>
#include <bitcoin/bitcoin/utility/writer.hpp>

namespace libbitcoin {
namespace message {
    
// The checksum is ignored by the version command.
class BC_API version
{
public:
    typedef std::shared_ptr<version> ptr;

    enum level: uint32_t
    {
        // This is currently unspecified.
        bip152 = 70014,

        // fee_filter
        bip133 = 70013,

        // send_headers
        bip130 = 70012,

        // reject (AND version.relay)
        bip61 = 70002,

        // filters, merkle_block, not_found (NOT version.relay)
        bip37 = 70001,

        // memory_pool
        bip35 = 60002,

        // ping.nonce, pong
        bip31 = 60001,

        // This preceded the BIP system.
        headers = 31800,

        // We require at least this of peers.
        minimum = 31402,

        // We support at most this internally (bound to settings default).
        maximum = bip130
    };

    static version factory_from_data(uint32_t version, const data_chunk& data);
    static version factory_from_data(uint32_t version, std::istream& stream);
    static version factory_from_data(uint32_t version, reader& source);

    bool from_data(uint32_t version, const data_chunk& data);
    bool from_data(uint32_t version, std::istream& stream);
    bool from_data(uint32_t version, reader& source);
    data_chunk to_data(uint32_t version) const;
    void to_data(uint32_t version, std::ostream& stream) const;
    void to_data(uint32_t version, writer& sink) const;
    bool is_valid() const;
    void reset();
    uint64_t serialized_size(uint32_t version) const;

    static const std::string command;
    static const uint32_t version_minimum;
    static const uint32_t version_maximum;

    uint32_t value;
    uint64_t services;
    uint64_t timestamp;
    network_address address_recevier;
    network_address address_sender;
    uint64_t nonce;
    std::string user_agent;
    uint32_t start_height;

    // version >= 70001
    bool relay;
};

} // namespace message
} // namespace libbitcoin

#endif
