.. _tut-quickstart:

**********
Quickstart
**********

::

    #include <bitcoin/bitcoin.hpp>
    using namespace bc;

    int main()
    {
        block_type blk = genesis_block();
        std::cout << pretty_hex(hash_block_header(blk)) << std::endl;
        return 0;
    }

::

    $ g++ -o test main.cpp $(pkg-config --cflags --libs libbitcoin)
    $ ./test
    000000000019d6689c085ae165831e934ff763ae46a2a6c172b3f1b60a8ce26f

