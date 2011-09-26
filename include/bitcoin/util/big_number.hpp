#ifndef LIBBITCOIN_BIG_NUMBER_H
#define LIBBITCOIN_BIG_NUMBER_H

#include <openssl/bn.h>

#include <bitcoin/types.hpp>

namespace libbitcoin {

// :se ft=cpp  if vim fails to apply c++ highlighting
// TODO: Lots of things *should* throw on error in this class
class big_number
{
public:
    big_number();
    big_number(uint64_t value);

    void set_compact(uint32_t compact);
    uint32_t get_compact() const;
    void set_data(data_chunk data);
    data_chunk get_data() const;
    void set_hash(hash_digest hash);
    hash_digest get_hash() const;

    bool operator==(const big_number& other);
    bool operator!=(const big_number& other);
    bool operator<=(const big_number& other);
    bool operator>=(const big_number& other);
    bool operator<(const big_number& other);
    bool operator>(const big_number& other);

    big_number& operator*=(const big_number& other);
    big_number& operator/=(const big_number& other);

private:
    void set_uint64(uint64_t value);

    BIGNUM bignum_;
};

} // libbitcoin

#endif

