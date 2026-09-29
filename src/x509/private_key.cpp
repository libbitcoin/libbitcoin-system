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
#include <bitcoin/system/x509/private_key.hpp>

#include <algorithm>
#include <string>
#include <bitcoin/system/crypto/crypto.hpp>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/hash/hash.hpp>
#include <bitcoin/system/math/math.hpp>
#include <bitcoin/system/x509/der.hpp>
#include <bitcoin/system/x509/oids.hpp>
#include <bitcoin/system/x509/pem.hpp>

// rfc5915 (sec1), rfc5958 (pkcs8), rfc8018 (pbes2), rfc3565 (aes-cbc)

namespace libbitcoin {
namespace system {
namespace x509 {

using namespace der;

constexpr uint64_t sec1_version = 1;
constexpr uint64_t pkcs8_version = 0;
constexpr std::string_view sec1_label{ "EC PRIVATE KEY" };
constexpr std::string_view pkcs8_label{ "PRIVATE KEY" };
constexpr std::string_view encrypted_label{ "ENCRYPTED PRIVATE KEY" };

static bool is_equal(const_byte_span left, const_byte_span right) NOEXCEPT
{
    return std::equal(left.begin(), left.end(), right.begin(), right.end());
}

template <typename Algorithm>
static void cbc_decrypt(data_chunk& text, const std::string& password,
    const_byte_span salt, size_t iterations, const_byte_span iv) NOEXCEPT
{
    const data_slice phrase(password);
    const data_slice seed(salt.begin(), salt.end());
    constexpr auto size = array_count<typename Algorithm::key_t>;
    auto key = pbkd<sha256>::key<size>(phrase, seed, iterations);
    auto schedule = Algorithm::expand(key);

    typename Algorithm::block_t chain{};
    std::copy(iv.begin(), iv.end(), chain.begin());
    Algorithm::cbc_decrypt(text, text, chain, schedule);
    wipe(key);
    wipe(schedule);
}

// decode
// ----------------------------------------------------------------------------

bool decode_sec1(secret& out, const_byte_span der) NOEXCEPT
{
    reader outer{ der };
    auto key = outer.read_nested(sequence_tag);
    const auto version = key.read_unsigned();
    const auto octets = key.read_octet_string();

    if (key.peek() == explicit_tag(0))
    {
        auto parameters = key.read_nested(explicit_tag(0));
        const auto curve = parameters.read_oid();
        if (!parameters.is_complete() || !is_equal(curve, oid::secp256r1))
            return false;
    }

    const_byte_span point{};
    if (key.peek() == explicit_tag(1))
    {
        auto wrapper = key.read_nested(explicit_tag(1));
        point = wrapper.read_bit_string();
        if (!wrapper.is_complete())
            return false;
    }

    if (!outer.is_complete() || !key.is_complete() ||
        (version != sec1_version) || (octets.size() != out.size()))
        return false;

    secret value{};
    std::copy(octets.begin(), octets.end(), value.begin());

    secp256r1::point_t expected{};
    if (!secp256r1::public_key(expected, value))
        return false;

    if (!point.empty() && !is_equal(point, expected))
        return false;

    out = value;
    return true;
}

bool decode_pkcs8(secret& out, const_byte_span der) NOEXCEPT
{
    reader outer{ der };
    auto info = outer.read_nested(sequence_tag);
    const auto version = info.read_unsigned();
    auto algorithm = info.read_nested(sequence_tag);
    const auto key_type = algorithm.read_oid();
    const auto curve = algorithm.read_oid();
    const auto octets = info.read_octet_string();

    // Attributes are ignored.
    if (info.peek() == explicit_tag(0))
        info.skip();

    if (!outer.is_complete() || !info.is_complete() ||
        !algorithm.is_complete() || (version != pkcs8_version) ||
        !is_equal(key_type, oid::ec_public_key) ||
        !is_equal(curve, oid::secp256r1))
        return false;

    return decode_sec1(out, octets);
}

bool decode_encrypted_pkcs8(secret& out, const_byte_span der,
    const std::string& password) NOEXCEPT
{
    reader outer{ der };
    auto info = outer.read_nested(sequence_tag);
    auto algorithm = info.read_nested(sequence_tag);
    const auto scheme = algorithm.read_oid();
    auto parameters = algorithm.read_nested(sequence_tag);
    auto derivation = parameters.read_nested(sequence_tag);
    const auto function = derivation.read_oid();
    auto settings = derivation.read_nested(sequence_tag);
    const auto salt = settings.read_octet_string();
    const auto iterations = settings.read_unsigned();

    uint64_t length{};
    if (settings.peek() == integer_tag)
        length = settings.read_unsigned();

    // The default (hmacWithSHA1) is not supported, so the prf is required.
    auto prf = settings.read_nested(sequence_tag);
    const auto hmac = prf.read_oid();
    if (prf.peek() == null_tag)
        prf.read_null();

    auto encryption = parameters.read_nested(sequence_tag);
    const auto cipher = encryption.read_oid();
    const auto iv = encryption.read_octet_string();
    const auto encrypted = info.read_octet_string();

    const auto complete = outer.is_complete() && info.is_complete() &&
        algorithm.is_complete() && parameters.is_complete() &&
        derivation.is_complete() && settings.is_complete() &&
        prf.is_complete() && encryption.is_complete();

    const auto supported = is_equal(scheme, oid::pbes2) &&
        is_equal(function, oid::pbkdf2) && is_equal(hmac, oid::hmac_sha256);

    const auto is_aes128 = is_equal(cipher, oid::aes128_cbc);
    const auto is_aes256 = is_equal(cipher, oid::aes256_cbc);
    const auto key_size = is_aes256 ? array_count<aes256::key_t> :
        array_count<aes128::key_t>;

    constexpr auto block = array_count<aes128::block_t>;
    const auto blocks = !encrypted.empty() &&
        is_zero(encrypted.size() % block);

    if (!complete || !supported || !(is_aes128 || is_aes256) || !blocks ||
        (iv.size() != block) || is_zero(iterations) ||
        (iterations > max_uint32) ||
        (!is_zero(length) && (length != key_size)))
        return false;

    data_chunk text(encrypted.begin(), encrypted.end());
    const auto count = possible_narrow_cast<size_t>(iterations);
    if (is_aes256)
        cbc_decrypt<aes256>(text, password, salt, count, iv);
    else
        cbc_decrypt<aes128>(text, password, salt, count, iv);

    // pkcs7 padding (rfc8018 6.1.1).
    const auto last = text.back();
    const auto pad = wide_cast<size_t>(last);
    const auto is_pad = [=](uint8_t byte) NOEXCEPT
    {
        return byte == last;
    };

    auto valid = !is_zero(pad) && (pad <= block);
    if (valid)
    {
        const auto padding = std::next(text.begin(), text.size() - pad);
        valid = std::all_of(padding, text.end(), is_pad) &&
            decode_pkcs8(out, const_byte_span{ text }.first(text.size() - pad));
    }

    wipe(text.data(), text.size());
    return valid;
}

bool decode_private_key(secret& out, const std::string& text,
    const std::string& password) NOEXCEPT
{
    pems blocks{};
    if (!decode_pem(blocks, text))
        return false;

    for (const auto& block: blocks)
    {
        if (block.label == sec1_label)
            return decode_sec1(out, block.data);

        if (block.label == pkcs8_label)
            return decode_pkcs8(out, block.data);

        if (block.label == encrypted_label)
            return decode_encrypted_pkcs8(out, block.data, password);
    }

    return false;
}

// encode
// ----------------------------------------------------------------------------

data_chunk encode_sec1(const secret& key) NOEXCEPT
{
    secp256r1::point_t point{};
    if (!secp256r1::public_key(point, key))
        return {};

    writer curve{};
    curve.write_oid(oid::secp256r1);

    writer public_key{};
    public_key.write_bit_string(point);

    writer body{};
    body.write_unsigned(sec1_version);
    body.write_octet_string(key);
    body.write_nested(explicit_tag(0), curve);
    body.write_nested(explicit_tag(1), public_key);

    writer out{};
    out.write_nested(sequence_tag, body);
    return out.data();
}

data_chunk encode_pkcs8(const secret& key) NOEXCEPT
{
    const auto sec1 = encode_sec1(key);
    if (sec1.empty())
        return {};

    writer algorithm{};
    algorithm.write_oid(oid::ec_public_key);
    algorithm.write_oid(oid::secp256r1);

    writer body{};
    body.write_unsigned(pkcs8_version);
    body.write_nested(sequence_tag, algorithm);
    body.write_octet_string(sec1);

    writer out{};
    out.write_nested(sequence_tag, body);
    return out.data();
}

std::string encode_private_key(const secret& key) NOEXCEPT
{
    const auto pkcs8 = encode_pkcs8(key);
    if (pkcs8.empty())
        return {};

    return encode_pem(std::string{ pkcs8_label }, pkcs8);
}

} // namespace x509
} // namespace system
} // namespace libbitcoin
