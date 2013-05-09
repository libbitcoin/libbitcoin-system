#include <bitcoin/bitcoin.hpp>
using namespace bc;

#include <fstream>
#include <iostream>

void error_exit(const std::string& message, int status=1)
{
    log_fatal() << "feeder: " << message;
    exit(status);
}

void blockchain_started(const std::error_code& ec)
{
    if (ec)
        error_exit(ec.message());
    else
        log_info() << "Blockchain initialized!";
}

std::string status_str(block_status status)
{
    switch (status)
    {
        case block_status::orphan:
            return "orphan";
        case block_status::confirmed:
            return "confirmed";
        case block_status::rejected:
            return "rejected";
        default:
            return "<unknown>";
    }
}

void block_stored(const std::error_code& ec, block_info info)
{
    if (ec)
    {
        log_error() << "feeder: " << ec.message();
        return;
    }
    log_info() << "Block stored:";
    log_info() << "    Depth: " << info.depth;
    log_info() << "    Status: " << status_str(info.status);
}

int main(int argc, char** argv)
{
    if (argc < 2)
        return 1;
    threadpool pool(1);
    bdb_blockchain chain(pool);
    chain.start("database", blockchain_started);
    for (int i = 1; i < argc; ++i)
    {
        std::ifstream inf(argv[i]);
        std::string pretty_rep;
        std::getline(inf, pretty_rep);
        message::block blk;
        data_chunk blockdat = bytes_from_pretty(pretty_rep);
        satoshi_load(blockdat.begin(), blockdat.end(), blk);
        BITCOIN_ASSERT(blk.merkle == generate_merkle_root(blk.transactions));
        log_info() << "Loaded " << hash_block_header(blk);
        chain.store(blk, block_stored);
    }
    std::cin.get();
    pool.stop();
    pool.join();
    chain.stop();
    log_info() << "Exiting...";
    return 0;
}

