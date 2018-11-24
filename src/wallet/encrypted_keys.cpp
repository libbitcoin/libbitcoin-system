/**
 * Copyright (c) 2011-2018 libbitcoin developers (see AUTHORS)
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
#include <bitcoin/system/wallet/encrypted_keys.hpp>

#include <cstdint>
#include <cstddef>
#include <algorithm>
#include <stdexcept>
#include <boost/locale.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/math/checksum.hpp>
#include <bitcoin/system/math/crypto.hpp>
#include <bitcoin/system/math/hash.hpp>
#include <bitcoin/system/math/elliptic_curve.hpp>
#include <bitcoin/system/unicode/unicode.hpp>
#include <bitcoin/system/utility/assert.hpp>
#include <bitcoin/system/utility/data.hpp>
#include <bitcoin/system/utility/endian.hpp>
#include <bitcoin/system/wallet/ec_private.hpp>
#include <bitcoin/system/wallet/ec_public.hpp>
#include "parse_encrypted_keys/parse_encrypted_key.hpp"
#include "parse_encrypted_keys/parse_encrypted_prefix.hpp"
#include "parse_encrypted_keys/parse_encrypted_private.hpp"
#include "parse_encrypted_keys/parse_encrypted_public.hpp"
#include "parse_encrypted_keys/parse_encrypted_token.hpp"

namespace libbitcoin {
namespace system {
namespace wallet {

// Alias commonly-used constants for brevity.
static constexpr auto half = half_hash_size;
static constexpr auto quarter = quarter_hash_size;

// Ensure that hash sizes are aligned with AES block size.
static_assert(2 * quarter == aes256_block_size, "oops!");

// address_
// ----------------------------------------------------------------------------

static hash_digest address_hash(const payment_address& address)
{
    return bitcoin_hash(to_chunk(address.encoded()));
}

static bool address_salt(ek_salt& salt, const payment_address& address)
{
    salt = slice<0, ek_salt_size>(address_hash(address));
    return true;
}

static bool address_salt(ek_salt& salt, const ec_compressed& point,
    uint8_t version, bool compressed)
{
    payment_address address({ point, compressed }, version);
    return address ? address_salt(salt, address) : false;
}

#ifdef WITH_ICU

static bool address_salt(ek_salt& salt, const ec_secret& secret,
    uint8_t version, bool compressed)
{
    payment_address address({ secret, version, compressed });
    return address ? address_salt(salt, address) : false;
}

static bool address_validate(const ek_salt& salt,
    const payment_address& address)
{
    const auto hash = address_hash(address);
    return std::equal(hash.begin(), hash.begin() + salt.size(), salt.begin());
}

static bool address_validate(const ek_salt& salt, const ec_compressed& point,
    uint8_t version, bool compressed)
{
    payment_address address({ point, compressed }, version);
    return address ? address_validate(salt, address) : false;
}

static bool address_validate(const ek_salt& salt, const ec_secret& secret,
    uint8_t version, bool compressed)
{
    payment_address address({ secret, version, compressed });
    return address ? address_validate(salt, address) : false;
}

#endif

// point_
// ----------------------------------------------------------------------------

static hash_digest point_hash(const ec_compressed& point)
{
    return slice<1, ec_compressed_size>(point);
}

static one_byte point_sign(uint8_t byte, const hash_digest& hash)
{
    static constexpr uint8_t low_bit_mask = 0x01;
    const uint8_t last_byte = hash.back();
    const uint8_t last_byte_odd_field = last_byte & low_bit_mask;
    const uint8_t sign_byte = byte ^ last_byte_odd_field;
    return to_array(sign_byte);
}

#ifdef WITH_ICU

static one_byte point_sign(const one_byte& single, const hash_digest& hash)
{
    return point_sign(single.front(), hash);
}

#endif

// scrypt_
// ----------------------------------------------------------------------------

#ifdef WITH_ICU

static hash_digest scrypt_token(data_slice data, data_slice salt)
{
    // Arbitrary scrypt parameters from BIP38.
    return scrypt<hash_size>(data, salt, 16384u, 8u, 8u);
}

#endif

static long_hash scrypt_pair(data_slice data, data_slice salt)
{
    // Arbitrary scrypt parameters from BIP38.
    return scrypt<long_hash_size>(data, salt, 1024u, 1u, 1u);
}

#ifdef WITH_ICU

static long_hash scrypt_private(data_slice data, data_slice salt)
{
    // Arbitrary scrypt parameters from BIP38.
    return scrypt<long_hash_size>(data, salt, 16384u, 8u, 8u);
}

#endif

// set_flags
// ----------------------------------------------------------------------------

static one_byte set_flags(bool compressed, bool lot_sequence, bool multiplied)
{
    uint8_t byte = 0;

    if (compressed)
        byte |= ek_flag::ec_compressed_key;

    if (lot_sequence)
        byte |= ek_flag::lot_sequence_key;

    if (!multiplied)
        byte |= ek_flag::ec_non_multiplied;

    return to_array(byte);
}

#ifdef WITH_ICU

static one_byte set_flags(bool compressed, bool lot_sequence)
{
    return set_flags(compressed, lot_sequence, false);
}

static one_byte set_flags(bool compressed)
{
    return set_flags(compressed, false);
}

#endif

// create_key_pair
// ----------------------------------------------------------------------------

static void create_private_key(encrypted_private& out_private,
    const one_byte& flags, const ek_salt& salt, const ek_entropy& entropy,
    const hash_digest& derived1, const hash_digest& derived2,
    const ek_seed& seed, uint8_t version)
{
    const auto prefix = parse_encrypted_private::prefix_factory(version, true);

    auto encrypt1 = xor_data<half>(seed, derived1);
    aes256_encrypt(derived2, encrypt1);
    const auto combined = splice(slice<quarter, half>(encrypt1),
        slice<half, half + quarter>(seed));

    auto encrypt2 = xor_data<half>(combined, derived1, 0, half);
    aes256_encrypt(derived2, encrypt2);
    const auto quarter1 = slice<0, quarter>(encrypt1);

    build_checked_array(out_private,
    {
        prefix,
        flags,
        salt,
        entropy,
        quarter1,
        encrypt2
    });
}

static bool create_public_key(encrypted_public& out_public,
    const one_byte& flags, const ek_salt& salt, const ek_entropy& entropy,
    const hash_digest& derived1, const hash_digest& derived2,
    const ec_secret& secret, uint8_t version)
{
    ec_compressed point;
    if (!secret_to_public(point, secret))
        return false;

    const auto prefix = parse_encrypted_public::prefix_factory(version);
    const auto hash = point_hash(point);

    auto encrypted1 = xor_data<half>(hash, derived1);
    aes256_encrypt(derived2, encrypted1);

    auto encrypted2 = xor_data<half>(hash, derived1, half);
    aes256_encrypt(derived2, encrypted2);

    const auto sign = point_sign(point.front(), derived2);
    return build_checked_array(out_public,
    {
        prefix,
        flags,
        salt,
        entropy,
        sign,
        encrypted1,
        encrypted2
    });
}

// There is no scenario requiring a public key, we support it for completeness.
bool create_key_pair(encrypted_private& out_private,
    encrypted_public& out_public, ec_compressed& out_point,
    const encrypted_token& token, const ek_seed& seed, uint8_t version,
    bool compressed)
{
    const parse_encrypted_token parse(token);
    if (!parse.valid())
        return false;

    const auto point = splice(parse.sign(), parse.data());
    auto point_copy = point;
    const auto factor = bitcoin_hash(seed);
    if (!ec_multiply(point_copy, factor))
        return false;

    ek_salt salt;
    if (!address_salt(salt, point_copy, version, compressed))
        return false;

    const auto salt_entropy = splice(salt, parse.entropy());
    const auto derived = split(scrypt_pair(point, salt_entropy));
    const auto flags = set_flags(compressed, parse.lot_sequence(), true);

    if (!create_public_key(out_public, flags, salt, parse.entropy(),
        derived.left, derived.right, factor, version))
        return false;

    create_private_key(out_private, flags, salt, parse.entropy(), derived.left,
        derived.right, seed, version);

    out_point = point_copy;
    return true;
}

bool create_key_pair(encrypted_private& out_private, ec_compressed& out_point,
    const encrypted_token& token, const ek_seed& seed, uint8_t version,
    bool compressed)
{
    encrypted_public out_public;
    return create_key_pair(out_private, out_public, out_point, token, seed,
        version, compressed);
}

#ifdef WITH_ICU

// create_token
// ----------------------------------------------------------------------------

// This call requires an ICU build, the other excluded calls are dependencies.
static data_chunk normal(const std::string& passphrase)
{
    return to_chunk(to_normal_nfc_form(passphrase));
}

static bool create_token(encrypted_token& out_token,
    const std::string& passphrase, data_slice owner_salt,
    const ek_entropy& owner_entropy,
    const byte_array<parse_encrypted_token::prefix_size>& prefix)
{
    BITCOIN_ASSERT(owner_salt.size() == ek_salt_size ||
        owner_salt.size() == ek_entropy_size);

    const auto lot_sequence = owner_salt.size() == ek_salt_size;
    auto factor = scrypt_token(normal(passphrase), owner_salt);

    if (lot_sequence)
        factor = bitcoin_hash(splice(factor, owner_entropy));

    ec_compressed point;
    if (!secret_to_public(point, factor))
        return false;

    return build_checked_array(out_token,
    {
        prefix,
        owner_entropy,
        point
    });
}

// The salt here is owner-supplied random bits, not the address hash.
bool create_token(encrypted_token& out_token, const std::string& passphrase,
    const ek_entropy& entropy)
{
    // BIP38: If lot and sequence numbers are not being included, then
    // owner_salt is 8 random bytes instead of 4, lot_sequence is omitted and
    // owner_entropy becomes an alias for owner_salt.
    const auto prefix = parse_encrypted_token::prefix_factory(false);
    return create_token(out_token, passphrase, entropy, entropy, prefix);
}

// The salt here is owner-supplied random bits, not the address hash.
bool create_token(encrypted_token& out_token, const std::string& passphrase,
    const ek_salt& salt, uint32_t lot, uint32_t sequence)
{
    if (lot > ek_max_lot || sequence > ek_max_sequence)
        return false;

    static constexpr size_t max_sequence_bits = 12;
    const uint32_t lot_sequence = (lot << max_sequence_bits) || sequence;
    const auto entropy = splice(salt, to_big_endian(lot_sequence));
    const auto prefix = parse_encrypted_token::prefix_factory(true);
    create_token(out_token, passphrase, salt, entropy, prefix);
    return true;
}

// encrypt
// ----------------------------------------------------------------------------

bool encrypt(encrypted_private& out_private, const ec_secret& secret,
    const std::string& passphrase, uint8_t version, bool compressed)
{
    ek_salt salt;
    if (!address_salt(salt, secret, version, compressed))
        return false;

    const auto derived = split(scrypt_private(normal(passphrase), salt));
    const auto prefix = parse_encrypted_private::prefix_factory(version,
        false);

    auto encrypted1 = xor_data<half>(secret, derived.left);
    aes256_encrypt(derived.right, encrypted1);

    auto encrypted2 = xor_data<half>(secret, derived.left, half);
    aes256_encrypt(derived.right, encrypted2);

    return build_checked_array(out_private,
    {
        prefix,
        set_flags(compressed),
        salt,
        encrypted1,
        encrypted2
    });
}

// decrypt private_key
// ----------------------------------------------------------------------------

static bool decrypt_multiplied(ec_secret& out_secret,
    const parse_encrypted_private& parse, const std::string& passphrase)
{
    auto secret = scrypt_token(normal(passphrase), parse.owner_salt());

    if (parse.lot_sequence())
        secret = bitcoin_hash(splice(secret, parse.entropy()));

    ec_compressed point;
    if (!secret_to_public(point, secret))
        return false;

    const auto salt_entropy = splice(parse.salt(), parse.entropy());
    const auto derived = split(scrypt_pair(point, salt_entropy));

    auto encrypt1 = parse.data1();
    auto encrypt2 = parse.data2();

    aes256_decrypt(derived.right, encrypt2);
    const auto decrypt2 = xor_data<half>(encrypt2, derived.left, 0, half);
    auto part = split(decrypt2);
    auto extended = splice(encrypt1, part.left);

    aes256_decrypt(derived.right, extended);
    const auto decrypt1 = xor_data<half>(extended, derived.left);
    const auto factor = bitcoin_hash(splice(decrypt1, part.right));
    if (!ec_multiply(secret, factor))
        return false;

    const auto compressed = parse.compressed();
    const auto address_version = parse.address_version();
    if (!address_validate(parse.salt(), secret, address_version, compressed))
        return false;

    out_secret = secret;
    return true;
}

static bool decrypt_secret(ec_secret& out_secret,
    const parse_encrypted_private& parse, const std::string& passphrase)
{
    auto encrypt1 = splice(parse.entropy(), parse.data1());
    auto encrypt2 = parse.data2();
    const auto derived = split(scrypt_private(normal(passphrase),
        parse.salt()));

    aes256_decrypt(derived.right, encrypt1);
    aes256_decrypt(derived.right, encrypt2);

    const auto encrypted = splice(encrypt1, encrypt2);
    const auto secret = xor_data<hash_size>(encrypted, derived.left);

    const auto compressed = parse.compressed();
    const auto address_version = parse.address_version();
    if (!address_validate(parse.salt(), secret, address_version, compressed))
        return false;

    out_secret = secret;
    return true;
}

bool decrypt(ec_secret& out_secret, uint8_t& out_version, bool& out_compressed,
    const encrypted_private& key, const std::string& passphrase)
{
    const parse_encrypted_private parse(key);
    if (!parse.valid())
        return false;

    const auto success = parse.multiplied() ?
        decrypt_multiplied(out_secret, parse, passphrase) :
        decrypt_secret(out_secret, parse, passphrase);

    if (success)
    {
        out_compressed = parse.compressed();
        out_version = parse.address_version();
    }

    return success;
}

// decrypt public_key
// ----------------------------------------------------------------------------

bool decrypt(ec_compressed& out_point, uint8_t& out_version,
    bool& out_compressed, const encrypted_public& key,
    const std::string& passphrase)
{
    const parse_encrypted_public parse(key);
    if (!parse.valid())
        return false;

    const auto version = parse.address_version();
    const auto lot_sequence = parse.lot_sequence();
    auto factor = scrypt_token(normal(passphrase), parse.owner_salt());

    if (lot_sequence)
        factor = bitcoin_hash(splice(factor, parse.entropy()));

    ec_compressed point;
    if (!secret_to_public(point, factor))
        return false;

    const auto salt_entropy = splice(parse.salt(), parse.entropy());
    auto derived = split(scrypt_pair(point, salt_entropy));
    auto encrypt = split(parse.data());

    aes256_decrypt(derived.right, encrypt.left);
    const auto decrypt1 = xor_data<half>(encrypt.left, derived.left);

    aes256_decrypt(derived.right, encrypt.right);
    const auto decrypt2 = xor_data<half>(encrypt.right, derived.left, 0, half);

    const auto sign_byte = point_sign(parse.sign(), derived.right);
    auto product = splice(sign_byte, decrypt1, decrypt2);
    if (!ec_multiply(product, factor))
        return false;

    if (!address_validate(parse.salt(), product, version, parse.compressed()))
        return false;

    out_point = product;
    out_version = version;
    out_compressed = parse.compressed();
    return true;
}

#endif // WITH_ICU

} // namespace wallet
} // namespace system
} // namespace libbitcoin
