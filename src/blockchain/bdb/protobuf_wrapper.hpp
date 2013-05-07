#ifndef LIBBITCOIN_PROTOBUF_WRAPPER_H
#define LIBBITCOIN_PROTOBUF_WRAPPER_H

#include <bitcoin/primitives.hpp>

#include "bdb_bitcoin.pb.h"

namespace libbitcoin {

proto::Block block_header_to_proto(uint32_t depth,
    const block_type serial_block);

proto::Transaction transaction_to_proto(
    const transaction_type& block_tx);

block_type proto_to_block_header(const proto::Block& proto_block);

transaction_type proto_to_transaction(
    const proto::Transaction& proto_tx);

} // namespace libbitcoin

#endif

