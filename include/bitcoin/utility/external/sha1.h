/* OpenBSD: sha1.h, v1.5 2007/09/10 */
/*
 * SHA-1 in C
 * By Steve Reid <steve@edmweb.com>
 * 100% Public Domain
 */
#ifndef LIBBITCOIN_SHA1_H
#define LIBBITCOIN_SHA1_H

#include <stdint.h>
#include <boost/detail/endian.hpp>

// TODO: this needs work.
// BYTE_ORDER and LITTLE_ENDIAN definitions required by ripemd160.
#define LITTLE_ENDIAN BOOST_ENDIAN_LITTLE_BYTE

#if BOOST_ENDIAN_BIG_BYTE
#   define BYTE_ORDER BOOST_ENDIAN_BIG_BYTE
#elif BOOST_ENDIAN_LITTLE_BYTE
#   define BYTE_ORDER BOOST_ENDIAN_LITTLE_BYTE
#elif BOOST_ENDIAN_LITTLE_WORD
#   error BOOST_ENDIAN_LITTLE_WORD byte order is not supported.
#elif BOOST_ENDIAN_BIG_WORD
#   error BOOST_ENDIAN_BIG_WORD byte order is not supported.
#else
#   error BOOST_ENDIAN symbol is required.
#endif

#define SHA1_BLOCK_LENGTH  64U
#define SHA1_DIGEST_LENGTH 20U

#ifdef __cplusplus
extern "C" {
#endif

typedef struct SHA1CTX
{
    uint32_t state[5];
    uint64_t count;
    uint8_t buffer[SHA1_BLOCK_LENGTH];
} SHA1CTX;

void SHA1(const uint8_t* input, uint32_t length,
    uint8_t digest[SHA1_DIGEST_LENGTH]);
void SHA1Final(SHA1CTX* context, uint8_t digest[SHA1_DIGEST_LENGTH]);
void SHA1Init(SHA1CTX* context);
void SHA1Pad(SHA1CTX* context);
void SHA1Transform(uint32_t state[5], const uint8_t block[SHA1_BLOCK_LENGTH]);
void SHA1Update(SHA1CTX* context, const uint8_t* input, uint32_t length);

#ifdef __cplusplus
}
#endif

#endif