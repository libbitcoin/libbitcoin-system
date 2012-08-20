#include <bitcoin/bitcoin.hpp>
using namespace libbitcoin;

void show_last(const std::error_code& ec, size_t last_depth)
{
    if (ec)
    {
        log_fatal() << ec.message();
        return;
    }
    log_info() << "Last block depth: " << last_depth;
}

void show_blk_hash(const std::error_code& ec, const message::block& blk)
{
    if (ec)
    {
        log_fatal() << ec.message();
        return;
    }
    log_info() << hash_block_header(blk);
}

void blockchain_started(const std::error_code& ec)
{
    if (ec)
        log_error() << ec.message();
    else
        log_info() << "Blockchain initialized!";
}

int main()
{
    async_service service(1);
    bdb_blockchain chain(service);
    chain.start("database", blockchain_started);
    chain.fetch_last_depth(show_last);
    chain.fetch_block_header(191810, show_blk_hash);
    std::cin.get();
    service.stop();
    service.join();
    chain.stop();
    return 0;
}

