#ifndef LIBBITCOIN_LEVELDB_PROTOBUF_WRAPPER_H
#define LIBBITCOIN_LEVELDB_PROTOBUF_WRAPPER_H

#include <bitcoin/messages.hpp>

#include "bitcoin.pb.h"

namespace libbitcoin {

protobuf::Block block_header_to_protobuf(uint32_t depth,
    const block_type serial_block);

protobuf::Transaction transaction_to_protobuf(
    const transaction_type& block_tx);

block_type protobuf_to_block_header(const protobuf::Block& proto_block);

transaction_type protobuf_to_transaction(
    const protobuf::Transaction& proto_tx);

} // namespace libbitcoin

#endif

