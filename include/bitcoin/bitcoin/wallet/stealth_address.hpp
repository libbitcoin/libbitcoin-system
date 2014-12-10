/**
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
#ifndef LIBBITCOIN_STEALTH_ADDRESS_HPP
#define LIBBITCOIN_STEALTH_ADDRESS_HPP

#include <cstdint>
#include <vector>
#include <bitcoin/bitcoin/constants.hpp>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/script.hpp>
#include <bitcoin/bitcoin/stealth.hpp>
#include <bitcoin/bitcoin/math/ec_keys.hpp>

namespace libbitcoin {

typedef std::vector<ec_point> pubkey_list;

// Supports testnet and mainnet addresses but not prefix > 0
class stealth_address
{
public:
    static const uint8_t max_prefix_bits = sizeof(uint32_t) * byte_bits;

    enum flags : uint8_t
    {
        none = 0x00,
        reuse_key = 0x01
    };

    // TODO: move this to higher level with dynamic testnet refactor.
    enum network : uint8_t
    {
        mainnet = 0x2a,
        testnet = 0x2b
    };

    // Construction
    BC_API stealth_address();
    BC_API stealth_address(const binary_type& prefix,
        const ec_point& scan_pubkey, const pubkey_list& spend_pubkeys,
        uint8_t signatures, bool testnet);

    // Serialization
    BC_API std::string encoded() const;
    BC_API bool set_encoded(const std::string& encoded_address);
    BC_API bool valid() const;

    // Properties
    BC_API const binary_type& get_prefix() const;
    BC_API const ec_point& get_scan_pubkey() const;
    BC_API uint8_t get_signatures() const;
    BC_API const pubkey_list& get_spend_pubkeys() const;
    BC_API bool get_testnet() const;

protected:
    bool get_reuse_key() const;
    uint8_t get_options() const;
    uint8_t get_version() const;

    bool valid_ = false;
    bool testnet_ = false;
    uint8_t signatures_ = 0;
    ec_point scan_pubkey_;
    pubkey_list spend_pubkeys_;
    binary_type prefix_;
};

} // namespace libbitcoin

#endif
