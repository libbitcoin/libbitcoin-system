#ifndef LIBBITCOIN_BLOCK_H
#define LIBBITCOIN_BLOCK_H

#include <bitcoin/messages.hpp>

namespace libbitcoin {

enum class block_status
{
    orphan,
    confirmed,
    bad
};

uint64_t block_value(size_t depth);

hash_digest hash_block_header(const message::block& block);

message::block genesis_block();

} // libbitcoin

#endif

