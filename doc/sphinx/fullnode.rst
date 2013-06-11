.. _tut-fullnode:

************************
Full Node Implementation
************************

Recap
=====

The library is based around the concept of *services* which operate through
*threadpools*. We've covered the basics of the different services present
to build Bitcoin applications.

:ref:`Chapter 1<tut-intro>` talked about the design philosophy of libbitcoin.
It was a largely theoretical text about the underpinnings of the library.

:ref:`Chapter 2<tut-quickstart>` gave a quick practical example.

:ref:`Chapter 3<tut-overview>` introduced core libbitcoin concepts, and
an anatomy of the library. We looked at basic data types, the logging subsystem
and useful snippets of the standard library.

:ref:`Chapter 4<tut-crypto>` covered basic crypto, working with keys,
deterministic wallets and converting different key formats.

:ref:`Chapter 5<tut-blockchain>` discussed how to operate the
:class:`blockchain` and use the :class:`leveldb_blockchain` backend.
We saw how the :class:`poller` service :ref:`polls new blocks from nodes<tut-poller>`,
and how new blocks/reorganisations are handled.

:ref:`Chapter 6<tut-network>` illustrated the networking concepts.
:class:`network` is the base service that handles networking, while
:class:`protocol` handles :ref:`joining the peer to peer network<tut-protocol>`.
There are other services that manage different aspects of the Bitcoin
network like :class:`handshake` for the initialization handshake between
2 nodes, or :class:`hosts` for managing lists of Bitcoin hosts.

In this chapter we will introduce the final puzzle piece: unconfirmed
transactions. Putting it all together we will build a full Bitcoin node
in under 200 lines of code.

We will introduce 2 new services. :class:`transaction_pool` is the unconfirmed
transaction memory pool and validates incoming transactions for us.
:class:`session` ties all the services together in a high level wrapper.
It's a convenience service dealing with details like requesting transactions
from the network or starting the protocol service.

Source Code
===========

You can view the source code: :ref:`examples_fullnode`.

Before starting, make sure to have initialized a blockchain database.
::

  $ cd examples/
  $ make
  $ mkdir database/
  $ ./initchain database
  Imported genesis block 000000000019d6689c085ae165831e934ff763ae46a2a6c172b3f1b60a8ce26f
  $ ./fullnode
  ...

Basic Outline
=============

We will make a :class:`fullnode` class that is responsible for holding all
the :class:`threadpool` and :class:`service` objects. There are 2 class methods
to :func:`start` and :func:`stop` it respectively.
::

    class fullnode
    {
    public:
        fullnode();
        void start();
        // Should only be called from the main thread.
        // It's an error to join() a thread from inside it.
        void stop();
    
    private:
        // ...
    
        // Threadpools
        threadpool net_pool_, disk_pool_, mem_pool_;
        // Services
        hosts hosts_;
        handshake handshake_;
        network network_;
        protocol protocol_;
        leveldb_blockchain chain_;
        poller poller_;
        transaction_pool txpool_;
        // Mac OSX needs the bc:: namespace qualifier to compile.
        // Other systems should be OK.
        bc::session session_;
    };

Our :func:`main` function instantiates the fullnode, starts it and then
waits for the user to stop the node by pressing enter.
::

    int main()
    {
        // ...
    
        fullnode app;
        app.start();
        std::cin.get();
        app.stop();
    
        return 0;
    }

The constructor of :class:`fullnode` creates the threadpools and services,
passing their dependencies into the constructor. Services generally use their
constructor for specifying their dependencies.
::

    fullnode::fullnode()
        // Threadpools and the number of threads they spawn.
        // 6 threads spawned in total.
      : net_pool_(1), disk_pool_(4), mem_pool_(1),
        // Networking related services.
        hosts_(net_pool_), handshake_(net_pool_), network_(net_pool_),
        protocol_(net_pool_, hosts_, handshake_, network_),
        // Blockchain database service.
        chain_(disk_pool_),
        // Poll new blocks, and transaction memory pool.
        poller_(mem_pool_, chain_), txpool_(mem_pool_, chain_),
        // Session manager service. Convenience wrapper.
        session_(net_pool_, {
            handshake_, protocol_, chain_, poller_, txpool_})
    {
    }

We also define the :func:`start` and :func:`stop` methods of the
:class:`fullnode`. If :func:`start` fails then :func:`fullnode::handle_start`
will be called with :class:`std::error_code` set. If the
:class:`std::error_code` is set then the error message is displayed.

It's a mistake to call :func:`fullnode::stop` from within the same completion
handler as we would then try to call :func:`threadpool::join` within the
same thread causing a resource deadlock. Instead it is preferable to use
:class:`std::condition_variable` to signal to :func:`main` that it's time
to exit. We leave this as an exercise to the reader.
::

    void fullnode::start()
    {
        // Start blockchain. Must finish before any operations
        // are performed on the database (or they will fail).
        std::promise<std::error_code> ec_chain;
        auto blockchain_started =
            [&](const std::error_code& ec)
            {
                ec_chain.set_value(ec);
            };
        chain_.start("database", blockchain_started);
        std::error_code ec = ec_chain.get_future().get();
        if (ec)
        {
            log_error() << "Problem starting blockchain: " << ec.message();
            return;
        }
        // Start transaction pool
        txpool_.start();
        // Fire off app.
        auto handle_start =
            std::bind(&fullnode::handle_start, this, _1);
        session_.start(handle_start);
    }
    
    void fullnode::stop()
    {
        session_.stop([](const std::error_code&) {});
    
        // Stop threadpools.
        net_pool_.stop();
        disk_pool_.stop();
        mem_pool_.stop();
        // Join threadpools. Wait for them to finish.
        net_pool_.join();
        disk_pool_.join();
        mem_pool_.join();
    
        // Safely close blockchain database.
        chain_.stop();
    }
    
    void fullnode::handle_start(const std::error_code& ec)
    {
        if (ec)
            log_error() << "fullnode: " << ec.message();
    }

Unconfirmed Transactions
========================

Before bitcoin transactions make it into a block, they go into
a transaction memory pool. :class:`transaction_pool` encapsulates that functionality
performing the neccessary validation of a transaction before accepting
it into its internal buffer.
::

    threadpool pool(1);
    // transaction_pool needs access to the blockchain
    blockchain* chain = load_our_backend();
    // create and initialize the transaction memory pool
    transaction_pool txpool(pool, *chain);
    txpool.start();

The :class:`session` service automatically does the task of asking for new
transactions that the :class:`transaction_pool` doesn't have. For every new
connection, we must subscribe to new transactions from the network using
:func:`channel::subscribe_transaction`.

These new transactions must then be validated by attempting to store it in the
transaction memory pool with :func:`transaction_pool::store`.
::

    class fullnode
    {
    public:
        // ...
    
    private:
        // ...
    
        // New connection has been started.
        // Subscribe to new transaction messages from the network.
        void connection_started(const std::error_code& ec, channel_ptr node);
        // New transaction message from the network.
        // Attempt to validate it by storing it in the transaction pool.
        void recv_tx(const std::error_code& ec,
            const transaction_type& tx, channel_ptr node);
        // Result of store operation in transaction pool.
        void new_unconfirm_valid_tx(
            const std::error_code& ec, const index_list& unconfirmed,
            const transaction_type& tx);
    
        // ...
    };

At the beginning of start, we subscribe to new connections.
::

    void fullnode::start()
    {
        // Subscribe to new connections.
        protocol_.subscribe_channel(
            std::bind(&fullnode::connection_started, this, _1, _2));
        // ...
    }

And for every new connection, we subscribe to transaction messages from the
network by calling :func:`channel::subscribe_transaction`. We again call
:func:`protocol::subscribe_channel` to continue being notified of new
connections.
::

    void fullnode::connection_started(const std::error_code& ec, channel_ptr node)
    {
        if (ec)
        {
            log_warning() << "Couldn't start connection: " << ec.message();
            return;
        }
        // Subscribe to transaction messages from this node.
        node->subscribe_transaction(
            std::bind(&fullnode::recv_tx, this, _1, _2, node));
        // Stay subscribed to new connections.
        protocol_.subscribe_channel(
            std::bind(&fullnode::connection_started, this, _1, _2));
    }

Validating The Transaction
--------------------------

The :class:`transaction_pool` interface is deliberately simple to minimise overhead.
This class attempts no tracking of inputs or spends and only provides
a store/fetch paradigm. Tracking must be performed externally and make
use of :func:`transaction_pool::store`'s ``handle_store`` and
``handle_confirm`` to manage changes in the state of memory pool transactions.

.. cpp:function:: void transaction_pool::store(const transaction_type& stored_transaction, confirm_handler handle_confirm, store_handler handle_store)

   Attempt to store a transaction.
   
   ``handle_store`` is called on completion. The second argument is a list
   of unconfirmed input indexes. These inputs refer to a transaction
   that is not in the blockchain and is currently in the memory pool.
   
   In the case where store results in :class:`error::input_not_found`, the
   unconfirmed field refers to the single problematic input.
   ::

    void handle_store(
        const std::error_code& ec,      // Status of operation
        const index_list& unconfirmed   // Unconfirmed input indexes
    );

   ``handle_confirm`` is called when the transaction makes it into a block
   (becoming confirmed) and is removed from the transaction pool.
   ::

    void handle_confirm(
        const std::error_code& ec    // Status of operation
    );

Upon receiving transactions in :func:`fullnode::recv_tx`, we validate the
transaction by attempting to store it in the transaction pool.
::

    void fullnode::recv_tx(const std::error_code& ec,
        const transaction_type& tx, channel_ptr node)
    {
        if (ec)
        {
            log_error() << "Receive transaction: " << ec.message();
            return;
        }
        // Called when the transaction becomes confirmed in a block.
        auto handle_confirm = [](const std::error_code& ec)
            {
                if (ec)
                    log_error() << "Confirm error: " << ec.message();
            };
        // Validate the transaction from the network.
        // Attempt to store in the transaction pool and check the result.
        txpool_.store(tx, handle_confirm,
            std::bind(&fullnode::new_unconfirm_valid_tx, this, _1, _2, tx));
        // Resubscribe to transaction messages from this node.
        node->subscribe_transaction(
            std::bind(&fullnode::recv_tx, this, _1, _2, node));
    }

We now have the result of this sequence of operations. We know whether the
transaction successfully passed validation or not.
::

    void fullnode::new_unconfirm_valid_tx(
        const std::error_code& ec, const index_list& unconfirmed,
        const transaction_type& tx)
    {
        const hash_digest& tx_hash = hash_transaction(tx);
        if (ec)
        {
            log_error()
                << "Error storing memory pool transaction "
                << tx_hash << ": " << ec.message();
        }
        else
        {
            auto l = log_info();
            l << "Accepted transaction ";
            if (!unconfirmed.empty())
            {
                l << "(Unconfirmed inputs";
                for (auto idx: unconfirmed)
                    l << " " << idx;
                l << ") ";
            }
            l << tx_hash;
        }
    }

Requesting Dependencies
-----------------------

If the transaction failed to validate because one of its inputs was missing,
then :class:`error::input_not_found` will be set as the
:class:`std::error_code`, and ``unconfirmed`` will be set to a single value
of which input was missing in the transaction. From this we can request the
missing dependency from the network.
::

    if (ec == error::input_not_found)
    {
        BITCOIN_ASSERT(unconfirmed.size() == 1);
        BITCOIN_ASSERT(unconfirmed[0] < tx.inputs.size());
        size_t missing_index = unconfirmed[0];
        const auto& prevout = tx.inputs[missing_index].previous_output;
        log_info() << "Requesting dependency " << encode_hex(prevout.hash)
            << " for " << encode_hex(tx_hash);
        get_data_type getdata;
        getdata.inventories.push_back(
            {inventory_type_id::transaction, prevout.hash});
        node->send(getdata, depends_requested);
    }

Upon receipt of the dependency transaction from the remote host, and its
successful validation in the :class:`transaction_pool`, we must resubmit this
transaction. Assuming no other inputs are missing, the resubmitted
transaction should then pass validation.

