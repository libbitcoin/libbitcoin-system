/**
 * Copyright (c) 2011-2019 libbitcoin developers (see AUTHORS)
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
#ifndef LIBBITCOIN_SYSTEM_MESSAGES_ADDRESS_ITEM_HPP
#define LIBBITCOIN_SYSTEM_MESSAGES_ADDRESS_ITEM_HPP

#include <cstdint>
#include <cstddef>
#include <memory>
#include <vector>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/stream/stream.hpp>

namespace libbitcoin {
namespace system {
namespace messages {

typedef data_array<16> ip_address;

struct BC_API address_item
{
    typedef std::vector<address_item> list;
    typedef std::shared_ptr<const address_item> ptr;

    static size_t size(uint32_t version, bool with_timestamp);
    static address_item deserialize(uint32_t version, reader& source,
        bool with_timestamp);
    void serialize(uint32_t version, writer& sink,
        bool with_timestamp) const;

    uint32_t timestamp;
    uint64_t services;
    ip_address ip;
    uint16_t port;
};

// version::services::none
constexpr uint32_t no_services = 0;
constexpr uint32_t no_timestamp = 0;
constexpr uint16_t unspecified_ip_port = 0;
constexpr ip_address unspecified_ip_address
{
    {
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00
    }
};

// Defaults to full node services.
constexpr address_item unspecified_address_item
{
    no_timestamp,
    no_services,
    unspecified_ip_address,
    unspecified_ip_port
};

} // namespace messages
} // namespace system
} // namespace libbitcoin

#endif
