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

size_t satoshi_raw_size(const header_type& head)
{
    return 20 + (head.checksum == 0 ? 0 : 4);
}

const std::string satoshi_command(const version_type&)
{
    return "version";
}
size_t satoshi_raw_size(const version_type& packet)
{
    return 84 +
        variable_uint_size(packet.user_agent.size()) +
        packet.user_agent.size();
}

const std::string satoshi_command(const verack_type&)
{
    return "verack";
}
size_t satoshi_raw_size(const verack_type& packet)
{
    return 0;
}

const std::string satoshi_command(const address_type&)
{
    return "addr";
}
size_t satoshi_raw_size(const address_type& packet)
{
    return variable_uint_size(packet.addresses.size()) +
        30 * packet.addresses.size();
}

const std::string satoshi_command(const get_address_type&)
{
    return "getaddr";
}
size_t satoshi_raw_size(const get_address_type& packet)
{
    return 0;
}

uint32_t inventory_type_to_number(inventory_type_id inv_type)
{
    switch (inv_type)
    {
        case inventory_type_id::error:
        case inventory_type_id::none:
        default:
            return 0;

        case inventory_type_id::transaction:
            return 1;

        case inventory_type_id::block:
            return 2;
    }
}

inventory_type_id inventory_type_from_number(uint32_t raw_type)
{
    switch (raw_type)
    {
        case 0:
            return inventory_type_id::error;
        case 1:
            return inventory_type_id::transaction;
        case 2:
            return inventory_type_id::block;
        default:
            return inventory_type_id::none;
    }
}

const std::string satoshi_command(const inventory_type&)
{
    return "inv";
}
size_t satoshi_raw_size(const inventory_type& packet)
{
    return raw_size_inventory_impl(packet);
}

const std::string satoshi_command(const get_data_type&)
{
    return "getdata";
}
size_t satoshi_raw_size(const get_data_type& packet)
{
    return raw_size_inventory_impl(packet);
}

const std::string satoshi_command(const get_blocks_type&)
{
    return "getblocks";
}
size_t satoshi_raw_size(const get_blocks_type& packet)
{
    return 36 +
        variable_uint_size(packet.start_hashes.size()) +
        32 * packet.start_hashes.size();
}

void save_transaction(
    serializer& serial, const transaction_type& packet)
{
    serial.write_4_bytes(packet.version);
    serial.write_variable_uint(packet.inputs.size());
    for (const transaction_input_type& input: packet.inputs)
    {
        serial.write_hash(input.previous_output.hash);
        serial.write_4_bytes(input.previous_output.index);
        data_chunk raw_script = save_script(input.input_script);
        serial.write_variable_uint(raw_script.size());
        serial.write_data(raw_script);
        serial.write_4_bytes(input.sequence);
    }
    serial.write_variable_uint(packet.outputs.size());
    for (const transaction_output_type& output: packet.outputs)
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
    std::string assert_msg = encode_hex(raw_script);
#endif
    BITCOIN_ASSERT_MSG(
        raw_script == save_script(parse_script(raw_script)),
        assert_msg.c_str());
    // Eventually plan to move parse_script to inside here
    return parse_script(raw_script);
}

transaction_type read_transaction(
    deserializer& deserial, transaction_type& packet)
{
    packet.version = deserial.read_4_bytes();
    uint64_t tx_in_count = deserial.read_variable_uint();
    for (size_t tx_in_i = 0; tx_in_i < tx_in_count; ++tx_in_i)
    {
        transaction_input_type input;
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
        transaction_output_type output;
        output.value = deserial.read_8_bytes();
        output.output_script = read_script(deserial);
        packet.outputs.push_back(output);
    }
    packet.locktime = deserial.read_4_bytes();
    return packet;
}

const std::string satoshi_command(const transaction_type&)
{
    return "tx";
}
size_t satoshi_raw_size(const transaction_type& packet)
{
    size_t tx_size = 8;
    tx_size += variable_uint_size(packet.inputs.size());
    for (const transaction_input_type& input: packet.inputs)
    {
        data_chunk raw_script = save_script(input.input_script);
        tx_size += 40 +
            variable_uint_size(raw_script.size()) +
            raw_script.size();
    }
    tx_size += variable_uint_size(packet.outputs.size());
    for (const transaction_output_type& output: packet.outputs)
    {
        data_chunk raw_script = save_script(output.output_script);
        tx_size += 8 +
            variable_uint_size(raw_script.size()) +
            raw_script.size();
    }
    return tx_size;
}

const std::string satoshi_command(const block_type&)
{
    return "block";
}
size_t satoshi_raw_size(const block_type& packet)
{
    size_t block_size = 80 + variable_uint_size(packet.transactions.size());
    for (const transaction_type& tx: packet.transactions)
        block_size += satoshi_raw_size(tx);
    return block_size;
}

const std::string satoshi_command(const ping_type&)
{
    return "ping";
}
size_t satoshi_raw_size(const ping_type& packet)
{
    return 8;
}

const std::string satoshi_command(const pong_type&)
{
    return "pong";
}
size_t satoshi_raw_size(const pong_type& packet)
{
    return 8;
}

} // libbitcoin

