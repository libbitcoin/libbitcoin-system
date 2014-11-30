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
#ifndef LIBBITCOIN_BITCOIN_HPP
#define LIBBITCOIN_BITCOIN_HPP

/**
 * @mainpage libbitcoin API dox
 *
 * @section intro_sec Introduction
 *
 * libbitcoin is a bitcoin library targeted towards high end use.
 * The library places a heavy focus around asychronicity. This enables a
 * big scope for future scalability as each component has its own
 * thread pool. By increasing the number of threads for that component
 * the library is able to scale outwards across CPU cores. This will be
 * vital in the future as the demands of the bitcoin network grow.
 *
 * Another core design principle is libbitcoin is not a framework, but
 * a toolkit. Frameworks hinder development during the latter stages of
 * a development cycle, enforce one style of coding and do not work well
 * with other frameworks. By contrast, we have gone to great pains to make
 * libbitcoin function as an independent set of mutual components with no
 * dependencies between them.
 *
 * @section overview_sec Overview
 *
 * Broadly speaking the main services in libbitcoin can be divided down
 * three lines.
 *
 * @subsection network Network services
 *
 * These services are concerned with the network side of things.
 *
 * - @link libbitcoin::network::channel channel @endlink
 * - @link libbitcoin::network::network network @endlink
 * - @link libbitcoin::network::protocol protocol @endlink
 * - @link libbitcoin::network::hosts hosts @endlink
 * - @link libbitcoin::network::handshake handshake @endlink
 *
 * @subsection supporting Supporting services
 *
 * These services utilise other services and provide additional
 * functionality. They can be thought of as composed services.
 *
 * - @link libbitcoin::poller poller @endlink
 * - @link libbitcoin::transaction_pool transaction_pool @endlink
 * - @link libbitcoin::session session @endlink
 *
 * @section primitive Primitive
 *
 * libbitcoin operates around primitives. Each primitive corresponds to
 * one of the messages in the bitcoin network protocol.
 *
 * These primitives make up the corpus of libbitcoin.
 *
 * - @link libbitcoin::version_type version_type @endlink
 * - @link libbitcoin::verack_type verack_type @endlink
 * - @link libbitcoin::address_type address_type @endlink
 * - @link libbitcoin::inventory_type inventory_type @endlink
 * - @link libbitcoin::get_data_type get_data_type @endlink
 * - @link libbitcoin::get_blocks_type get_blocks_type @endlink
 * - @link libbitcoin::transaction_type transaction_type @endlink
 * - @link libbitcoin::block_type block_type @endlink
 * - @link libbitcoin::get_address_type get_address_type @endlink
 * - @link libbitcoin::ping_type ping_type @endlink
 * - @link libbitcoin::pong_type pong_type @endlink
 *
 * @section base Foundational
 *
 * These classes provide foundational functionality that is widely used.
 *
 * - @link libbitcoin::serializer serializer @endlink /
 *   @link libbitcoin::deserializer deserializer @endlink
 * - @link libbitcoin::script script @endlink
 * - @link libbitcoin::payment_address payment_address @endlink
 * - @link libbitcoin::script_number script_number @endlink
 * - @link libbitcoin::threadpool threadpool @endlink
 *
 * @section useful-funcs Useful functions
 *
 * In accordance with C++ principles of encapsulation, operations which
 * wrap or compose other operations are not provided as member functions,
 * but as free functions.
 *
 * This minimises intrusion of class interfaces and provides a clear
 * separation as a matter of API design choice.
 *
 * @subsection transaction Transaction
 *
 * - @link libbitcoin::hash_transaction hash_transaction @endlink
 * - @link libbitcoin::generate_merkle_root generate_merkle_root @endlink
 * - @link libbitcoin::previous_output_is_null previous_output_is_null @endlink
 * - @link libbitcoin::is_coinbase is_coinbase @endlink
 * - @link libbitcoin::total_output_value total_output_value @endlink
 *
 * @subsection block Block
 *
 * - @link libbitcoin::hash_block_header hash_block_header @endlink
 * - @link libbitcoin::block_value block_value @endlink
 * - @link libbitcoin::block_work block_work @endlink
 * - @link libbitcoin::block_locator_indices block_locator_indices @endlink
 * - @link libbitcoin::genesis_block genesis_block @endlink
 *
 * @subsection format Format
 *
 * - @link libbitcoin::encode_hex pretty_hex @endlink
 * - @link libbitcoin::decode_hex bytes_from_pretty @endlink
 * - @link libbitcoin::decode_hex_digest hash_from_pretty @endlink
 * - @link libbitcoin::extend_data extend_data @endlink
 * - @link libbitcoin::to_little_endian to_little_endian @endlink
 * - @link libbitcoin::to_big_endian to_big_endian @endlink
 * - @link libbitcoin::from_little_endian from_little_endian @endlink
 * - @link libbitcoin::from_big_endian from_big_endian @endlink
 * - @link libbitcoin::btc_to_satoshi btc_to_satoshi @endlink
 * - @link libbitcoin::satoshi_to_btc satoshi_to_btc @endlink
 *
 * @subsection hashing Hashing
 *
 * - @link libbitcoin::short_hash ripemd160_hash @endlink
 * - @link libbitcoin::short_hash sha1_hash @endlink
 * - @link libbitcoin::hash_digest sha256_hash @endlink
 * - @link libbitcoin::long_hash sha512_hash @endlink
 * - @link libbitcoin::long_hash hmac_sha512_hash @endlink
 * - @link libbitcoin::hash_digest bitcoin_hash @endlink
 * - @link libbitcoin::short_hash bitcoin_short_hash @endlink
 * - @link libbitcoin::append_checksum append_checksum @endlink
 * - @link libbitcoin::uint32_t bitcoin_checksum @endlink
 * - @link libbitcoin::bool verify_checksum @endlink
 *
 * @subsection base58 Base58
 *
 * - @link libbitcoin::encode_base58 encode_base58 @endlink
 * - @link libbitcoin::decode_base58 decode_base58 @endlink
 *
 * @subsection wallet Wallet
 *
 * @author Amir Taaki <amir@unsystem.net>
 *
 */

// Convenience header that includes everything
// Not to be used internally. For API users.

#ifdef _MSC_VER
// Suppressing msvc warnings from boost that are heard to deal with
// because boost/algorithm carelessly defines _SCL_SECURE_NO_WARNINGS
// without first testing it.
#pragma warning(push)
#pragma warning(disable : 4267)
#endif
#include <bitcoin/bitcoin/block.hpp>
#include <bitcoin/bitcoin/compat.hpp>
#include <bitcoin/bitcoin/constants.hpp>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/error.hpp>
#include <bitcoin/bitcoin/format.hpp>
#include <bitcoin/bitcoin/network/channel.hpp>
#include <bitcoin/bitcoin/network/handshake.hpp>
#include <bitcoin/bitcoin/network/hosts.hpp>
#include <bitcoin/bitcoin/network/network.hpp>
#include <bitcoin/bitcoin/network/protocol.hpp>
#include <bitcoin/bitcoin/network/shared_const_buffer.hpp>
#include <bitcoin/bitcoin/primitives.hpp>
#include <bitcoin/bitcoin/script.hpp>
#include <bitcoin/bitcoin/stealth.hpp>
#include <bitcoin/bitcoin/transaction.hpp>
#include <bitcoin/bitcoin/types.hpp>
#include <bitcoin/bitcoin/utility/assert.hpp>
#include <bitcoin/bitcoin/utility/async_parallel.hpp>
#include <bitcoin/bitcoin/utility/checksum.hpp>
#include <bitcoin/bitcoin/utility/data.hpp>
#include <bitcoin/bitcoin/utility/ec_keys.hpp>
#include <bitcoin/bitcoin/utility/hash.hpp>
#include <bitcoin/bitcoin/utility/logger.hpp>
#include <bitcoin/bitcoin/utility/script_number.hpp>
#include <bitcoin/bitcoin/utility/serializer.hpp>
#include <bitcoin/bitcoin/utility/subscriber.hpp>
#include <bitcoin/bitcoin/utility/threadpool.hpp>
#include <bitcoin/bitcoin/utility/weak_bind.hpp>
#include <bitcoin/bitcoin/wallet/address.hpp>
#include <bitcoin/bitcoin/wallet/amount.hpp>
#include <bitcoin/bitcoin/wallet/base58.hpp>
#include <bitcoin/bitcoin/wallet/hd_keys.hpp>
#include <bitcoin/bitcoin/wallet/key_formats.hpp>
#include <bitcoin/bitcoin/wallet/message.hpp>
#include <bitcoin/bitcoin/wallet/mnemonic.hpp>
#include <bitcoin/bitcoin/wallet/stealth_address.hpp>
#include <bitcoin/bitcoin/wallet/uri.hpp>
#ifdef _MSC_VER
#pragma warning(pop)
#endif

namespace bc = libbitcoin;

#endif

