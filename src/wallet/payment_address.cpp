/**
 * Copyright (c) 2011-2019 libbitcoin developers (see AUTHORS)
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
#include <bitcoin/system/wallet/payment_address.hpp>

#include <algorithm>
#include <cstdint>
#include <iostream>
#include <string>
#include <utility>
#include <boost/program_options.hpp>
#include <bitcoin/system/formats/base_58.hpp>
#include <bitcoin/system/math/checksum.hpp>
#include <bitcoin/system/math/elliptic_curve.hpp>
#include <bitcoin/system/math/hash.hpp>
#include <bitcoin/system/wallet/ec_private.hpp>
#include <bitcoin/system/wallet/ec_public.hpp>

namespace libbitcoin {
namespace system {
namespace wallet {

using namespace bc::system::machine;

const uint8_t payment_address::mainnet_p2kh = 0x00;
const uint8_t payment_address::mainnet_p2sh = 0x05;

const uint8_t payment_address::testnet_p2kh = 0x6f;
const uint8_t payment_address::testnet_p2sh = 0xc4;

payment_address::payment_address()
  : valid_(false), version_(0), hash_(null_short_hash)
{
}

payment_address::payment_address(payment_address&& other)
  : valid_(other.valid_), version_(other.version_),
    hash_(std::move(other.hash_))
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

payment_address::payment_address(short_hash&& hash, uint8_t version)
  : valid_(true), version_(version), hash_(std::move(hash))
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
        return {};

    return { decoded };
}

payment_address payment_address::from_payment(const payment& decoded)
{
    if (!is_address(decoded))
        return {};

    const auto hash = slice<1, short_hash_size + 1>(decoded);
    return { hash, decoded.front() };
}

payment_address payment_address::from_private(const ec_private& secret)
{
    if (!secret)
        return {};

    return { secret.to_public(), secret.payment_version() };
}

payment_address payment_address::from_public(const ec_public& point,
    uint8_t version)
{
    if (!point)
        return {};

    data_chunk data;
    if (!point.to_data(data))
        return {};

    return { bitcoin_short_hash(data), version };
}

payment_address payment_address::from_script(const chain::script& script,
    uint8_t version)
{
    // Working around VC++ CTP compiler break here.
    const auto data = script.to_data(false);
    return { bitcoin_short_hash(data), version };
}

// Cast operators.
// ----------------------------------------------------------------------------

payment_address::operator bool() const
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

chain::script payment_address::output_script() const
{
    switch (version_)
    {
        case 0:
        case 111:
            return chain::script::to_pay_key_hash_pattern(hash_);

        case 5:
        case 196:
            return chain::script::to_pay_script_hash_pattern(hash_);
    }

    return {};
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

// Context free input extraction is provably ambiguous (see extract_input).
payment_address::list payment_address::extract(const chain::script& script,
    uint8_t p2kh_version, uint8_t p2sh_version)
{
    const auto input = extract_input(script, p2kh_version, p2sh_version);
    return input.empty() ? extract_output(script, p2kh_version, p2sh_version) :
        input;
}

// Context free input extraction is provably ambiguous. See inline comments.
payment_address::list payment_address::extract_input(
    const chain::script& script, uint8_t p2kh_version, uint8_t p2sh_version)
{
    // A sign_key_hash result always implies sign_script_hash as well.
    const auto pattern = script.input_pattern();

    switch (pattern)
    {
        // Given lack of context (prevout) sign_key_hash is always ambiguous
        // with sign_script_hash, so return both potentially-correct addresses.
        // A server can differentiate by extracting from the previous output.
        case script_pattern::sign_key_hash:
        {
            return
            {
                { ec_public{ script[1].data() }, p2kh_version }////,
                ////{ bitcoin_short_hash(script.back().data()), p2sh_version }
            };
        }
        case script_pattern::sign_script_hash:
        {
            return
            {
                { bitcoin_short_hash(script.back().data()), p2sh_version }
            };
        }

        // There is no address in sign_public_key script (signature only)
        // and the public key cannot be extracted from the signature.
        // Given lack of context (prevout) sign_public_key is always ambiguous
        // with sign_script_hash (though actual conflict seems very unlikely).
        // A server can obtain by extracting from the previous output.
        case script_pattern::sign_public_key:

        // There are no addresses in sign_multisig script, signatures only.
        // Nonstandard (non-zero) first op sign_multisig may conflict with
        // sign_key_hash and/or sign_script_hash (or will be non_standard).
        // A server can obtain the public keys extracting from the previous
        // output, but bare multisig does not associate a payment address.
        case script_pattern::sign_multisig:
        case script_pattern::non_standard:
        default:
        {
            return {};
        }
    }
}

// A server should use this against the prevout instead of using extract_input.
payment_address::list payment_address::extract_output(
    const chain::script& script, uint8_t p2kh_version, uint8_t p2sh_version)
{
    const auto pattern = script.output_pattern();

    switch (pattern)
    {
        case script_pattern::pay_key_hash:
        {
            return
            {
                { to_array<short_hash_size>(script[2].data()), p2kh_version }
            };
        }
        case script_pattern::pay_script_hash:
        {
            return
            {
                { to_array<short_hash_size>(script[1].data()), p2sh_version }
            };
        }
        case script_pattern::pay_public_key:
        {
            return
            {
                // pay_public_key is not p2kh but we conflate for tracking.
                { ec_public{ script[0].data() }, p2kh_version }
            };
        }

        // Bare multisig and null data do not associate a payment address.
        case script_pattern::pay_multisig:
        case script_pattern::pay_null_data:
        case script_pattern::non_standard:
        default:
        {
            return {};
        }
    }
}

} // namespace wallet
} // namespace system
} // namespace libbitcoin
