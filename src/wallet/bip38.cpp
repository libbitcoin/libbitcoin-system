/**
 * Copyright (c) 2011-2015 libbitcoin developers (see AUTHORS)
 *
 * This file is part of libbitcoin.
 *
 * libbitcoin is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License with
 * additional permissions to the one published by the Free Software
 * Foundation, either version 3 of the License, or (at your option)
 * any later version. For more information see LICENSE.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#include <bitcoin/bitcoin/wallet/bip38.hpp>

#include <cstdint>
#include <cstddef>
#include <algorithm>
#include <stdexcept>
#include <boost/locale.hpp>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/formats/base16.hpp>
#include <bitcoin/bitcoin/formats/base58.hpp>
#include <bitcoin/bitcoin/math/checksum.hpp>
#include <bitcoin/bitcoin/math/crypto.hpp>
#include <bitcoin/bitcoin/math/hash.hpp>
#include <bitcoin/bitcoin/math/ec_keys.hpp>
#include <bitcoin/bitcoin/unicode/unicode.hpp>
#include <bitcoin/bitcoin/utility/assert.hpp>
#include <bitcoin/bitcoin/utility/data.hpp>
#include <bitcoin/bitcoin/utility/endian.hpp>
#include <bitcoin/bitcoin/wallet/key_formats.hpp>
#include <bitcoin/bitcoin/wallet/payment_address.hpp>

namespace libbitcoin {
namespace bip38 {

using namespace bc::wallet;

static constexpr size_t max_sequence_bits = 12;
static constexpr size_t mainnet_p2pkh_version = 0x00;

static constexpr size_t two_block_size = bc::long_hash_size;
static constexpr size_t block_size = two_block_size / 2;
static constexpr size_t half = block_size / 2;
static constexpr size_t quarter = half / 2;

typedef byte_array<two_block_size> two_block;

// The above sizes are all tied to aes256.
static_assert(2 * quarter == bc::aes256_block_size, "oops!");

// BIP38
// It is requested that the unused flag bytes NOT be used for denoting that
// the key belongs to an alt-chain [This shoud read "flag bits"].
enum flag_byte : uint8_t
{
    none = 0,
    lot_sequence = 1 << 2,
    ec_compressed = 1 << 5,
    ec_non_multiplied_low = 1 << 6,
    ec_non_multiplied_high = 1 << 7,

    // Two bits are used to represent "not multiplied".
    ec_non_multiplied = (ec_non_multiplied_low | ec_non_multiplied_high)
};

static inline bool check_flag(uint8_t flags, flag_byte flag)
{
    return (flags & flag) != 0;
}

class parse_key
{
protected:
    parse_key()
    {
    }

public:
    data_chunk owner_salt() const
    {
        BITCOIN_ASSERT(entropy.size() == entropy_size);
        const auto lot = check_flag(flags, flag_byte::lot_sequence);
        return lot ? slice(entropy, 0, salt_size) : entropy;
    }

    // salt and entropy could be strongly typed but this is simpler.
    data_chunk prefix;
    uint8_t flags;
    data_chunk salt;
    data_chunk entropy;
};

class parse_private
  : public parse_key
{
public:
    parse_private(const private_key& key)
    {
        /// salt: address hash salt.
        /// entropy: owner salt + lot-sequence or owner entropy
        prefix =    slice(key, { 0, 2 });           // 2
        flags =     slice(key, { 2, 3 })[0];        // 1
        salt =      slice(key, { 3, 7 });           // 4
        entropy =   slice(key, { 7, 15 });          // 8
        data1 =     slice(key, { 15, 23 });         // 8
        data2 =     slice(key, { 23, 39 });         // 16
        BITCOIN_ASSERT(verify_checksum(key));       // 4 { 39, 43 }
    }

    data_chunk data1;
    data_chunk data2;
};

class parse_public
  : public parse_key
{
public:
    parse_public(const public_key& key)
    {
        /// salt: address hash salt.
        /// entropy: owner salt + lot-sequence or owner entropy
        prefix =    slice(key, { 0, 5 });           // 5
        flags =     slice(key, { 5, 6 })[0];        // 1
        salt =      slice(key, { 6, 10 });          // 4
        entropy =   slice(key, { 10, 18 });         // 8
        sign =      slice(key, { 18, 19 })[0];      // 1
        data =      slice(key, { 19, 51 });         // 32
        BITCOIN_ASSERT(verify_checksum(key));       // 4 { 51, 55 }
    }

    uint8_t sign;
    data_chunk data;
};

class parse_token
{
public:
    parse_token(const token& value)
    {
        /// entropy: owner salt + lot-sequence or owner entropy
        prefix =    slice(value, { 0, 8 });         // 8
        entropy =   slice(value, { 8, 16 });        // 8
        sign =      slice(value, { 16, 17 })[0];    // 1
        data =      slice(value, { 17, 49 });       // 32
        BITCOIN_ASSERT(verify_checksum(value));     // 4 { 49, 53 }
    }

    data_chunk prefix;
    data_chunk entropy;
    uint8_t sign;
    data_chunk data;
};

// BIP38
// Alt-chain implementers should exploit the address hash for [identification].
// Since each operation in this proposal involves hashing a text representation
// of a coin address which (for Bitcoin) includes the leading '1', an alt-chain
// can easily be denoted simply by using the alt-chain's preferred format for
// representing an address.... Alt-chain implementers may also change the prefix
// such that encrypted addresses do not start with "6P".
//
// The first byte in each prefix is also the base58check version byte.
namespace prefix
{
    // This prefix results in the prefix "6P" in the base58 encoding but is
    // modified when the payment address is Bitcoin mainnet (0).
    static const data_chunk private_key
    {
        0x01, 0x42
    };

    // This prefix results in the prefix "6P" in the base58 encoding but is
    // modified when the payment address is Bitcoin mainnet (0).
    static const data_chunk private_key_multiplied
    {
        0x01, 0x43
    };

    // This prefix results in the prefix "cfrm" in the base58 encoding but is
    // modified when the payment address is Bitcoin mainnet (0).
    static const data_chunk public_key
    {
        0x64, 0x3b, 0xf6, 0xa8, 0x9a
    };

    // This prefix results in the prefix "passphrase" in the base58 encoding.
    // The prefix is not modified as the result of variations to address.
    static const data_chunk lot_token
    {
        0x2c, 0xe9, 0xb3, 0xe1, 0xff, 0x39, 0xe2, 0x51
    };

    // This prefix results in the prefix "passphrase" in the base58 encoding.
    // The prefix is not modified as the result of variations to address.
    static const data_chunk plain_token
    {
        0x2c, 0xe9, 0xb3, 0xe1, 0xff, 0x39, 0xe2, 0x53
    };
}

static uint8_t set_flags(bool multiplied, bool compressed, bool lot_sequence)
{
    uint8_t byte = flag_byte::none;
    if (lot_sequence)
        byte |= flag_byte::lot_sequence;

    if (compressed)
        byte |= flag_byte::ec_compressed;

    if (!multiplied)
        byte |= flag_byte::ec_non_multiplied;

    return byte;
}

static data_chunk new_flags(bool compressed)
{
    return
    {
        set_flags(false, compressed, false)
    };
}

static data_chunk new_flags(data_slice prefix, bool compressed)
{
    const auto lot = prefix::lot_token;
    const auto is_lot = std::equal(lot.begin(), lot.end(), prefix.begin());
    return
    {
        set_flags(true, compressed, is_lot)
    };
}

// This provides a unidirectional mapping for all prefix variants.
static uint8_t address_from_key(const uint8_t prefix_version,
    const data_chunk& default_prefix)
{
    const auto default_prefix_version = default_prefix[0];
    return prefix_version == default_prefix_version ? mainnet_p2pkh_version :
        prefix_version;
}

// This provides a unidirectional mapping for all prefix variants.
static uint8_t address_to_key(const uint8_t address_version,
    const data_chunk& default_prefix)
{
    const auto default_prefix_version = default_prefix[0];
    return address_version == mainnet_p2pkh_version ? default_prefix_version :
        address_version;
}

static hash_digest address_hash(uint8_t version, const ec_point& point)
{
    const payment_address address(version, point);
    return bitcoin_hash(to_data_chunk(address.to_string()));
}

static data_chunk address_salt(uint8_t version, const ec_point& point)
{
    return slice(address_hash(version, point), 0, salt_size);
}

static bool address_validate(const ec_point& point, data_slice salt,
    uint8_t version, bool compressed)
{
    BITCOIN_ASSERT(salt.size() == salt_size);
    const auto hash = address_hash(version, point);
    return std::equal(hash.begin(), hash.begin() + salt.size(), salt.begin());
}

static bool address_validate(const ec_secret& secret, data_slice salt,
    uint8_t version, bool compressed)
{
    BITCOIN_ASSERT(salt.size() == salt_size);
    const auto point = secret_to_public_key(secret, compressed);
    return address_validate(point, salt, version, compressed);
}

static data_chunk point_sign(uint8_t byte, data_slice buffer)
{
    BITCOIN_ASSERT(!buffer.empty());
    static constexpr uint8_t low_bit_mask = 0x01;
    const uint8_t last_byte = buffer.data()[buffer.size() - 1];
    const uint8_t last_byte_odd_field = last_byte & low_bit_mask;
    const uint8_t sign_byte = byte ^ last_byte_odd_field;
    return
    {
        sign_byte
    };
}

static ec_point to_point(uint8_t sign, data_slice buffer)
{
    return build_data(
    {
        data_chunk{ sign },
        buffer
    });
}

static data_chunk private_key_prefix(const uint8_t address_version,
    const data_chunk& default_prefix)
{
    auto prefix_copy = default_prefix;
    prefix_copy[0] = address_to_key(address_version, default_prefix);
    return prefix_copy;
}

static data_chunk public_key_prefix(const uint8_t address_version)
{
    auto prefix_copy = prefix::public_key;
    prefix_copy[0] = address_to_key(address_version, prefix::public_key);
    return prefix_copy;
}

static void scrypt_token(hash_digest& out, data_slice data, data_slice salt)
{
    // Arbitrary scrypt parameters from BIP-38.
    scrypt(out, data, salt, 16384u, 8u, 8u);
}

static void scrypt_pair(long_hash& out, data_slice data, data_slice salt)
{
    // Arbitrary scrypt parameters from BIP-38.
    scrypt(out, data, salt, 1024u, 1u, 1u);
}

static void scrypt_private(long_hash& out, data_slice data, data_slice salt)
{
    // Arbitrary scrypt parameters from BIP-38.
    scrypt(out, data, salt, 16384u, 8u, 8u);
}

static void create_private_key(private_key& out_private, data_slice flags,
    data_slice salt, data_slice entropy, data_slice derived1,
    data_slice derived2, const seed& seed, uint8_t address_version)
{
    auto encrypted1 = xor_data(seed, derived1, 0, half);
    aes256_encrypt(derived2, encrypted1);

    const auto combined = build_data(
    {
        slice(encrypted1, quarter, half),
        slice(seed, half, half + quarter)
    });

    auto encrypted2 = xor_data(combined, derived1, 0, half, half);
    aes256_encrypt(derived2, encrypted2);
    const auto quarter1 = slice(encrypted1, 0, quarter);

    const auto prefix = private_key_prefix(address_version,
        prefix::private_key_multiplied);

    build_checked_array(out_private,
    {
        prefix,
        flags,
        salt,
        entropy,
        quarter1,
        encrypted2
    });
}

static void create_public_key(public_key& out_public, data_slice flags,
    data_slice salt, data_slice entropy, data_slice derived1,
    data_slice derived2, const ec_secret& secret, uint8_t address_version)
{
    const auto point = secret_to_public_key(secret, true);
    const data_chunk unsigned_point(point.begin() + 1, point.end());

    auto encrypted1 = xor_data(unsigned_point, derived1, 0, half);
    aes256_encrypt(derived2, encrypted1);

    auto encrypted2 = xor_data(unsigned_point, derived1, half, half);
    aes256_encrypt(derived2, encrypted2);

    const auto prefix = public_key_prefix(address_version);
    const auto sign = point_sign(point.front(), derived2);

    build_checked_array(out_public,
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
bool create_key_pair(private_key& out_private, public_key& out_public,
    ec_point& out_point, const token& token, const seed& seed, uint8_t version,
    bool compressed)
{
    // TODO: validate token prefix.
    if (!verify_checksum(token))
        return false;

    const parse_token parse(token);
    const auto point = to_point(parse.sign, parse.data);

    out_point = point;
    const auto factor = bitcoin_hash(seed);
    ec_multiply(out_point, factor);
    if (!compressed)
        out_point = decompress_public_key(out_point);

    const auto salt = address_salt(version, out_point);
    const auto salt_entropy = build_data({ salt, parse.entropy });

    two_block derived;
    scrypt_pair(derived, point, salt_entropy);

    data_chunk derived1;
    data_chunk derived2;
    split(derived1, derived2, derived, two_block_size);

    // This forwards the lot_sequence flag and sets multiplied.
    const auto flags = new_flags(parse.prefix, compressed);

    create_private_key(out_private, flags, salt, parse.entropy, derived1,
        derived2, seed, version);
    create_public_key(out_public, flags, salt, parse.entropy, derived1,
        derived2, factor, version);

    return true;
}

bool create_key_pair(private_key& out_private, ec_point& out_point,
    const token& token, const seed& seed, uint8_t version, bool compressed)
{
    public_key out_public;
    return create_key_pair(out_private, out_public, out_point, token, seed,
        version, compressed);
}

#ifdef WITH_ICU

// This call requires an ICU build, the other excluded calls are dependencies.
static data_chunk normal(const std::string& passphrase)
{
    return to_data_chunk(to_normal_nfc_form(passphrase));
}

static void create_token(token& out_token, const std::string& passphrase,
    data_slice owner_salt, data_slice owner_entropy, data_slice prefix)
{
    BITCOIN_ASSERT(owner_salt.size() == salt_size ||
        owner_salt.size() == entropy_size);
    BITCOIN_ASSERT(owner_entropy.size() == entropy_size);
    const auto lot = owner_salt.size() == salt_size;

    ec_secret factor;
    scrypt_token(factor, normal(passphrase), owner_salt);

    if (lot)
        factor = bitcoin_hash(build_data({ factor, owner_entropy }));

    const auto point = secret_to_public_key(factor, true);

    build_checked_array(out_token,
    {
        prefix,
        owner_entropy,
        point
    });
}

// The salt here is owner-supplied random bits, not the address hash.
void create_token(token& out_token, const std::string& passphrase,
    const entropy& entropy)
{
    // BIP38: If lot and sequence numbers are not being included, then
    // owner_salt is 8 random bytes instead of 4, lot_sequence is omitted and
    // owner_entropy becomes an alias for owner_salt.
    create_token(out_token, passphrase, entropy, entropy, prefix::plain_token);
}

// The salt here is owner-supplied random bits, not the address hash.
bool create_token(token& out_token, const std::string& passphrase,
    const salt& salt, uint32_t lot, uint32_t sequence)
{
    if (lot > max_token_lot || sequence > max_token_sequence)
        return false;

    const uint32_t lot_sequence = (lot << max_sequence_bits) || sequence;
    const auto entropy = build_data({ salt, to_big_endian(lot_sequence) });
    create_token(out_token, passphrase, salt, entropy, prefix::lot_token);
    return true;
}

void encrypt(private_key& out_private, const ec_secret& secret,
    const std::string& passphrase, uint8_t address_version, bool compressed)
{
    const auto point = secret_to_public_key(secret, compressed);
    const auto salt = address_salt(address_version, point);

    two_block derived;
    scrypt_private(derived, normal(passphrase), salt);

    data_chunk derived1;
    data_chunk derived2;
    split(derived1, derived2, derived, two_block_size);

    auto encrypted1 = xor_data(secret, derived1, 0, half);
    aes256_encrypt(derived2, encrypted1);

    auto encrypted2 = xor_data(secret, derived1, half, half);
    aes256_encrypt(derived2, encrypted2);

    const auto prefix = private_key_prefix(address_version,
        prefix::private_key);

    build_checked_array(out_private,
    {
        prefix,
        new_flags(compressed),
        salt,
        encrypted1,
        encrypted2
    });
}

static bool decrypt_multiplied(ec_secret& out_secret,
    const parse_private& parse, const std::string& passphrase,
    uint8_t address_version, bool compressed)
{
    const auto lot = check_flag(parse.flags, flag_byte::lot_sequence);

    ec_secret secret;
    const auto owner_salt = parse.owner_salt();
    scrypt_token(secret, normal(passphrase), owner_salt);

    if (lot)
        secret = bitcoin_hash(build_data({ secret, parse.entropy }));

    const auto point = secret_to_public_key(secret, true);
    const auto salt_entropy = build_data({ parse.salt, parse.entropy });

    two_block derived;
    scrypt_pair(derived, point, salt_entropy);

    data_chunk derived1;
    data_chunk derived2;
    split(derived1, derived2, derived, two_block_size);

    auto encrypted1 = parse.data1;
    auto encrypted2 = parse.data2;

    aes256_decrypt(derived2, encrypted2);
    const auto decrypted2 = xor_data(encrypted2, derived1, 0, half, half);

    data_chunk part1;
    data_chunk part2;
    split(part1, part2, decrypted2, half);
    extend_data(encrypted1, part1);

    aes256_decrypt(derived2, encrypted1);
    const auto decrypted1 = xor_data(encrypted1, derived1, 0, half);

    const auto factor = bitcoin_hash(build_data({ decrypted1, part2 }));
    ec_multiply(secret, factor);

    if (!address_validate(secret, parse.salt, address_version, compressed))
        return false;

    out_secret = secret;
    return true;
}

static bool decrypt_secret(ec_secret& out_secret, const parse_private& parse,
    const std::string& passphrase, uint8_t address_version, bool compressed)
{
    two_block derived;
    scrypt_private(derived, normal(passphrase), parse.salt);

    data_chunk derived1;
    data_chunk derived2;
    split(derived1, derived2, derived, two_block_size);

    auto encrypted1 = build_data({ parse.entropy, parse.data1 });
    auto encrypted2 = parse.data2;

    aes256_decrypt(derived2, encrypted1);
    aes256_decrypt(derived2, encrypted2);

    const auto encrypted = build_data({ encrypted1, encrypted2 });
    const auto decrypted = xor_data(encrypted, derived1, 0, block_size);

    ec_secret secret;
    build_array(secret, { decrypted });

    if (!address_validate(secret, parse.salt, address_version, compressed))
        return false;

    out_secret = secret;
    return true;
}

bool decrypt(ec_secret& out_secret, uint8_t& out_version, bool& compressed,
    const private_key& key, const std::string& passphrase)
{
    // TODO: validate private_key prefix.
    if (!verify_checksum(key))
        return false;
    
    const parse_private parse(key);
    const auto compress = check_flag(parse.flags, flag_byte::ec_compressed);
    const auto multiplied = !check_flag(parse.flags,
        flag_byte::ec_non_multiplied);
    const auto version = address_from_key(parse.prefix[0],
        prefix::private_key);

    const auto success = multiplied ?
        decrypt_multiplied(out_secret, parse, passphrase, version, compress) :
        decrypt_secret(out_secret, parse, passphrase, version, compress);

    if (success)
    {
        compressed = compress;
        out_version = version;
    }

    return success;
}

bool decrypt(ec_point& out_point, uint8_t& out_version, const public_key& key,
    const std::string& passphrase)
{
    // TODO: validate public_key prefix.
    if (!verify_checksum(key))
        return false;

    const parse_public parse(key);
    const auto compressed = check_flag(parse.flags, flag_byte::ec_compressed);
    const auto lot = check_flag(parse.flags, flag_byte::lot_sequence);

    ec_secret factor;
    const auto owner_salt = parse.owner_salt();
    scrypt_token(factor, normal(passphrase), owner_salt);

    if (lot)
        factor = bitcoin_hash(build_data({ factor, parse.entropy }));

    const auto point = secret_to_public_key(factor, true);
    const auto salt_entropy = build_data({ parse.salt, parse.entropy });

    two_block derived;
    scrypt_pair(derived, point, salt_entropy);

    data_chunk derived1;
    data_chunk derived2;
    split(derived1, derived2, derived, two_block_size);

    data_chunk encrypted1;
    data_chunk encrypted2;
    split(encrypted1, encrypted2, parse.data, block_size);

    aes256_decrypt(derived2, encrypted1);
    const auto decrypted1 = xor_data(encrypted1, derived1, 0, half);

    aes256_decrypt(derived2, encrypted2);
    const auto decrypted2 = xor_data(encrypted2, derived1, 0, half, half);

    const auto sign_byte = point_sign(parse.sign, derived2);
    auto generated = build_data({ sign_byte, decrypted1, decrypted2 });

    ec_multiply(generated, factor);
    if (!compressed)
        generated = decompress_public_key(generated);

    const auto version = address_from_key(parse.prefix[0], prefix::public_key);
    if (!address_validate(generated, parse.salt, version, compressed))
        return false;

    out_point = generated;
    out_version = version;
    return true;
}

#endif // WITH_ICU

} // namespace bip38
} // namespace libbitcoin
