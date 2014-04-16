/*	$OpenBSD: rmd160.h,v 1.5 2009/07/05 19:33:46 millert Exp $	*/
/*
 * Copyright (c) 2001 Markus Friedl.  All rights reserved.
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

#ifdef _WIN32
#include <stdint.h>
#endif

#include <boost/detail/endian.hpp>

// BYTE_ORDER and LITTLE_ENDIAN definitions required by ripemd160.
#define LITTLE_ENDIAN BOOST_ENDIAN_LITTLE_BYTE

#if BOOST_ENDIAN_BIG_BYTE
#   define BYTE_ORDER BOOST_ENDIAN_BIG_BYTE
#elif BOOST_ENDIAN_LITTLE_BYTE
#   define BYTE_ORDER BOOST_ENDIAN_LITTLE_BYTE
#elif BOOST_ENDIAN_LITTLE_WORD
    // TODO: compiler assert if not supported by ripemd160.c.
#   define BYTE_ORDER BOOST_ENDIAN_LITTLE_WORD
#elif BOOST_ENDIAN_BIG_WORD
    // TODO: compiler assert if not supported by ripemd160.c.
#   define BYTE_ORDER BOOST_ENDIAN_BIG_WORD
#endif

#define RMD160_BLOCK_LENGTH     64
#define RMD160_DIGEST_LENGTH    20

// count: number of bits, mod 2^64
typedef struct RMD160Context 
{
    uint32_t state[5];
    uint32_t count;
    uint8_t buffer[RMD160_BLOCK_LENGTH];
} RMD160_CTX;

void RMD160Init(RMD160_CTX*);
void RMD160Pad(RMD160_CTX* ctx);
void RMD160Transform(uint32_t[5], const uint8_t[RMD160_BLOCK_LENGTH]);
void RMD160Update(RMD160_CTX*, const uint8_t*, uint32_t);
void RMD160Final(uint8_t[RMD160_DIGEST_LENGTH], RMD160_CTX*);

#endif