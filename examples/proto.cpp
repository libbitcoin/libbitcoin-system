#include <bitcoin/bitcoin.hpp>
using namespace bc;

// We don't have a database open, and aren't doing any critical file
// operations so we aren't worries about exiting suddenly.
void check_error(const std::error_code& ec)
{
    if (!ec)
        return;
    log_fatal() << ec.message();
    exit(-1);
}

void handle_start(const std::error_code& ec)
{
    check_error(ec);
    log_debug() << "Started.";
}

void display_number_of_connections(
    const std::error_code& ec, size_t connection_count)
{
    check_error(ec);
    log_debug() << connection_count << " CONNECTIONS";
}

int main()
{
    threadpool pool(1);
    // Create dependencies for our protocol object.
    hosts hst(pool);
    handshake hs(pool);
    network net(pool);
    // protocol service.
    protocol prot(pool, hst, hs, net);
    prot.start(handle_start);
    while (true)
    {
        prot.fetch_connection_count(display_number_of_connections);
        sleep(1);
    }
    // Never reaches here.
    // Shown for example sake.
    pool.stop();
    pool.join();
    return 0;
}

