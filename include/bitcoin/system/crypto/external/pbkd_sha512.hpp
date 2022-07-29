/* OpenBSD: pkcs5_pbkdf2.c, v 1.9 2015/02/05 12:59:57 millert */
/**
 * Copyright (c) 2008 Damien Bergamini <damien.bergamini@free.fr>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */
#ifndef LIBBITCOIN_SYSTEM_CRYPTO_EXTERNAL_PBKD_SHA512_HPP
#define LIBBITCOIN_SYSTEM_CRYPTO_EXTERNAL_PBKD_SHA512_HPP

#include <bitcoin/system/define.hpp>
#include <bitcoin/system/math/math.hpp>

namespace libbitcoin {
namespace system {
namespace pbkd {
namespace sha512 {

/// pbkdf2::sha512 size limited to [sub1(2^64) * 2^5] (?verify).
constexpr auto maximum_size = sub1(power2<uint64_t>(64u)) * power2<uint64_t>(5u);

/// False if buffer_size > pbkdf2::maximum_size.
bool hash(const uint8_t* passphrase, size_t passphrase_size,
    const uint8_t* salt, size_t salt_size, uint64_t iterations,
    uint8_t* buffer, size_t buffer_size) NOEXCEPT;

} // namespace sha512
} // namespace pbkd
} // namespace system
} // namespace libbitcoin

#endif
