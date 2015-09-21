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
#include <cstdint>
#include <string>
#include <bitcoin/bitcoin/formats/base58.hpp>
#include <bitcoin/bitcoin/math/checksum.hpp>
#include <bitcoin/bitcoin/utility/assert.hpp>
#include <bitcoin/bitcoin/utility/endian.hpp>

namespace libbitcoin {
namespace wallet {

payment_address::payment_address()
  : valid_(false), version_(0), hash_(null_short_hash)
{
}

payment_address::payment_address(const payment& decoded)
  : payment_address(from_payment(decoded))
{
}

payment_address::payment_address(const std::string& encoded)
  : payment_address(from_string(encoded))
{
}

payment_address::payment_address(const payment_address& other)
  : valid_(other.valid_), version_(other.version_), hash_(other.hash_)
{
}

payment_address::payment_address(const short_hash& hash, uint8_t version)
  : valid_(true), version_(version), hash_(hash)
{
}

payment_address::payment_address(const chain::script& script, uint8_t version)
  : valid_(true), version_(version),
    hash_(bitcoin_short_hash(script.to_data(false)))
{
}

payment_address::payment_address(const ec_public& point, uint8_t version,
    bool compressed)
  : payment_address(from_public(point, version, compressed))
{
}

payment_address::payment_address(const ec_secret& secret, uint8_t version,
    bool compressed)
  : payment_address(from_secret(secret, version, compressed))
{
}

payment_address payment_address::from_payment(const payment& decoded)
{
    uint8_t version;
    short_hash hash;
    const auto valid = unwrap(version, hash, decoded);
    return valid ? payment_address(hash, version) : payment_address();
}

payment_address payment_address::from_string(const std::string& encoded)
{
    payment decoded;
    const auto valid = decode_base58(decoded, encoded);
    return valid ? from_payment(decoded) : payment_address();
}

payment_address payment_address::from_public(const ec_public& point,
    uint8_t version, bool compressed)
{
    ////BITCOIN_ASSERT_MSG(!compressed || point.is_compressed(),
    ////    "cannot compress an uncompressed public key");

    if (compressed == point.is_compressed())
        return payment_address(bitcoin_short_hash(point.data()), version);

    ec_uncompressed uncompress;
    if (!compressed && point.is_compressed() && decompress(uncompress, point))
        return payment_address(bitcoin_short_hash(uncompress), version);

    return payment_address();
}

payment_address payment_address::from_secret(const ec_secret& secret,
    uint8_t version, bool compressed)
{
    ec_compressed point;
    if (secret_to_public(point, secret))
        return payment_address(point, version, compressed);

    return payment_address();
}

payment_address::operator const bool() const
{
    return valid_;
}

payment_address::operator const payment() const
{
    return wrap(version_, hash_);
}

std::string payment_address::encoded() const
{
    return encode_base58(wrap(version_, hash_));
}

uint8_t payment_address::version() const
{
    return version_;
}

const short_hash& payment_address::hash() const
{
    return hash_;
}

bool operator==(const payment_address& left, const payment_address& right)
{
    return left.hash() == right.hash() && left.version() == right.version();
}

payment_address extract_address(const chain::script& script)
{
    if (!script.is_valid())
        return payment_address();

    short_hash hash;
    const auto& ops = script.operations;

    // Split out the assertions for readability.
    // We know that the script is valid and can therefore rely on these.
    switch (script.type())
    {
        case chain::payment_type::pubkey_hash:
            BITCOIN_ASSERT(ops.size() == 5);
            BITCOIN_ASSERT(ops[2].data.size() == short_hash_size);
            break;
        case chain::payment_type::script_hash:
            BITCOIN_ASSERT(ops.size() == 3);
            BITCOIN_ASSERT(ops[1].data.size() == short_hash_size);
            break;
        case chain::payment_type::script_code_sig:
            BITCOIN_ASSERT(ops.size() > 1);
            break;
        case chain::payment_type::pubkey:
            BITCOIN_ASSERT(ops.size() == 2);
            BITCOIN_ASSERT(
                ops[0].data.size() == ec_compressed_size ||
                ops[0].data.size() == ec_uncompressed_size);
            break;
        case chain::payment_type::pubkey_hash_sig:
            BITCOIN_ASSERT(ops.size() == 2);
            BITCOIN_ASSERT(
                ops[1].data.size() == ec_compressed_size ||
                ops[1].data.size() == ec_uncompressed_size);
            break;
        default:;
    }

    // Convert data to hash or point and construct address.
    switch (script.type())
    {
        case chain::payment_type::pubkey_hash:
            hash = to_array<short_hash_size>(ops[2].data);
            return payment_address(hash, payment_address::mainnet);

        case chain::payment_type::script_hash:
            hash = to_array<short_hash_size>(ops[1].data);
            return payment_address(hash, payment_address::mainnet_p2sh);

        case chain::payment_type::script_code_sig:
            hash = bitcoin_short_hash(ops.back().data);
            return payment_address(hash, payment_address::mainnet_p2sh);

        case chain::payment_type::pubkey:
        {
            const auto& data = ops[0].data;
            if (data.size() == ec_compressed_size)
            {
                const auto point = to_array<ec_compressed_size>(data);
                return payment_address(point, payment_address::mainnet);
            }

            const auto point = to_array<ec_uncompressed_size>(data);
            return payment_address(point, payment_address::mainnet);
        }

        case chain::payment_type::pubkey_hash_sig:
        {
            const auto& data = ops[1].data;
            if (data.size() == ec_compressed_size)
            {
                const auto point = to_array<ec_compressed_size>(data);
                return payment_address(point, payment_address::mainnet);
            }

            const auto point = to_array<ec_uncompressed_size>(data);
            return payment_address(point, payment_address::mainnet);
        }

        default:
        case chain::payment_type::multisig:
            // multisig is unimplemented here...
            return payment_address();
    }
}

} // namespace wallet
} // namespace libbitcoin
