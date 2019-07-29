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
static BC_CONSTEXPR size_t witness_prefix_size = 3u;
static BC_CONSTEXPR size_t witness_p2wsh_size = witness_prefix_size +
    hash_size + checksum_size;
static BC_CONSTEXPR size_t witness_p2wpkh_size = witness_prefix_size +
    short_hash_size + checksum_size;

typedef byte_array<witness_p2wsh_size> witness_p2wsh;
typedef byte_array<witness_p2wpkh_size> witness_p2wpkh;

/// A class for working with standard witness payment addresses.
class BC_API witness_address
  : public payment_address
{
  public:
    static const std::string mainnet_prefix;
    static const std::string testnet_prefix;

    static const std::string mainnet_p2wpkh;
    static const std::string testnet_p2wpkh;

    static const std::string mainnet_p2wsh;
    static const std::string testnet_p2wsh;

    static const uint8_t mainnet_bech32_p2wpkh;
    static const uint8_t mainnet_bech32_p2wsh;

    static const uint8_t testnet_bech32_p2wpkh;
    static const uint8_t testnet_bech32_p2wsh;

    static const uint8_t mainnet_base58_p2wpkh;
    static const uint8_t mainnet_base58_p2wsh;

    static const uint8_t testnet_base58_p2wpkh;
    static const uint8_t testnet_base58_p2wsh;

    typedef std::vector<witness_address> list;
    typedef std::shared_ptr<witness_address> ptr;

    /// Create base58 witness programs using provided information.
    static void to_witness(witness_p2wsh& out, uint8_t version,
        uint8_t witness_version, const hash_digest& hash);
    static void to_witness(witness_p2wpkh& out, uint8_t version,
        uint8_t witness_version, const short_hash& hash);

    /// Constructors.
    witness_address();
    witness_address(witness_address&& other);
    witness_address(const witness_address& other);
    witness_address(const witness_p2wpkh& decoded,
        uint8_t version=mainnet_bech32_p2wpkh);
    witness_address(const witness_p2wsh& decoded,
        uint8_t version=mainnet_bech32_p2wpkh);
    witness_address(const std::string& address,
        uint8_t version=mainnet_bech32_p2wpkh);
    witness_address(short_hash&& hash, uint8_t version=mainnet_bech32_p2wpkh,
        uint8_t witness_version=0, const std::string& prefix=mainnet_prefix);
    witness_address(const short_hash& hash,
        uint8_t version=mainnet_bech32_p2wpkh, uint8_t witness_version=0,
        const std::string& prefix=mainnet_prefix);
    witness_address(hash_digest&& hash, uint8_t version=mainnet_bech32_p2wpkh,
        uint8_t witness_version=0, const std::string& prefix=mainnet_prefix);
    witness_address(const hash_digest& hash,
        uint8_t version=mainnet_bech32_p2wpkh, uint8_t witness_version=0,
        const std::string& prefix=mainnet_prefix);
    witness_address(const chain::script& script,
        uint8_t version=mainnet_bech32_p2wpkh,
        const std::string& prefix=mainnet_prefix);
    witness_address(const ec_private& secret,
        uint8_t version=mainnet_bech32_p2wpkh,
        const std::string& prefix=mainnet_prefix);
    witness_address(const ec_public& point,
        uint8_t version=mainnet_bech32_p2wpkh,
        const std::string& prefix=mainnet_prefix);

    /// Operators.
    bool operator<(const witness_address& other) const;
    bool operator==(const witness_address& other) const;
    bool operator!=(const witness_address& other) const;
    witness_address& operator=(const witness_address& other);
    friend std::ostream& operator<<(std::ostream& out,
        const witness_address& of);

    /// Cast operators.
    operator bool() const;

    /// Serializer.
    std::string bech32() const;
    std::string encoded() const;

    /// Accessors.
    uint8_t witness_version() const;
    const hash_digest& witness_hash() const;
    chain::script output_script() const;

protected:
    /// Protected for unit testing.
    static data_chunk convert_bits(uint32_t from_bits, uint32_t to_bits,
        bool pad, const data_chunk& in, size_t in_offset);

private:
    /// Validators.
    static bool is_address(data_slice decoded);

    /// Factories.
    static witness_address from_string(const std::string& address,
        uint8_t version=mainnet_bech32_p2wpkh,
        const std::string& prefix=mainnet_prefix);
    static witness_address from_witness(const witness_p2wpkh& decoded,
        uint8_t version=mainnet_base58_p2wpkh);
    static witness_address from_witness(const witness_p2wsh& decoded,
        uint8_t version=mainnet_base58_p2wsh);
    static witness_address from_script(const chain::script& script,
        uint8_t version=mainnet_bech32_p2wpkh,
        const std::string& prefix=mainnet_prefix);
    static witness_address from_private(const ec_private& secret,
        uint8_t version=mainnet_bech32_p2wpkh,
        const std::string& prefix=mainnet_prefix);
    static witness_address from_public(const ec_public& point,
        uint8_t version=mainnet_bech32_p2wpkh,
        const std::string& prefix=mainnet_prefix);

    std::string prefix_;
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
