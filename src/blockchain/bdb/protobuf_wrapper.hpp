#ifndef LIBBITCOIN_PROTOBUF_WRAPPER_H
#define LIBBITCOIN_PROTOBUF_WRAPPER_H

#include <bitcoin/messages.hpp>

#include "bitcoin.pb.h"

namespace libbitcoin {

protobuf::Block block_header_to_protobuf(uint32_t depth,
    const message::block serial_block)
{
    protobuf::Block proto_block;
    proto_block.set_depth(depth);
    proto_block.set_version(serial_block.version);
    proto_block.set_previous_block_hash(
        serial_block.previous_block_hash.data(),
        serial_block.previous_block_hash.size());
    proto_block.set_merkle(
        serial_block.merkle.data(), serial_block.merkle.size());
    proto_block.set_timestamp(serial_block.timestamp);
    proto_block.set_bits(serial_block.bits);
    proto_block.set_nonce(serial_block.nonce);
    return proto_block;
}

protobuf::Transaction transaction_to_protobuf(
    const message::transaction& block_tx)
{
    protobuf::Transaction proto_tx;
    proto_tx.set_version(block_tx.version);
    proto_tx.set_locktime(block_tx.locktime);
    for (const message::transaction_input& block_input: block_tx.inputs)
    {
        protobuf::Transaction::Input& proto_input = *proto_tx.add_inputs();
        proto_input.set_previous_output_hash(
            block_input.previous_output.hash.data(),
            block_input.previous_output.hash.size());
        proto_input.set_previous_output_index(
            block_input.previous_output.index);
        data_chunk raw_script = save_script(block_input.input_script);
        proto_input.set_script(&raw_script[0], raw_script.size());
        proto_input.set_sequence(block_input.sequence);
    }
    for (const message::transaction_output& block_output: block_tx.outputs)
    {
        protobuf::Transaction::Output& proto_output = *proto_tx.add_outputs();
        proto_output.set_value(block_output.value);
        data_chunk raw_script = save_script(block_output.output_script);
        proto_output.set_script(&raw_script[0], raw_script.size());
    }
    return proto_tx;
}

message::block protobuf_to_block_header(const protobuf::Block& proto_block)
{
    message::block result_block;
    result_block.version = proto_block.version();
    const std::string& previous_block_hash = proto_block.previous_block_hash();
    std::copy(previous_block_hash.begin(), previous_block_hash.end(),
        result_block.previous_block_hash.begin());
    const std::string& merkle = proto_block.merkle();
    std::copy(merkle.begin(), merkle.end(), result_block.merkle.begin());
    result_block.timestamp = proto_block.timestamp();
    result_block.bits = proto_block.bits();
    result_block.nonce = proto_block.nonce();
    return result_block;
}

// Ugly boilerplate for ugly transformation
template <typename InOut>
data_chunk read_raw_script(const InOut& inout)
{
    const std::string& script_str = inout.script();
    return data_chunk(script_str.begin(), script_str.end());
}

message::transaction protobuf_to_transaction(
    const protobuf::Transaction& proto_tx)
{
    message::transaction result_tx;
    result_tx.version = proto_tx.version();
    result_tx.locktime = proto_tx.locktime();
    for (size_t i = 0; i < proto_tx.inputs_size(); ++i)
    {
        const protobuf::Transaction::Input& proto_input = proto_tx.inputs(i);
        message::transaction_input tx_input;
        const std::string& prev_out_hash = proto_input.previous_output_hash();
        std::copy(prev_out_hash.begin(), prev_out_hash.end(),
            tx_input.previous_output.hash.begin());
        tx_input.previous_output.index = proto_input.previous_output_index();
        const data_chunk& raw_script = read_raw_script(proto_input);
        if (previous_output_is_null(tx_input.previous_output))
            tx_input.input_script = coinbase_script(raw_script);
        else
            tx_input.input_script = parse_script(raw_script);
        tx_input.sequence = proto_input.sequence();
        result_tx.inputs.push_back(tx_input);
    }
    for (size_t i = 0; i < proto_tx.outputs_size(); ++i)
    {
        const protobuf::Transaction::Output& proto_output = proto_tx.outputs(i);
        message::transaction_output tx_output;
        tx_output.value = proto_output.value();
        tx_output.output_script = parse_script(read_raw_script(proto_output));
        result_tx.outputs.push_back(tx_output);
    }
    return result_tx;
}

} // libbitcoin

#endif

