/* OpenBSD: rmd160.h, v1.5 2009/07/05 */
/*
 * Copyright (c) 2001 Markus Friedl. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#ifndef LIBBITCOIN_RIPEMD160_H
#define LIBBITCOIN_RIPEMD160_H

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

#define RMD160_BLOCK_LENGTH  64U
#define RMD160_DIGEST_LENGTH 20U

#ifdef __cplusplus
extern "C" {
#endif

typedef struct RMD160CTX
{
    uint32_t state[5];
    uint64_t count;
    uint8_t buffer[RMD160_BLOCK_LENGTH];
} RMD160CTX;

void RMD160(const uint8_t* input, uint32_t length, 
    uint8_t digest[RMD160_DIGEST_LENGTH]);
void RMD160Final(RMD160CTX* context, uint8_t digest[RMD160_DIGEST_LENGTH]);
void RMD160Init(RMD160CTX* context);
void RMD160Pad(RMD160CTX* context);
void RMD160Transform(uint32_t state[5], 
    const uint8_t block[RMD160_BLOCK_LENGTH]);
void RMD160Update(RMD160CTX* context, const uint8_t* input, uint32_t length);

#ifdef __cplusplus
}
#endif

#endif