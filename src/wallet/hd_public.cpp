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
#include <bitcoin/bitcoin/wallet/hd_public.hpp>

#include <cstdint>
#include <iostream>
#include <string>
#include <boost/program_options.hpp>
#include <bitcoin/bitcoin/constants.hpp>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/formats/base_58.hpp>
#include <bitcoin/bitcoin/math/checksum.hpp>
#include <bitcoin/bitcoin/math/elliptic_curve.hpp>
#include <bitcoin/bitcoin/math/hash.hpp>
#include <bitcoin/bitcoin/math/limits.hpp>
#include <bitcoin/bitcoin/utility/container_source.hpp>
#include <bitcoin/bitcoin/utility/data.hpp>
#include <bitcoin/bitcoin/utility/endian.hpp>
#include <bitcoin/bitcoin/utility/istream_reader.hpp>
#include <bitcoin/bitcoin/wallet/ec_public.hpp>
#include <bitcoin/bitcoin/wallet/hd_private.hpp>

namespace libbitcoin {
namespace wallet {

const uint32_t hd_public::mainnet = 76067358;
const uint32_t hd_public::testnet = 70617039;

// hd_public
// ----------------------------------------------------------------------------

hd_public::hd_public()
  : valid_(false), chain_(null_hash), lineage_({0, 0, 0, 0}),
    point_(null_compressed_point)
{
}

hd_public::hd_public(const hd_public& other)
  : valid_(other.valid_), chain_(other.chain_), lineage_(other.lineage_),
    point_(other.point_)
{
}

// This cannot validate the version.
hd_public::hd_public(const hd_key& public_key)
  : hd_public(from_key(public_key))
{
}

// This cannot validate the version.
hd_public::hd_public(const std::string& encoded)
  : hd_public(from_string(encoded))
{
}

// This validates the version.
hd_public::hd_public(const hd_key& public_key, uint32_t prefix)
  : hd_public(from_key(public_key, prefix))
{
}

// This validates the version.
hd_public::hd_public(const std::string& encoded, uint32_t prefix)
  : hd_public(from_string(encoded, prefix))
{
}

hd_public::hd_public(const ec_compressed& point,
    const hd_chain_code& chain_code, const hd_lineage& lineage)
  : valid_(true), point_(point), chain_(chain_code), lineage_(lineage)
{
}

// Factories.
// ----------------------------------------------------------------------------

hd_public hd_public::from_secret(const ec_secret& secret,
    const hd_chain_code& chain_code, const hd_lineage& lineage)
{
    ec_compressed point;
    return secret_to_public(point, secret) ?
        hd_public(point, chain_code, lineage) : hd_public{};
}

hd_public hd_public::from_key(const hd_key& key)
{
    const auto prefix = from_big_endian_unsafe<uint32_t>(key.begin());
    return from_key(key, prefix);
}

hd_public hd_public::from_string(const std::string& encoded)
{
    hd_key key;
    if (!decode_base58(key, encoded))
        return{};

    return hd_public(from_key(key));
}

hd_public hd_public::from_key(const hd_key& key, uint32_t prefix)
{
    stream_source<hd_key> istream(key);
    istream_reader reader(istream);

    const auto actual_prefix = reader.read_4_bytes_big_endian();
    const auto depth = reader.read_byte();
    const auto parent = reader.read_4_bytes_big_endian();
    const auto child = reader.read_4_bytes_big_endian();
    const auto chain = reader.read_forward<hd_chain_code_size>();
    const auto compressed = reader.read_forward<ec_compressed_size>();
    const auto point = to_array<ec_compressed_size>(compressed);

    // Validate the prefix against the provided value.
    if (actual_prefix != prefix)
        return{};

    // The private prefix will be zero'd here, but there's no way to access it.
    const hd_lineage lineage
    {
        prefix,
        depth,
        parent,
        child
    };

    return hd_public(compressed, chain, lineage);
}

hd_public hd_public::from_string(const std::string& encoded,
    uint32_t prefix)
{
    hd_key key;
    if (!decode_base58(key, encoded))
        return{};

    return hd_public(from_key(key, prefix));
}

// Cast operators.
// ----------------------------------------------------------------------------

hd_public::operator const bool() const
{
    return valid_;
}

hd_public::operator const ec_compressed&() const
{
    return point_;
}

// Serializer.
// ----------------------------------------------------------------------------

std::string hd_public::encoded() const
{
    return encode_base58(to_hd_key());
}

// Accessors.
// ----------------------------------------------------------------------------

const hd_chain_code& hd_public::chain_code() const
{
    return chain_;
}

const hd_lineage& hd_public::lineage() const
{
    return lineage_;
}

const ec_compressed& hd_public::point() const
{
    return point_;
}

// Methods.
// ----------------------------------------------------------------------------

// HD keys do not carry a payment address prefix (just like WIF).
// So we are currently not converting to ec_public or ec_private.

hd_key hd_public::to_hd_key() const
{
    hd_key out;
    build_checked_array(out,
    {
        to_big_endian(to_prefix(lineage_.prefixes)),
        to_array(lineage_.depth),
        to_big_endian(lineage_.parent_fingerprint),
        to_big_endian(lineage_.child_number),
        chain_,
        point_
    });

    return out;
}

hd_public hd_public::derive_public(uint32_t index) const
{
    if (index >= hd_first_hardened_key)
        return{};

    const auto data = splice(point_, to_big_endian(index));
    const auto intermediate = split(hmac_sha512_hash(data, chain_));

    // The returned child key Ki is point(parse256(IL)) + Kpar.
    auto combined = point_;
    if (!ec_add(combined, intermediate.left))
        return{};

    if (lineage_.depth == max_uint8)
        return{};

    const hd_lineage lineage
    {
        lineage_.prefixes,
        static_cast<uint8_t>(lineage_.depth + 1),
        fingerprint(),
        index
    };

    return hd_public(combined, intermediate.right, lineage);
}

// Helpers.
// ----------------------------------------------------------------------------

uint32_t hd_public::fingerprint() const
{
    const auto message_digest = bitcoin_short_hash(point_);
    return from_big_endian_unsafe<uint32_t>(message_digest.begin());
}

// Operators.
// ----------------------------------------------------------------------------

hd_public& hd_public::operator=(const hd_public& other)
{
    valid_ = other.valid_;
    chain_ = other.chain_;
    lineage_ = other.lineage_;
    point_ = other.point_;
    return *this;
}

bool hd_public::operator<(const hd_public& other) const
{
    return encoded() < other.encoded();
}

bool hd_public::operator==(const hd_public& other) const
{
    return valid_ == other.valid_ && chain_ == other.chain_ &&
        lineage_ == other.lineage_ && point_ == other.point_;
}

bool hd_public::operator!=(const hd_public& other) const
{
    return !(*this == other);
}

std::istream& operator>>(std::istream& in, hd_public& to)
{
    std::string value;
    in >> value;
    to = hd_public(value);

    if (!to)
    {
        using namespace boost::program_options;
        BOOST_THROW_EXCEPTION(invalid_option_value(value));
    }

    return in;
}

std::ostream& operator<<(std::ostream& out, const hd_public& of)
{
    out << of.encoded();
    return out;
}

// hd_lineage
// ----------------------------------------------------------------------------

bool hd_lineage::operator == (const hd_lineage& other) const
{
    return prefixes == other.prefixes && depth == other.depth &&
        parent_fingerprint == other.parent_fingerprint &&
        child_number == other.child_number;
}

bool hd_lineage::operator!=(const hd_lineage& other) const
{
    return !(*this == other);
}

} // namespace wallet
} // namespace libbitcoin
