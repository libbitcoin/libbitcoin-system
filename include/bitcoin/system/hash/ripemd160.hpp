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
namespace ripemd160 {

constexpr size_t state_size = 5;
constexpr size_t chunk_size = 16;
constexpr size_t block_size = 64;
constexpr size_t digest_size = 20;

struct context
{
    uint32_t state[state_size];
    uint32_t chunk[chunk_size];
};

inline void hash(const uint8_t* message, size_t length, uint8_t* digest) NOEXCEPT;

inline void initialize(context& context) NOEXCEPT;
inline void update(context& context, const uint8_t* message, size_t length) NOEXCEPT;
inline void finalize(context& context, uint8_t* digest) NOEXCEPT;

} // namespace ripemd160
} // namespace system
} // namespace libbitcoin

#include <bitcoin/system/impl/hash/ripemd160.ipp>

#endif
