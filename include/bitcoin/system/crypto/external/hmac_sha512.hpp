/* libsodium: crypto_auth_hmacsha512.h, v0.4.5 2014/04/16 */
/**
 * Copyright 2005,2007,2009 Colin Percival. All rights reserved.
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
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */
#ifndef LIBBITCOIN_SYSTEM_CRYPTO_EXTERNAL_HMAC_SHA512_HPP
#define LIBBITCOIN_SYSTEM_CRYPTO_EXTERNAL_HMAC_SHA512_HPP

#include <bitcoin/system/define.hpp>
#include <bitcoin/system/crypto/accumulator.hpp>
#include <bitcoin/system/crypto/sha_algorithm.hpp>

namespace libbitcoin {
namespace system {
namespace hmac {
namespace sha512 {
    
using algorithm = sha::algorithm<sha::sha512>;
struct context
{
    system::accumulator<algorithm> in{};
    system::accumulator<algorithm> out{};
};

/// Single hash.
void hash(const uint8_t* data, size_t size, const uint8_t* key,
    size_t key_size, uint8_t* digest) NOEXCEPT;

// Streaming hash.
void initialize(context& context, const uint8_t* key, size_t size) NOEXCEPT;
void update(context& context, const uint8_t* data, size_t size) NOEXCEPT;
void finalize(context& context, uint8_t* digest) NOEXCEPT;

} // namespace sha512
} // namespace hmac
} // namespace system
} // namespace libbitcoin

#endif
