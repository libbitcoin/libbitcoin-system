#ifndef LIBBITCOIN_BIG_NUMBER_H
#define LIBBITCOIN_BIG_NUMBER_H

#include <openssl/bn.h>

#include <bitcoin/types.hpp>

namespace libbitcoin {

class big_number
{
public:
    big_number();
    big_number(int value);
    void set_compact(uint32_t compact);
    void set_data(data_chunk data);
    data_chunk get_data();
    void set_hash(hash_digest hash);
    hash_digest get_hash();

    bool operator==(const big_number& other);
    bool operator!=(const big_number& other);
    bool operator<=(const big_number& other);
    bool operator>=(const big_number& other);
    bool operator<(const big_number& other);
    bool operator>(const big_number& other);

private:
    BIGNUM bignum_;
};

} // libbitcoin

#endif

