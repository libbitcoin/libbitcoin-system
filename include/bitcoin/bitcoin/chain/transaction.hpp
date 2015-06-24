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
#ifndef LIBBITCOIN_CHAIN_TRANSACTION_HPP
#define LIBBITCOIN_CHAIN_TRANSACTION_HPP

#include <cstdint>
#include <istream>
#include <string>
#include <vector>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/chain/transaction_input.hpp>
#include <bitcoin/bitcoin/chain/transaction_output.hpp>
#include <bitcoin/bitcoin/math/ec_keys.hpp>
#include <bitcoin/bitcoin/utility/reader.hpp>
#include <bitcoin/bitcoin/utility/writer.hpp>

namespace libbitcoin {
namespace chain {

class BC_API transaction
{
public:

    typedef std::vector<transaction> list;

    static const std::string satoshi_command;

    uint32_t version;
    uint32_t locktime;
    transaction_input::list inputs;
    transaction_output::list outputs;

    bool from_data(const data_chunk& data);

    bool from_data(std::istream& stream);

    bool from_data(reader& source);

    data_chunk to_data() const;

    void to_data(std::ostream& stream) const;

    void to_data(writer& sink) const;

    std::string to_string() const;

    bool is_valid() const;

    void reset();

    hash_digest hash() const;

    // hash_type_code is used by OP_CHECKSIG
    hash_digest hash(uint32_t hash_type_code) const;

    bool is_coinbase() const;

    bool is_final(uint64_t block_height, uint32_t block_time) const;

    bool is_locktime_conflict() const;

    uint64_t total_output_value() const;

    uint64_t satoshi_size() const;

    static transaction factory_from_data(const data_chunk& data);

    static transaction factory_from_data(std::istream& stream);

    static transaction factory_from_data(reader& source);

    static uint64_t satoshi_fixed_size();
};

} // end chain
} // end libbitcoin

#endif
