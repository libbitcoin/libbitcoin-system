#include <stdexcept>
#include <algorithm>
#include <vector>
#include <iostream>
#include <iomanip>
#include <openssl/bn.h>

#include <bitcoin/util/big_number.hpp>
#include <bitcoin/util/logger.hpp>
#include <bitcoin/types.hpp>

using libbitcoin::data_chunk;
using libbitcoin::hash_digest;
using libbitcoin::big_number;

template<typename T>
std::string serialize_bytes(T data)
{
    std::stringstream ss;
    ss << std::hex;
    for (int val: data)
        ss << std::setw(2) << std::setfill('0') << val << ' ';
    // Remove end ' '
    std::string ret = ss.str();
    ret.resize(ret.size() - 1);
    return ret;
}

void test()
{
    big_number bignum;
    bignum.set_compact(0x1b0404cb);
    assert((bignum.get_hash() == hash_digest{0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x04, 0xCB, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}));

    big_number bignum_max;
    bignum_max.set_compact(0x1d00ffff);
    assert((bignum_max.get_hash() == hash_digest{0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}));

    assert(bignum < bignum_max);

    big_number other;
    other.set_hash(bignum.get_hash());
    assert(other.get_data() == bignum.get_data());
}

int main()
{
    test();
    return 0;
}

