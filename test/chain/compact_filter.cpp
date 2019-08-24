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

// Sponsored in part by Digital Contract Design, LLC

#include "../overloads.hpp"
#include <boost/test/unit_test.hpp>
#include <bitcoin/system.hpp>

using namespace bc::system;

BOOST_AUTO_TEST_SUITE(chain_compact_filter_tests)

BOOST_AUTO_TEST_CASE(compact_filter__constructor_1__always__invalid)
{
    chain::compact_filter instance;
    BOOST_REQUIRE_EQUAL(false, instance.is_valid());
}

BOOST_AUTO_TEST_CASE(compact_filter__constructor_2__always__equals_params)
{
    const uint8_t filter_type = 16u;
    const hash_digest block_hash = hash_literal(
        "4a5e1e4baab89f3a32518a88c31bc87f618f76673e2cc77ab2127b7afdeda33b");
    const data_chunk filter = to_chunk(base16_literal("0123456789abcdef"));

    chain::compact_filter instance(filter_type, block_hash, filter);
    BOOST_REQUIRE(instance.is_valid());
    BOOST_REQUIRE(filter_type == instance.filter_type());
    BOOST_REQUIRE(block_hash == instance.block_hash());
    BOOST_REQUIRE(filter == instance.filter());
}

BOOST_AUTO_TEST_CASE(compact_filter__constructor_3__always__equals_params)
{
    const uint8_t filter_type = 16u;
    const hash_digest block_hash = hash_literal(
        "4a5e1e4baab89f3a32518a88c31bc87f618f76673e2cc77ab2127b7afdeda33b");
    hash_digest dup_hash = block_hash;
    const data_chunk filter = to_chunk(base16_literal("0123456789abcdef"));
    data_chunk dup_filter = filter;

    chain::compact_filter instance(filter_type, std::move(dup_hash),
        std::move(dup_filter));
    BOOST_REQUIRE(instance.is_valid());
    BOOST_REQUIRE(filter_type == instance.filter_type());
    BOOST_REQUIRE(block_hash == instance.block_hash());
    BOOST_REQUIRE(filter == instance.filter());
}

BOOST_AUTO_TEST_CASE(compact_filter__constructor_4__always__equals_params)
{
    const uint8_t filter_type = 16u;
    const hash_digest block_hash = hash_literal(
        "000000000019d6689c085ae165831e934ff763ae46a2a6c172b3f1b60a8ce26f");
    const data_chunk filter = to_chunk(base16_literal("fedcba9876543210"));

    chain::compact_filter value(filter_type, block_hash, filter);

    chain::compact_filter instance(value);
    BOOST_REQUIRE(instance.is_valid());
    BOOST_REQUIRE(filter_type == instance.filter_type());
    BOOST_REQUIRE(block_hash == instance.block_hash());
    BOOST_REQUIRE(filter == instance.filter());
}

BOOST_AUTO_TEST_CASE(compact_filter__constructor_5__always__equals_params)
{
    const uint8_t filter_type = 16u;
    const hash_digest block_hash = hash_literal(
        "000000000019d6689c085ae165831e934ff763ae46a2a6c172b3f1b60a8ce26f");
    const data_chunk filter = to_chunk(base16_literal("fedcba9876543210"));

    chain::compact_filter value(filter_type, block_hash, filter);

    chain::compact_filter instance(std::move(value));
    BOOST_REQUIRE(instance.is_valid());
    BOOST_REQUIRE(filter_type == instance.filter_type());
    BOOST_REQUIRE(block_hash == instance.block_hash());
    BOOST_REQUIRE(filter == instance.filter());
}

BOOST_AUTO_TEST_CASE(compact_filter__from_data__insufficient_bytes__failure)
{
    const data_chunk raw{ 0xab, 0xcd };
    chain::compact_filter instance;
    BOOST_REQUIRE_EQUAL(false, instance.from_data(raw));
}

BOOST_AUTO_TEST_CASE(compact_filter__factory_1__valid_input__success)
{
    const auto raw = to_chunk(base16_literal(
        "0a0000006fe28c0ab6f1b372c1a6a246ae63f74f931e8365e15a089c68d619000022"
        "00003ba3edfd7a7b12b27ac72c3e67768f617fc81bc3888a51323a9fb8aa4b1e5e4a"));

    chain::compact_filter expected;
    expected.from_data(raw);
    const auto data = expected.to_data();
    BOOST_REQUIRE(raw == data);

    const auto result = chain::compact_filter::factory(data);

    BOOST_REQUIRE(result.is_valid());
    BOOST_REQUIRE(expected == result);
    BOOST_REQUIRE_EQUAL(data.size(), result.serialized_size());
    BOOST_REQUIRE_EQUAL(expected.serialized_size(), result.serialized_size());
}

BOOST_AUTO_TEST_CASE(compact_filter__factory_2__valid_input__success)
{
    const auto raw = to_chunk(base16_literal(
        "0a0000006fe28c0ab6f1b372c1a6a246ae63f74f931e8365e15a089c68d619000022"
        "00003ba3edfd7a7b12b27ac72c3e67768f617fc81bc3888a51323a9fb8aa4b1e5e4a"));

    chain::compact_filter expected;
    expected.from_data(raw);
    const auto data = expected.to_data();
    BOOST_REQUIRE(raw == data);

    data_source istream(data);
    auto result = chain::compact_filter::factory(istream);

    BOOST_REQUIRE(result.is_valid());
    BOOST_REQUIRE(expected == result);
    BOOST_REQUIRE_EQUAL(data.size(), result.serialized_size());
    BOOST_REQUIRE_EQUAL(expected.serialized_size(), result.serialized_size());
}

BOOST_AUTO_TEST_CASE(compact_filter__factory_3__valid_input__success)
{
    const auto raw = to_chunk(base16_literal(
        "0a0000006fe28c0ab6f1b372c1a6a246ae63f74f931e8365e15a089c68d619000022"
        "00003ba3edfd7a7b12b27ac72c3e67768f617fc81bc3888a51323a9fb8aa4b1e5e4a"));

    chain::compact_filter expected;
    expected.from_data(raw);
    const auto data = expected.to_data();
    BOOST_REQUIRE(raw == data);

    data_source istream(data);
    istream_reader source(istream);
    const auto result = chain::compact_filter::factory(source);

    BOOST_REQUIRE(result.is_valid());
    BOOST_REQUIRE(expected == result);
    BOOST_REQUIRE_EQUAL(data.size(), result.serialized_size());
    BOOST_REQUIRE_EQUAL(expected.serialized_size(), result.serialized_size());
}

BOOST_AUTO_TEST_CASE(compact_filter__filter_type_accessor__always__returns_initialized_value)
{
    const uint8_t filter_type = 55u;
    const hash_digest block_hash = hash_literal(
        "4a5e1e4baab89f3a32518a88c31bc87f618f76673e2cc77ab2127b7afdeda33b");
    const data_chunk filter = to_chunk(base16_literal("0123456789abcdef"));

    chain::compact_filter instance(filter_type, block_hash, filter);
    BOOST_REQUIRE(filter_type == instance.filter_type());
}

BOOST_AUTO_TEST_CASE(compact_filter__filter_type_setter__roundtrip__success)
{
    const uint8_t filter_type = 38u;

    chain::compact_filter instance;
    BOOST_REQUIRE(filter_type != instance.filter_type());
    instance.set_filter_type(filter_type);
    BOOST_REQUIRE(filter_type == instance.filter_type());
}

BOOST_AUTO_TEST_CASE(compact_filter__block_hash_accessor_1__always__returns_initialized_value)
{
    const uint8_t filter_type = 55u;
    const hash_digest block_hash = hash_literal(
        "4a5e1e4baab89f3a32518a88c31bc87f618f76673e2cc77ab2127b7afdeda33b");
    const data_chunk filter = to_chunk(base16_literal("0123456789abcdef"));

    chain::compact_filter instance(filter_type, block_hash, filter);
    BOOST_REQUIRE(block_hash == instance.block_hash());
}

BOOST_AUTO_TEST_CASE(compact_filter__block_hash_accessor_2__always__returns_initialized_value)
{
    const uint8_t filter_type = 55u;
    const hash_digest block_hash = hash_literal(
        "4a5e1e4baab89f3a32518a88c31bc87f618f76673e2cc77ab2127b7afdeda33b");
    const data_chunk filter = to_chunk(base16_literal("0123456789abcdef"));

    const chain::compact_filter instance(filter_type, block_hash, filter);
    BOOST_REQUIRE(block_hash == instance.block_hash());
}

BOOST_AUTO_TEST_CASE(compact_filter__block_hash_setter_1__roundtrip__success)
{
    const hash_digest block_hash = hash_literal(
        "4a5e1e4baab89f3a32518a88c31bc87f618f76673e2cc77ab2127b7afdeda33b");

    chain::compact_filter instance;
    BOOST_REQUIRE(block_hash != instance.block_hash());
    instance.set_block_hash(block_hash);
    BOOST_REQUIRE(block_hash == instance.block_hash());
}

BOOST_AUTO_TEST_CASE(compact_filter__block_hash_setter_2__roundtrip__success)
{
    const hash_digest block_hash = hash_literal(
        "4a5e1e4baab89f3a32518a88c31bc87f618f76673e2cc77ab2127b7afdeda33b");
    hash_digest dup = block_hash;

    chain::compact_filter instance;
    BOOST_REQUIRE(block_hash != instance.block_hash());
    instance.set_block_hash(std::move(dup));
    BOOST_REQUIRE(block_hash == instance.block_hash());
}

BOOST_AUTO_TEST_CASE(compact_filter__filter_accessor_1__always__returns_initialized_value)
{
    const uint8_t filter_type = 55u;
    const hash_digest block_hash = hash_literal(
        "4a5e1e4baab89f3a32518a88c31bc87f618f76673e2cc77ab2127b7afdeda33b");
    const data_chunk filter = to_chunk(base16_literal("0123456789abcdef"));

    chain::compact_filter instance(filter_type, block_hash, filter);
    BOOST_REQUIRE(filter == instance.filter());
}

BOOST_AUTO_TEST_CASE(compact_filter__filter_accessor_2__always__returns_initialized_value)
{
    const uint8_t filter_type = 55u;
    const hash_digest block_hash = hash_literal(
        "4a5e1e4baab89f3a32518a88c31bc87f618f76673e2cc77ab2127b7afdeda33b");
    const data_chunk filter = to_chunk(base16_literal("0123456789abcdef"));

    const chain::compact_filter instance(filter_type, block_hash, filter);
    BOOST_REQUIRE(filter == instance.filter());
}

BOOST_AUTO_TEST_CASE(compact_filter__filter_setter_1__roundtrip__success)
{
    const data_chunk filter = to_chunk(base16_literal("0123456789abcdef"));

    chain::compact_filter instance;
    BOOST_REQUIRE(filter != instance.filter());
    instance.set_filter(filter);
    BOOST_REQUIRE(filter == instance.filter());
}

BOOST_AUTO_TEST_CASE(compact_filter__filter_setter_2__roundtrip__success)
{
    const data_chunk filter = to_chunk(base16_literal("0123456789abcdef"));
    data_chunk dup = filter;

    chain::compact_filter instance;
    BOOST_REQUIRE(filter != instance.filter());
    instance.set_filter(std::move(dup));
    BOOST_REQUIRE(filter == instance.filter());
}

BOOST_AUTO_TEST_CASE(compact_filter__operator_assign_equals__always__matches_equivalent)
{
    const uint8_t filter_type = 55u;
    const hash_digest block_hash = hash_literal(
        "4a5e1e4baab89f3a32518a88c31bc87f618f76673e2cc77ab2127b7afdeda33b");
    const data_chunk filter = to_chunk(base16_literal("0123456789abcdef"));

    chain::compact_filter value(filter_type, block_hash, filter);
    BOOST_REQUIRE(value.is_valid());
    chain::compact_filter instance;
    BOOST_REQUIRE_EQUAL(false, instance.is_valid());
    instance = std::move(value);
    BOOST_REQUIRE(instance.is_valid());
    BOOST_REQUIRE(filter_type == instance.filter_type());
    BOOST_REQUIRE(block_hash == instance.block_hash());
    BOOST_REQUIRE(filter == instance.filter());
}

BOOST_AUTO_TEST_CASE(compact_filter__operator_boolean_equals__duplicates__returns_true)
{
    const chain::compact_filter expected(
        19u,
        hash_literal("4a5e1e4baab89f3a32518a88c31bc87f618f76673e2cc77ab2127b7afdeda33b"),
        to_chunk(base16_literal("0123456789abcdef")));

    chain::compact_filter instance(expected);
    BOOST_REQUIRE(instance == expected);
}

BOOST_AUTO_TEST_CASE(compact_filter__operator_boolean_equals__differs__returns_false)
{
    const chain::compact_filter expected(
        19u,
        hash_literal("4a5e1e4baab89f3a32518a88c31bc87f618f76673e2cc77ab2127b7afdeda33b"),
        to_chunk(base16_literal("0123456789abcdef")));

    chain::compact_filter instance;
    BOOST_REQUIRE_EQUAL(false, instance == expected);
}

BOOST_AUTO_TEST_CASE(compact_filter__operator_boolean_not_equals__duplicates__returns_false)
{
    const chain::compact_filter expected(
        19u,
        hash_literal("4a5e1e4baab89f3a32518a88c31bc87f618f76673e2cc77ab2127b7afdeda33b"),
        to_chunk(base16_literal("0123456789abcdef")));

    chain::compact_filter instance(expected);
    BOOST_REQUIRE_EQUAL(false, instance != expected);
}

BOOST_AUTO_TEST_CASE(compact_filter__operator_boolean_not_equals__differs__returns_true)
{
    const chain::compact_filter expected(
        19u,
        hash_literal("4a5e1e4baab89f3a32518a88c31bc87f618f76673e2cc77ab2127b7afdeda33b"),
        to_chunk(base16_literal("0123456789abcdef")));

    chain::compact_filter instance;
    BOOST_REQUIRE(instance != expected);
}

BOOST_AUTO_TEST_CASE(basic_compact_filter__get_headers__block_0__success)
{
    const auto expected = hash_literal(
        "21584579b7eb08997773e5aeff3a7f932700042d0ed2a6129012b7d7ae81b750");

    const auto previous_header = hash_literal(
        "0000000000000000000000000000000000000000000000000000000000000000");

    const auto filter = chain::compact_filter(
        bc::basic_filter_type,
        hash_literal("000000000933ea01ad0ee984209779baaec3ced90fa3f408719526f8d77f4943"),
        to_chunk(base16_literal("019dfca8")));

    const auto result = filter.get_header(previous_header);
    BOOST_REQUIRE_EQUAL(result, expected);
}

BOOST_AUTO_TEST_CASE(basic_compact_filter__get_headers__block_2__success)
{
    const auto expected = hash_literal(
        "186afd11ef2b5e7e3504f2e8cbf8df28a1fd251fe53d60dff8b1467d1b386cf0");

    const auto previous_header = hash_literal(
        "d7bdac13a59d745b1add0d2ce852f1a0442e8945fc1bf3848d3cbffd88c24fe1");

    const auto filter = chain::compact_filter(
        bc::basic_filter_type,
        hash_literal("000000006c02c8ea6e4ff69651f7fcde348fb9d557a06e6957b65552002a7820"),
        to_chunk(base16_literal("0174a170")));

    const auto result = filter.get_header(previous_header);
    BOOST_REQUIRE_EQUAL(result, expected);
}

BOOST_AUTO_TEST_CASE(basic_compact_filter__get_headers__block_3__success)
{
    const auto expected = hash_literal(
        "8d63aadf5ab7257cb6d2316a57b16f517bff1c6388f124ec4c04af1212729d2a");

    const auto previous_header = hash_literal(
        "186afd11ef2b5e7e3504f2e8cbf8df28a1fd251fe53d60dff8b1467d1b386cf0");

    const auto filter = chain::compact_filter(
        bc::basic_filter_type,
        hash_literal("000000008b896e272758da5297bcd98fdc6d97c9b765ecec401e286dc1fdbe10"),
        to_chunk(base16_literal("016cf7a0")));

    const auto result = filter.get_header(previous_header);
    BOOST_REQUIRE_EQUAL(result, expected);
}

BOOST_AUTO_TEST_CASE(basic_compact_filter__get_headers__block_15007__success)
{
    const auto expected = hash_literal(
        "07384b01311867949e0c046607c66b7a766d338474bb67f66c8ae9dbd454b20e");

    const auto previous_header = hash_literal(
        "18b5c2b0146d2d09d24fb00ff5b52bd0742f36c9e65527abdb9de30c027a4748");

    const auto filter = chain::compact_filter(
        bc::basic_filter_type,
        hash_literal("0000000038c44c703bae0f98cdd6bf30922326340a5996cc692aaae8bacf47ad"),
        to_chunk(base16_literal("013c3710")));

    const auto result = filter.get_header(previous_header);
    BOOST_REQUIRE_EQUAL(result, expected);
}

BOOST_AUTO_TEST_CASE(basic_compact_filter__get_headers__block_49291__success)
{
    const auto expected = hash_literal(
        "b6d98692cec5145f67585f3434ec3c2b3030182e1cb3ec58b855c5c164dfaaa3");

    const auto previous_header = hash_literal(
        "ed47705334f4643892ca46396eb3f4196a5e30880589e4009ef38eae895d4a13");

    const auto filter = chain::compact_filter(
        bc::basic_filter_type,
        hash_literal("0000000018b07dca1b28b4b5a119f6d6e71698ce1ed96f143f54179ce177a19c"),
        to_chunk(base16_literal("0afbc2920af1b027f31f87b592276eb4c32094bb4d3697021b4c6380")));

    const auto result = filter.get_header(previous_header);
    BOOST_REQUIRE_EQUAL(result, expected);
}

BOOST_AUTO_TEST_CASE(basic_compact_filter__get_headers__block_180480__success)
{
    const auto expected = hash_literal(
        "c582d51c0ca365e3fcf36c51cb646d7f83a67e867cb4743fd2128e3e022b700c");

    const auto previous_header = hash_literal(
        "d34ef98386f413769502808d4bac5f20f8dfd5bffc9eedafaa71de0eb1f01489");

    const auto filter = chain::compact_filter(
        bc::basic_filter_type,
        hash_literal("00000000fd3ceb2404ff07a785c7fdcc76619edc8ed61bd25134eaa22084366a"),
        to_chunk(base16_literal(
            "0db414c859a07e8205876354a210a75042d0463404913d61a8e068e58a3ae2aa080026")));

    const auto result = filter.get_header(previous_header);
    BOOST_REQUIRE_EQUAL(result, expected);
}

BOOST_AUTO_TEST_CASE(basic_compact_filter__get_headers__block_926485__success)
{
    const auto expected = hash_literal(
        "546c574a0472144bcaf9b6aeabf26372ad87c7af7d1ee0dbfae5e099abeae49c");

    const auto previous_header = hash_literal(
        "8f13b9a9c85611635b47906c3053ac53cfcec7211455d4cb0d63dc9acc13d472");

    const auto filter = chain::compact_filter(
        bc::basic_filter_type,
        hash_literal("000000000000015d6077a411a8f5cc95caf775ccf11c54e27df75ce58d187313"),
        to_chunk(base16_literal("09027acea61b6cc3fb33f5d52f7d088a6b2f75d234e89ca800")));

    const auto result = filter.get_header(previous_header);
    BOOST_REQUIRE_EQUAL(result, expected);
}

BOOST_AUTO_TEST_CASE(basic_compact_filter__get_headers__block_987876__success)
{
    const auto expected = hash_literal(
        "0965a544743bbfa36f254446e75630c09404b3d164a261892372977538928ed5");

    const auto previous_header = hash_literal(
        "fe4d230dbb0f4fec9bed23a5283e08baf996e3f32b93f52c7de1f641ddfd04ad");

    const auto filter = chain::compact_filter(
        bc::basic_filter_type,
        hash_literal("0000000000000c00901f2049055e2a437c819d79a3d54fd63e6af796cd7b8a79"),
        to_chunk(base16_literal("010c0b40")));

    const auto result = filter.get_header(previous_header);
    BOOST_REQUIRE_EQUAL(result, expected);
}

BOOST_AUTO_TEST_CASE(basic_compact_filter__get_headers__block_1263442__success)
{
    const auto expected = hash_literal(
        "4e6d564c2a2452065c205dd7eb2791124e0c4e0dbb064c410c24968572589dec");

    const auto previous_header = hash_literal(
        "31d66d516a9eda7de865df29f6ef6cb8e4bf9309e5dac899968a9a62a5df61e3");

    const auto filter = chain::compact_filter(
        bc::basic_filter_type,
        hash_literal("000000006f27ddfe1dd680044a34548f41bed47eba9e6f0b310da21423bc5f33"),
        to_chunk(base16_literal("0385acb4f0fe889ef0")));

    const auto result = filter.get_header(previous_header);
    BOOST_REQUIRE_EQUAL(result, expected);
}

BOOST_AUTO_TEST_CASE(basic_compact_filter__get_headers__block_1414221__success)
{
    const auto expected = hash_literal(
        "021e8882ef5a0ed932edeebbecfeda1d7ce528ec7b3daa27641acf1189d7b5dc");

    const auto previous_header = hash_literal(
        "5e5e12d90693c8e936f01847859404c67482439681928353ca1296982042864e");

    const auto filter = chain::compact_filter(
        bc::basic_filter_type,
        hash_literal("0000000000000027b2b3b3381f114f674f481544ff2be37ae3788d7e078383b1"),
        to_chunk(base16_literal("00")));

    const auto result = filter.get_header(previous_header);
    BOOST_REQUIRE_EQUAL(result, expected);
}

BOOST_AUTO_TEST_SUITE_END()
