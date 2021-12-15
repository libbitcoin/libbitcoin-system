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
#ifndef LIBBITCOIN_SYSTEM_MESSAGES_ALERT_ITEM_HPP
#define LIBBITCOIN_SYSTEM_MESSAGES_ALERT_ITEM_HPP

#include <cstddef>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>
#include <bitcoin/system/crypto/crypto.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/stream/stream.hpp>

namespace libbitcoin {
namespace system {
namespace messages {

struct BC_API alert_item
{
    typedef std::vector<uint32_t> cancels;
    typedef std::vector<std::string> sub_versions;
    static const ec_uncompressed satoshi_public_key;

    static alert_item deserialize(uint32_t version, reader& source);
    void serialize(uint32_t version, writer& sink) const;
    size_t size(uint32_t version) const;

    uint32_t version;
    uint64_t relay_until;
    uint64_t expiration;
    uint32_t id;
    uint32_t cancel;
    cancels set_cancel;
    uint32_t min_version;
    uint32_t max_version;
    sub_versions set_sub_version;
    uint32_t priority;
    std::string comment;
    std::string status_bar;
    std::string reserved;
};

} // namespace messages
} // namespace system
} // namespace libbitcoin

#endif
