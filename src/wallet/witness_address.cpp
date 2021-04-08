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

// BIP173 prefix constants.
// github.com/bitcoin/bips/blob/master/bip-0173.mediawiki#segwit-address-format
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
    format_(other.format_),
    witness_version_(other.witness_version_),
    witness_hash_(std::move(other.witness_hash_))
{
}

witness_address::witness_address(const witness_address& other)
  : payment_address(other),
    prefix_(other.prefix_),
    format_(other.format_),
    witness_hash_(other.witness_hash_)
{
}

witness_address::witness_address(const std::string& address,
    address_format format)
  : witness_address(from_string(address, format))
{
}

witness_address::witness_address(const ec_private& secret,
    address_format format, const std::string& prefix)
  : witness_address(from_private(secret, format, prefix))
{
}

witness_address::witness_address(const ec_public& point, address_format format,
    const std::string& prefix)
  : witness_address(from_public(point, format, prefix))
{
}

witness_address::witness_address(const chain::script& script,
    address_format format, const std::string& prefix)
  : witness_address(from_script(script, format, prefix))
{
}

witness_address::witness_address(short_hash&& hash, address_format format,
    uint8_t witness_version, const std::string& prefix)
  : payment_address(std::move(hash)),
    prefix_(prefix),
    format_(format),
    witness_version_(witness_version),
    witness_hash_(null_hash)
{
}

witness_address::witness_address(const short_hash& hash, address_format format,
    uint8_t witness_version, const std::string& prefix)
  : payment_address(hash),
    prefix_(prefix),
    format_(format),
    witness_version_(witness_version),
    witness_hash_(null_hash)
{
}

witness_address::witness_address(hash_digest&& hash, address_format format,
    uint8_t witness_version, const std::string& prefix)
  : payment_address(null_short_hash),
    prefix_(prefix),
    format_(format),
    witness_version_(witness_version),
    witness_hash_(std::move(hash))
{
}

witness_address::witness_address(const hash_digest& hash, address_format format,
    uint8_t witness_version, const std::string& prefix)
  : payment_address(null_short_hash),
    prefix_(prefix),
    format_(format),
    witness_version_(witness_version),
    witness_hash_(hash)
{
}

// Factories.
// ----------------------------------------------------------------------------
// static
witness_address witness_address::from_string(const std::string& address,
    address_format format, const std::string& prefix)
{
    // BIP 141 constants
    static constexpr size_t witness_program_min_size = 2;
    static constexpr size_t witness_program_max_size = 40;

    // BIP 173 witness version 0 constants
    static constexpr size_t witness_pubkey_hash_size = 42;
    static constexpr size_t witness_script_hash_size = 62;
    static constexpr size_t bech32_address_min_size = 14;
    static constexpr size_t bech32_address_max_size = 74;
    static constexpr size_t bech32_address_size_modulo = 8;
    static constexpr size_t bech32_address_modulo_invalid_1 = 0;
    static constexpr size_t bech32_address_modulo_invalid_2 = 3;
    static constexpr size_t bech32_address_modulo_invalid_3 = 5;

    // Attempt to decode BIP 173 address format.
    base32 bech32_decoded;
    if (!decode_base32(bech32_decoded, address))
        return {};

    const uint8_t witness_version = bech32_decoded.payload.front();

    // Checks specific to witness version 0.
    if (witness_version == 0 && (address.size() < witness_pubkey_hash_size ||
        address.size() > witness_script_hash_size))
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

    if (converted.size() == hash_size)
    {
        auto hash = to_array<hash_size>(converted);
        return { std::move(hash), format, witness_version, prefix };
    }

    BITCOIN_ASSERT(converted.size() == short_hash_size);
    auto hash = to_array<short_hash_size>(converted);
    return { std::move(hash), format, witness_version, prefix };
}

// static
witness_address witness_address::from_private(const ec_private& secret,
    address_format format, const std::string& prefix)
{
    return secret ? witness_address(secret.to_public(), format, prefix) : 
        witness_address{};
}

// static
witness_address witness_address::from_public(const ec_public& point,
    address_format format, const std::string& prefix)
{
    if (!point)
        return {};

    data_chunk data;
    if (!point.to_data(data))
        return {};

    // Witness version.
    const uint8_t version = 0;

    return format == address_format::witness_pubkey_hash ?
        witness_address(bitcoin_short_hash(data), format, version, prefix) :
        witness_address(bitcoin_hash(data), format, version, prefix);
}

// static
witness_address witness_address::from_script(const chain::script& script,
    address_format format, const std::string& prefix)
{
    // Witness version.
    const uint8_t version = 0;
    const auto key = script.to_payments_key();

    return format == address_format::witness_pubkey_hash ?
        witness_address(ripemd160_hash(key), format, version, prefix) :
        witness_address(key, format, version, prefix);
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
std::string witness_address::encoded() const
{
    const size_t conversion_offset = 0;
    const auto hash = witness_hash_ == null_hash ? to_chunk(hash_) : 
        to_chunk(witness_hash_);

    base32 bech32;
    bech32.prefix = prefix_;
    bech32.payload = build_chunk(
    {
        data_chunk{ witness_version_ },
        convert_bits(bech32_expanded_bit_size, bech32_contracted_bit_size,
            true, hash, conversion_offset)
    });

    return encode_base32(bech32);
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
    return format_ == address_format::witness_pubkey_hash ?
        chain::script::to_pay_witness_key_hash_pattern(hash_) :
        chain::script::to_pay_witness_script_hash_pattern(witness_hash_);
}

// Operators.
// ----------------------------------------------------------------------------

witness_address& witness_address::operator=(const witness_address& other)
{
    valid_ = other.valid_;
    hash_ = other.hash_;
    prefix_ = other.prefix_;
    format_ = other.format_;
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
    return valid_ == other.valid_
        && version_ == other.version_
        && prefix_ == other.prefix_
        && format_ == other.format_
        && witness_version_ == other.witness_version_
        && hash_ == other.hash_
        && witness_hash_ == other.witness_hash_;
}

bool witness_address::operator!=(const witness_address& other) const
{
    return !(*this == other);
}

std::istream& operator>>(std::istream& in, witness_address& to)
{
    std::string value;
    in >> value;
    to = witness_address(value);

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

        accumulator = ((accumulator << from_bits) | value);
        accumulator &= maximum_accumulator;

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
