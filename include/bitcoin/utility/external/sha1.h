/* This code is public-domain - it is based on libcrypt
 * placed in the public domain by Wei Dai and other contributors.
 */
#ifndef LIBBITCOIN_SHA1_H
#define LIBBITCOIN_SHA1_H

#include <stdint.h>
#include <string.h>

#ifdef __cplusplus
extern "C" 
{
#endif

/* header */

#define HASH_LENGTH 20
#define BLOCK_LENGTH 64

typedef struct sha1nfo {
    uint32_t buffer[BLOCK_LENGTH/4];
    uint32_t state[HASH_LENGTH/4];
    uint32_t byteCount;
    uint8_t bufferOffset;
    uint8_t keyBuffer[BLOCK_LENGTH];
    uint8_t innerHash[HASH_LENGTH];
} sha1nfo;

void sha1_init(sha1nfo *s);
void sha1_write(sha1nfo *s, const char *data, size_t len);
uint8_t* sha1_result(sha1nfo *s);

#ifdef __cplusplus
}
#endif

#endif
