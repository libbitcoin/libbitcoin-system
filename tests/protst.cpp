#include <bitcoin/bitcoin.hpp>
using namespace libbitcoin;

protocol_ptr prot;

void error_exit(const std::error_code& ec)
{
    log_fatal() << "Error: " << ec.message();
    exit(-1);
}

void handle_stop(const std::error_code& ec)
{
    if (ec)
        error_exit(ec);
    log_debug() << "stopped";
}
void handle_start(const std::error_code& ec)
{
    if (ec)
        error_exit(ec);
    log_debug() << "started";
    //prot->stop(handle_stop);
}
int main()
{
    prot = std::make_shared<protocol>();
    prot->start(handle_start);
    std::cin.get();
    prot->stop(handle_stop);
    log_debug() << "stopping.";
    std::cin.get();
    return 0;
}

