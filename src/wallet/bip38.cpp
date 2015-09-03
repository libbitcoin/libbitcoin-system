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
#include <bitcoin/bitcoin/wallet/key_formats.hpp>
#include <bitcoin/bitcoin/wallet/payment_address.hpp>
#include "../math/external/crypto_scrypt.h"

namespace libbitcoin {
namespace bip38 {

// The total length in bytes of the address hash used as a salt in bip38.
static constexpr size_t salt_size = 4;
typedef byte_array<salt_size> salt;

// The total length in bytes of an aes256 encrypted block used in bip38.
static constexpr size_t two_block_size = sizeof(long_hash);
typedef byte_array<two_block_size> two_block;
static_assert(two_block_size == long_hash_size, "oops!");

// The total length in bytes of an aes256 encrypted block used in bip38.
static constexpr size_t block_size = two_block_size / 2;
typedef byte_array<block_size> full_block;

// The half length in bytes of a bip38 encrypted key.
static constexpr size_t block_half_size = block_size / 2;
typedef byte_array<block_half_size> half_block;
static_assert(block_half_size == aes256_block_size, "oops!");

// Scrypt parameters r used in bip38.
static constexpr size_t scrypt_r = 8;
static constexpr size_t scrypt_r_2 = 1;

// Scrypt parameters p used in bip38.
static constexpr size_t scrypt_p = 8;
static constexpr size_t scrypt_p_2 = 1;

// Scrypt parameters N used in bip38.
static constexpr size_t scrypt_N = 16384;
static constexpr size_t scrypt_N_2 = 1024;

// The position of crfm flags used in bip38.
static constexpr size_t cfrm_flag_index = 5;

// Fixed byte ranges used bip38.
static constexpr size_t cfrm_address_hash_start = 6;
static constexpr size_t cfrm_address_hash_end = 10;
static constexpr size_t cfrm_owner_entropy_start = 10;
static constexpr size_t cfrm_owner_entropy_end = 18;
static constexpr size_t cfrm_encrypted_start = 18;
static constexpr size_t cfrm_encrypted_end = 51;
static constexpr size_t salt_index_start = 3;
static constexpr size_t salt_index_end = 7;
static constexpr size_t key_index_start = 7;
static constexpr size_t key_index_end = 39;
static constexpr size_t owner_entropy_start = 7;
static constexpr size_t owner_entropy_end = 15;
static constexpr size_t intm_owner_entropy_start = 8;
static constexpr size_t intm_owner_entropy_end = 16;
static constexpr size_t pass_point_start = 16;
static constexpr size_t pass_point_end = 49;
static constexpr size_t enc_part1_start = 15;
static constexpr size_t enc_part1_end = 23;
static constexpr size_t enc_part2_start = 23;
static constexpr size_t enc_part2_end = 39;

// The fixed bip38 non-multiplied prefix data constants.
static constexpr uint8_t nm_prefix[2] =
{
    0x01, 0x42
};

// The fixed bip38 ec-multiplied prefix data constants.
static constexpr uint8_t m_prefix[2] =
{
    0x01, 0x43
};

// The fixed bip38 magic bytes used when lot is specified.
static constexpr uint8_t magic_w_lot[8] =
{
    0x2C, 0xE9, 0xB3, 0xE1, 0xFF, 0x39, 0xE2, 0x51
};

// The fixed bip38 magic bytes used for confirmation codes.
static constexpr uint8_t confirmation_prefix[5] =
{
    0x64, 0x3B, 0xF6, 0xA8, 0x9A
};

static constexpr size_t flag_index = 2;
enum flag_byte : uint8_t
{
    none = 0,
    lot_sequence = 1 << 2,
    ec_compressed = 1 << 5,
    ec_non_multiplied_low = 1 << 6,
    ec_non_multiplied_high = 1 << 7,
    ec_non_multiplied = (ec_non_multiplied_low | ec_non_multiplied_high)
};

// Dead code?
//static constexpr size_t comp_mult_index = 1;
//static constexpr size_t decrypt_xor_offset = 16;
//static constexpr size_t decrypt_xor_length = 16;

// Dead code?
//// The fixed bip38 magic bytes used when lot is not specified.
//static constexpr uint8_t magic_wo_lot[8] =
//{
//    0x2C, 0xE9, 0xB3, 0xE1, 0xFF, 0x39, 0xE2, 0x53
//};

template <class Type>
static bool scrypt_hash(data_slice passphrase, data_slice salt, Type& output)
{
    return crypto_scrypt(passphrase.data(), passphrase.size(), salt.data(),
        salt.size(), scrypt_N, scrypt_r, scrypt_p, output.data(),
        sizeof(Type)) == 0;
}

template <class Type>
static bool scrypt_hash_2(data_slice pass_point, data_slice salt, Type& output)
{
    return crypto_scrypt(pass_point.data(), pass_point.size(), salt.data(),
        salt.size(), scrypt_N_2, scrypt_r_2, scrypt_p_2, output.data(),
        sizeof(Type)) == 0;
}

template <class Type>
static data_chunk chop(const Type& key, size_t begin, size_t end)
{
    const auto& key_data = key.data();
    return data_chunk
    {
        &key_data[begin], &key_data[end]
    };
}

static inline uint8_t generate_flag_byte(bool compressed, bool multiplied)
{
    uint8_t byte = flag_byte::none;

    if (compressed)
        byte |= flag_byte::ec_compressed;

    if (!multiplied)
        byte |= flag_byte::ec_non_multiplied;

    return byte;
}

static uint8_t get_flag_byte(data_slice key)
{
    BITCOIN_ASSERT(flag_index < key.size());
    return key.data()[flag_index];
}

static uint8_t get_cfrm_flag_byte(data_slice key)
{
    BITCOIN_ASSERT(cfrm_flag_index < key.size());
    return key.data()[cfrm_flag_index];
}

static bool is_compressed(data_slice key)
{
    return ((get_flag_byte(key) & flag_byte::ec_compressed) != 0);
}

static bool is_cfrm_compressed(data_slice key)
{
    return ((get_cfrm_flag_byte(key) & flag_byte::ec_compressed) != 0);
}

static bool has_lot_seq(data_slice key)
{
    return ((get_flag_byte(key) & flag_byte::lot_sequence) != 0);
}

static bool cfrm_has_lot_seq(data_slice key)
{
    return ((get_cfrm_flag_byte(key) & flag_byte::lot_sequence) != 0);
}

static bool is_ec_non_multiplied(data_slice key)
{
    return ((get_flag_byte(key) & flag_byte::ec_non_multiplied) != 0);
}

static bool is_ec_multiplied(data_slice key)
{
    return !is_ec_non_multiplied(key);
}

uint8_t last_byte(data_slice buffer)
{
    return buffer.data()[buffer.size() - 1];
}

data_chunk normal(const std::string& passphrase)
{
    return to_data_chunk(to_normal_nfc_form(passphrase));
}

static data_chunk xor_offset(data_slice buffer1, data_slice buffer2,
    size_t buffer1_offset, size_t buffer2_offset, size_t length)
{
    data_chunk out;
    const auto& data1 = buffer1.data();
    const auto& data2 = buffer2.data();
    for(size_t i = 0; i < length; i++)
        out.push_back(data1[i + buffer1_offset] ^ data2[i + buffer2_offset]);

    return out;
}

static data_chunk xor_(data_slice buffer1, data_slice buffer2,
    size_t offset, size_t length)
{
    return xor_offset(buffer1, buffer2, offset, offset, length);
}

static void generate_confirmation(uint8_t flag_byte,
    data_slice address_hash_owner_entropy, const ec_secret& factorb,
    data_slice derived_half1, data_slice derived_half2,
    confirmation_code& confirmation_code, bool use_compression)
{
    const auto pointb = secret_to_public_key(factorb, use_compression);
    const auto pointb_data = to_data_chunk(pointb);
    auto half1 = xor_offset(pointb_data, derived_half1, 1, 0, block_half_size);

    aes256_encrypt(derived_half2, half1);

    // TODO: magic numbers: 17, 16?
    auto half2 = xor_offset(pointb_data, derived_half1, 17, 16,
        block_half_size);
    aes256_encrypt(derived_half2, half2);

    // TODO: magic flag: 0x01?
    uint8_t pointb_prefix = pointb[0] ^ (last_byte(derived_half2) & 0x01);
    data_chunk encrypted_pointb;
    encrypted_pointb.push_back(pointb_prefix);
    extend_data(encrypted_pointb, half1);
    extend_data(encrypted_pointb, half2);

    data_chunk confirmation;
    for (const auto& prefix: confirmation_prefix)
        confirmation.push_back(prefix);

    confirmation.push_back(flag_byte);
    extend_data(confirmation, address_hash_owner_entropy);
    extend_data(confirmation, encrypted_pointb);
    append_checksum(confirmation);

    // TODO: this copy can be avoided.
    BITCOIN_ASSERT(confirmation.size() == confirmation_code_decoded_size);
    std::copy(confirmation.begin(), confirmation.end(),
        confirmation_code.begin());
}

#ifdef WITH_ICU

data_chunk lock_intermediate(const intermediate& intermediate,
    const seed& seed, confirmation_code& out_confirmation_code,
    bool use_compression)
{
    const auto uses_lot_sequence = std::equal(intermediate.begin(),
        intermediate.begin() + sizeof(magic_w_lot), magic_w_lot);

    static const auto multiplied = true;
    auto flag_byte = generate_flag_byte(use_compression, multiplied);
    if (uses_lot_sequence)
        flag_byte |= lot_sequence;

    auto owner_entropy = chop(intermediate, intm_owner_entropy_start,
        intm_owner_entropy_end);
    auto pass_point_data = chop(intermediate, pass_point_start,
        pass_point_end);

    const ec_secret factorb(bitcoin_hash(seed));

    auto generated_point = pass_point_data;
    ec_multiply(generated_point, factorb);
    if (!use_compression)
        generated_point = decompress_public_key(generated_point);

    wallet::payment_address address;
    address.set_public_key(generated_point);
    const auto hash = bitcoin_hash(to_data_chunk(address.to_string()));

    auto address_hash_owner_entropy = chop(hash, 0, salt_size);
    extend_data(address_hash_owner_entropy, owner_entropy);

    two_block derived;
    if (!scrypt_hash_2(pass_point_data, address_hash_owner_entropy, derived))
        return data_chunk();

    data_chunk derived_half1;
    data_chunk derived_half2;
    split(derived, derived_half1, derived_half2, two_block_size);

    auto half1 = xor_(seed, derived_half1, 0, block_half_size);
    aes256_encrypt(derived_half2, half1);

    // TODO: magic number: 16, 16, 24?
    auto combined_data = chop(half1, 8, 16);
    const auto seed_data = chop(seed, 16, 24);
    extend_data(combined_data, seed_data);

    // TODO: magic number: 16?
    auto half2 = xor_offset(combined_data, derived_half1, 0, 16,
        block_half_size);
    aes256_encrypt(derived_half2, half2);

    // TODO: magic number: 8?
    auto encrypted_data = chop(half1, 0, 8);
    extend_data(encrypted_data, half2);

    data_chunk encrypted_key;
    encrypted_key.push_back(m_prefix[0]);
    encrypted_key.push_back(m_prefix[1]);
    encrypted_key.push_back(flag_byte);
    extend_data(encrypted_key, address_hash_owner_entropy);
    extend_data(encrypted_key, encrypted_data);
    append_checksum(encrypted_key);

    generate_confirmation(flag_byte, address_hash_owner_entropy, factorb,
        derived_half1, derived_half2, out_confirmation_code, use_compression);

    return encrypted_key;
}

bool lock_verify(const confirmation_code& confirmation_code,
    const std::string& passphrase, wallet::payment_address& out_address)
{
    const bool compressed = is_cfrm_compressed(confirmation_code);
    const bool uses_lot_seq = cfrm_has_lot_seq(confirmation_code);

    auto address_hash = chop(confirmation_code, cfrm_address_hash_start,
        cfrm_address_hash_end);
    auto owner_entropy = chop(confirmation_code, cfrm_owner_entropy_start,
        cfrm_owner_entropy_end);
    auto encrypted_pointb = chop(confirmation_code, cfrm_encrypted_start + 1,
        cfrm_encrypted_end);

    auto owner_salt_end = salt_size;
    if (uses_lot_seq)
        owner_salt_end = owner_entropy.size();

    auto owner_salt = chop(owner_entropy, 0, owner_salt_end);

    hash_digest pre_factor;
    if (!scrypt_hash(normal(passphrase), owner_salt, pre_factor))
        return false;

    ec_secret pass_factor;
    if (uses_lot_seq)
    {
        auto extended_pre_factor = to_data_chunk(pre_factor);
        extend_data(extended_pre_factor, owner_entropy);
        pre_factor = bitcoin_hash(extended_pre_factor);
    }
    
    pass_factor = ec_secret(pre_factor);
    static const auto compress = true;
    const auto pass_point = secret_to_public_key(pass_factor, compress);
    auto address_hash_owner_entropy = address_hash;
    extend_data(address_hash_owner_entropy, owner_entropy);

    two_block derived_data;
    if (!scrypt_hash_2(pass_point, address_hash_owner_entropy, derived_data))
        return false;

    data_chunk derived_half1;
    data_chunk derived_half2;
    split(derived_data, derived_half1, derived_half2, two_block_size);

    data_chunk encrypted_half1;
    data_chunk encrypted_half2;
    split(encrypted_pointb, encrypted_half1, encrypted_half2, block_size);

    aes256_decrypt(derived_half2, encrypted_half1);
    auto decrypted_half1 = xor_(encrypted_half1, derived_half1, 0,
        block_half_size);

    // TODO: magic number: 16?
    aes256_decrypt(derived_half2, encrypted_half2);
    auto decrypted_half2 = xor_offset(encrypted_half2, derived_half1, 0, 16,
        block_half_size);

    // TODO: magic flag: 0x01?
    data_chunk pointb_data;
    const auto pointb_sign = confirmation_code[cfrm_encrypted_start];
    pointb_data.push_back(pointb_sign ^ (last_byte(derived_half2) & 0x01));
    extend_data(pointb_data, decrypted_half1);
    extend_data(pointb_data, decrypted_half2);

    auto pointb = pointb_data;
    ec_multiply(pointb, pass_factor);
    const auto generated = (compressed ? pointb : decompress_public_key(pointb));

    wallet::payment_address address;
    address.set_public_key(generated);
    const auto hash = bitcoin_hash(to_data_chunk(address.to_string()));
    if (!std::equal(address_hash.begin(), address_hash.end(), hash.begin()))
        return false;

    out_address = address;
    return true;
}

data_chunk lock_secret(const ec_secret& secret, const std::string& passphrase,
    bool use_compression)
{
    const auto public_key = secret_to_public_key(secret, use_compression);
    wallet::payment_address address;
    address.set_public_key(public_key);

    const auto address_hash = bitcoin_hash(to_data_chunk(address.to_string()));
    auto salt = chop(address_hash, 0, salt_size);

    two_block derived_data;
    if (!scrypt_hash(normal(passphrase), salt, derived_data))
        return data_chunk();

    data_chunk derived_half1;
    data_chunk derived_half2;
    split(derived_data, derived_half1, derived_half2, two_block_size);

    auto half1 = xor_(secret, derived_half1, 0, block_half_size);
    auto half2 = xor_(secret, derived_half1, block_half_size, block_half_size);

    aes256_encrypt(derived_half2, half1);
    aes256_encrypt(derived_half2, half2);

    data_chunk encrypted_key;
    encrypted_key.push_back(nm_prefix[0]);
    encrypted_key.push_back(nm_prefix[1]);
    const auto multiplied = false;
    encrypted_key.push_back(generate_flag_byte(use_compression, multiplied));

    const auto salt_end = salt.begin() + salt_size;
    encrypted_key.insert(encrypted_key.end(), salt.begin(), salt_end);
    extend_data(encrypted_key, half1);
    extend_data(encrypted_key, half2);
    append_checksum(encrypted_key);

    return encrypted_key;
}

static ec_secret unlock_ec_multiplied_secret(const encrypted_private_key& key,
    const std::string& passphrase)
{
    const bool compressed = is_compressed(key);
    const bool uses_lot_seq = has_lot_seq(key);
    auto owner_salt_end = owner_entropy_end;
    if (uses_lot_seq)
        owner_salt_end = owner_entropy_start + salt_size;

    const auto address_hash = chop(key, salt_index_start, salt_index_end);
    const auto entropy = chop(key, owner_entropy_start, owner_entropy_end);
    const auto owner_salt = chop(key, owner_entropy_start, owner_salt_end);
    auto encrypted_part1 = chop(key, enc_part1_start, enc_part1_end);
    auto encrypted_part2 = chop(key, enc_part2_start, enc_part2_end);

    hash_digest pre_factor;
    if (!scrypt_hash(normal(passphrase), owner_salt, pre_factor))
        return ec_secret();

    ec_secret pass_factor;
    if (uses_lot_seq)
    {
        auto extended_pre_factor = to_data_chunk(pre_factor);
        extend_data(extended_pre_factor, entropy);
        pre_factor = bitcoin_hash(extended_pre_factor);
    }

    pass_factor = ec_secret(pre_factor);
    const auto compress = true;
    const auto pass_point = secret_to_public_key(pass_factor, compress);
    data_chunk address_hash_owner_entropy = address_hash;
    extend_data(address_hash_owner_entropy, entropy);

    two_block seedb_pass;
    if (!scrypt_hash_2(pass_point, address_hash_owner_entropy, seedb_pass))
        return ec_secret();

    data_chunk derived_half1;
    data_chunk derived_half2;
    split(seedb_pass, derived_half1, derived_half2, two_block_size);

    aes256_decrypt(derived_half2, encrypted_part2);
    const auto& decrypted_part2 = encrypted_part2;
    auto xor_seedb = xor_offset(decrypted_part2, derived_half1, 0,
        block_half_size, block_half_size);

    data_chunk remaining_part1;
    data_chunk seedb_part2;
    split(xor_seedb, remaining_part1, seedb_part2, block_half_size);

    extend_data(encrypted_part1, remaining_part1);
    aes256_decrypt(derived_half2, encrypted_part1);
    const auto& decrypted_part1 = encrypted_part1;

    auto seedb_part1 = xor_(decrypted_part1, derived_half1, 0, block_half_size);
    auto& seedb = seedb_part1;
    extend_data(seedb, seedb_part2);

    const auto factorb = ec_secret(bitcoin_hash(seedb));
    ec_multiply(pass_factor, factorb);

    const auto& unlocked_key = pass_factor;
    const auto public_key = secret_to_public_key(unlocked_key, compressed);

    wallet::payment_address address;
    address.set_public_key(public_key);
    const auto hash = bitcoin_hash(to_data_chunk(address.to_string()));
    const auto hash_end = hash.begin() + address_hash.size();
    if (std::equal(hash.begin(), hash_end, address_hash.begin()))
        return unlocked_key;

    return ec_secret();
}

ec_secret unlock_secret(const encrypted_private_key& encrypted_secret,
    const std::string& passphrase)
{
    ec_secret secret;
    if (is_ec_multiplied(encrypted_secret))
        return unlock_ec_multiplied_secret(encrypted_secret, passphrase);

    BITCOIN_ASSERT(is_ec_non_multiplied(encrypted_secret));

    const auto salt = chop(encrypted_secret, salt_index_start, salt_index_end);
    const auto encrypted_data = chop(encrypted_secret, key_index_start,
        key_index_end);

    two_block derived_data;
    if (!scrypt_hash(normal(passphrase), salt, derived_data))
        return ec_secret();

    data_chunk data_half1;
    data_chunk data_half2;
    split(encrypted_data, data_half1, data_half2, block_size);

    data_chunk derived_half1;
    data_chunk derived_half2;
    split(derived_data, derived_half1, derived_half2, two_block_size);

    aes256_decrypt(derived_half2, data_half1);
    aes256_decrypt(derived_half2, data_half2);

    data_chunk combined_data;
    extend_data(combined_data, data_half1);
    extend_data(combined_data, data_half2);

    const auto decrypted = xor_(combined_data, derived_half1, 0, block_size);
    std::copy(decrypted.begin(), decrypted.end(), secret.begin());

    const auto compressed = is_compressed(encrypted_secret);
    const auto public_key = secret_to_public_key(secret, compressed);

    wallet::payment_address address;
    address.set_public_key(public_key);
    const auto hash = bitcoin_hash(to_data_chunk(address.to_string()));
    if (std::equal(salt.begin(), salt.end(), hash.begin()))
        return secret;

    return ec_secret();
}

#endif

} // namespace bip38
} // namespace libbitcoin

