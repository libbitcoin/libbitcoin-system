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

void blockchain_started(const std::error_code& ec, blockchain*)
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
    std::cin.get();
    service.shutdown();
    return 0;
}

