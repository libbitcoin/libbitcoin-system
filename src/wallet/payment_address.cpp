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
#include <bitcoin/bitcoin/wallet/payment_address.hpp>

#include <algorithm>
#include <cstdint>
#include <iostream>
#include <string>
#include <boost/program_options.hpp>
#include <bitcoin/bitcoin/formats/base_58.hpp>
#include <bitcoin/bitcoin/math/checksum.hpp>
#include <bitcoin/bitcoin/math/hash.hpp>
#include <bitcoin/bitcoin/wallet/ec_private.hpp>
#include <bitcoin/bitcoin/wallet/ec_public.hpp>

namespace libbitcoin {
namespace wallet {

using namespace bc::machine;

const uint8_t payment_address::mainnet_p2kh = 0x00;
const uint8_t payment_address::mainnet_p2sh = 0x05;

const uint8_t payment_address::testnet_p2kh = 0x6f;
const uint8_t payment_address::testnet_p2sh = 0xc4;

payment_address::payment_address()
  : valid_(false), version_(0), hash_(null_short_hash)
{
}

payment_address::payment_address(const payment_address& other)
  : valid_(other.valid_), version_(other.version_), hash_(other.hash_)
{
}

payment_address::payment_address(const payment& decoded)
  : payment_address(from_payment(decoded))
{
}

payment_address::payment_address(const std::string& address)
  : payment_address(from_string(address))
{
}

// MSVC (CTP) casts this down to 8 bits if a variable is not used:
// const payment_address address({ ec_secret, 0x806F });
// Alternatively explicit construction of the ec_private also works.
// const payment_address address(ec_private(ec_secret, 0x806F));
// const payment_address address(ec_private{ ec_secret, 0x806F });
// However this doesn't impact payment_address, since it only uses the LSB.
payment_address::payment_address(const ec_private& secret)
  : payment_address(from_private(secret))
{
}

payment_address::payment_address(const ec_public& point, uint8_t version)
  : payment_address(from_public(point, version))
{
}

payment_address::payment_address(const chain::script& script, uint8_t version)
  : payment_address(from_script(script, version))
{
}

payment_address::payment_address(const short_hash& hash, uint8_t version)
  : valid_(true), version_(version), hash_(hash)
{
}

// Validators.
// ----------------------------------------------------------------------------

bool payment_address::is_address(data_slice decoded)
{
    return (decoded.size() == payment_size) && verify_checksum(decoded);
}

// Factories.
// ----------------------------------------------------------------------------

payment_address payment_address::from_string(const std::string& address)
{
    payment decoded;
    if (!decode_base58(decoded, address) || !is_address(decoded))
        return payment_address();

    return payment_address(decoded);
}

payment_address payment_address::from_payment(const payment& decoded)
{
    if (!is_address(decoded))
        return payment_address();

    const auto hash = slice<1, short_hash_size + 1>(decoded);
    return payment_address(hash, decoded.front());
}

payment_address payment_address::from_private(const ec_private& secret)
{
    if (!secret)
        return payment_address();

    return payment_address(secret.to_public(), secret.payment_version());
}

payment_address payment_address::from_public(const ec_public& point,
    uint8_t version)
{
    if (!point)
        return payment_address();

    data_chunk data;
    return point.to_data(data) ?
        payment_address(bitcoin_short_hash(data), version) :
        payment_address();
}

payment_address payment_address::from_script(const chain::script& script,
    uint8_t version)
{
    return payment_address(bitcoin_short_hash(script.to_data(false)), version);
}

// Cast operators.
// ----------------------------------------------------------------------------

payment_address::operator const bool() const
{
    return valid_;
}

payment_address::operator const short_hash&() const
{
    return hash_;
}

// Serializer.
// ----------------------------------------------------------------------------

std::string payment_address::encoded() const
{
    return encode_base58(wrap(version_, hash_));
}

// Accessors.
// ----------------------------------------------------------------------------

uint8_t payment_address::version() const
{
    return version_;
}

const short_hash& payment_address::hash() const
{
    return hash_;
}

// Methods.
// ----------------------------------------------------------------------------

payment payment_address::to_payment() const
{
    return wrap(version_, hash_);
}

// Operators.
// ----------------------------------------------------------------------------

payment_address& payment_address::operator=(const payment_address& other)
{
    valid_ = other.valid_;
    version_ = other.version_;
    hash_ = other.hash_;
    return *this;
}

bool payment_address::operator<(const payment_address& other) const
{
    return encoded() < other.encoded();
}

bool payment_address::operator==(const payment_address& other) const
{
    return valid_ == other.valid_ && version_ == other.version_ &&
        hash_ == other.hash_;
}

bool payment_address::operator!=(const payment_address& other) const
{
    return !(*this == other);
}

std::istream& operator>>(std::istream& in, payment_address& to)
{
    std::string value;
    in >> value;
    to = payment_address(value);

    if (!to)
    {
        using namespace boost::program_options;
        BOOST_THROW_EXCEPTION(invalid_option_value(value));
    }

    return in;
}

std::ostream& operator<<(std::ostream& out, const payment_address& of)
{
    out << of.encoded();
    return out;
}

// Static functions.
// ----------------------------------------------------------------------------

payment_address payment_address::extract(const chain::script& script,
    uint8_t p2kh_version, uint8_t p2sh_version)
{
    if (!script.is_valid())
        return{};

    short_hash hash;
    const auto pattern = script.pattern();

    // Split out the assertions for readability.
    // We know that the script is valid and can therefore rely on these.
    switch (pattern)
    {
        // pay
        // --------------------------------------------------------------------
        case script_pattern::pay_multisig:
            break;
        case script_pattern::pay_public_key:
            BITCOIN_ASSERT(script.size() == 2);
            BITCOIN_ASSERT(
                script[0].data().size() == ec_compressed_size ||
                script[0].data().size() == ec_uncompressed_size);
            break;
        case script_pattern::pay_key_hash:
            BITCOIN_ASSERT(script.size() == 5);
            BITCOIN_ASSERT(script[2].data().size() == short_hash_size);
            break;
        case script_pattern::pay_script_hash:
            BITCOIN_ASSERT(script.size() == 3);
            BITCOIN_ASSERT(script[1].data().size() == short_hash_size);
            break;

        // sign
        // --------------------------------------------------------------------
        case script_pattern::sign_multisig:
            break;
        case script_pattern::sign_public_key:
            break;
        case script_pattern::sign_key_hash:
            BITCOIN_ASSERT(script.size() == 2);
            BITCOIN_ASSERT(
                script[1].data().size() == ec_compressed_size ||
                script[1].data().size() == ec_uncompressed_size);
            break;
        case script_pattern::sign_script_hash:
            BITCOIN_ASSERT(script.size() > 1);
            break;
        case script_pattern::non_standard:
        default:;
    }

    // Convert data to hash or point and construct address.
    switch (pattern)
    {
        // pay
        // --------------------------------------------------------------------

        // TODO: extract addresses into a vector result.
        // Bare multisig is not tracked in association with any address.
        case script_pattern::pay_multisig:
            return{};

        case script_pattern::pay_public_key:
        {
            const auto& data = script[0].data();
            if (data.size() == ec_compressed_size)
            {
                const auto point = to_array<ec_compressed_size>(data);
                return payment_address(point, p2kh_version);
            }

            const auto point = to_array<ec_uncompressed_size>(data);
            return payment_address(point, p2kh_version);
        }

        case script_pattern::pay_key_hash:
            hash = to_array<short_hash_size>(script[2].data());
            return payment_address(hash, p2kh_version);

        case script_pattern::pay_script_hash:
            hash = to_array<short_hash_size>(script[1].data());
            return payment_address(hash, p2sh_version);

        // sign
        // --------------------------------------------------------------------

        // There are no addresses in sign_multisig (can get from prevout).
        // Bare multisig is not tracked in association with any address.
        case script_pattern::sign_multisig:
            return{};

        // There is no address in a sign_public_key (can get from prevout).
        // TODO: server should extract from prevout and track (legacy).
        case script_pattern::sign_public_key:
            return{};

        case script_pattern::sign_key_hash:
        {
            const auto& data = script[1].data();
            if (data.size() == ec_compressed_size)
            {
                const auto point = to_array<ec_compressed_size>(data);
                return payment_address(point, p2kh_version);
            }

            const auto point = to_array<ec_uncompressed_size>(data);
            return payment_address(point, p2kh_version);
        }

        // All p2sh scripts are tracked using the script hash only.
        case script_pattern::sign_script_hash:
            hash = bitcoin_short_hash(script.back().data());
            return payment_address(hash, p2sh_version);

        case script_pattern::non_standard:
        default:
            return{};
    }
}

} // namespace wallet
} // namespace libbitcoin
