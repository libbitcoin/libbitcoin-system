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

// BIP 173 prefix constants.
// https://github.com/bitcoin/bips/blob/master/bip-0173.mediawiki#segwit-address-format
const std::string witness_address::mainnet_prefix = "bc";
const std::string witness_address::testnet_prefix = "tb";


// Discovery based on string is not ideal.  This use case is for
// accepting a witness_address as a string without a further specified
// encoding type.
static witness_address::encoding get_bech32_encoding(const std::string& address,
    size_t hash_length)
{
    const auto prefix = address.substr(0, 2);
    if (prefix == "bc")
        return hash_length == short_hash_size ?
            witness_address::encoding::mainnet_p2wpkh :
            witness_address::encoding::mainnet_p2wsh;
    else if (prefix == "tb")
        return hash_length == short_hash_size ?
            witness_address::encoding::testnet_p2wpkh :
            witness_address::encoding::testnet_p2wsh;

    return witness_address::encoding::unknown;
}

static witness_address::encoding get_base58_encoding(const std::string& address,
    size_t decoded_length)
{
    if (decoded_length == witness_p2wpkh_size)
    {
        const auto prefix = address.substr(0, 2);
        return prefix == "p2" ? witness_address::encoding::mainnet_base58_p2wpkh :
            witness_address::encoding::testnet_base58_p2wpkh;
    }
    else if (decoded_length == witness_p2wsh_size)
    {
        const auto prefix = address.substr(0, 3);
        return prefix == "7Xh" ? witness_address::encoding::mainnet_base58_p2wsh :
            witness_address::encoding::testnet_base58_p2wsh;
    }

    return witness_address::encoding::unknown;
}

witness_address::witness_address()
  : payment_address(),
    encoding_(encoding::unknown),
    witness_version_(0),
    witness_hash_(null_hash)
{
}

witness_address::witness_address(witness_address&& other)
  : payment_address(other),
    encoding_(other.encoding_),
    witness_version_(other.witness_version_),
    witness_hash_(std::move(other.witness_hash_))
{
}

witness_address::witness_address(const witness_address& other)
  : payment_address(other),
    encoding_(other.encoding_),
    witness_version_(other.witness_version_),
    witness_hash_(other.witness_hash_)
{
}

witness_address::witness_address(const witness_p2wpkh& decoded,
    encoding out_type)
  : witness_address(from_witness(decoded, out_type))
{
}

witness_address::witness_address(const witness_p2wsh& decoded,
    encoding out_type)
  : witness_address(from_witness(decoded, out_type))
{
}

witness_address::witness_address(const data_chunk& data, encoding out_type)
  : witness_address(from_data(data, out_type))
{
}

witness_address::witness_address(const std::string& address, encoding out_type)
  : witness_address(from_string(address, out_type))
{
}

witness_address::witness_address(const ec_private& secret, encoding out_type)
  : witness_address(from_private(secret, out_type))
{
}

witness_address::witness_address(const ec_public& point, encoding out_type)
  : witness_address(from_public(point, out_type))
{
}

witness_address::witness_address(const chain::script& script, encoding out_type)
  : witness_address(from_script(script, out_type))
{
}

witness_address::witness_address(short_hash&& hash, encoding out_type,
    uint8_t witness_version)
  : payment_address(std::move(hash)),
    encoding_(out_type),
    witness_version_(witness_version),
    witness_hash_(null_hash)
{
}

witness_address::witness_address(const short_hash& hash, encoding out_type,
    uint8_t witness_version)
  : payment_address(hash),
    encoding_(out_type),
    witness_version_(witness_version),
    witness_hash_(null_hash)
{
}

witness_address::witness_address(hash_digest&& hash, encoding out_type,
    uint8_t witness_version)
  : payment_address(null_short_hash),
    encoding_(out_type),
    witness_version_(witness_version),
    witness_hash_(std::move(hash))
{
}

witness_address::witness_address(const hash_digest& hash, encoding out_type,
    uint8_t witness_version)
  : payment_address(null_short_hash),
    encoding_(out_type),
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
    encoding out_type)
{
    // Attempt to decode BIP 173 address format.
    base32 bech32_decoded;
    if (decode_base32(bech32_decoded, address))
    {
        const uint8_t witness_version = bech32_decoded.payload.front();

        // Checks specific to witness version 0.
        if (witness_version == 0 && (address.size() < 42 || address.size() > 62))
            return {};

        // Verify witness version is valid (only version 0 is
        // currently supported).
        if (witness_version != 0)
            return {};

        // Verify additional properties (BIP173 decoding).
        if (address.size() < 14 || address.size() > 74)
            return {};

        const auto address_mod = address.size() % 8;
        if (address_mod == 0 || address_mod == 3 || address_mod == 5)
            return {};

        const auto converted = convert_bits(5, 8, false,
            bech32_decoded.payload, 1);
        if (converted.size() < 2 || converted.size() > 40)
            return {};

        if (out_type == encoding::unknown)
            out_type = get_bech32_encoding(address, converted.size());

        // Reinterpret casting avoids a copy, but should probably be replaced.
        if (converted.size() == hash_size)
            return { *reinterpret_cast<const hash_digest*>(&converted[0]),
                out_type, witness_version };
        else if (converted.size() == short_hash_size)
            return { *reinterpret_cast<const short_hash*>(&converted[0]),
                out_type, witness_version };

        // This is a witness_version 0 of an unsupported length.
        return {};
    }

    // Attempt to decode BIP 142 address format.
    data_chunk decoded;
    if (!decode_base58(decoded, address) || !is_address(decoded))
        return {};

    if (out_type == encoding::unknown)
        out_type = get_base58_encoding(address, decoded.size());

    // Reinterpret casting avoids a copy, but should probably be replaced.
    if (decoded.size() == witness_p2wpkh_size)
        return { *reinterpret_cast<witness_p2wpkh*>(&decoded[0]), out_type };
    else if (decoded.size() == witness_p2wsh_size)
        return { *reinterpret_cast<witness_p2wsh*>(&decoded[0]), out_type };

    // Address is an unrecognized format.
    return {};
}

// static
witness_address witness_address::from_data(const data_chunk& data,
    encoding out_type)
{
    switch (out_type)
    {
        case encoding::mainnet_p2wpkh:
        case encoding::testnet_p2wpkh:
            return witness_address(ripemd160_hash(sha256_hash(data)), out_type);

        case encoding::mainnet_p2wsh:
        case encoding::testnet_p2wsh:
            return witness_address(sha256_hash(data), out_type);

        default:
            return {};
    }
}

// static
witness_address witness_address::from_witness(const witness_p2wpkh& decoded,
    encoding out_type)
{
    if (!is_address(decoded))
        return {};

    const short_hash hash = slice<3, short_hash_size + 3>(decoded);
    return { hash, out_type };
}

// static
witness_address witness_address::from_witness(const witness_p2wsh& decoded,
    encoding out_type)
{
    if (!is_address(decoded))
        return {};

    const hash_digest hash = slice<3, hash_size + 3>(decoded);
    return { hash, out_type };
}

// static
witness_address witness_address::from_private(const ec_private& secret,
    encoding out_type)
{
    if (secret)
        return { secret.to_public(), out_type };

    return {};
}

// static
witness_address witness_address::from_public(const ec_public& point,
    encoding out_type)
{
    if (!point)
        return {};

    data_chunk data;
    if (!point.to_data(data))
        return {};

    switch (out_type)
    {
        case encoding::mainnet_base58_p2wsh:
        case encoding::testnet_base58_p2wsh:
        case encoding::mainnet_p2wsh:
        case encoding::testnet_p2wsh:
            return { bitcoin_hash(data), out_type };

        case encoding::mainnet_base58_p2wpkh:
        case encoding::testnet_base58_p2wpkh:
        case encoding::mainnet_p2wpkh:
        case encoding::testnet_p2wpkh:
        default: // if no encoding is set, default to p2wpkh
            return { bitcoin_short_hash(data), out_type };
    }
}

// static
witness_address witness_address::from_script(const chain::script& script,
    encoding out_type)
{
    switch (out_type)
    {
        case encoding::mainnet_base58_p2wpkh:
        case encoding::testnet_base58_p2wpkh:
        case encoding::mainnet_p2wpkh:
        case encoding::testnet_p2wpkh:
            return witness_address(ripemd160_hash(sha256_hash(
                script.to_data(false))), out_type);

        case encoding::mainnet_base58_p2wsh:
        case encoding::testnet_base58_p2wsh:
        case encoding::mainnet_p2wsh:
        case encoding::testnet_p2wsh:
            return witness_address(sha256_hash(script.to_data(false)),
                out_type);

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
std::string witness_address::bech32(const std::string& prefix) const
{
    base32 bech32;
    bech32.prefix = prefix;
    bech32.payload = to_chunk(witness_version_);

    const auto converted = (witness_hash_ == null_hash ?
        convert_bits(8, 5, true, to_chunk(hash_), 0) :
        convert_bits(8, 5, true, to_chunk(witness_hash_), 0));

    extend_data(bech32.payload, converted);
    return encode_base32(bech32);
}

// Returns the bech32 or base58 encoded witness address, depending on
// the encoding.
std::string witness_address::encoded() const
{
    switch (encoding_)
    {
        case encoding::mainnet_base58_p2wpkh:
        case encoding::testnet_base58_p2wpkh:
        {
            witness_p2wpkh witness;
            to_witness(witness, encoding_, witness_version_, hash_);
            return encode_base58(witness);
        }

        case encoding::mainnet_base58_p2wsh:
        case encoding::testnet_base58_p2wsh:
        {
            witness_p2wsh witness;
            to_witness(witness, encoding_, witness_version_, witness_hash_);
            return encode_base58(witness);
        }

        case encoding::mainnet_p2wpkh:
        case encoding::mainnet_p2wsh:
            return bech32(mainnet_prefix);

        case encoding::testnet_p2wpkh:
        case encoding::testnet_p2wsh:
            return bech32(testnet_prefix);

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
    switch (encoding_)
    {
        case encoding::mainnet_p2wpkh:
        case encoding::testnet_p2wpkh:
        case encoding::mainnet_base58_p2wpkh:
        case encoding::testnet_base58_p2wpkh:
            return chain::script::to_pay_witness_key_hash_pattern(hash_);

        case encoding::mainnet_p2wsh:
        case encoding::testnet_p2wsh:
        case encoding::mainnet_base58_p2wsh:
        case encoding::testnet_base58_p2wsh:
            return chain::script::to_pay_witness_script_hash_pattern(witness_hash_);

        default:
            return {};
    }
}

// Methods.
// ----------------------------------------------------------------------------

/// static
void witness_address::to_witness(witness_p2wpkh& out, encoding out_type,
    uint8_t witness_version, short_hash hash)
{
    build_array(out,
    {
        to_array(static_cast<uint8_t>(out_type)),
        to_array(witness_version),
        to_array(padding),
        hash
    });

    insert_checksum(out);
}

/// static
void witness_address::to_witness(witness_p2wsh& out, encoding out_type,
    uint8_t witness_version, hash_digest hash)
{
    build_array(out,
    {
        to_array(static_cast<uint8_t>(out_type)),
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
    encoding_ = other.encoding_;
    witness_version_ = other.witness_version_;
    hash_ = other.hash_;
    witness_hash_ = other.witness_hash_;
    return *this;
}

bool witness_address::operator<(const witness_address& other) const
{
    return encoded() < other.encoded();
}

bool witness_address::operator==(const witness_address& other) const
{
    return valid_ == other.valid_ && encoding_ == other.encoding_ &&
        witness_version_ == other.witness_version_ &&
        hash_ == other.hash_ && witness_hash_ == other.witness_hash_;
}

bool witness_address::operator!=(const witness_address& other) const
{
    return !(*this == other);
}

std::istream& operator>>(std::istream& in, witness_address& to)
{
    static constexpr size_t p2wpkh_address_size = 42;
    static constexpr size_t p2wsh_address_size = 62;
    static constexpr size_t base58_p2wpkh_address_size = 36;
    static constexpr size_t base58_p2wsh_address_size = 53;

    std::string value;
    in >> value;

    // witness version 0 only supported.
    witness_address::encoding out_type;
    switch(value.size())
    {
        case p2wpkh_address_size:
            out_type = get_bech32_encoding(value, short_hash_size);
            break;

        case p2wsh_address_size:
            out_type = get_bech32_encoding(value, hash_size);
            break;

        case base58_p2wpkh_address_size:
            out_type = get_base58_encoding(value, witness_p2wpkh_size);
            break;

        case base58_p2wsh_address_size:
            out_type = get_base58_encoding(value, witness_p2wsh_size);
            break;

        default:
            out_type = witness_address::encoding::unknown;
            break;
    }

    to = witness_address(value, out_type);

    if (!to)
    {
        using namespace boost::program_options;
        BOOST_THROW_EXCEPTION(invalid_option_value(value));
    }

    return in;
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
