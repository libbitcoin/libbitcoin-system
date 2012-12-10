#include <bitcoin/satoshi_serialize.hpp>

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

size_t satoshi_raw_size(const header& head)
{
    return 20 + (head.checksum == 0 ? 0 : 4);
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

const std::string satoshi_command(const verack&)
{
    return "verack";
}
size_t satoshi_raw_size(const verack& packet)
{
    return 0;
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

const std::string satoshi_command(const get_address&)
{
    return "getaddr";
}
size_t satoshi_raw_size(const get_address& packet)
{
    return 0;
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

const std::string satoshi_command(const inventory&)
{
    return "inv";
}
size_t satoshi_raw_size(const inventory& packet)
{
    return raw_size_inventory_impl(packet);
}

const std::string satoshi_command(const get_data&)
{
    return "getdata";
}
size_t satoshi_raw_size(const get_data& packet)
{
    return raw_size_inventory_impl(packet);
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
#ifndef BITCOIN_DISABLE_ASSERTS
    std::string assert_msg = pretty_hex(raw_script);
#endif
    BITCOIN_ASSERT_MSG(
        raw_script == save_script(parse_script(raw_script)),
        assert_msg.c_str());
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

const std::string satoshi_command(const ping&)
{
    return "ping";
}
size_t satoshi_raw_size(const ping& packet)
{
    return 8;
}

const std::string satoshi_command(const pong&)
{
    return "pong";
}
size_t satoshi_raw_size(const pong& packet)
{
    return 8;
}

} // message
} // libbitcoin

