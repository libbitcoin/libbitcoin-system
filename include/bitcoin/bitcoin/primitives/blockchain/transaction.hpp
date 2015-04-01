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
#ifndef LIBBITCOIN_BLOCKCHAIN_PRIMITIVE_TRANSACTION_HPP
#define LIBBITCOIN_BLOCKCHAIN_PRIMITIVE_TRANSACTION_HPP

#include <cstdint>
#include <string>
#include <vector>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/primitives/blockchain/transaction_input.hpp>
#include <bitcoin/bitcoin/primitives/blockchain/transaction_output.hpp>
#include <bitcoin/bitcoin/math/ec_keys.hpp>
#include <bitcoin/bitcoin/utility/serializer.hpp>

namespace libbitcoin {

class BC_API transaction_type
{
public:

    static const std::string satoshi_command;

    uint32_t version;
    uint32_t locktime;
    transaction_input_list inputs;
    transaction_output_list outputs;

    transaction_type();

    transaction_type(const data_chunk& value);

    template <typename Iterator, bool SafeCheckLast>
    transaction_type(deserializer<Iterator, SafeCheckLast>& deserial);

    template<typename Iterator>
    transaction_type(const Iterator begin, const Iterator end);

//    transaction_type(const std::string& human_readable);

    operator const data_chunk() const;

    size_t satoshi_size() const;

    static size_t satoshi_fixed_size();

    std::string to_string() const;

private:

    template <typename Iterator, bool SafeCheckLast>
    void deserialize(deserializer<Iterator, SafeCheckLast>& deserial);
};

typedef std::vector<transaction_type> transaction_list;

}

#include <bitcoin/bitcoin/impl/primitives/blockchain/transaction.ipp>

#endif
