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

void display_num_conns(const std::error_code& ec, size_t num)
{
    log_debug() << num << " CONNECTIONS";
}
void mewcj(channel_ptr node)
{
    log_debug() << "New channel!";
    prot->subscribe_channel(mewcj);
}

int main()
{
    async_service service(1);
    prot = std::make_shared<protocol>(service);
    prot->start(handle_start);
    prot->subscribe_channel(mewcj);
    //prot->bootstrap(handle_start);
    //std::cin.get();
    //prot->stop(handle_stop);
    //log_debug() << "stopping.";
    //std::cin.get();
    while (true)
    {
        prot->fetch_connection_count(display_num_conns);
        sleep(1);
    }
    return 0;
}

