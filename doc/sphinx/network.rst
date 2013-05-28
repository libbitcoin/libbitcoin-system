.. _tut-network:

****************
Network Protocol
****************

The Bitcoin protocol was defined by the software released by Satoshi.
But as Satoshi left the project and the ecosystem began to diversify,
the BIP process was created to document and reach consensus on protocol
changes.

`BIP 14 <https://en.bitcoin.it/wiki/BIP_0014>`_ adds a user agent field
to version handshake messages in the Bitcoin protocol. The format of the
user agent field is::

    /Name:Version/Name:Version/.../

Reading the user agent fields from Bitcoin nodes is a useful way to gather
information about clients such as their version or the codebase they are using.
By reading user agents from the network we can gather statistics about the
general health of the Bitcoin network.

Simple Network Application
==========================

Inside :ref:`examples/accept.cpp <examples_accept>` is a simple program that
waits for a connection and displays that node's user agent field.
::

    $ cd examples/
    $ make
    $ ./accept

The daemon will be running on localhost and accepting connections from port
8333 (the standard Bitcoin network port).

The :class:`network` service offers an interface for connecting to remote
nodes. Calling :func:`network::connect` will attempt to establish a connection
calling the handler once the operation is complete.
::

    threadpool pool(1);
    network net(pool);
    net.connect("localhost", 8333, connect_started);

The completion handler is called once the connection is established.
The :class:`channel_ptr` is our connection to the node, at which point
we can begin sending.
::

    void connect_started(const std::error_code& ec, channel_ptr node);

During the first stage of a connection between 2 nodes, they will swap
version messages and version acknowledgement messages. This is the
initialization handshake in the Bitcoin protocol. Before anything happens,
the handshake protocol should be completed.

We define a version message with several fields filled in. The interesting line
is where we set the user agent. Anything is possible in this field, but
`BIP 14 <https://en.bitcoin.it/wiki/BIP_0014>`_ specifies a specific format
for this field.
::

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
    version.start_depth = 0;
    version.nonce = rand();

We then send the message by calling :func:`channel::send` on the
:class:`channel_ptr` object.
::

    // Begin the send.
    // Calls version_sent callback when complete.
    node->send(version, std::bind(version_sent, _1, node));

See :ref:`examples_connect` for the full source code.

Accept Connections
------------------

To accept connections, we begin a listen operation which returns an
:class:`acceptor_ptr`. Then we call :func:`acceptor::accept` which calls
our provided callback when a new connection is made to our node.
::

    net.listen(8333, listening_started);

::

    void listening_started(const std::error_code& ec, acceptor_ptr accept)
    {
        // ...
        // Accept first connection.
        accept->accept(accepted_connection);
    }
    
    void accepted_connection(const std::error_code& ec, channel_ptr node)
    {
        // ...
    }

This initiates the accept, but once a connection is established, the
:class:`acceptor` stops. We therefore need to call :func:`acceptor::accept`
to keep listening for new connections.

We pass the :class:`acceptor_ptr` by binding it as an argument to the callback
for when a new connection is accepted.
::

    void listening_started(const std::error_code& ec, acceptor_ptr accept)
    {
        // ...
        // Accept first connection.
        accept->accept(
            std::bind(accepted_connection, _1, _2, accept));
    }
    
    void accepted_connection(const std::error_code& ec, channel_ptr node,
        acceptor_ptr accept)
    {
        // ...
        log_info() << "Accepted connection!";
        // Keep accepting more connections.
        accept->accept(
            std::bind(accepted_connection, _1, _2, accept));
    }

We want to be notified of version messages from the host and display the
user agent.

.. cpp:function:: void channel::subscribe_version(receive_version_handler handle_receive)

   Subscribe to version messages from the node. Unsubscribes when the message
   is received. To stay subscribed, call this method again inside the
   receiving handler.
   ::

    void handle_receive(
        const std::error_code& ec,  // Status of operation
        const version_type& tx      // Version message
    );

We use this method to receive a single version message, and print the
user agent field.
::

    void accepted_connection(const std::error_code& ec, channel_ptr node,
        acceptor_ptr accept)
    {
        if (ec)
        {
            log_error() << "Accept: " << ec.message();
            return;
        }
        log_info() << "Accepted connection!";
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

See :ref:`examples_connect` for the full source code.

handshake Service
-----------------

Before a node will respond to your traffic, you must complete the version
handshake which is swapping a verack (version acknowledgement) with their
version, and giving a version and getting a verack in response.

We can do this ourselves, but then there are other tasks like fetching our
IP address (which is done from a few websites). We can only speculate why
Satoshi decided to require such a strange task. libbitcoin stays faithful
to the Bitcoin protocol by Satoshi and conforms to this requirement.

For this libbitcoin provides a :class:`handshake` service which performs
the composed operation of doing the initial handshake.

If you're not interested in the process of creating your own network
connection channel and then calling the :class:`handshake` service, then
use the :ref:`composed operation<composed_operations>` :func:`connect`.

Network Services
================

Here is a list of the network-related services present in ``<bitcoin/network/*.hpp>``.

* :class:`channel`: a basic communication channel to a node. Allows you to
  send and receive messages from Bitcoin nodes (see
  ``<bitcoin/primitives.hpp>``).
* :class:`acceptor`: allows you to accept inbound connections from other nodes.
* :class:`network`: creates connections and spawns :class:`acceptor` instances.
* :class:`handshake`: handles the initialization handshake.
* :class:`hosts`: stores a list of all nodes and allows random fetches from that list.
* :class:`protocol`: handles the node discovery mechanism, negotiation
  through the protocol and managing a consistent number of connections.

:class:`protocol` is the class most developers will use to create a bunch of
connections to the network and manage them.

.. cpp:function:: void protocol::subscribe_channel(channel_handler handle_channel)

   Tell us about new connections to the Bitcoin network.
   ::

    void handle_channel(
        channel_ptr node    // New connection
    );

.. cpp:function:: void protocol::fetch_connection_count(fetch_connection_count_handler handle_fetch)

   Fetch the number of connections we have to the network.
   ::

    void handle_fetch(
        const std::error_code& ec,  // Status of operation
        size_t connection_count
    );

.. cpp:function:: void protocol::broadcast<Message>(const Message& packet)

   Broadcast packet to all our connected nodes.

Join the Peer To Peer Network
=============================

We start with a simple program which connects to random Bitcoin nodes in
the network. It displays the connection count every second.

::

    #include <bitcoin/bitcoin.hpp>
    using namespace bc;
    
    // We don't have a database open, and aren't doing any critical file
    // operations so we aren't worried about exiting suddenly.
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
    
    // Needed for the C callback capturing the signals.
    bool stopped = false;
    void signal_handler(int sig)
    {
        log_info() << "Caught signal: " << sig;
        stopped = true;
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

For notifications about new connections, :func:`protocol::subscribe_channel`
calls a given notification handler when a new channel becomes available.
::

    // ...
    prot.start(handle_start);
    // Notify us of new connections.
    // We can subscribe to protocol at any time after start() is called.
    prot.subscribe_channel(
        std::bind(connection_started, _1, std::ref(prot)));

We pass the :class:`protocol` service in by reference and resubscribe.
:func:`connection_started` is *continuously* notified of new
communication channels when they are opened.
::

    void connection_started(channel_ptr node, protocol& prot)
    {
        log_info() << "Connection established.";
        // Resubscribe to new nodes.
        prot.subscribe_channel(
            std::bind(connection_started, _1, std::ref(prot)));
    }

