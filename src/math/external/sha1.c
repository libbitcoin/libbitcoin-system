/**
 *  sha1.c
 *
 *  Description:
 *      This file implements the Secure Hashing Algorithm 1 as
 *      defined in FIPS PUB 180-1 published April 17, 1995.
 *
 *      The SHA-1, produces a 160-bit message digest for a given
 *      data stream.  It should take about 2**n steps to find a
 *      message with the same digest as a given message and
 *      2**(n/2) to find any two messages with the same digest,
 *      when n is the digest size in bits.  Therefore, this
 *      algorithm can serve as a means of providing a
 *      "fingerprint" for a message.
 *
 *  Portability Issues:
 *      SHA-1 is defined in terms of 32-bit "words".  This code
 *      uses <stdint.h> (included via "sha1.h" to define 32 and 8
 *      bit unsigned integer types.  If your C compiler does not
 *      support 32 bit unsigned integers, this code is not
 *      appropriate.
 *
 *  Caveats:
 *      SHA-1 is designed to work with messages less than 2^64 bits
 *      long.  Although SHA-1 allows a message digest to be generated
 *      for messages of any number of bits less than 2^64, this
 *      implementation only works with messages with a length that is
 *      a multiple of the size of an 8-bit character.
 */
#include "sha1.h"

#include <stdint.h>
#include <stddef.h>

enum
{
    shaSuccess = 0,
    shaNull,
    shaInputTooLong,
    shaStateError
};

#define SHIFT(bits,word) \
    (((word) << (bits)) | ((word) >> (32 - (bits))))

void SHA1PadMessage(SHA1CTX* context);
void SHA1ProcessMessageBlock(SHA1CTX* context);

int SHA1Reset(SHA1CTX* context)
{
    if (!context)
    {
        return shaNull;
    }

    context->length_low = 0;
    context->length_high = 0;
    context->index  = 0;
    context->state[0] = 0x67452301;
    context->state[1] = 0xEFCDAB89;
    context->state[2] = 0x98BADCFE;
    context->state[3] = 0x10325476;
    context->state[4] = 0xC3D2E1F0;
    context->computed = 0;
    context->corrupted = 0;

    return shaSuccess;
}

int SHA1Input(SHA1CTX* context, const uint8_t* message, size_t length)
{
    if (!length)
    {
        return shaSuccess;
    }

    if (!context || !message)
    {
        return shaNull;
    }

    if (context->computed)
    {
        context->corrupted = shaStateError;
        return shaStateError;
    }

    if (context->corrupted)
    {
         return context->corrupted;
    }

    while (length-- && !context->corrupted)
    {
        context->block[context->index++] = (*message & 0xFF);
        context->length_low += 8;

        if (context->length_low == 0)
        {
            context->length_high++;

            if (context->length_high == 0)
            {
                context->corrupted = 1;
            }
        }

        if (context->index == 64)
        {
            SHA1ProcessMessageBlock(context);
        }

        message++;
    }

    return shaSuccess;
}

int SHA1Result(SHA1CTX* context, uint8_t digest[SHA1_DIGEST_LENGTH])
{
    int i;

    if (!context || !digest)
    {
        return shaNull;
    }

    if (context->corrupted)
    {
        return context->corrupted;
    }

    if (!context->computed)
    {
        SHA1PadMessage(context);

        for (i = 0; i < SHA1_BLOCK_LENGTH; ++i)
        {
            context->block[i] = 0;
        }

        context->length_low = 0;
        context->length_high = 0;
        context->computed = 1;
    }

    for (i = 0; i < SHA1_DIGEST_LENGTH; ++i)
    {
        digest[i] = context->state[i >> 2] >> 8 * (3 - (i & 0x03));
    }

    return shaSuccess;
}

void SHA1_(const uint8_t* message, size_t length,
    uint8_t digest[SHA1_DIGEST_LENGTH])
{
    SHA1CTX context;
    SHA1Reset(&context);
    SHA1Input(&context, message, length);
    SHA1Result(&context, digest);
}

void SHA1ProcessMessageBlock(SHA1CTX* context)
{
    const uint32_t K[] =
    {
        0x5A827999,
        0x6ED9EBA1,
        0x8F1BBCDC,
        0xCA62C1D6
    };

    int t; 
    uint32_t temp;
    uint32_t W[80];
    uint32_t A, B, C, D, E;

    for (t = 0; t < 16; t++)
    {
        W[t] = context->block[t * 4] << 24;
        W[t] |= context->block[t * 4 + 1] << 16;
        W[t] |= context->block[t * 4 + 2] << 8;
        W[t] |= context->block[t * 4 + 3];
    }

    for (t = 16; t < 80; t++)
    {
       W[t] = SHIFT(1, W[t-3] ^ W[t-8] ^ W[t-14] ^ W[t-16]);
    }

    A = context->state[0];
    B = context->state[1];
    C = context->state[2];
    D = context->state[3];
    E = context->state[4];

    for (t = 0; t < 20; t++)
    {
        temp = SHIFT(5, A) + ((B & C) | ((~B) & D)) + E + W[t] + K[0];
        E = D;
        D = C;
        C = SHIFT(30, B);

        B = A;
        A = temp;
    }

    for (t = 20; t < 40; t++)
    {
        temp = SHIFT(5, A) + (B ^ C ^ D) + E + W[t] + K[1];
        E = D;
        D = C;
        C = SHIFT(30, B);
        B = A;
        A = temp;
    }

    for (t = 40; t < 60; t++)
    {
        temp = SHIFT(5, A) + ((B & C) | (B & D) | (C & D)) + E + W[t] + K[2];
        E = D;
        D = C;
        C = SHIFT(30, B);
        B = A;
        A = temp;
    }

    for (t = 60; t < 80; t++)
    {
        temp = SHIFT(5, A) + (B ^ C ^ D) + E + W[t] + K[3];
        E = D;
        D = C;
        C = SHIFT(30, B);
        B = A;
        A = temp;
    }

    context->state[0] += A;
    context->state[1] += B;
    context->state[2] += C;
    context->state[3] += D;
    context->state[4] += E;
    context->index = 0;
}

void SHA1PadMessage(SHA1CTX* context)
{
    if (context->index > 55)
    {
        context->block [context->index++] = 0x80;

        while (context->index < 64)
        {
            context->block[context->index++] = 0;
        }

        SHA1ProcessMessageBlock(context);

        while (context->index < 56)
        {
            context->block[context->index++] = 0;
        }
    }
    else
    {
        context->block[context->index++] = 0x80;

        while (context->index < 56)
        {
            context->block[context->index++] = 0;
        }
    }

    context->block[56] = context->length_high >> 24;
    context->block[57] = context->length_high >> 16;
    context->block[58] = context->length_high >> 8;
    context->block[59] = context->length_high >> 0;

    context->block[60] = context->length_low >> 24;
    context->block[61] = context->length_low >> 16;
    context->block[62] = context->length_low >> 8;
    context->block[63] = context->length_low >> 0;

    SHA1ProcessMessageBlock(context);
}
