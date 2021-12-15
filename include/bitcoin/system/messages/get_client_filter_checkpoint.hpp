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

// Sponsored in part by Digital Contract Design, LLC

#ifndef LIBBITCOIN_SYSTEM_MESSAGES_GET_CLIENT_FILTER_CHECKPOINT_HPP
#define LIBBITCOIN_SYSTEM_MESSAGES_GET_CLIENT_FILTER_CHECKPOINT_HPP

#include <cstddef>
#include <cstdint>
#include <memory>
#include <string>
#include <bitcoin/system/crypto/crypto.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/messages/identifier.hpp>
#include <bitcoin/system/stream/stream.hpp>

namespace libbitcoin {
namespace system {
namespace messages {

struct BC_API get_client_filter_checkpoint
{
    typedef std::shared_ptr<const get_client_filter_checkpoint> ptr;

    static const identifier id;
    static const std::string command;
    static const uint32_t version_minimum;
    static const uint32_t version_maximum;

    static size_t size(uint32_t version);
    static get_client_filter_checkpoint deserialize(uint32_t version,
        reader& source);
    void serialize(uint32_t version, writer& sink) const;

    uint8_t filter_type;
    hash_digest stop_hash;
};

} // namespace messages
} // namespace system
} // namespace libbitcoin

#endif
