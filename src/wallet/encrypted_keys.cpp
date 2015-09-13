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
#include <bitcoin/bitcoin/wallet/encrypted_keys.hpp>

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
#include <bitcoin/bitcoin/wallet/payment_address.hpp>
#include <bitcoin/bitcoin/wallet/wif_keys.hpp>

namespace libbitcoin {
namespace wallet {

static constexpr size_t mainnet = 0x00;
static constexpr size_t max_sequence_bits = 12;
static constexpr size_t half = hash_size / 2;
static constexpr size_t quarter = half / 2;

typedef byte_array<half> half_hash;
typedef byte_array<quarter> quarter_hash;

// The above sizes are all tied to aes256.
static_assert(2 * quarter == bc::aes256_block_size, "oops!");

// BIP38
// It is requested that the unused flag bytes NOT be used for denoting that
// the key belongs to an alt-chain [This shoud read "flag bits"].
enum ek_flag : uint8_t
{
    none = 0,
    lot_sequence = 1 << 2,
    ec_compressed = 1 << 5,
    ec_non_multiplied_low = 1 << 6,
    ec_non_multiplied_high = 1 << 7,

    // Two bits are used to represent "not multiplied".
    ec_non_multiplied = (ec_non_multiplied_low | ec_non_multiplied_high)
};

static inline bool is_set(uint8_t flags, ek_flag flag)
{
    return (flags & flag) != 0;
}

// BIP38
// Alt-chain implementers should exploit the address hash for [identification].
// Since each operation in this proposal involves hashing a text representation
// of a coin address which (for Bitcoin) includes the leading '1', an alt-chain
// can easily be denoted simply by using the alt-chain's preferred format for
// representing an address.... Alt-chain implementers may also change the prefix
// such that encrypted addresses do not start with "6P".

template<uint8_t Check, size_t Size>
class parse_prefix
{
public:
    parse_prefix(const byte_array<Size>& value)
      : version_(slice<0, 1>(value)),
        magic_(slice<1, Size - 1>(value)),
        context_(slice<Size - 1, Size>(value)),
        valid_(verify_version())
    {
    }

    uint8_t context() const
    {
        return context_.front();
    }

    byte_array<Size> prefix() const
    {
        return splice(version_, magic_, context_);
    }

    uint8_t prefix_version() const
    {
        return version_.front();
    }

    bool valid() const
    {
        return valid_;
    }

    void valid(bool value)
    {
        valid_ = value;
    }

    static constexpr uint8_t prefix_size = Size;
    static constexpr uint8_t default_check = Check;
    static constexpr uint8_t magic_size = prefix_size - 2;

private:
    bool verify_version()
    {
        return true;
    }

    const one_byte version_;
    const byte_array<magic_size> magic_;
    const one_byte context_;
    bool valid_;
};

template<uint8_t Check, size_t Size>
class parse_key
  : public parse_prefix<Check, Size>
{
public:
    parse_key(const byte_array<Size>& prefix, const one_byte& flags,
        const wallet::salt& salt, const wallet::entropy& entropy)
      : parse_prefix<Check, Size>(prefix),
        flags_(flags), salt_(salt), entropy_(entropy)
    {
    }

    bool compressed() const
    {
        return is_set(flags(), ek_flag::ec_compressed);
    }

    /// The owner salt + lot-sequence or owner entropy.
    wallet::entropy entropy() const
    {
        return entropy_;
    }

    uint8_t flags() const
    {
        return flags_.front();
    }

    bool lot_sequence() const
    {
        return is_set(flags(), ek_flag::lot_sequence);
    }

    /// Either 4 or 8 bytes, depending on the lot sequence flags.
    data_chunk owner_salt() const
    {
        if (lot_sequence())
            return to_chunk(slice<0, salt_size>(entropy()));
        else
            return to_chunk(entropy());
    }

    /// The address hash salt.
    wallet::salt salt() const
    {
        return salt_;
    }

private:
    const one_byte flags_;
    const wallet::salt salt_;
    const wallet::entropy entropy_;
};

class parse_private
  : public parse_key<0x01, 2>
{
public:
    static byte_array<prefix_size> prefix(uint8_t address, bool multiplied)
    {
        const auto check = address == mainnet ? default_check : address;
        const auto context = multiplied ? multiplied_context : default_context;
        return splice(to_array(check), to_array(context));
    }

    parse_private(const private_key& key)
      : parse_key<default_check, prefix_size>(
            slice<0, 2>(key),
            slice<2, 3>(key),
            slice<3, 7>(key),
            slice<7, 15>(key)),
        data1_(slice<15, 23>(key)),
        data2_(slice<23, 39>(key))
    {
        valid(valid() && verify_context() && verify_flags() &&
            verify_checksum(key));
    }

    uint8_t address_version() const
    {
        const auto check = prefix_version();
        return check == default_check ? mainnet : check;
    }

    bool multiplied() const
    {
        return !is_set(flags(), ek_flag::ec_non_multiplied);
    }

    quarter_hash data1() const
    {
        return data1_;
    }

    half_hash data2() const
    {
        return data2_;
    }

private:
    bool verify_flags() const
    {
        // This guards against a conflict that can result from a redundancy in
        // the BIP38 specification - multiplied context exists in two places.
        return multiplied() == (context() == multiplied_context);
    }

    bool verify_context() const
    {
        return context() == default_context || context() == multiplied_context;
    }

    static constexpr uint8_t default_context = 0x42;
    static constexpr uint8_t multiplied_context = 0x43;

    const quarter_hash data1_;
    const half_hash data2_;
};

class parse_public
  : public parse_key<0x64, 5>
{
public:
    static byte_array<prefix_size> prefix(uint8_t address)
    {
        const auto check = address == mainnet ? default_check : address;
        return splice(to_array(check), public_magic, to_array(only_context));
    }

    parse_public(const public_key& key)
      : parse_key<default_check, prefix_size>(
            slice<0, 5>(key),
            slice<5, 6>(key),
            slice<6, 10>(key),
            slice<10, 18>(key)),
        sign_(slice<18, 19>(key)),
        data_(slice<19, 51>(key))
    {
        valid(valid() && verify_context() && verify_checksum(key));
    }

    uint8_t address_version() const
    {
        const auto check = prefix_version();
        return check == default_check ? mainnet : check;
    }

    hash_digest data() const
    {
        return data_;
    }

    one_byte sign() const
    {
        return sign_;
    }

private:
    bool verify_context() const
    {
        return context() == only_context;
    }

    static constexpr uint8_t only_context = 0x9a;
    static const byte_array<magic_size> public_magic;

    const one_byte sign_;
    const hash_digest data_;
};

// This prefix results in the prefix "cfrm" in the base58 encoding but is
// modified when the payment address is Bitcoin mainnet (0).
const byte_array<parse_public::magic_size> parse_public::public_magic
{
    { 0x3b, 0xf6, 0xa8 }
};

class parse_token
  : public parse_prefix<0x2c, 8>
{
public:
    static byte_array<prefix_size> prefix(bool lot_sequence)
    {
        const auto context = lot_sequence ? lot_context : default_context;
        return splice(to_array(default_check), token_magic,
            to_array(context));
    }

    parse_token(const token& value)
      : parse_prefix(slice<0, 8>(value)),
        entropy_(slice<8, 16>(value)),
        sign_(slice<16, 17>(value)),
        data_(slice<17, 49>(value))
    {
        valid(valid() && verify_context() && verify_checksum(value));
    }

    uint8_t address_version() const
    {
        return mainnet;
    }

    hash_digest data() const
    {
        return data_;
    }

    wallet::entropy entropy() const
    {
        return entropy_;
    }

    bool lot_sequence() const
    {
        // Note: there is no "flags" byte in token, we rely on prefix context.
        return context() == lot_context;
    }

    one_byte sign() const
    {
        return sign_;
    }

private:
    bool verify_context() const
    {
        return context() == default_context || context() == lot_context;
    }

    static constexpr uint8_t lot_context = 0x51;
    static constexpr uint8_t default_context = 0x53;
    static const byte_array<magic_size> token_magic;

    const wallet::entropy entropy_;
    const one_byte sign_;
    const hash_digest data_;
};

// This prefix results in the prefix "passphrase" in the base58 encoding.
// The prefix is not modified as the result of variations to address.
const byte_array<parse_token::magic_size> parse_token::token_magic
{
    { 0xe9, 0xb3, 0xe1, 0xff, 0x39, 0xe2 } 
};

// Flags
// ----------------------------------------------------------------------------

static one_byte set_flags(bool compressed, bool lot_sequence, bool multiplied)
{
    uint8_t byte = ek_flag::none;

    if (compressed)
        byte |= ek_flag::ec_compressed;

    if (lot_sequence)
        byte |= ek_flag::lot_sequence;

    if (!multiplied)
        byte |= ek_flag::ec_non_multiplied;

    return to_array(byte);
}

static one_byte set_flags(bool compressed, bool lot_sequence)
{
    return set_flags(compressed, lot_sequence, false);
}

static one_byte set_flags(bool compressed)
{
    return set_flags(compressed, false);
}

// Address
// ----------------------------------------------------------------------------

static hash_digest address_hash(uint8_t version, const ec_point& point)
{
    const payment_address address(version, point);
    return bitcoin_hash(to_chunk(address.to_string()));
}

static salt address_salt(uint8_t version, const ec_point& point)
{
    return slice<0, salt_size>(address_hash(version, point));
}

static bool address_validate(const ec_point& point, const wallet::salt& salt,
    uint8_t version, bool compressed)
{
    const auto hash = address_hash(version, point);
    return std::equal(hash.begin(), hash.begin() + salt.size(), salt.begin());
}

static bool address_validate(const ec_secret& secret, const wallet::salt& salt,
    uint8_t version, bool compressed)
{
    const auto point = secret_to_public_key(secret, compressed);
    return address_validate(point, salt, version, compressed);
}

// Point
// ----------------------------------------------------------------------------

static hash_digest point_hash(const ec_point& compressed_point)
{
    static constexpr size_t compressed_size = 1 + hash_size;
    BITCOIN_ASSERT(compressed_point.size() == compressed_size);
    const auto point_array = to_array<compressed_size>(compressed_point);
    return slice<1, compressed_size>(point_array);
}

static one_byte point_sign(uint8_t byte, const hash_digest& hash)
{
    static constexpr uint8_t low_bit_mask = 0x01;
    const uint8_t last_byte = hash[hash_size - 1];
    const uint8_t last_byte_odd_field = last_byte & low_bit_mask;
    const uint8_t sign_byte = byte ^ last_byte_odd_field;
    return to_array(sign_byte);
}

static one_byte point_sign(const one_byte& single, const hash_digest& hash)
{
    return point_sign(single.front(), hash);
}

static ec_point to_point(const one_byte& sign, const hash_digest& hash)
{
    return build_chunk({ sign, hash });
}

// Scrypt
// ----------------------------------------------------------------------------

static hash_digest scrypt_token(data_slice data, data_slice salt)
{
    // Arbitrary scrypt parameters from BIP-38.
    return scrypt<hash_size>(data, salt, 16384u, 8u, 8u);
}

static long_hash scrypt_pair(data_slice data, data_slice salt)
{
    // Arbitrary scrypt parameters from BIP-38.
    return scrypt<long_hash_size>(data, salt, 1024u, 1u, 1u);
}

static long_hash scrypt_private(data_slice data, data_slice salt)
{
    // Arbitrary scrypt parameters from BIP-38.
    return scrypt<long_hash_size>(data, salt, 16384u, 8u, 8u);
}

// create_key_pair
// ----------------------------------------------------------------------------

static void create_private_key(private_key& out_private, const one_byte& flags,
    const salt& salt, const entropy& entropy, const hash_digest& derived1,
    const hash_digest& derived2, const seed& seed, uint8_t version)
{
    const auto prefix = parse_private::prefix(version, true);

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

static void create_public_key(public_key& out_public, const one_byte& flags,
    const salt& salt, const entropy& entropy, const hash_digest& derived1,
    const hash_digest& derived2, const ec_secret& secret, uint8_t version)
{
    const auto prefix = parse_public::prefix(version);
    const auto point = secret_to_public_key(secret, true);
    const auto hash = point_hash(point);

    auto encrypted1 = xor_data<half>(hash, derived1);
    aes256_encrypt(derived2, encrypted1);

    auto encrypted2 = xor_data<half>(hash, derived1, half);
    aes256_encrypt(derived2, encrypted2);

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
    const parse_token parse(token);
    if (!parse.valid())
        return false;

    const auto point = to_point(parse.sign(), parse.data());
    out_point = point;
    const auto factor = bitcoin_hash(seed);
    ec_multiply(out_point, factor);

    if (!compressed)
        out_point = decompress_public_key(out_point);

    const auto salt = address_salt(version, out_point);
    const auto salt_entropy = splice(salt, parse.entropy());
    const auto derived = split(scrypt_pair(point, salt_entropy));
    const auto flags = set_flags(compressed, parse.lot_sequence(), true);

    create_private_key(out_private, flags, salt, parse.entropy(), derived.left,
        derived.right, seed, version);
    create_public_key(out_public, flags, salt, parse.entropy(), derived.left,
        derived.right, factor, version);

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

// create_token
// ----------------------------------------------------------------------------

// This call requires an ICU build, the other excluded calls are dependencies.
static data_chunk normal(const std::string& passphrase)
{
    return to_chunk(to_normal_nfc_form(passphrase));
}

static void create_token(token& out_token, const std::string& passphrase,
    data_slice owner_salt, const wallet::entropy& owner_entropy,
    const byte_array<parse_token::prefix_size>& prefix)
{
    BITCOIN_ASSERT(owner_salt.size() == salt_size ||
        owner_salt.size() == entropy_size);

    const auto lot_sequence = owner_salt.size() == salt_size;
    auto factor = scrypt_token(normal(passphrase), owner_salt);

    if (lot_sequence)
        factor = bitcoin_hash(splice(factor, owner_entropy));

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
    const wallet::entropy& entropy)
{
    // BIP38: If lot and sequence numbers are not being included, then
    // owner_salt is 8 random bytes instead of 4, lot_sequence is omitted and
    // owner_entropy becomes an alias for owner_salt.
    const auto prefix = parse_token::prefix(false);
    create_token(out_token, passphrase, entropy, entropy, prefix);
}

// The salt here is owner-supplied random bits, not the address hash.
bool create_token(token& out_token, const std::string& passphrase,
    const wallet::salt& salt, uint32_t lot, uint32_t sequence)
{
    if (lot > max_token_lot || sequence > max_token_sequence)
        return false;

    const auto prefix = parse_token::prefix(true);
    const uint32_t lot_sequence = (lot << max_sequence_bits) || sequence;
    const auto entropy = splice(salt, to_big_endian(lot_sequence));
    create_token(out_token, passphrase, salt, entropy, prefix);
    return true;
}

// encrypt
// ----------------------------------------------------------------------------

void encrypt(private_key& out_private, const ec_secret& secret,
    const std::string& passphrase, uint8_t version, bool compressed)
{
    const auto prefix = parse_private::prefix(version, false);
    const auto point = secret_to_public_key(secret, compressed);
    const auto salt = address_salt(version, point);
    const auto derived = split(scrypt_private(normal(passphrase), salt));

    auto encrypted1 = xor_data<half>(secret, derived.left);
    aes256_encrypt(derived.right, encrypted1);

    auto encrypted2 = xor_data<half>(secret, derived.left, half);
    aes256_encrypt(derived.right, encrypted2);

    build_checked_array(out_private,
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
    const parse_private& parse, const std::string& passphrase)
{
    auto secret = scrypt_token(normal(passphrase), parse.owner_salt());

    if (parse.lot_sequence())
        secret = bitcoin_hash(splice(secret, parse.entropy()));

    const auto point = secret_to_public_key(secret, true);
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
    ec_multiply(secret, factor);

    const auto compressed = parse.compressed();
    const auto address_version = parse.address_version();
    if (!address_validate(secret, parse.salt(), address_version, compressed))
        return false;

    out_secret = secret;
    return true;
}

static bool decrypt_secret(ec_secret& out_secret, const parse_private& parse,
    const std::string& passphrase)
{
    const auto derived = split(scrypt_private(normal(passphrase), parse.salt()));
    auto encrypt1 = splice(parse.entropy(), parse.data1());
    auto encrypt2 = parse.data2();

    aes256_decrypt(derived.right, encrypt1);
    aes256_decrypt(derived.right, encrypt2);

    const auto encrypted = splice(encrypt1, encrypt2);
    const auto secret = xor_data<hash_size>(encrypted, derived.left);

    const auto compressed = parse.compressed();
    const auto address_version = parse.address_version();
    if (!address_validate(secret, parse.salt(), address_version, compressed))
        return false;

    out_secret = secret;
    return true;
}

bool decrypt(ec_secret& out_secret, uint8_t& out_version, bool& compressed,
    const private_key& key, const std::string& passphrase)
{
    const parse_private parse(key);
    if (!parse.valid())
        return false;

    const auto success = parse.multiplied() ?
        decrypt_multiplied(out_secret, parse, passphrase) :
        decrypt_secret(out_secret, parse, passphrase);

    if (success)
    {
        compressed = parse.compressed();
        out_version = parse.address_version();
    }

    return success;
}

// decrypt public_key
// ----------------------------------------------------------------------------

bool decrypt(ec_point& out_point, uint8_t& out_version, const public_key& key,
    const std::string& passphrase)
{
    const parse_public parse(key);
    if (!parse.valid())
        return false;

    const auto version = parse.address_version();
    const auto compressed = parse.compressed();
    const auto lot_sequence = parse.lot_sequence();
    auto factor = scrypt_token(normal(passphrase), parse.owner_salt());

    if (lot_sequence)
        factor = bitcoin_hash(splice(factor, parse.entropy()));

    const auto point = secret_to_public_key(factor, true);
    const auto salt_entropy = splice(parse.salt(), parse.entropy());
    auto derived = split(scrypt_pair(point, salt_entropy));
    auto encrypt = split(parse.data());

    aes256_decrypt(derived.right, encrypt.left);
    const auto decrypt1 = xor_data<half>(encrypt.left, derived.left);

    aes256_decrypt(derived.right, encrypt.right);
    const auto decrypt2 = xor_data<half>(encrypt.right, derived.left, 0, half);

    const auto sign_byte = point_sign(parse.sign(), derived.right);
    auto generated = build_chunk({ sign_byte, decrypt1, decrypt2 });

    ec_multiply(generated, factor);
    if (!compressed)
        generated = decompress_public_key(generated);

    if (!address_validate(generated, parse.salt(), version, compressed))
        return false;

    out_point = generated;
    out_version = version;
    return true;
}

#endif // WITH_ICU

} // namespace wallet
} // namespace libbitcoin
