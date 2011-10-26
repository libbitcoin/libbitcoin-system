#ifndef LIBBITCOIN_BASE58_H
#define LIBBITCOIN_BASE58_H

#include <bitcoin/types.hpp>

namespace libbitcoin {

std::string encode_base58(const data_chunk& unencoded_data);

data_chunk decode_base58(const std::string& encoded_data);

} // libbitcoin

#endif

