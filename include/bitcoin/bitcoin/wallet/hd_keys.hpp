/*
 * Copyright (c) 2011-2013 libbitcoin developers (see AUTHORS)
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
#ifndef LIBBITCOIN_HD_KEYS_HPP
#define LIBBITCOIN_HD_KEYS_HPP

#include <cstdint>
#include <bitcoin/bitcoin/compat.hpp>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/math/ec_keys.hpp>
#include <bitcoin/bitcoin/utility/data.hpp>
#include <bitcoin/bitcoin/wallet/address.hpp>

namespace libbitcoin {

BC_CONSTEXPR size_t chain_code_size = 32;
typedef byte_array<chain_code_size> chain_code_type;

BC_CONSTEXPR uint32_t first_hardened_key = 1 << 31;

/**
 * Key derivation information used in the serialization format.
 */
struct BC_API hd_key_lineage
{
    bool testnet;
    uint8_t depth;
    uint32_t parent_fingerprint;
    uint32_t child_number;
};

/**
 * An extended public key, as defined by BIP 32.
 */
class hd_public_key
{
public:
    BC_API hd_public_key();
    BC_API hd_public_key(const ec_point& public_key,
        const chain_code_type& chain_code, hd_key_lineage lineage);

    BC_API bool valid() const;

    BC_API const ec_point& public_key() const;
    BC_API const chain_code_type& chain_code() const;
    BC_API const hd_key_lineage& lineage() const;

    BC_API bool set_encoded(const std::string& encoded);
    BC_API std::string encoded() const;
    BC_API uint32_t fingerprint() const;
    BC_API payment_address address() const;

    BC_API hd_public_key generate_public_key(uint32_t i) const;

protected:
    bool valid_;
    ec_point K_; // EC point
    chain_code_type c_;
    hd_key_lineage lineage_;
};

/**
 * An extended private key, as defined by BIP 32.
 */
class hd_private_key
  : public hd_public_key
{
public:
    BC_API hd_private_key();
    BC_API hd_private_key(const ec_secret& private_key,
        const chain_code_type& chain_code, hd_key_lineage lineage);
    BC_API hd_private_key(data_slice seed, bool testnet=false);

    BC_API const ec_secret& private_key() const;

    BC_API bool set_encoded(const std::string& encoded);
    BC_API std::string encoded() const;

    BC_API hd_private_key generate_private_key(uint32_t i) const;
    BC_API hd_public_key generate_public_key(uint32_t i) const;

protected:
    ec_secret k_;
};

} // namespace libbitcoin

#endif

