#ifndef LIBBITCOIN_BLOCK_H
#define LIBBITCOIN_BLOCK_H

#include <bitcoin/messages.hpp>

namespace libbitcoin {

hash_digest hash_block_header(message::block block);

} // libbitcoin

#endif

