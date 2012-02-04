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
const char* satoshi_exporter::command_name(const message::get_address&) const
{
    return "getaddr";
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

data_chunk satoshi_exporter::save(const message::version& version) const
{
    serializer payload;
    payload.write_4_bytes(version.version);
    payload.write_8_bytes(version.services);
    payload.write_8_bytes(version.timestamp);
    payload.write_network_address(version.address_me);
    payload.write_network_address(version.address_you);
    payload.write_8_bytes(version.nonce);
    payload.write_string(version.user_agent);
    payload.write_4_bytes(version.start_height);
    return payload.data();
}

data_chunk satoshi_exporter::save(const message::verack&) const
{
    return data_chunk();
}

data_chunk satoshi_exporter::save(const message::get_address&) const
{
    return data_chunk();
}

data_chunk satoshi_exporter::save(
    const message::get_blocks& getblocks) const
{
    serializer payload;
    payload.write_4_bytes(31900);
    payload.write_variable_uint(getblocks.start_hashes.size());
    for (hash_digest start_hash: getblocks.start_hashes)
        payload.write_hash(start_hash);
    payload.write_hash(getblocks.hash_stop);
    return payload.data();
}

data_chunk satoshi_exporter::save(const message::block& block) const
{
    return data_chunk();
}

data_chunk satoshi_exporter::save(const message::transaction& tx) const
{
    serializer payload;
    payload.write_4_bytes(tx.version);
    payload.write_variable_uint(tx.inputs.size());
    for (const message::transaction_input& input: tx.inputs)
    {
        payload.write_hash(input.previous_output.hash);
        payload.write_4_bytes(input.previous_output.index);
        data_chunk raw_script = save_script(input.input_script);
        payload.write_variable_uint(raw_script.size());
        payload.write_data(raw_script);
        payload.write_4_bytes(input.sequence);
    }
    payload.write_variable_uint(tx.outputs.size());
    for (const message::transaction_output& output: tx.outputs)
    {
        payload.write_8_bytes(output.value);
        data_chunk raw_script = save_script(output.output_script);
        payload.write_variable_uint(raw_script.size());
        payload.write_data(raw_script);
    }
    payload.write_4_bytes(tx.locktime);
    return payload.data();
}

data_chunk satoshi_exporter::save(const message::get_data& getdata) const
{
    serializer payload;
    payload.write_variable_uint(getdata.inventories.size());
    for (const message::inventory_vector inv: getdata.inventories)
    {
        switch (inv.type)
        {
            case message::inventory_type::transaction:
                payload.write_4_bytes(1);
                break;
            case message::inventory_type::block:
                payload.write_4_bytes(2);
                break;
            case message::inventory_type::error:
            case message::inventory_type::none:
            default:
                BITCOIN_ASSERT(0);
                break;
        }
        payload.write_hash(inv.hash);
    }
    return payload.data();
}

message::header satoshi_exporter::load_header(const data_chunk& stream)  const
{
    deserializer deserial(stream);
    message::header header;
    header.magic = deserial.read_4_bytes();
    header.command = deserial.read_fixed_string(command_size);
    header.payload_length = deserial.read_4_bytes();
    header.checksum = 0;
    return header;
}

uint32_t satoshi_exporter::checksum_from_network(const data_chunk& chunk) const
{
    deserializer deserial(chunk);
    return deserial.read_4_bytes();
}

message::version satoshi_exporter::load_version(const data_chunk& stream) const
{
    deserializer deserial(stream);
    message::version payload;
    payload.version = deserial.read_4_bytes();
    payload.services = deserial.read_8_bytes();
    payload.timestamp = deserial.read_8_bytes();
    payload.address_me = deserial.read_network_address();
    // Ignored field
    payload.address_me.timestamp = 0;
    if (payload.version < 106) {
        BITCOIN_ASSERT(stream.size() == 4 + 8 + 8 + 26);
        return payload;
    }
    payload.address_you = deserial.read_network_address();
    // Ignored field
    payload.address_you.timestamp = 0;
    payload.nonce = deserial.read_8_bytes();
    payload.user_agent = deserial.read_string();
    if (payload.version < 209) {
        BITCOIN_ASSERT(stream.size() == 4 + 8 + 8 + 26 + 26 + 8 + 1);
        return payload;
    }
    payload.start_height = deserial.read_4_bytes();
    BITCOIN_ASSERT(stream.size() == 4 + 8 + 8 + 26 + 26 + 8 + 1 + 4);
    return payload;
}

message::verack satoshi_exporter::load_verack(const data_chunk& stream) const
{
    return message::verack();
}

message::address satoshi_exporter::load_address(const data_chunk& stream) const
{
    message::address payload;
    deserializer deserial(stream);
    uint64_t count = deserial.read_var_uint();
    for (size_t i = 0; i < count; ++i)
    {
        uint32_t timestamp = deserial.read_4_bytes();
        message::network_address addr = deserial.read_network_address();
        addr.timestamp = timestamp;
        payload.addresses.push_back(addr);
    }
    return payload;
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

message::inventory satoshi_exporter::load_inventory(
    const data_chunk& stream) const
{
    deserializer deserial(stream);
    message::inventory payload;
    uint64_t count = deserial.read_var_uint();
    for (size_t i = 0; i < count; ++i)
    {
        message::inventory_vector inv_vect;
        uint32_t raw_type = deserial.read_4_bytes();
        inv_vect.type = inventory_type_from_number(raw_type);
        inv_vect.hash = deserial.read_hash();
        payload.inventories.push_back(inv_vect);
    }
    return payload;
}

data_chunk read_raw_script(deserializer& deserial)
{
    data_chunk raw_script;
    uint64_t script_length = deserial.read_var_uint();
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
    message::transaction txn;
    txn.version = deserial.read_4_bytes();
    uint64_t txn_in_count = deserial.read_var_uint();
    for (size_t txn_in_i = 0; txn_in_i < txn_in_count; ++txn_in_i)
    {
        message::transaction_input input;
        input.previous_output.hash = deserial.read_hash();
        input.previous_output.index = deserial.read_4_bytes();
        if (previous_output_is_null(input.previous_output))
            input.input_script = coinbase_script(read_raw_script(deserial));
        else
            input.input_script = read_script(deserial);
        input.sequence = deserial.read_4_bytes();
        txn.inputs.push_back(input);
    }
    uint64_t txn_out_count = deserial.read_var_uint();
    for (size_t txn_out_i = 0; txn_out_i < txn_out_count; ++txn_out_i)
    {
        message::transaction_output output;
        output.value = deserial.read_8_bytes();
        output.output_script = read_script(deserial);
        txn.outputs.push_back(output);
    }
    txn.locktime = deserial.read_4_bytes();
    return txn;
}

message::transaction satoshi_exporter::load_transaction(
    const data_chunk& stream) const
{
    deserializer deserial(stream);
    return read_transaction(deserial);
}

message::block satoshi_exporter::load_block(const data_chunk& stream) const
{
    deserializer deserial(stream);
    message::block payload;
    payload.version = deserial.read_4_bytes();
    payload.previous_block_hash = deserial.read_hash();
    payload.merkle = deserial.read_hash();
    payload.timestamp = deserial.read_4_bytes();
    payload.bits = deserial.read_4_bytes();
    payload.nonce = deserial.read_4_bytes();
    uint64_t txn_count = deserial.read_var_uint();
    for (size_t txn_i = 0; txn_i < txn_count; ++txn_i)
    {
        const message::transaction txn = read_transaction(deserial);
        payload.transactions.push_back(txn);
    }
    return payload;
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

bool satoshi_exporter::checksum_used(const message::header& header_msg) const
{
    return header_msg.command != "version" && header_msg.command != "verack";
}

bool satoshi_exporter::verify_checksum(const message::header& header_msg,
        const data_chunk& stream) const
{
    if (!checksum_used(header_msg))
        return true;
    return header_msg.checksum == generate_sha256_checksum(stream);
}

} // libbitcoin

