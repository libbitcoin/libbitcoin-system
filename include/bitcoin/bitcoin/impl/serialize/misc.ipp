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
#ifndef LIBBITCOIN_MISC_IPP
#define LIBBITCOIN_MISC_IPP

#include <bitcoin/bitcoin/constants.hpp>
#include <bitcoin/bitcoin/utility/assert.hpp>
#include <bitcoin/bitcoin/utility/serializer.hpp>

namespace libbitcoin {

// message headers
template <typename Iterator>
Iterator satoshi_save(const header_type& head, Iterator result)
{
    auto serial = make_serializer(result);
    serial.write_4_bytes(head.magic);
    serial.write_fixed_string(head.command, command_size);
    serial.write_4_bytes(head.payload_length);
    if (head.checksum != 0)
        serial.write_4_bytes(head.checksum);
    return serial.iterator();
}
template <typename Iterator>
void satoshi_load(const Iterator first, const Iterator last,
    header_type& head)
{
    auto deserial = make_deserializer(first, last);
    head.magic = deserial.read_4_bytes();
    head.command = deserial.read_fixed_string(command_size);
    head.payload_length = deserial.read_4_bytes();
    head.checksum = 0;
}

// version messages
template <typename Iterator>
Iterator satoshi_save(const version_type& packet, Iterator result)
{
    auto serial = make_serializer(result);
    serial.write_4_bytes(packet.version);
    serial.write_8_bytes(packet.services);
    serial.write_8_bytes(packet.timestamp);
    serial.write_network_address(packet.address_me);
    serial.write_network_address(packet.address_you);
    serial.write_8_bytes(packet.nonce);
    serial.write_string(packet.user_agent);
    serial.write_4_bytes(packet.start_height);
    return serial.iterator();
}
template <typename Iterator>
void satoshi_load(const Iterator first, const Iterator last,
    version_type& packet)
{
    auto deserial = make_deserializer(first, last);
    packet.version = deserial.read_4_bytes();
    packet.services = deserial.read_8_bytes();
    packet.timestamp = deserial.read_8_bytes();
    packet.address_me = deserial.read_network_address();
    // Ignored field
    packet.address_me.timestamp = 0;
    if (packet.version < 106)
    {
        BITCOIN_ASSERT(std::distance(first, last) >= 46);
        return;
    }
    packet.address_you = deserial.read_network_address();
    // Ignored field
    packet.address_you.timestamp = 0;
    packet.nonce = deserial.read_8_bytes();
    packet.user_agent = deserial.read_string();
    if (packet.version < 209)
    {
        BITCOIN_ASSERT(std::distance(first, last) >= 46 + 26 + 8 + 1);
        return;
    }
    packet.start_height = deserial.read_4_bytes();
    BITCOIN_ASSERT(std::distance(first, last) >= 81 + 4);
}

// verack messages
template <typename Iterator>
Iterator satoshi_save(const verack_type& DEBUG_ONLY(packet), Iterator result)
{
    BITCOIN_ASSERT(satoshi_raw_size(packet) == 0);
    return result;
}
template <typename Iterator>
void satoshi_load(const Iterator, const Iterator, 
    verack_type& DEBUG_ONLY(packet))
{
    BITCOIN_ASSERT(satoshi_raw_size(packet) == 0);
}

// addr messages
template <typename Iterator>
Iterator satoshi_save(const address_type& packet, Iterator result)
{
    auto serial = make_serializer(result);
    serial.write_variable_uint(packet.addresses.size());
    for (const network_address_type& net_address: packet.addresses)
    {
        serial.write_4_bytes(net_address.timestamp);
        serial.write_network_address(net_address);
    }
    return serial.iterator();
}
template <typename Iterator>
void satoshi_load(const Iterator first, const Iterator last,
    address_type& packet)
{
    auto deserial = make_deserializer(first, last);
    uint64_t count = deserial.read_variable_uint();
    for (size_t i = 0; i < count; ++i)
    {
        uint32_t timestamp = deserial.read_4_bytes();
        network_address_type addr = deserial.read_network_address();
        addr.timestamp = timestamp;
        packet.addresses.push_back(addr);
    }
    BITCOIN_ASSERT(deserial.iterator() == first + satoshi_raw_size(packet));
    BITCOIN_ASSERT(deserial.iterator() == last);
}

// getaddr messages
template <typename Iterator>
Iterator satoshi_save(const get_address_type& DEBUG_ONLY(packet),
    Iterator result)
{
    BITCOIN_ASSERT(satoshi_raw_size(packet) == 0);
    return result;
}
template <typename Iterator>
void satoshi_load(const Iterator, const Iterator, 
    get_address_type& DEBUG_ONLY(packet))
{
    BITCOIN_ASSERT(satoshi_raw_size(packet) == 0);
}

// inventory related stuff
BC_API uint32_t inventory_type_to_number(inventory_type_id inv_type);
BC_API inventory_type_id inventory_type_from_number(uint32_t raw_type);

template <typename Message>
size_t raw_size_inventory_impl(const Message& packet)
{
    return variable_uint_size(packet.inventories.size()) +
        36 * packet.inventories.size();
}
template <typename Message, typename Iterator>
Iterator save_inventory_impl(const Message& packet, Iterator result)
{
    auto serial = make_serializer(result);
    serial.write_variable_uint(packet.inventories.size());
    for (const inventory_vector_type inv: packet.inventories)
    {
        uint32_t raw_type = inventory_type_to_number(inv.type);
        serial.write_4_bytes(raw_type);
        serial.write_hash(inv.hash);
    }
    return serial.iterator();
}
template <typename Message, typename Iterator>
void load_inventory_impl(const Iterator first, const Iterator last,
    Message& packet)
{
    auto deserial = make_deserializer(first, last);
    uint64_t count = deserial.read_variable_uint();
    for (size_t i = 0; i < count; ++i)
    {
        inventory_vector_type inv;
        uint32_t raw_type = deserial.read_4_bytes();
        inv.type = inventory_type_from_number(raw_type);
        inv.hash = deserial.read_hash();
        packet.inventories.push_back(inv);
    }
    BITCOIN_ASSERT(deserial.iterator() == first + satoshi_raw_size(packet));
}

// inv messages
template <typename Iterator>
Iterator satoshi_save(const inventory_type& packet, Iterator result)
{
    return save_inventory_impl(packet, result);
}
template <typename Iterator>
void satoshi_load(const Iterator first, const Iterator last,
    inventory_type& packet)
{
    load_inventory_impl(first, last, packet);
}

// getdata messages
template <typename Iterator>
Iterator satoshi_save(const get_data_type& packet, Iterator result)
{
    return save_inventory_impl(packet, result);
}
template <typename Iterator>
void satoshi_load(const Iterator first, const Iterator last,
    get_data_type& packet)
{
    load_inventory_impl(first, last, packet);
}

// getblocks messages
template <typename Iterator>
Iterator satoshi_save(const get_blocks_type& packet, Iterator result)
{
    auto serial = make_serializer(result);
    serial.write_4_bytes(protocol_version);
    serial.write_variable_uint(packet.start_hashes.size());
    for (hash_digest start_hash: packet.start_hashes)
        serial.write_hash(start_hash);
    serial.write_hash(packet.hash_stop);
    return serial.iterator();
}
template <typename Iterator>
void satoshi_load(const Iterator first, const Iterator last,
    get_blocks_type& packet)
{
    auto deserial = make_deserializer(first, last);
    // Discard protocol version because it is stupid
    deserial.read_4_bytes();
    
    // Note: changed to uint64_t to preclude possible loss of data.
    uint64_t count = deserial.read_variable_uint();
    for (uint64_t i = 0; i < count; ++i)
    {
        hash_digest start_hash = deserial.read_hash();
        packet.start_hashes.push_back(start_hash);
    }
    packet.hash_stop = deserial.read_hash();
    BITCOIN_ASSERT(deserial.iterator() == first + satoshi_raw_size(packet));
    BITCOIN_ASSERT(deserial.iterator() == last);
}

// ping messages
template <typename Iterator>
Iterator satoshi_save(const ping_type& packet, Iterator result)
{
    auto serial = make_serializer(result);
    serial.write_8_bytes(packet.nonce);
    return serial.iterator();
}
template <typename Iterator>
void satoshi_load(const Iterator first, const Iterator last,
    ping_type& packet)
{
    auto deserial = make_deserializer(first, last);
    packet.nonce = deserial.read_8_bytes();
    BITCOIN_ASSERT(deserial.iterator() == first + satoshi_raw_size(packet));
    BITCOIN_ASSERT(deserial.iterator() == last);
}

// pong messages
template <typename Iterator>
Iterator satoshi_save(const pong_type& packet, Iterator result)
{
    auto serial = make_serializer(result);
    serial.write_8_bytes(packet.nonce);
    return serial.iterator();
}
template <typename Iterator>
void satoshi_load(const Iterator first, const Iterator last,
    pong_type& packet)
{
    auto deserial = make_deserializer(first, last);
    packet.nonce = deserial.read_8_bytes();
    BITCOIN_ASSERT(deserial.iterator() == first + satoshi_raw_size(packet));
    BITCOIN_ASSERT(deserial.iterator() == last);
}

} // libbitcoin

#endif

