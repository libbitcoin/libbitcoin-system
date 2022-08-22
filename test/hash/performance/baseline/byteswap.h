// Copyright (c) 2014-2019 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_COMPAT_BYTESWAP_H
#define BITCOIN_COMPAT_BYTESWAP_H

#include <string.h>
#include <stdint.h>
#include "../../../test.hpp"

BC_PUSH_WARNING(USE_CONSTEXPR_FOR_FUNCTION)
BC_PUSH_WARNING(USE_NOEXCEPT_FOR_FUNCTION)

namespace baseline {

// Give baseline benefit of intrinsics.
// native implementation is equivalent.
#ifndef bswap_16
#define bswap_16 byte_swap16
#endif
#ifndef bswap_32
#define bswap_32 byte_swap32
#endif
#ifndef bswap_64
#define bswap_64 byte_swap64
#endif
    
////inline uint16_t bswap_16(uint16_t x)
////{
////    return (x >> 8) | (x << 8);
////}
////
////inline uint32_t bswap_32(uint32_t x)
////{
////    return (((x & 0xff000000U) >> 24) | ((x & 0x00ff0000U) >>  8) |
////            ((x & 0x0000ff00U) <<  8) | ((x & 0x000000ffU) << 24));
////}
////
////inline uint64_t bswap_64(uint64_t x)
////{
////     return (((x & 0xff00000000000000ull) >> 56)
////          | ((x & 0x00ff000000000000ull) >> 40)
////          | ((x & 0x0000ff0000000000ull) >> 24)
////          | ((x & 0x000000ff00000000ull) >> 8)
////          | ((x & 0x00000000ff000000ull) << 8)
////          | ((x & 0x0000000000ff0000ull) << 24)
////          | ((x & 0x000000000000ff00ull) << 40)
////          | ((x & 0x00000000000000ffull) << 56));
////}

} // baseline

BC_POP_WARNING()
BC_POP_WARNING()

#endif // BITCOIN_COMPAT_BYTESWAP_H
