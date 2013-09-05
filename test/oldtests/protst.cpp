#include <bitcoin/bitcoin.hpp>
using namespace libbitcoin;

protocol* prot;

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
    threadpool pool(1);
    hosts hst(pool);
    handshake hs(pool);
    network net(pool);
    prot = new protocol(pool, hst, hs, net);
    prot->start(handle_start);
    prot->subscribe_channel(mewcj);
    //prot->bootstrap(handle_start);
    //std::cin.get();
    //prot->stop(handle_stop);
    //log_debug() << "stopping.";
    std::cin.get();
    //while (true)
    //{
    //    prot->fetch_connection_count(display_num_conns);
    //    sleep(1);
    //}
    pool.stop();
    pool.join();
    delete prot;
    return 0;
}

