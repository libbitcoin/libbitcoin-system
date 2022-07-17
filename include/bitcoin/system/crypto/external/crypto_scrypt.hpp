/**
 * Copyright 2009 Colin Percival
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
 *
 * This file was originally written by Colin Percival as part of the Tarsnap
 * online backup system.
 */
#ifndef LIBBITCOIN_SYSTEM_CRYPTO_SCRYPT_HPP
#define LIBBITCOIN_SYSTEM_CRYPTO_SCRYPT_HPP

#include <bitcoin/system/define.hpp>
#include <bitcoin/system/data/data.hpp>

namespace libbitcoin {
namespace system {
namespace scrypt {

/// [W]ork must be a power of 2 greater than 1.
/// [R]esources and [P]arallelism must satisfy [(R * P) < 2^30].
/// buffer size must not exceed [sub1(2^32) * 2^5].
/// Memory required in bytes = [add1(W + P) * R * 2^7].
/// If scrypt::hash returns false then out will remain zeroized.
/// This can only be caused by out-of-memory or invalid parameterization.
bool hash(size_t W, size_t R, size_t P, 
    const uint8_t* phrase, size_t phrase_size, const uint8_t* salt,
    size_t salt_size, uint8_t* buffer, size_t size) NOEXCEPT;

// TODO: Add type contraints based on below limits (also in implementation).
// TODO: This just requires moving from .cpp to .ipp source implementation.
template<size_t Size, size_t W, size_t R, size_t P>
inline data_array<Size> hash(const data_slice& phrase,
    const data_slice& salt) NOEXCEPT
{
    data_array<Size> hash{};
    scrypt::hash(W, R, P, phrase.data(), phrase.size(), salt.data(),
        salt.size(), hash.data(), Size);
    return hash;
}

} // namespace scrypt
} // namespace system
} // namespace libbitcoin

#endif
