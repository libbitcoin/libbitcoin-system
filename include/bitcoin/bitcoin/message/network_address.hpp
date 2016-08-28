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
#ifndef LIBBITCOIN_MESSAGE_NETWORK_ADDRESS_HPP
#define LIBBITCOIN_MESSAGE_NETWORK_ADDRESS_HPP

#include <cstdint>
#include <istream>
#include <vector>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/utility/data.hpp>
#include <bitcoin/bitcoin/utility/reader.hpp>
#include <bitcoin/bitcoin/utility/writer.hpp>

namespace libbitcoin {
namespace message {

typedef byte_array<16> ip_address;

BC_CONSTEXPR ip_address localhost_ip_address =
{
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0xff, 0xff, 0x0a, 0x00, 0x00, 0x01
};

class BC_API network_address
{
public:
    typedef std::vector<network_address> list;

    static network_address factory_from_data(uint32_t version,
        const data_chunk& data, bool with_timestamp /*= true*/);
    static network_address factory_from_data(uint32_t version,
        std::istream& stream, bool with_timestamp /*= true*/);
    static network_address factory_from_data(uint32_t version,
        reader& source, bool with_timestamp /*=true*/);
    static uint64_t satoshi_fixed_size(uint32_t version,
        bool with_timestamp /*= false*/);

    bool from_data(uint32_t version, const data_chunk& data,
        bool with_timestamp /*= true*/);
    bool from_data(uint32_t version, std::istream& stream,
        bool with_timestamp /*= true*/);
    bool from_data(uint32_t version, reader& source,
        bool with_timestamp /*= true*/);
    data_chunk to_data(uint32_t version,
        bool with_timestamp /*= true*/) const;
    void to_data(uint32_t version, std::ostream& stream,
        bool with_timestamp /*= true*/) const;
    void to_data(uint32_t version, writer& sink,
        bool with_timestamp /*= true*/) const;
    bool is_valid() const;
    void reset();
    uint64_t serialized_size(uint32_t version,
        bool with_timestamp /*= false*/) const;

    // Starting version 31402, addresses are prefixed with a timestamp.
    uint32_t timestamp;
    uint64_t services;
    ip_address ip;
    uint16_t port;
};

} // namespace message
} // namespace libbitcoin

#endif
