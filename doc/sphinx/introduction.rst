.. _tut-intro:

***************
Introduction
***************

libbitcoin is a Bitcoin library targeted towards high end use. The library
places a heavy focus around asychronicity. This enables a big scope for future
scalability as each component has its own thread pool. By increasing the number
of threads for that component the library is able to scale outwards across CPU
cores. This will be vital in the future as the demands of the Bitcoin network
grow.

Another core design principle is libbitcoin is not a framework, but a toolkit.
Frameworks hinder development during the latter stages of a development cycle,
enforce one style of coding and do not work well with other frameworks. By
contrast, we have gone to great pains to make libbitcoin function as an
independent set of mutual components with no dependencies between them.

The approach we took to our threaded design is built not around the data, but
around tasks. On a finer level: operations. libbitcoin is a toolkit library that
uses the proactor design pattern. It implements the proactor pattern through the
use of completion handlers like in boost::asio.

* **Scalability**. The library should facilitate the development of applications
  that scale to thousands of concurrent operations.
* **Model concepts in an intuitive manner**. The library models different
  subsystems of Bitcoin in a clear and intuitive manner. We choose abstractions
  that allow designing a wide range of applications that rely on Bitcoin.
* **Basis for further abstraction**. The library should permit the development
  of other libraries that provide higher levels of abstraction. For example,
  implementations of the Bitcoin protocol in other networks such as Tor.
* **No blocking**. No blocking ever occurs waiting for another thread to complete
  (except possibly on a low level within boost dispatches- but that is uncommon).
* **UNIX approach**. The library attempts to provide small units of
  functionality that perform one single task. Our philosophy is to break down
  higher level functionality into small parts and to simply provide those
  parts. The cost is inconvenience. The benefit is flexibility.

.. _intro_design:

Design
======

libbitcoin follows a few basic code design principles that quality does not
necessarily increase with functionality. There is a point where less
functionality is a preferable option in terms of practicality and usability.

* **Simplicity**. It is more important for the implementation to be simple than
  the interface. Simplicity is the most important consideration in a design.
* **Correctness**. The design should be correct in all aspects.
* **Consistency**. The design must not be overly inconsistent. Consistency can
  be sacrificed for simplicity in some cases, but it is better to drop those
  parts of the design that deal with less common circumstances than to
  introduce either complexity or inconsistency in the implementation.
* **Completeness**. The design must cover as many important situations as is
  practical. Completeness must be sacrificed whenever implementation simplicity
  is jeopardized.

Unix and C are examples of this design. Small building blocks that are flexible
in how they combine together.

Generally the API focuses on implementation simplicity and only implements the
bare neccessary functionality. Keep implementation simple and don't pollute
class interfaces. Instead composed operations wrap lower level class methods
to simplify common operations.
::

    threadpool pool(1);
    network net(pool);
    handshake shake(pool);
    // ...
    connect(shake, net, "localhost", 8333, handle_handshake);

Composed operations take the services they wrap as their primary arguments
before their function parameters.

**Classes do not implement more functionality than is neccessary.**

Dependency Injection
--------------------

Dependency injection is a software design pattern that allows removing
hard-coded dependencies and making it possible to change them, whether at
run-time or compile-time.

Instead of having your objects creating a dependency or asking a factory
object to make one for them, you pass the needed dependencies into the
constructor.
::

    threadpool pool(1);
    leveldb_blockchain chain(pool);
    // The dependencies for transaction_pool are passed into its constructor.
    // We could instead pass in a bdb_blockchain.
    transaction_pool txpool(pool, chain);

The Zen of libbitcoin
=====================

| Readability over speed.
| Beauty over convenience.
| Simplicity over complexity.
| Architected, not hacked.
| Flat, not nested.
| Explicit, not implicit.
| Errors should be loud.
| Never is better than right now.
| Now is better than never.
| Be flexible and configurable.
| Build houses from bricks, software from modules. 

Examples
========

Each section comes with its own example code listing. These can be found in
the examples/ directory of the source libbitcoin package.

* :ref:`priv.cpp <examples_priv>` generates new private keys, shows a Bitcoin address and sign or
  verify data using the private key.
* :ref:`determ.cpp <examples_determ>` shows working with deterministic wallets, generating both
  public and private keys.
* :ref:`initchain.cpp <examples_initchain>`, initialize :class:`leveldb_blockchain` by creating a
  blank new database and adding the genesis block at depth 0.
* :ref:`display-last.cpp <examples_display-last>` fetches and displays the last
  block in a blockchain database.
* :ref:`fullnode.cpp <examples_fullnode>` is a full node Bitcoin implementation. Blocks and
  unconfirmed transactions are validated.

