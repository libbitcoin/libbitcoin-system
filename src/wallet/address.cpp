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
#include <bitcoin/bitcoin/wallet/address.hpp>

#include <algorithm>
#include <bitcoin/bitcoin/format.hpp>
#include <bitcoin/bitcoin/math/checksum.hpp>
#include <bitcoin/bitcoin/utility/assert.hpp>
#include <bitcoin/bitcoin/wallet/base58.hpp>

namespace libbitcoin {

payment_address::payment_address()
{
}
payment_address::payment_address(uint8_t version, const short_hash& hash)
  : payment_address()
{
    set(version, hash);
}
payment_address::payment_address(const std::string& encoded_address)
  : payment_address()
{
    set_encoded(encoded_address);
}

void payment_address::set(uint8_t version, const short_hash& hash)
{
    version_ = version;
    hash_ = hash;
}

uint8_t payment_address::version() const
{
    return version_;
}
const short_hash& payment_address::hash() const
{
    return hash_;
}

bool payment_address::set_encoded(const std::string& encoded_address)
{
    if (!is_base58(encoded_address))
        return false;
    const data_chunk decoded_address = decode_base58(encoded_address);
    // version + 20 bytes short hash + 4 bytes checksum
    if (decoded_address.size() != 25)
        return false;
    if (!verify_checksum(decoded_address))
        return false;

    version_ = decoded_address[0];
    std::copy_n(decoded_address.begin() + 1, hash_.size(), hash_.begin());
    return true;
}

std::string payment_address::encoded() const
{
    auto data = build_data({to_byte(version_), hash_}, checksum_size);
    append_checksum(data);
    BITCOIN_ASSERT(data.size() == 25);
    return encode_base58(data);
}

void set_public_key_hash(payment_address& address,
    const short_hash& pubkey_hash)
{
    address.set(payment_address::pubkey_version, pubkey_hash);
}

void set_script_hash(payment_address& address,
    const short_hash& script_hash)
{
    address.set(payment_address::script_version, script_hash);
}

void set_public_key(payment_address& address, const ec_point& public_key)
{
    address.set(payment_address::pubkey_version,
        bitcoin_short_hash(public_key));
}

void set_script(payment_address& address, const script_type& eval_script)
{
    address.set(payment_address::script_version,
        bitcoin_short_hash(save_script(eval_script)));
}

bool extract(payment_address& address, const script_type& script)
{
    // Cast a data_chunk to a short_hash and set the address
    auto set_hash_data =
        [&address](uint8_t version, const data_chunk& raw_hash)
        {
            short_hash hash_data;
            BITCOIN_ASSERT(raw_hash.size() == hash_data.size());
            std::copy(raw_hash.begin(), raw_hash.end(), hash_data.begin());
            address.set(version, hash_data);
        };
    const operation_stack& ops = script.operations();
    payment_type pay_type = script.type();
    switch (pay_type)
    {
        case payment_type::pubkey:
            BITCOIN_ASSERT(ops.size() == 2);
            set_public_key(address, ops[0].data);
            return true;

        case payment_type::pubkey_hash:
            BITCOIN_ASSERT(ops.size() == 5);
            set_hash_data(payment_address::pubkey_version, ops[2].data);
            return true;

        case payment_type::script_hash:
            BITCOIN_ASSERT(ops.size() == 3);
            set_hash_data(payment_address::script_version, ops[1].data);
            return true;

        case payment_type::multisig:
            // Unimplemented...
            return false;

        case payment_type::pubkey_hash_sig:
            BITCOIN_ASSERT(ops.size() == 2);
            set_public_key(address, ops[1].data);
            return true;

        case payment_type::script_code_sig:
            // Should have at least 1 sig and the script code.
            BITCOIN_ASSERT(ops.size() > 1);
            set_script_hash(address,
                bitcoin_short_hash(ops.back().data));
            return true;

        default:
            return false;
    }
    // Should never happen!
    return false;
}

bool operator==(const payment_address& lhs, const payment_address& rhs)
{
    return lhs.hash() == rhs.hash() && lhs.version() == rhs.version();
}

} // namespace libbitcoin

