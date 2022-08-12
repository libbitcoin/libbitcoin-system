// Copyright (c) 2014-2018 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_COMPAT_ENDIAN_H
#define BITCOIN_COMPAT_ENDIAN_H

#include "byteswap.h"
#include <stdint.h>
#include "../../../test.hpp"

BC_PUSH_WARNING(USE_CONSTEXPR_FOR_FUNCTION)
BC_PUSH_WARNING(USE_NOEXCEPT_FOR_FUNCTION)

namespace baseline {

inline uint16_t htobe16_(uint16_t host_16bits)
{
    if constexpr (is_big_endian)
        return host_16bits;
    else
        return bswap_16(host_16bits);
}

inline uint16_t htole16_(uint16_t host_16bits)
{
    if constexpr (is_big_endian)
        return bswap_16(host_16bits);
    else
        return host_16bits;
}

inline uint16_t be16toh_(uint16_t big_endian_16bits)
{
    if constexpr (is_big_endian)
        return big_endian_16bits;
    else
        return bswap_16(big_endian_16bits);
}

inline uint16_t le16toh_(uint16_t little_endian_16bits)
{
    if constexpr (is_big_endian)
        return bswap_16(little_endian_16bits);
    else
        return little_endian_16bits;
}

inline uint32_t htobe32_(uint32_t host_32bits)
{
    if constexpr (is_big_endian)
        return host_32bits;
    else
        return bswap_32(host_32bits);
}

inline uint32_t htole32_(uint32_t host_32bits)
{
    if constexpr (is_big_endian)
        return bswap_32(host_32bits);
    else
        return host_32bits;
}

inline uint32_t be32toh_(uint32_t big_endian_32bits)
{
    if constexpr (is_big_endian)
        return big_endian_32bits;
    else
        return bswap_32(big_endian_32bits);
}

inline uint32_t le32toh_(uint32_t little_endian_32bits)
{
    if constexpr (is_big_endian)
        return bswap_32(little_endian_32bits);
    else
        return little_endian_32bits;
}

inline uint64_t htobe64_(uint64_t host_64bits)
{
    if constexpr (is_big_endian)
        return host_64bits;
    else
        return bswap_64(host_64bits);
}

inline uint64_t htole64_(uint64_t host_64bits)
{
    if constexpr (is_big_endian)
        return bswap_64(host_64bits);
    else
        return host_64bits;
}

inline uint64_t be64toh_(uint64_t big_endian_64bits)
{
    if constexpr (is_big_endian)
        return big_endian_64bits;
    else
        return bswap_64(big_endian_64bits);
}

inline uint64_t le64toh_(uint64_t little_endian_64bits)
{
    if constexpr (is_big_endian)
        return bswap_64(little_endian_64bits);
    else
        return little_endian_64bits;
}

} // baseline

BC_POP_WARNING()
BC_POP_WARNING()

#endif // BITCOIN_COMPAT_ENDIAN_H
