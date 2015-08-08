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
#include <bitcoin/bitcoin/wallet/address.hpp>
#include <bitcoin/bitcoin/wallet/key_formats.hpp>
#include "../math/external/crypto_scrypt.h"


namespace libbitcoin {
namespace bip38 {

static inline uint8_t generate_flag_byte(bool compressed)
{
    return (compressed ? bip38_compressed : bip38_uncompressed);
}

static inline uint8_t get_flag_byte(const encrypted_private_key& key)
{
    return key[bip38_flag_index];
}

static inline bool is_compressed(const encrypted_private_key key)
{
    return (get_flag_byte(key) == bip38_compressed);
}

static inline bool is_bip38_ec_multiplied(
    const encrypted_private_key& key)
{
    return (key[bip38_comp_mult_index] ==
            bip38_uc_m_prefix_data[bip38_comp_mult_index]);
}

static inline bool is_bip38_ec_non_multiplied(
    const encrypted_private_key& key)
{
    return (key[bip38_comp_mult_index] ==
            bip38_uc_nm_prefix_data[bip38_comp_mult_index]);
}

static std::string normalize_nfc(const std::string& value)
{
    using namespace boost::locale;
    const generator locale;
    return normalize(value, norm_type::norm_nfc, locale(""));
}

static bool bip38_scrypt_hash(const data_chunk& passphrase,
    const data_slice& salt, long_hash& long_hash)
{
    return crypto_scrypt(passphrase.data(), passphrase.size(),
        salt.data(), bip38_salt_length, bip38_scrypt_N, bip38_scrypt_r,
        bip38_scrypt_p, long_hash.data(), sizeof(long_hash)) == 0;
}

static data_chunk xor_data(const data_chunk& in1, const data_chunk& in2,
    size_t offset, size_t length)
{
    data_chunk out;

    BITCOIN_ASSERT(in1.size() == in2.size());
    for(auto i = offset; i < (offset + length); i++)
        out.push_back(in1[i] ^ in2[i]);

    return out;
}

static data_chunk bip38_aes256_encrypt_data(
    const data_chunk& private_key, const long_hash& derived_data)
{
    BITCOIN_ASSERT(private_key.size() == sizeof(ec_secret));

    const data_chunk derived_half1(
        &derived_data[0],
        &derived_data[bip38_encrypted_block_length]);
    const data_chunk derived_half2(
        &derived_data[bip38_encrypted_block_length],
        &derived_data[sizeof(derived_data)]);

    data_chunk half1 = xor_data(private_key, derived_half1,
        0, bip38_encrypted_block_half_length);
    data_chunk half2 = xor_data(private_key, derived_half1,
        bip38_encrypted_block_half_length,
        bip38_encrypted_block_half_length);

    aes256_encrypt(derived_half2, half1);
    aes256_encrypt(derived_half2, half2);

    data_chunk encrypted_data;
    extend_data(encrypted_data, half1);
    extend_data(encrypted_data, half2);
    return encrypted_data;
}

static data_chunk bip38_aes256_decrypt_data(
    const data_chunk& data, const long_hash& derived_data)
{
    BITCOIN_ASSERT(data.size() == bip38_encrypted_block_length);

    data_chunk data_half1(&data[0],
        &data[bip38_encrypted_block_half_length]);
    data_chunk data_half2(
        &data[bip38_encrypted_block_half_length],
        &data[bip38_encrypted_block_length]);
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

    return xor_data(combined_data, derived_half1,
        0, bip38_encrypted_block_length);
}

data_chunk bip38_lock_secret(
    const ec_secret& private_key, const std::string& passphrase,
    bool use_compression)
{
    long_hash derived_data;
    payment_address address;

    const auto public_key = secret_to_public_key(
        private_key, use_compression);
    set_public_key(address, public_key);
    const auto salt = bitcoin_hash(to_data_chunk(address.encoded()));

    if (!bip38_scrypt_hash(to_data_chunk(normalize_nfc(passphrase)),
        salt, derived_data))
        return data_chunk();

    data_chunk encrypted_data = bip38_aes256_encrypt_data(
        to_data_chunk(private_key), derived_data);

    data_chunk bip38_encrypted_key(&bip38_uc_nm_prefix_data[0],
        &bip38_uc_nm_prefix_data[2]);
    bip38_encrypted_key.push_back(generate_flag_byte(use_compression));
    bip38_encrypted_key.insert(bip38_encrypted_key.end(),
        salt.begin(), salt.begin() + bip38_salt_length);
    extend_data(bip38_encrypted_key, encrypted_data);
    append_checksum(bip38_encrypted_key);

    return bip38_encrypted_key;
}

ec_secret bip38_unlock_secret(
    const encrypted_private_key& bip38_key,
    const std::string& passphrase)
{
    long_hash derived_data;
    payment_address address;
    data_chunk decrypted_half1;
    data_chunk decrypted_half2;

    // NOTE: ec-multiplied keys are not currently supported
    if (is_bip38_ec_multiplied(bip38_key))
        return ec_secret();

    BITCOIN_ASSERT(is_bip38_ec_non_multiplied(bip38_key));

    const data_chunk salt(&bip38_key[bip38_salt_index_start],
        &bip38_key[bip38_salt_index_end]);

    if (!bip38_scrypt_hash(to_data_chunk(normalize_nfc(passphrase)),
        salt, derived_data))
        return ec_secret();

    data_chunk encrypted_data(&bip38_key[bip38_salt_index_end],
        &bip38_key[bip38_key_index_end]);

    data_chunk decrypted_data = bip38_aes256_decrypt_data(
        encrypted_data, derived_data);

    ec_secret bip38_decrypted_key;
    std::copy_n(decrypted_data.begin(), ec_secret_size,
        bip38_decrypted_key.begin());

    /* verify that the address hash matches the salt */
    const auto public_key = secret_to_public_key(
        bip38_decrypted_key, is_compressed(bip38_key));
    set_public_key(address, public_key);
    const auto address_hash = bitcoin_hash(
        to_data_chunk(address.encoded()));

    if (!std::equal(salt.begin(), salt.end(), address_hash.begin()))
        return ec_secret();

    return ec_secret(bip38_decrypted_key);
}

} // namespace bip38
} // namespace libbitcoin

