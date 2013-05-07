#ifndef LIBBITCOIN_PROTOBUF_WRAPPER_H
#define LIBBITCOIN_PROTOBUF_WRAPPER_H

#include <bitcoin/messages.hpp>

#include "bdb_bitcoin.pb.h"

namespace libbitcoin {

proto::Block block_header_to_proto(uint32_t depth,
    const message::block serial_block);

proto::Transaction transaction_to_proto(
    const message::transaction& block_tx);

message::block proto_to_block_header(const proto::Block& proto_block);

message::transaction proto_to_transaction(
    const proto::Transaction& proto_tx);

} // namespace libbitcoin

#endif

