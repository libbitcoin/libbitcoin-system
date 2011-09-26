#include <stdexcept>
#include <algorithm>
#include <vector>
#include <iostream>
#include <iomanip>
#include <openssl/bn.h>

#include <bitcoin/util/assert.hpp>
#include <bitcoin/util/big_number.hpp>
#include <bitcoin/util/logger.hpp>
#include <bitcoin/types.hpp>
#include <bitcoin/constants.hpp>

using libbitcoin::data_chunk;
using libbitcoin::hash_digest;
using libbitcoin::big_number;
using libbitcoin::hexlify;
using libbitcoin::log_debug;

void test()
{
    big_number bignum;
    bignum.set_compact(0x1b0404cb);
    BITCOIN_ASSERT((bignum.get_hash() == hash_digest{0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x04, 0xCB, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}));

    big_number bignum_max;
    bignum_max.set_compact(0x1d00ffff);
    BITCOIN_ASSERT((bignum_max.get_hash() == hash_digest{0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}));
    BITCOIN_ASSERT(bignum_max.get_compact() == 0x1d00ffff);

    BITCOIN_ASSERT(bignum < bignum_max);

    big_number other;
    other.set_hash(bignum.get_hash());
    BITCOIN_ASSERT(other.get_data() == bignum.get_data());

    BITCOIN_ASSERT(bignum_max <= libbitcoin::max_target());
    BITCOIN_ASSERT(bignum < libbitcoin::max_target());

    big_number arith(0x1f1f1f1f);
    arith *= 0x1f1f1f1f;
    BITCOIN_ASSERT((arith.get_data() == data_chunk{0x03, 0xc8, 0x8d, 0x52, 0x0f, 0x4a, 0x85, 0xc1}));
}

int main()
{
    test();
    return 0;
}

