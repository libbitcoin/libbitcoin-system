/**
 * Copyright (c) 2011-2015 libbitcoin developers (see AUTHORS)
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
#include <bitcoin/bitcoin/wallet/hd_public_key.hpp>

#include <cstdint>
#include <string>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/formats/base58.hpp>
#include <bitcoin/bitcoin/math/checksum.hpp>
#include <bitcoin/bitcoin/math/ec_keys.hpp>
#include <bitcoin/bitcoin/math/hash.hpp>
#include <bitcoin/bitcoin/utility/data.hpp>
#include <bitcoin/bitcoin/utility/deserializer.hpp>
#include <bitcoin/bitcoin/utility/serializer.hpp>

namespace libbitcoin {
namespace wallet {

hd_public_key::hd_public_key()
  : valid_(false), chain_({}), lineage_({}), point_({})
{
}

hd_public_key::hd_public_key(const hd_public_key& other)
  : valid_(other.valid_), chain_(other.chain_), lineage_(other.lineage_),
    point_(other.point_)
{
}

hd_public_key::hd_public_key(const hd_key& public_key, uint32_t prefix)
  : hd_public_key(from_key(public_key, prefix))
{
}

hd_public_key::hd_public_key(const std::string& encoded, uint32_t prefix)
  : hd_public_key(from_string(encoded, prefix))
{
}

hd_public_key::hd_public_key(const ec_compressed& point,
    const chain_code_type& chain_code, const hd_key_lineage& lineage)
  : valid_(true), point_(point), chain_(chain_code), lineage_(lineage)
{
}

hd_public_key::operator const bool() const
{
    return valid_;
}

hd_public_key::operator const ec_compressed&() const
{
    return point_;
}

hd_public_key::operator const hd_key() const
{
    hd_key out;
    build_checked_array(out,
    {
        to_big_endian(to_prefix(lineage_.prefixes)),
        to_big_endian(lineage_.depth),
        to_big_endian(lineage_.parent_fingerprint),
        to_big_endian(lineage_.child_number),
        chain_,
        point_
    });

    return out;
}

const chain_code_type& hd_public_key::chain_code() const
{
    return chain_;
}

const hd_key_lineage& hd_public_key::lineage() const
{
    return lineage_;
}

std::string hd_public_key::encoded() const
{
    return encode_base58((hd_key)*this);
}

hd_public_key hd_public_key::from_secret(const ec_secret& secret,
    const chain_code_type& chain_code, const hd_key_lineage& lineage)
{
    ec_compressed point;
    return secret_to_public(point, secret) ? 
        hd_public_key(point, chain_code, lineage) : hd_public_key();
}

hd_public_key hd_public_key::from_key(const hd_key& key, uint32_t prefix)
{
    auto stream = make_deserializer(key.begin(), key.end());
    const auto actual_prefix = stream.read_big_endian<uint32_t>();
    const auto depth = stream.read_big_endian<uint8_t>();
    const auto parent = stream.read_big_endian<uint32_t>();
    const auto child = stream.read_big_endian<uint32_t>();
    const auto chain = stream.read_bytes<hd_chain_code_size>();
    const auto compressed = stream.read_bytes<ec_compressed_size>();
    const auto point = to_array<ec_compressed_size>(compressed);

    BITCOIN_ASSERT(actual_prefix == prefix);

    // Validate the prefix against the provided value.
    if (actual_prefix != prefix)
        return hd_public_key();

    BITCOIN_ASSERT(prefix == hd_public_key::mainnet);

    // The private prefix will be zero'd here, but there's no way to access it.
    const hd_key_lineage lineage
    {
        prefix,
        depth, 
        parent,
        child
    };

    return hd_public_key(compressed, chain, lineage);
}

hd_public_key hd_public_key::from_string(const std::string& encoded,
    uint32_t prefix)
{
    hd_key key;
    if (!decode_base58(key, encoded))
        return hd_public_key();

    return hd_public_key(from_key(key, prefix));
}

hd_public_key hd_public_key::derive_public(uint32_t index) const
{
    if (hd_first_hardened_key <= index)
        return hd_public_key();

    const auto data = splice(point_, to_big_endian(index));
    const auto intermediate = split(hmac_sha512_hash(data, chain_));

    // The returned child key Ki is point(parse256(IL)) + Kpar.
    auto combined = point_;
    if (!ec_add(combined, intermediate.left))
    {
        BITCOIN_ASSERT_MSG(false, "unable derive public key at index");
        return hd_public_key();
    }

    BITCOIN_ASSERT(to_prefix(lineage_.prefixes) == hd_public_key::mainnet);

    const hd_key_lineage lineage
    {
        lineage_.prefixes,
        lineage_.depth + 1u,
        fingerprint(),
        index
    };

    return hd_public_key(combined, intermediate.right, lineage);
}

uint32_t hd_public_key::fingerprint() const
{
    const auto message_digest = bitcoin_short_hash(point_);
    return from_big_endian_unsafe<uint32_t>(message_digest.begin());
}

} // namespace wallet
} // namespace libbitcoin
