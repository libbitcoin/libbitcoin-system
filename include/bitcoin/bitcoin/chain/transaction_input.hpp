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
#ifndef LIBBITCOIN_CHAIN_TRANSACTION_INPUT_HPP
#define LIBBITCOIN_CHAIN_TRANSACTION_INPUT_HPP

#include <cstdint>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/chain/point.hpp>
#include <bitcoin/bitcoin/chain/script.hpp>
#include <bitcoin/bitcoin/math/hash.hpp>
#include <bitcoin/bitcoin/utility/serializer.hpp>

namespace libbitcoin {
namespace chain {

class BC_API transaction_input
{
public:

    output_point previous_output;
    chain::script script;
    uint32_t sequence;

    transaction_input();

    transaction_input(const data_chunk& value);

    template <typename Iterator, bool SafeCheckLast>
    transaction_input(deserializer<Iterator, SafeCheckLast>& deserial);

    template<typename Iterator>
    transaction_input(const Iterator begin, const Iterator end);

    operator const data_chunk() const;

    size_t satoshi_size() const;

    static size_t satoshi_fixed_size();

    std::string to_string() const;

    bool is_final() const;

private:

    template <typename Iterator, bool SafeCheckLast>
    void deserialize(deserializer<Iterator, SafeCheckLast>& deserial);
};

typedef std::vector<transaction_input> transaction_input_list;

} // end chain
} // end libbitcoin

#include <bitcoin/bitcoin/impl/chain/transaction_input.ipp>

#endif
