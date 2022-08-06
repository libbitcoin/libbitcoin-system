/**
 * Copyright (c) 2011-2022 libbitcoin developers (see AUTHORS)
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
#include <bitcoin/system/wallet/keys/encrypted_keys.hpp>

#include <algorithm>
#include <stdexcept>
#include <bitcoin/system/crypto/crypto.hpp>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/endian/endian.hpp>
#include <bitcoin/system/math/math.hpp>
#include <bitcoin/system/unicode/unicode.hpp>
#include <bitcoin/system/wallet/keys/ec_private.hpp>
#include <bitcoin/system/wallet/keys/ec_public.hpp>
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
static_assert(quarter == to_half(aes256::block_size));

// address_
// ----------------------------------------------------------------------------

static hash_digest address_hash(const payment_address& address) NOEXCEPT
{
    return bitcoin_hash_slice(address.encoded());
}

static bool address_salt(ek_salt& salt,
    const payment_address& address) NOEXCEPT
{
    salt = slice<zero, ek_salt_size>(address_hash(address));
    return true;
}

static bool address_salt(ek_salt& salt, const ec_compressed& point,
    uint8_t version, bool compressed) NOEXCEPT
{
    const payment_address address(ec_public{ point, compressed }, version);
    return address ? address_salt(salt, address) : false;
}

static bool address_salt(ek_salt& salt, const ec_secret& secret,
    uint8_t version, bool compressed) NOEXCEPT
{
    const payment_address address({ secret, version, compressed });
    return address ? address_salt(salt, address) : false;
}

static bool address_validate(const ek_salt& salt,
    const payment_address& address) NOEXCEPT
{
    const auto hash = address_hash(address);
    return std::equal(hash.begin(), hash.begin() + salt.size(), salt.begin());
}

static bool address_validate(const ek_salt& salt, const ec_compressed& point,
    uint8_t version, bool compressed) NOEXCEPT
{
    const payment_address address(ec_public{ point, compressed }, version);
    return address ? address_validate(salt, address) : false;
}

static bool address_validate(const ek_salt& salt, const ec_secret& secret,
    uint8_t version, bool compressed) NOEXCEPT
{
    const payment_address address({ secret, version, compressed });
    return address ? address_validate(salt, address) : false;
}

// point_
// ----------------------------------------------------------------------------

static hash_digest point_hash(const ec_compressed& point) NOEXCEPT
{
    return slice<one, ec_compressed_size>(point);
}

static one_byte point_sign(uint8_t byte, const hash_digest& hash) NOEXCEPT
{
    static constexpr uint8_t low_bit_mask = 0x01;
    const uint8_t last_byte = hash.back();
    const uint8_t last_byte_odd_field = last_byte & low_bit_mask;
    const uint8_t sign_byte = byte ^ last_byte_odd_field;
    return to_array(sign_byte);
}

static one_byte point_sign(const one_byte& single,
    const hash_digest& hash) NOEXCEPT
{
    return point_sign(single.front(), hash);
}

// scrypt_
// ----------------------------------------------------------------------------

static hash_digest scrypt_token(const data_slice& data,
    const data_slice& salt) NOEXCEPT
{
    // Arbitrary scrypt parameters from BIP38.
    return scrypt<16384, 8, 8, true>::hash<hash_size>(data, salt);
}

static long_hash scrypt_pair(const data_slice& data,
    const data_slice& salt) NOEXCEPT
{
    // Arbitrary scrypt parameters from BIP38.
    return scrypt<1024, 1, 1, true>::hash<long_hash_size>(data, salt);
}

static long_hash scrypt_private(const data_slice& data,
    const data_slice& salt) NOEXCEPT
{
    // Arbitrary scrypt parameters from BIP38.
    return scrypt<16384, 8, 8, true>::hash<long_hash_size>(data, salt);
}

// set_flags
// ----------------------------------------------------------------------------

constexpr one_byte set_flags(bool compressed, bool lot_sequence,
    bool multiplied) NOEXCEPT
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

static constexpr one_byte set_flags(bool compressed, bool lot_sequence) NOEXCEPT
{
    return set_flags(compressed, lot_sequence, false);
}

static constexpr one_byte set_flags(bool compressed) NOEXCEPT
{
    return set_flags(compressed, false);
}

// create_key_pair
// ----------------------------------------------------------------------------

static void create_private_key(encrypted_private& out_private,
    const one_byte& flags, const ek_salt& salt, const ek_entropy& entropy,
    const hash_digest& derived1, const hash_digest& derived2,
    const ek_seed& seed, uint8_t version) NOEXCEPT
{
    const auto prefix = parse_encrypted_private::prefix_factory(version, true);

    auto encrypt1 = xor_data<half>(seed, derived1);
    aes256::encrypt(encrypt1, derived2);
    const auto combined = splice(slice<quarter, half>(encrypt1),
        slice<half, half + quarter>(seed));

    auto encrypt2 = xor_offset<half, zero, half>(combined, derived1);
    aes256::encrypt(encrypt2, derived2);
    const auto quarter1 = slice<zero, quarter>(encrypt1);
    out_private = insert_checksum<ek_private_decoded_size>(
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
    const ec_secret& secret, uint8_t version) NOEXCEPT
{
    ec_compressed point;
    if (!secret_to_public(point, secret))
        return false;

    const auto prefix = parse_encrypted_public::prefix_factory(version);
    const auto hash = point_hash(point);

    auto encrypted1 = xor_data<half>(hash, derived1);
    aes256::encrypt(encrypted1, derived2);

    auto encrypted2 = xor_offset<half, half, half>(hash, derived1);
    aes256::encrypt(encrypted2, derived2);

    const auto sign = point_sign(point.front(), derived2);
    out_public = insert_checksum<encrypted_public_decoded_size>(
    {
        prefix,
        flags,
        salt,
        entropy,
        sign,
        encrypted1,
        encrypted2
    });

    return true;
}

// There is no scenario requiring a public key, we support it for completeness.
bool create_key_pair(encrypted_private& out_private,
    encrypted_public& out_public, ec_compressed& out_point,
    const encrypted_token& token, const ek_seed& seed, uint8_t version,
    bool compressed) NOEXCEPT
{
    const parse_encrypted_token parse(token);
    if (!parse.is_valid())
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
        derived.first, derived.second, factor, version))
        return false;

    create_private_key(out_private, flags, salt, parse.entropy(),
        derived.first, derived.second, seed, version);

    out_point = point_copy;
    return true;
}

bool create_key_pair(encrypted_private& out_private, ec_compressed& out_point,
    const encrypted_token& token, const ek_seed& seed, uint8_t version,
    bool compressed) NOEXCEPT
{
    encrypted_public out_public;
    return create_key_pair(out_private, out_public, out_point, token, seed,
        version, compressed);
}

// create_token
// ----------------------------------------------------------------------------

// This call requires an ICU build, the other excluded calls are dependencies.
static data_chunk normal(const std::string& passphrase) NOEXCEPT
{
    std::string copy{ passphrase };

    LCOV_EXCL_START("Always succeeds unless HAVE_ICU undefined.")
    return to_canonical_composition(copy) ? to_chunk(copy) : data_chunk{};
    LCOV_EXCL_STOP()
}

static bool create_token(encrypted_token& out_token,
    const std::string& passphrase, const data_slice& owner_salt,
    const ek_entropy& owner_entropy,
    const data_array<parse_encrypted_token::prefix_size>& prefix) NOEXCEPT
{
    BC_ASSERT(owner_salt.size() == ek_salt_size ||
        owner_salt.size() == ek_entropy_size);

    const auto lot_sequence = owner_salt.size() == ek_salt_size;
    auto factor = scrypt_token(normal(passphrase), owner_salt);

    if (lot_sequence)
        factor = bitcoin_hash_slice(factor, owner_entropy);

    ec_compressed point;
    if (!secret_to_public(point, factor))
        return false;

    out_token = insert_checksum<encrypted_token_decoded_size>(
    {
        prefix,
        owner_entropy,
        point
    });

    return true;
}

// The salt here is owner-supplied random bits, not the address hash.
bool create_token(encrypted_token& out_token, const std::string& passphrase,
    const ek_entropy& entropy) NOEXCEPT
{
    // BIP38: If lot and sequence numbers are not being included, then
    // owner_salt is 8 random bytes instead of 4, lot_sequence is omitted and
    // owner_entropy becomes an alias for owner_salt.
    const auto prefix = parse_encrypted_token::prefix_factory(false);
    return create_token(out_token, passphrase, entropy, entropy, prefix);
}

// The salt here is owner-supplied random bits, not the address hash.
bool create_token(encrypted_token& out_token, const std::string& passphrase,
    const ek_salt& salt, uint32_t lot, uint32_t sequence) NOEXCEPT
{
    if (lot > ek_max_lot || sequence > ek_max_sequence)
        return false;

    static constexpr size_t sequence_bits = 12;
    const auto lot_sequence = shift_left(lot, sequence_bits) | sequence;
    const auto entropy = splice(salt, to_big_endian(lot_sequence));
    const auto prefix = parse_encrypted_token::prefix_factory(true);
    create_token(out_token, passphrase, salt, entropy, prefix);
    return true;
}

// encrypt
// ----------------------------------------------------------------------------

bool encrypt(encrypted_private& out_private, const ec_secret& secret,
    const std::string& passphrase, uint8_t version, bool compressed) NOEXCEPT
{
    ek_salt salt;
    if (!address_salt(salt, secret, version, compressed))
        return false;

    const auto derived = split(scrypt_private(normal(passphrase), salt));
    const auto prefix = parse_encrypted_private::prefix_factory(version,
        false);

    auto encrypted1 = xor_data<half>(secret, derived.first);
    aes256::encrypt(encrypted1, derived.second);

    auto encrypted2 = xor_offset<half, half, half>(secret, derived.first);
    aes256::encrypt(encrypted2, derived.second);

    out_private = insert_checksum<ek_private_decoded_size>(
    {
        prefix,
        set_flags(compressed),
        salt,
        encrypted1,
        encrypted2
    });

    return true;
}

// decrypt private_key
// ----------------------------------------------------------------------------

static bool decrypt_multiplied(ec_secret& out_secret,
    const parse_encrypted_private& parse,
    const std::string& passphrase) NOEXCEPT
{
    auto secret = scrypt_token(normal(passphrase), parse.owner_salt());

    if (parse.lot_sequence())
        secret = bitcoin_hash_slice(secret, parse.entropy());

    ec_compressed point;
    if (!secret_to_public(point, secret))
        return false;

    const auto salt_entropy = splice(parse.salt(), parse.entropy());
    const auto derived = split(scrypt_pair(point, salt_entropy));

    const auto encrypt1 = parse.data1();
    auto encrypt2 = parse.data2();

    aes256::decrypt(encrypt2, derived.second);
    const auto decrypt2 = xor_offset<half, 0, half>(encrypt2, derived.first);
    const auto part = split(decrypt2);
    auto extended = splice(encrypt1, part.first);

    aes256::decrypt(extended, derived.second);
    const auto decrypt1 = xor_data<half>(extended, derived.first);
    const auto factor = bitcoin_hash_slice(decrypt1, part.second);
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
    const parse_encrypted_private& parse,
    const std::string& passphrase) NOEXCEPT
{
    auto encrypt1 = splice(parse.entropy(), parse.data1());
    auto encrypt2 = parse.data2();
    const auto derived = split(scrypt_private(normal(passphrase),
        parse.salt()));

    aes256::decrypt(encrypt1, derived.second);
    aes256::decrypt(encrypt2, derived.second);

    const auto encrypted = splice(encrypt1, encrypt2);
    const auto secret = xor_data<hash_size>(encrypted, derived.first);

    const auto compressed = parse.compressed();
    const auto address_version = parse.address_version();
    if (!address_validate(parse.salt(), secret, address_version, compressed))
        return false;

    out_secret = secret;
    return true;
}

bool decrypt(ec_secret& out_secret, uint8_t& out_version, bool& out_compressed,
    const encrypted_private& key, const std::string& passphrase) NOEXCEPT
{
    const parse_encrypted_private parse(key);
    if (!parse.is_valid())
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
    const std::string& passphrase) NOEXCEPT
{
    const parse_encrypted_public parse(key);
    if (!parse.is_valid())
        return false;

    const auto version = parse.address_version();
    const auto lot_sequence = parse.lot_sequence();
    auto factor = scrypt_token(normal(passphrase), parse.owner_salt());

    if (lot_sequence)
        factor = bitcoin_hash_slice(factor, parse.entropy());

    ec_compressed point;
    if (!secret_to_public(point, factor))
        return false;

    const auto salt_entropy = splice(parse.salt(), parse.entropy());
    const auto derived = split(scrypt_pair(point, salt_entropy));
    auto encrypt = split(parse.data());

    aes256::decrypt(encrypt.first, derived.second);
    const auto decrypt1 = xor_data<half>(encrypt.first, derived.first);

    aes256::decrypt(encrypt.second, derived.second);
    const auto decrypt2 = xor_offset<half, zero, half>(encrypt.second, derived.first);

    const auto sign_byte = point_sign(parse.sign(), derived.second);
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

} // namespace wallet
} // namespace system
} // namespace libbitcoin
