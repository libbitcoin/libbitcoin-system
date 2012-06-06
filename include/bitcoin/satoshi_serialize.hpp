#ifndef LIBBITCOIN_SATOSHI_SERIALIZE_H
#define LIBBITCOIN_SATOSHI_SERIALIZE_H

#include <bitcoin/constants.hpp>
#include <bitcoin/messages.hpp>
#include <bitcoin/transaction.hpp>
#include <bitcoin/utility/assert.hpp>
#include <bitcoin/utility/serializer.hpp>

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

namespace message {

constexpr size_t command_size = 12;

size_t satoshi_raw_size(const header& head)
{
    return 20 + (head.checksum == 0 ? 0 : 4);
}
template <typename Iterator>
void satoshi_save(const header& head, Iterator result)
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
void satoshi_load(Iterator first, Iterator last, header& head)
{
    data_chunk stream(first, last);
    deserializer deserial(stream);
    head.magic = deserial.read_4_bytes();
    head.command = deserial.read_fixed_string(command_size);
    head.payload_length = deserial.read_4_bytes();
    head.checksum = 0;
    BITCOIN_ASSERT(satoshi_raw_size(head) == stream.size());
}

const std::string satoshi_command(const version&)
{
    return "version";
}
size_t satoshi_raw_size(const version& packet)
{
    return 84 +
        variable_uint_size(packet.user_agent.size()) +
        packet.user_agent.size();
}
template <typename Iterator>
void satoshi_save(const version& packet, Iterator result)
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
    data_chunk raw_data = serial.data();
    BITCOIN_ASSERT(satoshi_raw_size(packet) == raw_data.size());
    std::copy(raw_data.begin(), raw_data.end(), result);
}
template <typename Iterator>
void satoshi_load(Iterator first, Iterator last, version& packet)
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
    BITCOIN_ASSERT(stream.size() >= 4 + 8 + 8 + 26 + 26 + 8 + 1 + 4);
    BITCOIN_ASSERT(satoshi_raw_size(packet) == stream.size());
}

const std::string satoshi_command(const verack&)
{
    return "verack";
}
size_t satoshi_raw_size(const verack& packet)
{
    return 0;
}
template <typename Iterator>
void satoshi_save(const verack& packet, Iterator result)
{
    BITCOIN_ASSERT(satoshi_raw_size(packet) == 0);
}
template <typename Iterator>
void satoshi_load(Iterator first, Iterator last, verack& packet)
{
    BITCOIN_ASSERT(satoshi_raw_size(packet) == 0);
}

const std::string satoshi_command(const address&)
{
    return "addr";
}
size_t satoshi_raw_size(const address& packet)
{
    return variable_uint_size(packet.addresses.size()) +
        30 * packet.addresses.size();
}
template <typename Iterator>
void satoshi_save(const address& packet, Iterator result)
{
    serializer serial;
    serial.write_variable_uint(packet.addresses.size());
    for (const message::network_address& net_address: packet.addresses)
    {
        serial.write_4_bytes(net_address.timestamp);
        serial.write_network_address(net_address);
    }
    data_chunk raw_data = serial.data();
    BITCOIN_ASSERT(satoshi_raw_size(packet) == raw_data.size());
    std::copy(raw_data.begin(), raw_data.end(), result);
}
template <typename Iterator>
void satoshi_load(Iterator first, Iterator last, address& packet)
{
    data_chunk stream(first, last);
    deserializer deserial(stream);
    uint64_t count = deserial.read_variable_uint();
    for (size_t i = 0; i < count; ++i)
    {
        uint32_t timestamp = deserial.read_4_bytes();
        message::network_address addr = deserial.read_network_address();
        addr.timestamp = timestamp;
        packet.addresses.push_back(addr);
    }
    BITCOIN_ASSERT(satoshi_raw_size(packet) == stream.size());
}

const std::string satoshi_command(const get_address&)
{
    return "getaddr";
}
size_t satoshi_raw_size(const get_address& packet)
{
    return 0;
}
template <typename Iterator>
void satoshi_save(const get_address& packet, Iterator result)
{
    BITCOIN_ASSERT(satoshi_raw_size(packet) == 0);
}
template <typename Iterator>
void satoshi_load(Iterator first, Iterator last, get_address& packet)
{
    BITCOIN_ASSERT(satoshi_raw_size(packet) == 0);
}

uint32_t inventory_type_to_number(message::inventory_type inv_type)
{
    switch (inv_type)
    {
        case message::inventory_type::error:
        case message::inventory_type::none:
        default:
            return 0;

        case message::inventory_type::transaction:
            return 1;

        case message::inventory_type::block:
            return 2;
    }
}

message::inventory_type inventory_type_from_number(uint32_t raw_type)
{
    switch (raw_type)
    {
        case 0:
            return message::inventory_type::error;
        case 1:
            return message::inventory_type::transaction;
        case 2:
            return message::inventory_type::block;
        default:
            return message::inventory_type::none;
    }
}

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
    for (const message::inventory_vector inv: packet.inventories)
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
        message::inventory_vector inv;
        uint32_t raw_type = deserial.read_4_bytes();
        inv.type = inventory_type_from_number(raw_type);
        inv.hash = deserial.read_hash();
        packet.inventories.push_back(inv);
    }
    BITCOIN_ASSERT(satoshi_raw_size(packet) == stream.size());
}

const std::string satoshi_command(const inventory&)
{
    return "inv";
}
size_t satoshi_raw_size(const inventory& packet)
{
    return raw_size_inventory_impl(packet);
}
template <typename Iterator>
void satoshi_save(const inventory& packet, Iterator result)
{
    save_inventory_impl(packet, result);
}
template <typename Iterator>
void satoshi_load(Iterator first, Iterator last, inventory& packet)
{
    load_inventory_impl(first, last, packet);
}

const std::string satoshi_command(const get_data&)
{
    return "getdata";
}
size_t satoshi_raw_size(const get_data& packet)
{
    return raw_size_inventory_impl(packet);
}
template <typename Iterator>
void satoshi_save(const get_data& packet, Iterator result)
{
    save_inventory_impl(packet, result);
}
template <typename Iterator>
void satoshi_load(Iterator first, Iterator last, get_data& packet)
{
    load_inventory_impl(first, last, packet);
}

const std::string satoshi_command(const get_blocks&)
{
    return "getblocks";
}
size_t satoshi_raw_size(const get_blocks& packet)
{
    return 36 +
        variable_uint_size(packet.start_hashes.size()) +
        32 * packet.start_hashes.size();
}
template <typename Iterator>
void satoshi_save(const get_blocks& packet, Iterator result)
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
void satoshi_load(Iterator first, Iterator last, get_blocks& packet)
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
    serializer& serial, const message::transaction& packet)
{
    serial.write_4_bytes(packet.version);
    serial.write_variable_uint(packet.inputs.size());
    for (const message::transaction_input& input: packet.inputs)
    {
        serial.write_hash(input.previous_output.hash);
        serial.write_4_bytes(input.previous_output.index);
        data_chunk raw_script = save_script(input.input_script);
        serial.write_variable_uint(raw_script.size());
        serial.write_data(raw_script);
        serial.write_4_bytes(input.sequence);
    }
    serial.write_variable_uint(packet.outputs.size());
    for (const message::transaction_output& output: packet.outputs)
    {
        serial.write_8_bytes(output.value);
        data_chunk raw_script = save_script(output.output_script);
        serial.write_variable_uint(raw_script.size());
        serial.write_data(raw_script);
    }
    serial.write_4_bytes(packet.locktime);
}

data_chunk read_raw_script(deserializer& deserial)
{
    data_chunk raw_script;
    uint64_t script_length = deserial.read_variable_uint();
    return deserial.read_data(script_length);
}

script read_script(deserializer& deserial)
{
    data_chunk raw_script = read_raw_script(deserial);
    BITCOIN_ASSERT(raw_script == save_script(parse_script(raw_script)));
    // Eventually plan to move parse_script to inside here
    return parse_script(raw_script);
}

message::transaction read_transaction(
    deserializer& deserial, message::transaction& packet)
{
    packet.version = deserial.read_4_bytes();
    uint64_t tx_in_count = deserial.read_variable_uint();
    for (size_t tx_in_i = 0; tx_in_i < tx_in_count; ++tx_in_i)
    {
        message::transaction_input input;
        input.previous_output.hash = deserial.read_hash();
        input.previous_output.index = deserial.read_4_bytes();
        if (previous_output_is_null(input.previous_output))
            input.input_script = coinbase_script(read_raw_script(deserial));
        else
            input.input_script = read_script(deserial);
        input.sequence = deserial.read_4_bytes();
        packet.inputs.push_back(input);
    }
    uint64_t tx_out_count = deserial.read_variable_uint();
    for (size_t tx_out_i = 0; tx_out_i < tx_out_count; ++tx_out_i)
    {
        message::transaction_output output;
        output.value = deserial.read_8_bytes();
        output.output_script = read_script(deserial);
        packet.outputs.push_back(output);
    }
    packet.locktime = deserial.read_4_bytes();
    return packet;
}

const std::string satoshi_command(const transaction&)
{
    return "tx";
}
size_t satoshi_raw_size(const transaction& packet)
{
    size_t tx_size = 8;
    tx_size += variable_uint_size(packet.inputs.size());
    for (const message::transaction_input& input: packet.inputs)
    {
        data_chunk raw_script = save_script(input.input_script);
        tx_size += 40 +
            variable_uint_size(raw_script.size()) +
            raw_script.size();
    }
    tx_size += variable_uint_size(packet.outputs.size());
    for (const message::transaction_output& output: packet.outputs)
    {
        data_chunk raw_script = save_script(output.output_script);
        tx_size += 8 +
            variable_uint_size(raw_script.size()) +
            raw_script.size();
    }
    return tx_size;
}
template <typename Iterator>
void satoshi_save(const transaction& packet, Iterator result)
{
    serializer serial;
    save_transaction(serial, packet);
    data_chunk raw_data = serial.data();
    BITCOIN_ASSERT(satoshi_raw_size(packet) == raw_data.size());
    std::copy(raw_data.begin(), raw_data.end(), result);
}
template <typename Iterator>
void satoshi_load(Iterator first, Iterator last, transaction& packet)
{
    data_chunk stream(first, last);
    deserializer deserial(stream);
    read_transaction(deserial, packet);
    BITCOIN_ASSERT(satoshi_raw_size(packet) == stream.size());
}

const std::string satoshi_command(const block&)
{
    return "block";
}
size_t satoshi_raw_size(const block& packet)
{
    size_t block_size = 80 + variable_uint_size(packet.transactions.size());
    for (const message::transaction& tx: packet.transactions)
        block_size += satoshi_raw_size(tx);
    return block_size;
}
template <typename Iterator>
void satoshi_save(const block& packet, Iterator result)
{
    serializer serial;
    serial.write_4_bytes(packet.version);
    serial.write_hash(packet.previous_block_hash);
    serial.write_hash(packet.merkle);
    serial.write_4_bytes(packet.timestamp);
    serial.write_4_bytes(packet.bits);
    serial.write_4_bytes(packet.nonce);
    serial.write_variable_uint(packet.transactions.size());
    for (const message::transaction& tx: packet.transactions)
        save_transaction(serial, tx);
    data_chunk raw_data = serial.data();
    BITCOIN_ASSERT(satoshi_raw_size(packet) == raw_data.size());
    std::copy(raw_data.begin(), raw_data.end(), result);
}
template <typename Iterator>
void satoshi_load(Iterator first, Iterator last, block& packet)
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
        message::transaction tx;
        read_transaction(deserial, tx);
        packet.transactions.push_back(std::move(tx));
    }
    BITCOIN_ASSERT(satoshi_raw_size(packet) == stream.size());
}

const std::string satoshi_command(const ping&)
{
    return "ping";
}
size_t satoshi_raw_size(const ping& packet)
{
    return 0;
}
template <typename Iterator>
void satoshi_save(const ping& packet, Iterator result)
{
    BITCOIN_ASSERT(satoshi_raw_size(packet) == 0);
}
template <typename Iterator>
void satoshi_load(Iterator first, Iterator last, ping& packet)
{
    BITCOIN_ASSERT(satoshi_raw_size(packet) == 0);
}

} // message
} // libbitcoin

#endif

