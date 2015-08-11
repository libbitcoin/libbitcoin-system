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
#ifndef LIBBITCOIN_BIP38_HPP
#define LIBBITCOIN_BIP38_HPP

#include <string>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/script.hpp>
#include <bitcoin/bitcoin/math/crypto.hpp>
#include <bitcoin/bitcoin/utility/data.hpp>


namespace libbitcoin {
namespace bip38 {

typedef data_chunk encrypted_private_key;

/**
 * Scrypt parameters N used in bip38.
 */
BC_CONSTEXPR size_t bip38_scrypt_N   = 16384;
BC_CONSTEXPR size_t bip38_scrypt_N_2 =  1024;

/**
 * Scrypt parameters r used in bip38.
 */
BC_CONSTEXPR size_t bip38_scrypt_r   = 8;
BC_CONSTEXPR size_t bip38_scrypt_r_2 = 1;

/**
 * Scrypt parameters p used in bip38.
 */
BC_CONSTEXPR size_t bip38_scrypt_p   = 8;
BC_CONSTEXPR size_t bip38_scrypt_p_2 = 1;

/**
 * The total length in bytes of the address hash used
 * as a salt in bip38.
 */
BC_CONSTEXPR size_t bip38_salt_length = 4;

/**
 * The total length in bytes of an aes256 encrypted block
 * used in bip38.
 */
BC_CONSTEXPR size_t bip38_encrypted_block_length = 32;

/**
 * The half length in bytes of a bip38 encrypted key.
 */
BC_CONSTEXPR size_t bip38_encrypted_block_half_length = 16;

/**
 * The total length in bytes of a bip38 encrypted key.
 */
BC_CONSTEXPR size_t bip38_encrypted_key_length = 58;

/**
 * The fixed bip38 non-multiplied
 * prefix data constants.
 */
BC_CONSTEXPR uint8_t bip38_nm_prefix_data[2] = { 0x01, 0x42 };

/**
 * The fixed bip38 ec-multiplied prefix data constants.
 */
BC_CONSTEXPR uint8_t bip38_m_prefix_data[2] = { 0x01, 0x43 };

/**
 * The fixed number of bip38 base58 checked intermediate bytes required.
 */
BC_CONSTEXPR size_t bip38_intermediate_length = 8;

/**
 * The fixed number of bip38 random seed bytes required.
 */
BC_CONSTEXPR size_t bip38_seed_length = 8;

/**
 * The fixed number of bip38 magic bytes.
 */
BC_CONSTEXPR size_t bip38_magic_length = 8;

typedef uint8_t bip38_magic[bip38_magic_length];

/**
 * The fixed bip38 magic bytes used when lot is specified.
 */
BC_CONSTEXPR bip38_magic bip38_magic_w_lot =
{
    0x2C, 0xE9, 0xB3, 0xE1, 0xFF, 0x39, 0xE2, 0x51
};

/**
 * The fixed bip38 magic bytes used when lot is not specified.
 */
BC_CONSTEXPR bip38_magic bip38_magic_wo_lot =
{
    0x2C, 0xE9, 0xB3, 0xE1, 0xFF, 0x39, 0xE2, 0x53
};

/**
 * Fixed byte indices used for bip38 decryption.
 */
BC_CONSTEXPR size_t bip38_comp_mult_index     =  1;
BC_CONSTEXPR size_t bip38_flag_index          =  2;
BC_CONSTEXPR size_t bip38_salt_index_start    =  3;
BC_CONSTEXPR size_t bip38_salt_index_end      =  7;
BC_CONSTEXPR size_t bip38_key_index_start     =  7;
BC_CONSTEXPR size_t bip38_owner_entropy_start =  7;
BC_CONSTEXPR size_t bip38_owner_entropy_end   = 15;
BC_CONSTEXPR size_t bip38_pass_point_start    = 16;
BC_CONSTEXPR size_t bip38_pass_point_end      = 49;
BC_CONSTEXPR size_t bip38_enc_part1_start     = 15;
BC_CONSTEXPR size_t bip38_decrypt_xor_offset  = 16;
BC_CONSTEXPR size_t bip38_decrypt_xor_length  = 16;
BC_CONSTEXPR size_t bip38_enc_part1_end       = 23;
BC_CONSTEXPR size_t bip38_enc_part2_start     = 23;
BC_CONSTEXPR size_t bip38_enc_part2_end       = 39;
BC_CONSTEXPR size_t bip38_key_index_end       = 39;


/**
 * The bip38 flag byte masks used in this implementation.
 */
BC_CONSTEXPR uint8_t bip38_lot_sequence      = 0x04;
BC_CONSTEXPR uint8_t bip38_compressed        = 0x20;
BC_CONSTEXPR uint8_t bip38_ec_multiplied     = 0x00;
BC_CONSTEXPR uint8_t bip38_ec_non_multiplied = 0xC0;


/**
 * Performs bip38 encryption on the private key given
 * the specified passphrase.
 *
 * bip38_lock_secret(private_key, passphrase)
 */
BC_API data_chunk bip38_lock_secret(
    const ec_secret& private_key, const std::string& passphrase,
    bool use_compression);

/**
 * Performs bip38 encryption based on the given
 * intermediate and random data seed provided.
 *
 * bip38_lock_secret(private_key, passphrase)
 */
BC_API data_chunk bip38_lock_intermediate(
    const data_chunk& intermediate, const data_chunk& seedb,
    uint32_t num_out_addrs, bool use_compression);

/**
 * Performs bip38 decryption on the encrypted key given
 * the specified passphrase.
 *
 * bip38_unlock_secret(encrypted_key, passphrase)
 */
BC_API ec_secret bip38_unlock_secret(
    const encrypted_private_key& bip38_key, const std::string& passphrase);

} // namespace bip38
} // namespace libbitcoin

#endif
