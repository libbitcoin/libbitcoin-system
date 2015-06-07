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
#include <bitcoin/bitcoin/chain/block.hpp>
#include <bitcoin/bitcoin/chain/transaction.hpp>
#include <bitcoin/bitcoin/message/address.hpp>
#include <bitcoin/bitcoin/message/announce_version.hpp>
#include <bitcoin/bitcoin/message/get_address.hpp>
#include <bitcoin/bitcoin/message/get_blocks.hpp>
#include <bitcoin/bitcoin/message/get_data.hpp>
#include <bitcoin/bitcoin/message/inventory.hpp>
#include <bitcoin/bitcoin/message/nonce.hpp>
#include <bitcoin/bitcoin/message/verack.hpp>

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

const std::string chain::block::satoshi_command = "block";
const std::string chain::transaction::satoshi_command = "tx";
const std::string message::address::satoshi_command = "addr";
const std::string message::announce_version::satoshi_command = "version";
const std::string message::get_address::satoshi_command = "getaddr";
const std::string message::get_blocks::satoshi_command = "getblocks";
const std::string message::get_data::satoshi_command = "getdata";
const std::string message::inventory::satoshi_command = "inv";
const std::string message::ping::satoshi_command = "ping";
const std::string message::pong::satoshi_command = "pong";
const std::string message::verack::satoshi_command = "verack";

} // namespace libbitcoin

