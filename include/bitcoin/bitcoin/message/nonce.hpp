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

// TODO: Satoshi peers will might 0-length nonces as of 0.10.
// Allow the ping & pong payloads to be any size.

class BC_API nonce_base
{
public:

    uint64_t nonce;

    bool from_data(const data_chunk& data);

    bool from_data(std::istream& stream);

    bool from_data(reader& source);

    data_chunk to_data() const;

    void to_data(std::ostream& stream) const;

    void to_data(writer& sink) const;

    bool is_valid() const;

    void reset();

    uint64_t satoshi_size() const;

    static uint64_t satoshi_fixed_size();
};

bool operator==(const nonce_base& a, const nonce_base& b);

bool operator!=(const nonce_base& a, const nonce_base& b);

class BC_API ping : public nonce_base
{
public:

    static const std::string satoshi_command;

    static ping factory_from_data(const data_chunk& data);

    static ping factory_from_data(std::istream& stream);

    static ping factory_from_data(reader& source);

    static uint64_t satoshi_fixed_size();
};

class BC_API pong : public nonce_base
{
public:

    static const std::string satoshi_command;

    static pong factory_from_data(const data_chunk& data);

    static pong factory_from_data(std::istream& stream);

    static pong factory_from_data(reader& source);

    static uint64_t satoshi_fixed_size();
};

} // end message
} // end libbitcoin

#endif
