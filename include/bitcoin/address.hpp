#ifndef LIBBITCOIN_ADDRESS_H
#define LIBBITCOIN_ADDRESS_H

/*
 * This is a convenience file which is going to be subject
 * to heavy revision as the API is more finely revised.
 * It is strictly temporary.
 */

#include <bitcoin/types.hpp>

namespace libbitcoin {

std::string public_key_to_address(const data_chunk& public_key);
std::string public_key_hash_to_address(const short_hash& pubkey_hash);

short_hash address_to_short_hash(const std::string& address);

} // namespace libbitcoin

#endif


