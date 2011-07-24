#ifndef LIBBITCOIN_TRANSACTION_H
#define LIBBITCOIN_TRANSACTION_H

#include <bitcoin/net/messages.hpp>

namespace libbitcoin {

hash_digest hash_transaction(net::message::transaction transaction);

} // libbitcoin

#endif

