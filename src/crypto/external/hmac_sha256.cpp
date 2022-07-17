/* libsodium: hmac_hmacsha256.c, v0.4.5 2014/04/16 */
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
#include <bitcoin/system/crypto/external/hmac_sha256.hpp>

#include <bitcoin/system/crypto/sha256.hpp>
#include <bitcoin/system/crypto/sha256_context.hpp>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>

namespace libbitcoin {
namespace system {
namespace hmac_sha256 {

using namespace bc::system::sha256;

constexpr void xor_n(uint8_t* to, const uint8_t* from, size_t size) NOEXCEPT
{
    for (size_t i = 0; i < size; ++i)
        to[i] ^= from[i];
}

void hash(const uint8_t* data, size_t length, const uint8_t* key,
    size_t key_size, uint8_t* digest) NOEXCEPT
{
    hmac_sha256_context context;
    initialize(context, key, key_size);
    update(context, data, length);
    finalize(context, digest);
}

BC_PUSH_WARNING(NO_UNGUARDED_POINTERS)
void initialize(hmac_sha256_context& context, const uint8_t* key,
    size_t size) NOEXCEPT
BC_POP_WARNING()
{
    BC_PUSH_WARNING(LOCAL_VARIABLE_NOT_INITIALIZED)
    data_array<block_size> pad;
    data_array<digest_size> key_hash;
    BC_POP_WARNING()

    if (size > block_size)
    {
        context.in.write(size, key);
        context.in.flush(key_hash.data());
        key = key_hash.data();
        size = digest_size;
    }
    
    BC_PUSH_WARNING(NO_ARRAY_INDEXING)
    BC_PUSH_WARNING(NO_DYNAMIC_ARRAY_INDEXING)
    BC_PUSH_WARNING(NO_POINTER_ARITHMETIC)

    pad.fill(0x36_u8);
    xor_n(&pad[0], key, size);
    context.in.write(block_size, pad.data());
    pad.fill(0x5c_u8);
    xor_n(&pad[0], key, size);

    BC_POP_WARNING()
    BC_POP_WARNING()
    BC_POP_WARNING()

    context.out.write(block_size, pad.data());
}

void update(hmac_sha256_context& context, const uint8_t* data,
    size_t size) NOEXCEPT
{
    context.in.write(size, data);
}

void finalize(hmac_sha256_context& context, uint8_t* digest) NOEXCEPT
{
    context.in.flush(digest);
    context.out.write(digest_size, digest);
    context.out.flush(digest);
}

} // namespace hmac_sha256
} // namespace system
} // namespace libbitcoin
