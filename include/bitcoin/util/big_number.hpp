#ifndef LIBBITCOIN_BIG_NUMBER_H
#define LIBBITCOIN_BIG_NUMBER_H

#include <openssl/bn.h>

#include <bitcoin/types.hpp>
#include <bitcoin/util/logger.hpp>

namespace libbitcoin {

class big_number;
typedef std::pair<big_number, big_number> divmod_result;

// TODO: Lots of things *should* throw on error in this class
class big_number
{
public:
    big_number();
    big_number(const big_number& other);
    big_number(uint64_t value);
    ~big_number();

    big_number& operator=(const big_number& other);

    void set_compacted(uint32_t compact);
    uint32_t compacted() const;
    void set_data(data_chunk load_data);
    data_chunk data() const;
    void set_hash(hash_digest load_hash);
    hash_digest hash() const;
    void set_uint64(uint64_t value);
    uint64_t uint64() const;

    bool operator==(const big_number& other);
    bool operator!=(const big_number& other);
    bool operator<=(const big_number& other);
    bool operator>=(const big_number& other);
    bool operator<(const big_number& other);
    bool operator>(const big_number& other);

    big_number& operator+=(const big_number& other);
    big_number& operator*=(const big_number& other);
    big_number& operator/=(const big_number& other);

private:
    friend divmod_result divmod(const big_number& a, const big_number& b);
    friend const big_number operator+(
        const big_number& a, const big_number& b);
    friend const big_number operator/(const big_number& a,
        const big_number& b);
    friend const big_number operator<<(const big_number& a,
        unsigned int shift);

    void initialize();
    void copy(const big_number& other);

    BIGNUM bignum_;
};

const big_number operator+(const big_number& a, const big_number& b);
const big_number operator/(const big_number& a, const big_number& b);
const big_number operator<<(const big_number& a, unsigned int shift);

divmod_result divmod(const big_number& a, const big_number& b);

} // libbitcoin

#endif

