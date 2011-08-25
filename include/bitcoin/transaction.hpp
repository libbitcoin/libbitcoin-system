#ifndef LIBBITCOIN_TRANSACTION_H
#define LIBBITCOIN_TRANSACTION_H

#include <bitcoin/messages.hpp>

namespace libbitcoin {

hash_digest hash_transaction(const message::transaction& transaction);
// hash_type_code is used by OP_CHECKSIG
hash_digest hash_transaction(const message::transaction& transaction, 
        uint32_t hash_type_code);

hash_digest generate_merkle_root(const message::transaction_list& transactions);

std::string string_repr(const message::transaction& transaction);

bool previous_output_is_null(const message::transaction_input& input);
bool is_coinbase(const message::transaction& tx);

} // libbitcoin

#endif

