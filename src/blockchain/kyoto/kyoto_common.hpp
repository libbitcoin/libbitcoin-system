#ifndef LIBBITCOIN_BLOCKCHAIN_KYOTO_KYOTO_COMMON_H
#define LIBBITCOIN_BLOCKCHAIN_KYOTO_KYOTO_COMMON_H

#include <bitcoin/blockchain/kyoto_blockchain.hpp>

namespace libbitcoin {

class kyoto_common
{
public:
    bool start_databases(const std::string& prefix,
        const kyoto_blockchain_options& options);

    bool save_block(uint32_t depth, const message::block& blk);
    bool save_transaction(const message::transaction& block_tx,
        const hash_digest& tx_hash, uint32_t block_depth, uint32_t tx_index);

    bool fetch_block_header(uint32_t depth, message::block& blk);

    kyotocabinet::TreeDB blocks_;
    kyotocabinet::HashDB blocks_hash_, txs_, spends_, address_;
};

} // libbitcoin

#endif

