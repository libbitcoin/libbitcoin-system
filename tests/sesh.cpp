#include <bitcoin/bitcoin.hpp>
using namespace libbitcoin;

#include <bitcoin/blockchain/bdb_blockchain.hpp>

void error_exit(const std::string& message, int status=1)
{
    log_error() << "session: " << message;
    exit(status);
}

void handle_start(const std::error_code& ec)
{
    if (ec)
        error_exit(ec.message());
}

int main()
{
    bdb_blockchain::setup("database");
    session s;
    s.start(handle_start);
    std::cin.get();
    return 0;
}

