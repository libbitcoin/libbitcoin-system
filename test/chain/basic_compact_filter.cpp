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

struct prevout_data
{
public:
    typedef std::vector<prevout_data> list;

    hash_digest block_tx;
    uint32_t input_index;
    hash_digest input_tx;
    uint32_t output_index;
    uint64_t output_value;
    data_chunk script;
};

void print_metadata(chain::block& block)
{
    for (const auto& tx: block.transactions())
    {
        std::cout << "tx: " << tx.hash() << "\t\t\tis_coinbase: " << tx.is_coinbase() << std::endl;
        for (size_t index = 0; index < tx.inputs().size(); index++)
        {
            auto& output = tx.inputs()[index].previous_output().metadata.cache;
            data_chunk script = output.script().to_data(false);
            std::cout << "  input: " << index << std::endl;
            std::cout << "    populated : " << (output.value() != chain::output::not_found) << std::endl;
            std::cout << "    script    : " << script << std::endl;

        }
    }
}

bool add_metadata(chain::block& block, prevout_data::list& metadata)
{
    bool result = true;

    for (const auto& prevout: metadata)
    {
        result = false;
        for (const auto& tx: block.transactions())
        {
            if (tx.hash() == prevout.block_tx)
            {
                auto& output = tx.inputs()[prevout.input_index]
                   .previous_output().metadata.cache;

                output.set_value(prevout.output_value);
                output.set_script(chain::script(prevout.script, false));
                result = true;
            }
        }

        if (!result)
            break;
    }

    return result;
}

BOOST_AUTO_TEST_SUITE(chain_basic_compact_filter_tests)

BOOST_AUTO_TEST_CASE(basic_compact_filter__constructor_1__always__invalid)
{
    chain::basic_compact_filter instance;
    BOOST_REQUIRE_EQUAL(false, instance.is_valid());
}

BOOST_AUTO_TEST_CASE(basic_compact_filter__constructor_2__always__equals_params)
{
    const uint8_t filter_type = bc::basic_filter_type;
    const hash_digest block_hash = hash_literal(
        "4a5e1e4baab89f3a32518a88c31bc87f618f76673e2cc77ab2127b7afdeda33b");
    const data_chunk filter = to_chunk(base16_literal("0123456789abcdef"));

    chain::basic_compact_filter instance(block_hash, filter);
    BOOST_REQUIRE(instance.is_valid());
    BOOST_REQUIRE(filter_type == instance.filter_type());
    BOOST_REQUIRE(block_hash == instance.block_hash());
    BOOST_REQUIRE(filter == instance.filter());
}

BOOST_AUTO_TEST_CASE(basic_compact_filter__constructor_3__always__equals_params)
{
    const uint8_t filter_type = bc::basic_filter_type;
    const hash_digest block_hash = hash_literal(
        "4a5e1e4baab89f3a32518a88c31bc87f618f76673e2cc77ab2127b7afdeda33b");
    hash_digest dup_hash = block_hash;
    const data_chunk filter = to_chunk(base16_literal("0123456789abcdef"));
    data_chunk dup_filter = filter;

    chain::basic_compact_filter instance(std::move(dup_hash),
        std::move(dup_filter));
    BOOST_REQUIRE(instance.is_valid());
    BOOST_REQUIRE(filter_type == instance.filter_type());
    BOOST_REQUIRE(block_hash == instance.block_hash());
    BOOST_REQUIRE(filter == instance.filter());
}

BOOST_AUTO_TEST_CASE(basic_compact_filter__constructor_4__always__equals_params)
{
    const uint8_t filter_type = bc::basic_filter_type;
    const hash_digest block_hash = hash_literal(
        "000000000019d6689c085ae165831e934ff763ae46a2a6c172b3f1b60a8ce26f");
    const data_chunk filter = to_chunk(base16_literal("fedcba9876543210"));

    chain::basic_compact_filter value(block_hash, filter);

    chain::basic_compact_filter instance(value);
    BOOST_REQUIRE(instance.is_valid());
    BOOST_REQUIRE(filter_type == instance.filter_type());
    BOOST_REQUIRE(block_hash == instance.block_hash());
    BOOST_REQUIRE(filter == instance.filter());
}

BOOST_AUTO_TEST_CASE(basic_compact_filter__constructor_5__always__equals_params)
{
    const uint8_t filter_type = bc::basic_filter_type;
    const hash_digest block_hash = hash_literal(
        "000000000019d6689c085ae165831e934ff763ae46a2a6c172b3f1b60a8ce26f");
    const data_chunk filter = to_chunk(base16_literal("fedcba9876543210"));

    chain::basic_compact_filter value(block_hash, filter);

    chain::basic_compact_filter instance(std::move(value));
    BOOST_REQUIRE(instance.is_valid());
    BOOST_REQUIRE(filter_type == instance.filter_type());
    BOOST_REQUIRE(block_hash == instance.block_hash());
    BOOST_REQUIRE(filter == instance.filter());
}

BOOST_AUTO_TEST_CASE(basic_compact_filter__constructor_6__always__equals_params)
{
    const uint8_t filter_type = bc::basic_filter_type;
    const hash_digest block_hash = hash_literal(
        "000000000019d6689c085ae165831e934ff763ae46a2a6c172b3f1b60a8ce26f");
    const data_chunk filter = to_chunk(base16_literal("fedcba9876543210"));

    chain::compact_filter value(filter_type, block_hash, filter);

    chain::basic_compact_filter instance(value);
    BOOST_REQUIRE(instance.is_valid());
    BOOST_REQUIRE(filter_type == instance.filter_type());
    BOOST_REQUIRE(block_hash == instance.block_hash());
    BOOST_REQUIRE(filter == instance.filter());
}

BOOST_AUTO_TEST_CASE(basic_compact_filter__constructor_7__always__equals_params)
{
    const uint8_t filter_type = bc::basic_filter_type;
    const hash_digest block_hash = hash_literal(
        "000000000019d6689c085ae165831e934ff763ae46a2a6c172b3f1b60a8ce26f");
    const data_chunk filter = to_chunk(base16_literal("fedcba9876543210"));

    chain::compact_filter value(filter_type, block_hash, filter);

    chain::basic_compact_filter instance(std::move(value));
    BOOST_REQUIRE(instance.is_valid());
    BOOST_REQUIRE(filter_type == instance.filter_type());
    BOOST_REQUIRE(block_hash == instance.block_hash());
    BOOST_REQUIRE(filter == instance.filter());
}

BOOST_AUTO_TEST_CASE(basic_compact_filter__from_data__insufficient_bytes__failure)
{
    const data_chunk raw{ 0xab, 0xcd };
    chain::basic_compact_filter instance;
    BOOST_REQUIRE_EQUAL(false, instance.from_data(raw));
}

BOOST_AUTO_TEST_CASE(basic_compact_filter__factory_1__valid_input__success)
{
    const auto raw = to_chunk(base16_literal(
        "000000006fe28c0ab6f1b372c1a6a246ae63f74f931e8365e15a089c68d619000022"
        "00003ba3edfd7a7b12b27ac72c3e67768f617fc81bc3888a51323a9fb8aa4b1e5e4a"));

    chain::basic_compact_filter expected;
    expected.from_data(raw);
    const auto data = expected.to_data();
    BOOST_REQUIRE(raw == data);

    const auto result = chain::basic_compact_filter::factory(data);

    BOOST_REQUIRE(result.is_valid());
    BOOST_REQUIRE(expected == result);
    BOOST_REQUIRE_EQUAL(data.size(), result.serialized_size());
    BOOST_REQUIRE_EQUAL(expected.serialized_size(), result.serialized_size());
}

BOOST_AUTO_TEST_CASE(basic_compact_filter__factory_1__invalid_input__is_not_valid)
{
    const auto raw = to_chunk(base16_literal(
        "0a0000006fe28c0ab6f1b372c1a6a246ae63f74f931e8365e15a089c68d619000022"
        "00003ba3edfd7a7b12b27ac72c3e67768f617fc81bc3888a51323a9fb8aa4b1e5e4a"));

    chain::basic_compact_filter expected;
    expected.from_data(raw);
    const auto data = expected.to_data();
    BOOST_REQUIRE(raw == data);

    const auto result = chain::basic_compact_filter::factory(data);

    BOOST_REQUIRE(!result.is_valid());
}

BOOST_AUTO_TEST_CASE(basic_compact_filter__factory_2__valid_input__success)
{
    const auto raw = to_chunk(base16_literal(
        "000000006fe28c0ab6f1b372c1a6a246ae63f74f931e8365e15a089c68d619000022"
        "00003ba3edfd7a7b12b27ac72c3e67768f617fc81bc3888a51323a9fb8aa4b1e5e4a"));

    chain::basic_compact_filter expected;
    expected.from_data(raw);
    const auto data = expected.to_data();
    BOOST_REQUIRE(raw == data);

    data_source istream(data);
    auto result = chain::basic_compact_filter::factory(istream);

    BOOST_REQUIRE(result.is_valid());
    BOOST_REQUIRE(expected == result);
    BOOST_REQUIRE_EQUAL(data.size(), result.serialized_size());
    BOOST_REQUIRE_EQUAL(expected.serialized_size(), result.serialized_size());
}

BOOST_AUTO_TEST_CASE(basic_compact_filter__factory_2__invalid_input__is_not_valid)
{
    const auto raw = to_chunk(base16_literal(
        "0a0000006fe28c0ab6f1b372c1a6a246ae63f74f931e8365e15a089c68d619000022"
        "00003ba3edfd7a7b12b27ac72c3e67768f617fc81bc3888a51323a9fb8aa4b1e5e4a"));

    chain::basic_compact_filter expected;
    expected.from_data(raw);
    const auto data = expected.to_data();
    BOOST_REQUIRE(raw == data);

    data_source istream(data);
    auto result = chain::basic_compact_filter::factory(istream);

    BOOST_REQUIRE(!result.is_valid());
}

BOOST_AUTO_TEST_CASE(basic_compact_filter__factory_3__valid_input__success)
{
    const auto raw = to_chunk(base16_literal(
        "000000006fe28c0ab6f1b372c1a6a246ae63f74f931e8365e15a089c68d619000022"
        "00003ba3edfd7a7b12b27ac72c3e67768f617fc81bc3888a51323a9fb8aa4b1e5e4a"));

    chain::basic_compact_filter expected;
    expected.from_data(raw);
    const auto data = expected.to_data();
    BOOST_REQUIRE(raw == data);

    data_source istream(data);
    istream_reader source(istream);
    const auto result = chain::basic_compact_filter::factory(source);

    BOOST_REQUIRE(result.is_valid());
    BOOST_REQUIRE(expected == result);
    BOOST_REQUIRE_EQUAL(data.size(), result.serialized_size());
    BOOST_REQUIRE_EQUAL(expected.serialized_size(), result.serialized_size());
}

BOOST_AUTO_TEST_CASE(basic_compact_filter__factory_3__invalid_input__is_not_valid)
{
    const auto raw = to_chunk(base16_literal(
        "0a0000006fe28c0ab6f1b372c1a6a246ae63f74f931e8365e15a089c68d619000022"
        "00003ba3edfd7a7b12b27ac72c3e67768f617fc81bc3888a51323a9fb8aa4b1e5e4a"));

    chain::basic_compact_filter expected;
    expected.from_data(raw);
    const auto data = expected.to_data();
    BOOST_REQUIRE(raw == data);

    data_source istream(data);
    istream_reader source(istream);
    const auto result = chain::basic_compact_filter::factory(source);

    BOOST_REQUIRE(!result.is_valid());
}

BOOST_AUTO_TEST_CASE(basic_compact_filter__operator_assign_equals_1__always__matches_equivalent)
{
    const uint8_t filter_type = bc::basic_filter_type;
    const hash_digest block_hash = hash_literal(
        "4a5e1e4baab89f3a32518a88c31bc87f618f76673e2cc77ab2127b7afdeda33b");
    const data_chunk filter = to_chunk(base16_literal("0123456789abcdef"));

    chain::basic_compact_filter value(block_hash, filter);
    BOOST_REQUIRE(value.is_valid());
    chain::basic_compact_filter instance;
    BOOST_REQUIRE_EQUAL(false, instance.is_valid());
    instance = std::move(value);
    BOOST_REQUIRE(instance.is_valid());
    BOOST_REQUIRE(filter_type == instance.filter_type());
    BOOST_REQUIRE(block_hash == instance.block_hash());
    BOOST_REQUIRE(filter == instance.filter());
}

BOOST_AUTO_TEST_CASE(basic_compact_filter__operator_assign_equals_2__always__matches_equivalent)
{
    const uint8_t filter_type = bc::basic_filter_type;
    const hash_digest block_hash = hash_literal(
        "4a5e1e4baab89f3a32518a88c31bc87f618f76673e2cc77ab2127b7afdeda33b");
    const data_chunk filter = to_chunk(base16_literal("0123456789abcdef"));

    chain::compact_filter value(filter_type, block_hash, filter);
    BOOST_REQUIRE(value.is_valid());
    chain::basic_compact_filter instance;
    BOOST_REQUIRE_EQUAL(false, instance.is_valid());
    instance = std::move(value);
    BOOST_REQUIRE(instance.is_valid());
    BOOST_REQUIRE(filter_type == instance.filter_type());
    BOOST_REQUIRE(block_hash == instance.block_hash());
    BOOST_REQUIRE(filter == instance.filter());
}

BOOST_AUTO_TEST_CASE(basic_compact_filter__operator_boolean_equals_1__duplicates__returns_true)
{
    const chain::basic_compact_filter expected(
        hash_literal("4a5e1e4baab89f3a32518a88c31bc87f618f76673e2cc77ab2127b7afdeda33b"),
        to_chunk(base16_literal("0123456789abcdef")));

    chain::basic_compact_filter instance(expected);
    BOOST_REQUIRE(instance == expected);
}

BOOST_AUTO_TEST_CASE(basic_compact_filter__operator_boolean_equals_1__differs__returns_false)
{
    const chain::basic_compact_filter expected(
        hash_literal("4a5e1e4baab89f3a32518a88c31bc87f618f76673e2cc77ab2127b7afdeda33b"),
        to_chunk(base16_literal("0123456789abcdef")));

    chain::basic_compact_filter instance;
    BOOST_REQUIRE_EQUAL(false, instance == expected);
}

BOOST_AUTO_TEST_CASE(basic_compact_filter__operator_boolean_equals_2__duplicates__returns_true)
{
    const chain::compact_filter expected(
        bc::basic_filter_type,
        hash_literal("4a5e1e4baab89f3a32518a88c31bc87f618f76673e2cc77ab2127b7afdeda33b"),
        to_chunk(base16_literal("0123456789abcdef")));

    chain::basic_compact_filter instance(expected);
    BOOST_REQUIRE(instance == expected);
}

BOOST_AUTO_TEST_CASE(basic_compact_filter__operator_boolean_equals_2__differs__returns_false)
{
    const chain::compact_filter expected(
        bc::basic_filter_type,
        hash_literal("4a5e1e4baab89f3a32518a88c31bc87f618f76673e2cc77ab2127b7afdeda33b"),
        to_chunk(base16_literal("0123456789abcdef")));

    chain::basic_compact_filter instance;
    BOOST_REQUIRE_EQUAL(false, instance == expected);
}

BOOST_AUTO_TEST_CASE(basic_compact_filter__operator_boolean_not_equals_1__duplicates__returns_false)
{
    const chain::basic_compact_filter expected(
        hash_literal("4a5e1e4baab89f3a32518a88c31bc87f618f76673e2cc77ab2127b7afdeda33b"),
        to_chunk(base16_literal("0123456789abcdef")));

    chain::basic_compact_filter instance(expected);
    BOOST_REQUIRE_EQUAL(false, instance != expected);
}

BOOST_AUTO_TEST_CASE(basic_compact_filter__operator_boolean_not_equals_1__differs__returns_true)
{
    const chain::basic_compact_filter expected(
        hash_literal("4a5e1e4baab89f3a32518a88c31bc87f618f76673e2cc77ab2127b7afdeda33b"),
        to_chunk(base16_literal("0123456789abcdef")));

    chain::basic_compact_filter instance;
    BOOST_REQUIRE(instance != expected);
}

BOOST_AUTO_TEST_CASE(basic_compact_filter__operator_boolean_not_equals_2__duplicates__returns_false)
{
    const chain::compact_filter expected(
        bc::basic_filter_type,
        hash_literal("4a5e1e4baab89f3a32518a88c31bc87f618f76673e2cc77ab2127b7afdeda33b"),
        to_chunk(base16_literal("0123456789abcdef")));

    chain::basic_compact_filter instance(expected);
    BOOST_REQUIRE_EQUAL(false, instance != expected);
}

BOOST_AUTO_TEST_CASE(basic_compact_filter__operator_boolean_not_equals_2__differs__returns_true)
{
    const chain::compact_filter expected(
        bc::basic_filter_type,
        hash_literal("4a5e1e4baab89f3a32518a88c31bc87f618f76673e2cc77ab2127b7afdeda33b"),
        to_chunk(base16_literal("0123456789abcdef")));

    chain::basic_compact_filter instance;
    BOOST_REQUIRE(instance != expected);
}

BOOST_AUTO_TEST_CASE(basic_compact_filter__populate__block_0__success)
{
    const auto expected_block_hash = hash_literal(
        "000000000933ea01ad0ee984209779baaec3ced90fa3f408719526f8d77f4943");

    const auto expected_filter = to_chunk(base16_literal("019dfca8"));

    const auto raw_block = to_chunk(base16_literal(
        "01000000000000000000000000000000000000000000000000000000000000000000"
        "00003ba3edfd7a7b12b27ac72c3e67768f617fc81bc3888a51323a9fb8aa4b1e5e4a"
        "dae5494dffff001d1aa4ae1801010000000100000000000000000000000000000000"
        "00000000000000000000000000000000ffffffff4d04ffff001d0104455468652054"
        "696d65732030332f4a616e2f32303039204368616e63656c6c6f72206f6e20627269"
        "6e6b206f66207365636f6e64206261696c6f757420666f722062616e6b73ffffffff"
        "0100f2052a01000000434104678afdb0fe5548271967f1a67130b7105cd6a828e039"
        "09a67962e0ea1f61deb649f6bc3f4cef38c4f35504e51ec112de5c384df7ba0b8d57"
        "8a4c702b6bf11d5fac00000000"));

    auto validated_block = chain::block::factory(raw_block);
    BOOST_REQUIRE(validated_block.is_valid());

    prevout_data::list metadata = {};

    BOOST_REQUIRE(add_metadata(validated_block, metadata));

    chain::basic_compact_filter filter;
    BOOST_REQUIRE(filter.populate(validated_block));
    BOOST_REQUIRE_EQUAL(filter.block_hash(), expected_block_hash);
    BOOST_REQUIRE_EQUAL(filter.filter(), expected_filter);
}

BOOST_AUTO_TEST_CASE(basic_compact_filter__populate__block_2__success)
{
    const auto expected_block_hash = hash_literal(
        "000000006c02c8ea6e4ff69651f7fcde348fb9d557a06e6957b65552002a7820");

    const auto expected_filter = to_chunk(base16_literal("0174a170"));

    const auto raw_block = to_chunk(base16_literal(
        "0100000006128e87be8b1b4dea47a7247d5528d2702c96826c7a648497e773b80000"
        "0000e241352e3bec0a95a6217e10c3abb54adfa05abb12c126695595580fb92e2220"
        "32e7494dffff001d00d2353401010000000100000000000000000000000000000000"
        "00000000000000000000000000000000ffffffff0e0432e7494d010e062f50325348"
        "2fffffffff0100f2052a010000002321038a7f6ef1c8ca0c588aa53fa860128077c9"
        "e6c11e6830f4d7ee4e763a56b7718fac00000000"));

    auto validated_block = chain::block::factory(raw_block);
    BOOST_REQUIRE(validated_block.is_valid());

    prevout_data::list metadata = {};

    BOOST_REQUIRE(add_metadata(validated_block, metadata));

    chain::basic_compact_filter filter;
    BOOST_REQUIRE(filter.populate(validated_block));
    BOOST_REQUIRE_EQUAL(filter.block_hash(), expected_block_hash);
    BOOST_REQUIRE_EQUAL(filter.filter(), expected_filter);
}

BOOST_AUTO_TEST_CASE(basic_compact_filter__populate__block_3__success)
{
    const auto expected_block_hash = hash_literal(
        "000000008b896e272758da5297bcd98fdc6d97c9b765ecec401e286dc1fdbe10");

    const auto expected_filter = to_chunk(base16_literal("016cf7a0"));

    const auto raw_block = to_chunk(base16_literal(
        "0100000020782a005255b657696ea057d5b98f34defcf75196f64f6eeac8026c0000"
        "000041ba5afc532aae03151b8aa87b65e1594f97504a768e010c98c0add792162471"
        "86e7494dffff001d058dc2b601010000000100000000000000000000000000000000"
        "00000000000000000000000000000000ffffffff0e0486e7494d0151062f50325348"
        "2fffffffff0100f2052a01000000232103f6d9ff4c12959445ca5549c811683bf9c8"
        "8e637b222dd2e0311154c4c85cf423ac00000000"));

    auto validated_block = chain::block::factory(raw_block);
    BOOST_REQUIRE(validated_block.is_valid());

    prevout_data::list metadata = {};

    BOOST_REQUIRE(add_metadata(validated_block, metadata));

    chain::basic_compact_filter filter;
    BOOST_REQUIRE(filter.populate(validated_block));
    BOOST_REQUIRE_EQUAL(filter.block_hash(), expected_block_hash);
    BOOST_REQUIRE_EQUAL(filter.filter(), expected_filter);
}

BOOST_AUTO_TEST_CASE(basic_compact_filter__populate__block_15007__success)
{
    const auto expected_block_hash = hash_literal(
        "0000000038c44c703bae0f98cdd6bf30922326340a5996cc692aaae8bacf47ad");

    const auto expected_filter = to_chunk(base16_literal("013c3710"));

    const auto raw_block = to_chunk(base16_literal(
        "0100000002394092aa378fe35d7e9ac79c869b975c4de4374cd75eb5484b0e1e0000"
        "0000eb9b8670abd44ad6c55cee18e3020fb0c6519e7004b01a16e9164867531b67af"
        "c33bc94fffff001d123f100501010000000100000000000000000000000000000000"
        "00000000000000000000000000000000ffffffff0e04c33bc94f0115062f50325348"
        "2fffffffff0100f2052a01000000232103f268e9ae07e0f8cb2f6e901d87c510d650"
        "b97230c0365b021df8f467363cafb1ac00000000"));

    auto validated_block = chain::block::factory(raw_block);
    BOOST_REQUIRE(validated_block.is_valid());

    prevout_data::list metadata = {};

    BOOST_REQUIRE(add_metadata(validated_block, metadata));

    chain::basic_compact_filter filter;
    BOOST_REQUIRE(filter.populate(validated_block));
    BOOST_REQUIRE_EQUAL(filter.block_hash(), expected_block_hash);
    BOOST_REQUIRE_EQUAL(filter.filter(), expected_filter);
}

BOOST_AUTO_TEST_CASE(basic_compact_filter__populate__block_49291__success)
{
    const auto expected_block_hash = hash_literal(
        "0000000018b07dca1b28b4b5a119f6d6e71698ce1ed96f143f54179ce177a19c");

    const auto expected_filter = to_chunk(base16_literal(
        "0afbc2920af1b027f31f87b592276eb4c32094bb4d3697021b4c6380"));

    const auto raw_block = to_chunk(base16_literal(
        "02000000abfaf47274223ca2fea22797e44498240e482cb4c2f2baea088962f80000"
        "0000604b5b52c32305b15d7542071d8b04e750a547500005d4010727694b6e72a776"
        "e55d0d51ffff001d2118064802010000000100000000000000000000000000000000"
        "00000000000000000000000000000000ffffffff0d038bc0000102062f503253482f"
        "ffffffff01a078072a01000000232102971dd6034ed0cf52450b608d196c07d63451"
        "84fcb14deb277a6b82d526a6163dac0000000001000000081cefd96060ecb1c4fbe6"
        "75ad8a4f8bdc61d634c52b3a1c4116dee23749fe80ff000000009300493046022100"
        "866859c21f306538152e83f115bcfbf59ab4bb34887a88c03483a5dff9895f960221"
        "00a6dfd83caa609bf0516debc2bf65c3df91813a4842650a1858b3f61cfa8af24901"
        "4730440220296d4b818bb037d0f83f9f7111665f49532dfdcbec1e6b784526e9ac40"
        "46eaa602204acf3a5cb2695e8404d80bf49ab04828bcbe6fc31d25a2844ced7a8d24"
        "afbdff01ffffffff1cefd96060ecb1c4fbe675ad8a4f8bdc61d634c52b3a1c4116de"
        "e23749fe80ff020000009400483045022100e87899175991aa008176cb553c6f2bad"
        "bb5b741f328c9845fcab89f8b18cae2302200acce689896dc82933015e7230e5230d"
        "5cff8a1ffe82d334d60162ac2c5b0c9601493046022100994ad29d1e7b03e41731a4"
        "316e5f4992f0d9b6e2efc40a1ccd2c949b461175c502210099b69fdc2db00fbba214"
        "f16e286f6a49e2d8a0d5ffc6409d87796add475478d601ffffffff1e4a6d2d280ea0"
        "6680d6cf8788ac90344a9c67cca9b06005bbd6d3f6945c8272010000009500493046"
        "022100a27400ba52fd842ce07398a1de102f710a10c5599545e6c95798934352c2e4"
        "df022100f6383b0b14c9f64b6718139f55b6b9494374755b86bae7d63f5d3e583b57"
        "255a01493046022100fdf543292f34e1eeb1703b264965339ec4a450ec47585009c6"
        "06b3edbc5b617b022100a5fbb1c8de8aaaa582988cdb23622838e38de90bebcaab39"
        "28d949aa502a65d401ffffffff1e4a6d2d280ea06680d6cf8788ac90344a9c67cca9"
        "b06005bbd6d3f6945c8272020000009400493046022100ac626ac3051f875145b4fe"
        "4cfe089ea895aac73f65ab837b1ac30f5d875874fa022100bc03e79fa4b7eb707fb7"
        "35b95ff6613ca33adeaf3a0607cdcead4cfd3b51729801483045022100b720b04a5c"
        "5e2f61b7df0fcf334ab6fea167b7aaede5695d3f7c6973496adbf1022043328c4cc1"
        "cdc3e5db7bb895ccc37133e960b2fd3ece98350f774596badb387201ffffffff23a8"
        "733e349c97d6cd90f520fdd084ba15ce0a395aad03cd51370602bb9e5db301000000"
        "4a00483045022100e8556b72c5e9c0da7371913a45861a61c5df434dfd962de7b238"
        "48e1a28c86ca02205d41ceda00136267281be0974be132ac4cda1459fe2090ce4556"
        "19d8b91045e901ffffffff6856d609b881e875a5ee141c235e2a82f6b039f2b9babe"
        "82333677a5570285a6000000006a473044022040a1c631554b8b210fbdf2a73f191b"
        "2851afb51d5171fb53502a3a040a38d2c0022040d11cf6e7b41fe1b66c3d08f6ada1"
        "aee07a047cb77f242b8ecc63812c832c9a012102bcfad931b502761e452962a5976c"
        "79158a0f6d307ad31b739611dac6a297c256ffffffff6856d609b881e875a5ee141c"
        "235e2a82f6b039f2b9babe82333677a5570285a601000000930048304502205b109d"
        "f098f7e932fbf71a45869c3f80323974a826ee2770789eae178a21bfc8022100c0e7"
        "5615e53ee4b6e32b9bb5faa36ac539e9c05fa2ae6b6de5d09c08455c8b9601483045"
        "022009fb7d27375c47bea23b24818634df6a54ecf72d52e0c1268fb2a2c84f1885de"
        "022100e0ed4f15d62e7f537da0d0f1863498f9c7c0c0a4e00e4679588c8d1a9eb20b"
        "b801ffffffffa563c3722b7b39481836d5edfc1461f97335d5d1e9a23ade13680d0e"
        "2c1c371f030000006c493046022100ecc38ae2b1565643dc3c0dad5e961a5f0ea09c"
        "ab28d024f92fa05c922924157e022100ebc166edf6fbe4004c72bfe8cf40130263f9"
        "8ddff728c8e67b113dbd621906a601210211a4ed241174708c07206601b44a4c1c29"
        "e5ad8b1f731c50ca7e1d4b2a06dc1fffffffff02d0223a00000000001976a91445db"
        "0b779c0b9fa207f12a8218c94fc77aff504588ac80f0fa02000000000000000000"));

    auto validated_block = chain::block::factory(raw_block);
    BOOST_REQUIRE(validated_block.is_valid());

    prevout_data::list metadata = {
        prevout_data {
            hash_literal("4852fe372ff7534c16713b3146bbc1e86379c70bea4d5c02fb1fa0112980a081"),
            0u,
            hash_literal("ff80fe4937e2de16411c3a2bc534d661dc8b4f8aad75e6fbc4b1ec6060d9ef1c"),
            0u,
            5000000u,
            to_chunk(base16_literal("5221033423007d8f263819a2e42becaaf5b06f34cb09919e06304349d950668209eaed21021d69e2b68c3960903b702af7829fadcd80bd89b158150c85c4a75b2c8cb9c39452ae"))
        },
        prevout_data {
            hash_literal("4852fe372ff7534c16713b3146bbc1e86379c70bea4d5c02fb1fa0112980a081"),
            1u,
            hash_literal("ff80fe4937e2de16411c3a2bc534d661dc8b4f8aad75e6fbc4b1ec6060d9ef1c"),
            2u,
            5000000u,
            to_chunk(base16_literal("52210279be667ef9dcbbac55a06295ce870b07029bfcdb2dce28d959f2815b16f8179821021d69e2b68c3960903b702af7829fadcd80bd89b158150c85c4a75b2c8cb9c39452ae"))
        },
        prevout_data {
            hash_literal("4852fe372ff7534c16713b3146bbc1e86379c70bea4d5c02fb1fa0112980a081"),
            2u,
            hash_literal("72825c94f6d3d6bb0560b0a9cc679c4a3490ac8887cfd68066a00e282d6d4a1e"),
            1u,
            5000000u,
            to_chunk(base16_literal("522102a7ae1e0971fc1689bd66d2a7296da3a1662fd21a53c9e38979e0f090a375c12d21022adb62335f41eb4e27056ac37d462cda5ad783fa8e0e526ed79c752475db285d52ae"))
        },
        prevout_data {
            hash_literal("4852fe372ff7534c16713b3146bbc1e86379c70bea4d5c02fb1fa0112980a081"),
            3u,
            hash_literal("72825c94f6d3d6bb0560b0a9cc679c4a3490ac8887cfd68066a00e282d6d4a1e"),
            2u,
            5000000u,
            to_chunk(base16_literal("52210279be667ef9dcbbac55a06295ce870b07029bfcdb2dce28d959f2815b16f8179821022adb62335f41eb4e27056ac37d462cda5ad783fa8e0e526ed79c752475db285d52ae"))
        },
        prevout_data {
            hash_literal("4852fe372ff7534c16713b3146bbc1e86379c70bea4d5c02fb1fa0112980a081"),
            4u,
            hash_literal("b35d9ebb02063751cd03ad5a390ace15ba84d0fd20f590cdd6979c343e73a823"),
            1u,
            5000000u,
            to_chunk(base16_literal("512103b9d1d0e2b4355ec3cdef7c11a5c0beff9e8b8d8372ab4b4e0aaf30e80173001951ae"))
        },
        prevout_data {
            hash_literal("4852fe372ff7534c16713b3146bbc1e86379c70bea4d5c02fb1fa0112980a081"),
            5u,
            hash_literal("a6850257a577363382bebab9f239b0f6822a5e231c14eea575e881b809d65668"),
            0u,
            4070000u,
            to_chunk(base16_literal("76a9149144761ebaccd5b4bbdc2a35453585b5637b2f8588ac"))
        },
        prevout_data {
            hash_literal("4852fe372ff7534c16713b3146bbc1e86379c70bea4d5c02fb1fa0112980a081"),
            6u,
            hash_literal("a6850257a577363382bebab9f239b0f6822a5e231c14eea575e881b809d65668"),
            1u,
            5000000u,
            to_chunk(base16_literal("522103f1848b40621c5d48471d9784c8174ca060555891ace6d2b03c58eece946b1a9121020ee5d32b54d429c152fdc7b1db84f2074b0564d35400d89d11870f9273ec140c52ae"))
        },
        prevout_data {
            hash_literal("4852fe372ff7534c16713b3146bbc1e86379c70bea4d5c02fb1fa0112980a081"),
            7u,
            hash_literal("1f371c2c0e0d6813de3aa2e9d1d53573f96114fcedd5361848397b2b72c363a5"),
            3u,
            19840000u,
            to_chunk(base16_literal("76a914f4fa1cc7de742d135ea82c17adf0bb9cf5f4fb8388ac"))
        }
    };

    BOOST_REQUIRE(add_metadata(validated_block, metadata));

    chain::basic_compact_filter filter;
    BOOST_REQUIRE(filter.populate(validated_block));
    BOOST_REQUIRE_EQUAL(filter.block_hash(), expected_block_hash);
    BOOST_REQUIRE_EQUAL(filter.filter(), expected_filter);
}

BOOST_AUTO_TEST_CASE(basic_compact_filter__populate__block_180480__success)
{
    const auto expected_block_hash = hash_literal(
        "00000000fd3ceb2404ff07a785c7fdcc76619edc8ed61bd25134eaa22084366a");

    const auto expected_filter = to_chunk(base16_literal(
        "0db414c859a07e8205876354a210a75042d0463404913d61a8e068e58a3ae2aa080026"));

    const auto raw_block = to_chunk(base16_literal(
        "020000006058aa080a655aa991a444bd7d1f2defd9a3bbe68aabb69030cf3b4e0000"
        "0000d2e826bfd7ef0beaa891a7eedbc92cd6a544a6cb61c7bdaa436762eb2123ef97"
        "90f5f552ffff001d0002c90f05010000000100000000000000000000000000000000"
        "00000000000000000000000000000000ffffffff0e0300c102024608062f50325348"
        "2fffffffff01c0c6072a01000000232102e769e60137a4df6b0df8ebd387cca44c4c"
        "57ae74cc0114a8e8317c8f3bfd85e9ac00000000010000000381a0802911a01ffb02"
        "5c4dea0bc77963e8c1bb46313b71164c53f72f37fe5248010000000151ffffffffc9"
        "04b267833d215e2128bd9575242232ac2bc311550c7fc1f0ef6f264b40d14c010000"
        "000151ffffffffdf0915666649dba81886519c531649b7b02180b4af67d6885e8712"
        "99e9d5f775000000000151ffffffff0180817dcb00000000232103bb52138972c48a"
        "132fc1f637858c5189607dd0f7fe40c4f20f6ad65f2d389ba4ac0000000001000000"
        "018da38b434fba82d66052af74fc5e4e94301b114d9bc03f819dc876398404c8b401"
        "0000006c493046022100fe738b7580dc5fb5168e51fc61b5aed211125eb710680310"
        "09a22d9bbad752c5022100be5086baa384d40bcab0fa586e4f728397388d86e18b66"
        "cc417dc4f7fa4f9878012103f233299455134caa2687bdf15cb0becdfb03bd0ff2ff"
        "38e65ec6b7834295c34fffffffff022ebc1400000000001976a9147779b7fba1c1e0"
        "6b717069b80ca170e8b04458a488ac9879c40f000000001976a9142a0307cd925dbb"
        "66b534c4db33003dd18c57015788ac0000000001000000026139a62e3422a602de36"
        "c873a225c1d3ca5aeee598539ceecb9f0dc8d1ad0f83010000006b483045022100ad"
        "9f32b4a0a2ddc19b5a74eba78123e57616f1b3cfd72ce68c03ea35a3dda1f002200d"
        "bd22aa6da17213df5e70dfc3b2611d40f70c98ed9626aa5e2cde9d97461f0a012103"
        "ddb295d2f1e8319187738fb4b230fdd9aa29d0e01647f69f6d770b9ab24eea90ffff"
        "ffff983c82c87cf020040d671956525014d5c2b28c6d948c85e1a522362c0059eeae"
        "010000006b4830450221009ca544274c786d30a5d5d25e17759201ea16d3aedddf0b"
        "9e9721246f7ef6b32e02202cfa5564b6e87dfd9fd98957820e4d4e6238baeb0f65fe"
        "305d91506bb13f5f4f012103c99113deac0d5d044e3ac0346abc02501542af8c8d37"
        "59f1382c72ff84e704f7ffffffff02c0c62d00000000001976a914ae19d27efe12f5"
        "a886dc79af37ad6805db6f922d88ac70ce2000000000001976a9143b8d051d37a07e"
        "a1042067e93efe63dbf73920b988ac000000000100000002be566e8cd9933f0c75c4"
        "a82c027f7d0c544d5c101d0607ef6ae5d07b98e7f1dc000000006b483045022036a8"
        "cdfd5ea7ebc06c2bfb6e4f942bbf9a1caeded41680d11a3a9f5d8284abad022100ca"
        "cb92a5be3f39e8bc14db1710910ef7b395fa1e18f45d41c28d914fcdde33be012102"
        "bf59abf110b5131fae0a3ce1ec379329b4c896a6ae5d443edb68529cc2bc7816ffff"
        "ffff96cf67645b76ceb23fe922874847456a15feee1655082ff32d25a6bf2c0dfc90"
        "000000006a47304402203471ca2001784a5ac0abab583581f2613523da47ec5f53df"
        "833c117b5abd81500220618a2847723d57324f2984678db556dbca1a72230fc7e39d"
        "f04c2239942ba942012102925c9794fd7bb9f8b29e207d5fc491b1150135a21f5050"
        "41858889fa4edf436fffffffff026c840f00000000001976a914797fb8777d7991d8"
        "284d88bfd421ce520f0f843188ac00ca9a3b000000001976a9146d10f3f592699265"
        "d10b106eda37c3ce793f7a8588ac00000000"));

    auto validated_block = chain::block::factory(raw_block);
    BOOST_REQUIRE(validated_block.is_valid());

    prevout_data::list metadata = {
        prevout_data {
            hash_literal("62a972ba5593255dd4662d470dfb0a075cfac6302a70ceb44d07c9c04a6b9a28"),
            0u,
            hash_literal("4852fe372ff7534c16713b3146bbc1e86379c70bea4d5c02fb1fa0112980a081"),
            1u,
            50000000u,
            to_chunk(base16_literal(""))
        },
        prevout_data {
            hash_literal("62a972ba5593255dd4662d470dfb0a075cfac6302a70ceb44d07c9c04a6b9a28"),
            1u,
            hash_literal("4cd1404b266feff0c17f0c5511c32bac3222247595bd28215e213d8367b204c9"),
            1u,
            3354000000,
            to_chunk(base16_literal(""))
        },
        prevout_data {
            hash_literal("62a972ba5593255dd4662d470dfb0a075cfac6302a70ceb44d07c9c04a6b9a28"),
            2u,
            hash_literal("75f7d5e99912875e88d667afb48021b0b74916539c518618a8db4966661509df"),
            0u,
            10000000u,
            to_chunk(base16_literal(""))
        },
        prevout_data {
            hash_literal("88b760ee751176d80b0808e7e72916a63684688f9ed6374c2368f300c1f84dd0"),
            0u,
            hash_literal("b4c804843976c89d813fc09b4d111b30944e5efc74af5260d682ba4f438ba38d"),
            1u,
            265903318u,
            to_chunk(base16_literal("76a9142903b138c24be9e070b3e73ec495d77a204615e788ac"))
        },
        prevout_data {
            hash_literal("28934e7f3b8ae2b0a0d75463a5313aa3ccea5522e226eee58e4f46ff9f2b98db"),
            0u,
            hash_literal("830fadd1c80d9fcbee9c5398e5ee5acad3c125a273c836de02a622342ea63961"),
            1u,
            4000000u,
            to_chunk(base16_literal("76a91433a1941fd9a37b9821d376f5a51bd4b52fa50e2888ac"))
        },
        prevout_data {
            hash_literal("28934e7f3b8ae2b0a0d75463a5313aa3ccea5522e226eee58e4f46ff9f2b98db"),
            1u,
            hash_literal("aeee59002c3622a5e1858c946d8cb2c2d51450525619670d0420f07cc8823c98"),
            1u,
            1250000u,
            to_chunk(base16_literal("76a914e4374e8155d0865742ca12b8d4d14d41b57d682f88ac"))
        },
        prevout_data {
            hash_literal("5f0be77c5bba162290f74d01770dab8fb3b9c0a6fb9f02079de9505b6a1b2b35"),
            0u,
            hash_literal("dcf1e7987bd0e56aef07061d105c4d540c7d7f022ca8c4750c3f93d98c6e56be"),
            0u,
            999990000u,
            to_chunk(base16_literal("76a914001fa7459a6cfc64bdc178ba7e7a21603bb2568f88ac"))
        },
        prevout_data {
            hash_literal("5f0be77c5bba162290f74d01770dab8fb3b9c0a6fb9f02079de9505b6a1b2b35"),
            1u,
            hash_literal("90fc0d2cbfa6252df32f085516eefe156a4547488722e93fb2ce765b6467cf96"),
            0u,
            1036940u,
            to_chunk(base16_literal("76a914f6039952bc2b307aeec5371bfb96b66078ec17f688ac"))
        }
    };

    BOOST_REQUIRE(add_metadata(validated_block, metadata));

    chain::basic_compact_filter filter;
    BOOST_REQUIRE(filter.populate(validated_block));
    BOOST_REQUIRE_EQUAL(filter.block_hash(), expected_block_hash);
    BOOST_REQUIRE_EQUAL(filter.filter(), expected_filter);
}

BOOST_AUTO_TEST_CASE(basic_compact_filter__populate__block_926485__success)
{
    const auto expected_block_hash = hash_literal(
        "000000000000015d6077a411a8f5cc95caf775ccf11c54e27df75ce58d187313");

    const auto expected_filter = to_chunk(base16_literal(
        "09027acea61b6cc3fb33f5d52f7d088a6b2f75d234e89ca800"));

    const auto raw_block = to_chunk(base16_literal(
        "0000002060bbab0edbf3ef8a49608ee326f8fd75c473b7e3982095e2d10000000000"
        "0000c30134f8c9b6d2470488d7a67a888f6fa12f8692e0c3411fbfb92f0f68f67eed"
        "ae03ca57ef13021acc22dc4105010000000001010000000000000000000000000000"
        "000000000000000000000000000000000000ffffffff2f0315230e0004ae03ca5704"
        "3e3d1e1d0c8796bf579aef0c0000000000122f4e696e6a61506f6f6c2f5345475749"
        "542fffffffff038427a112000000001976a914876fbb82ec05caa6af7a3b5e5a983a"
        "ae6c6cc6d688ac0000000000000000266a24aa21a9ed5c748e121c0fe146d973a4ac"
        "26fa4a68b0549d46ee22d25f50a5e46fe1b377ee00000000000000002952534b424c"
        "4f434b3acd16772ad61a3c5f00287480b720f6035d5e54c9efc71be94bb5e3727f10"
        "90900120000000000000000000000000000000000000000000000000000000000000"
        "0000000000000100000000010145310e878941a1b2bc2d33797ee4d89d95eaaf2e13"
        "488063a2aa9a74490f510a0100000023220020b6744de4f6ec63cc92f7c220cdefee"
        "b1b1bed2b66c8e5706d80ec247d37e65a1ffffffff01002d3101000000001976a914"
        "3ebc40e411ed3c76f86711507ab952300890397288ac0400473044022001dd489a5d"
        "4e2fbd8a3ade27177f6b49296ba7695c40dbbe650ea83f106415fd02200b23a0602d"
        "8ff1bdf79dee118205fc7e9b40672bf31563e5741feb53fb86388501483045022100"
        "f88f040e90cc5dc6c6189d04718376ac19ed996bf9e4a3c29c3718d90ffd27180220"
        "761711f16c9e3a44f71aab55cbc0634907a1fa8bb635d971a9a01d368727bea10169"
        "522103b3623117e988b76aaabe3d63f56a4fc88b228a71e64c4cc551d1204822fe85"
        "cb2103dd823066e096f72ed617a41d3ca56717db335b1ea47a1b4c5c9dbdd0963acb"
        "a621033d7c89bd9da29fa8d44db7906a9778b53121f72191184a9fee785c39180e4b"
        "e153ae00000000010000000120925534261de4dcebb1ed5ab1b62bfe7a3ef968fb11"
        "1dc2c910adfebc6e3bdf010000006b483045022100f50198f5ae66211a4f485190ab"
        "e4dc7accdabe3bc214ebc9ea7069b97097d46e0220316a70a03014887086e335fc1b"
        "48358d46cd6bdc9af3b57c109c94af76fc915101210316cff587a01a2736d5e12e53"
        "551b18d73780b83c3bfb4fcf209c869b11b6415effffffff0220a107000000000019"
        "76a91450333046115eaa0ac9e0216565f945070e44573988ac2e7cd01a0000000019"
        "76a914c01a7ca16b47be50cbdbc60724f701d52d75156688ac000000000100000002"
        "03a25f58630d7a1ea52550365fd2156683f56daf6ca73a4b4bbd097e665163220100"
        "00006a47304402204efc3d70e4ca3049c2a425025edf22d5ca355f9ec899dbfbbeeb"
        "2268533a0f2b02204780d3739653035af4814ea52e1396d021953f948c29754edd0e"
        "e537364603dc012103f7a897e4dbecab2264b21917f90664ea8256189ea725d28740"
        "cf7ba5d85b5763ffffffff03a25f58630d7a1ea52550365fd2156683f56daf6ca73a"
        "4b4bbd097e66516322000000006a47304402202d96defdc5b4af71d6ba28c9a6042c"
        "2d5ee7bc6de565d4db84ef517445626e03022022da80320e9e489c8f41b74833dfb6"
        "a54a4eb5087cdb46eb663eef0b25caa526012103f7a897e4dbecab2264b21917f906"
        "64ea8256189ea725d28740cf7ba5d85b5763ffffffff0200e1f5050000000017a914"
        "b7e6f7ff8658b2d1fb107e3d7be7af4742e6b1b3876f88fc00000000001976a91491"
        "3bcc2be49cb534c20474c4dee1e9c4c317e7eb88ac0000000001000000043ffd60d3"
        "818431c495b89be84afac205d5d1ed663009291c560758bbd0a66df5010000006b48"
        "3045022100f344607de9df42049688dcae8ff1db34c0c7cd25ec05516e30d2bc8f12"
        "ac9b2f022060b648f6a21745ea6d9782e17bcc4277b5808326488a1f40d41e125879"
        "723d3a012103f7a897e4dbecab2264b21917f90664ea8256189ea725d28740cf7ba5"
        "d85b5763ffffffffa5379401cce30f84731ef1ba65ce27edf2cc7ce57704507ebe87"
        "14aa16a96b92010000006a473044022020c37a63bf4d7f564c2192528709b6a38ab8"
        "271bd96898c6c2e335e5208661580220435c6f1ad4d9305d2c0a818b2feb5e45d443"
        "f2f162c0f61953a14d097fd07064012103f7a897e4dbecab2264b21917f90664ea82"
        "56189ea725d28740cf7ba5d85b5763ffffffff70e731e193235ff12c318451089573"
        "1a099112ffca4b00246c60003c40f843ce000000006a473044022053760f74c29a87"
        "9e30a17b5f03a5bb057a5751a39f86fa6ecdedc36a1b7db04c022041d41c9b95f00d"
        "2d10a0373322a9025dba66c942196bc9d8adeb0e12d3024728012103f7a897e4dbec"
        "ab2264b21917f90664ea8256189ea725d28740cf7ba5d85b5763ffffffff66b7a71b"
        "3e50379c8e85fc18fe3f1a408fc985f257036c34702ba205cef09f6f000000006a47"
        "30440220499bf9e2db3db6e930228d0661395f65431acae466634d098612fd80b084"
        "59ee022040e069fc9e3c60009f521cef54c38aadbd1251aee37940e6018aadb10f19"
        "4d6a012103f7a897e4dbecab2264b21917f90664ea8256189ea725d28740cf7ba5d8"
        "5b5763ffffffff0200e1f5050000000017a9148fc37ad460fdfbd2b44fe446f6e307"
        "1a4f64faa6878f447f0b000000001976a914913bcc2be49cb534c20474c4dee1e9c4"
        "c317e7eb88ac00000000"));

    auto validated_block = chain::block::factory(raw_block);
    BOOST_REQUIRE(validated_block.is_valid());

    prevout_data::list metadata = {
        prevout_data {
            hash_literal("d06d86bacf88f1f316d4470080b7869f1c298b850e7b219124ae131c0475abb0"),
            0u,
            hash_literal("0a510f49749aaaa2638048132eafea959dd8e47e79332dbcb2a14189870e3145"),
            1u,
            20021300u,
            to_chunk(base16_literal("a914feb8a29635c56d9cd913122f90678756bf23887687"))
        },
        prevout_data {
            hash_literal("06eee51317a76a76c67499c8f782819745b58d28cdb4d8357ef7f7e6d79cc513"),
            0u,
            hash_literal("df3b6ebcfead10c9c21d11fb68f93e7afe2bb6b15aedb1ebdce41d2634559220"),
            1u,
            450380894u,
            to_chunk(base16_literal("76a914c01a7ca16b47be50cbdbc60724f701d52d75156688ac"))
        },
        prevout_data {
            hash_literal("f56da6d0bb5807561c29093066edd1d505c2fa4ae89bb895c4318481d360fd3f"),
            0u,
            hash_literal("226351667e09bd4b4b3aa76caf6df5836615d25f365025a51e7a0d63585fa203"),
            0u,
            100000000u,
            to_chunk(base16_literal("76a914913bcc2be49cb534c20474c4dee1e9c4c317e7eb88ac"))
        },
        prevout_data {
            hash_literal("f56da6d0bb5807561c29093066edd1d505c2fa4ae89bb895c4318481d360fd3f"),
            1u,
            hash_literal("226351667e09bd4b4b3aa76caf6df5836615d25f365025a51e7a0d63585fa203"),
            1u,
            16559999u,
            to_chunk(base16_literal("76a914913bcc2be49cb534c20474c4dee1e9c4c317e7eb88ac"))
        },
        prevout_data {
            hash_literal("32a52be869fc148b6104244859c879f1319cfd86e89e6f7fc1ffaaf518fa14be"),
            0u,
            hash_literal("f56da6d0bb5807561c29093066edd1d505c2fa4ae89bb895c4318481d360fd3f"),
            1u,
            16549999u,
            to_chunk(base16_literal("76a914913bcc2be49cb534c20474c4dee1e9c4c317e7eb88ac"))
        },
        prevout_data {
            hash_literal("32a52be869fc148b6104244859c879f1319cfd86e89e6f7fc1ffaaf518fa14be"),
            1u,
            hash_literal("926ba916aa1487be7e500477e57cccf2ed27ce65baf11e73840fe3cc019437a5"),
            1u,
            100000u,
            to_chunk(base16_literal("76a914913bcc2be49cb534c20474c4dee1e9c4c317e7eb88ac"))
        },
        prevout_data {
            hash_literal("32a52be869fc148b6104244859c879f1319cfd86e89e6f7fc1ffaaf518fa14be"),
            2u,
            hash_literal("ce43f8403c00606c24004bcaff1291091a7395085184312cf15f2393e131e770"),
            0u,
            16250000u,
            to_chunk(base16_literal("76a914913bcc2be49cb534c20474c4dee1e9c4c317e7eb88ac"))
        },
        prevout_data {
            hash_literal("32a52be869fc148b6104244859c879f1319cfd86e89e6f7fc1ffaaf518fa14be"),
            3u,
            hash_literal("6f9ff0ce05a22b70346c0357f285c98f401a3ffe18fc858e9c37503e1ba7b766"),
            0u,
            260000000u,
            to_chunk(base16_literal("76a914913bcc2be49cb534c20474c4dee1e9c4c317e7eb88ac"))
        }
    };

    BOOST_REQUIRE(add_metadata(validated_block, metadata));

    chain::basic_compact_filter filter;
    BOOST_REQUIRE(filter.populate(validated_block));
    BOOST_REQUIRE_EQUAL(filter.block_hash(), expected_block_hash);
    BOOST_REQUIRE_EQUAL(filter.filter(), expected_filter);
}

BOOST_AUTO_TEST_CASE(basic_compact_filter__populate__block_987876__success)
{
    const auto expected_block_hash = hash_literal(
        "0000000000000c00901f2049055e2a437c819d79a3d54fd63e6af796cd7b8a79");

    const auto expected_filter = to_chunk(base16_literal("010c0b40"));

    const auto raw_block = to_chunk(base16_literal(
        "000000202694f74969fdb542090e95a56bc8aa2d646e27033850e32f1c5f00000000"
        "0000f7e53676b3f12d5beb524ed617f2d25f5a93b5f4f52c1ba2678260d72712f8dd"
        "0a6dfe5740257e1a4b17689601010000000100000000000000000000000000000000"
        "00000000000000000000000000000000ffffffff1603e4120ff9c30a1c216900002f"
        "424d4920546573742fffffff0001205fa012000000001e76a914c486de584a735ec2"
        "f22da7cd9681614681f92173d83d0aa68688ac00000000"));

    auto validated_block = chain::block::factory(raw_block);
    BOOST_REQUIRE(validated_block.is_valid());

    prevout_data::list metadata = {};

    BOOST_REQUIRE(add_metadata(validated_block, metadata));

    chain::basic_compact_filter filter;
    BOOST_REQUIRE(filter.populate(validated_block));
    BOOST_REQUIRE_EQUAL(filter.block_hash(), expected_block_hash);
    BOOST_REQUIRE_EQUAL(filter.filter(), expected_filter);
}

BOOST_AUTO_TEST_CASE(basic_compact_filter__populate__block_1263442__success)
{
    const auto expected_block_hash = hash_literal(
        "000000006f27ddfe1dd680044a34548f41bed47eba9e6f0b310da21423bc5f33");

    const auto expected_filter = to_chunk(base16_literal("0385acb4f0fe889ef0"));

    const auto raw_block = to_chunk(base16_literal(
        "000000201c8d1a529c39a396db2db234d5ec152fa651a2872966daccbde028b40000"
        "0000083f14492679151dbfaa1a825ef4c18518e780c1f91044180280a7d33f4a98ff"
        "5f45765aaddc001d38333b9a02010000000001010000000000000000000000000000"
        "000000000000000000000000000000000000ffffffff230352471300fe5f45765afe"
        "94690a000963676d696e6572343208000000000000000000ffffffff024423a80400"
        "0000001976a914f2c25ac3d59f3d674b1d1d0a25c27339aaac0ba688ac0000000000"
        "000000266a24aa21a9edcb26cb3052426b9ebb4d19c819ef87c19677bbf3a7c46ef0"
        "855bd1b2abe834910120000000000000000000000000000000000000000000000000"
        "00000000000000000000000002000000000101d20978463906ba4ff5e7192494b88d"
        "d5eb0de85d900ab253af909106faa22cc5010000000004000000014777ff00000000"
        "0016001446c29eabe8208a33aa1023c741fa79aa92e881ff0347304402207d7ca961"
        "34f2bcfdd6b536536fdd39ad17793632016936f777ebb32c22943fda02206014d2fb"
        "8a6aa58279797f861042ba604ebd2f8f61e5bddbd9d3be5a245047b201004b632103"
        "eeaeba7ce5dc2470221e9517fb498e8d6bd4e73b85b8be655196972eb9ccd5566754"
        "b2752103a40b74d43df244799d041f32ce1ad515a6cd99501701540e38750d883ae2"
        "1d3a68ac00000000"));

    auto validated_block = chain::block::factory(raw_block);
    BOOST_REQUIRE(validated_block.is_valid());

    prevout_data::list metadata = {
        prevout_data {
            hash_literal("2c21d40599523d6d24ed1cfe06346d0080362dc1d13f86d4a7f06931c73ce0e0"),
            0u,
            hash_literal("c52ca2fa069190af53b20a905de80debd58db8942419e7f54fba0639467809d2"),
            1u,
            16745155u,
            to_chunk(base16_literal(
                "002027a5000c7917f785d8fc6e5a55adfca8717ecb973ebb7743849ff956d"
                "896a7ed"))
        }
    };

    BOOST_REQUIRE(add_metadata(validated_block, metadata));

    chain::basic_compact_filter filter;
    BOOST_REQUIRE(filter.populate(validated_block));
    BOOST_REQUIRE_EQUAL(filter.block_hash(), expected_block_hash);
    BOOST_REQUIRE_EQUAL(filter.filter(), expected_filter);
}

BOOST_AUTO_TEST_CASE(basic_compact_filter__populate__block_1414221__success)
{
    const auto expected_block_hash = hash_literal(
        "0000000000000027b2b3b3381f114f674f481544ff2be37ae3788d7e078383b1");

    const auto expected_filter = to_chunk(base16_literal("00"));

    const auto raw_block = to_chunk(base16_literal(
        "000000204ea88307a7959d8207968f152bedca5a93aefab253f1fb2cfb032a400000"
        "000070cebb14ec6dbc27a9dfd066d9849a4d3bac5f674665f73a5fe1de01a022a0c8"
        "51fda85bf05f4c19a779d14501020000000100000000000000000000000000000000"
        "00000000000000000000000000000000ffffffff18034d94154d696e657247617465"
        "3030310d000000f238f401ffffffff01c817a804000000000000000000"));

    auto validated_block = chain::block::factory(raw_block);
    BOOST_REQUIRE(validated_block.is_valid());

    prevout_data::list metadata = {};

    BOOST_REQUIRE(add_metadata(validated_block, metadata));

    chain::basic_compact_filter filter;
    BOOST_REQUIRE(filter.populate(validated_block));
    BOOST_REQUIRE_EQUAL(filter.block_hash(), expected_block_hash);
    BOOST_REQUIRE_EQUAL(filter.filter(), expected_filter);
}

BOOST_AUTO_TEST_CASE(basic_compact_filter__match_1__input_prevout__return_true)
{
    const chain::basic_compact_filter basic_filter(
        hash_literal(
            "00000000fd3ceb2404ff07a785c7fdcc76619edc8ed61bd25134eaa22084366a"),
        to_chunk(base16_literal(
            "0db414c859a07e8205876354a210a75042d0463404913d61a8e068e58a3ae2aa080026")));

    const wallet::payment_address address(
        base16_literal("001fa7459a6cfc64bdc178ba7e7a21603bb2568f"),
        wallet::payment_address::testnet_p2kh);

    BOOST_REQUIRE_EQUAL(true, basic_filter.match(address));
}

BOOST_AUTO_TEST_CASE(basic_compact_filter__match_1__unrelated_address__return_false)
{
    const chain::basic_compact_filter basic_filter(
        hash_literal(
            "00000000fd3ceb2404ff07a785c7fdcc76619edc8ed61bd25134eaa22084366a"),
        to_chunk(base16_literal(
            "0db414c859a07e8205876354a210a75042d0463404913d61a8e068e58a3ae2aa080026")));

    const wallet::payment_address address(
        base16_literal("001fa005900cf004b00100ba700021000b00500f"),
        wallet::payment_address::testnet_p2kh);

    BOOST_REQUIRE_EQUAL(false, basic_filter.match(address));
}

BOOST_AUTO_TEST_CASE(basic_compact_filter__match_2__input_prevout__return_true)
{
    const chain::basic_compact_filter basic_filter(
        hash_literal(
            "00000000fd3ceb2404ff07a785c7fdcc76619edc8ed61bd25134eaa22084366a"),
        to_chunk(base16_literal(
            "0db414c859a07e8205876354a210a75042d0463404913d61a8e068e58a3ae2aa080026")));

    const wallet::payment_address::list addresses = {
        {
            base16_literal("001fa7459a6cfc64bdc100ba700a21003b005000"),
            wallet::payment_address::testnet_p2kh
        },
        {
            base16_literal("001fa7459a6cfc64bdc178ba7e7a21603bb2568f"),
            wallet::payment_address::testnet_p2kh
        }
    };

    BOOST_REQUIRE_EQUAL(true, basic_filter.match(addresses));
}

BOOST_AUTO_TEST_CASE(basic_compact_filter__match_2__unrelated_address__return_false)
{
    const chain::basic_compact_filter basic_filter(
        hash_literal(
            "00000000fd3ceb2404ff07a785c7fdcc76619edc8ed61bd25134eaa22084366a"),
        to_chunk(base16_literal(
            "0db414c859a07e8205876354a210a75042d0463404913d61a8e068e58a3ae2aa080026")));

    const wallet::payment_address::list addresses = {
        {
            base16_literal("001fa7459a6cfc64bdc100ba700a21003b005000"),
            wallet::payment_address::testnet_p2kh
        }
    };

    BOOST_REQUIRE_EQUAL(false, basic_filter.match(addresses));
}

BOOST_AUTO_TEST_SUITE_END()
