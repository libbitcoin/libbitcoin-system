/*
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
#include <algorithm>
#include <boost/locale.hpp>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/math/checksum.hpp>
#include <bitcoin/bitcoin/math/crypto.hpp>
#include <bitcoin/bitcoin/math/hash.hpp>
#include <bitcoin/bitcoin/math/ec_keys.hpp>
#include <bitcoin/bitcoin/formats/base16.hpp>
#include <bitcoin/bitcoin/formats/base58.hpp>
#include <bitcoin/bitcoin/utility/assert.hpp>
#include <bitcoin/bitcoin/utility/data.hpp>
#include <bitcoin/bitcoin/wallet/key_formats.hpp>
#include <bitcoin/bitcoin/wallet/payment_address.hpp>
#include "../math/external/crypto_scrypt.h"


namespace libbitcoin {
namespace bip38 {

static inline uint8_t generate_flag_byte(
    bool compressed, bool ec_multiplied)
{
    return ((compressed ? bip38_compressed : 0x00) |
        (ec_multiplied ? bip38_ec_multiplied : bip38_ec_non_multiplied));
}

static inline uint8_t get_flag_byte(const encrypted_private_key& key)
{
    return key[bip38_flag_index];
}

static inline uint8_t get_cfrm_flag_byte(const encrypted_private_key& key)
{
    return key[bip38_cfrm_flag_index];
}

static inline bool is_compressed(const encrypted_private_key& key)
{
    return ((get_flag_byte(key) & bip38_compressed) != 0);
}

static inline bool is_cfrm_compressed(const data_chunk& key)
{
    return ((get_cfrm_flag_byte(key) & bip38_compressed) != 0);
}

static inline bool has_lot_seq(const encrypted_private_key& key)
{
    return ((get_flag_byte(key) & bip38_lot_sequence) != 0);
}

static inline bool cfrm_has_lot_seq(const data_chunk& key)
{
    return ((get_cfrm_flag_byte(key) & bip38_lot_sequence) != 0);
}

static inline bool is_bip38_ec_non_multiplied(
    const encrypted_private_key& key)
{
    return ((get_flag_byte(key) & bip38_ec_non_multiplied) != 0);
}

static inline bool is_bip38_ec_multiplied(
    const encrypted_private_key& key)
{
    return !is_bip38_ec_non_multiplied(key);
}

#ifdef WITH_ICU
static std::string normalize_nfc(const std::string& value)
{
    using namespace boost::locale;
    const generator locale;
    return normalize(value, norm_type::norm_nfc, locale(""));
}
#endif

template<class T>
static bool bip38_scrypt_hash(const data_chunk passphrase,
    data_slice salt, T& output)
{
    return crypto_scrypt(passphrase.data(), passphrase.size(),
        salt.data(), salt.size(), bip38_scrypt_N, bip38_scrypt_r,
        bip38_scrypt_p, output.data(), sizeof(T)) == 0;
}

template<class T>
static bool bip38_scrypt_hash_2(const ec_point& pass_point,
    data_slice salt, T& output)
{
    return crypto_scrypt(pass_point.data(), pass_point.size(),
        salt.data(), salt.size(), bip38_scrypt_N_2,
        bip38_scrypt_r_2, bip38_scrypt_p_2,
        output.data(), sizeof(T)) == 0;
}

static inline data_chunk xor_data_offset(
    const data_chunk& in1, const data_chunk& in2,
    size_t in1_offset, size_t in2_offset, size_t length)
{
    data_chunk out;

    for(size_t i = 0; i < length; i++)
        out.push_back(in1[i+in1_offset] ^ in2[i+in2_offset]);

    return out;
}

static inline data_chunk xor_data(
    const data_chunk& in1, const data_chunk& in2,
    size_t offset, size_t length)
{
    return xor_data_offset(in1, in2, offset, offset, length);
}

static void bip38_generate_confirmation(uint8_t flag_byte,
    const data_chunk& address_hash_oe, const ec_secret& factorb,
    const data_chunk& derived_half1, const data_chunk& derived_half2,
    data_chunk& confirmation_code, bool use_compression)
{
    ec_point pointb = secret_to_public_key(
        factorb, true);

    uint8_t pointb_prefix =
        pointb[0] ^ (derived_half2[sizeof(derived_half2)-1] & 0x01);

    data_chunk pointb_data = to_data_chunk(pointb);

    data_chunk half1 = xor_data_offset(pointb_data,
        derived_half1, 1, 0, bip38_encrypted_block_half_length);
    aes256_encrypt(derived_half2, half1);

    data_chunk half2 = xor_data_offset(pointb_data,
        derived_half1, 17, 16, bip38_encrypted_block_half_length);
    aes256_encrypt(derived_half2, half2);

    data_chunk encrypted_pointb;
    encrypted_pointb.push_back(pointb_prefix);
    extend_data(encrypted_pointb, half1);
    extend_data(encrypted_pointb, half2);

    confirmation_code.clear();
    for(const auto& b : bip38_confirmation_prefix)
        confirmation_code.push_back(b);
    confirmation_code.push_back(flag_byte);
    extend_data(confirmation_code, address_hash_oe);
    extend_data(confirmation_code, encrypted_pointb);
    data_chunk checksum_hash = to_data_chunk(bitcoin_hash(confirmation_code));
    data_chunk checksum(&checksum_hash[0], &checksum_hash[4]);
    extend_data(confirmation_code, checksum);
}

data_chunk bip38_lock_intermediate(
    const data_chunk& intermediate, const data_chunk& seedb,
    data_chunk& confirmation_code, bool use_compression)
{
    BITCOIN_ASSERT(encode_base58(intermediate).size() ==
        bip38_intermediate_length);
    BITCOIN_ASSERT(seedb.size() == bip38_seed_length);

    bool uses_lot_sequence = std::equal(
        intermediate.begin(), intermediate.begin() + bip38_magic_length,
        bip38_magic_w_lot);

    uint8_t flag_byte = generate_flag_byte(use_compression, true);
    flag_byte |= (uses_lot_sequence ? bip38_lot_sequence : 0);

    data_chunk owner_entropy(
        &intermediate[bip38_intm_owner_entropy_start],
        &intermediate[bip38_intm_owner_entropy_end]);

    data_chunk pass_point_data(
        &intermediate[bip38_pass_point_start],
        &intermediate[bip38_pass_point_end]);

    const ec_secret factorb = ec_secret(bitcoin_hash(seedb));

    ec_point pass_point = pass_point_data;
    ec_point tmp_pass_point = pass_point_data;
    ec_multiply(tmp_pass_point, factorb);

    ec_point generated_point = (use_compression ? tmp_pass_point :
        decompress_public_key(tmp_pass_point));

    wallet::payment_address address;
    address.set_public_key(generated_point);
    const auto salt = bitcoin_hash(to_data_chunk(address.to_string()));

    data_chunk address_hash_oe(&salt[0], &salt[bip38_salt_length]);
    extend_data(address_hash_oe, owner_entropy);

    long_hash derived_data;
    if (!bip38_scrypt_hash_2(pass_point, address_hash_oe, derived_data))
        return data_chunk();

    const data_chunk derived_half1(
        &derived_data[0],
        &derived_data[bip38_encrypted_block_length]);
    const data_chunk derived_half2(
        &derived_data[bip38_encrypted_block_length],
        &derived_data[sizeof(derived_data)]);

    data_chunk half1 = xor_data(seedb, derived_half1,
        0, bip38_encrypted_block_half_length);
    aes256_encrypt(derived_half2, half1);

    data_chunk combined_data;
    for(size_t i = 8; i < 16; i++)
        combined_data.push_back(half1[i]);

    for(size_t i = 16; i < 24; i++)
        combined_data.push_back(seedb[i]);

    data_chunk half2 = xor_data_offset(
        combined_data, derived_half1, 0, 16,
        bip38_encrypted_block_half_length);
    aes256_encrypt(derived_half2, half2);

    data_chunk encrypted_data;
    for(size_t i = 0; i < 8; i++)
        encrypted_data.push_back(half1[i]);
    extend_data(encrypted_data, half2);

    data_chunk bip38_encrypted_key;
    bip38_encrypted_key.push_back(bip38_m_prefix_data[0]);
    bip38_encrypted_key.push_back(bip38_m_prefix_data[1]);
    bip38_encrypted_key.push_back(flag_byte);
    extend_data(bip38_encrypted_key, address_hash_oe);
    extend_data(bip38_encrypted_key, encrypted_data);
    append_checksum(bip38_encrypted_key);

    bip38_generate_confirmation(flag_byte, address_hash_oe, factorb,
        derived_half1, derived_half2, confirmation_code, use_compression);

    return bip38_encrypted_key;
}

#ifdef WITH_ICU
bool bip38_lock_verify(
    const data_chunk& confirmation_code,
    const std::string& passphrase, std::string& out_address)
{
    BITCOIN_ASSERT(encode_base58(confirmation_code).size() ==
        bip38_confirmation_code_length);

    const bool compressed = is_cfrm_compressed(confirmation_code);
    const bool uses_lot_seq = cfrm_has_lot_seq(confirmation_code);

    const data_chunk address_hash(
        &confirmation_code[bip38_cfrm_address_hash_start],
        &confirmation_code[bip38_cfrm_address_hash_end]);

    const data_chunk owner_entropy(
        &confirmation_code[bip38_cfrm_owner_entropy_start],
        &confirmation_code[bip38_cfrm_owner_entropy_end]);

    data_chunk encrypted_pointb(
        &confirmation_code[bip38_cfrm_encrypted_start],
        &confirmation_code[bip38_cfrm_encrypted_end]);

    const auto owner_salt_end = (uses_lot_seq ?
        bip38_salt_length : owner_entropy.size());
    const data_chunk owner_salt(
        &owner_entropy[0],
        &owner_entropy[owner_salt_end]);

    hash_digest pre_factor;
    if (!bip38_scrypt_hash(to_data_chunk(normalize_nfc(passphrase)),
        owner_salt, pre_factor))
        return false;

    ec_secret pass_factor;
    if (uses_lot_seq)
    {
        data_chunk extended_pre_factor = to_data_chunk(pre_factor);
        extend_data(extended_pre_factor, owner_entropy);
        pass_factor = ec_secret(bitcoin_hash(extended_pre_factor));
    }
    else
    {
        pass_factor = ec_secret(pre_factor);
    }

    ec_point pass_point = secret_to_public_key(pass_factor, true);

    data_chunk address_hash_oe = address_hash;
    extend_data(address_hash_oe, owner_entropy);

    long_hash derived_data;
    if (!bip38_scrypt_hash_2(pass_point, address_hash_oe, derived_data))
        return false;

    data_chunk derived_half1(
        &derived_data[0],
        &derived_data[bip38_encrypted_block_length]);
    data_chunk derived_half2(
        &derived_data[bip38_encrypted_block_length],
        &derived_data[sizeof(derived_data)]);

    data_chunk encrypted_half1(
        &encrypted_pointb[1],
        &encrypted_pointb[bip38_encrypted_block_half_length+1]);
    data_chunk encrypted_half2(
        &encrypted_pointb[bip38_encrypted_block_half_length+1],
        &encrypted_pointb[bip38_encrypted_block_length+1]);

    aes256_decrypt(derived_half2, encrypted_half1);
    data_chunk decrypted_half1 = xor_data(
        encrypted_half1, derived_half1, 0,
        bip38_encrypted_block_half_length);

    aes256_decrypt(derived_half2, encrypted_half2);
    data_chunk decrypted_half2 = xor_data_offset(
        encrypted_half2, derived_half1, 0, 16,
        bip38_encrypted_block_half_length);

    data_chunk pointb_data;
    pointb_data.push_back(encrypted_pointb[0] ^
        (derived_half2[sizeof(derived_half2)-1] & 0x01));
    extend_data(pointb_data, decrypted_half1);
    extend_data(pointb_data, decrypted_half2);

    ec_point pointb = pointb_data;
    ec_multiply(pointb, pass_factor);

    ec_point generated_point = (compressed ? pointb :
        decompress_public_key(pointb));

    wallet::payment_address address;
    address.set_public_key(generated_point);
    const auto new_address_hash = bitcoin_hash(
        to_data_chunk(address.to_string()));

    if (std::equal(address_hash.begin(), address_hash.end(),
        new_address_hash.begin()))
    {
        out_address = address.to_string();
        return true;
    }
    return false;
}

data_chunk bip38_lock_secret(
    const ec_secret& private_key, const std::string& passphrase,
    bool use_compression)
{
    const ec_point public_key = secret_to_public_key(
        private_key, use_compression);
    wallet::payment_address address;
    address.set_public_key(public_key);

    const hash_digest salt_complete = bitcoin_hash(to_data_chunk(address.to_string()));

    const data_chunk salt(
        &salt_complete[0], &salt_complete[bip38_salt_length]);

    long_hash derived_data;
    if (!bip38_scrypt_hash(to_data_chunk(normalize_nfc(passphrase)),
        salt, derived_data))
        return data_chunk();

    const data_chunk derived_half1(
        &derived_data[0],
        &derived_data[bip38_encrypted_block_length]);
    const data_chunk derived_half2(
        &derived_data[bip38_encrypted_block_length],
        &derived_data[sizeof(derived_data)]);

    data_chunk private_key_data = to_data_chunk(private_key);
    data_chunk half1 = xor_data(private_key_data, derived_half1,
        0, bip38_encrypted_block_half_length);
    data_chunk half2 = xor_data(private_key_data, derived_half1,
        bip38_encrypted_block_half_length,
        bip38_encrypted_block_half_length);

    aes256_encrypt(derived_half2, half1);
    aes256_encrypt(derived_half2, half2);

    data_chunk bip38_encrypted_key;
    bip38_encrypted_key.push_back(bip38_nm_prefix_data[0]);
    bip38_encrypted_key.push_back(bip38_nm_prefix_data[1]);
    bip38_encrypted_key.push_back(
        generate_flag_byte(use_compression, false));
    bip38_encrypted_key.insert(bip38_encrypted_key.end(),
        salt.begin(), salt.begin() + bip38_salt_length);
    extend_data(bip38_encrypted_key, half1);
    extend_data(bip38_encrypted_key, half2);
    append_checksum(bip38_encrypted_key);

    return bip38_encrypted_key;
}

static ec_secret bip38_unlock_ec_multiplied_secret(
    const encrypted_private_key& bip38_key,
    const std::string& passphrase)
{
    const bool compressed = is_compressed(bip38_key);
    const bool uses_lot_seq = has_lot_seq(bip38_key);

    const data_chunk address_hash(
        &bip38_key[bip38_salt_index_start],
        &bip38_key[bip38_salt_index_end]);

    const data_chunk owner_entropy(
        &bip38_key[bip38_owner_entropy_start],
        &bip38_key[bip38_owner_entropy_end]);

    const unsigned char* owner_salt_end = (uses_lot_seq ?
        &bip38_key[bip38_owner_entropy_start + bip38_salt_length] :
        &bip38_key[bip38_owner_entropy_end]);

    const data_chunk owner_salt(
        &bip38_key[bip38_owner_entropy_start], owner_salt_end);

    data_chunk encrypted_part1(
        &bip38_key[bip38_enc_part1_start],
        &bip38_key[bip38_enc_part1_end]);

    data_chunk encrypted_part2(
        &bip38_key[bip38_enc_part2_start],
        &bip38_key[bip38_enc_part2_end]);

    hash_digest pre_factor;
    if (!bip38_scrypt_hash(to_data_chunk(normalize_nfc(passphrase)),
        owner_salt, pre_factor))
        return ec_secret();

    ec_secret pass_factor;
    if (uses_lot_seq)
    {
        data_chunk extended_pre_factor = to_data_chunk(pre_factor);
        extend_data(extended_pre_factor, owner_entropy);
        pass_factor = ec_secret(bitcoin_hash(extended_pre_factor));
    }
    else
    {
        pass_factor = ec_secret(pre_factor);
    }

    ec_point pass_point = secret_to_public_key(pass_factor, true);

    data_chunk address_hash_oe = address_hash;
    extend_data(address_hash_oe, owner_entropy);

    long_hash seedb_pass;
    if (!bip38_scrypt_hash_2(pass_point, address_hash_oe, seedb_pass))
        return ec_secret();

    data_chunk derived_half1(
        &seedb_pass[0],
        &seedb_pass[bip38_encrypted_block_length]);
    data_chunk derived_half2(
        &seedb_pass[bip38_encrypted_block_length],
        &seedb_pass[sizeof(seedb_pass)]);

    aes256_decrypt(derived_half2, encrypted_part2);
    const data_chunk& decrypted_part2 = encrypted_part2;

    data_chunk tmp_seedb = xor_data_offset(
        decrypted_part2, derived_half1, 0,
        bip38_encrypted_block_half_length,
        bip38_encrypted_block_half_length);
    data_chunk seedb_part2(
        &tmp_seedb[8],
        &tmp_seedb[bip38_encrypted_block_half_length]);

    data_chunk remaining_encrypted_part1(
        &tmp_seedb[0], &tmp_seedb[8]);
    extend_data(encrypted_part1, remaining_encrypted_part1);

    aes256_decrypt(derived_half2, encrypted_part1);
    const data_chunk& decrypted_part1 = encrypted_part1;

    data_chunk seedb_part1 = xor_data(
        decrypted_part1, derived_half1, 0, 16);

    data_chunk seedb(seedb_part1);
    extend_data(seedb, seedb_part2);

    ec_secret factorb = ec_secret(bitcoin_hash(seedb));

    ec_multiply(pass_factor, factorb);

    const auto& unlocked_key = pass_factor;
    const auto public_key = secret_to_public_key(
        unlocked_key, compressed);

    wallet::payment_address address;
    address.set_public_key(public_key);
    const auto new_address_hash = bitcoin_hash(
        to_data_chunk(address.to_string()));

    if (!std::equal(
        new_address_hash.begin(),
        new_address_hash.begin() + address_hash.size(),
        address_hash.begin()))
        return ec_secret();

    return unlocked_key;
}

ec_secret bip38_unlock_secret(
    const encrypted_private_key& bip38_key,
    const std::string& passphrase)
{
    ec_secret bip38_decrypted_key;

    if (is_bip38_ec_multiplied(bip38_key))
    {
        bip38_decrypted_key = bip38_unlock_ec_multiplied_secret(
            bip38_key, passphrase);
    }
    else
    {
        BITCOIN_ASSERT(is_bip38_ec_non_multiplied(bip38_key));

        const data_chunk encrypted_data(
            &bip38_key[bip38_key_index_start],
            &bip38_key[bip38_key_index_end]);

        const data_chunk salt(
            &bip38_key[bip38_salt_index_start],
            &bip38_key[bip38_salt_index_end]);

        long_hash derived_data;
        if (!bip38_scrypt_hash(to_data_chunk(normalize_nfc(passphrase)),
            salt, derived_data))
            return ec_secret();

        data_chunk data_half1(
            &encrypted_data[0],
            &encrypted_data[bip38_encrypted_block_half_length]);
        data_chunk data_half2(
            &encrypted_data[bip38_encrypted_block_half_length],
            &encrypted_data[bip38_encrypted_block_length]);
        const data_chunk derived_half1(
            &derived_data[0],
            &derived_data[bip38_encrypted_block_length]);
        const data_chunk derived_half2(
            &derived_data[bip38_encrypted_block_length],
            &derived_data[sizeof(derived_data)]);

        aes256_decrypt(derived_half2, data_half1);
        aes256_decrypt(derived_half2, data_half2);

        data_chunk combined_data;
        extend_data(combined_data, data_half1);
        extend_data(combined_data, data_half2);

        const data_chunk decrypted_data = xor_data(
            combined_data, derived_half1,
            0, bip38_encrypted_block_length);

        std::copy_n(decrypted_data.begin(), ec_secret_size,
            bip38_decrypted_key.begin());

        const auto public_key = secret_to_public_key(
            bip38_decrypted_key, is_compressed(bip38_key));

        wallet::payment_address address;
        address.set_public_key(public_key);
        const auto address_hash = bitcoin_hash(
            to_data_chunk(address.to_string()));

        if (!std::equal(salt.begin(), salt.end(), address_hash.begin()))
            return ec_secret();
    }
    return bip38_decrypted_key;
}
#endif

} // namespace bip38
} // namespace libbitcoin

