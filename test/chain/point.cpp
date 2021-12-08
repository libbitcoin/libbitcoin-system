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
#include "../test.hpp"

BOOST_AUTO_TEST_SUITE(point_tests)

const auto valid_raw_point = base16_chunk("000000000019d6689c085ae165831e934ff763ae46a2a6c172b3f1b60a8ce26f00000015");

BOOST_AUTO_TEST_CASE(point__constructor_1__always__default_initialized)
{
    chain::point instance;
    BOOST_REQUIRE(instance.is_valid());
}

BOOST_AUTO_TEST_CASE(point__constructor_2__valid_input__input_initialized)
{
    const auto hash = base16_hash("000000000019d6689c085ae165831e934ff763ae46a2a6c172b3f1b60a8ce26f");
    uint32_t index = 1234u;
    chain::point instance(hash, index);
    BOOST_REQUIRE(instance.is_valid());
    BOOST_REQUIRE_EQUAL(instance.hash(), hash);
    BOOST_REQUIRE_EQUAL(instance.index(), index);
}

BOOST_AUTO_TEST_CASE(point__constructor_3__valid_input__input_initialized)
{
    const auto hash = base16_hash("000000000019d6689c085ae165831e934ff763ae46a2a6c172b3f1b60a8ce26f");
    uint32_t index = 1234u;
    auto copy = hash;
    chain::point instance(std::move(copy), index);
    BOOST_REQUIRE(instance.is_valid());
    BOOST_REQUIRE_EQUAL(instance.hash(), hash);
    BOOST_REQUIRE_EQUAL(instance.index(), index);
}

BOOST_AUTO_TEST_CASE(point__constructor_4__valid_input__input_initialized)
{
    const chain::point expected
    {
        base16_hash("000000000019d6689c085ae165831e934ff763ae46a2a6c172b3f1b60a8ce26f"),
        524342u
    };

    chain::point instance(expected);
    BOOST_REQUIRE(instance.is_valid());
    BOOST_REQUIRE(expected == instance);
}

BOOST_AUTO_TEST_CASE(point__constructor_5__valid_input__input_initialized)
{
    chain::point expected
    {
        base16_hash("000000000019d6689c085ae165831e934ff763ae46a2a6c172b3f1b60a8ce26f"),
        524342u
    };

    chain::point instance(std::move(expected));
    BOOST_REQUIRE(instance.is_valid());
}

BOOST_AUTO_TEST_CASE(point__from_data__insufficient_bytes__failure)
{
    data_chunk data(10);
    chain::point instance;
    BOOST_REQUIRE(!instance.from_data(data));
    BOOST_REQUIRE(!instance.is_valid());
}

BOOST_AUTO_TEST_CASE(point__from_data__roundtrip__success)
{
    uint32_t index = 53213;
    const hash_digest hash
    {
        {
            0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18,
            0x01, 0x01, 0xab, 0x11, 0x11, 0xcd, 0x11, 0x11,
            0x01, 0x10, 0x11, 0xab, 0x11, 0x11, 0xcd, 0x11,
            0x01, 0x11, 0x11, 0x11, 0xab, 0x11, 0x11, 0xcd
        }
    };

    chain::point initial{ hash, index };
    BOOST_REQUIRE(initial.is_valid());
    BOOST_REQUIRE_EQUAL(initial.hash(), hash);
    BOOST_REQUIRE_EQUAL(initial.index(), index);

    chain::point point;
    BOOST_REQUIRE(point != initial);
    BOOST_REQUIRE(point.from_data(initial.to_data()));
    BOOST_REQUIRE(point.is_valid());
    BOOST_REQUIRE(point == initial);
}

BOOST_AUTO_TEST_CASE(point__factory_1__roundtrip__success)
{
    const auto raw = base16_chunk("46682488f0a721124a3905a1bb72445bf13493e2cd46c5c0c8db1c15afa0d58e00000000");
    const data_chunk data
    {
        0x46, 0x68, 0x24, 0x88, 0xf0, 0xa7, 0x21, 0x12, 0x4a, 0x39, 0x05, 0xa1,
        0xbb, 0x72, 0x44, 0x5b, 0xf1, 0x34, 0x93, 0xe2, 0xcd, 0x46, 0xc5, 0xc0,
        0xc8, 0xdb, 0x1c, 0x15, 0xaf, 0xa0, 0xd5, 0x8e, 0x00, 0x00, 0x00, 0x00
    };

    BOOST_REQUIRE_EQUAL(data, raw);

    const chain::point point(raw);
    BOOST_REQUIRE(point.is_valid());
    BOOST_REQUIRE_EQUAL(encode_hash(point.hash()), "8ed5a0af151cdbc8c0c546cde29334f15b4472bba105394a1221a7f088246846");
    BOOST_REQUIRE_EQUAL(point.index(), 0);

    const auto output = point.to_data();
    BOOST_REQUIRE_EQUAL(output, raw);
}

BOOST_AUTO_TEST_CASE(point__factory_2__roundtrip__success)
{
    const auto raw = base16_chunk("46682488f0a721124a3905a1bb72445bf13493e2cd46c5c0c8db1c15afa0d58e00000000");
    const data_chunk data
    {
        0x46, 0x68, 0x24, 0x88, 0xf0, 0xa7, 0x21, 0x12, 0x4a, 0x39, 0x05, 0xa1,
        0xbb, 0x72, 0x44, 0x5b, 0xf1, 0x34, 0x93, 0xe2, 0xcd, 0x46, 0xc5, 0xc0,
        0xc8, 0xdb, 0x1c, 0x15, 0xaf, 0xa0, 0xd5, 0x8e, 0x00, 0x00, 0x00, 0x00
    };

    BOOST_REQUIRE_EQUAL(data, raw);

    stream::in::copy istream(raw);
    const chain::point point(istream);
    BOOST_REQUIRE(point.is_valid());
    BOOST_REQUIRE_EQUAL(encode_hash(point.hash()), "8ed5a0af151cdbc8c0c546cde29334f15b4472bba105394a1221a7f088246846");
    BOOST_REQUIRE_EQUAL(point.index(), 0);

    const auto output = point.to_data();
    BOOST_REQUIRE_EQUAL(output, raw);
}

BOOST_AUTO_TEST_CASE(point__factory_3__roundtrip__success)
{
    const auto raw = base16_chunk("46682488f0a721124a3905a1bb72445bf13493e2cd46c5c0c8db1c15afa0d58e00000000");
    const data_chunk data
    {
        0x46, 0x68, 0x24, 0x88, 0xf0, 0xa7, 0x21, 0x12, 0x4a, 0x39, 0x05, 0xa1,
        0xbb, 0x72, 0x44, 0x5b, 0xf1, 0x34, 0x93, 0xe2, 0xcd, 0x46, 0xc5, 0xc0,
        0xc8, 0xdb, 0x1c, 0x15, 0xaf, 0xa0, 0xd5, 0x8e, 0x00, 0x00, 0x00, 0x00
    };

    BOOST_REQUIRE_EQUAL(data, raw);

    read::bytes::copy source(raw);
    const chain::point point(source);
    BOOST_REQUIRE(point.is_valid());
    BOOST_REQUIRE_EQUAL(encode_hash(point.hash()), "8ed5a0af151cdbc8c0c546cde29334f15b4472bba105394a1221a7f088246846");
    BOOST_REQUIRE_EQUAL(point.index(), 0);

    const auto output = point.to_data();
    BOOST_REQUIRE_EQUAL(output, raw);
}

BOOST_AUTO_TEST_CASE(point__operator_assign_equals_1__always__matches_equivalent)
{
    chain::point expected;
    BOOST_REQUIRE(expected.from_data(valid_raw_point));

    chain::point instance;
    chain::point value;
    BOOST_REQUIRE(value.from_data(valid_raw_point));
    instance = std::move(value);
    BOOST_REQUIRE(instance == expected);
}

BOOST_AUTO_TEST_CASE(point__operator_assign_equals_2__always__matches_equivalent)
{
    chain::point expected;
    BOOST_REQUIRE(expected.from_data(valid_raw_point));

    chain::point instance;
    instance = expected;
    BOOST_REQUIRE(instance == expected);
}

BOOST_AUTO_TEST_CASE(point__operator_boolean_equals__duplicates__true)
{
    chain::point alpha;
    chain::point beta;
    BOOST_REQUIRE(alpha.from_data(valid_raw_point));
    BOOST_REQUIRE(beta.from_data(valid_raw_point));
    BOOST_REQUIRE(alpha == beta);
}

BOOST_AUTO_TEST_CASE(point__operator_boolean_equals__differs__false)
{
    chain::point alpha;
    chain::point beta;
    BOOST_REQUIRE(alpha.from_data(valid_raw_point));
    BOOST_REQUIRE(!(alpha == beta));
}

BOOST_AUTO_TEST_CASE(point__operator_boolean_not_equals__duplicates__false)
{
    chain::point alpha;
    chain::point beta;
    BOOST_REQUIRE(alpha.from_data(valid_raw_point));
    BOOST_REQUIRE(beta.from_data(valid_raw_point));
    BOOST_REQUIRE(!(alpha != beta));
}

BOOST_AUTO_TEST_CASE(point__operator_boolean_not_equals__differs__true)
{
    chain::point alpha;
    chain::point beta;
    BOOST_REQUIRE(alpha.from_data(valid_raw_point));
    BOOST_REQUIRE(alpha != beta);
}

////BOOST_AUTO_TEST_CASE(point__is_mature__mature_coinbase_prevout__true)
////{
////    size_t target_height = 162u;
////    chain::point instance(hash1, 42);
////    instance.metadata.height = 50u;
////    instance.metadata.coinbase = true;
////    BOOST_REQUIRE(!instance.is_null());
////    BOOST_REQUIRE(instance.is_mature(target_height));
////}
////
////BOOST_AUTO_TEST_CASE(point__is_mature__immature_coinbase_prevout__false)
////{
////    size_t target_height = 162u;
////    chain::point instance(hash1, 42);
////    instance.metadata.height = 100u;
////    instance.metadata.coinbase = true;
////    BOOST_REQUIRE(!instance.is_null());
////    BOOST_REQUIRE(!instance.is_mature(target_height));
////}
////
////BOOST_AUTO_TEST_CASE(point__is_mature__immature_coinbase_prevout_null_input__true)
////{
////    size_t target_height = 162u;
////    chain::point instance(null_hash, chain::point::null_index);
////    instance.metadata.height = 100u;
////    instance.metadata.coinbase = true;
////    BOOST_REQUIRE(instance.is_null());
////    BOOST_REQUIRE(instance.is_mature(target_height));
////}
////
////BOOST_AUTO_TEST_CASE(point__is_mature__mature_non_coinbase_prevout__true)
////{
////    size_t target_height = 162u;
////    chain::point instance(hash1, 42);
////    instance.metadata.height = 50u;
////    instance.metadata.coinbase = false;
////    BOOST_REQUIRE(!instance.is_null());
////    BOOST_REQUIRE(instance.is_mature(target_height));
////}
////
////BOOST_AUTO_TEST_CASE(point__is_mature__immature_non_coinbase_prevout__true)
////{
////    size_t target_height = 162u;
////    chain::point instance(hash1, 42);
////    instance.metadata.height = 100u;
////    instance.metadata.coinbase = false;
////    BOOST_REQUIRE(!instance.is_null());
////    BOOST_REQUIRE(instance.is_mature(target_height));
////}

BOOST_AUTO_TEST_SUITE_END()
