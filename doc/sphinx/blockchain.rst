.. _tut-blockchain:

*******************************
Fun With The Bitcoin Blockchain
*******************************

For this tutorial we will use the :class:`leveldb_blockchain` backend, which
uses the LevelDB database to store the Bitcoin blockchain.
Backends implement the :class:`blockchain` interface. Starting and stopping
a backend is down to the individual implementation.

The blockchain can consume a large number of open file handles during normal
operation. In particular, the LevelDB backend may accumulate a number of data
files. The creation of numerous data files is normal. Most operating
systems can change the open-files limit using the ``ulimit -n`` command.
Example::

    ulimit -n 4096

However, this only changes the limit for the current shell session. Changing the
limit on a system-wide, permanent basis varies more between systems. See
`this guide <http://docs.basho.com/riak/latest/cookbooks/Open-Files-Limit/>`_
for more information on open files limits.

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

The only thing we add to the blockchain is new blocks. There is one method called
:func:`blockchain::store`. This method handles the internal details of
validating the block against the current blockchain, returning competing blocks
to the orphan pool (if needed), insertion into the database and processing
any dependent blocks.

In our example we want to fetch and display the last block header. To fetch
the last depth number, we use :func:`blockchain::fetch_last_depth`. To fetch
the block header for a depth number, we use
:func:`blockchain::fetch_block_header`.

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

All the blockchain fetch methods give you access to all of the data in the
blockchain to reconstruct or link any piece of data. Full navigation around
the chain is possible.

Starting at the basic level, we start with an application to start the
blockchain otherwise report the error back.

We create a threadpool, blockchain, and then call start. Then after when
the user is ready to exit, we stop and join the threadpool, and safely
close the blockchain.

::

    #include <bitcoin/bitcoin.hpp>
    using namespace bc;
    
    blockchain* chain = nullptr;
    
    // Completion handler for when the blockchain has finished initializing.
    void blockchain_started(const std::error_code& ec)
    {
        // std::error_code's can be tested like bools, and
        // compared against specific error enums.
        // See <bitcoin/error.hpp> for a full list of them.
        if (ec)
        {
            log_error() << "Blockchain failed to start: " << ec.message();
            return;
        }
        // Blockchain has safely started.
        log_info() << "Blockchain started.";
    }
    
    int main()
    {
        // Define a threadpool with 1 thread.
        threadpool pool(1);
        // Create a LevelDB blockchain.
        leveldb_blockchain ldb_chain(pool);
        // Initialize our global 'chain' pointer from above.
        chain = &ldb_chain;
        // Start the database using its implementation specific method.
        ldb_chain.start("../database", blockchain_started);
        // Keep running until the user presses enter.
        // Since libbitcoin is asynchronous, you need to synchronise with
        // them to know when to exit safely.
        // For these examples we just pause until enter for simplicity sake.
        std::cin.get();
        // Begin stopping the threadpools in parallel (only 1 here).
        pool.stop();
        // Join them one by one.
        pool.join();
        // Finally stop the blockchain safely now everything has stopped.
        ldb_chain.stop();
        return 0;
    }

After the blockchain has started, we want to begin the entire process.
The process starts with getting the last depth in our blockchain, then
fetching the block header at that depth, and finally displaying the
block header to the screen.

::

    // Completion handler for when the blockchain has finished initializing.
    void blockchain_started(const std::error_code& ec);
    // Fetch tbe last block now that we have the depth.
    void depth_fetched(const std::error_code& ec, size_t last_depth);
    // Result: print the block header.
    void display_block_header(const std::error_code& ec, const block_type& blk);

After the blockchain has started, we begin the operation to fetch the last
depth, calling :func:`depth_fetched` after it's finished.

::

    void blockchain_started(const std::error_code& ec)
    {
        // std::error_code's can be tested like bools, and
        // compared against specific error enums.
        // See <bitcoin/error.hpp> for a full list of them.
        if (ec)
        {
            log_error() << "Blockchain failed to start: " << ec.message();
            return;
        }
        // Blockchain has safely started.
        log_info() << "Blockchain started.";
        // chain should've been set inside main().
        assert(chain);
        // Begin fetching the last depth number.
        chain->fetch_last_depth(depth_fetched);
    }

After :func:`depth_fetched` has been called, we know the block number and
begin fetching the block header.

::

    void depth_fetched(const std::error_code& ec, size_t last_depth)
    {
        if (ec)
        {
            log_error() << "Failed to fetch last depth: " << ec.message();
            return;
        }
        // Display the block number.
        log_info() << "Depth: " << last_depth;
        assert(chain);
        // Begin fetching the block header.
        chain->fetch_block_header(last_depth, display_block_header);
    }

Now finally the block header is received, and can be displayed. This is
the final operation in this sequence.

As we only requested the block header, the transactions list will be
empty. Getting a full block involves getting the transaction hashes
associated with that block, and fetching each one which is provided
by the composed operation :func:`fetch_block`.

::

    void display_block_header(const std::error_code& ec, const block_type& blk)
    {
        if (ec)
        {
            log_error() << "Failure fetching block header: " << ec.message();
            return;
        }
        // 32 byte std::array of uint8_t
        const hash_digest& blk_hash = hash_block_header(blk);
        // Encode block hash into a pretty hex string.
        log_info() << "hash: " << pretty_hex(blk_hash);
        // Display a few fields from the block header.
        // See <bitcoin/primitives.hpp> for the definition of block_type.
        log_info() << "version: " << blk.version;
        // hash_digest can be used directly in log_info(),
        // implicity calling pretty_hex() on the hash_digest.
        log_info() << "previous_block_hash: " << blk.previous_block_hash;
        log_info() << "merkle: " << blk.merkle;
        log_info() << "timestamp: " << blk.timestamp;
        log_info() << "bits: " << blk.bits;
        log_info() << "nonce: " << blk.nonce;
        // This is not the full block, only the header.
        // For the full block use fetch_block() instead.
        assert(blk.transactions.size() == 0);
        // A goodbye message.
        log_info() << "Finished.";
    }

The full example is in :ref:`examples/display-last.cpp <examples_display-last>`.

Message from Satoshi, Bitcoin's creator
=======================================

Satoshi left us a message inside the first Bitcoin *genesis* block.
::

    // The Times 03/Jan/2009 Chancellor on brink of second bailout for banks

The message is inside the first input, of the first transaction, of the
first Bitcoin block.

Block 0 is predefined by Bitcoin. All blockchains must begin with
the same block otherwise they aren't Bitcoin. :func:`genesis_block`
recreates a copy of block 0.

#. Create genesis block.
#. Lookup first transaction in block (the coinbase transaction).
#. Get the first input from the coinbase transaction.
#. Serialize the input's script back into raw form.
#. Display the raw input script.

The input script for the first input of the coinbase transaction inside the
genesis block contains the message from Satoshi.

::

    // examples/satoshiwords.cpp
    #include <bitcoin/bitcoin.hpp>
    using namespace bc;
    
    int main()
    {
        // Create genesis block.
        block_type genblk = genesis_block();
        // Genesis block contains a single coinbase transaction.
        assert(genblk.transactions.size() == 1);
        // Get first transaction in block (coinbase).
        const transaction_type& coinbase_tx = genblk.transactions[0];
        // Coinbase tx has a single input.
        assert(coinbase_tx.inputs.size() == 1);
        const transaction_input_type& coinbase_input = coinbase_tx.inputs[0];
        // Get the input script (sometimes called scriptSig).
        const script& input_script = coinbase_input.input_script;
        // Convert this to its raw format.
        const data_chunk& raw_block_message = save_script(input_script);
        // Convert this to an std::string.
        std::string message;
        message.resize(raw_block_message.size());
        std::copy(raw_block_message.begin(), raw_block_message.end(),
            message.begin());
        // Display the genesis block message.
        std::cout << message << std::endl;
        return 0;
    }

Reconstruct Block Transactions
==============================

To reconstruct an entire block from a block header, first obtain a list of
transaction hashes that makeup that block. Then iterate the list of
transaction hashes, fetching the transactions one by one.

.. cpp:function:: void blockchain::fetch_block_transaction_hashes(const hash_digest &block_hash, fetch_handler_block_transaction_hashes handle_fetch)

   Fetches list of transaction hashes in a block given the block hash.
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

.. _composed_operations:

:func:`fetch_block` and Composed Operations
-------------------------------------------

libbitcoin provides a convenience function :func:`fetch_block` to wrap the
details of fetching a full block. These kind of operations that wrap a bunch
of other operations are called *composed operations*.

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

.. _tut-poller:

Polling Blocks From Nodes
=========================

The :class:`poller` service downloads blocks from nodes into the blockchain.
::

    // ...

    void connection_established(const std::error_code& ec, channel_ptr node,
        poller& poll)
    {
        // ...
        // getblocks request asking node for a list of blocks to download.
        // Usually you call query() on the first node you connect to.
        poll.query(node);
        // Monitor for inventory packets containing blocks we don't have.
        // Then request and attempt to store the blocks in the blockchain.
        poll.monitor(node);
    }

    int main()
    {
        threadpool pool(2);
        leveldb_blockchain chain(pool);
        // ...
        poller poll(pool, chain);
        // ...
        return 0;
    }

:class:`poller` handles the details of watching for notification of new blocks,
sending requests as needed and storing them in the blockchain by calling
:func:`blockchain::store`.

Reorganizations And New Blocks
------------------------------

While polling new blocks from the network, callbacks registered with
:func:`blockchain::subscribe_reorganize` will be notified of any changes
to the blockchain.

.. cpp:function:: void blockchain::subscribe_reorganize(reorganize_handler handle_reorganize)

   Be notified of the next blockchain change.
   
   Subscriber is notified exactly once of changes to the blockchain
   and needs to re-subscribe to continue being notified.
   ::
   
    void handle_reorganize(
        const std::error_code& ec,   // Status of operation
        size_t fork_point,           // Index where blockchain forks
        const block_list& added,     // New blocks added to blockchain
        const block_list& removed    // Blocks removed (empty if none)
    );

The ``fork_point`` gives the depth of the ancestor block before the split.
Both lists are ordered from lowest depth first.
::

    for (size_t i = 0; i < added_blocks.size(); ++i)
    {
        size_t depth = fork_point + 1 + i;
        const block_type& blk = *added_blocks[i];
    }

Optimization Techniques
=======================

* Create a separate partition for the database directory with the
  *noatime* parameter set.
* Lower VM `swappiness <http://en.wikipedia.org/wiki/Swappiness>`_.
* Increase the `max number of open files <http://stackoverflow.com/questions/34588/how-do-i-change-the-number-of-open-files-limit-in-linux>`_.
* Disable filesystem caching to prevent double caching by database and
  operating system.
* Make heavy use of replication i.e multiple :class:`blockchain` instances
  each with their own database.
* Follow the `guidelines here <http://docs.basho.com/riak/latest/tutorials/choosing-a-backend/LevelDB/>`_.

