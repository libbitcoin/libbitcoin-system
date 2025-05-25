/**
 * Copyright (c) 2011-2025 libbitcoin developers (see AUTHORS)
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
#include <bitcoin/system/wallet/keys/hd_public.hpp>

#include <bitcoin/system/crypto/crypto.hpp>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/endian/endian.hpp>
#include <bitcoin/system/hash/hash.hpp>
#include <bitcoin/system/math/math.hpp>
#include <bitcoin/system/radix/radix.hpp>
#include <bitcoin/system/stream/stream.hpp>
#include <bitcoin/system/wallet/keys/ec_public.hpp>
#include <bitcoin/system/wallet/keys/hd_private.hpp>

namespace libbitcoin {
namespace system {
namespace wallet {

// Assumptions inherent in the implementation.
static_assert(ec_secret_size == hd_chain_code_size,
    "secret_size != chain_code_size");
static_assert(long_hash_size == ec_secret_size + hd_chain_code_size,
    "secret_size + chain_code_size != long_hash_size");

const uint32_t hd_public::mainnet = 0x0488b21e;
const uint32_t hd_public::testnet = 0x043587cf;

// hd_public
// ----------------------------------------------------------------------------

hd_public::hd_public() NOEXCEPT
  : valid_(false), chain_(null_hash), lineage_({0, 0, 0, 0}),
    point_(null_ec_compressed)
{
}

// This cannot validate the version.
hd_public::hd_public(const hd_key& public_key) NOEXCEPT
  : hd_public(from_key(public_key))
{
}

// This cannot validate the version.
hd_public::hd_public(const std::string& encoded) NOEXCEPT
  : hd_public(from_string(encoded))
{
}

// This validates the version.
hd_public::hd_public(const hd_key& public_key, uint32_t prefix) NOEXCEPT
  : hd_public(from_key(public_key, prefix))
{
}

// This validates the version.
hd_public::hd_public(const std::string& encoded, uint32_t prefix) NOEXCEPT
  : hd_public(from_string(encoded, prefix))
{
}

hd_public::hd_public(const ec_compressed& point,
    const hd_chain_code& chain_code, const hd_lineage& lineage) NOEXCEPT
  : valid_(true), point_(point), chain_(chain_code), lineage_(lineage)
{
}

// Factories.
// ----------------------------------------------------------------------------

hd_public hd_public::from_secret(const ec_secret& secret,
    const hd_chain_code& chain_code, const hd_lineage& lineage) NOEXCEPT
{
    ec_compressed point;
    return secret_to_public(point, secret) ?
        hd_public(point, chain_code, lineage) : hd_public{};
}

hd_public hd_public::from_key(const hd_key& key) NOEXCEPT
{
    return from_key(key, from_big_endian<uint32_t>(key));
}

hd_public hd_public::from_string(const std::string& encoded) NOEXCEPT
{
    hd_key key;
    if (!decode_base58(key, encoded))
        return {};

    return hd_public(from_key(key));
}

hd_public hd_public::from_key(const hd_key& key, uint32_t prefix) NOEXCEPT
{
    read::bytes::copy source(key);

    const auto actual_prefix = source.read_4_bytes_big_endian();
    const auto depth = source.read_byte();
    const auto parent = source.read_4_bytes_big_endian();
    const auto child = source.read_4_bytes_big_endian();
    const auto chain = source.read_forward<hd_chain_code_size>();
    const auto compressed = source.read_forward<ec_compressed_size>();

    // Validate the prefix against the provided value.
    if (actual_prefix != prefix)
        return {};

    // The private prefix will be zero'd here, but there's no way to access it.
    const hd_lineage lineage
    {
        prefix,
        depth,
        parent,
        child
    };

    return { compressed, chain, lineage };
}

hd_public hd_public::from_string(const std::string& encoded,
    uint32_t prefix) NOEXCEPT
{
    hd_key key;
    if (!decode_base58(key, encoded))
        return {};

    return { from_key(key, prefix) };
}

// Cast operators.
// ----------------------------------------------------------------------------

hd_public::operator bool() const NOEXCEPT
{
    return valid_;
}

hd_public::operator const ec_compressed&() const NOEXCEPT
{
    return point_;
}

// Serializer.
// ----------------------------------------------------------------------------

std::string hd_public::encoded() const NOEXCEPT
{
    return encode_base58(to_hd_key());
}

// Accessors.
// ----------------------------------------------------------------------------

const hd_chain_code& hd_public::chain_code() const NOEXCEPT
{
    return chain_;
}

const hd_lineage& hd_public::lineage() const NOEXCEPT
{
    return lineage_;
}

const ec_compressed& hd_public::point() const NOEXCEPT
{
    return point_;
}

// Methods.
// ----------------------------------------------------------------------------

// HD keys do not carry a payment address prefix (just like WIF).
// So we are currently not converting to ec_public or ec_private.

hd_key hd_public::to_hd_key() const NOEXCEPT
{
    return insert_checksum<hd_key_size>(
    {
        to_big_endian(to_prefix(lineage_.prefixes)),
        to_array(lineage_.depth),
        to_big_endian(lineage_.parent_fingerprint),
        to_big_endian(lineage_.child_number),
        chain_,
        point_
    });
}

hd_public hd_public::derive_public(uint32_t index) const NOEXCEPT
{
    if (index >= hd_first_hardened_key)
        return {};

    const auto data = splice(point_, to_big_endian(index));
    const auto intermediate = split(hmac<sha512>::code(data, chain_));

    // The returned child key Ki is point(parse256(IL)) + Kpar.
    auto child = point_;
    if (!ec_add(child, intermediate.first))
        return {};

    if (lineage_.depth == max_uint8)
        return {};

    const hd_lineage lineage
    {
        lineage_.prefixes,
        add1(lineage_.depth),
        fingerprint(),
        index
    };

    return { child, intermediate.second, lineage };
}

// Helpers.
// ----------------------------------------------------------------------------

uint32_t hd_public::fingerprint() const NOEXCEPT
{
    return from_big_endian<uint32_t>(bitcoin_short_hash(point_));
}

// Operators.
// ----------------------------------------------------------------------------

bool hd_public::operator<(const hd_public& other) const NOEXCEPT
{
    return encoded() < other.encoded();
}

bool hd_public::operator==(const hd_public& other) const NOEXCEPT
{
    return valid_ == other.valid_ && chain_ == other.chain_ &&
        lineage_ == other.lineage_ && point_ == other.point_;
}

bool hd_public::operator!=(const hd_public& other) const NOEXCEPT
{
    return !(*this == other);
}

std::istream& operator>>(std::istream& in, hd_public& to)
{
    std::string value;
    in >> value;
    to.from_string(value);

    if (!to)
        throw istream_exception(value);

    return in;
}

std::ostream& operator<<(std::ostream& out, const hd_public& of) NOEXCEPT
{
    out << of.encoded();
    return out;
}

// hd_lineage
// ----------------------------------------------------------------------------

bool hd_lineage::operator==(const hd_lineage& other) const NOEXCEPT
{
    return prefixes == other.prefixes && depth == other.depth &&
        parent_fingerprint == other.parent_fingerprint &&
        child_number == other.child_number;
}

bool hd_lineage::operator!=(const hd_lineage& other) const NOEXCEPT
{
    return !(*this == other);
}

} // namespace wallet
} // namespace system
} // namespace libbitcoin
