#ifndef LIBBITCOIN_TRANSACTION_H
#define LIBBITCOIN_TRANSACTION_H

#include <bitcoin/net/messages.hpp>

namespace libbitcoin {

hash_digest hash_transaction(net::message::transaction transaction);

hash_digest generate_merkle_root(net::message::transaction_list transactions);

std::string transaction_repr(net::message::transaction transaction);

} // libbitcoin

#endif

