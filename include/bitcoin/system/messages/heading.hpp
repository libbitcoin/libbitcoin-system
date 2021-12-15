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
#ifndef LIBBITCOIN_SYSTEM_MESSAGES_HEADING_HPP
#define LIBBITCOIN_SYSTEM_MESSAGES_HEADING_HPP

#include <cstddef>
#include <cstdint>
#include <memory>
#include <string>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/messages/identifier.hpp>
#include <bitcoin/system/stream/stream.hpp>

namespace libbitcoin {
namespace system {
namespace messages {

struct BC_API heading
{
    typedef std::shared_ptr<const heading> ptr;

    static const uint32_t version_minimum;
    static const uint32_t version_maximum;

    static size_t maximum_payload_size(uint32_t version, bool witness);
    static heading factory(uint32_t magic, const std::string& command,
        const data_slice& payload);

    // Heading does not use version.
    static size_t size();
    static heading deserialize(reader& source);
    void serialize(writer& sink) const;

    identifier id() const;
    bool verify_checksum(const data_slice& body) const;

    uint32_t magic;
    std::string command;
    uint32_t payload_size;
    uint32_t checksum;
};

} // namespace messages
} // namespace system
} // namespace libbitcoin

#endif
