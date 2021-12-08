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
#include <string>

BOOST_AUTO_TEST_SUITE(reject_tests)

// /Satoshi:0.12.1/
// Invalid reject payload from [46.101.110.115:8333] bad data stream
// This contradicts docs in that it is tx with readable text vs. hash.
// tx : nonstandard : too-long-mempool-chain : <empty>
#define MALFORMED_REJECT "0274784016746f6f2d6c6f6e672d6d656d706f6f6c2d636861696e"

static const std::string reason_text = "My Reason...";
static const auto version_maximum = messages::version::level::maximum;

static const hash_digest data
{
    {
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
        0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
        0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F
    }
};

BOOST_AUTO_TEST_CASE(reject__factory__tx_nonstandard_empty_data__valid)
{
    data_chunk payload;
    BOOST_REQUIRE(decode_base16(payload, MALFORMED_REJECT));
    const auto reject = messages::reject::factory(version_maximum, payload);
    BOOST_REQUIRE(reject.is_valid());
}


BOOST_AUTO_TEST_CASE(reject__constructor_1__always__invalid)
{
    messages::reject instance;
    BOOST_REQUIRE_EQUAL(false, instance.is_valid());
}

BOOST_AUTO_TEST_CASE(reject__constructor_2__always__equals_params)
{
    auto code = messages::reject::reason_code::nonstandard;
    std::string message = "Alpha Beta";
    std::string reason = "Gamma Delta";
    hash_digest data = base16_hash("4a5e1e4baab89f3a32518a88c31bc87f618f76673e2cc77ab2127b7afdeda33b");
    messages::reject instance(code, message, reason, data);
    BOOST_REQUIRE(instance.is_valid());
    BOOST_REQUIRE(code == instance.code());
    BOOST_REQUIRE_EQUAL(message, instance.message());
    BOOST_REQUIRE_EQUAL(reason, instance.reason());
    BOOST_REQUIRE(data == instance.data());
}

BOOST_AUTO_TEST_CASE(reject__constructor_3__always__equals_params)
{
    auto code = messages::reject::reason_code::nonstandard;
    std::string message = "sadfasdgd";
    std::string reason = "jgfghkggfsr";
    hash_digest data = base16_hash("ce8f4b713ffdd2658900845251890f30371856be201cd1f5b3d970f793634333");
    messages::reject instance(code, std::move(message), std::move(reason), std::move(data));
    BOOST_REQUIRE(instance.is_valid());
}

BOOST_AUTO_TEST_CASE(reject__constructor_4__always__equals_params)
{
    auto code = messages::reject::reason_code::nonstandard;
    std::string message = "Alpha Beta";
    std::string reason = "Gamma Delta";
    hash_digest data = base16_hash("4a5e1e4baab89f3a32518a88c31bc87f618f76673e2cc77ab2127b7afdeda33b");
    messages::reject expected(code, message, reason, data);
    messages::reject instance(expected);
    BOOST_REQUIRE(instance.is_valid());
    BOOST_REQUIRE(expected == instance);
    BOOST_REQUIRE(code == instance.code());
    BOOST_REQUIRE_EQUAL(message, instance.message());
    BOOST_REQUIRE_EQUAL(reason, instance.reason());
    BOOST_REQUIRE(data == instance.data());
}

BOOST_AUTO_TEST_CASE(reject__constructor_5__always__equals_params)
{
    auto code = messages::reject::reason_code::nonstandard;
    std::string message = "Alpha Beta";
    std::string reason = "Gamma Delta";
    hash_digest data = base16_hash("4a5e1e4baab89f3a32518a88c31bc87f618f76673e2cc77ab2127b7afdeda33b");
    messages::reject expected(code, message, reason, data);
    messages::reject instance(std::move(expected));
    BOOST_REQUIRE(instance.is_valid());
    BOOST_REQUIRE(code == instance.code());
    BOOST_REQUIRE_EQUAL(message, instance.message());
    BOOST_REQUIRE_EQUAL(reason, instance.reason());
    BOOST_REQUIRE(data == instance.data());
}

BOOST_AUTO_TEST_CASE(reject__from_data__insufficient_bytes__failure)
{
    static const data_chunk raw{ 0xab };
    messages::reject instance{};
    BOOST_REQUIRE_EQUAL(false, instance.from_data(version_maximum, raw));
}

BOOST_AUTO_TEST_CASE(reject__from_data__insufficient_version__failure)
{
    const messages::reject expected(
        messages::reject::reason_code::dust,
        messages::block::command,
        reason_text,
        data
    );

    const data_chunk raw = expected.to_data(version_maximum);
    messages::reject instance{};
    BOOST_REQUIRE_EQUAL(false, instance.from_data( messages::reject::version_minimum - 1, raw));
}

BOOST_AUTO_TEST_CASE(reject__from_data__code_malformed__success)
{
    const messages::reject expected(
        messages::reject::reason_code::malformed,
        messages::block::command,
        reason_text,
        data
    );

    const data_chunk raw = expected.to_data(version_maximum);
    messages::reject instance{};
    BOOST_REQUIRE(instance.from_data(messages::reject::version_minimum, raw));
    BOOST_REQUIRE(expected == instance);
}

BOOST_AUTO_TEST_CASE(reject__from_data__code_invalid__success)
{
    const messages::reject expected(
        messages::reject::reason_code::invalid,
        messages::block::command,
        reason_text,
        data
    );

    const data_chunk raw = expected.to_data(version_maximum);
    messages::reject instance{};

    BOOST_REQUIRE(instance.from_data(messages::reject::version_minimum, raw));
    BOOST_REQUIRE(expected == instance);
}

BOOST_AUTO_TEST_CASE(reject__from_data__code_obsolete__success)
{
    const messages::reject expected(
        messages::reject::reason_code::obsolete,
        messages::block::command,
        reason_text,
        data
    );

    const data_chunk raw = expected.to_data(version_maximum);
    messages::reject instance{};
    BOOST_REQUIRE(instance.from_data(messages::reject::version_minimum, raw));
    BOOST_REQUIRE(expected == instance);
}

BOOST_AUTO_TEST_CASE(reject__from_data__code_duplicate__success)
{
    const messages::reject expected(
        messages::reject::reason_code::duplicate,
        messages::block::command,
        reason_text,
        data
    );

    const data_chunk raw = expected.to_data(version_maximum);
    messages::reject instance{};
    BOOST_REQUIRE(instance.from_data(messages::reject::version_minimum, raw));
    BOOST_REQUIRE(expected == instance);
}

BOOST_AUTO_TEST_CASE(reject__from_data__code_nonstandard__success)
{
    const messages::reject expected(
        messages::reject::reason_code::nonstandard,
        messages::block::command,
        reason_text,
        data
    );

    const data_chunk raw = expected.to_data(version_maximum);
    messages::reject instance{};
    BOOST_REQUIRE(instance.from_data(messages::reject::version_minimum, raw));
    BOOST_REQUIRE(expected == instance);
}

BOOST_AUTO_TEST_CASE(reject__from_data__code_dust__success)
{
    const messages::reject expected(
        messages::reject::reason_code::dust,
        messages::block::command,
        reason_text,
        data
    );

    const data_chunk raw = expected.to_data(version_maximum);
    messages::reject instance{};
    BOOST_REQUIRE(instance.from_data(messages::reject::version_minimum, raw));
    BOOST_REQUIRE(expected == instance);
}

BOOST_AUTO_TEST_CASE(reject__from_data__code_insufficient_fee__success)
{
    const messages::reject expected(
        messages::reject::reason_code::insufficient_fee,
        messages::block::command,
        reason_text,
        data
    );

    const data_chunk raw = expected.to_data(version_maximum);
    messages::reject instance{};
    BOOST_REQUIRE(instance.from_data(messages::reject::version_minimum, raw));
    BOOST_REQUIRE(expected == instance);
}

BOOST_AUTO_TEST_CASE(reject__from_data__code_checkpoint__success)
{
    const messages::reject expected(
        messages::reject::reason_code::checkpoint,
        messages::block::command,
        reason_text,
        data
    );

    const data_chunk raw = expected.to_data(version_maximum);
    messages::reject instance{};

    BOOST_REQUIRE(instance.from_data(messages::reject::version_minimum, raw));
    BOOST_REQUIRE(expected == instance);
}

BOOST_AUTO_TEST_CASE(reject__from_data__code_undefined__success)
{
    const messages::reject expected(
        messages::reject::reason_code::undefined,
        messages::block::command,
        reason_text,
        data
    );

    const data_chunk raw = expected.to_data(version_maximum);
    messages::reject instance{};

    BOOST_REQUIRE(instance.from_data(messages::reject::version_minimum, raw));
    BOOST_REQUIRE(expected == instance);
}

BOOST_AUTO_TEST_CASE(reject__factory_1__valid_input__success)
{
    const messages::reject expected(
        messages::reject::reason_code::dust,
        messages::block::command,
        reason_text,
        data
    );

    const auto data = expected.to_data(version_maximum);
    const auto result = messages::reject::factory(version_maximum, data);
    BOOST_REQUIRE(result.is_valid());
    BOOST_REQUIRE(expected == result);
    BOOST_REQUIRE_EQUAL(data.size(), result.serialized_size(version_maximum));
    BOOST_REQUIRE_EQUAL(expected.serialized_size(version_maximum), result.serialized_size(version_maximum));
}

BOOST_AUTO_TEST_CASE(reject__factory_2__valid_input__success)
{
    const messages::reject expected(
        messages::reject::reason_code::insufficient_fee,
        messages::block::command,
        reason_text,
        data
    );

    const auto data = expected.to_data(version_maximum);
    stream::in::copy istream(data);
    const auto result = messages::reject::factory(version_maximum, istream);
    BOOST_REQUIRE(result.is_valid());
    BOOST_REQUIRE(expected == result);
    BOOST_REQUIRE_EQUAL(data.size(), result.serialized_size(version_maximum));
    BOOST_REQUIRE_EQUAL(expected.serialized_size(version_maximum), result.serialized_size(version_maximum));
}

BOOST_AUTO_TEST_CASE(reject__factory_3__valid_input__success)
{
    const messages::reject expected(
        messages::reject::reason_code::duplicate,
        messages::transaction::command,
        reason_text,
        data
    );

    const auto data = expected.to_data(version_maximum);
    read::bytes::copy source(data);
    const auto result = messages::reject::factory(version_maximum, source);
    BOOST_REQUIRE(result.is_valid());
    BOOST_REQUIRE(expected == result);
    BOOST_REQUIRE_EQUAL(data.size(), result.serialized_size(version_maximum));
    BOOST_REQUIRE_EQUAL(expected.serialized_size(version_maximum), result.serialized_size(version_maximum));
}

BOOST_AUTO_TEST_CASE(reject__code_accessor__always__initialized_value)
{
    auto code = messages::reject::reason_code::nonstandard;
    std::string message = "Alpha Beta";
    std::string reason = "Gamma Delta";
    hash_digest data = base16_hash("4a5e1e4baab89f3a32518a88c31bc87f618f76673e2cc77ab2127b7afdeda33b");
    messages::reject instance(code, message, reason, data);
    BOOST_REQUIRE(code == instance.code());
}

BOOST_AUTO_TEST_CASE(reject__code_setter__roundtrip__success)
{
    auto code = messages::reject::reason_code::nonstandard;
    std::string message = "Alpha Beta";
    std::string reason = "Gamma Delta";
    messages::reject instance;
    BOOST_REQUIRE(code != instance.code());
    instance.set_code(code);
    BOOST_REQUIRE(code == instance.code());
}

BOOST_AUTO_TEST_CASE(reject__message_accessor_1__always__initialized_value)
{
    auto code = messages::reject::reason_code::nonstandard;
    std::string message = "Alpha Beta";
    std::string reason = "Gamma Delta";
    hash_digest data = base16_hash("4a5e1e4baab89f3a32518a88c31bc87f618f76673e2cc77ab2127b7afdeda33b");
    messages::reject instance(code, message, reason, data);
    BOOST_REQUIRE_EQUAL(message, instance.message());
}

BOOST_AUTO_TEST_CASE(reject__message_accessor_2__always__initialized_value)
{
    auto code = messages::reject::reason_code::nonstandard;
    std::string message = "Alpha Beta";
    std::string reason = "Gamma Delta";
    hash_digest data = base16_hash("4a5e1e4baab89f3a32518a88c31bc87f618f76673e2cc77ab2127b7afdeda33b");
    const messages::reject instance(code, message, reason, data);
    BOOST_REQUIRE_EQUAL(message, instance.message());
}

BOOST_AUTO_TEST_CASE(reject__message_setter_1__roundtrip__success)
{
    std::string message = "Alpha Beta";
    messages::reject instance;
    BOOST_REQUIRE(message != instance.message());
    instance.set_message(message);
    BOOST_REQUIRE_EQUAL(message, instance.message());
}

BOOST_AUTO_TEST_CASE(reject__message_setter_2__roundtrip__success)
{
    std::string duplicate = "Gamma";
    std::string message = "Gamma";
    messages::reject instance;
    BOOST_REQUIRE(duplicate != instance.message());
    instance.set_message(std::move(message));
    BOOST_REQUIRE_EQUAL(duplicate, instance.message());
}

BOOST_AUTO_TEST_CASE(reject__reason_accessor_1__always__initialized_value)
{
    auto code = messages::reject::reason_code::nonstandard;
    std::string message = "Alpha Beta";
    std::string reason = "Gamma Delta";
    hash_digest data = base16_hash("4a5e1e4baab89f3a32518a88c31bc87f618f76673e2cc77ab2127b7afdeda33b");
    messages::reject instance(code, message, reason, data);
    BOOST_REQUIRE_EQUAL(reason, instance.reason());
}

BOOST_AUTO_TEST_CASE(reject__reason_accessor_2__always__initialized_value)
{
    auto code = messages::reject::reason_code::nonstandard;
    std::string message = "Alpha Beta";
    std::string reason = "Gamma Delta";
    hash_digest data = base16_hash("4a5e1e4baab89f3a32518a88c31bc87f618f76673e2cc77ab2127b7afdeda33b");
    const messages::reject instance(code, message, reason, data);
    BOOST_REQUIRE_EQUAL(reason, instance.reason());
}

BOOST_AUTO_TEST_CASE(reject__reason_setter_1__roundtrip__success)
{
    std::string reason = "Alpha Beta";
    messages::reject instance;
    BOOST_REQUIRE(reason != instance.reason());
    instance.set_reason(reason);
    BOOST_REQUIRE_EQUAL(reason, instance.reason());
}

BOOST_AUTO_TEST_CASE(reject__reason_setter_2__roundtrip__success)
{
    std::string duplicate = "Gamma";
    std::string reason = "Gamma";
    messages::reject instance;
    BOOST_REQUIRE(duplicate != instance.reason());
    instance.set_reason(std::move(reason));
    BOOST_REQUIRE_EQUAL(duplicate, instance.reason());
}

BOOST_AUTO_TEST_CASE(reject__data_accessor_1__always__initialized_value)
{
    auto code = messages::reject::reason_code::nonstandard;
    std::string message = "Alpha Beta";
    std::string reason = "Gamma Delta";
    hash_digest data = base16_hash("4a5e1e4baab89f3a32518a88c31bc87f618f76673e2cc77ab2127b7afdeda33b");
    messages::reject instance(code, message, reason, data);
    BOOST_REQUIRE(data == instance.data());
}

BOOST_AUTO_TEST_CASE(reject__data_accessor_2__always__initialized_value)
{
    auto code = messages::reject::reason_code::nonstandard;
    std::string message = "Alpha Beta";
    std::string reason = "Gamma Delta";
    hash_digest data = base16_hash("4a5e1e4baab89f3a32518a88c31bc87f618f76673e2cc77ab2127b7afdeda33b");
    const messages::reject instance(code, message, reason, data);
    BOOST_REQUIRE(data == instance.data());
}

BOOST_AUTO_TEST_CASE(reject__data_setter_1__roundtrip__success)
{
    hash_digest data = base16_hash("4a5e1e4baab89f3a32518a88c31bc87f618f76673e2cc77ab2127b7afdeda33b");
    messages::reject instance;
    BOOST_REQUIRE(data != instance.data());
    instance.set_data(data);
    BOOST_REQUIRE(data == instance.data());
}

BOOST_AUTO_TEST_CASE(reject__data_setter_2__roundtrip__success)
{
    hash_digest duplicate = base16_hash("4a5e1e4baab89f3a32518a88c31bc87f618f76673e2cc77ab2127b7afdeda33b");
    hash_digest data = base16_hash("4a5e1e4baab89f3a32518a88c31bc87f618f76673e2cc77ab2127b7afdeda33b");
    messages::reject instance;
    BOOST_REQUIRE(duplicate != instance.data());
    instance.set_data(std::move(data));
    BOOST_REQUIRE(duplicate == instance.data());
}

BOOST_AUTO_TEST_CASE(reject__operator_assign_equals__always__matches_equivalent)
{
    messages::reject value(
        messages::reject::reason_code::dust,
        "My Message",
        "My Reason",
        base16_hash("4a5e1e4baab89f3a32518a88c31bc87f618f76673e2cc77ab2127b7afdeda33b")
    );

    BOOST_REQUIRE(value.is_valid());

    messages::reject instance;
    BOOST_REQUIRE_EQUAL(false, instance.is_valid());

    instance = std::move(value);
    BOOST_REQUIRE(instance.is_valid());
}

BOOST_AUTO_TEST_CASE(reject__operator_boolean_equals__duplicates__true)
{
    const messages::reject expected(
        messages::reject::reason_code::dust,
        "My Message",
        "My Reason",
        base16_hash("4a5e1e4baab89f3a32518a88c31bc87f618f76673e2cc77ab2127b7afdeda33b")
    );

    messages::reject instance(expected);
    BOOST_REQUIRE(instance == expected);
}

BOOST_AUTO_TEST_CASE(reject__operator_boolean_equals__differs__false)
{
    const messages::reject expected(
        messages::reject::reason_code::dust,
        "My Message",
        "My Reason",
        base16_hash("4a5e1e4baab89f3a32518a88c31bc87f618f76673e2cc77ab2127b7afdeda33b")
    );

    messages::reject instance;
    BOOST_REQUIRE_EQUAL(false, instance == expected);
}

BOOST_AUTO_TEST_CASE(reject__operator_boolean_not_equals__duplicates__false)
{
    const messages::reject expected(
        messages::reject::reason_code::dust,
        "My Message",
        "My Reason",
        base16_hash("4a5e1e4baab89f3a32518a88c31bc87f618f76673e2cc77ab2127b7afdeda33b")
    );

    messages::reject instance(expected);
    BOOST_REQUIRE_EQUAL(false, instance != expected);
}

BOOST_AUTO_TEST_CASE(reject__operator_boolean_not_equals__differs__true)
{
    const messages::reject expected(
        messages::reject::reason_code::dust,
        "My Message",
        "My Reason",
        base16_hash("4a5e1e4baab89f3a32518a88c31bc87f618f76673e2cc77ab2127b7afdeda33b")
    );

    messages::reject instance;
    BOOST_REQUIRE(instance != expected);
}

BOOST_AUTO_TEST_SUITE_END()
