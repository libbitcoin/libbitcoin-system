/*
  Accept connections from Bitcoin nodes on port 8333.
*/
#include <bitcoin/bitcoin.hpp>
using namespace bc;

using std::placeholders::_1;
using std::placeholders::_2;

// Listening has started.
// Wait to accept a connection.
void listening_started(const std::error_code& ec, acceptor_ptr accept);
// Connection to another Bitcoin node has been established.
// Wait for version message.
void accepted_connection(const std::error_code& ec, channel_ptr node,
    acceptor_ptr accept);
// Version message received.
// Display the user agent.
void version_received(const std::error_code& ec, const version_type& version,
    channel_ptr node);
void node_stopped(const std::error_code& ec);

void listening_started(const std::error_code& ec, acceptor_ptr accept)
{
    if (ec)
    {
        log_error() << "Listen: " << ec.message();
        return;
    }
    // Accept first connection.
    accept->accept(
        std::bind(accepted_connection, _1, _2, accept));
}

void accepted_connection(const std::error_code& ec, channel_ptr node,
    acceptor_ptr accept)
{
    if (ec)
    {
        log_error() << "Accept: " << ec.message();
        return;
    }
    log_info() << "Accepted connection!";
    node->subscribe_stop(node_stopped);
    // Now we need to keep it alive otherwise the connection is closed.
    node->subscribe_version(
        std::bind(version_received, _1, _2, node));
    // Keep accepting more connections.
    accept->accept(
        std::bind(accepted_connection, _1, _2, accept));
}

void version_received(const std::error_code& ec, const version_type& version,
    channel_ptr node)
{
    // error::service_stopped means the connection was closed.
    if (ec == error::service_stopped)
        return;
    else if (ec)
    {
        log_error() << "Version message: " << ec.message();
        return;
    }
    log_info() << "User agent: " << version.user_agent;
}

void node_stopped(const std::error_code& ec)
{
    if (ec == error::service_stopped)
        log_info() << "Connection closed.";
    else if (ec)
        log_error() << "Connection closed: " << ec.message();
}

int main()
{
    threadpool pool(4);
    network net(pool);
    net.listen(8333, listening_started);
    std::cin.get();
    pool.stop();
    pool.join();
    return 0;
}

