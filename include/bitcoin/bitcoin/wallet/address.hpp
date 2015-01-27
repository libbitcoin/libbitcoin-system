/*
 * Copyright (c) 2011-2013 libbitcoin developers (see AUTHORS)
 *
 * This file is part of libbitcoin.
 *
 * libbitcoin is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License with
 * additional permissions to the one published by the Free Software
 * Foundation, either version 3 of the License, or (at your option)
 * any later version. For more information see LICENSE.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef LIBBITCOIN_ADDRESS_HPP
#define LIBBITCOIN_ADDRESS_HPP

#include <bitcoin/bitcoin/constants.hpp>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/script.hpp>
#include <bitcoin/bitcoin/math/hash.hpp>

namespace libbitcoin {

/**
 * A class for handling Bitcoin addresses. Supports encoding and decoding
 * Bitcoin string addresses.
 *
 * To validate a Bitcoin address we can try to set a string address.
 *
 * @code
 *   payment_address payaddr;
 *   if (!payaddr.set_encoded("155GwFbFET2HCT6r6jHAHUoxc897sSdjaq"))
 *       // Address is invalid
 * @endcode
 *
 * To check whether a payment_address has successfully been set, the
 * hash value can be compared to null_short_hash (defined in constants.hpp).
 *
 * @code
 *   if (payaddr.version() == payment_address::invalid_version)
 *       // This payment_address is empty.
 * @endcode
 */
class payment_address
{
public:
#ifdef ENABLE_TESTNET
    enum
    {
        pubkey_version = 0x6f,
        script_version = 0xc4,
        wif_version = 0xef,
        invalid_version = 0xff
    };
#else
    enum
    {
        pubkey_version = 0x00,
        script_version = 0x05,
        wif_version = 0x80,
        invalid_version = 0xff
    };
#endif
    BC_API payment_address();
    BC_API payment_address(uint8_t version, const short_hash& hash);
    BC_API payment_address(const std::string& encoded_address);

    BC_API void set(uint8_t version, const short_hash& hash);

    BC_API uint8_t version() const;
    BC_API const short_hash& hash() const;

    BC_API bool set_encoded(const std::string& encoded_address);
    BC_API std::string encoded() const;

private:
    uint8_t version_ = invalid_version;
    short_hash hash_ = null_short_hash;
};

BC_API void set_public_key_hash(payment_address& address,
    const short_hash& pubkey_hash);
BC_API void set_script_hash(payment_address& address,
    const short_hash& script_hash);

BC_API void set_public_key(payment_address& address,
    const ec_point& public_key);
BC_API void set_script(payment_address& address,
    const script_type& eval_script);

/**
 * Extract a Bitcoin address from an input or output script.
 * Returns false on failure.
 */
BC_API bool extract(payment_address& address, const script_type& script);

BC_API bool operator==(const payment_address& lhs, const payment_address& rhs);

/**
 * Unwrap a wrapped payload.
 * @param[out] version   The version byte of the wrapped data.
 * @param[out] hash      The short_hash payload of the wrapped data.
 * @param[out] checksum  The validated checksum of the wrapped data.
 * @param[in]  wrapped   The wrapped data to unwrap.
 * @return               True if input checksum validates.
 */
BC_API bool unwrap(uint8_t& version, short_hash& hash, uint32_t& checksum,
    data_slice wrapped);

/**
 * Unwrap a wrapped payload.
 * @param[out] version   The version byte of the wrapped data.
 * @param[out] payload   The payload of the wrapped data.
 * @param[out] checksum  The validated checksum of the wrapped data.
 * @param[in]  wrapped   The wrapped data to unwrap.
 * @return               True if input checksum validates.
 */
BC_API bool unwrap(uint8_t& version, data_chunk& payload, uint32_t& checksum,
    data_slice wrapped);

/**
 * Wrap arbitrary data.
 * @param[in]  version  The version byte for the wrapped data.
 * @param[out] payload  The payload to wrap.
 * @return              The wrapped data.
 */
BC_API data_chunk wrap(uint8_t version, data_slice payload);

} // namespace libbitcoin

// Allow payment_address to be in indexed in std::*map classes.
namespace std
{
    template <>
    struct BC_API hash<libbitcoin::payment_address>
    {
        size_t operator()(const libbitcoin::payment_address& payaddr) const
        {
            using libbitcoin::short_hash;
            using libbitcoin::short_hash_size;
            std::string raw_addr;
            raw_addr.resize(short_hash_size + 1);
            raw_addr[0] = payaddr.version();
            const short_hash& addr_hash = payaddr.hash();
            std::copy(addr_hash.begin(), addr_hash.end(),
                raw_addr.begin() + 1);
            std::hash<std::string> functor;
            return functor(raw_addr);
        }
    };

} // namespace std

#endif
