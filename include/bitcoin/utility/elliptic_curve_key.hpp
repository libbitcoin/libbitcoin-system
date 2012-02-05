#ifndef LIBBITCOIN_ELLIPTIC_CURVE_KEY_H
#define LIBBITCOIN_ELLIPTIC_CURVE_KEY_H

#include <openssl/ec.h>
#include <stdexcept>

#include <bitcoin/types.hpp>

namespace libbitcoin {

typedef data_chunk private_data;

class elliptic_curve_key
{
public:
    elliptic_curve_key();
    ~elliptic_curve_key();

    elliptic_curve_key(const elliptic_curve_key& other);
    elliptic_curve_key& operator=(const elliptic_curve_key& other);

    bool set_public_key(const data_chunk& pubkey);
    data_chunk public_key() const;
    bool verify(hash_digest hash, const data_chunk& signature);

    bool new_key_pair();
    bool set_private_key(const private_data& privkey);
    private_data private_key() const;
    data_chunk sign(hash_digest hash) const;

private:
    bool initialize();

    EC_KEY* key_;
};

} // libbitcoin

#endif

