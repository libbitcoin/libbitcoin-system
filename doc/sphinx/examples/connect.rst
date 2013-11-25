.. _examples_connect:

examples/connect.cpp
#####################

Connect to a Bitcoin node on localhost, port 8333.

::

    #include <bitcoin/bitcoin.hpp>
    using namespace bc;
    
    using std::placeholders::_1;
    
    // Connection is established.
    // Send version message to remote host.
    void connect_started(const std::error_code& ec, channel_ptr node);
    // Verson message finished sending.
    // Program completed.
    void version_sent(const std::error_code& ec, channel_ptr node);
    
    void connect_started(const std::error_code& ec, channel_ptr node)
    {
        if (ec)
        {
            log_error() << "Connect: " << ec.message();
            return;
        }
        // Create our version message we want to send.
        // Fill in a bunch of fields.
        version_type version;
        version.version = 60000;
        version.services = 1;
        version.address_me.services = version.services;
        version.address_me.ip =
            ip_address_type{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
                            0x00, 0x00, 0xff, 0xff, 0x0a, 0x00, 0x00, 0x01};
        version.address_me.port = 8333;
        version.address_you.services = version.services;
        version.address_you.ip = 
            ip_address_type{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
                            0x00, 0x00, 0xff, 0xff, 0x0a, 0x00, 0x00, 0x01};
        version.address_you.port = 8333;
        // Set the user agent.
        version.user_agent = "/libbitcoin/connect-test/";
        version.start_height = 0;
        version.nonce = rand();
        // Begin the send.
        // Calls version_sent callback when complete.
        node->send(version, std::bind(version_sent, _1, node));
    }
    
    void version_sent(const std::error_code& ec, channel_ptr node)
    {
        if (ec)
            log_error() << "Sending version: " << ec.message();
        else
            log_info() << "Version sent.";
    }
    
    int main()
    {
        threadpool pool(1);
        network net(pool);
        net.connect("localhost", 8333, connect_started);
        std::cin.get();
        pool.stop();
        pool.join();
        return 0;
    }

