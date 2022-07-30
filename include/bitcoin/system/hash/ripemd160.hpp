/********************************************************************\
 *  FILE:     rmd160.c
 *  CONTENTS: A sample C-implementation of the RIPEMD-160 hash-function.
 *  TARGET:   any computer with an ANSI C compiler
 *  AUTHOR:   Antoon Bosselaers, Dept. Electrical Eng.-ESAT/COSIC
 *  DATE:     1 March 1996       VERSION:  1.0
  * ADAPTED:  by Libbitcoin Developers on 7 September 2016
\********************************************************************/
#ifndef LIBBITCOIN_SYSTEM_HASH_RIPEMD160_HPP
#define LIBBITCOIN_SYSTEM_HASH_RIPEMD160_HPP

#include <bitcoin/system/define.hpp>

namespace libbitcoin {
namespace system {

#define RMD160_STATE_LENGTH 5U
#define RMD160_CHUNK_LENGTH 16U
#define RMD160_BLOCK_LENGTH 64U
#define RMD160_DIGEST_LENGTH 20U

typedef struct RMD160CTX
{
    uint32_t state[RMD160_STATE_LENGTH];
    uint32_t chunk[RMD160_CHUNK_LENGTH];
} RMD160CTX;

void RMD160(const uint8_t* message, size_t length,
    uint8_t digest[RMD160_DIGEST_LENGTH]) NOEXCEPT;

void RMDInit(RMD160CTX* context) NOEXCEPT;
void RMDUpdate(RMD160CTX* context, const uint8_t* message, size_t length) NOEXCEPT;
void RMDFinal(RMD160CTX* context, uint8_t digest[RMD160_DIGEST_LENGTH]) NOEXCEPT;

} // namespace system
} // namespace libbitcoin

#endif
