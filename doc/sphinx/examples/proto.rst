.. _examples_proto:

examples/proto.cpp
#####################

Join the Bitcoin network.

::

    #include <bitcoin/bitcoin.hpp>
    using namespace bc;

    using std::placeholders::_1;
    using std::placeholders::_2;

    // We don't have a database open, and aren't doing any critical file
    // operations so we aren't worried about exiting suddenly.
    void check_error(const std::error_code& ec)
    {
        if (!ec)
            return;
        log_fatal() << ec.message();
        exit(-1);
    }

    // Needed for the C callback capturing the signals.
    bool stopped = false;
    void signal_handler(int sig)
    {
        log_info() << "Caught signal: " << sig;
        stopped = true;
    }

    // Started protocol. Node discovery complete.
    void handle_start(const std::error_code& ec);
    // After number of connections is fetched, this completion handler is called
    // and the number of connections is displayed.
    void display_number_of_connections(
        const std::error_code& ec, size_t connection_count);

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

    void connection_started(const std::error_code& ec, channel_ptr node,
        protocol& prot)
    {
        if (ec)
        {
            log_warning() << "Couldn't start connection: " << ec.message();
            return;
        }
        log_info() << "Connection established.";
        // Resubscribe to new nodes.
        prot.subscribe_channel(
            std::bind(connection_started, _1, _2, std::ref(prot)));
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
        // Perform node discovery if needed and then creating connections.
        prot.start(handle_start);
        // Notify us of new connections.
        prot.subscribe_channel(
            std::bind(connection_started, _1, _2, std::ref(prot)));
        // Catch C signals for stopping the program.
        signal(SIGABRT, signal_handler);
        signal(SIGTERM, signal_handler);
        signal(SIGINT, signal_handler);
        while (!stopped)
        {
            prot.fetch_connection_count(display_number_of_connections);
            sleep(1);
        }
        // Safely close down.
        pool.stop();
        pool.join();
        return 0;
    }

