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
#ifndef LIBBITCOIN_PRIMITIVES_HPP
#define LIBBITCOIN_PRIMITIVES_HPP

#include <cstdint>
#include <string>
#include <vector>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/script.hpp>
#include <bitcoin/bitcoin/math/hash.hpp>

namespace libbitcoin {

typedef std::vector<hash_digest> block_locator_type;

typedef byte_array<16> ip_address_type;

BC_CONSTEXPR ip_address_type localhost_ip_address =
{
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0xff, 0xff, 0x0a, 0x00, 0x00, 0x01
};

struct BC_API network_address_type
{
    uint32_t timestamp;
    uint64_t services;
    ip_address_type ip;
    uint16_t port;
};

enum class inventory_type_id
{
    error,
    transaction,
    block,
    none
};

struct BC_API inventory_vector_type
{
    inventory_type_id type;
    hash_digest hash;
};

typedef std::vector<inventory_vector_type> inventory_list;

struct BC_API header_type
{
    uint32_t magic;
    std::string command;
    uint32_t payload_length;
    // Ignored by version and verack commands
    uint32_t checksum;
};

struct BC_API version_type
{
    uint32_t version;
    uint64_t services;
    uint64_t timestamp;
    network_address_type address_me;
    network_address_type address_you;
    uint64_t nonce;
    std::string user_agent;
    uint32_t start_height;
};

struct BC_API verack_type
{
};

struct BC_API get_address_type
{
};

struct BC_API get_blocks_type
{
    // 10 sequential hashes, then exponential samples until reaching genesis
    block_locator_type start_hashes;
    hash_digest hash_stop;
};

struct BC_API point_type
{
    hash_digest hash;
    uint32_t index;
};

typedef point_type input_point;
typedef point_type output_point;
typedef std::vector<input_point> input_point_list;
typedef std::vector<output_point> output_point_list;

// Used by transaction_indexer and select_outputs()
struct BC_API output_info_type
{
    output_point point;
    uint64_t value;
};

typedef std::vector<output_info_type> output_info_list;
// Not used
//typedef std::vector<uint64_t> output_value_list;

struct BC_API transaction_input_type
{
    output_point previous_output;
    script_type script;
    uint32_t sequence;
};

struct BC_API transaction_output_type
{
    uint64_t value;
    script_type script;
};

typedef std::vector<transaction_input_type> transaction_input_list;
typedef std::vector<transaction_output_type> transaction_output_list;

struct BC_API transaction_type
{
    uint32_t version;
    uint32_t locktime;
    transaction_input_list inputs;
    transaction_output_list outputs;
};
typedef std::vector<transaction_type> transaction_list;

struct BC_API block_header_type
{
    uint32_t version;
    hash_digest previous_block_hash;
    hash_digest merkle;
    uint32_t timestamp;
    uint32_t bits;
    uint32_t nonce;
};

struct BC_API block_type
{
    block_header_type header;
    transaction_list transactions;
};

typedef std::vector<network_address_type> network_address_list;

struct BC_API address_type
{
    network_address_list addresses;
};

struct BC_API get_data_type
{
    inventory_list inventories;
};

struct BC_API inventory_type
{
    inventory_list inventories;
};

// TODO: Satoshi peers might send 0-length nonces as of 0.10.
// Allow the ping & pong payloads to be any size.
struct BC_API ping_type
{
    uint64_t nonce;
};

struct BC_API pong_type
{
    uint64_t nonce;
};

} // namespace libbitcoin

#endif

