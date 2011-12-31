#include <iostream>

#include <bitcoin/blockchain/bdb_blockchain.hpp>
#include <bitcoin/utility/logger.hpp>
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

int main()
{
    blockchain_ptr blocks = std::make_shared<bdb_blockchain>("database/");
    blocks->fetch_last_depth(show_last);
    std::cin.get();
    return 0;
}

