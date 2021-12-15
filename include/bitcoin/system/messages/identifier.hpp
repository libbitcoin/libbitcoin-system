/**
 * Copyright (c) 2011-2021 libbitcoin developers (see AUTHORS)
 *
 * This file is part of libbitcoin.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef LIBBITCOIN_SYSTEM_MESSAGES_IDENTIFIER_HPP
#define LIBBITCOIN_SYSTEM_MESSAGES_IDENTIFIER_HPP

namespace libbitcoin {
namespace system {
namespace messages {

enum class identifier
{
    unknown,
    address,
    alert,
    block,
    compact_transactions,
    compact_block,
    client_filter,
    client_filter_checkpoint,
    client_filter_headers,
    fee_filter,
    bloom_filter_add,
    bloom_filter_clear,
    bloom_filter_load,
    get_address,
    get_compact_transactions,
    get_blocks,
    get_client_filter_checkpoint,
    get_client_filter_headers,
    get_client_filters,
    get_data,
    get_headers,
    headers,
    inventory,
    memory_pool,
    merkle_block,
    not_found,
    ping,
    pong,
    reject,
    send_compact,
    send_headers,
    transaction,
    version_acknowledge,
    version
};

} // namespace messages
} // namespace system
} // namespace libbitcoin
#endif
