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
void satoshi_load(Iterator first, Iterator last, header_type& head)
{
    auto deserial = make_deserializer(first, last);
    head.magic = deserial.read_4_bytes();
    head.command = deserial.read_fixed_string(command_size);
    head.payload_length = deserial.read_4_bytes();
    head.checksum = 0;
    BITCOIN_ASSERT(satoshi_raw_size(head) == std::distance(first, last));
}

const std::string satoshi_command(const version_type&);
size_t satoshi_raw_size(const version_type& packet);
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
    serial.write_4_bytes(packet.start_depth);
    return serial.iterator();
}
template <typename Iterator>
void satoshi_load(Iterator first, Iterator last, version_type& packet)
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
    packet.start_depth = deserial.read_4_bytes();
    BITCOIN_ASSERT(std::distance(first, last) >= 81 + 4);
    //BITCOIN_ASSERT(satoshi_raw_size(packet) <= std::distance(first, last));
}

const std::string satoshi_command(const verack_type&);
size_t satoshi_raw_size(const verack_type& packet);
template <typename Iterator>
Iterator satoshi_save(const verack_type& packet, Iterator result)
{
    BITCOIN_ASSERT(satoshi_raw_size(packet) == 0);
    return result;
}
template <typename Iterator>
void satoshi_load(Iterator first, Iterator last, verack_type& packet)
{
    BITCOIN_ASSERT(satoshi_raw_size(packet) == 0);
}

const std::string satoshi_command(const address_type&);
size_t satoshi_raw_size(const address_type& packet);
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
void satoshi_load(Iterator first, Iterator last, address_type& packet)
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
    BITCOIN_ASSERT(satoshi_raw_size(packet) == std::distance(first, last));
}

const std::string satoshi_command(const get_address_type&);
size_t satoshi_raw_size(const get_address_type& packet);
template <typename Iterator>
Iterator satoshi_save(const get_address_type& packet, Iterator result)
{
    BITCOIN_ASSERT(satoshi_raw_size(packet) == 0);
    return result;
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
void load_inventory_impl(Iterator first, Iterator last, Message& packet)
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
    BITCOIN_ASSERT(satoshi_raw_size(packet) == std::distance(first, last));
}

const std::string satoshi_command(const inventory_type&);
size_t satoshi_raw_size(const inventory_type& packet);
template <typename Iterator>
Iterator satoshi_save(const inventory_type& packet, Iterator result)
{
    return save_inventory_impl(packet, result);
}
template <typename Iterator>
void satoshi_load(Iterator first, Iterator last, inventory_type& packet)
{
    load_inventory_impl(first, last, packet);
}

const std::string satoshi_command(const get_data_type&);
size_t satoshi_raw_size(const get_data_type& packet);
template <typename Iterator>
Iterator satoshi_save(const get_data_type& packet, Iterator result)
{
    return save_inventory_impl(packet, result);
}
template <typename Iterator>
void satoshi_load(Iterator first, Iterator last, get_data_type& packet)
{
    load_inventory_impl(first, last, packet);
}

const std::string satoshi_command(const get_blocks_type&);
size_t satoshi_raw_size(const get_blocks_type& packet);
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
void satoshi_load(Iterator first, Iterator last, get_blocks_type& packet)
{
    auto deserial = make_deserializer(first, last);
    // Discard protocol version because it is stupid
    deserial.read_4_bytes();
    uint32_t count = deserial.read_variable_uint();
    for (size_t i = 0; i < count; ++i)
    {
        hash_digest start_hash = deserial.read_hash();
        packet.start_hashes.push_back(start_hash);
    }
    packet.hash_stop = deserial.read_hash();
    BITCOIN_ASSERT(satoshi_raw_size(packet) == std::distance(first, last));
}

template <typename Deserializer>
data_chunk read_raw_script(Deserializer& deserial)
{
    data_chunk raw_script;
    uint64_t script_length = deserial.read_variable_uint();
    return deserial.read_data(script_length);
}

template <typename Deserializer>
script read_script(Deserializer& deserial)
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
    return serial.iterator();
}
template <typename Iterator>
void satoshi_load(Iterator first, Iterator last, transaction_type& packet)
{
    auto deserial = make_deserializer(first, last);
    read_transaction(deserial, packet);
    BITCOIN_ASSERT(satoshi_raw_size(packet) == std::distance(first, last));
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
void satoshi_load(Iterator first, Iterator last, block_header_type& packet)
{
    auto deserial = make_deserializer(first, last);
    packet.version = deserial.read_4_bytes();
    packet.previous_block_hash = deserial.read_hash();
    packet.merkle = deserial.read_hash();
    packet.timestamp = deserial.read_4_bytes();
    packet.bits = deserial.read_4_bytes();
    packet.nonce = deserial.read_4_bytes();
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
    return serial.iterator();
}
template <typename Iterator>
void satoshi_load(Iterator first, Iterator last, block_type& packet)
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
    BITCOIN_ASSERT(satoshi_raw_size(packet) == std::distance(first, last));
}

const std::string satoshi_command(const ping_type&);
size_t satoshi_raw_size(const ping_type& packet);
template <typename Iterator>
Iterator satoshi_save(const ping_type& packet, Iterator result)
{
    auto serial = make_serializer(result);
    serial.write_8_bytes(packet.nonce);
    return serial.iterator();
}
template <typename Iterator>
void satoshi_load(Iterator first, Iterator last, ping_type& packet)
{
    auto deserial = make_deserializer(first, last);
    packet.nonce = deserial.read_8_bytes();
    BITCOIN_ASSERT(satoshi_raw_size(packet) == std::distance(first, last));
}

const std::string satoshi_command(const pong_type&);
size_t satoshi_raw_size(const pong_type& packet);
template <typename Iterator>
Iterator satoshi_save(const pong_type& packet, Iterator result)
{
    auto serial = make_serializer(result);
    serial.write_8_bytes(packet.nonce);
    return serial.iterator();
}
template <typename Iterator>
void satoshi_load(Iterator first, Iterator last, pong_type& packet)
{
    auto deserial = make_deserializer(first, last);
    packet.nonce = deserial.read_8_bytes();
    BITCOIN_ASSERT(satoshi_raw_size(packet) == std::distance(first, last));
}

} // libbitcoin

#endif

