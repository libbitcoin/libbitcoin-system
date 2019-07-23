/**
 * Copyright (c) 2011-2019 libbitcoin developers (see AUTHORS)
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
#include <bitcoin/system/wallet/witness_address.hpp>

#include <algorithm>
#include <cstdint>
#include <string>
#include <utility>
#include <boost/program_options.hpp>
#include <bitcoin/system/formats/base_32.hpp>
#include <bitcoin/system/formats/base_58.hpp>
#include <bitcoin/system/math/checksum.hpp>
#include <bitcoin/system/math/elliptic_curve.hpp>
#include <bitcoin/system/math/hash.hpp>
#include <bitcoin/system/wallet/ec_private.hpp>
#include <bitcoin/system/wallet/ec_public.hpp>

namespace libbitcoin {
namespace system {
namespace wallet {

using namespace bc::system::machine;

// BIP 142 p2wpkh and p2wsh address versions.
// https://github.com/bitcoin/bips/blob/master/bip-0142.mediawiki#specification
const uint8_t witness_address::mainnet_base58_p2wpkh = 0x06;
const uint8_t witness_address::mainnet_base58_p2wsh = 0x0a;

const uint8_t witness_address::testnet_base58_p2wpkh = 0x03;
const uint8_t witness_address::testnet_base58_p2wsh = 0x28;

// These version values are not meaningful but are required to
// distinguish from other address version values.
const uint8_t witness_address::mainnet_bech32_p2wpkh = 0xd0;
const uint8_t witness_address::mainnet_bech32_p2wsh = 0xe0;

const uint8_t witness_address::testnet_bech32_p2wpkh = 0xd1;
const uint8_t witness_address::testnet_bech32_p2wsh = 0xe1;

// BIP 142 prefix constants.
// https://github.com/bitcoin/bips/blob/master/bip-0142.mediawiki#Specification
const std::string witness_address::mainnet_p2wpkh = "p2";
const std::string witness_address::testnet_p2wpkh = "7Xh";
const std::string witness_address::mainnet_p2wsh = "QW";
const std::string witness_address::testnet_p2wsh = "T7n";

// BIP 173 prefix constants.
// https://github.com/bitcoin/bips/blob/master/bip-0173.mediawiki#segwit-address-format
const std::string witness_address::mainnet_prefix = "bc";
const std::string witness_address::testnet_prefix = "tb";

const size_t bech32_contracted_bit_size = 5;
const size_t bech32_expanded_bit_size = 8;

witness_address::witness_address()
  : payment_address(),
    witness_version_(0),
    witness_hash_(null_hash)
{
}

witness_address::witness_address(witness_address&& other)
  : payment_address(other),
    prefix_(other.prefix_),
    witness_version_(other.witness_version_),
    witness_hash_(std::move(other.witness_hash_))
{
}

witness_address::witness_address(const witness_address& other)
  : payment_address(other),
    prefix_(other.prefix_),
    witness_version_(other.witness_version_),
    witness_hash_(other.witness_hash_)
{
}

witness_address::witness_address(const witness_p2wpkh& decoded, uint8_t version)
  : witness_address(from_witness(decoded, version))
{
}

witness_address::witness_address(const witness_p2wsh& decoded, uint8_t version)
  : witness_address(from_witness(decoded, version))
{
}

witness_address::witness_address(const std::string& address, uint8_t version)
  : witness_address(from_string(address, version))
{
}

witness_address::witness_address(const ec_private& secret, uint8_t version,
    const std::string& prefix)
  : witness_address(from_private(secret, version, prefix))
{
}

witness_address::witness_address(const ec_public& point, uint8_t version,
    const std::string& prefix)
  : witness_address(from_public(point, version, prefix))
{
}

witness_address::witness_address(const chain::script& script, uint8_t version,
    const std::string& prefix)
  : witness_address(from_script(script, version, prefix))
{
}

witness_address::witness_address(short_hash&& hash, uint8_t version,
    uint8_t witness_version, const std::string& prefix)
  : payment_address(std::move(hash), version),
    prefix_(prefix),
    witness_version_(witness_version),
    witness_hash_(null_hash)
{
}

witness_address::witness_address(const short_hash& hash, uint8_t version,
    uint8_t witness_version, const std::string& prefix)
  : payment_address(hash, version),
    prefix_(prefix),
    witness_version_(witness_version),
    witness_hash_(null_hash)
{
}

witness_address::witness_address(hash_digest&& hash, uint8_t version,
    uint8_t witness_version, const std::string& prefix)
  : payment_address(null_short_hash, version),
    prefix_(prefix),
    witness_version_(witness_version),
    witness_hash_(std::move(hash))
{
}

witness_address::witness_address(const hash_digest& hash, uint8_t version,
    uint8_t witness_version, const std::string& prefix)
  : payment_address(null_short_hash, version),
    prefix_(prefix),
    witness_version_(witness_version),
    witness_hash_(hash)
{
}

// Validators.
// ----------------------------------------------------------------------------

bool witness_address::is_address(data_slice decoded)
{
    return ((decoded.size() == witness_p2wpkh_size) ||
            (decoded.size() == witness_p2wsh_size)) && verify_checksum(decoded);
}

// Factories.
// ----------------------------------------------------------------------------
// static
witness_address witness_address::from_string(const std::string& address,
    uint8_t version, const std::string& prefix)
{
    // BIP 141 constants
    static constexpr size_t witness_program_min_size = 2;
    static constexpr size_t witness_program_max_size = 40;

    // BIP 173 witness version 0 constants
    static constexpr size_t p2wpkh_size = 42;
    static constexpr size_t p2wsh_size = 62;
    static constexpr size_t bech32_address_min_size = 14;
    static constexpr size_t bech32_address_max_size = 74;
    static constexpr size_t bech32_address_size_modulo = 8;
    static constexpr size_t bech32_address_modulo_invalid_1 = 0;
    static constexpr size_t bech32_address_modulo_invalid_2 = 3;
    static constexpr size_t bech32_address_modulo_invalid_3 = 5;

    // Attempt to decode BIP 173 address format.
    base32 bech32_decoded;
    if (decode_base32(bech32_decoded, address))
    {
        const uint8_t witness_version = bech32_decoded.payload.front();

        // Checks specific to witness version 0.
        if (witness_version == 0 &&
            (address.size() < p2wpkh_size || address.size() > p2wsh_size))
            return {};

        // Verify witness version is valid (only version 0 is
        // currently supported).
        if (witness_version != 0)
            return {};

        // Verify additional properties (BIP173 decoding).
        if (address.size() < bech32_address_min_size ||
            address.size() > bech32_address_max_size)
            return {};

        const auto address_mod = address.size() % bech32_address_size_modulo;
        if (address_mod == bech32_address_modulo_invalid_1 ||
            address_mod == bech32_address_modulo_invalid_2 ||
            address_mod == bech32_address_modulo_invalid_3)
            return {};

        const auto converted = convert_bits(bech32_contracted_bit_size,
            bech32_expanded_bit_size, false, bech32_decoded.payload, 1);
        if (converted.size() < witness_program_min_size ||
            converted.size() > witness_program_max_size)
            return {};

        // Reinterpret casting avoids a copy, but should probably be replaced.
        if (converted.size() == hash_size)
            return { *reinterpret_cast<const hash_digest*>(&converted[0]),
                version, witness_version, prefix };
        else if (converted.size() == short_hash_size)
            return { *reinterpret_cast<const short_hash*>(&converted[0]),
                version, witness_version, prefix };

        // This is a witness_version 0 of an unsupported length.
        return {};
    }

    // Attempt to decode BIP 142 address format.
    data_chunk decoded;
    if (!decode_base58(decoded, address) || !is_address(decoded))
        return {};

    // Reinterpret casting avoids a copy, but should probably be replaced.
    if (decoded.size() == witness_p2wpkh_size)
        return { *reinterpret_cast<witness_p2wpkh*>(&decoded[0]), version };
    else if (decoded.size() == witness_p2wsh_size)
        return { *reinterpret_cast<witness_p2wsh*>(&decoded[0]), version };

    // Address is an unrecognized format.
    return {};
}

// static
witness_address witness_address::from_witness(const witness_p2wpkh& decoded,
    uint8_t version)
{
    if (!is_address(decoded))
        return {};

    const short_hash hash = slice<witness_prefix_size, short_hash_size +
        witness_prefix_size>(decoded);
    return { hash, version };
}

// static
witness_address witness_address::from_witness(const witness_p2wsh& decoded,
    uint8_t version)
{
    if (!is_address(decoded))
        return {};

    const hash_digest hash = slice<witness_prefix_size, hash_size +
        witness_prefix_size>(decoded);
    return { hash, version };
}

// static
witness_address witness_address::from_private(const ec_private& secret,
    uint8_t version, const std::string& prefix)
{
    if (secret)
        return { secret.to_public(), version, prefix };

    return {};
}

// static
witness_address witness_address::from_public(const ec_public& point,
    uint8_t version, const std::string& prefix)
{
    if (!point)
        return {};

    data_chunk data;
    if (!point.to_data(data))
        return {};

    const uint8_t witness_version = 0;

    switch (version)
    {
        case witness_address::mainnet_bech32_p2wsh:
        case witness_address::testnet_bech32_p2wsh:
        case witness_address::mainnet_base58_p2wsh:
        case witness_address::testnet_base58_p2wsh:
            return { bitcoin_hash(data), version, witness_version, prefix };

        case witness_address::mainnet_bech32_p2wpkh:
        case witness_address::testnet_bech32_p2wpkh:
        case witness_address::mainnet_base58_p2wpkh:
        case witness_address::testnet_base58_p2wpkh:
            return { bitcoin_short_hash(data), version, witness_version,
                prefix };

        default:
            return {};
    }
}

// static
witness_address witness_address::from_script(const chain::script& script,
    uint8_t version, const std::string& prefix)
{
    const uint8_t witness_version = 0;

    switch (version)
    {
        case witness_address::mainnet_bech32_p2wpkh:
        case witness_address::testnet_bech32_p2wpkh:
        case witness_address::mainnet_base58_p2wpkh:
        case witness_address::testnet_base58_p2wpkh:
            return witness_address(ripemd160_hash(script.to_payments_key()),
                version, witness_version, prefix);

        case witness_address::mainnet_bech32_p2wsh:
        case witness_address::testnet_bech32_p2wsh:
        case witness_address::mainnet_base58_p2wsh:
        case witness_address::testnet_base58_p2wsh:
            return witness_address(script.to_payments_key(), version,
                witness_version, prefix);

        default:
            return {};
    }
}

// Cast operators.
// ----------------------------------------------------------------------------

witness_address::operator bool() const
{
    return valid_;
}

// Serializer.
// ----------------------------------------------------------------------------

// Returns the bech32 encoded witness address.
std::string witness_address::bech32() const
{
    base32 bech32;
    bech32.prefix = prefix_;
    bech32.payload = to_chunk(witness_version_);

    const auto converted = (witness_hash_ == null_hash ?
        convert_bits(bech32_expanded_bit_size, bech32_contracted_bit_size, true,
            to_chunk(hash_), 0) : convert_bits(bech32_expanded_bit_size,
                bech32_contracted_bit_size, true, to_chunk(witness_hash_), 0));

    extend_data(bech32.payload, converted);
    return encode_base32(bech32);
}

// Returns the bech32 or base58 encoded witness address, depending on
// the encoding.
std::string witness_address::encoded() const
{
    switch (version_)
    {
        case witness_address::mainnet_base58_p2wpkh:
        case witness_address::testnet_base58_p2wpkh:
        {
            witness_p2wpkh witness;
            to_witness(witness, version_, witness_version_, hash_);
            return encode_base58(witness);
        }

        case witness_address::mainnet_base58_p2wsh:
        case witness_address::testnet_base58_p2wsh:
        {
            witness_p2wsh witness;
            to_witness(witness, version_, witness_version_, witness_hash_);
            return encode_base58(witness);
        }

        case witness_address::mainnet_bech32_p2wpkh:
        case witness_address::mainnet_bech32_p2wsh:
        case witness_address::testnet_bech32_p2wpkh:
        case witness_address::testnet_bech32_p2wsh:
            return bech32();

        default:
            return {};
    }
}

// Accessors.
// ----------------------------------------------------------------------------

uint8_t witness_address::witness_version() const
{
    return witness_version_;
}

const hash_digest& witness_address::witness_hash() const
{
    return witness_hash_;
}

chain::script witness_address::output_script() const
{
    switch (version_)
    {
        case witness_address::mainnet_bech32_p2wpkh:
        case witness_address::testnet_bech32_p2wpkh:
        case witness_address::mainnet_base58_p2wpkh:
        case witness_address::testnet_base58_p2wpkh:
            return chain::script::to_pay_witness_key_hash_pattern(hash_);

        case witness_address::mainnet_bech32_p2wsh:
        case witness_address::testnet_bech32_p2wsh:
        case witness_address::mainnet_base58_p2wsh:
        case witness_address::testnet_base58_p2wsh:
            return chain::script::to_pay_witness_script_hash_pattern(witness_hash_);

        default:
            return {};
    }
}

// Methods.
// ----------------------------------------------------------------------------

/// static
void witness_address::to_witness(witness_p2wpkh& out, uint8_t version,
    uint8_t witness_version, short_hash hash)
{
    build_array(out,
    {
        to_array(version),
        to_array(witness_version),
        to_array(padding),
        hash
    });

    insert_checksum(out);
}

/// static
void witness_address::to_witness(witness_p2wsh& out, uint8_t version,
    uint8_t witness_version, hash_digest hash)
{
    build_array(out,
    {
        to_array(version),
        to_array(witness_version),
        to_array(padding),
        hash
    });

    insert_checksum(out);
}

// Operators.
// ----------------------------------------------------------------------------

witness_address& witness_address::operator=(const witness_address& other)
{
    valid_ = other.valid_;
    version_ = other.version_;
    hash_ = other.hash_;
    prefix_ = other.prefix_;
    witness_version_ = other.witness_version_;
    witness_hash_ = other.witness_hash_;
    return *this;
}

bool witness_address::operator<(const witness_address& other) const
{
    return encoded() < other.encoded();
}

bool witness_address::operator==(const witness_address& other) const
{
    return valid_ == other.valid_ && version_ == other.version_ &&
        prefix_ == other.prefix_ &&
        witness_version_ == other.witness_version_ && hash_ == other.hash_ &&
        witness_hash_ == other.witness_hash_;
}

bool witness_address::operator!=(const witness_address& other) const
{
    return !(*this == other);
}

std::ostream& operator<<(std::ostream& out, const witness_address& of)
{
    out << of.encoded();
    return out;
}

// static
data_chunk witness_address::convert_bits(uint32_t from_bits, uint32_t to_bits,
    bool pad, const data_chunk& in, size_t in_offset)
{
    const uint32_t maximum = (1 << to_bits) - 1;
    const uint32_t maximum_accumulator = (1 << (from_bits + to_bits - 1)) - 1;

    size_t offset = 0;
    uint32_t bit_count = 0;
    uint32_t accumulator = 0;

    data_chunk out;
    for (const auto value: in)
    {
        if (offset++ < in_offset)
            continue;

        accumulator = ((accumulator << from_bits) | value) &
            maximum_accumulator;
        bit_count += from_bits;

        while (bit_count >= to_bits)
        {
            bit_count -= to_bits;
            out.push_back((accumulator >> bit_count) & maximum);
        }
    }

    if (pad && bit_count > 0)
        out.push_back((accumulator << (to_bits - bit_count)) & maximum);
    else if (!pad && (bit_count >= from_bits ||
        ((accumulator << (to_bits - bit_count)) & maximum)))
        return {};

    return out;
}

} // namespace wallet
} // namespace system
} // namespace libbitcoin
