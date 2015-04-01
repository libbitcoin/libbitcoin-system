/*
 * Copyright (c) 2011-2013 libbitcoin developers (see AUTHORS)
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
#include <bitcoin/bitcoin/constants.hpp>
#include <bitcoin/bitcoin/primitives/blockchain/block.hpp>
#include <bitcoin/bitcoin/primitives/blockchain/transaction.hpp>
#include <bitcoin/bitcoin/primitives/satoshi/address.hpp>
#include <bitcoin/bitcoin/primitives/satoshi/get_address.hpp>
#include <bitcoin/bitcoin/primitives/satoshi/get_blocks.hpp>
#include <bitcoin/bitcoin/primitives/satoshi/get_data.hpp>
#include <bitcoin/bitcoin/primitives/satoshi/inventory.hpp>
#include <bitcoin/bitcoin/primitives/satoshi/ping.hpp>
#include <bitcoin/bitcoin/primitives/satoshi/verack.hpp>
#include <bitcoin/bitcoin/primitives/satoshi/version.hpp>

namespace libbitcoin {

hash_number max_target()
{
    hash_number max_target;
    max_target.set_compact(max_work_bits);
    return max_target;
}

uint32_t magic_value()
{
#ifdef ENABLE_TESTNET
    return 0x0709110b;
#else
    return 0xd9b4bef9;
#endif
}

const std::string block_type::satoshi_command = "block";
const std::string transaction_type::satoshi_command = "tx";
const std::string address_type::satoshi_command = "addr";
const std::string get_address_type::satoshi_command = "getaddr";
const std::string get_blocks_type::satoshi_command = "getblocks";
const std::string get_data_type::satoshi_command = "getdata";
const std::string inventory_type::satoshi_command = "inv";
const std::string ping_type::satoshi_command = "ping";
const std::string pong_type::satoshi_command = "pong";
const std::string verack_type::satoshi_command = "verack";
const std::string version_type::satoshi_command = "version";

} // namespace libbitcoin

