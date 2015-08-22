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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef LIBBITCOIN_MESSAGE_NONCE_HPP
#define LIBBITCOIN_MESSAGE_NONCE_HPP

#include <cstdint>
#include <istream>
#include <string>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/utility/data.hpp>
#include <bitcoin/bitcoin/utility/reader.hpp>
#include <bitcoin/bitcoin/utility/writer.hpp>

namespace libbitcoin {
namespace message {

class BC_API ping_pong
{
public:
    static uint64_t satoshi_fixed_size();

    bool from_data(const data_chunk& data);
    bool from_data(std::istream& stream);
    bool from_data(reader& source);
    data_chunk to_data() const;
    void to_data(std::ostream& stream) const;
    void to_data(writer& sink) const;
    bool is_valid() const;
    void reset();
    uint64_t satoshi_size() const;

    uint64_t nonce;
};

BC_API bool operator==(const ping_pong& left, const ping_pong& right);
BC_API bool operator!=(const ping_pong& left, const ping_pong& right);

class BC_API ping
  : public ping_pong
{
public:
    static ping factory_from_data(const data_chunk& data);
    static ping factory_from_data(std::istream& stream);
    static ping factory_from_data(reader& source);
    static uint64_t satoshi_fixed_size();

    ping();
    ping(const uint64_t nonce);

    static const std::string satoshi_command;
};

class BC_API pong
  : public ping_pong
{
public:
    static pong factory_from_data(const data_chunk& data);
    static pong factory_from_data(std::istream& stream);
    static pong factory_from_data(reader& source);
    static uint64_t satoshi_fixed_size();

    pong();
    pong(const uint64_t nonce);

    static const std::string satoshi_command;
};

} // namspace message
} // namspace libbitcoin

#endif
