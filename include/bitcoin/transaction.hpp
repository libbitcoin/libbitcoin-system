#ifndef LIBBITCOIN_TRANSACTION_H
#define LIBBITCOIN_TRANSACTION_H

#include <bitcoin/messages.hpp>

namespace libbitcoin {

hash_digest hash_transaction(const message::transaction& tx);
// hash_type_code is used by OP_CHECKSIG
hash_digest hash_transaction(const message::transaction& tx, 
    uint32_t hash_type_code);

hash_digest generate_merkle_root(const message::transaction_list& transactions);

std::string pretty(const message::transaction& transaction);

bool previous_output_is_null(const message::output_point& previous_output);
bool is_coinbase(const message::transaction& tx);

uint64_t total_output_value(const message::transaction& tx);

bool operator==(const message::output_point& output_a,
    const message::output_point& output_b);

} // namespace libbitcoin

#endif

