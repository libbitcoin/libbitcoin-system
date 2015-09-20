/**
 * Copyright (c) 2011-2015 libbitcoin developers (see AUTHORS)
 *
 * This file is part of libbitcoin.
 *
 * libbitcoin is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef LIBBITCOIN_WALLET_HD_PRIVATE_KEY_HPP
#define LIBBITCOIN_WALLET_HD_PRIVATE_KEY_HPP

#include <cstdint>
#include <string>
#include <bitcoin/bitcoin/compat.hpp>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/math/ec_keys.hpp>
#include <bitcoin/bitcoin/utility/data.hpp>
#include <bitcoin/bitcoin/wallet/payment_address.hpp>
#include <bitcoin/bitcoin/wallet/hd_public_key.hpp>

namespace libbitcoin {
namespace wallet {

/**
 * An extended private key, as defined by BIP 32.
 */
class BC_API hd_private_key
  : public hd_public_key
{
public:
    static BC_CONSTFUNC uint32_t to_prefix(uint64_t prefixes)
    {
        return prefixes >> 32;
    }

    static BC_CONSTEXPR uint64_t mainnet = uint64_t(0x0488ade4) << 32 |
        hd_public_key::mainnet;

    hd_private_key();
    hd_private_key(const hd_private_key& other);
    hd_private_key(const data_chunk& seed, uint64_t prefixes=mainnet);
    hd_private_key(const hd_key& private_key, uint64_t prefixes=mainnet);
    hd_private_key(const std::string& encoded, uint64_t prefixes=mainnet);

    /// Test for valididity.
    operator const bool() const;

    /// Get the decoded representation (including prefix and checksum).
    operator const hd_key() const;

    /// Get the contained secret.
    operator const ec_secret&() const;

    /// Get the base58 encoded representation.
    std::string encoded() const;

    /// Derive a private key at the specified index.
    hd_private_key derive_private(uint32_t index) const;

    /// Derive a public key at the specified index.
    hd_public_key derive_public(uint32_t index) const override;

private:
    static hd_private_key from_key(const hd_key& decoded, uint64_t prefixes);
    static hd_private_key from_seed(data_slice seed, uint64_t prefixes);
    static hd_private_key from_string(const std::string& encoded,
        uint64_t prefixes);

    hd_private_key(const ec_secret& secret, const chain_code_type& chain_code,
        const hd_key_lineage& lineage);

    const ec_secret secret_;
};

} // namespace wallet
} // namespace libbitcoin

#endif
