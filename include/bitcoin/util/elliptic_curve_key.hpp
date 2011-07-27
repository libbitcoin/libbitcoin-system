#ifndef LIBBITCOIN_ELLIPTIC_CURVE_KEY_H
#define LIBBITCOIN_ELLIPTIC_CURVE_KEY_H

#include <openssl/ec.h>
#include <stdexcept>

#include <bitcoin/types.hpp>

namespace libbitcoin {

class elliptic_curve_key
{
public:
    elliptic_curve_key();

    bool set_public_key(data_chunk pubkey);
    bool verify(hash_digest hash, data_chunk signature);

//private:
    bool initialize();

    EC_KEY* key_;
};

} // libbitcoin

#endif

