#include "kyoto_common.hpp"

#include <boost/filesystem.hpp>

#include <bitcoin/utility/logger.hpp>
#include <bitcoin/exporter.hpp>
#include <bitcoin/format.hpp>
#include <bitcoin/transaction.hpp>

namespace libbitcoin {

namespace fs = boost::filesystem;
namespace ky = kyotocabinet;

constexpr size_t block_header_size = 80;

class blocks_depth_comparator
  : public ky::Comparator
{
public:
    explicit blocks_depth_comparator() {}
    int32_t compare(const char* buffer1, size_t size1,
        const char* buffer2, size_t size2)
    {
        data_chunk key_data1(size1), key_data2(size2);
        memcpy(key_data1.data(), buffer1, size1);
        memcpy(key_data2.data(), buffer2, size2);
        uint32_t depth1 = cast_chunk<uint32_t>(key_data1),
            depth2 = cast_chunk<uint32_t>(key_data2);
        if (depth1 < depth2)
            return -1;
        else if (depth1 > depth2)
            return 1;
        return 0;
    }
};

bool kyoto_common::start_databases(const std::string& prefix,
    const kyoto_blockchain_options& options)
{
    fs::path prefix_path = prefix;
    blocks_.tune_comparator(new blocks_depth_comparator);
    if (!blocks_.open((prefix_path / "blocks.kct").native()))
    {
        log_error(log_domain::blockchain) << blocks_.error().name();
        return false;
    }
    if (!blocks_hash_.open((prefix_path / "blocks_hash.kch").native()))
    {
        log_error(log_domain::blockchain) << blocks_hash_.error().name();
        return false;
    }
    if (!txs_.open((prefix_path / "txs.kch").native()))
    {
        log_error(log_domain::blockchain) << txs_.error().name();
        return false;
    }
    if (!spends_.open((prefix_path / "spends.kch").native()))
    {
        log_error(log_domain::blockchain) << spends_.error().name();
        return false;
    }
    if (!address_.open((prefix_path / "address.kch").native()))
    {
        log_error(log_domain::blockchain) << address_.error().name();
        return false;
    }
    return true;
}

template <typename DataBuffer>
const char* char_ptr(const DataBuffer& buffer)
{
    return reinterpret_cast<const char*>(&buffer[0]);
}
template <typename DataBuffer>
char* char_ptr(DataBuffer& buffer)
{
    return reinterpret_cast<char*>(&buffer[0]);
}

bool kyoto_common::save_block(uint32_t depth, const message::block& blk)
{
    satoshi_exporter sexport;
    data_chunk value = sexport.save(blk);
    // block header = 80 bytes
    value.resize(block_header_size + sha256_length * blk.transactions.size());
    auto hashes_begin = value.begin() + block_header_size;
    for (uint32_t tx_index = 0;
        tx_index < blk.transactions.size(); ++tx_index)
    {
        const message::transaction& tx = blk.transactions[tx_index];
        const hash_digest& tx_hash = hash_transaction(tx);
        if (!save_transaction(tx, tx_hash, depth, tx_index))
        {
            log_fatal(log_domain::blockchain)
                << "Could not save transaction";
            return false;
        }
        BITCOIN_ASSERT(tx_hash.size() == sha256_length);
        std::copy(tx_hash.begin(), tx_hash.end(), hashes_begin);
        hashes_begin += sha256_length;
    }
    BITCOIN_ASSERT(hashes_begin == value.end());
    data_chunk depth_key = uncast_type(depth);
    BITCOIN_ASSERT(depth_key.size() == 4);
    if (!blocks_.set(
            char_ptr(depth_key), depth_key.size(),
            char_ptr(value), value.size()))
    {
        log_error(log_domain::blockchain) << blocks_.error().name();
        return false;
    }
    const hash_digest& blk_hash = hash_block_header(blk);
    if (!blocks_hash_.set(
            char_ptr(blk_hash), blk_hash.size(),
            char_ptr(depth_key), depth_key.size()))
    {
        log_error(log_domain::blockchain) << blocks_hash_.error().name();
        return false;
    }
    return true;
}

bool kyoto_common::save_transaction(const message::transaction& block_tx,
    const hash_digest& tx_hash, uint32_t block_depth, uint32_t tx_index)
{
    return true;
}

bool kyoto_common::fetch_block_header(uint32_t depth, message::block& blk)
{
    data_chunk depth_key = uncast_type(depth), raw_block(block_header_size);
    BITCOIN_ASSERT(depth_key.size() == 4);
    int32_t size = blocks_.get(
        char_ptr(depth_key), depth_key.size(),
        char_ptr(raw_block), raw_block.size());
    if (size == -1)
        return false;
    // blocks should always be 80 bytes and at least 1 tx
    // size should always be 80 + some multiple of 32
    BITCOIN_ASSERT(size >= block_header_size + sha256_length);
    BITCOIN_ASSERT((size - block_header_size) % sha256_length == 0);
    satoshi_exporter sexport;
    // satoshi_exporter expects varint number of transactions
    raw_block.push_back(0);
    blk = sexport.load_block(raw_block);
    return true;
}

} // libbitcoin

