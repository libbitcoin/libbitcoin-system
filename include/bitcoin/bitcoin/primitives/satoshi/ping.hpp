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
#ifndef LIBBITCOIN_SATOSHI_PRIMITIVE_PING_HPP
#define LIBBITCOIN_SATOSHI_PRIMITIVE_PING_HPP

#include <cstdint>
#include <string>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/utility/data.hpp>
#include <bitcoin/bitcoin/utility/serializer.hpp>

namespace libbitcoin {

// TODO: Satoshi peers will might 0-length nonces as of 0.10.
// Allow the ping & pong payloads to be any size.

class BC_API nonce_type
{
public:

    uint64_t nonce;

    nonce_type();

    nonce_type(const data_chunk& value);

    template <typename Iterator, bool SafeCheckLast>
    nonce_type(deserializer<Iterator, SafeCheckLast>& deserial);

    template<typename Iterator>
    nonce_type(const Iterator begin, const Iterator end);

    operator const data_chunk() const;

    size_t satoshi_size() const;

    static size_t satoshi_fixed_size();

private:

    template <typename Iterator, bool SafeCheckLast>
    void deserialize(deserializer<Iterator, SafeCheckLast>& deserial);
};

class BC_API ping_type : public nonce_type
{
public:

    static const std::string satoshi_command;

    ping_type();

    ping_type(const data_chunk& value);

    template <typename Iterator, bool SafeCheckLast>
    ping_type(deserializer<Iterator, SafeCheckLast>& deserial);

    template<typename Iterator>
    ping_type(const Iterator begin, const Iterator end);

    static size_t satoshi_fixed_size();
};

class BC_API pong_type : public nonce_type
{
public:

    static const std::string satoshi_command;

    pong_type();

    pong_type(const data_chunk& value);

    template <typename Iterator, bool SafeCheckLast>
    pong_type(deserializer<Iterator, SafeCheckLast>& deserial);

    template<typename Iterator>
    pong_type(const Iterator begin, const Iterator end);

    static size_t satoshi_fixed_size();
};

}

#include <bitcoin/bitcoin/impl/primitives/satoshi/ping.ipp>

#endif
