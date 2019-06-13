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
#ifndef LIBBITCOIN_SYSTEM_WALLET_WITNESS_ADDRESS_HPP
#define LIBBITCOIN_SYSTEM_WALLET_WITNESS_ADDRESS_HPP

#include <bitcoin/system/math/hash.hpp>
#include <bitcoin/system/wallet/payment_address.hpp>

namespace libbitcoin {
namespace system {
namespace wallet {

static BC_CONSTEXPR uint8_t padding = 0x00;
static BC_CONSTEXPR size_t witness_p2wpkh_size = 3u + short_hash_size + checksum_size;
static BC_CONSTEXPR size_t witness_p2wsh_size = 3u + hash_size + checksum_size;
typedef byte_array<witness_p2wpkh_size> witness_p2wpkh;
typedef byte_array<witness_p2wsh_size> witness_p2wsh;

/// A class for working with standard witness payment addresses.
class BC_API witness_address
  : public payment_address
{
public:
    enum class encoding: uint8_t
    {
        // BIP 142 p2wpkh and p2wsh address versions.
        // https://github.com/bitcoin/bips/blob/master/bip-0142.mediawiki#specification
        mainnet_base58_p2wpkh = 0x06,
        testnet_base58_p2wpkh = 0x03,
        mainnet_base58_p2wsh = 0x0a,
        testnet_base58_p2wsh = 0x28,

        // These encoding values are not from a specification.
        mainnet_p2wpkh,
        mainnet_p2wsh,
        testnet_p2wpkh,
        testnet_p2wsh,

        unknown
    };

    static const std::string mainnet_prefix;
    static const std::string testnet_prefix;

    typedef std::vector<witness_address> list;
    typedef std::shared_ptr<witness_address> ptr;

    /// Public for unit testing.
    static data_chunk convert_bits(uint32_t from_bits, uint32_t to_bits,
        bool pad, const data_chunk& in, size_t in_offset);

    /// Create base58 witness programs using provided information.
    static void to_witness(witness_p2wpkh& out, encoding out_type,
        uint8_t witness_version, short_hash hash);
    static void to_witness(witness_p2wsh& out, encoding out_type,
        uint8_t witness_version, hash_digest hash);

    /// Constructors.
    witness_address();
    witness_address(witness_address&& other);
    witness_address(const witness_address& other);
    witness_address(const witness_p2wpkh& decoded,
        encoding out_type=encoding::unknown);
    witness_address(const witness_p2wsh& decoded,
        encoding out_type=encoding::unknown);
    witness_address(const data_chunk& data,
        encoding out_type=encoding::unknown);
    witness_address(const std::string& address,
        encoding out_type=encoding::unknown);
    witness_address(short_hash&& hash, encoding out_type=encoding::unknown,
        uint8_t witness_version=0);
    witness_address(const short_hash& hash, encoding out_type=encoding::unknown,
        uint8_t witness_version=0);
    witness_address(hash_digest&& hash, encoding out_type=encoding::unknown,
        uint8_t witness_version=0);
    witness_address(const hash_digest& hash,
        encoding out_type=encoding::unknown,
        uint8_t witness_version=0);
    witness_address(const chain::script& script,
        encoding out_type=encoding::unknown);
    witness_address(const ec_private& secret,
        encoding out_type=encoding::unknown);
    witness_address(const ec_public& point,
        encoding out_type=encoding::unknown);

    /// Operators.
    bool operator<(const witness_address& other) const;
    bool operator==(const witness_address& other) const;
    bool operator!=(const witness_address& other) const;
    witness_address& operator=(const witness_address& other);
    friend std::istream& operator>>(std::istream& in, witness_address& to);
    friend std::ostream& operator<<(std::ostream& out,
        const witness_address& of);

    /// Cast operators.
    operator bool() const;

    /// Serializer.
    std::string bech32(const std::string& prefix=mainnet_prefix) const;
    std::string encoded() const;

    /// Accessors.
    uint8_t witness_version() const;
    const hash_digest& witness_hash() const;
    chain::script output_script() const;

private:
    /// Validators.
    static bool is_address(data_slice decoded);

    /// Factories.
    static witness_address from_string(const std::string& address,
        encoding out_type=encoding::unknown);
    static witness_address from_witness(const witness_p2wpkh& decoded,
        encoding out_type=encoding::unknown);
    static witness_address from_witness(const witness_p2wsh& decoded,
        encoding out_type=encoding::unknown);
    static witness_address from_data(const data_chunk& data,
        encoding out_type=encoding::unknown);
    static witness_address from_script(const chain::script& script,
        encoding out_type=encoding::unknown);
    static witness_address from_private(const ec_private& secret,
        encoding out_type=encoding::unknown);
    static witness_address from_public(const ec_public& point,
        encoding out_type=encoding::unknown);

    encoding encoding_;
    uint8_t witness_version_;
    hash_digest witness_hash_;
};

} // namespace wallet
} // namespace system
} // namespace libbitcoin

// Allow witness_address to be in indexed in std::*map classes.
namespace std
{
template <>
struct hash<bc::system::wallet::witness_address>
{
    size_t operator()(const bc::system::wallet::witness_address& address) const
    {
        return address.witness_hash() == bc::system::null_hash ?
            std::hash<bc::system::short_hash>()(address.hash()) :
            std::hash<bc::system::hash_digest>()(address.witness_hash());
    }
};

} // namespace std

#endif
