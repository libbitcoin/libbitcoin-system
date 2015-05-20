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
#include <istream>
#include <vector>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/chain/script.hpp>
#include <bitcoin/bitcoin/utility/serializer.hpp>

namespace libbitcoin {
namespace chain {

class BC_API transaction_output
{
public:

    uint64_t value;
    chain::script script;

    bool from_data(const data_chunk& data);

    bool from_data(std::istream& stream);

    data_chunk to_data() const;

    std::string to_string() const;

    bool is_valid() const;

    void reset();

    uint64_t satoshi_size() const;

    static transaction_output factory_from_data(const data_chunk& data);

    static transaction_output factory_from_data(std::istream& stream);

    static uint64_t satoshi_fixed_size();
};

typedef std::vector<transaction_output> transaction_output_list;

} // end chain
} // end libbitcoin

#endif
