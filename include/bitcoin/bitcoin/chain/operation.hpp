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
#ifndef LIBBITCOIN_CHAIN_OPERATION_HPP
#define LIBBITCOIN_CHAIN_OPERATION_HPP

#include <istream>
#include <vector>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/chain/opcode.hpp>
#include <bitcoin/bitcoin/utility/data.hpp>
#include <bitcoin/bitcoin/utility/serializer.hpp>

namespace libbitcoin {
namespace chain {

class BC_API operation
{
public:

    opcode code;
    data_chunk data;

    bool from_data(const data_chunk& data);

    bool from_data(std::istream& stream);

    data_chunk to_data() const;

    std::string to_string() const;

    bool is_valid() const;

    void reset();

    uint64_t satoshi_size() const;

//    static operation factory_from_data(const data_chunk& data);
//
//    static operation factory_from_data(std::istream& stream);

private:

    static uint64_t read_opcode_data_byte_count(opcode code, uint8_t raw_byte,
        std::istream& stream);

    static bool must_read_data(opcode code);
};

typedef std::vector<operation> operation_stack;

bool is_push(const opcode code);

uint64_t count_non_push(const operation_stack& operations);

bool is_push_only(const operation_stack& operations);

bool is_pubkey_type(const operation_stack& ops);

bool is_pubkey_hash_type(const operation_stack& ops);

bool is_script_hash_type(const operation_stack& ops);

bool is_stealth_info_type(const operation_stack& ops);

bool is_multisig_type(const operation_stack&);

bool is_pubkey_hash_sig_type(const operation_stack& ops);

bool is_script_code_sig_type(const operation_stack& ops);

} // end chain
} // end libbitcoin

#endif
