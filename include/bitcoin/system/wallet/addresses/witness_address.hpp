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
#ifndef LIBBITCOIN_SYSTEM_WALLET_ADDRESSES_WITNESS_ADDRESS_HPP
#define LIBBITCOIN_SYSTEM_WALLET_ADDRESSES_WITNESS_ADDRESS_HPP

#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <bitcoin/system/chain/chain.hpp>
#include <bitcoin/system/crypto/crypto.hpp>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/wallet/keys/ec_private.hpp>
#include <bitcoin/system/wallet/keys/ec_public.hpp>

namespace libbitcoin {
namespace system {
namespace wallet {

/// A class for working with witness payment addresses.
/// For non-witness addresses, see payment_address.
/// Supports BIP173 (bech32) with BIP350 (bech32m) for non-zero versions.
/// Strict parsing requires that the prefix is known (mainnet or testnet).
/// Strict parsing requires that the program is known (currently version 0).
/// Lax (not strict) allows unknown programs, but not invalid known programs.
class BC_API witness_address
{
  public:
    enum class parse_result
    {
        /// parse_prefix failures.
        prefix_not_ascii,
        prefix_not_lower_case,
        prefix_too_short,
        prefix_too_long,
        prefix_invalid_character,
        prefix_unknown,

        /// parse_address failures.
        address_not_ascii,
        address_mixed_case,
        address_too_long,
        prefix_missing,
        payload_too_short,
        payload_too_long,
        payload_not_base32,
        checksum_invalid,
        version_invalid,
        program_unknown,
        program_invalid,

        valid
    };

    enum class program_type
    {
        version_0_p2kh,
        version_0_p2sh,
        unknown,
        invalid
    };

    typedef std::vector<witness_address> list;
    typedef std::shared_ptr<witness_address> ptr;

    /// BIP173 prefix.
    static const std::string mainnet;
    static const std::string testnet;

    /// BIP173 constants.
    static const char prefix_minimum_character;
    static const char prefix_maximum_character;
    static const size_t prefix_minimum_length;
    static const size_t program_minimum_length;
    static const size_t program_maximum_length;
    static const size_t address_maximum_length;
    static const size_t checksum_length;

    /// BIP141 constants.
    static const size_t program_minimum_size;
    static const size_t program_maximum_size;
    static const size_t version_0_p2kh_program_size;
    static const size_t version_0_p2sh_program_size;

    // TODO: script address extraction, see payment_address.

    /// Parsers.
    static parse_result parse_prefix(const std::string& prefix,
        bool strict=false) NOEXCEPT;
    static program_type parse_program(uint8_t version,
        const data_slice& program, bool strict=false) NOEXCEPT;
    static parse_result parse_address(std::string& out_prefix,
        uint8_t& out_version, data_chunk& out_program,
        const std::string& address, bool strict=false) NOEXCEPT;

    /// Constructors.
    witness_address() NOEXCEPT;
    witness_address(const std::string& address, bool strict=false) NOEXCEPT;
    witness_address(const data_slice& program, const std::string& prefix,
        uint8_t version) NOEXCEPT;

    // version_0_p2kh
    witness_address(const short_hash& public_key_hash,
        const std::string& prefix=mainnet) NOEXCEPT;
    witness_address(const ec_private& secret,
        const std::string& prefix=mainnet) NOEXCEPT;
    witness_address(const ec_public& point,
        const std::string& prefix=mainnet) NOEXCEPT;

    // version_0_p2sh
    // NOTE: ec_secret ends up here, not above in construction of ec_private.
    witness_address(const hash_digest& script_hash,
        const std::string& prefix=mainnet) NOEXCEPT;
    witness_address(const chain::script& script,
        const std::string& prefix=mainnet) NOEXCEPT;

    /// Operators.
    bool operator<(const witness_address& other) const NOEXCEPT;

    friend std::istream& operator>>(std::istream& in, witness_address& to);
    friend std::ostream& operator<<(std::ostream& out,
        const witness_address& of) NOEXCEPT;

    /// Cast operators.
    operator bool() const NOEXCEPT;

    /// Serializer.
    std::string encoded() const NOEXCEPT;

    /// Properties.
    const std::string& prefix() const NOEXCEPT;
    const data_chunk& program() const NOEXCEPT;
    uint8_t version() const NOEXCEPT;
    program_type identifier() const NOEXCEPT;
    chain::script script() const NOEXCEPT;

protected:
    witness_address(const std::string& prefix, uint8_t version,
        data_chunk&& program) NOEXCEPT;

    // Factories.

    // fully specified
    static witness_address from_address(const std::string& address,
        bool strict) NOEXCEPT;
    static witness_address from_parameters(const data_slice& program,
        const std::string& prefix, uint8_t version) NOEXCEPT;

    // version_0_p2kh
    static witness_address from_short(const short_hash& hash,
        const std::string& prefix) NOEXCEPT;
    static witness_address from_private(const ec_private& secret,
        const std::string& prefix) NOEXCEPT;
    static witness_address from_public(const ec_public& point,
        const std::string& prefix) NOEXCEPT;

    // version_0_p2sh
    static witness_address from_long(const hash_digest& hash,
        const std::string& prefix) NOEXCEPT;
    static witness_address from_script(const chain::script& script,
        const std::string& prefix) NOEXCEPT;

private:
    data_chunk program_;
    std::string prefix_;
    uint8_t version_;
};

BC_API bool operator==(const witness_address& left,
    const witness_address& right) NOEXCEPT;
BC_API bool operator!=(const witness_address& left,
    const witness_address& right) NOEXCEPT;

} // namespace wallet
} // namespace system
} // namespace libbitcoin

#endif
