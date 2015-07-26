/**
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
#include <bitcoin/bitcoin/satoshi_serialize.hpp>

#include <string>
#include <cstdint>

namespace libbitcoin {

size_t variable_uint_size(uint64_t v)
{
    if (v < 0xfd)
        return 1;
    else if (v <= 0xffff)
        return 3;
    else if (v <= 0xffffffff)
        return 5;
    else
        return 9;
}

size_t satoshi_raw_size(const header_type& head)
{
    return 20 + (head.checksum == 0 ? 0 : 4);
}

const std::string satoshi_command(const version_type&)
{
    return "version";
}
size_t satoshi_raw_size(const version_type& packet)
{
    return 84 +
        variable_uint_size(packet.user_agent.size()) +
        packet.user_agent.size();
}

const std::string satoshi_command(const verack_type&)
{
    return "verack";
}
size_t satoshi_raw_size(const verack_type&)
{
    return 0;
}

const std::string satoshi_command(const address_type&)
{
    return "addr";
}
size_t satoshi_raw_size(const address_type& packet)
{
    return variable_uint_size(packet.addresses.size()) +
        30 * packet.addresses.size();
}

const std::string satoshi_command(const get_address_type&)
{
    return "getaddr";
}
size_t satoshi_raw_size(const get_address_type&)
{
    return 0;
}

uint32_t inventory_type_to_number(inventory_type_id inv_type)
{
    switch (inv_type)
    {
        case inventory_type_id::error:
        case inventory_type_id::none:
        default:
            return 0;

        case inventory_type_id::transaction:
            return 1;

        case inventory_type_id::block:
            return 2;
    }
}

inventory_type_id inventory_type_from_number(uint32_t raw_type)
{
    switch (raw_type)
    {
        case 0:
            return inventory_type_id::error;
        case 1:
            return inventory_type_id::transaction;
        case 2:
            return inventory_type_id::block;
        default:
            return inventory_type_id::none;
    }
}

const std::string satoshi_command(const inventory_type&)
{
    return "inv";
}
size_t satoshi_raw_size(const inventory_type& packet)
{
    return raw_size_inventory_impl(packet);
}

const std::string satoshi_command(const get_data_type&)
{
    return "getdata";
}
size_t satoshi_raw_size(const get_data_type& packet)
{
    return raw_size_inventory_impl(packet);
}

const std::string satoshi_command(const get_blocks_type&)
{
    return "getblocks";
}
size_t satoshi_raw_size(const get_blocks_type& packet)
{
    return 36 +
        variable_uint_size(packet.start_hashes.size()) +
        32 * packet.start_hashes.size();
}

const std::string satoshi_command(const transaction_type&)
{
    return "tx";
}
size_t satoshi_raw_size(const transaction_type& packet)
{
    size_t tx_size = 8;
    tx_size += variable_uint_size(packet.inputs.size());
    for (const transaction_input_type& input: packet.inputs)
    {
        data_chunk raw_script = save_script(input.script);
        tx_size += 40 +
            variable_uint_size(raw_script.size()) +
            raw_script.size();
    }
    tx_size += variable_uint_size(packet.outputs.size());
    for (const transaction_output_type& output: packet.outputs)
    {
        data_chunk raw_script = save_script(output.script);
        tx_size += 8 +
            variable_uint_size(raw_script.size()) +
            raw_script.size();
    }
    return tx_size;
}

size_t satoshi_raw_size(const block_header_type&)
{
    return 80;
}

const std::string satoshi_command(const block_type&)
{
    return "block";
}
size_t satoshi_raw_size(const block_type& packet)
{
    size_t block_size = 80 + variable_uint_size(packet.transactions.size());
    for (const transaction_type& tx: packet.transactions)
        block_size += satoshi_raw_size(tx);
    return block_size;
}

const std::string satoshi_command(const ping_type&)
{
    return "ping";
}
size_t satoshi_raw_size(const ping_type&)
{
    return 8;
}

const std::string satoshi_command(const pong_type&)
{
    return "pong";
}
size_t satoshi_raw_size(const pong_type&)
{
    return 8;
}

} // libbitcoin

