/**
 * Copyright 2005,2007,2009 Colin Percival
 * All rights reserved.
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
#include <bitcoin/system/crypto/external/pbkdf2_sha256.hpp>

#include <bitcoin/system/crypto/external/hmac_sha256.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/endian/endian.hpp>
#include <bitcoin/system/math/math.hpp>

namespace libbitcoin {
namespace system {
namespace pbkdf2 {

using namespace sha256;
namespace hmac = hmac_sha256;

BC_PUSH_WARNING(NO_UNGUARDED_POINTERS)
void sha256(const uint8_t* passphrase, size_t passphrase_size,
    const uint8_t* salt, size_t salt_size, uint64_t interations,
    uint8_t* buffer, size_t buffer_size) NOEXCEPT
BC_POP_WARNING()
{
    hmac::hmac_sha256_context salted;
    hmac::initialize(salted, passphrase, passphrase_size);
    hmac::update(salted, salt, salt_size);
    
    BC_PUSH_WARNING(LOCAL_VARIABLE_NOT_INITIALIZED)
    digest U;
    BC_POP_WARNING()

    for (uint32_t i = 0; i * digest_size < buffer_size; ++i)
    {
        constexpr auto size = sizeof(uint32_t);

        auto context = salted;
        hmac::update(context, to_big_endian(add1(i)).data(), size);
        hmac::finalize(context, U.data());
        auto T = U;

        for (size_t j = one; j < interations; ++j)
        {
            hmac::initialize(context, passphrase, passphrase_size);
            hmac::update(context, U.data(), digest_size);
            hmac::finalize(context, U.data());
            
            BC_PUSH_WARNING(NO_ARRAY_INDEXING)
            BC_PUSH_WARNING(NO_DYNAMIC_ARRAY_INDEXING)
            for (size_t k = 0; k < digest_size; ++k)
                T[k] ^= U[k];
            BC_POP_WARNING()
            BC_POP_WARNING()
        }

        const auto offset = i * digest_size;
        const auto length = limit(buffer_size - offset, digest_size);

        BC_PUSH_WARNING(NO_ARRAY_INDEXING)
        BC_PUSH_WARNING(NO_DYNAMIC_ARRAY_INDEXING)
        BC_PUSH_WARNING(NO_POINTER_ARITHMETIC)
        std::memcpy(&buffer[offset], T.data(), length);
        BC_POP_WARNING()
        BC_POP_WARNING()
        BC_POP_WARNING()
    }
}

} // namespace sha256
} // namespace system
} // namespace libbitcoin
