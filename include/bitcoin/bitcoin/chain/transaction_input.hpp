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
#include <istream>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/chain/point.hpp>
#include <bitcoin/bitcoin/chain/script.hpp>
#include <bitcoin/bitcoin/math/hash.hpp>

namespace libbitcoin {
namespace chain {

class BC_API transaction_input
{
public:

    transaction_input();

    transaction_input(const output_point& previous_output,
        const chain::script& script, const uint32_t sequence);

    output_point& previous_output();

    const output_point& previous_output() const;

    void previous_output(const output_point& previous);

    chain::script& script();

    const chain::script& script() const;

    void script(const chain::script& script);

    uint32_t sequence() const;

    void sequence(const uint32_t sequence);

    bool from_data(const data_chunk& data);

    bool from_data(std::istream& stream);

    data_chunk to_data() const;

    std::string to_string() const;

    void reset();

    bool is_final() const;

    uint64_t satoshi_size() const;

    static uint64_t satoshi_fixed_size();

private:

    output_point previous_output_;
    chain::script script_;
    uint32_t sequence_;
};

typedef std::vector<transaction_input> transaction_input_list;

} // end chain
} // end libbitcoin

#endif
