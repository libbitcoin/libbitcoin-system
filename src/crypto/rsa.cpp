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
#include <bitcoin/system/crypto/rsa.hpp>

#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/endian/endian.hpp>
#include <bitcoin/system/hash/hash.hpp>
#include <bitcoin/system/math/math.hpp>
#include <bitcoin/system/x509/x509.hpp>

// based on:
// datatracker.ietf.org/doc/html/rfc8017
// [Moriarty, Kaliski, Jonsson, Rusch]

namespace libbitcoin {
namespace system {

BC_PUSH_WARNING(NO_ARRAY_INDEXING)
BC_PUSH_WARNING(NO_DYNAMIC_ARRAY_INDEXING)

// pkcs1 v1.5: 0x00 || 0x01 || 0xff padding || 0x00 || payload (rfc8017 9.2).
constexpr uint8_t pkcs1_block = 0x01;
constexpr uint8_t pkcs1_padding = 0xff;
constexpr auto pkcs1_minimum_padding = 8_size;
constexpr auto pkcs1_framing = 3_size;

// pss: masked (padding || 0x01 || salt) || hash || 0xbc (rfc8017 9.1.1).
constexpr uint8_t pss_separator = 0x01;
constexpr uint8_t pss_trailer = 0xbc;
constexpr data_array<8> pss_prefix{};

// Big-endian magnitude without leading zeros.
static data_chunk to_magnitude(const_byte_span value) NOEXCEPT
{
    const auto first = std::find_if(value.begin(), value.end(),
        [](uint8_t byte) NOEXCEPT
        {
            return is_nonzero(byte);
        });

    return { first, value.end() };
}

// mgf1 (rfc8017 B.2.1), hashes of seed and each big-endian counter.
template <typename Hash>
static data_chunk mask(const_byte_span seed, size_t size) NOEXCEPT
{
    data_chunk out{};
    for (uint32_t counter{}; out.size() < size; ++counter)
    {
        accumulator<Hash> hasher{};
        hasher.write(seed.size(), seed.data());
        hasher.write(to_big_endian(counter));
        const auto block = hasher.flush();
        out.insert(out.end(), block.begin(), block.end());
    }

    out.resize(size);
    return out;
}

// emsa-pss-verify (rfc8017 9.1.2), salt size is hash size.
template <typename Hash>
static bool pss_verify(const_byte_span encoded, size_t bits,
    const_byte_span digest) NOEXCEPT
{
    constexpr auto hash_size = array_count<typename Hash::digest_t>;
    constexpr auto salt_size = hash_size;
    const auto size = encoded.size();
    if (size < hash_size + salt_size + two || encoded.back() != pss_trailer)
        return false;

    // Encoding bits beyond its bit size must be zero.
    const auto masked = encoded.first(size - hash_size - one);
    const auto hash = encoded.subspan(masked.size(), hash_size);
    const auto unused = to_bits(size) - bits;
    if (is_nonzero(shift_right(masked.front(), byte_bits - unused)))
        return false;

    auto block = mask<Hash>(hash, masked.size());
    for (size_t index{}; index < block.size(); ++index)
        block[index] ^= masked[index];

    mask_left_into(block.front(), unused);

    // Unmasked block is zero padding, separator and salt.
    const auto padding = block.size() - salt_size - one;
    const auto start = block.begin();
    const auto zero_padding = std::all_of(start, std::next(start, padding),
        [](uint8_t byte) NOEXCEPT
        {
            return is_zero(byte);
        });

    if (!zero_padding || block[padding] != pss_separator)
        return false;

    accumulator<Hash> hasher{};
    hasher.write(pss_prefix);
    hasher.write(digest.size(), digest.data());
    hasher.write(salt_size, &block[add1(padding)]);
    const auto expected = hasher.flush();
    return std::equal(expected.begin(), expected.end(), hash.begin());
}

// rsa
// ----------------------------------------------------------------------------

bool rsa::decode(public_key& out, const_byte_span der) NOEXCEPT
{
    x509::der::reader reader{ der };
    auto key = reader.read_nested(x509::der::sequence_tag);
    const auto modulus = key.read_integer();
    const auto exponent = key.read_integer();
    if (!reader.is_complete() || !key.is_complete() || modulus.empty() ||
        exponent.empty())
        return false;

    // Both integers are positive.
    if (get_left(modulus.front()) || get_left(exponent.front()))
        return false;

    out.modulus = to_magnitude(modulus);
    out.exponent = to_magnitude(exponent);
    return is_valid(out);
}

bool rsa::verify_pkcs1(const public_key& key, const_byte_span signature,
    const_byte_span payload) NOEXCEPT
{
    data_chunk encoded{};
    if (!open(encoded, key, signature))
        return false;

    // Payload follows zero byte, block type, padding and zero byte.
    const auto size = encoded.size();
    if (size < payload.size() + pkcs1_minimum_padding + pkcs1_framing)
        return false;

    data_chunk expected(size, pkcs1_padding);
    expected[zero] = 0x00;
    expected[one] = pkcs1_block;
    expected[sub1(size - payload.size())] = 0x00;
    std::copy(payload.begin(), payload.end(),
        std::prev(expected.end(), payload.size()));

    return encoded == expected;
}

bool rsa::verify_pss(const public_key& key, const_byte_span signature,
    const_byte_span digest) NOEXCEPT
{
    data_chunk encoded{};
    if (!open(encoded, key, signature))
        return false;

    // Encoding has one bit less than modulus, a byte less if bits = 8n + 1.
    const auto modulus_bits = to_bits(sub1(key.modulus.size())) +
        bit_width(key.modulus.front());
    const auto bits = sub1(modulus_bits);
    const auto size = to_ceilinged_bytes(bits);
    if (size < encoded.size() && is_nonzero(encoded.front()))
        return false;

    const auto start = std::prev(encoded.end(), size);
    const const_byte_span message{ start, encoded.end() };

    switch (digest.size())
    {
        case array_count<sha256::digest_t>:
        {
            return pss_verify<sha256>(message, bits, digest);
        }
        case array_count<sha512_384::digest_t>:
        {
            return pss_verify<sha512_384>(message, bits, digest);
        }
        case array_count<sha512::digest_t>:
        {
            return pss_verify<sha512>(message, bits, digest);
        }
        default:
        {
            return false;
        }
    }
}

// protected
// ----------------------------------------------------------------------------

// Minimal odd modulus of supported size, odd exponent in [3, n - 1].
bool rsa::is_valid(const public_key& key) NOEXCEPT
{
    const auto& n = key.modulus;
    const auto& e = key.exponent;
    if (n.empty() || e.empty() || is_zero(n.front()) || is_zero(e.front()) ||
        n.size() > maximum_size || e.size() > n.size() ||
        !is_odd(n.back()) || !is_odd(e.back()))
        return false;

    const auto modulus = uintx_from_big_endian_chunk<maximum_size>(n);
    const auto exponent = uintx_from_big_endian_chunk<maximum_size>(e);
    return exponent > 1u && exponent < modulus;
}

rsa::element rsa::multiply(const element& left, const element& right,
    const element& modulus) NOEXCEPT
{
    return element{ (product{ left } * product{ right }) % product{ modulus } };
}

// Left to right binary exponentiation over public exponent.
rsa::element rsa::power(const element& base, const element& exponent,
    const element& modulus) NOEXCEPT
{
    element result{ 1 };
    for (auto bit = bit_width(exponent); bit > zero; --bit)
    {
        result = multiply(result, result, modulus);
        if (exponent.bit(sub1(bit)))
            result = multiply(result, base, modulus);
    }

    return result;
}

// rsavp1 and i2osp (rfc8017 5.2.2, 4.1), to modulus size.
bool rsa::open(data_chunk& out, const public_key& key,
    const_byte_span signature) NOEXCEPT
{
    if (!is_valid(key) || signature.size() != key.modulus.size())
        return false;

    const data_chunk bytes{ signature.begin(), signature.end() };
    const auto value = uintx_from_big_endian_chunk<maximum_size>(bytes);
    const auto modulus = uintx_from_big_endian_chunk<maximum_size>(
        key.modulus);
    if (value >= modulus)
        return false;

    const auto exponent = uintx_from_big_endian_chunk<maximum_size>(
        key.exponent);
    const auto encoded = to_big_endian_size<maximum_size>(
        power(value, exponent, modulus));

    out.assign(std::prev(encoded.end(), signature.size()), encoded.end());
    return true;
}

BC_POP_WARNING()
BC_POP_WARNING()

} // namespace system
} // namespace libbitcoin
