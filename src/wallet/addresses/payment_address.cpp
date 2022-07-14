/**
 * Copyright (c) 2011-2022 libbitcoin developers (see AUTHORS)
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
#include <bitcoin/system/wallet/addresses/payment_address.hpp>

#include <algorithm>
/// DELETECSTDINT
#include <iostream>
#include <string>
#include <tuple>
#include <utility>
#include <bitcoin/system/chain/chain.hpp>
#include <bitcoin/system/crypto/crypto.hpp>
/// DELETEMENOW
#include <bitcoin/system/radix/radix.hpp>
#include <bitcoin/system/wallet/keys/ec_private.hpp>
#include <bitcoin/system/wallet/keys/ec_public.hpp>

namespace libbitcoin {
namespace system {
namespace wallet {

const uint8_t payment_address::mainnet_p2kh = 0x00;
const uint8_t payment_address::mainnet_p2sh = 0x05;

const uint8_t payment_address::testnet_p2kh = 0x6f;
const uint8_t payment_address::testnet_p2sh = 0xc4;

payment_address::payment_address() NOEXCEPT
  : payment_()
{
}

payment_address::payment_address(payment_address&& other) NOEXCEPT
  : payment_(std::move(other.payment_))
{
}

payment_address::payment_address(const payment_address& other) NOEXCEPT
  : payment_(other.payment_)
{
}

payment_address::payment_address(payment&& decoded) NOEXCEPT
  : payment_(std::move(decoded))
{
}

payment_address::payment_address(const payment& decoded) NOEXCEPT
  : payment_(decoded)
{
}

payment_address::payment_address(const std::string& address) NOEXCEPT
  : payment_address(from_string(address))
{
}

payment_address::payment_address(const ec_private& secret) NOEXCEPT
  : payment_address(from_private(secret))
{
}

payment_address::payment_address(const ec_public& point,
    uint8_t prefix) NOEXCEPT
  : payment_address(from_public(point, prefix))
{
}

payment_address::payment_address(const chain::script& script,
    uint8_t prefix) NOEXCEPT
  : payment_address(from_script(script, prefix))
{
}

payment_address::payment_address(const short_hash& hash,
    uint8_t prefix) NOEXCEPT
  : payment_(to_array(prefix), hash)
{
}

// Factories.
// ----------------------------------------------------------------------------

payment_address payment_address::from_string(
    const std::string& address) NOEXCEPT
{
    data_array<payment::value_size> decoded;
    if (!decode_base58(decoded, address) || 
        decoded.size() != payment::value_size)
        return {};

    payment value(std::move(decoded));

    // Validates checksum.
    if (!value)
        return {};

    return { std::move(value) };
}

payment_address payment_address::from_private(const ec_private& secret) NOEXCEPT
{
    if (!secret)
        return {};

    return { secret.to_public(), secret.payment_version() };
}

payment_address payment_address::from_public(const ec_public& point,
    uint8_t prefix) NOEXCEPT
{
    if (!point)
        return {};

    data_chunk data;
    if (!point.to_data(data))
        return {};

    return { bitcoin_short_hash(data), prefix };
}

payment_address payment_address::from_script(const chain::script& script,
    uint8_t prefix) NOEXCEPT
{
    if (!script.is_valid())
        return {};

    return { bitcoin_short_hash(script.to_data(false)), prefix };
}

// Cast operators.
// ----------------------------------------------------------------------------

payment_address::operator bool() const NOEXCEPT
{
    return payment_;
}

// Serializer.
// ----------------------------------------------------------------------------

std::string payment_address::encoded() const NOEXCEPT
{
    return encode_base58(payment_);
}

// Properties.
// ----------------------------------------------------------------------------

uint8_t payment_address::prefix() const NOEXCEPT
{
    return payment_.prefix().front();
}

short_hash payment_address::hash() const NOEXCEPT
{
    return payment_.payload();
}

chain::script payment_address::output_script() const NOEXCEPT
{
    switch (prefix())
    {
        case mainnet_p2kh:
        case testnet_p2kh:
            return chain::script::to_pay_key_hash_pattern(hash());

        case mainnet_p2sh:
        case testnet_p2sh:
            return chain::script::to_pay_script_hash_pattern(hash());
    }

    return {};
}

// Methods.
// ----------------------------------------------------------------------------

const payment& payment_address::to_payment() const NOEXCEPT
{
    return payment_;
}

// Operators.
// ----------------------------------------------------------------------------

payment_address& payment_address::operator=(payment_address&& other) NOEXCEPT
{
    payment_ = std::move(other.payment_);
    return *this;
}

payment_address& payment_address::operator=(
    const payment_address& other) NOEXCEPT
{
    payment_ = other.payment_;
    return *this;
}

bool payment_address::operator<(const payment_address& other) const NOEXCEPT
{
    return encoded() < other.encoded();
}

bool operator==(const payment_address& left,
    const payment_address& right) NOEXCEPT
{
    return left.to_payment() == right.to_payment();
}

bool operator!=(const payment_address& left,
    const payment_address& right) NOEXCEPT
{
    return !(left == right);
}

std::istream& operator>>(std::istream& in, payment_address& to)
{
    std::string value;
    in >> value;
    to = payment_address(value);

    if (!to)
        throw istream_exception(value);

    return in;
}

std::ostream& operator<<(std::ostream& out, const payment_address& of) NOEXCEPT
{
    out << of.encoded();
    return out;
}

// Static functions.
// ----------------------------------------------------------------------------

// Context free input extraction is provably ambiguous (see extract_input).
payment_address::list payment_address::extract(const chain::script& script,
    uint8_t p2kh_prefix, uint8_t p2sh_prefix) NOEXCEPT
{
    const auto input = extract_input(script, p2kh_prefix, p2sh_prefix);

    if (!input.empty())
        return input;

    return { extract_output(script, p2kh_prefix, p2sh_prefix) };
}

// Context free input extraction is provably ambiguous. See inline comments.
payment_address::list payment_address::extract_input(
    const chain::script& script, uint8_t p2kh_prefix,
    uint8_t p2sh_prefix) NOEXCEPT
{
    // A sign_key_hash result always implies sign_script_hash as well.
    const auto pattern = script.input_pattern();

    switch (pattern)
    {
        // Given lack of context (prevout) sign_key_hash is always ambiguous
        // with sign_script_hash, so return both potentially-correct addresses.
        // A server can differentiate by extracting from the previous output.
        case chain::script_pattern::sign_key_hash:
        {
            return
            {
                { ec_public{ script.ops().front().data() }, p2kh_prefix }
                ////,{ bitcoin_short_hash(script.back().data()), p2sh_prefix }
            };
        }
        case chain::script_pattern::sign_script_hash:
        {
            return
            {
                { bitcoin_short_hash(script.ops().back().data()), p2sh_prefix }
            };
        }

        // There is no address in sign_public_key script (signature only)
        // and the public key cannot be extracted from the signature.
        // Given lack of context (prevout) sign_public_key is always ambiguous
        // with sign_script_hash (though actual conflict seems very unlikely).
        // A server can obtain by extracting from the previous output.
        case chain::script_pattern::sign_public_key:

        // There are no addresses in sign_multisig script, signatures only.
        // Nonstandard (non-zero) first op sign_multisig may conflict with
        // sign_key_hash and/or sign_script_hash (or will be non_standard).
        // A server can obtain the public keys extracting from the previous
        // output, but bare multisig does not associate a payment address.
        case chain::script_pattern::sign_multisig:
        case chain::script_pattern::non_standard:
        default:
        {
            return {};
        }
    }
}

// A server should use this against the prevout instead of using extract_input.
payment_address payment_address::extract_output(
    const chain::script& script, uint8_t p2kh_prefix,
    uint8_t p2sh_prefix) NOEXCEPT
{
    const auto pattern = script.output_pattern();

    switch (pattern)
    {
        case chain::script_pattern::pay_key_hash:
            return
            {
                to_array<short_hash_size>(script.ops()[2].data()),
                p2kh_prefix
            };
        case chain::script_pattern::pay_script_hash:
            return
            { 
                to_array<short_hash_size>(script.ops()[1].data()),
                p2sh_prefix
            };
        case chain::script_pattern::pay_public_key:
            return
            {
                // pay_public_key is not p2kh but we conflate for tracking.
                ec_public{ script.ops().front().data() },
                p2kh_prefix
            };

        // Bare multisig and null data do not associate a payment address.
        case chain::script_pattern::pay_multisig:
        case chain::script_pattern::pay_null_data:
        case chain::script_pattern::non_standard:
        default:
            return {};
    }
}

} // namespace wallet
} // namespace system
} // namespace libbitcoin
