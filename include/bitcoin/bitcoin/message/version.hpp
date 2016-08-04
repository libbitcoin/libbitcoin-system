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

    static version factory_from_data(const data_chunk& data);
    static version factory_from_data(std::istream& stream);
    static version factory_from_data(reader& source);

    bool from_data(const data_chunk& data);
    bool from_data(std::istream& stream);
    bool from_data(reader& source);
    data_chunk to_data() const;
    void to_data(std::ostream& stream) const;
    void to_data(writer& sink) const;
    bool is_valid() const;
    void reset();
    uint64_t serialized_size() const;

    static const std::string command;

    // version >= 0
    uint32_t value;
    uint64_t services_sender;
    uint64_t timestamp;

    // version >= 106
    //-------------------------------------------------------------------------
    // This is not present in protocol version 60002 according to bitcoin.it.
    // But according to bitcoin.org it was added in version 106.
    ////uint64_t services_recevier;
    //-------------------------------------------------------------------------
    network_address address_recevier;

    // version >= 0
    //-------------------------------------------------------------------------
    // This is not present in protocol version 60002 according to bitcoin wiki.
    // But according to bitcoin.org it was original (and duplicated).
    ////uint64_t services_sender;
    //-------------------------------------------------------------------------
    network_address address_sender;
    uint64_t nonce;
    std::string user_agent;
    //-------------------------------------------------------------------------
    // This was previously >= version 209 in libbitcoin, but that is not
    // documented by bitcoin.it or bitcoin.org, so is now treated as original.
    uint32_t start_height;
    //-------------------------------------------------------------------------

    // version >= 70001
    bool relay;
};

} // namspace message
} // namspace libbitcoin

#endif
