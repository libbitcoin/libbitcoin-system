#ifndef LIBBITCOIN_IMPL_SERIALIZE_BLOCK_IPP
#define LIBBITCOIN_IMPL_SERIALIZE_BLOCK_IPP

namespace libbitcoin {

template <typename Deserializer>
data_chunk read_raw_script(Deserializer& deserial)
{
    data_chunk raw_script;
    uint64_t script_length = deserial.read_variable_uint();
    return deserial.read_data(script_length);
}

template <typename Deserializer>
script_type read_script(Deserializer& deserial)
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

template <typename Deserializer>
transaction_type read_transaction(
    Deserializer& deserial, transaction_type& packet)
{
    packet.version = deserial.read_4_bytes();
    uint64_t tx_in_count = deserial.read_variable_uint();
    for (size_t tx_in_i = 0; tx_in_i < tx_in_count; ++tx_in_i)
    {
        transaction_input_type input;
        input.previous_output.hash = deserial.read_hash();
        input.previous_output.index = deserial.read_4_bytes();
        if (previous_output_is_null(input.previous_output))
            input.script = coinbase_script(read_raw_script(deserial));
        else
            input.script = read_script(deserial);
        input.sequence = deserial.read_4_bytes();
        packet.inputs.push_back(input);
    }
    uint64_t tx_out_count = deserial.read_variable_uint();
    for (size_t tx_out_i = 0; tx_out_i < tx_out_count; ++tx_out_i)
    {
        transaction_output_type output;
        output.value = deserial.read_8_bytes();
        output.script = read_script(deserial);
        packet.outputs.push_back(output);
    }
    packet.locktime = deserial.read_4_bytes();
    return packet;
}

const std::string satoshi_command(const transaction_type&);
size_t satoshi_raw_size(const transaction_type& packet);
template <typename Iterator>
Iterator satoshi_save(const transaction_type& packet, Iterator result)
{
    auto serial = make_serializer(result);
    serial.write_4_bytes(packet.version);
    serial.write_variable_uint(packet.inputs.size());
    for (const transaction_input_type& input: packet.inputs)
    {
        serial.write_hash(input.previous_output.hash);
        serial.write_4_bytes(input.previous_output.index);
        data_chunk raw_script = save_script(input.script);
        serial.write_variable_uint(raw_script.size());
        serial.write_data(raw_script);
        serial.write_4_bytes(input.sequence);
    }
    serial.write_variable_uint(packet.outputs.size());
    for (const transaction_output_type& output: packet.outputs)
    {
        serial.write_8_bytes(output.value);
        data_chunk raw_script = save_script(output.script);
        serial.write_variable_uint(raw_script.size());
        serial.write_data(raw_script);
    }
    serial.write_4_bytes(packet.locktime);
    return serial.iterator();
}
template <typename Iterator>
void satoshi_load(const Iterator first, const Iterator last,
    transaction_type& packet)
{
    auto deserial = make_deserializer(first, last);
    read_transaction(deserial, packet);
    // Only true for valid transaction streams.
    //BITCOIN_ASSERT(satoshi_raw_size(packet) == std::distance(first, last));
}

size_t satoshi_raw_size(const block_header_type& packet);
template <typename Iterator>
Iterator satoshi_save(const block_header_type& packet, Iterator result)
{
    auto serial = make_serializer(result);
    serial.write_4_bytes(packet.version);
    serial.write_hash(packet.previous_block_hash);
    serial.write_hash(packet.merkle);
    serial.write_4_bytes(packet.timestamp);
    serial.write_4_bytes(packet.bits);
    serial.write_4_bytes(packet.nonce);
    BITCOIN_ASSERT(std::distance(result, serial.iterator()) == 80);
    return serial.iterator();
}
template <typename Iterator>
void satoshi_load(const Iterator first, const Iterator last,
    block_header_type& packet)
{
    auto deserial = make_deserializer(first, last);
    packet.version = deserial.read_4_bytes();
    packet.previous_block_hash = deserial.read_hash();
    packet.merkle = deserial.read_hash();
    packet.timestamp = deserial.read_4_bytes();
    packet.bits = deserial.read_4_bytes();
    packet.nonce = deserial.read_4_bytes();
    BITCOIN_ASSERT(std::distance(first, deserial.iterator()) == 80);
}

const std::string satoshi_command(const block_type&);
size_t satoshi_raw_size(const block_type& packet);
template <typename Iterator>
Iterator satoshi_save(const block_type& packet, Iterator result)
{
    satoshi_save(packet.header, result);
    auto serial = make_serializer(result + 80);
    serial.write_variable_uint(packet.transactions.size());
    Iterator write_iter = serial.iterator();
    for (const transaction_type& tx: packet.transactions)
        write_iter = satoshi_save(tx, write_iter);
    return write_iter;
}
template <typename Iterator>
void satoshi_load(const Iterator first, const Iterator last,
    block_type& packet)
{
    satoshi_load(first, first + 80, packet.header);
    auto deserial = make_deserializer(first + 80, last);
    uint64_t tx_count = deserial.read_variable_uint();
    for (size_t tx_i = 0; tx_i < tx_count; ++tx_i)
    {
        transaction_type tx;
        read_transaction(deserial, tx);
        packet.transactions.push_back(std::move(tx));
    }
}

} // libbitcoin

#endif

