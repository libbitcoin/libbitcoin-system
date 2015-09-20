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
#ifndef LIBBITCOIN_WALLET_HD_PUBLIC_KEY_HPP
#define LIBBITCOIN_WALLET_HD_PUBLIC_KEY_HPP

#include <cstdint>
#include <string>
#include <bitcoin/bitcoin/compat.hpp>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/math/ec_keys.hpp>
#include <bitcoin/bitcoin/utility/data.hpp>
#include <bitcoin/bitcoin/wallet/payment_address.hpp>

namespace libbitcoin {
namespace wallet {

static BC_CONSTEXPR uint32_t hd_first_hardened_key = 1 << 31;

/**
 * An hd key chain code.
 */
static BC_CONSTEXPR size_t hd_chain_code_size = 32;
typedef byte_array<hd_chain_code_size> chain_code_type;

/**
 * A decoded hd public or private key.
 */
static BC_CONSTEXPR size_t hd_key_size = 82;
typedef byte_array<hd_key_size> hd_key;

/**
 * Key derivation information used in the serialization format.
 */
struct BC_API hd_key_lineage
{
    uint64_t prefixes;
    uint8_t depth;
    uint32_t parent_fingerprint;
    uint32_t child_number;
};

/**
 * An extended public key, as defined by BIP 32.
 */
class BC_API hd_public_key
{
public:
    static BC_CONSTFUNC uint32_t to_prefix(uint64_t prefixes)
    {
        return prefixes & 0x00000000FFFFFFFF;
    }

    static BC_CONSTEXPR uint32_t mainnet = 0x0488b21eu;

    hd_public_key();
    hd_public_key(const hd_public_key& other);
    hd_public_key(const hd_key& public_key, uint32_t prefix=mainnet);
    hd_public_key(const std::string& encoded, uint32_t prefix=mainnet);

    /// Test for valididity.
    operator const bool() const;

    /// Get the decoded representation (including prefix and checksum).
    operator const hd_key() const;

    /// Get the contained point.
    operator const ec_compressed&() const;

    /// Get the base58 encoded representation.
    std::string encoded() const;
    const chain_code_type& chain_code() const;
    const hd_key_lineage& lineage() const;

    /// Derive a public key at the specified index.
    virtual hd_public_key derive_public(uint32_t index) const;

protected:
    static hd_public_key from_secret(const ec_secret& secret,
        const chain_code_type& chain_code, const hd_key_lineage& lineage);

    uint32_t fingerprint() const;

    bool valid_;
    const chain_code_type chain_;
    const hd_key_lineage lineage_;
    ec_compressed point_;

private:
    static hd_public_key from_key(const hd_key& public_key, uint32_t prefix);
    static hd_public_key from_string(const std::string& encoded, uint32_t prefix);

    hd_public_key(const ec_compressed& point,
        const chain_code_type& chain_code, const hd_key_lineage& lineage);
};

} // namespace wallet
} // namespace libbitcoin

#endif
