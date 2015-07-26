/**
 * Copyright (c) 2011-2015 libbitcoin developers (see AUTHORS)
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
#include <bitcoin/bitcoin/wallet/stealth_address.hpp>

#include <algorithm>
#include <cstdint>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/stealth.hpp>
#include <bitcoin/bitcoin/formats/base58.hpp>
#include <bitcoin/bitcoin/math/checksum.hpp>
#include <bitcoin/bitcoin/math/ec_keys.hpp>
#include <bitcoin/bitcoin/math/hash.hpp>
#include <bitcoin/bitcoin/utility/assert.hpp>
#include <bitcoin/bitcoin/utility/endian.hpp>

namespace libbitcoin {

constexpr uint8_t options_size = sizeof(uint8_t);
constexpr uint8_t version_size = sizeof(uint8_t);
constexpr uint8_t compressed_pubkey_size = 33;
constexpr uint8_t number_keys_size = sizeof(uint8_t);
constexpr uint8_t number_sigs_size = sizeof(uint8_t);
constexpr uint8_t prefix_length_size = sizeof(uint8_t);
constexpr uint8_t max_spend_key_count = max_uint8;

// wiki.unsystem.net/index.php/DarkWallet/Stealth#Address_format
// [version:1=0x2a][options:1][scan_pubkey:33][N:1][spend_pubkey_1:33]..
// [spend_pubkey_N:33][number_signatures:1][prefix_number_bits:1]
// [prefix:prefix_number_bits / 8, round up][checksum:4]
// Estimate assumes N = 0 and prefix_length = 0:
constexpr size_t min_address_size = version_size + options_size +
    compressed_pubkey_size + number_keys_size + number_sigs_size +
    prefix_length_size + checksum_size;

// Document the assumption that the prefix is defined with an 8 bit block size.
static_assert(binary_type::bits_per_block == byte_bits,
    "The declaraction of stealh_prefix must have an 8 bit block size.");

stealth_address::stealth_address()
    : valid_(false)
{
}

stealth_address::stealth_address(const binary_type& prefix,
    const ec_point& scan_pubkey, const pubkey_list& spend_pubkeys,
    uint8_t signatures, bool testnet)
{
    // Guard against uncompressed pubkey or junk data.
    if (scan_pubkey.size() != compressed_pubkey_size)
        return;

    // Guard against uncompressed pubkey or junk data.
    for (const auto& spend_pubkey: spend_pubkeys)
        if (spend_pubkey.size() != compressed_pubkey_size)
            return;

    // Apply 'reuse'.
    auto spend_points = spend_pubkeys;
    if (spend_points.empty())
        spend_points.push_back(scan_pubkey);

    // Guard against too many keys.
    auto spend_pubkeys_size = spend_points.size();
    if (spend_pubkeys_size > max_spend_key_count)
        return;

    // Guard against prefix too long.
    auto prefix_number_bits = static_cast<uint8_t>(prefix.size());
    if (prefix_number_bits > max_prefix_bits)
        return;

    // Coerce signatures to a valid range.
    if (signatures == 0 || signatures > spend_pubkeys_size)
        signatures_ = static_cast<uint8_t>(spend_pubkeys_size);
    else
        signatures_ = signatures;

    prefix_ = prefix;
    testnet_ = testnet;
    scan_pubkey_ = scan_pubkey;
    spend_pubkeys_ = spend_points;
    valid_ = true;
}

std::string stealth_address::encoded() const
{
    if (!valid_)
        return std::string();

    data_chunk raw_address;
    raw_address.push_back(get_version());
    raw_address.push_back(get_options());
    extend_data(raw_address, scan_pubkey_);

    // Spend_pubkeys must be guarded against a size greater than 255.
    auto number_spend_pubkeys = static_cast<uint8_t>(spend_pubkeys_.size());

    // Adjust for key reuse.
    if (get_reuse_key())
        --number_spend_pubkeys;

    raw_address.push_back(number_spend_pubkeys);

    // Serialize the spend keys, excluding any that match the scan key.
    for (const ec_point& pubkey: spend_pubkeys_)
        if (pubkey != scan_pubkey_)
            extend_data(raw_address, pubkey);

    raw_address.push_back(signatures_);

    // The prefix must be guarded against a size greater than 32
    // so that the bitfield can convert into uint32_t and sized by uint8_t.
    auto prefix_number_bits = static_cast<uint8_t>(prefix_.size());

    // Serialize the prefix bytes/blocks.
    raw_address.push_back(prefix_number_bits);
    extend_data(raw_address, prefix_.blocks());

    append_checksum(raw_address);
    return encode_base58(raw_address);
}

bool stealth_address::set_encoded(const std::string& encoded_address)
{
    valid_ = false;
    data_chunk raw_address;
    if (!decode_base58(raw_address, encoded_address))
        return false;

    // Size is guarded until we get to N.
    auto required_size = min_address_size;
    if (raw_address.size() < required_size)
        return valid_;

    if (!verify_checksum(raw_address))
        return valid_;

    // Start walking the array.
    auto iter = raw_address.begin();

    // [version:1 = 0x2a]
    auto version = *iter;
    if (version != network::mainnet && version != network::testnet)
        return valid_;
    testnet_ = (version == network::testnet);
    ++iter;

    // [options:1]
    auto options = *iter;
    if (options != flags::none && options != flags::reuse_key)
        return valid_;
    ++iter;

    // [scan_pubkey:33]
    auto scan_key_begin = iter;
    iter += compressed_pubkey_size;
    scan_pubkey_ = ec_point(scan_key_begin, iter);

    // [N:1]
    auto number_spend_pubkeys = *iter;
    ++iter;

    // Adjust and retest required size. for pubkey list.
    required_size += number_spend_pubkeys * compressed_pubkey_size;
    if (raw_address.size() < required_size)
        return valid_;

    // We don't explicitly save 'reuse', instead we add to spend_pubkeys_.
    if (options == flags::reuse_key)
        spend_pubkeys_.emplace_back(scan_pubkey_);

    // [spend_pubkey_1:33]..[spend_pubkey_N:33]
    for (auto key = 0; key < number_spend_pubkeys; ++key)
    {
        auto spend_key_begin = iter;
        iter += compressed_pubkey_size;
        spend_pubkeys_.emplace_back(ec_point(spend_key_begin, iter));
    }

    // [number_signatures:1]
    signatures_ = *iter;
    ++iter;

    // [prefix_number_bits:1]
    auto prefix_number_bits = *iter;
    if (prefix_number_bits > max_prefix_bits)
        return valid_;
    ++iter;

    // [prefix:prefix_number_bits / 8, round up]
    // Adjust and retest required size for prefix bytes.
    auto prefix_bytes = (prefix_number_bits + (byte_bits - 1)) / byte_bits;
    required_size += prefix_bytes;
    if (raw_address.size() != required_size)
        return valid_;

    // Prefix not yet supported on server!
    //BITCOIN_ASSERT(prefix_number_bits == 0);
    //if (prefix_number_bits != 0)
    //    return valid_;

    // Deserialize the prefix bytes/blocks.
    data_chunk raw_prefix(iter, iter + prefix_bytes);
    prefix_ = binary_type(prefix_number_bits, raw_prefix);

    valid_ = true;
    return valid_;
}

bool stealth_address::valid() const
{
    return valid_;
}

const binary_type& stealth_address::get_prefix() const
{
    return prefix_;
}

const ec_point& stealth_address::get_scan_pubkey() const
{
    return scan_pubkey_;
}

uint8_t stealth_address::get_signatures() const
{
    return signatures_;
}

const pubkey_list& stealth_address::get_spend_pubkeys() const
{
    return spend_pubkeys_;
}

bool stealth_address::get_testnet() const
{
    return testnet_;
}

bool stealth_address::get_reuse_key() const
{
    // If the spend_pubkeys_ contains the scan_pubkey_ then the key is reused.
    return std::find(spend_pubkeys_.begin(), spend_pubkeys_.end(),
        scan_pubkey_) != spend_pubkeys_.end();
}

uint8_t stealth_address::get_options() const
{
    if (get_reuse_key())
        return flags::reuse_key;
    else
        return flags::none;
}

uint8_t stealth_address::get_version() const
{
    if (testnet_)
        return network::testnet;
    else
        return network::mainnet;
}

} // namespace libbitcoin

