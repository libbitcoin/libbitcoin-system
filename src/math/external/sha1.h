/* OpenBSD: sha1.h, v1.5 2007/09/10 */
/**
* SHA-1 in C
* By Steve Reid <steve@edmweb.com>
* 100% Public Domain
*/
#ifndef LIBBITCOIN_SHA1_H
#define LIBBITCOIN_SHA1_H

#include <stdint.h>
#include <stddef.h>

#define SHA1_STATE_LENGTH 5U
#define SHA1_BLOCK_LENGTH 64U
#define SHA1_DIGEST_LENGTH 20U

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct SHA1CTX
{
    uint32_t state[SHA1_STATE_LENGTH];
    uint64_t count;
    uint8_t buffer[SHA1_BLOCK_LENGTH];
} SHA1CTX;

void SHA1_(const uint8_t* input, size_t length,
    uint8_t digest[SHA1_DIGEST_LENGTH]);

void SHA1Final(SHA1CTX* context, uint8_t digest[SHA1_DIGEST_LENGTH]);

void SHA1Init(SHA1CTX* context);

void SHA1Pad(SHA1CTX* context);

void SHA1Transform(uint32_t state[SHA1_STATE_LENGTH],
    const uint8_t block[SHA1_BLOCK_LENGTH]);

void SHA1Update(SHA1CTX* context, const uint8_t* input, size_t length);

#ifdef __cplusplus
}
#endif

#endif
