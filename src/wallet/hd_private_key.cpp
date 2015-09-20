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
#include <bitcoin/bitcoin/wallet/hd_private_key.hpp>

#include <cstdint>
#include <string>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/formats/base58.hpp>
#include <bitcoin/bitcoin/math/checksum.hpp>
#include <bitcoin/bitcoin/math/ec_keys.hpp>
#include <bitcoin/bitcoin/math/hash.hpp>
#include <bitcoin/bitcoin/utility/assert.hpp>
#include <bitcoin/bitcoin/utility/data.hpp>
#include <bitcoin/bitcoin/utility/deserializer.hpp>
#include <bitcoin/bitcoin/utility/serializer.hpp>

namespace libbitcoin {
namespace wallet {

hd_private_key::hd_private_key()
  : hd_public_key(), secret_({})
{
}

hd_private_key::hd_private_key(const hd_private_key& other)
  : hd_public_key(other), secret_(other.secret_)
{
}

hd_private_key::hd_private_key(const data_chunk& seed, uint64_t prefixes)
  : hd_private_key(from_seed(seed, prefixes))
{
}

hd_private_key::hd_private_key(const hd_key& private_key, uint64_t prefixes)
  : hd_private_key(from_key(private_key, prefixes))
{
}

hd_private_key::hd_private_key(const std::string& encoded, uint64_t prefixes)
  : hd_private_key(from_string(encoded, prefixes))
{
}

hd_private_key::hd_private_key(const ec_secret& secret,
    const chain_code_type& chain_code, const hd_key_lineage& lineage)
    : hd_public_key(from_secret(secret, chain_code, lineage)),
    secret_(secret)
{
}

hd_private_key::operator const ec_secret&() const
{
    return secret_;
}

hd_private_key::operator const hd_key() const
{
    static constexpr uint8_t private_key_padding = 0x00;

    BITCOIN_ASSERT(lineage_.prefixes == hd_private_key::mainnet);
    BITCOIN_ASSERT(to_prefix(lineage_.prefixes) == hd_private_key::mainnet);
    BITCOIN_ASSERT(hd_public_key::to_prefix(lineage_.prefixes) == hd_public_key::mainnet);

    hd_key out;
    build_checked_array(out,
    {
        to_big_endian(to_prefix(lineage_.prefixes)),
        to_big_endian(lineage_.depth),
        to_big_endian(lineage_.parent_fingerprint),
        to_big_endian(lineage_.child_number),
        chain_,
        to_big_endian(private_key_padding),
        secret_
    });

    return out;
}

std::string hd_private_key::encoded() const
{
    return encode_base58((hd_key)*this);
}

hd_private_key hd_private_key::from_key(const hd_key& key, uint64_t prefixes)
{
    auto stream = make_deserializer(key.begin(), key.end());
    const auto prefix = stream.read_big_endian<uint32_t>();
    const auto depth = stream.read_big_endian<uint8_t>();
    const auto parent = stream.read_big_endian<uint32_t>();
    const auto child = stream.read_big_endian<uint32_t>();
    const auto chain = stream.read_bytes<hd_chain_code_size>();
    /*const auto padding =*/ stream.read_big_endian<uint8_t>();
    const auto secret = stream.read_bytes<ec_secret_size>();

    BITCOIN_ASSERT(prefix == to_prefix(prefixes));

    // Validate the prefix against the provided value.
    if (prefix != to_prefix(prefixes))
        return hd_private_key();

    BITCOIN_ASSERT(prefixes == hd_private_key::mainnet);
    BITCOIN_ASSERT(to_prefix(prefixes) == hd_private_key::mainnet);
    BITCOIN_ASSERT(hd_public_key::to_prefix(prefixes) == hd_public_key::mainnet);

    const hd_key_lineage lineage
    {
        prefixes,
        depth,
        parent,
        child
    };

    return hd_private_key(secret, chain, lineage);
}

hd_private_key hd_private_key::from_seed(data_slice seed, uint64_t prefixes)
{
    // This is a magic constant from BIP32.
    // see: bip-0032.mediawiki#master-key-generation
    static const std::string key("Bitcoin seed");

    const auto intermediate = split(hmac_sha512_hash(seed, to_chunk(key)));

    // The key is invalid if parse256(IL) >= n or 0:
    if (!verify(intermediate.left))
        return hd_private_key();

    BITCOIN_ASSERT(prefixes == hd_private_key::mainnet);
    BITCOIN_ASSERT(to_prefix(prefixes) == hd_private_key::mainnet);
    BITCOIN_ASSERT(hd_public_key::to_prefix(prefixes) == hd_public_key::mainnet);

    const auto master = hd_key_lineage
    {
        prefixes,
        0x00,
        0x00000000,
        0x00000000
    };

    return hd_private_key(intermediate.left, intermediate.right, master);
}

hd_private_key hd_private_key::from_string(const std::string& encoded,
    uint64_t prefixes)
{
    hd_key key;
    return decode_base58(key, encoded) ? hd_private_key(key) : hd_private_key();
}

hd_private_key hd_private_key::derive_private(uint32_t index) const
{
    constexpr uint8_t depth = 0x00;
    constexpr size_t size = sizeof(depth) + ec_secret_size + sizeof(index);

    byte_array<size> data;
    if (hd_first_hardened_key <= index)
        data = splice(to_array(depth), secret_, to_big_endian(index));
    else
        data = splice(point_, to_big_endian(index));

    const auto intermediate = split(hmac_sha512_hash(data, chain_));

    // The child key ki is (parse256(IL) + kpar) mod n:
    auto combined = secret_;
    if (!ec_add(combined, intermediate.left))
    {
        BITCOIN_ASSERT_MSG(false, "unable derive private key at index");
        return hd_private_key();
    }

    BITCOIN_ASSERT(lineage_.prefixes == hd_private_key::mainnet);
    BITCOIN_ASSERT(to_prefix(lineage_.prefixes) == hd_private_key::mainnet);
    BITCOIN_ASSERT(hd_public_key::to_prefix(lineage_.prefixes) == hd_public_key::mainnet);

    const hd_key_lineage lineage
    {
        lineage_.prefixes,
        lineage_.depth + 1u,
        fingerprint(),
        index
    };

    return hd_private_key(combined, intermediate.right, lineage);
}

hd_public_key hd_private_key::derive_public(uint32_t index) const
{
    return derive_private(index);
}

} // namespace wallet
} // namespace libbitcoin
