/*
 * Copyright (c) 2011-2013 libbitcoin developers (see AUTHORS)
 *
 * This file is part of libbitcoin.
 *
 * libbitcoin is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#include <bitcoin/bitcoin/wallet/hd_keys.hpp>

#include <cstdint>
#include <bitcoin/bitcoin/address.hpp>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/format.hpp>
#include <bitcoin/bitcoin/utility/base58.hpp>
#include <bitcoin/bitcoin/utility/checksum.hpp>
#include <bitcoin/bitcoin/utility/ec_keys.hpp>
#include <bitcoin/bitcoin/utility/hash.hpp>
#include <bitcoin/bitcoin/utility/serializer.hpp>

namespace libbitcoin {

constexpr uint32_t mainnet_private_prefix = 0x0488ADE4;
constexpr uint32_t mainnet_public_prefix = 0x0488B21E;
constexpr uint32_t testnet_private_prefix = 0x04358394;
constexpr uint32_t testnet_public_prefix = 0x043587CF;
constexpr size_t serialized_length = 4 + 1 + 4 + 4 + 32 + 33 + 4;

// long_hash is used for hmac_sha512 in libbitcoin
constexpr size_t half_long_hash_size = long_hash_size / 2;
typedef byte_array<half_long_hash_size> half_long_hash;

/**
 * Corresponds to a split HMAC-SHA256 result, as used in BIP 32.
 */
struct split_long_hash
{
    half_long_hash L;
    half_long_hash R;
};

static split_long_hash split(const long_hash& hmac)
{
    split_long_hash I;
    std::copy(hmac.begin(), hmac.begin() + half_long_hash_size, I.L.begin());
    std::copy(hmac.begin() + half_long_hash_size, hmac.end(), I.R.begin());
    return I;
}

hd_public_key::hd_public_key()
  : valid_(false)
{
}

hd_public_key::hd_public_key(const ec_point& public_key,
    const chain_code_type& chain_code, hd_key_lineage lineage)
  : valid_(true), K_(public_key), c_(chain_code), lineage_(lineage)
{
}

bool hd_public_key::valid() const
{
    return valid_;
}

const ec_point& hd_public_key::public_key() const
{
    return K_;
}

const chain_code_type& hd_public_key::chain_code() const
{
    return c_;
}

const hd_key_lineage& hd_public_key::lineage() const
{
    return lineage_;
}

bool hd_public_key::set_encoded(std::string encoded)
{
    if (!is_base58(encoded))
        return false;
    const data_chunk decoded = decode_base58(encoded);
    if (decoded.size() != serialized_length)
        return false;
    if (!verify_checksum(decoded))
        return false;

    auto ds = make_deserializer(decoded.begin(), decoded.end());
    auto prefix = ds.read_big_endian<uint32_t>();
    if (prefix != mainnet_public_prefix && prefix != testnet_public_prefix)
        return false;

    valid_ = true;
    lineage_.testnet = prefix == testnet_public_prefix;
    lineage_.depth = ds.read_byte();
    lineage_.parent_fingerprint = ds.read_little_endian<uint32_t>();
    lineage_.child_number = ds.read_big_endian<uint32_t>();
    c_ = ds.read_bytes<chain_code_size>();
    K_ = ds.read_data(33);
    return true;
}

std::string hd_public_key::encoded() const
{
    data_chunk data;
    data.reserve(serialized_length);
    auto prefix = mainnet_public_prefix;
    if (lineage_.testnet)
        prefix = testnet_public_prefix;

    extend_data(data, to_big_endian(prefix));
    data.push_back(lineage_.depth);
    extend_data(data, to_little_endian(lineage_.parent_fingerprint));
    extend_data(data, to_big_endian(lineage_.child_number));
    extend_data(data, c_);
    extend_data(data, K_);

    append_checksum(data);
    return encode_base58(data);
}

uint32_t hd_public_key::fingerprint() const
{
    short_hash md = bitcoin_short_hash(K_);
    return from_little_endian<uint32_t>(md.begin());
}

payment_address hd_public_key::address() const
{
    payment_address address;
    set_public_key(address, K_);
    return address;
}

hd_public_key hd_public_key::generate_public_key(uint32_t i) const
{
    if (!valid_)
        return hd_private_key();
    if (first_hardened_key <= i)
        return hd_public_key();

    data_chunk data;
    data.reserve(33 + 4);
    extend_data(data, K_);
    extend_data(data, to_big_endian(i));
    auto I = split(hmac_sha512_hash(data, to_data_chunk(c_)));

    // The returned child key Ki is point(parse256(IL)) + Kpar.
    ec_point Ki = K_;
    if (!ec_add(Ki, I.L))
        return hd_public_key();

    hd_key_lineage lineage
    {
        lineage_.testnet,
        static_cast<uint8_t>(lineage_.depth + 1),
        fingerprint(), i
    };
    return hd_public_key(Ki, I.R, lineage);
}

hd_private_key::hd_private_key()
  : hd_public_key()
{
}

hd_private_key::hd_private_key(const ec_secret& private_key,
    const chain_code_type& chain_code, hd_key_lineage lineage)
  : hd_public_key(secret_to_public_key(private_key), chain_code, lineage),
    k_(private_key)
{
}

hd_private_key::hd_private_key(const data_chunk& seed, bool testnet)
  : hd_public_key()
{
    std::string key("Bitcoin seed");
    auto I = split(hmac_sha512_hash(seed, to_data_chunk(key)));

    // The key is invalid if parse256(IL) >= n or 0:
    if (!verify_private_key(I.L))
        return;

    auto lineage = hd_key_lineage{testnet, 0, 0, 0};
    *this = hd_private_key(I.L, I.R, lineage);
}

const ec_secret& hd_private_key::private_key() const
{
    return k_;
}

bool hd_private_key::set_encoded(std::string encoded)
{
    if (!is_base58(encoded))
        return false;
    const data_chunk decoded = decode_base58(encoded);
    if (decoded.size() != serialized_length)
        return false;
    if (!verify_checksum(decoded))
        return false;

    auto ds = make_deserializer(decoded.begin(), decoded.end());
    auto prefix = ds.read_big_endian<uint32_t>();
    if (prefix != mainnet_private_prefix && prefix != testnet_private_prefix)
        return false;

    valid_ = true;
    lineage_.testnet = prefix == testnet_private_prefix;
    lineage_.depth = ds.read_byte();
    lineage_.parent_fingerprint = ds.read_little_endian<uint32_t>();
    lineage_.child_number = ds.read_big_endian<uint32_t>();
    c_ = ds.read_bytes<chain_code_size>();
    ds.read_byte();
    k_ = ds.read_bytes<ec_secret_size>();
    K_ = secret_to_public_key(k_);
    return true;
}

std::string hd_private_key::encoded() const
{
    data_chunk data;
    data.reserve(4 + 1 + 4 + 4 + 32 + 33 + 4);
    auto prefix = mainnet_private_prefix;
    if (lineage_.testnet)
        prefix = testnet_private_prefix;

    extend_data(data, to_big_endian(prefix));
    data.push_back(lineage_.depth);
    extend_data(data, to_little_endian(lineage_.parent_fingerprint));
    extend_data(data, to_big_endian(lineage_.child_number));
    extend_data(data, c_);
    data.push_back(0x00);
    extend_data(data, k_);

    append_checksum(data);
    return encode_base58(data);
}

hd_private_key hd_private_key::generate_private_key(uint32_t i) const
{
    if (!valid_)
        return hd_private_key();

    data_chunk data;
    data.reserve(33 + 4);
    if (first_hardened_key <= i)
    {
        data.push_back(0x00);
        extend_data(data, k_);
        extend_data(data, to_big_endian(i));
    }
    else
    {
        extend_data(data, K_);
        extend_data(data, to_big_endian(i));
    }
    auto I = split(hmac_sha512_hash(data, to_data_chunk(c_)));

    // The child key ki is (parse256(IL) + kpar) mod n:
    ec_secret ki = k_;
    if (!ec_add(ki, I.L))
        return hd_private_key();

    hd_key_lineage lineage
    {
        lineage_.testnet,
        static_cast<uint8_t>(lineage_.depth + 1),
        fingerprint(),
        i
    };
    return hd_private_key(ki, I.R, lineage);
}

hd_public_key hd_private_key::generate_public_key(uint32_t i) const
{
    return generate_private_key(i);
}

} // namespace libbitcoin

