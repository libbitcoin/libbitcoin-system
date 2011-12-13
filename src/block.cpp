#include <bitcoin/block.hpp>

#include <bitcoin/util/serializer.hpp>
#include <bitcoin/util/sha256.hpp>
#include <bitcoin/util/assert.hpp>
#include <bitcoin/types.hpp>
#include <bitcoin/constants.hpp>
#include <bitcoin/transaction.hpp>

namespace libbitcoin {

uint64_t block_value(size_t depth)
{
    uint64_t subsidy = coin_price(block_reward);
    subsidy >>= (depth / reward_interval);
    return subsidy;
}

big_number block_work(uint32_t bits)
{
    big_number target;
    target.set_compact(bits);
    if (target <= 0)
        return 0;
    return (big_number(1) << 256) / (target + 1);
}

hash_digest hash_block_header(const message::block& block)
{
    serializer key;
    key.write_4_bytes(block.version);
    key.write_hash(block.previous_block_hash);
    key.write_hash(block.merkle);
    key.write_4_bytes(block.timestamp);
    key.write_4_bytes(block.bits);
    key.write_4_bytes(block.nonce);
    return generate_sha256_hash(key.get_data());
}

std::vector<size_t> block_locator_indices(int top_depth)
{
    // Start at max_depth
    std::vector<size_t> indices;
    // Push last 10 indices first
    size_t step = 1, start = 0;
    for (int i = top_depth; i > 0; i -= step, ++start)
    {
        if (start >= 10)
            step *= 2;
        indices.push_back(i);
    }
    indices.push_back(0);
    return indices;
}

message::block genesis_block()
{
    message::block genesis;
    genesis.version = 1;
    genesis.previous_block_hash = null_hash;
    genesis.merkle = 
        hash_digest{0x4a, 0x5e, 0x1e, 0x4b, 0xaa, 0xb8, 0x9f, 0x3a,
                    0x32, 0x51, 0x8a, 0x88, 0xc3, 0x1b, 0xc8, 0x7f,
                    0x61, 0x8f, 0x76, 0x67, 0x3e, 0x2c, 0xc7, 0x7a, 
                    0xb2, 0x12, 0x7b, 0x7a, 0xfd, 0xed, 0xa3, 0x3b};
    genesis.timestamp = 1231006505;
    genesis.bits = 0x1d00ffff;
    genesis.nonce = 2083236893;

    message::transaction coinbase_tx;
    coinbase_tx.version = 1;
    coinbase_tx.locktime = 0;

    message::transaction_input coinbase_input;
    coinbase_input.previous_output.hash = null_hash;
    coinbase_input.previous_output.index = ~0u;
    // The Times 03/Jan/2009 Chancellor on brink of second bailout for banks
    coinbase_input.input_script = coinbase_script(
        data_chunk{0x04, 0xff, 0xff, 0x00, 0x1d, 0x01, 0x04, 0x45, 
                   0x54, 0x68, 0x65, 0x20, 0x54, 0x69, 0x6d, 0x65, 
                   0x73, 0x20, 0x30, 0x33, 0x2f, 0x4a, 0x61, 0x6e, 
                   0x2f, 0x32, 0x30, 0x30, 0x39, 0x20, 0x43, 0x68, 
                   0x61, 0x6e, 0x63, 0x65, 0x6c, 0x6c, 0x6f, 0x72, 
                   0x20, 0x6f, 0x6e, 0x20, 0x62, 0x72, 0x69, 0x6e, 
                   0x6b, 0x20, 0x6f, 0x66, 0x20, 0x73, 0x65, 0x63, 
                   0x6f, 0x6e, 0x64, 0x20, 0x62, 0x61, 0x69, 0x6c, 
                   0x6f, 0x75, 0x74, 0x20, 0x66, 0x6f, 0x72, 0x20, 
                   0x62, 0x61, 0x6e, 0x6b, 0x73});
    coinbase_input.sequence = ~0u;
    coinbase_tx.inputs.push_back(coinbase_input);

    message::transaction_output coinbase_output;
    coinbase_output.value = coin_price(50);
    coinbase_output.output_script.push_operation(
        operation{opcode::special, 
            data_chunk{0x04, 0x67, 0x8a, 0xfd, 0xb0, 0xfe, 0x55, 0x48, 
                       0x27, 0x19, 0x67, 0xf1, 0xa6, 0x71, 0x30, 0xb7, 
                       0x10, 0x5c, 0xd6, 0xa8, 0x28, 0xe0, 0x39, 0x09, 
                       0xa6, 0x79, 0x62, 0xe0, 0xea, 0x1f, 0x61, 0xde, 
                       0xb6, 0x49, 0xf6, 0xbc, 0x3f, 0x4c, 0xef, 0x38, 
                       0xc4, 0xf3, 0x55, 0x04, 0xe5, 0x1e, 0xc1, 0x12, 
                       0xde, 0x5c, 0x38, 0x4d, 0xf7, 0xba, 0x0b, 0x8d, 
                       0x57, 0x8a, 0x4c, 0x70, 0x2b, 0x6b, 0xf1, 0x1d, 
                       0x5f}});
    coinbase_output.output_script.push_operation(
        operation{opcode::checksig, data_chunk()});
    coinbase_tx.outputs.push_back(coinbase_output);

    genesis.transactions.push_back(coinbase_tx);
    BITCOIN_ASSERT(genesis.transactions.size() == 1);
    BITCOIN_ASSERT(
        generate_merkle_root(genesis.transactions) == genesis.merkle);
    return genesis;
}

} // libbitcoin

