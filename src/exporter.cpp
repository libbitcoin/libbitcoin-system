#include <bitcoin/exporter.hpp>

#include <boost/assert.hpp>

#include <bitcoin/messages.hpp>
#include <bitcoin/transaction.hpp>
#include <bitcoin/utility/assert.hpp>
#include <bitcoin/utility/logger.hpp>

namespace libbitcoin {

constexpr size_t command_size = 12;

const char* satoshi_exporter::command_name(const message::version&) const
{
    return "version";
}
const char* satoshi_exporter::command_name(const message::verack&) const
{
    return "verack";
}
const char* satoshi_exporter::command_name(const message::address&) const
{
    return "addr";
}
const char* satoshi_exporter::command_name(const message::get_address&) const
{
    return "getaddr";
}
const char* satoshi_exporter::command_name(const message::inventory&) const
{
    return "inv";
}
const char* satoshi_exporter::command_name(const message::get_data&) const
{
    return "getdata";
}
const char* satoshi_exporter::command_name(const message::get_blocks&) const
{
    return "getblocks";
}
const char* satoshi_exporter::command_name(const message::block&) const
{
    return "block";
}
const char* satoshi_exporter::command_name(const message::transaction&) const
{
    return "tx";
}
const char* satoshi_exporter::command_name(const message::ping&) const
{
    return "ping";
}

data_chunk satoshi_exporter::save(
    const message::header& packet_header) const
{
    serializer serial;
    serial.write_4_bytes(packet_header.magic);
    serial.write_fixed_string(packet_header.command, command_size);
    serial.write_4_bytes(packet_header.payload_length);
    if (packet_header.checksum != 0)
        serial.write_4_bytes(packet_header.checksum);
    return serial.data();
}

data_chunk satoshi_exporter::save(const message::version& packet) const
{
    serializer serial;
    serial.write_4_bytes(packet.version);
    serial.write_8_bytes(packet.services);
    serial.write_8_bytes(packet.timestamp);
    serial.write_network_address(packet.address_me);
    serial.write_network_address(packet.address_you);
    serial.write_8_bytes(packet.nonce);
    serial.write_string(packet.user_agent);
    serial.write_4_bytes(packet.start_height);
    return serial.data();
}

data_chunk satoshi_exporter::save(const message::verack&) const
{
    return data_chunk();
}

data_chunk satoshi_exporter::save(const message::address& packet) const
{
    serializer serial;
    serial.write_variable_uint(packet.addresses.size());
    for (const message::network_address& net_address: packet.addresses)
    {
        serial.write_4_bytes(net_address.timestamp);
        serial.write_network_address(net_address);
    }
    return serial.data();
}

data_chunk satoshi_exporter::save(const message::get_address&) const
{
    return data_chunk();
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

template <typename Message>
void save_inventory_impl(serializer& serial, const Message& packet)
{
    serial.write_variable_uint(packet.inventories.size());
    for (const message::inventory_vector inv: packet.inventories)
    {
        uint32_t raw_type = inventory_type_to_number(inv.type);
        serial.write_4_bytes(raw_type);
        serial.write_hash(inv.hash);
    }
}

data_chunk satoshi_exporter::save(const message::inventory& packet) const
{
    serializer serial;
    save_inventory_impl(serial, packet);
    return serial.data();
}

data_chunk satoshi_exporter::save(const message::get_data& packet) const
{
    serializer serial;
    save_inventory_impl(serial, packet);
    return serial.data();
}

data_chunk satoshi_exporter::save(
    const message::get_blocks& packet) const
{
    serializer serial;
    serial.write_4_bytes(protocol_version);
    serial.write_variable_uint(packet.start_hashes.size());
    for (hash_digest start_hash: packet.start_hashes)
        serial.write_hash(start_hash);
    serial.write_hash(packet.hash_stop);
    return serial.data();
}

void save_transaction(serializer& serial, const message::transaction& packet)
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

data_chunk satoshi_exporter::save(const message::transaction& packet) const
{
    serializer serial;
    save_transaction(serial, packet);
    return serial.data();
}

data_chunk satoshi_exporter::save(const message::block& packet) const
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
    return serial.data();
}

data_chunk satoshi_exporter::save(const message::ping&) const
{
    return data_chunk();
}

message::header satoshi_exporter::load_header(const data_chunk& stream)  const
{
    message::header header;
    deserializer deserial(stream);
    header.magic = deserial.read_4_bytes();
    header.command = deserial.read_fixed_string(command_size);
    header.payload_length = deserial.read_4_bytes();
    header.checksum = 0;
    return header;
}

message::version satoshi_exporter::load_version(const data_chunk& stream) const
{
    message::version packet;
    deserializer deserial(stream);
    packet.version = deserial.read_4_bytes();
    packet.services = deserial.read_8_bytes();
    packet.timestamp = deserial.read_8_bytes();
    packet.address_me = deserial.read_network_address();
    // Ignored field
    packet.address_me.timestamp = 0;
    if (packet.version < 106) {
        BITCOIN_ASSERT(stream.size() == 4 + 8 + 8 + 26);
        return packet;
    }
    packet.address_you = deserial.read_network_address();
    // Ignored field
    packet.address_you.timestamp = 0;
    packet.nonce = deserial.read_8_bytes();
    packet.user_agent = deserial.read_string();
    if (packet.version < 209) {
        BITCOIN_ASSERT(stream.size() == 4 + 8 + 8 + 26 + 26 + 8 + 1);
        return packet;
    }
    packet.start_height = deserial.read_4_bytes();
    BITCOIN_ASSERT(stream.size() == 4 + 8 + 8 + 26 + 26 + 8 + 1 + 4);
    return packet;
}

message::verack satoshi_exporter::load_verack(const data_chunk&) const
{
    return message::verack();
}

message::address satoshi_exporter::load_address(const data_chunk& stream) const
{
    message::address packet;
    deserializer deserial(stream);
    uint64_t count = deserial.read_variable_uint();
    for (size_t i = 0; i < count; ++i)
    {
        uint32_t timestamp = deserial.read_4_bytes();
        message::network_address addr = deserial.read_network_address();
        addr.timestamp = timestamp;
        packet.addresses.push_back(addr);
    }
    return packet;
}

message::get_address satoshi_exporter::load_get_address(
    const data_chunk&) const
{
    return message::get_address();
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
void load_inventory_impl(deserializer& deserial, Message& packet)
{
    uint64_t count = deserial.read_variable_uint();
    for (size_t i = 0; i < count; ++i)
    {
        message::inventory_vector inv;
        uint32_t raw_type = deserial.read_4_bytes();
        inv.type = inventory_type_from_number(raw_type);
        inv.hash = deserial.read_hash();
        packet.inventories.push_back(inv);
    }
}

message::inventory satoshi_exporter::load_inventory(
    const data_chunk& stream) const
{
    message::inventory packet;
    deserializer deserial(stream);
    load_inventory_impl(deserial, packet);
    return packet;
}

message::get_data satoshi_exporter::load_get_data(
    const data_chunk& stream) const
{
    message::get_data packet;
    deserializer deserial(stream);
    load_inventory_impl(deserial, packet);
    return packet;
}

message::get_blocks satoshi_exporter::load_get_blocks(
    const data_chunk& stream) const
{
    message::get_blocks packet;
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
    return packet;
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

message::transaction read_transaction(deserializer& deserial)
{
    message::transaction packet;
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

message::transaction satoshi_exporter::load_transaction(
    const data_chunk& stream) const
{
    deserializer deserial(stream);
    return read_transaction(deserial);
}

message::block satoshi_exporter::load_block(const data_chunk& stream) const
{
    message::block packet;
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
        const message::transaction tx = read_transaction(deserial);
        packet.transactions.push_back(tx);
    }
    return packet;
}

message::ping satoshi_exporter::load_ping(const data_chunk&) const
{
    return message::ping();
}

bool satoshi_exporter::verify_header(const message::header& header_msg) const
{
    if (header_msg.magic != magic_value)
        return false;
    if (header_msg.command == "version")
    {
        if (header_msg.payload_length != 85)
            return false;
    }
    else if (header_msg.command == "verack"
            || header_msg.command == "getaddr"
            || header_msg.command == "ping")
    {
        if (header_msg.payload_length != 0)
            return false;
    }
    else if (header_msg.command == "inv"
            || header_msg.command == "addr"
            || header_msg.command == "getdata"
            || header_msg.command == "getblocks"
            || header_msg.command == "getheaders"
            || header_msg.command == "tx"
            || header_msg.command == "block"
            || header_msg.command == "headers"
            || header_msg.command == "alert")
    {
        // Should check if sizes make sense
        // i.e for addr should be multiple of 30x + 1 byte
        // Also then add ASSERTS to handlers above.
    }
    else
    {
        // Unknown header
        return false;
    }
    return true;
}

bool satoshi_exporter::is_checksum_used(
    const message::header& header_msg) const
{
    return header_msg.command != "version" && header_msg.command != "verack";
}
uint32_t satoshi_exporter::load_checksum(const data_chunk& chunk) const
{
    deserializer deserial(chunk);
    return deserial.read_4_bytes();
}
bool satoshi_exporter::verify_checksum(const message::header& header_msg,
        const data_chunk& stream) const
{
    if (!is_checksum_used(header_msg))
        return true;
    return header_msg.checksum == generate_sha256_checksum(stream);
}

} // libbitcoin

