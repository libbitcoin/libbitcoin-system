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
#include <bitcoin/system/wallet/ec_private.hpp>

#include <cstdint>
#include <iostream>
#include <string>
#include <utility>
#include <boost/program_options.hpp>
#include <bitcoin/system/formats/base_58.hpp>
#include <bitcoin/system/math/checksum.hpp>
#include <bitcoin/system/math/ec_scalar.hpp>
#include <bitcoin/system/math/elliptic_curve.hpp>
#include <bitcoin/system/math/hash.hpp>
#include <bitcoin/system/utility/data.hpp>
#include <bitcoin/system/wallet/ec_public.hpp>
#include <bitcoin/system/wallet/hd_private.hpp>
#include <bitcoin/system/wallet/payment_address.hpp>

namespace libbitcoin {
namespace system {
namespace wallet {

const uint8_t ec_private::compressed_sentinel = 0x01;

const uint8_t ec_private::mainnet_wif = 0x80;
const uint8_t ec_private::mainnet_p2kh = 0x00;
const uint16_t ec_private::mainnet = to_version(mainnet_p2kh, mainnet_wif);

const uint8_t ec_private::testnet_wif = 0xef;
const uint8_t ec_private::testnet_p2kh = 0x6f;
const uint16_t ec_private::testnet = to_version(testnet_p2kh, testnet_wif);

// TODO: review construction for consistency WRT version/address_version.

ec_private::ec_private()
  : ec_private(ec_scalar{})
{
}

ec_private::ec_private(const ec_scalar& scalar, uint8_t address_version)
  : ec_scalar(scalar), compress_(true), version_(address_version)
{
}

ec_private::ec_private(const ec_private& other)
  : ec_scalar(other), compress_(other.compress_), version_(other.version_)
{
}

ec_private::ec_private(const data_chunk& seed, uint8_t address_version)
  : ec_private(from_seed(seed, address_version))
{
}

ec_private::ec_private(const std::string& wif, uint8_t address_version)
  : ec_private(from_string(wif, address_version))
{
}

ec_private::ec_private(const wif_compressed& wif, uint8_t address_version)
  : ec_private(from_compressed(wif, address_version))
{
}

ec_private::ec_private(const wif_uncompressed& wif, uint8_t address_version)
  : ec_private(from_uncompressed(wif, address_version))
{
}

ec_private::ec_private(const ec_secret& secret, uint16_t version, bool compress)
  : ec_scalar(secret), compress_(compress), version_(version)
{
}

// Validators.
// ----------------------------------------------------------------------------

bool ec_private::is_wif(const data_slice& decoded)
{
    const auto size = decoded.size();
    if (size != wif_compressed_size && size != wif_uncompressed_size)
        return false;

    if (!verify_checksum(decoded))
        return false;

    return (size == wif_uncompressed_size) ||
        decoded.data()[1u + ec_secret_size] == compressed_sentinel;
}

// Factories.
// ----------------------------------------------------------------------------

ec_private ec_private::from_seed(const data_chunk& seed,
    uint8_t address_version)
{
    // This technique ensures consistent secrets with BIP32 from a given seed.
    const hd_private key(seed);

    // The key is invalid if parse256(IL) >= n or 0:
    if (!key)
        return {};

    return { key.secret(), address_version, true };
}

ec_private ec_private::from_string(const std::string& wif,
    uint8_t address_version)
{
    data_chunk decoded;
    if (!decode_base58(decoded, wif) || !is_wif(decoded))
        return {};

    if (decoded.size() == wif_compressed_size)
        return { to_array<wif_compressed_size>(decoded), address_version };

    return { to_array<wif_uncompressed_size>(decoded), address_version };
}

ec_private ec_private::from_compressed(const wif_compressed& wif,
    uint8_t address_version)
{
    if (!is_wif(wif))
        return {};

    const auto version = to_version(address_version, wif.front());
    const auto secret = slice<1u, ec_secret_size + 1u>(wif);
    return { secret, version, true };
}

ec_private ec_private::from_uncompressed(const wif_uncompressed& wif,
    uint8_t address_version)
{
    if (!is_wif(wif))
        return {};

    const auto version = to_version(address_version, wif.front());
    const auto secret = slice<1u, ec_secret_size + 1u>(wif);
    return { secret, version, false };
}

// Serializer.
// ----------------------------------------------------------------------------

// Conversion to WIF loses payment address version info.
std::string ec_private::encoded() const
{
    const auto prefix = to_array(wif_version());

    if (compressed())
    {
        return encode_base58(build_checked_array<wif_compressed_size>(
        {
            prefix, secret(), to_array(compressed_sentinel)
        }));
    }
    else
    {
        return encode_base58(build_checked_array<wif_uncompressed_size>(
        {
            prefix, secret()
        }));
    }
}

// Accessors.
// ----------------------------------------------------------------------------

uint16_t ec_private::version() const
{
    return version_;
}

uint8_t ec_private::payment_version() const
{
    return to_p2kh_prefix(version_);
}

uint8_t ec_private::wif_version() const
{
    return to_wif_prefix(version_);
}

bool ec_private::compressed() const
{
    return compress_;
}

// Methods.
// ----------------------------------------------------------------------------

// Conversion to ec_public loses all version information.
// In the case of failure the key is always compressed (ec_compressed_null).
ec_public ec_private::to_public() const
{
    if (!(*this))
        return {};

    ec_compressed point;
    if (!secret_to_public(point, secret()))
        return {};

    return { point, compressed() };
}

payment_address ec_private::to_payment_address() const
{
    return { *this };
}

// Operators.
// ----------------------------------------------------------------------------

ec_private& ec_private::operator=(ec_private other)
{
    swap(*this, other);
    return *this;
}

bool ec_private::operator<(const ec_private& other) const
{
    return encoded() < other.encoded();
}

bool ec_private::operator==(const ec_private& other) const
{
    return
        compress_ == other.compress_ &&
        version_ == other.version_ &&
        *static_cast<const ec_scalar*>(this) == other;
}

bool ec_private::operator!=(const ec_private& other) const
{
    return !(*this == other);
}

std::istream& operator>>(std::istream& in, ec_private& to)
{
    std::string value;
    in >> value;
    to = ec_private(value);

    if (!to)
    {
        using namespace boost::program_options;
        BOOST_THROW_EXCEPTION(invalid_option_value(value));
    }

    return in;
}

std::ostream& operator<<(std::ostream& out, const ec_private& of)
{
    out << of.encoded();
    return out;
}

// friend function, see: stackoverflow.com/a/5695855/1172329
void swap(ec_private& left, ec_private& right)
{
    using std::swap;

    // Must be unqualified (no std namespace).
    swap(static_cast<ec_scalar&>(left), static_cast<ec_scalar&>(right));
    swap(left.compress_, right.compress_);
    swap(left.version_, right.version_);
}

} // namespace wallet
} // namespace system
} // namespace libbitcoin
