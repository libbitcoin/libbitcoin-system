#include <bitcoin/block.hpp>

#include <bitcoin/satoshi_serialize.hpp>
#include <bitcoin/types.hpp>
#include <bitcoin/constants.hpp>
#include <bitcoin/transaction.hpp>
#include <bitcoin/utility/serializer.hpp>
#include <bitcoin/utility/sha256.hpp>
#include <bitcoin/utility/assert.hpp>

namespace libbitcoin {

bool operator==(
    const block_header_type block_a, const block_header_type& block_b)
{
    return block_a.version == block_b.version &&
        block_a.previous_block_hash == block_b.previous_block_hash &&
        block_a.merkle == block_b.merkle &&
        block_a.timestamp == block_b.timestamp &&
        block_a.bits == block_b.bits &&
        block_a.nonce == block_b.nonce;
}

uint64_t block_value(size_t height)
{
    uint64_t subsidy = coin_price(block_reward);
    subsidy >>= (height / reward_interval);
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

hash_digest hash_block_header(const block_header_type& header)
{
    data_chunk raw_block_header(80);
    satoshi_save(header, raw_block_header.begin());
    return generate_sha256_hash(raw_block_header);
}

index_list block_locator_indexes(int top_height)
{
    // Start at max_height
    index_list indexes;
    // Push last 10 indexes first
    size_t step = 1, start = 0;
    for (int i = top_height; i > 0; i -= step, ++start)
    {
        if (start >= 10)
            step *= 2;
        indexes.push_back(i);
    }
    indexes.push_back(0);
    return indexes;
}

block_type genesis_block()
{
    block_header_type header;
    header.version = 1;
    header.previous_block_hash = null_hash;
    header.merkle =
        hash_digest{0x4a, 0x5e, 0x1e, 0x4b, 0xaa, 0xb8, 0x9f, 0x3a,
                    0x32, 0x51, 0x8a, 0x88, 0xc3, 0x1b, 0xc8, 0x7f,
                    0x61, 0x8f, 0x76, 0x67, 0x3e, 0x2c, 0xc7, 0x7a,
                    0xb2, 0x12, 0x7b, 0x7a, 0xfd, 0xed, 0xa3, 0x3b};
    header.timestamp = 1231006505;
    header.bits = 0x1d00ffff;
    header.nonce = 2083236893;

    transaction_type coinbase_tx;
    coinbase_tx.version = 1;
    coinbase_tx.locktime = 0;

    transaction_input_type coinbase_input;
    coinbase_input.previous_output.hash = null_hash;
    coinbase_input.previous_output.index =
        std::numeric_limits<uint32_t>::max();
    // The Times 03/Jan/2009 Chancellor on brink of second bailout for banks
    coinbase_input.script = coinbase_script(
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
    coinbase_input.sequence = std::numeric_limits<uint32_t>::max();
    coinbase_tx.inputs.push_back(coinbase_input);

    transaction_output_type coinbase_output;
    coinbase_output.value = coin_price(50);
    coinbase_output.script.push_operation(
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
    coinbase_output.script.push_operation(
        operation{opcode::checksig, data_chunk()});
    coinbase_tx.outputs.push_back(coinbase_output);

    block_type genesis;
    genesis.header = header;
    genesis.transactions.push_back(coinbase_tx);
    BITCOIN_ASSERT(genesis.transactions.size() == 1);
    BITCOIN_ASSERT(
        generate_merkle_root(genesis.transactions) == genesis.header.merkle);
    return genesis;
}

} // namespace libbitcoin

