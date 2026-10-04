/**
 * Copyright (c) 2011-2026 libbitcoin developers
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
#include "../test.hpp"

BOOST_AUTO_TEST_SUITE(short_id_tests)

using namespace system::chain;

static const header instance
{
    10,
    base16_hash("000000000019d6689c085ae165831e934ff763ae46a2a6c172b3f1b60a8ce26f"),
    base16_hash("4a5e1e4baab89f3a32518a88c31bc87f618f76673e2cc77ab2127b7afdeda33b"),
    531234,
    6523454,
    68644
};

BOOST_AUTO_TEST_CASE(short_id__mask__low_six_bytes)
{
    BOOST_REQUIRE_EQUAL(short_id::bits, 48u);
    BOOST_REQUIRE_EQUAL(short_id::mask, 0x0000ffffffffffff_u64);
}

BOOST_AUTO_TEST_CASE(short_id__to_key__header_and_nonce__sha256_first_half)
{
    constexpr uint64_t nonce = 0x0102030405060708;
    const auto data = splice(instance.to_data(), to_little_endian(nonce));
    const auto expected = to_siphash_key(split(sha256_hash(data)).first);
    BOOST_REQUIRE(short_id::to_key(instance, nonce) == expected);
}

BOOST_AUTO_TEST_CASE(short_id__to_id__hash__masked_siphash)
{
    constexpr auto hash = base16_hash("4a5e1e4baab89f3a32518a88c31bc87f618f76673e2cc77ab2127b7afdeda33b");
    const auto key = short_id::to_key(instance, 42);
    const auto id = short_id::to_id(key, hash);
    BOOST_REQUIRE_EQUAL(id, bit_and(siphash(key, hash), short_id::mask));
    BOOST_REQUIRE(is_zero(bit_and(id, bit_not(short_id::mask))));
}

BOOST_AUTO_TEST_CASE(short_id__from_mini__bytes__little_endian)
{
    constexpr mini_hash id{ 0x01, 0x02, 0x03, 0x04, 0x05, 0x06 };
    BOOST_REQUIRE_EQUAL(short_id::from_mini(id), 0x0000060504030201_u64);
}

BOOST_AUTO_TEST_CASE(short_id__to_mini__integer__low_six_bytes)
{
    constexpr mini_hash expected{ 0x01, 0x02, 0x03, 0x04, 0x05, 0x06 };
    BOOST_REQUIRE_EQUAL(short_id::to_mini(0x0807060504030201_u64), expected);
}

BOOST_AUTO_TEST_CASE(short_id__to_mini__from_mini__round_trip)
{
    constexpr mini_hash id{ 0xff, 0x00, 0xaa, 0x55, 0x01, 0x80 };
    BOOST_REQUIRE_EQUAL(short_id::to_mini(short_id::from_mini(id)), id);
}

BOOST_AUTO_TEST_SUITE_END()
