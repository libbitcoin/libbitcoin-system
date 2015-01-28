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
#ifndef LIBBITCOIN_BLOCK_IPP
#define LIBBITCOIN_BLOCK_IPP

#ifndef NDEBUG
#include <bitcoin/bitcoin/formats/base16.hpp>
#endif
#include <bitcoin/bitcoin/utility/serializer.hpp>

namespace libbitcoin {

template <typename Deserializer>
data_chunk read_raw_script(Deserializer& deserial)
{
    auto script_length = deserial.read_variable_uint();
    BITCOIN_ASSERT(script_length <= max_uint32);
    auto script_length32 = static_cast<uint32_t>(script_length);
    return deserial.read_data(script_length32);
}

template <typename Deserializer>
script_type read_script(Deserializer& deserial)
{
    data_chunk raw_script = read_raw_script(deserial);
    // Output scripts can be empty so we attempt to parse them.
    // If unsuccessful then they can't be spent and we just return
    // a raw_data script.
    script_type result;
    try
    {
        result = parse_script(raw_script);
    }
    catch (end_of_stream)
    {
        return raw_data_script(raw_script);
    }
    DEBUG_ONLY(std::string assert_msg = encode_base16(raw_script));
    BITCOIN_ASSERT_MSG(raw_script == save_script(result), assert_msg.c_str());
    return result;
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
            input.script = raw_data_script(read_raw_script(deserial));
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
template <typename Iterator, bool SafeCheckLast>
void satoshi_load(const Iterator first, const Iterator last,
    transaction_type& packet)
{
    auto deserial = deserializer<Iterator, SafeCheckLast>(first, last);
    read_transaction(deserial, packet);
    // Only true for valid transaction streams.
    //BITCOIN_ASSERT(satoshi_raw_size(packet) == std::distance(first, last));
}

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
template <typename Iterator, bool SafeCheckLast>
void satoshi_load(const Iterator first, const Iterator last,
    block_header_type& packet)
{
    auto deserial = deserializer<Iterator, SafeCheckLast>(first, last);
    packet.version = deserial.read_4_bytes();
    packet.previous_block_hash = deserial.read_hash();
    packet.merkle = deserial.read_hash();
    packet.timestamp = deserial.read_4_bytes();
    packet.bits = deserial.read_4_bytes();
    packet.nonce = deserial.read_4_bytes();
    BITCOIN_ASSERT(std::distance(first, deserial.iterator()) == 80);
}

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

