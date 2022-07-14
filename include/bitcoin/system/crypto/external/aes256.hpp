/**
 *   Byte-oriented AES-256 implementation.
 *   All lookup tables replaced with 'on the fly' calculations. 
 *
 *   Copyright (c) 2007-2009 Ilya O. Levin, http://www.literatecode.com
 *   Other contributors: Hal Finney
 *
 *   Permission to use, copy, modify, and distribute this software for any
 *   purpose with or without fee is hereby granted, provided that the above
 *   copyright notice and this permission notice appear in all copies.
 *
 *   THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 *   WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 *   MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 *   ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 *   WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 *   ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 *   OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */
#ifndef LIBBITCOIN_SYSTEM_CRYPTO_EXTERNAL_AES256_HPP
#define LIBBITCOIN_SYSTEM_CRYPTO_EXTERNAL_AES256_HPP

#include <array>
#include <bitcoin/system/define.hpp>

namespace libbitcoin {
namespace system {
namespace aes256 {
    
/// This is an implementation of AES256.
/// NIST selected three members of the Rijndael family, each with a block
/// size of 128 bits, but three different key lengths: 128, 192 and 256 bits.

constexpr size_t block_size = bytes<128>;
typedef std::array<uint8_t, block_size> block;

constexpr size_t secret_size = bytes<256>;
typedef std::array<uint8_t, secret_size> secret;

/// Perform aes256 encryption/decryption on a data block.
void encrypt(block& bytes, const secret& key) NOEXCEPT;
void decrypt(block& bytes, const secret& key) NOEXCEPT;

} // namespace aes256
} // namespace system
} // namespace libbitcoin

#endif
