/**
 * Copyright (c) 2011-2025 libbitcoin developers (see AUTHORS)
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
#include <bitcoin/system/wallet/keys/ec_private.hpp>

#include <utility>
#include <bitcoin/system/crypto/crypto.hpp>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/radix/radix.hpp>
#include <bitcoin/system/wallet/addresses/payment_address.hpp>
#include <bitcoin/system/wallet/keys/ec_public.hpp>
#include <bitcoin/system/wallet/keys/ec_scalar.hpp>
#include <bitcoin/system/wallet/keys/hd_private.hpp>

namespace libbitcoin {
namespace system {
namespace wallet {

const uint8_t ec_private::compressed_sentinel = 0x01;

const uint8_t ec_private::mainnet_wif = 0x80;
const uint8_t ec_private::mainnet_p2kh = 0x00;
const uint16_t ec_private::mainnet = to_versions(mainnet_p2kh, mainnet_wif);

const uint8_t ec_private::testnet_wif = 0xef;
const uint8_t ec_private::testnet_p2kh = 0x6f;
const uint16_t ec_private::testnet = to_versions(testnet_p2kh, testnet_wif);

ec_private::ec_private() NOEXCEPT
  : ec_private(ec_scalar{})
{
}

ec_private::ec_private(const ec_scalar& scalar, uint8_t address) NOEXCEPT
  : ec_scalar(scalar), compress_(true), versions_(address)
{
}

ec_private::ec_private(const data_chunk& entropy, uint8_t address) NOEXCEPT
  : ec_private(from_entropy(entropy, address))
{
}

ec_private::ec_private(const std::string& wif, uint8_t address) NOEXCEPT
  : ec_private(from_string(wif, address))
{
}

ec_private::ec_private(const wif_compressed& wif, uint8_t address) NOEXCEPT
  : ec_private(from_compressed(wif, address))
{
}

ec_private::ec_private(const wif_uncompressed& wif, uint8_t address) NOEXCEPT
  : ec_private(from_uncompressed(wif, address))
{
}

ec_private::ec_private(const ec_secret& secret, uint16_t versions,
    bool compress) NOEXCEPT
  : ec_scalar(secret), compress_(compress), versions_(versions)
{
}

// Validators.
// ----------------------------------------------------------------------------

bool ec_private::is_wif(const data_slice& decoded) NOEXCEPT
{
    const auto size = decoded.size();
    if (size != wif_compressed_size && size != wif_uncompressed_size)
        return false;

    if (!verify_checksum(to_chunk(decoded)))
        return false;

    return (size == wif_uncompressed_size) ||
        decoded[add1(ec_secret_size)] == compressed_sentinel;
}

// Factories.
// ----------------------------------------------------------------------------

ec_private ec_private::from_string(const std::string& wif,
    uint8_t address) NOEXCEPT
{
    data_chunk decoded;
    if (!decode_base58(decoded, wif) || !is_wif(decoded))
        return {};

    if (decoded.size() == wif_compressed_size)
        return
        { 
            unsafe_array_cast<uint8_t, wif_compressed_size>(decoded.data()),
            address
        };

    return
    {
        unsafe_array_cast<uint8_t, wif_uncompressed_size>(decoded.data()),
        address
    };
}

ec_private ec_private::from_compressed(const wif_compressed& wif,
    uint8_t address) NOEXCEPT
{
    if (!is_wif(wif))
        return {};

    const auto versions = to_versions(address, wif.front());
    const auto secret = slice<one, add1(ec_secret_size)>(wif);
    return { secret, versions, true };
}

ec_private ec_private::from_uncompressed(const wif_uncompressed& wif,
    uint8_t address) NOEXCEPT
{
    if (!is_wif(wif))
        return {};

    const auto versions = to_versions(address, wif.front());
    const auto secret = slice<one, add1(ec_secret_size)>(wif);
    return { secret, versions, false };
}

ec_private ec_private::from_entropy(const data_chunk& entropy,
    uint8_t version) NOEXCEPT
{
    // This technique ensures consistent secrets with BIP32 from a given seed.
    const hd_private key(entropy);

    // The key is invalid if parse256(IL) >= n or 0:
    if (!key)
        return {};

    return { key.secret(), version, true };
}

// Serializer.
// ----------------------------------------------------------------------------

// Conversion to WIF loses payment address version info.
std::string ec_private::encoded() const NOEXCEPT
{
    const auto prefix = to_array(wif_version());

    if (compressed())
    {
        return encode_base58(insert_checksum<wif_compressed_size>(
        {
            prefix, secret(), to_array(compressed_sentinel)
        }));
    }
    else
    {
        return encode_base58(insert_checksum<wif_uncompressed_size>(
        {
            prefix, secret()
        }));
    }
}

// Accessors.
// ----------------------------------------------------------------------------

uint16_t ec_private::versions() const NOEXCEPT
{
    return versions_;
}

uint8_t ec_private::payment_version() const NOEXCEPT
{
    return to_address_version(versions_);
}

uint8_t ec_private::wif_version() const NOEXCEPT
{
    return to_wif_version(versions_);
}

bool ec_private::compressed() const NOEXCEPT
{
    return compress_;
}

// Methods.
// ----------------------------------------------------------------------------

// Conversion to ec_public loses all version information.
// In the case of failure the key is always compressed (ec_compressed_null).
ec_public ec_private::to_public() const NOEXCEPT
{
    if (!(*this))
        return {};

    ec_compressed point;
    if (!secret_to_public(point, secret()))
        return {};

    return { point, compressed() };
}

payment_address ec_private::to_payment_address() const NOEXCEPT
{
    return { *this };
}

// Operators.
// ----------------------------------------------------------------------------

bool ec_private::operator<(const ec_private& other) const NOEXCEPT
{
    return encoded() < other.encoded();
}

bool ec_private::operator==(const ec_private& other) const NOEXCEPT
{
    return
        compress_ == other.compress_ && versions_ == other.versions_ &&
        secret() == other.secret();
}

bool ec_private::operator!=(const ec_private& other) const NOEXCEPT
{
    return !(*this == other);
}

std::istream& operator>>(std::istream& in, ec_private& to)
{
    std::string value;
    in >> value;
    to = ec_private(value);

    if (!to)
        throw istream_exception(value);

    return in;
}

std::ostream& operator<<(std::ostream& out, const ec_private& of) NOEXCEPT
{
    out << of.encoded();
    return out;
}

} // namespace wallet
} // namespace system
} // namespace libbitcoin
