#include <bitcoin/storage/bdb_storage.hpp>
#include <bitcoin/transaction.hpp>
#include <bitcoin/util/logger.hpp>
using namespace libbitcoin;

void show_block(const std::error_code& ec, const message::block& blk)
{
    log_debug() << "Fetch";
    log_debug() << pretty_hex(blk.merkle);
    log_debug() << blk.transactions.size();
    log_debug() << pretty_hex(hash_transaction(blk.transactions[0]));
}

void show_locator(const std::error_code& ec, const message::block_locator& loc)
{
    log_debug() << "LOC";
    for (hash_digest h: loc)
        log_debug() << pretty_hex(h);
}

int main()
{
    bdb_storage::setup("database/");
    log_debug() << "Setup finished";
    shared_ptr<bdb_storage> store(new bdb_storage("database/"));
    log_debug() << "Opened";
    store->fetch_block(0, show_block);
    store->fetch_block(
        hash_digest{0x00, 0x00, 0x00, 0x00, 0x00, 0x19, 0xd6, 0x68, 
                    0x9c, 0x08, 0x5a, 0xe1, 0x65, 0x83, 0x1e, 0x93, 
                    0x4f, 0xf7, 0x63, 0xae, 0x46, 0xa2, 0xa6, 0xc1, 
                    0x72, 0xb3, 0xf1, 0xb6, 0x0a, 0x8c, 0xe2, 0x6f},
        show_block);
    store->fetch_block_locator(show_locator);
    std::cin.get();
    return 0;
}

