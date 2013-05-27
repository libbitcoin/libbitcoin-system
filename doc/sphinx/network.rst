.. _tut-network:

****************
Network Protocol
****************

Simple Network Application
==========================

::

    #include <bitcoin/bitcoin.hpp>
    using namespace bc;
    
    void listening_started(const std::error_code& ec, acceptor_ptr accept);
    void accepted_connection(const std::error_code& ec, channel_ptr node);
    
    void listening_started(const std::error_code& ec, acceptor_ptr accept)
    {
        if (ec)
        {
            log_error() << "Listen: " << ec.message();
            return;
        }
        accept->accept(accepted_connection);
    }
    
    void accepted_connection(const std::error_code& ec, channel_ptr node)
    {
        if (ec)
        {
            log_error() << "Accept: " << ec.message();
            return;
        }
        log_info() << "Accepted connection!";
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


.. read user agent.

handshake Service
-----------------

.. mention is a composed operation. link to blockchain section on this.

Network Services
================

Join the Peer To Peer Network
=============================

.. gather stats about user agent. rank top 10 with count by each.

