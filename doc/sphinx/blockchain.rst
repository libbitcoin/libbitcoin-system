.. _tut-blockchain:

*******************************
Fun With The Bitcoin Blockchain
*******************************

For this tutorial we will use the :class:`leveldb_blockchain` backend, which
uses the LevelDB database to store the Bitcoin blockchain.
Backends implement the :class:`blockchain` interface. Starting and stopping
a backend is down to the individual implementation.

Initialization: Importing The Genesis Block
===========================================

Before we can use :class:`leveldb_blockchain`, the database needs to be
initialised.

#. Create a :class:`leveldb_blockchain` object. If no database exists in the
   specified path, it will automatically be created.
#. Recreate the genesis block. The first block in the Bitcoin blockchain is
   part of the specification for the Bitcoin standard.
#. Import the genesis block at depth 0 in the blockchain.

You now have a working :class:`leveldb_blockchain`.

To create the database which initialises a new one if one doesn't already
exist, we call :func:`leveldb_blockchain::start` with a root path. The
completion handler is called passing an :class:`std::error_code` that
indicates whether the operation was successful or not.

::

    // Threadpool context containing 1 thread.
    threadpool pool(1);
    // leveldb_blockchain operations execute in pool's thread.
    leveldb_blockchain chain(pool);
    // Completion handler for starting the leveldb_blockchain.
    // Does nothing.
    auto blockchain_start = [](const std::error_code& ec) {};
    // Start blockchain with a database path.
    chain.start(dbpath, blockchain_start);

The function :func:`genesis_block` returns a genesis block.

::

    // First block is the genesis block.
    block_type first_block = genesis_block();

We call :func:`blockchain::import` to save a block in the blockchain
at a specified depth directly. It doesn't validate or perform any safety
checks on the imported block. Instead the block is written directly.

::

    // Completion handler for import method.
    auto import_finished = 
        [&ec_promise](const std::error_code& ec)
        {
            ec_promise.set_value(ec);
        };
    // Import the genesis block at depth 0.
    // Doesn't validate or perform checks on the block.
    chain.import(first_block, 0, import_finished);
    // Wait until std::error_code is set by
    // import_finished completion handler.
    std::error_code ec = ec_promise.get_future().get();
    if (ec)
    {
        log_error() << "Importing genesis block failed: " << ec.message();
        return -1;
    }

All operations need to finish on :class:`leveldb_blockchain` before it can
be closed properly so we first stop the threadpool before calling
:func:`leveldb_blockchain::stop`.

::

    // All threadpools stopping in parallel...
    pool.stop();
    // ... Make them all join main thread and wait until they finish.
    pool.join();
    // Now safely close leveldb_blockchain.
    chain.stop();

:func:`blockchain::store` is the recommended way to add new blocks to
the blockchain. It finds the correct depth by looking up the previous block,
handles reorganisations, validates the blocks and calls the subscription
handlers.

.. cpp:function:: void blockchain::store(const block_type& block, store_block_handler handle_store)

   Store a new block.
   
   Subscriber is notified exactly once of changes to the blockchain
   and needs to re-subscribe to continue being notified.
   ::

    void handle_store(
        const std::error_code& ec,   // Status of operation
        block_info info              // Status and depth of block
    );

The full sourcecode can be found in :ref:`examples_initchain`.

Fetch and Display Block Info
============================

Services like blockchain do not block. Methods return immediately and upon
completion call a completion handler. The semantics of the blockchain reflect
this with the ``set/get_*`` methods being equivalently called ``store/fetch_*``.

We only store new blocks in the blockchain. There is one method called
:func:`blockchain::store`. This method handles the internal details of
validating the block against the current blockchain, returning competing blocks
to the orphan pool (if needed), insertion into the database and processing
any dependent blocks.

.. cpp:function:: void blockchain::fetch_block_header(size_t depth, fetch_handler_block_header handle_fetch)

   Fetches the block header by depth.
   ::
   
    void handle_fetch(
        const std::error_code& ec,  // Status of operation
        const block_type& blk       // Block header
    );

.. cpp:function:: void blockchain::fetch_last_depth(fetch_handler_last_depth handle_fetch)

   Fetches the depth of the last block in our blockchain.
   ::

    void handle_fetch(
        const std::error_code& ec, // Status of operation
        size_t block_depth         // Depth of last block
    );

These methods give you access to all of the data in the blockchain to
reconstruct or link any piece of data.

Message from Satoshi, Bitcoin's creator
------------------------------------------------

message from satoshi in tx 0 coinbase

Reconstruct Block Transactions
==============================

.. cpp:function:: void blockchain::fetch_block_transaction_hashes(const hash_digest &block_hash, fetch_handler_block_transaction_hashes handle_fetch)

   Fetches list of transaction hashes in a block by block hash.
   ::

    void handle_fetch(
        const std::error_code& ec,      // Status of operation
        const inventory_list& hashes    // List of hashes
    );

.. cpp:function:: void blockchain::fetch_transaction(const hash_digest &transaction_hash, fetch_handler_transaction handle_fetch)

   Fetches a transaction by hash
   ::

    void handle_fetch(
        const std::error_code& ec,  // Status of operation
        const transaction_type& tx  // Transaction
    );

:func:`fetch_block` and Composed Operations
-------------------------------------------

A general :ref:`design principle of libbitcoin <intro_design>` is to keep the implementation simple
and not pollute class interfaces. Instead composed operations wrap lower
level class methods to simplify common operations.

.. cpp:function:: void fetch_block(blockchain& chain, size_t depth, blockchain_fetch_handler_block handle_fetch)

   Fetch a block by depth.
   If the blockchain reorganises, operation may fail halfway.
   ::

    void handle_fetch(
        const std::error_code& ec,  // Status of operation
        const block_type& blk       // Block header
    );

Fetch and Dump Transactions
===========================

Store More Blocks
=================

Polling Blocks From Nodes
-------------------------

.. Briefly mention poller here.

