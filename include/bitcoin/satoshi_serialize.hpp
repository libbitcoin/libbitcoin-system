#ifndef LIBBITCOIN_SATOSHI_SERIALIZE_HPP
#define LIBBITCOIN_SATOSHI_SERIALIZE_HPP

#include <bitcoin/constants.hpp>
#include <bitcoin/format.hpp>
#include <bitcoin/primitives.hpp>
#include <bitcoin/transaction.hpp>
#include <bitcoin/utility/assert.hpp>
#include <bitcoin/utility/logger.hpp>
#include <bitcoin/utility/serializer.hpp>
#include <bitcoin/utility/sha256.hpp>

namespace libbitcoin {

size_t variable_uint_size(uint64_t v);

constexpr size_t command_size = 12;

size_t satoshi_raw_size(const header_type& head);
template <typename Iterator>
void satoshi_save(const header_type& head, Iterator result)
{
    serializer serial;
    serial.write_4_bytes(head.magic);
    serial.write_fixed_string(head.command, command_size);
    serial.write_4_bytes(head.payload_length);
    if (head.checksum != 0)
        serial.write_4_bytes(head.checksum);
    data_chunk raw_data = serial.data();
    BITCOIN_ASSERT(satoshi_raw_size(head) == raw_data.size());
    std::copy(raw_data.begin(), raw_data.end(), result);
}
template <typename Iterator>
void satoshi_load(Iterator first, Iterator last, header_type& head)
{
    data_chunk stream(first, last);
    deserializer deserial(stream);
    head.magic = deserial.read_4_bytes();
    head.command = deserial.read_fixed_string(command_size);
    head.payload_length = deserial.read_4_bytes();
    head.checksum = 0;
    BITCOIN_ASSERT(satoshi_raw_size(head) == stream.size());
}

const std::string satoshi_command(const version_type&);
size_t satoshi_raw_size(const version_type& packet);
template <typename Iterator>
void satoshi_save(const version_type& packet, Iterator result)
{
    serializer serial;
    serial.write_4_bytes(packet.version);
    serial.write_8_bytes(packet.services);
    serial.write_8_bytes(packet.timestamp);
    serial.write_network_address(packet.address_me);
    serial.write_network_address(packet.address_you);
    serial.write_8_bytes(packet.nonce);
    serial.write_string(packet.user_agent);
    serial.write_4_bytes(packet.start_depth);
    serial.write_byte(packet.relay_transactions);
    data_chunk raw_data = serial.data();
    BITCOIN_ASSERT(satoshi_raw_size(packet) == raw_data.size());
    std::copy(raw_data.begin(), raw_data.end(), result);
}
template <typename Iterator>
void satoshi_load(Iterator first, Iterator last, version_type& packet)
{
    data_chunk stream(first, last);
    deserializer deserial(stream);
    packet.version = deserial.read_4_bytes();
    packet.services = deserial.read_8_bytes();
    packet.timestamp = deserial.read_8_bytes();
    packet.address_me = deserial.read_network_address();
    // Ignored field
    packet.address_me.timestamp = 0;
    if (packet.version < 106)
    {
        BITCOIN_ASSERT(stream.size() >= 4 + 8 + 8 + 26);
        return;
    }
    packet.address_you = deserial.read_network_address();
    // Ignored field
    packet.address_you.timestamp = 0;
    packet.nonce = deserial.read_8_bytes();
    packet.user_agent = deserial.read_string();
    if (packet.version < 209)
    {
        BITCOIN_ASSERT(stream.size() >= 4 + 8 + 8 + 26 + 26 + 8 + 1);
        return;
    }
    packet.start_depth = deserial.read_4_bytes();
    if (packet.version < 70001)
    {
        BITCOIN_ASSERT(stream.size() >= 4 + 8 + 8 + 26 + 26 + 8 + 1 + 4);
        return;
    }
    packet.relay_transactions = deserial.read_byte();
    BITCOIN_ASSERT(stream.size() >= 4 + 8 + 8 + 26 + 26 + 8 + 1 + 4 + 1);
    BITCOIN_ASSERT(satoshi_raw_size(packet) == stream.size());
}

const std::string satoshi_command(const verack_type&);
size_t satoshi_raw_size(const verack_type& packet);
template <typename Iterator>
void satoshi_save(const verack_type& packet, Iterator result)
{
    BITCOIN_ASSERT(satoshi_raw_size(packet) == 0);
}
template <typename Iterator>
void satoshi_load(Iterator first, Iterator last, verack_type& packet)
{
    BITCOIN_ASSERT(satoshi_raw_size(packet) == 0);
}

const std::string satoshi_command(const address_type&);
size_t satoshi_raw_size(const address_type& packet);
template <typename Iterator>
void satoshi_save(const address_type& packet, Iterator result)
{
    serializer serial;
    serial.write_variable_uint(packet.addresses.size());
    for (const network_address_type& net_address: packet.addresses)
    {
        serial.write_4_bytes(net_address.timestamp);
        serial.write_network_address(net_address);
    }
    data_chunk raw_data = serial.data();
    BITCOIN_ASSERT(satoshi_raw_size(packet) == raw_data.size());
    std::copy(raw_data.begin(), raw_data.end(), result);
}
template <typename Iterator>
void satoshi_load(Iterator first, Iterator last, address_type& packet)
{
    data_chunk stream(first, last);
    deserializer deserial(stream);
    uint64_t count = deserial.read_variable_uint();
    for (size_t i = 0; i < count; ++i)
    {
        uint32_t timestamp = deserial.read_4_bytes();
        network_address_type addr = deserial.read_network_address();
        addr.timestamp = timestamp;
        packet.addresses.push_back(addr);
    }
    BITCOIN_ASSERT(satoshi_raw_size(packet) == stream.size());
}

const std::string satoshi_command(const get_address_type&);
size_t satoshi_raw_size(const get_address_type& packet);
template <typename Iterator>
void satoshi_save(const get_address_type& packet, Iterator result)
{
    BITCOIN_ASSERT(satoshi_raw_size(packet) == 0);
}
template <typename Iterator>
void satoshi_load(Iterator first, Iterator last, get_address_type& packet)
{
    BITCOIN_ASSERT(satoshi_raw_size(packet) == 0);
}

uint32_t inventory_type_to_number(inventory_type_id inv_type);
inventory_type_id inventory_type_from_number(uint32_t raw_type);

template <typename Message>
size_t raw_size_inventory_impl(const Message& packet)
{
    return variable_uint_size(packet.inventories.size()) +
        36 * packet.inventories.size();
}
template <typename Message, typename Iterator>
void save_inventory_impl(const Message& packet, Iterator result)
{
    serializer serial;
    serial.write_variable_uint(packet.inventories.size());
    for (const inventory_vector_type inv: packet.inventories)
    {
        uint32_t raw_type = inventory_type_to_number(inv.type);
        serial.write_4_bytes(raw_type);
        serial.write_hash(inv.hash);
    }
    data_chunk raw_data = serial.data();
    BITCOIN_ASSERT(satoshi_raw_size(packet) == raw_data.size());
    std::copy(raw_data.begin(), raw_data.end(), result);
}
template <typename Message, typename Iterator>
void load_inventory_impl(Iterator first, Iterator last, Message& packet)
{
    data_chunk stream(first, last);
    deserializer deserial(stream);
    uint64_t count = deserial.read_variable_uint();
    for (size_t i = 0; i < count; ++i)
    {
        inventory_vector_type inv;
        uint32_t raw_type = deserial.read_4_bytes();
        inv.type = inventory_type_from_number(raw_type);
        inv.hash = deserial.read_hash();
        packet.inventories.push_back(inv);
    }
    BITCOIN_ASSERT(satoshi_raw_size(packet) == stream.size());
}

const std::string satoshi_command(const inventory_type&);
size_t satoshi_raw_size(const inventory_type& packet);
template <typename Iterator>
void satoshi_save(const inventory_type& packet, Iterator result)
{
    save_inventory_impl(packet, result);
}
template <typename Iterator>
void satoshi_load(Iterator first, Iterator last, inventory_type& packet)
{
    load_inventory_impl(first, last, packet);
}

const std::string satoshi_command(const get_data_type&);
size_t satoshi_raw_size(const get_data_type& packet);
template <typename Iterator>
void satoshi_save(const get_data_type& packet, Iterator result)
{
    save_inventory_impl(packet, result);
}
template <typename Iterator>
void satoshi_load(Iterator first, Iterator last, get_data_type& packet)
{
    load_inventory_impl(first, last, packet);
}

const std::string satoshi_command(const get_blocks_type&);
size_t satoshi_raw_size(const get_blocks_type& packet);
template <typename Iterator>
void satoshi_save(const get_blocks_type& packet, Iterator result)
{
    serializer serial;
    serial.write_4_bytes(protocol_version);
    serial.write_variable_uint(packet.start_hashes.size());
    for (hash_digest start_hash: packet.start_hashes)
        serial.write_hash(start_hash);
    serial.write_hash(packet.hash_stop);
    data_chunk raw_data = serial.data();
    BITCOIN_ASSERT(satoshi_raw_size(packet) == raw_data.size());
    std::copy(raw_data.begin(), raw_data.end(), result);
}
template <typename Iterator>
void satoshi_load(Iterator first, Iterator last, get_blocks_type& packet)
{
    data_chunk stream(first, last);
    deserializer deserial(stream);
    // Discard protocol version because it is stupid
    deserial.read_4_bytes();
    uint32_t count = deserial.read_variable_uint();
    for (size_t i = 0; i < count; ++i)
    {
        hash_digest start_hash = deserial.read_hash();
        packet.start_hashes.push_back(start_hash);
    }
    packet.hash_stop = deserial.read_hash();
    BITCOIN_ASSERT(satoshi_raw_size(packet) == stream.size());
}

void save_transaction(
    serializer& serial, const transaction_type& packet);

data_chunk read_raw_script(deserializer& deserial);
script read_script(deserializer& deserial);
transaction_type read_transaction(
    deserializer& deserial, transaction_type& packet);

const std::string satoshi_command(const transaction_type&);
size_t satoshi_raw_size(const transaction_type& packet);
template <typename Iterator>
void satoshi_save(const transaction_type& packet, Iterator result)
{
    serializer serial;
    save_transaction(serial, packet);
    data_chunk raw_data = serial.data();
    BITCOIN_ASSERT(satoshi_raw_size(packet) == raw_data.size());
    std::copy(raw_data.begin(), raw_data.end(), result);
}
template <typename Iterator>
void satoshi_load(Iterator first, Iterator last, transaction_type& packet)
{
    data_chunk stream(first, last);
    deserializer deserial(stream);
    read_transaction(deserial, packet);
    BITCOIN_ASSERT(satoshi_raw_size(packet) == stream.size());
}

const std::string satoshi_command(const block_type&);
size_t satoshi_raw_size(const block_type& packet);
template <typename Iterator>
void satoshi_save(const block_type& packet, Iterator result)
{
    serializer serial;
    serial.write_4_bytes(packet.version);
    serial.write_hash(packet.previous_block_hash);
    serial.write_hash(packet.merkle);
    serial.write_4_bytes(packet.timestamp);
    serial.write_4_bytes(packet.bits);
    serial.write_4_bytes(packet.nonce);
    serial.write_variable_uint(packet.transactions.size());
    for (const transaction_type& tx: packet.transactions)
        save_transaction(serial, tx);
    data_chunk raw_data = serial.data();
    BITCOIN_ASSERT(satoshi_raw_size(packet) == raw_data.size());
    std::copy(raw_data.begin(), raw_data.end(), result);
}
template <typename Iterator>
void satoshi_load(Iterator first, Iterator last, block_type& packet)
{
    data_chunk stream(first, last);
    deserializer deserial(stream);
    packet.version = deserial.read_4_bytes();
    packet.previous_block_hash = deserial.read_hash();
    packet.merkle = deserial.read_hash();
    packet.timestamp = deserial.read_4_bytes();
    packet.bits = deserial.read_4_bytes();
    packet.nonce = deserial.read_4_bytes();
    uint64_t tx_count = deserial.read_variable_uint();
    for (size_t tx_i = 0; tx_i < tx_count; ++tx_i)
    {
        transaction_type tx;
        read_transaction(deserial, tx);
        packet.transactions.push_back(std::move(tx));
    }
    BITCOIN_ASSERT(satoshi_raw_size(packet) == stream.size());
}

const std::string satoshi_command(const ping_type&);
size_t satoshi_raw_size(const ping_type& packet);
template <typename Iterator>
void satoshi_save(const ping_type& packet, Iterator result)
{
    serializer serial;
    serial.write_8_bytes(packet.nonce);
    data_chunk raw_data = serial.data();
    BITCOIN_ASSERT(satoshi_raw_size(packet) == raw_data.size());
    std::copy(raw_data.begin(), raw_data.end(), result);
}
template <typename Iterator>
void satoshi_load(Iterator first, Iterator last, ping_type& packet)
{
    data_chunk stream(first, last);
    deserializer deserial(stream);
    packet.nonce = deserial.read_8_bytes();
    BITCOIN_ASSERT(satoshi_raw_size(packet) == stream.size());
}

const std::string satoshi_command(const pong_type&);
size_t satoshi_raw_size(const pong_type& packet);
template <typename Iterator>
void satoshi_save(const pong_type& packet, Iterator result)
{
    serializer serial;
    serial.write_8_bytes(packet.nonce);
    data_chunk raw_data = serial.data();
    BITCOIN_ASSERT(satoshi_raw_size(packet) == raw_data.size());
    std::copy(raw_data.begin(), raw_data.end(), result);
}
template <typename Iterator>
void satoshi_load(Iterator first, Iterator last, pong_type& packet)
{
    data_chunk stream(first, last);
    deserializer deserial(stream);
    packet.nonce = deserial.read_8_bytes();
    BITCOIN_ASSERT(satoshi_raw_size(packet) == stream.size());
}

} // libbitcoin

#endif

