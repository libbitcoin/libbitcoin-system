/**
 * Copyright (c) 2011-2026 libbitcoin developers
 *
 * This file is part of libbitcoin.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef LIBBITCOIN_SYSTEM_CRYPTO_NIST_NIST_HPP
#define LIBBITCOIN_SYSTEM_CRYPTO_NIST_NIST_HPP

#include <bitcoin/system/define.hpp>
#include <bitcoin/system/hash/hash.hpp>
#include <bitcoin/system/radix/radix.hpp>

// Based on:
// SEC 2 [Recommended Elliptic Curve Domain Parameters] (2.4.2, 2.5.1).
// secg.org/sec2-v2.pdf

namespace libbitcoin {
namespace system {
namespace nist {

struct curve_t{};

/// Short Weierstrass curve y^2 = x^3 - 3x + b over prime p, of prime order n
/// with generator g. Hash is the rfc6979 nonce hash. Constants big-endian.
template <size_t Strength, typename Hash>
struct curve
{
    using T = curve_t;
    using H = Hash;
    static constexpr auto strength = Strength;
    static constexpr auto size = bytes<Strength>;
    using element_t = data_array<size>;
};

/// secp256r1 (nist p-256).
struct p256
  : curve<256, sha256>
{
    static constexpr element_t prime = base16_array(
        "ffffffff00000001000000000000000000000000ffffffffffffffffffffffff");
    static constexpr element_t order = base16_array(
        "ffffffff00000000ffffffffffffffffbce6faada7179e84f3b9cac2fc632551");
    static constexpr element_t b = base16_array(
        "5ac635d8aa3a93e7b3ebbd55769886bc651d06b0cc53b0f63bce3c3e27d2604b");
    static constexpr element_t gx = base16_array(
        "6b17d1f2e12c4247f8bce6e563a440f277037d812deb33a0f4a13945d898c296");
    static constexpr element_t gy = base16_array(
        "4fe342e2fe1a7f9b8ee7eb4a7c0f9e162bce33576b315ececbb6406837bf51f5");
};

/// secp384r1 (nist p-384).
struct p384
  : curve<384, sha512_384>
{
    static constexpr element_t prime = base16_array(
        "fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffe"
        "ffffffff0000000000000000ffffffff");
    static constexpr element_t order = base16_array(
        "ffffffffffffffffffffffffffffffffffffffffffffffffc7634d81f4372ddf"
        "581a0db248b0a77aecec196accc52973");
    static constexpr element_t b = base16_array(
        "b3312fa7e23ee7e4988e056be3f82d19181d9c6efe8141120314088f5013875a"
        "c656398d8a2ed19d2a85c8edd3ec2aef");
    static constexpr element_t gx = base16_array(
        "aa87ca22be8b05378eb1c71ef320ad746e1d3b628ba79b9859f741e082542a38"
        "5502f25dbf55296c3a545e3872760ab7");
    static constexpr element_t gy = base16_array(
        "3617de4a96262c6f5d9e98bf9292dc29f8f41dbd289a147ce9da3113b5f0b8c0"
        "0a60b1ce1d7e819d7a431d7c90ea0e5f");
};

} // namespace nist
} // namespace system
} // namespace libbitcoin

#endif
