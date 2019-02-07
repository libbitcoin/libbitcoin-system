/**
 * Copyright (c) 2011-2017 libbitcoin developers (see AUTHORS)
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
#ifndef LIBBITCOIN__WALLET_HD_PRIVATE_KEY_HPP
#define LIBBITCOIN__WALLET_HD_PRIVATE_KEY_HPP

#include <cstdint>
//#include <iostream>
//#include <string>
#include <bitcoin/bitcoin/define.hpp>
//#include <bitcoin/bitcoin/math/elliptic_curve.hpp>
//#include <bitcoin/bitcoin/utility/data.hpp>
//#include <bitcoin/bitcoin/wallet/ec_private.hpp>
//#include <bitcoin/bitcoin/wallet/ec_public.hpp>
//#include <bitcoin/bitcoin/wallet/hd_public.hpp>
#include <bitcoin/bitcoin/wallet/hd_private.hpp>
#include <math_ec_secret.hpp>
#include <utility_data_chunk.hpp>
#include <wallet_hd_public.hpp>
#include <wallet_hd_key.hpp>

namespace libbitcoin {
//namespace wallet {
namespace api {

/// An extended private key, as defined by BIP 32.
class BC_API wallet_hd_private
{
public:
//    static const uint64_t mainnet;
//    static const uint64_t testnet;

    static uint32_t to_prefix(uint64_t prefixes)
    {
        return prefixes >> 32;
    }

    static uint64_t to_prefixes(uint32_t private_prefix,
        uint32_t public_prefix)
    {
        return uint64_t(private_prefix) << 32 | public_prefix;
    }

    /// Constructors.
    wallet_hd_private();
    wallet_hd_private(const wallet_hd_private& other);
    wallet_hd_private(const utility_data_chunk& seed, uint64_t prefixes=wallet::hd_private::mainnet);
    wallet_hd_private(const wallet_hd_key& private_key);
    wallet_hd_private(const wallet_hd_key& private_key, uint64_t prefixes);
    wallet_hd_private(const wallet_hd_key& private_key, uint32_t public_prefix);
    wallet_hd_private(const std::string& encoded);
    wallet_hd_private(const std::string& encoded, uint64_t prefixes);
    wallet_hd_private(const std::string& encoded, uint32_t public_prefix);

    /// Operators.
//    bool operator<(const wallet_hd_private& other) const;
    bool operatorlt(const wallet_hd_private& other) const;
//    bool operator==(const wallet_hd_private& other) const;
    bool eq(const wallet_hd_private& other) const;
//    bool operator!=(const wallet_hd_private& other) const;
//    wallet_hd_private& operator=(wallet_hd_private other);
    wallet_hd_private& assign(wallet_hd_private other);
//    friend std::istream& operator>>(std::istream& in, wallet_hd_private& to);
//    friend std::ostream& operator<<(std::ostream& out,
//        const wallet_hd_private& of);

    // Swap implementation required to properly handle base class.
//    friend void swap(wallet_hd_private& left, wallet_hd_private& right);

    /// Cast operators.
//    operator const ec_secret&() const;
    const math_ec_secret& to_ec_secret() const;

    /// Serializer.
    std::string encoded() const;

    /// Accessors.
    const math_ec_secret& secret() const;

    /// Methods.
    wallet_hd_key to_hd_key() const;
    wallet_hd_public to_public() const;
    wallet_hd_private derive_private(uint32_t index) const;
    wallet_hd_public derive_public(uint32_t index) const;

    wallet::hd_private getValue() {
		return value;
	}

	void setValue(wallet::hd_private value) {
		this->value = value;
	}
private:
	wallet::hd_private value;
//    /// Factories.
//    static wallet_hd_private from_seed(data_slice seed, uint64_t prefixes);
//    static wallet_hd_private from_key(const hd_key& decoded, uint32_t prefix);
//    static wallet_hd_private from_key(const hd_key& decoded, uint64_t public_prefix);
//    static wallet_hd_private from_string(const std::string& encoded,
//        uint32_t public_prefix);
//    static wallet_hd_private from_string(const std::string& encoded,
//        uint64_t prefixes);
//
//    wallet_hd_private(const math_ec_secret& secret, const hd_chain_code& chain_code,
//        const hd_lineage& lineage);
//
//    /// Members.
//    /// This should be const, apart from the need to implement assignment.
//    ec_secret secret_;
};

} // namespace api
//} // namespace wallet
} // namespace libbitcoin

#endif
