/* OpenBSD: sha1.c, v 1.23 2014/01/08 */
/**
* SHA-1 in C
* By Steve Reid <steve@edmweb.com>
* 100% Public Domain
*
* Test Vectors (from FIPS PUB 180-1)
* "abc"
*   A9993E36 4706816A BA3E2571 7850C26C 9CD0D89D
* "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq"
*   84983E44 1C3BD26E BAAE4AA1 F95129E5 E54670F1
* A million repetitions of "a"
*   34AA973C D4C4DAA4 F61EEB2B DBAD2731 6534016F
*/
#include "sha1.h"

#include <stdint.h>
#include <string.h>
#include <bitcoin/bitcoin/compat.h>
#include "zeroize.h"

#ifdef __BIG_ENDIAN__
    #define SHA1_BIG_ENDIAN
#elif defined __LITTLE_ENDIAN__
    /* override */
#elif defined __BYTE_ORDER
    #if __BYTE_ORDER__ ==  __ORDER_BIG_ENDIAN__
        #define SHA1_BIG_ENDIAN
    #endif
#else /* !defined __LITTLE_ENDIAN__ */
    #include <endian.h> /* machine/endian.h */
    #if __BYTE_ORDER__ ==  __ORDER_BIG_ENDIAN__
        #define SHA1_BIG_ENDIAN
    #endif
#endif

#define rol(value, bits) (((value) << (bits)) | ((value) >> (32 - (bits))))

#ifdef SHA1_BIG_ENDIAN
#define blk0(b, i) b->l[i]
#else
#define blk0(b, i) (b->l[i] = \
    (rol(b->l[i], 24) & 0xFF00FF00) | \
    (rol(b->l[i], 8) & 0x00FF00FF))
#endif

#define blk(b, i) (b->l[i & 15] = \
    rol(b->l[(i + 13) & 15]^ \
    b->l[(i + 8) & 15]^ \
    b->l[(i + 2) & 15]^ \
    b->l[i & 15], 1))

#define R0(b, v, w, x, y, z, i) \
    z += ((w & (x ^ y)) ^ y) + blk0(b, i) + 0x5A827999 + rol(v, 5); \
    w = rol(w, 30);

#define R1(b, v, w, x, y, z, i) \
    z += ((w & (x ^ y)) ^ y) + blk(b, i) + 0x5A827999 + rol(v, 5); \
    w = rol(w, 30);

#define R2(b, v, w, x, y, z, i) \
    z += (w ^ x ^ y) + blk(b, i) + 0x6ED9EBA1 + rol(v, 5); \
    w = rol(w, 30);

#define R3(b, v, w, x, y, z, i) \
    z += (((w | x) & y) | (w & x)) + blk(b, i) + 0x8F1BBCDC + rol(v, 5); \
    w = rol(w, 30);

#define R4(b, v, w, x, y, z, i) \
    z += (w ^ x ^ y) + blk(b, i)+ 0xCA62C1D6 + rol(v, 5); \
    w = rol(w, 30);

typedef union {
    uint8_t c[64];
    uint32_t l[16];
} CHAR64LONG16;

void SHA1_(const uint8_t* input, size_t length,
    uint8_t digest[SHA1_DIGEST_LENGTH])
{
    SHA1CTX context;
    SHA1Init(&context);
    SHA1Update(&context, input, length);
    SHA1Final(&context, digest);
}

void SHA1Final(SHA1CTX* context, uint8_t digest[SHA1_DIGEST_LENGTH])
{
    uint8_t i;
    SHA1Pad(context);
    for (i = 0; i < SHA1_DIGEST_LENGTH; i++)
    {
        digest[i] = (uint8_t)
            ((context->state[i >> 2] >> ((3 - (i & 3)) * 8)) & 255);
    }

    zeroize(context, sizeof *context);
}

void SHA1Init(SHA1CTX* context)
{
    context->count = 0;

    context->state[0] = 0x67452301;
    context->state[1] = 0xEFCDAB89;
    context->state[2] = 0x98BADCFE;
    context->state[3] = 0x10325476;
    context->state[4] = 0xC3D2E1F0;
}

void SHA1Pad(SHA1CTX* context)
{
    uint8_t i;
    uint8_t len[8];

    for (i = 0; i < 8; i++)
    {
        len[i] = (uint8_t)
            ((context->count >> ((7 - (i & 7)) * 8)) & 255);
    }

    SHA1Update(context, (uint8_t*)"\200", 1);

    while ((context->count & 504) != 448)
    {
        SHA1Update(context, (uint8_t*)"\0", 1);
    }

    SHA1Update(context, len, 8);
}

void SHA1Transform(uint32_t state[SHA1_STATE_LENGTH],
    const uint8_t block[SHA1_BLOCK_LENGTH])
{
    uint32_t a, b, c, d, e;
    uint8_t workspace[SHA1_BLOCK_LENGTH];
    CHAR64LONG16* w = (CHAR64LONG16*)workspace;

    memcpy(w, block, SHA1_BLOCK_LENGTH);

    a = state[0];
    b = state[1];
    c = state[2];
    d = state[3];
    e = state[4];

    R0(w, a, b, c, d, e, 0);
    R0(w, e, a, b, c, d, 1);
    R0(w, d, e, a, b, c, 2);
    R0(w, c, d, e, a, b, 3);
    R0(w, b, c, d, e, a, 4);
    R0(w, a, b, c, d, e, 5);
    R0(w, e, a, b, c, d, 6);
    R0(w, d, e, a, b, c, 7);
    R0(w, c, d, e, a, b, 8);
    R0(w, b, c, d, e, a, 9);
    R0(w, a, b, c, d, e, 10);
    R0(w, e, a, b, c, d, 11);
    R0(w, d, e, a, b, c, 12);
    R0(w, c, d, e, a, b, 13);
    R0(w, b, c, d, e, a, 14);
    R0(w, a, b, c, d, e, 15);
    R1(w, e, a, b, c, d, 16);
    R1(w, d, e, a, b, c, 17);
    R1(w, c, d, e, a, b, 18);
    R1(w, b, c, d, e, a, 19);
    R2(w, a, b, c, d, e, 20);
    R2(w, e, a, b, c, d, 21);
    R2(w, d, e, a, b, c, 22);
    R2(w, c, d, e, a, b, 23);
    R2(w, b, c, d, e, a, 24);
    R2(w, a, b, c, d, e, 25);
    R2(w, e, a, b, c, d, 26);
    R2(w, d, e, a, b, c, 27);
    R2(w, c, d, e, a, b, 28);
    R2(w, b, c, d, e, a, 29);
    R2(w, a, b, c, d, e, 30);
    R2(w, e, a, b, c, d, 31);
    R2(w, d, e, a, b, c, 32);
    R2(w, c, d, e, a, b, 33);
    R2(w, b, c, d, e, a, 34);
    R2(w, a, b, c, d, e, 35);
    R2(w, e, a, b, c, d, 36);
    R2(w, d, e, a, b, c, 37);
    R2(w, c, d, e, a, b, 38);
    R2(w, b, c, d, e, a, 39);
    R3(w, a, b, c, d, e, 40);
    R3(w, e, a, b, c, d, 41);
    R3(w, d, e, a, b, c, 42);
    R3(w, c, d, e, a, b, 43);
    R3(w, b, c, d, e, a, 44);
    R3(w, a, b, c, d, e, 45);
    R3(w, e, a, b, c, d, 46);
    R3(w, d, e, a, b, c, 47);
    R3(w, c, d, e, a, b, 48);
    R3(w, b, c, d, e, a, 49);
    R3(w, a, b, c, d, e, 50);
    R3(w, e, a, b, c, d, 51);
    R3(w, d, e, a, b, c, 52);
    R3(w, c, d, e, a, b, 53);
    R3(w, b, c, d, e, a, 54);
    R3(w, a, b, c, d, e, 55);
    R3(w, e, a, b, c, d, 56);
    R3(w, d, e, a, b, c, 57);
    R3(w, c, d, e, a, b, 58);
    R3(w, b, c, d, e, a, 59);
    R4(w, a, b, c, d, e, 60);
    R4(w, e, a, b, c, d, 61);
    R4(w, d, e, a, b, c, 62);
    R4(w, c, d, e, a, b, 63);
    R4(w, b, c, d, e, a, 64);
    R4(w, a, b, c, d, e, 65);
    R4(w, e, a, b, c, d, 66);
    R4(w, d, e, a, b, c, 67);
    R4(w, c, d, e, a, b, 68);
    R4(w, b, c, d, e, a, 69);
    R4(w, a, b, c, d, e, 70);
    R4(w, e, a, b, c, d, 71);
    R4(w, d, e, a, b, c, 72);
    R4(w, c, d, e, a, b, 73);
    R4(w, b, c, d, e, a, 74);
    R4(w, a, b, c, d, e, 75);
    R4(w, e, a, b, c, d, 76);
    R4(w, d, e, a, b, c, 77);
    R4(w, c, d, e, a, b, 78);
    R4(w, b, c, d, e, a, 79);

    state[0] += a;
    state[1] += b;
    state[2] += c;
    state[3] += d;
    state[4] += e;

    a = b = c = d = e = 0;
}

void SHA1Update(SHA1CTX* context, const uint8_t* input, size_t length)
{
    uint32_t i = 0;
    uint32_t j = (uint32_t)((context->count >> 3) & 63);

    context->count += (length << 3);

    if ((j + length) > 63)
    {
        memcpy(&context->buffer[j], input, (i = 64 - j));
        SHA1Transform(context->state, context->buffer);

        for (; i + 63 < length; i += 64)
            SHA1Transform(context->state, (uint8_t*)&input[i]);

        j = 0;
    }

    memcpy(&context->buffer[j], &input[i], length - i);
}
