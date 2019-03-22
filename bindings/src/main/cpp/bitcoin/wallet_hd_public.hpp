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
#ifndef LIBBITCOIN__WALLET_HD_PUBLIC_KEY_HPP
#define LIBBITCOIN__WALLET_HD_PUBLIC_KEY_HPP

//#include <cstdint>
//#include <iostream>
//#include <string>
#include <bitcoin/bitcoin/define.hpp>
//#include <bitcoin/bitcoin/math/elliptic_curve.hpp>
#include <math_ec_compressed.hpp>
//#include <bitcoin/bitcoin/utility/data.hpp>
//#include <bitcoin/bitcoin/wallet/ec_public.hpp>
#include <wallet_hd_key.hpp>
#include <wallet_hd_chain_code.hpp>

namespace libbitcoin {
//namespace wallet {
namespace api {

/// A constant used in key derivation.
//static BC_CONSTEXPR uint32_t hd_first_hardened_key = 1 << 31;

/// An hd key chain code.
//static BC_CONSTEXPR size_t hd_chain_code_size = 32;
//typedef byte_array<hd_chain_code_size> hd_chain_code;

/// A decoded hd public or private key.
//static BC_CONSTEXPR size_t hd_key_size = 82;
//typedef byte_array<hd_key_size> hd_key;

/// Key derivation information used in the serialization format.
class BC_API wallet_hd_lineage
{
public:
//    uint64_t prefixes;
//    uint8_t depth;
//    uint32_t parent_fingerprint;
//    uint32_t child_number;

//    bool operator==(const hd_lineage& other) const;
    bool eq(const wallet_hd_lineage& other) const;
//    bool operator!=(const hd_lineage& other) const;
    wallet::hd_lineage getValue() {
		return value;
	}

	void setValue(wallet::hd_lineage value) {
		this->value = value;
	}
private:
	wallet::hd_lineage value;
};

//class hd_private;

/// An extended public key, as defined by BIP 32.
class BC_API wallet_hd_public
{
public:
//    static const uint32_t mainnet;
//    static const uint32_t testnet;

    static uint32_t to_prefix(uint64_t prefixes)
    {
        return prefixes & 0x00000000FFFFFFFF;
    }

    /// Constructors.
    wallet_hd_public();
    wallet_hd_public(const wallet_hd_public& other);
    wallet_hd_public(const wallet_hd_key& public_key);
    wallet_hd_public(const wallet_hd_key& public_key, uint32_t prefix);
    wallet_hd_public(const std::string& encoded);
    wallet_hd_public(const std::string& encoded, uint32_t prefix);

    /// Operators.
//    bool operator<(const wallet_hd_public& other) const;
    bool lt(const wallet_hd_public& other) const;
//    bool operator==(const wallet_hd_public& other) const;
    bool eq(const wallet_hd_public& other) const;
//    bool operator!=(const wallet_hd_public& other) const;
//    wallet_hd_public& operator=(const wallet_hd_public& other);
    wallet_hd_public& assign(const wallet_hd_public& other);
//    friend std::istream& operator>>(std::istream& in, wallet_hd_public& to);
//    friend std::ostream& operator<<(std::ostream& out,
//        const wallet_hd_public& of);

    /// Cast operators.
//    operator bool() const;
    bool toBoolean() const;
//    operator const ec_compressed&() const;
    const math_ec_compressed& to_ec_compressed() const;

    /// Serializer.
    std::string encoded() const;

    /// Accessors.
    const wallet_hd_chain_code& chain_code() const;
    const wallet_hd_lineage& lineage() const;
    const math_ec_compressed& point() const;

    /// Methods.
    wallet_hd_key to_hd_key() const;
    wallet_hd_public derive_public(uint32_t index) const;

//protected:
//    /// Factories.
//    static wallet_hd_public from_secret(const ec_secret& secret,
//        const hd_chain_code& chain_code, const hd_lineage& lineage);
//
//    /// Helpers.
//    uint32_t fingerprint() const;

//    /// Members.
//    /// These should be const, apart from the need to implement assignment.
//    bool valid_;
//    hd_chain_code chain_;
//    hd_lineage lineage_;
//    ec_compressed point_;

    wallet::hd_public getValue() {
		return value;
	}

	void setValue(wallet::hd_public value) {
		this->value = value;
	}
private:
	wallet::hd_public value;
//    static wallet_hd_public from_key(const hd_key& public_key);
//    static wallet_hd_public from_string(const std::string& encoded);
//    static wallet_hd_public from_key(const hd_key& public_key, uint32_t prefix);
//    static wallet_hd_public from_string(const std::string& encoded, uint32_t prefix);
//
//    wallet_hd_public(const ec_compressed& point,
//        const hd_chain_code& chain_code, const hd_lineage& lineage);
};

} // namespace api
//} // namespace wallet
} // namespace libbitcoin

#endif
