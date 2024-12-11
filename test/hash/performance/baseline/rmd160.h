// Copyright (c) 2014-2016 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_CRYPTO_RIPEMD160_H
#define BITCOIN_CRYPTO_RIPEMD160_H

#include <stdint.h>
#include <stdlib.h>
#include "../../../test.hpp"

namespace baseline {

class CRIPEMD160
{
private:
    uint32_t s[5];
    unsigned char buf[64];
    uint64_t bytes;

public:
    static const size_t digest_bytes = 20;

    CRIPEMD160();
    CRIPEMD160& Write(const unsigned char* data, size_t len);
    void Finalize(unsigned char hash[digest_bytes]);
    CRIPEMD160& Reset();
};

} // namespace baseline

#endif // BITCOIN_CRYPTO_RIPEMD160_H
