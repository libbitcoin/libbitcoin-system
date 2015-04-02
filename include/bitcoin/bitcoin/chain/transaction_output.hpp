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
#ifndef LIBBITCOIN_CHAIN_TRANSACTION_OUTPUT_HPP
#define LIBBITCOIN_CHAIN_TRANSACTION_OUTPUT_HPP

#include <cstdint>
#include <vector>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/chain/script.hpp>
#include <bitcoin/bitcoin/utility/deserializer.hpp>
#include <bitcoin/bitcoin/utility/serializer.hpp>

namespace libbitcoin {
namespace chain {

class BC_API transaction_output
{
public:

    uint64_t value;
    chain::script script;

    transaction_output();

    transaction_output(const data_chunk& value);

    template <typename Iterator, bool SafeCheckLast>
    transaction_output(deserializer<Iterator, SafeCheckLast>& deserial);

    template<typename Iterator>
    transaction_output(const Iterator begin, const Iterator end);

    operator const data_chunk() const;

    size_t satoshi_size() const;

    static size_t satoshi_fixed_size();

    std::string to_string() const;

private:

    template <typename Iterator, bool SafeCheckLast>
    void deserialize(deserializer<Iterator, SafeCheckLast>& deserial);
};

typedef std::vector<transaction_output> transaction_output_list;

} // end chain
} // end libbitcoin

#include <bitcoin/bitcoin/impl/chain/transaction_output.ipp>


#endif
