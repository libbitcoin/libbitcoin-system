/**
 * Copyright (c) 2011-2023 libbitcoin developers (see AUTHORS)
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
#ifndef LIBBITCOIN_SYSTEM_WALLET_KEYS_EC_PRIVATE_HPP
#define LIBBITCOIN_SYSTEM_WALLET_KEYS_EC_PRIVATE_HPP

#include <iostream>
#include <string>
#include <bitcoin/system/crypto/crypto.hpp>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/math/math.hpp>
#include <bitcoin/system/wallet/keys/ec_public.hpp>
#include <bitcoin/system/wallet/keys/ec_scalar.hpp>

namespace libbitcoin {
namespace system {
namespace wallet {

class payment_address;

/// Private keys with public key compression metadata:
static constexpr size_t wif_uncompressed_size = 37u;
typedef data_array<wif_uncompressed_size> wif_uncompressed;

static constexpr size_t wif_compressed_size = wif_uncompressed_size + 1u;
typedef data_array<wif_compressed_size> wif_compressed;

/// Use to pass an ec secret with compression and version information.
class BC_API ec_private
  : public ec_scalar
{
public:
    static const uint8_t compressed_sentinel;

    static const uint8_t mainnet_wif;
    static const uint8_t mainnet_p2kh;

    static const uint8_t testnet_wif;
    static const uint8_t testnet_p2kh;

    // WIF carries a compression flag for payment address generation but
    // assumes a mapping to payment address version. This is insufficient
    // as a parameterized mapping is required, so we use the same technique as
    // with hd keys, merging the two necessary values into one.
    static const uint16_t mainnet;
    static const uint16_t testnet;

    static constexpr uint8_t to_address_version(uint16_t versions) NOEXCEPT
    {
        return narrow_cast<uint8_t>(versions);
    }

    static constexpr uint8_t to_wif_version(uint16_t versions) NOEXCEPT
    {
        return narrow_cast<uint8_t>(shift_right(versions, bits<uint8_t>));
    }

    static constexpr uint16_t to_versions(uint8_t address, uint8_t wif) NOEXCEPT
    {
        return shift_left<uint16_t>(wif, bits<uint8_t>) | address;
    }

    /// Constructors.
    ec_private() NOEXCEPT;
    ec_private(const ec_scalar& scalar,
        uint8_t address=mainnet_p2kh) NOEXCEPT;
    ec_private(const data_chunk& entropy,
        uint8_t address=mainnet_p2kh) NOEXCEPT;
    ec_private(const std::string& wif,
        uint8_t address=mainnet_p2kh) NOEXCEPT;
    ec_private(const wif_compressed& wif,
        uint8_t address=mainnet_p2kh) NOEXCEPT;
    ec_private(const wif_uncompressed& wif,
        uint8_t address=mainnet_p2kh) NOEXCEPT;

    /// The version is 16 bits. The most significant byte is the WIF prefix and
    /// the least significant byte is the address perfix. 0x8000 by default.
    ec_private(const ec_secret& secret, uint16_t versions=mainnet,
        bool compress=true) NOEXCEPT;

    /// Operators.
    bool operator<(const ec_private& other) const NOEXCEPT;
    bool operator==(const ec_private& other) const NOEXCEPT;
    bool operator!=(const ec_private& other) const NOEXCEPT;
    friend std::istream& operator>>(std::istream& in, ec_private& to);
    friend std::ostream& operator<<(std::ostream& out,
        const ec_private& of) NOEXCEPT;

    /// Serializer.
    std::string encoded() const NOEXCEPT;

    /// Accessors.
    uint16_t versions() const NOEXCEPT;
    uint8_t payment_version() const NOEXCEPT;
    uint8_t wif_version() const NOEXCEPT;
    bool compressed() const NOEXCEPT;

    /// Methods.
    ec_public to_public() const NOEXCEPT;
    payment_address to_payment_address() const NOEXCEPT;

private:
    /// Validators.
    static bool is_wif(const data_slice& decoded) NOEXCEPT;

    /// Factories.
    static ec_private from_string(const std::string& wif,
        uint8_t address) NOEXCEPT;
    static ec_private from_entropy(const data_chunk& entropy,
        uint8_t address) NOEXCEPT;
    static ec_private from_compressed(const wif_compressed& wif,
        uint8_t address) NOEXCEPT;
    static ec_private from_uncompressed(const wif_uncompressed& wif,
        uint8_t address) NOEXCEPT;

    /// Members.
    /// These should be const, apart from the need to implement assignment.
    bool compress_;
    uint16_t versions_;
};

} // namespace wallet
} // namespace system
} // namespace libbitcoin

#endif
