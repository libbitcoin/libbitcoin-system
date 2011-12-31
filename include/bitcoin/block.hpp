#ifndef LIBBITCOIN_BLOCK_H
#define LIBBITCOIN_BLOCK_H

#include <bitcoin/messages.hpp>
#include <bitcoin/utility/big_number.hpp>

namespace libbitcoin {

enum class block_status
{
    orphan,
    confirmed,
    rejected
};

struct block_info
{
    block_status status;
    size_t depth;
};

bool operator==(const message::block block_a, const message::block& block_b);

uint64_t block_value(size_t depth);
big_number block_work(uint32_t bits);

hash_digest hash_block_header(const message::block& block);

std::vector<size_t> block_locator_indices(int top_depth);

message::block genesis_block();

} // libbitcoin

#endif

