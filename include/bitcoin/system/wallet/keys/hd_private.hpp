/**
 * Copyright (c) 2011-2022 libbitcoin developers (see AUTHORS)
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
#ifndef LIBBITCOIN_SYSTEM_WALLET_KEYS_HD_PRIVATE_HPP
#define LIBBITCOIN_SYSTEM_WALLET_KEYS_HD_PRIVATE_HPP

#include <cstdint>
#include <iostream>
#include <string>
#include <bitcoin/system/crypto/crypto.hpp>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/math/math.hpp>
#include <bitcoin/system/wallet/keys/ec_private.hpp>
#include <bitcoin/system/wallet/keys/ec_public.hpp>
#include <bitcoin/system/wallet/keys/hd_public.hpp>

namespace libbitcoin {
namespace system {
namespace wallet {

/// An extended private key, as defined by BIP32.
/// Additional prefix codes are documented in SLIP132.
/// github.com/satoshilabs/slips/blob/master/slip-0132.md
class BC_API hd_private
  : public hd_public
{
public:
    static const uint64_t mainnet;
    static const uint64_t testnet;

    static constexpr uint32_t to_prefix(uint64_t prefixes) noexcept
    {
        return narrow_cast<uint32_t>(shift_right(prefixes, width<uint32_t>()));
    }

    static constexpr uint64_t to_prefixes(uint32_t private_prefix,
        uint32_t public_prefix) noexcept
    {
        return shift_left<uint64_t>(private_prefix, width<uint32_t>()) |
            public_prefix;
    }

    /// Constructors.
    hd_private() noexcept;
    hd_private(const hd_private& other) noexcept;
    hd_private(const data_chunk& entropy, uint64_t prefixes=mainnet) noexcept;
    hd_private(const hd_key& private_key) noexcept;
    hd_private(const hd_key& private_key, uint64_t prefixes) noexcept;
    hd_private(const hd_key& private_key, uint32_t public_prefix) noexcept;
    hd_private(const std::string& encoded) noexcept;
    hd_private(const std::string& encoded, uint64_t prefixes) noexcept;
    hd_private(const std::string& encoded, uint32_t public_prefix) noexcept;
    hd_private(const ec_secret& secret, const hd_chain_code& chain_code,
        uint64_t prefixes=mainnet) noexcept;

    /// Operators.
    bool operator<(const hd_private& other) const noexcept;
    bool operator==(const hd_private& other) const noexcept;
    bool operator!=(const hd_private& other) const noexcept;
    hd_private& operator=(hd_private other) noexcept;
    friend std::istream& operator>>(std::istream& in, hd_private& to);
    friend std::ostream& operator<<(std::ostream& out,
        const hd_private& of) noexcept;

    /// Swap implementation required to properly handle derived class assign.
    friend void swap(hd_private& left, hd_private& right) noexcept;

    /// Cast operators.
    operator const ec_secret&() const noexcept;

    /// Serializer.
    std::string encoded() const noexcept;

    /// Accessors.
    const ec_secret& secret() const noexcept;

    /// Methods.
    hd_key to_hd_key() const noexcept;
    hd_public to_public() const noexcept;
    hd_private derive_private(uint32_t index) const noexcept;
    hd_public derive_public(uint32_t index) const noexcept;

private:
    /// Factories.
    static hd_private from_entropy(const data_slice& seed,
        uint64_t prefixes) noexcept;
    static hd_private from_key(const hd_key& decoded,
        uint32_t public_prefix) noexcept;
    static hd_private from_key(const hd_key& decoded,
        uint64_t prefixes) noexcept;
    static hd_private from_private(const ec_secret& secret,
        const hd_chain_code& chain_code, uint64_t prefixes) noexcept;
    static hd_private from_string(const std::string& encoded,
        uint32_t public_prefix) noexcept;
    static hd_private from_string(const std::string& encoded,
        uint64_t prefixes) noexcept;

    hd_private(const ec_secret& secret, const hd_chain_code& chain_code,
        const hd_lineage& lineage) noexcept;

    /// Members.
    /// This should be const, apart from the need to implement assignment.
    ec_secret secret_;
};

} // namespace wallet
} // namespace system
} // namespace libbitcoin

#endif
