#ifndef LIBBITCOIN_BLOCK_H
#define LIBBITCOIN_BLOCK_H

#include <bitcoin/net/messages.hpp>

namespace libbitcoin {

hash_digest hash_block_header(net::message::block block);

} // libbitcoin

#endif

