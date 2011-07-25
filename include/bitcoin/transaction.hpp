#ifndef LIBBITCOIN_TRANSACTION_H
#define LIBBITCOIN_TRANSACTION_H

#include <bitcoin/messages.hpp>

namespace libbitcoin {

hash_digest hash_transaction(message::transaction transaction);

hash_digest generate_merkle_root(message::transaction_list transactions);

std::string transaction_repr(message::transaction transaction);

} // libbitcoin

#endif

