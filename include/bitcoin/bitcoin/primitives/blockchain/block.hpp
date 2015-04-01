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
#ifndef LIBBITCOIN_BLOCKCHAIN_PRIMITIVE_BLOCK_HPP
#define LIBBITCOIN_BLOCKCHAIN_PRIMITIVE_BLOCK_HPP

#include <string>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/primitives/blockchain/block_header.hpp>
#include <bitcoin/bitcoin/primitives/blockchain/transaction.hpp>
#include <bitcoin/bitcoin/utility/data.hpp>
#include <bitcoin/bitcoin/utility/serializer.hpp>

namespace libbitcoin {

class BC_API block_type
{
public:

    static const std::string satoshi_command;

    block_header_type header;
    transaction_list transactions;

    block_type();

    block_type(const data_chunk& value);

    template <typename Iterator, bool SafeCheckLast>
    block_type(deserializer<Iterator, SafeCheckLast>& deserial);

    template<typename Iterator>
    block_type(const Iterator begin, const Iterator end);

    operator const data_chunk() const;

    size_t satoshi_size() const;

private:

    template <typename Iterator, bool SafeCheckLast>
    void deserialize(deserializer<Iterator, SafeCheckLast>& deserial);
};

}

#include <bitcoin/bitcoin/impl/primitives/blockchain/block.ipp>

#endif
