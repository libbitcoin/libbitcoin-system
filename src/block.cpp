#include <bitcoin/block.hpp>

#include <bitcoin/util/serializer.hpp>
#include <bitcoin/util/sha256.hpp>
#include <bitcoin/types.hpp>
#include <bitcoin/constants.hpp>

namespace libbitcoin {

uint64_t block_value(size_t depth)
{
    uint64_t subsidy = coin_price(block_reward);
    subsidy >>= (depth / reward_interval);
    return subsidy;
}

hash_digest hash_block_header(const message::block& block)
{
    serializer key;
    key.write_4_bytes(block.version);
    key.write_hash(block.prev_block);
    key.write_hash(block.merkle_root);
    key.write_4_bytes(block.timestamp);
    key.write_4_bytes(block.bits);
    key.write_4_bytes(block.nonce);
    return generate_sha256_hash(key.get_data());
}

} // libbitcoin

