/**
 * Copyright (c) 2011-2015 libbitcoin developers (see AUTHORS)
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
#include <bitcoin/bitcoin/wallet/payment_address.hpp>

#include <algorithm>
#include <bitcoin/bitcoin/formats/base58.hpp>
#include <bitcoin/bitcoin/math/checksum.hpp>
#include <bitcoin/bitcoin/utility/assert.hpp>
#include <bitcoin/bitcoin/utility/endian.hpp>
#include <bitcoin/bitcoin/utility/deserializer.hpp>
#include <bitcoin/bitcoin/utility/serializer.hpp>

namespace libbitcoin {
namespace wallet {

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
    if (!from_string(encoded_address))
        throw std::invalid_argument("encoded_address");
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

bool payment_address::from_string(const std::string& encoded_address)
{
    data_chunk decoded_address;

    if (!decode_base58(decoded_address, encoded_address))
        return false;

    uint32_t checksum;

    return unwrap(version_, hash_, checksum, decoded_address);
}

std::string payment_address::to_string() const
{
    const auto data = wrap(version_, hash_);
    return encode_base58(data);
}

void payment_address::set_public_key_hash(const short_hash& pubkey_hash)
{
    set(payment_address::pubkey_version, pubkey_hash);
}

void payment_address::set_script_hash(const short_hash& script_hash)
{
    set(payment_address::script_version, script_hash);
}

void payment_address::set_public_key(const ec_point& public_key)
{
    set(payment_address::pubkey_version, bitcoin_short_hash(public_key));
}

void payment_address::set_script(const chain::script& eval_script)
{
    set(payment_address::script_version, bitcoin_short_hash(eval_script.to_data(false)));
}

bool extract(payment_address& address, const chain::script& script)
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
    const chain::operation_stack& ops = script.operations;
    chain::payment_type pay_type = script.type();

    switch (pay_type)
    {
        case chain::payment_type::pubkey:
            BITCOIN_ASSERT(ops.size() == 2);
            address.set_public_key(ops[0].data);
            return true;

        case chain::payment_type::pubkey_hash:
            BITCOIN_ASSERT(ops.size() == 5);
            set_hash_data(payment_address::pubkey_version, ops[2].data);
            return true;

        case chain::payment_type::script_hash:
            BITCOIN_ASSERT(ops.size() == 3);
            set_hash_data(payment_address::script_version, ops[1].data);
            return true;

        case chain::payment_type::multisig:
            // Unimplemented...
            return false;

        case chain::payment_type::pubkey_hash_sig:
            BITCOIN_ASSERT(ops.size() == 2);
            address.set_public_key(ops[1].data);
            return true;

        case chain::payment_type::script_code_sig:
            // Should have at least 1 sig and the script code.
            BITCOIN_ASSERT(ops.size() > 1);
            address.set_script_hash(bitcoin_short_hash(ops.back().data));
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

bool unwrap(uint8_t& version, short_hash& hash, uint32_t& checksum,
    data_slice wrapped)
{
    data_chunk payload;
    auto result = unwrap(version, payload, checksum, wrapped) && 
        (payload.size() == hash.size());
    if (result)
        std::copy_n(payload.begin(), hash.size(), hash.begin());
    return result;
}

bool unwrap(uint8_t& version, data_chunk& payload, uint32_t& checksum,
    data_slice wrapped)
{
    constexpr size_t version_length = sizeof(version);
    constexpr size_t checksum_length = sizeof(checksum);
    // guard against insufficient buffer length
    if (wrapped.size() < version_length + checksum_length)
        return false;
    if (!verify_checksum(wrapped))
        return false;
    // set return values
    version = wrapped.data()[0];
    payload = data_chunk(wrapped.begin() + version_length,
        wrapped.end() - checksum_length);
    const auto checksum_start = wrapped.end() - checksum_length;
    auto deserial = make_deserializer(checksum_start, wrapped.end());
    checksum = deserial.read_4_bytes();
    return true;
}

data_chunk wrap(uint8_t version, data_slice payload)
{
    data_chunk wrapped;
    wrapped.push_back(version);
    extend_data(wrapped, payload);
    append_checksum(wrapped);
    return wrapped;
}

} // namespace wallet
} // namespace libbitcoin
