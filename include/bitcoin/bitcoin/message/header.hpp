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
#ifndef LIBBITCOIN_MESSAGE_HEADER_HPP
#define LIBBITCOIN_MESSAGE_HEADER_HPP

#include <cstdint>
#include <istream>
#include <string>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/utility/data.hpp>
#include <bitcoin/bitcoin/utility/deserializer.hpp>
#include <bitcoin/bitcoin/utility/serializer.hpp>

namespace libbitcoin {
namespace message {

class BC_API header
{
public:

    header();

    header(uint32_t magic, std::string command, uint32_t payload_length,
        uint32_t checksum);

    header(std::istream& stream);

    uint32_t magic() const;

    void magic(const uint32_t value);

    std::string& command();

    const std::string& command() const;

    void command(const std::string& value);

    uint32_t payload_length() const;

    void payload_length(const uint32_t value);

    uint32_t checksum() const;

    void checksum(const uint32_t value);

    data_chunk to_data() const;

    size_t satoshi_size() const;

private:

    uint32_t magic_;
    std::string command_;
    uint32_t payload_length_;
    // Ignored by version and verack commands
    uint32_t checksum_;
};

} // end message
} // end libbitcoin

#endif
