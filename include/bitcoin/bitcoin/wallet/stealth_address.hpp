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

#include <bitcoin/bitcoin/wallet/stealth_address.hpp>

#include <cstdint>
#include <vector>
#include <bitcoin/bitcoin/constants.hpp>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/script.hpp>
#include <bitcoin/bitcoin/stealth.hpp>
#include <bitcoin/bitcoin/utility/ec_keys.hpp>

namespace libbitcoin {

typedef std::vector<ec_point> pubkey_list;

struct BC_API stealth_info
{
    ec_point ephem_pubkey;
    stealth_bitfield bitfield;
};

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
    BC_API stealth_address(const stealth_prefix& prefix,
        const ec_point& scan_pubkey, const pubkey_list& spend_pubkeys,
        uint8_t signatures, bool testnet);

    // Serialization
    BC_API std::string encoded() const;
    BC_API bool set_encoded(const std::string& encoded_address);
    BC_API bool valid() const;

    // Properties
    BC_API const stealth_prefix& get_prefix() const;
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
    stealth_prefix prefix_;
};

// See libbitcoin::extract()
BC_API bool extract_stealth_info(stealth_info& info,
    const script_type& output_script);
BC_API ec_point initiate_stealth(
    const ec_secret& ephem_secret, const ec_point& scan_pubkey,
    const ec_point& spend_pubkey);
BC_API ec_secret shared_secret(const ec_secret& secret,
    const ec_point& point);
BC_API ec_point uncover_stealth(
    const ec_point& ephem_pubkey, const ec_secret& scan_secret,
    const ec_point& spend_pubkey);
BC_API ec_secret uncover_stealth_secret(
    const ec_point& ephem_pubkey, const ec_secret& scan_secret,
    const ec_secret& spend_secret);

BC_API data_chunk prefix_to_bytes(const stealth_prefix& prefix);

} // namespace libbitcoin

#endif
