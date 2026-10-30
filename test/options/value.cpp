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

using namespace bc::system::options;

BOOST_AUTO_TEST_SUITE(value_tests)

template <typename Type>
static std::unique_ptr<typed_value<Type>> make(Type* store=nullptr)
{
    return std::unique_ptr<typed_value<Type>>{ value<Type>(store) };
}

template <typename Type>
static Type parse_one(const std::string& token)
{
    std::any out{};
    make<Type>()->parse(out, { token }, "name");
    return std::any_cast<Type>(out);
}

// arity

BOOST_AUTO_TEST_CASE(value__tokens__default__one)
{
    const auto instance = make<int32_t>();
    BOOST_REQUIRE_EQUAL(instance->min_tokens(), 1u);
    BOOST_REQUIRE_EQUAL(instance->max_tokens(), 1u);
}

BOOST_AUTO_TEST_CASE(value__tokens__zero_tokens__zero)
{
    const auto instance = make<bool>();
    instance->zero_tokens();
    BOOST_REQUIRE_EQUAL(instance->min_tokens(), 0u);
    BOOST_REQUIRE_EQUAL(instance->max_tokens(), 0u);
}

BOOST_AUTO_TEST_CASE(value__tokens__implicit__zero_to_one)
{
    const auto instance = make<bool>();
    instance->implicit_value(true);
    BOOST_REQUIRE_EQUAL(instance->min_tokens(), 0u);
    BOOST_REQUIRE_EQUAL(instance->max_tokens(), 1u);
}

BOOST_AUTO_TEST_CASE(value__tokens__multitoken__unlimited)
{
    const auto instance = make<string_list>();
    instance->multitoken();
    BOOST_REQUIRE_EQUAL(instance->min_tokens(), 1u);
    BOOST_REQUIRE_EQUAL(instance->max_tokens(), max_size_t);
}

BOOST_AUTO_TEST_CASE(value__flags__default__false)
{
    const auto instance = make<int32_t>();
    BOOST_REQUIRE(!instance->is_composing());
    BOOST_REQUIRE(!instance->is_required());
}

BOOST_AUTO_TEST_CASE(value__flags__set__true)
{
    const auto instance = make<int32_t>();
    instance->composing()->required();
    BOOST_REQUIRE(instance->is_composing());
    BOOST_REQUIRE(instance->is_required());
}

// name

BOOST_AUTO_TEST_CASE(value__name__undefaulted__arg)
{
    BOOST_REQUIRE_EQUAL(make<int32_t>()->name(), "arg");
}

BOOST_AUTO_TEST_CASE(value__name__defaulted__default_text)
{
    const auto instance = make<int32_t>();
    instance->default_value(42);
    BOOST_REQUIRE_EQUAL(instance->name(), "arg (=42)");
}

BOOST_AUTO_TEST_CASE(value__name__defaulted_byte__decimal_text)
{
    const auto instance = make<uint8_t>();
    instance->default_value(uint8_t{ 42 });
    BOOST_REQUIRE_EQUAL(instance->name(), "arg (=42)");
}

BOOST_AUTO_TEST_CASE(value__name__defaulted_text__given_text)
{
    const auto instance = make<int32_t>();
    instance->default_value(42, "answer");
    BOOST_REQUIRE_EQUAL(instance->name(), "arg (=answer)");
}

BOOST_AUTO_TEST_CASE(value__name__defaulted_empty_text__arg)
{
    const auto instance = make<std::string>();
    instance->default_value("");
    BOOST_REQUIRE_EQUAL(instance->name(), "arg");
}

BOOST_AUTO_TEST_CASE(value__name__implicit_defaulted__both)
{
    const auto instance = make<bool>();
    instance->implicit_value(true)->default_value(false);
    BOOST_REQUIRE_EQUAL(instance->name(), "[=arg(=1)] (=0)");
}

// apply_default

BOOST_AUTO_TEST_CASE(value__apply_default__undefaulted__false_empty)
{
    std::any out{};
    BOOST_REQUIRE(!make<int32_t>()->apply_default(out));
    BOOST_REQUIRE(!out.has_value());
}

BOOST_AUTO_TEST_CASE(value__apply_default__defaulted__true_default)
{
    std::any out{};
    const auto instance = make<int32_t>();
    instance->default_value(42);
    BOOST_REQUIRE(instance->apply_default(out));
    BOOST_REQUIRE_EQUAL(std::any_cast<int32_t>(out), 42);
}

// notify

BOOST_AUTO_TEST_CASE(value__notify__store__assigned)
{
    int32_t store{};
    make<int32_t>(&store)->notify(std::any{ 42 });
    BOOST_REQUIRE_EQUAL(store, 42);
}

BOOST_AUTO_TEST_CASE(value__notify__notifier__invoked)
{
    int32_t notified{};
    const auto instance = make<int32_t>();
    instance->notifier([&](const int32_t& value) { notified = value; });
    instance->notify(std::any{ 42 });
    BOOST_REQUIRE_EQUAL(notified, 42);
}

BOOST_AUTO_TEST_CASE(value__notify__empty__unassigned)
{
    int32_t store{ 7 };
    make<int32_t>(&store)->notify(std::any{});
    BOOST_REQUIRE_EQUAL(store, 7);
}

// parse

BOOST_AUTO_TEST_CASE(value__parse__string__whole_token)
{
    BOOST_REQUIRE_EQUAL(parse_one<std::string>(" a b "), " a b ");
    BOOST_REQUIRE_EQUAL(parse_one<std::string>(""), "");
}

BOOST_AUTO_TEST_CASE(value__parse__path__whole_token)
{
    BOOST_REQUIRE_EQUAL(parse_one<std::filesystem::path>("a b/c"), std::filesystem::path{ "a b/c" });
}

BOOST_AUTO_TEST_CASE(value__parse__bool_true__true)
{
    BOOST_REQUIRE(parse_one<bool>("true"));
    BOOST_REQUIRE(parse_one<bool>("TRUE"));
    BOOST_REQUIRE(parse_one<bool>("yes"));
    BOOST_REQUIRE(parse_one<bool>("on"));
    BOOST_REQUIRE(parse_one<bool>("1"));
    BOOST_REQUIRE(parse_one<bool>(""));
}

BOOST_AUTO_TEST_CASE(value__parse__bool_false__false)
{
    BOOST_REQUIRE(!parse_one<bool>("false"));
    BOOST_REQUIRE(!parse_one<bool>("False"));
    BOOST_REQUIRE(!parse_one<bool>("no"));
    BOOST_REQUIRE(!parse_one<bool>("off"));
    BOOST_REQUIRE(!parse_one<bool>("0"));
}

BOOST_AUTO_TEST_CASE(value__parse__bool_invalid__throws)
{
    BOOST_REQUIRE_EXCEPTION(parse_one<bool>("2"), istream_exception, [](const istream_exception& ex) { return std::string{ ex.what() } == "the argument ('2') for option 'name' is invalid. Valid choices are 'on|off', 'yes|no', '1|0' and 'true|false'"; });
    BOOST_REQUIRE_THROW(parse_one<bool>("truth"), istream_exception);
}

BOOST_AUTO_TEST_CASE(value__parse__bool_no_tokens__true)
{
    std::any out{};
    make<bool>()->parse(out, {}, "name");
    BOOST_REQUIRE(std::any_cast<bool>(out));
}

BOOST_AUTO_TEST_CASE(value__parse__integers__expected)
{
    BOOST_REQUIRE_EQUAL(parse_one<int32_t>("-42"), -42);
    BOOST_REQUIRE_EQUAL(parse_one<uint32_t>("4294967295"), max_uint32);
    BOOST_REQUIRE_EQUAL(parse_one<int64_t>("-9223372036854775808"), min_int64);
    BOOST_REQUIRE_EQUAL(parse_one<uint64_t>("18446744073709551615"), max_uint64);
    BOOST_REQUIRE_EQUAL(parse_one<int32_t>("+42"), 42);
    BOOST_REQUIRE_EQUAL(parse_one<uint32_t>("+42"), 42u);
}

BOOST_AUTO_TEST_CASE(value__parse__bytes__numbers)
{
    BOOST_REQUIRE_EQUAL(parse_one<uint8_t>("255"), 255u);
    BOOST_REQUIRE_EQUAL(parse_one<int8_t>("-128"), -128);
}

BOOST_AUTO_TEST_CASE(value__parse__integer_invalid__throws)
{
    BOOST_REQUIRE_THROW(parse_one<uint32_t>("-1"), istream_exception);
    BOOST_REQUIRE_THROW(parse_one<int32_t>("+-1"), istream_exception);
    BOOST_REQUIRE_THROW(parse_one<int32_t>("+"), istream_exception);
    BOOST_REQUIRE_THROW(parse_one<int32_t>(" 1"), istream_exception);
    BOOST_REQUIRE_THROW(parse_one<uint32_t>("4294967296"), istream_exception);
    BOOST_REQUIRE_THROW(parse_one<uint32_t>("42x"), istream_exception);
    BOOST_REQUIRE_THROW(parse_one<uint32_t>(""), istream_exception);
    BOOST_REQUIRE_THROW(parse_one<uint8_t>("256"), istream_exception);
    BOOST_REQUIRE_THROW(parse_one<int8_t>("-129"), istream_exception);
}

BOOST_AUTO_TEST_CASE(value__parse__double__expected)
{
    BOOST_REQUIRE_EQUAL(parse_one<double>("0.000001"), 0.000001);
    BOOST_REQUIRE_EQUAL(parse_one<double>("-2.5"), -2.5);
}

BOOST_AUTO_TEST_CASE(value__parse__double_invalid__throws)
{
    BOOST_REQUIRE_THROW(parse_one<double>("2.5x"), istream_exception);
    BOOST_REQUIRE_THROW(parse_one<double>(""), istream_exception);
}

BOOST_AUTO_TEST_CASE(value__parse__constructible__constructed)
{
    BOOST_REQUIRE_EQUAL(parse_one<config::hash256>(encode_hash(null_hash)).to_string(), encode_hash(null_hash));
}

BOOST_AUTO_TEST_CASE(value__parse__constructible_invalid__throws_named)
{
    BOOST_REQUIRE_EXCEPTION(parse_one<config::hash256>("bogus"), istream_exception, [](const istream_exception& ex) { return ex.token() == "bogus" && std::string{ ex.what() } == "the argument ('bogus') for option 'name' is invalid"; });
}

BOOST_AUTO_TEST_CASE(value__parse__extractable_invalid__throws)
{
    BOOST_REQUIRE_THROW(parse_one<wallet::ec_public>("bogus"), istream_exception);
}

BOOST_AUTO_TEST_CASE(value__parse__extractable_spaces__whole_token)
{
    BOOST_REQUIRE(parse_one<config::path>("a b/c") == config::path{ "a b/c" });
}

BOOST_AUTO_TEST_CASE(value__parse__vector__appended)
{
    std::any out{};
    const auto instance = make<std::vector<uint32_t>>();
    instance->parse(out, { "1", "2" }, "name");
    instance->parse(out, { "3" }, "name");
    BOOST_REQUIRE_EQUAL(std::any_cast<std::vector<uint32_t>>(out), (std::vector<uint32_t>{ 1, 2, 3 }));
}

BOOST_AUTO_TEST_CASE(value__parse__vector_invalid__throws)
{
    std::any out{};
    BOOST_REQUIRE_THROW(make<std::vector<uint32_t>>()->parse(out, { "1", "x" }, "name"), istream_exception);
}

BOOST_AUTO_TEST_CASE(value__parse__repeated_scalar__throws)
{
    std::any out{};
    const auto instance = make<uint32_t>();
    instance->parse(out, { "1" }, "name");
    BOOST_REQUIRE_THROW(instance->parse(out, { "2" }, "name"), validation_exception);
}

BOOST_AUTO_TEST_CASE(value__parse__multiple_tokens_scalar__throws)
{
    std::any out{};
    BOOST_REQUIRE_THROW(make<uint32_t>()->parse(out, { "1", "2" }, "name"), validation_exception);
}

BOOST_AUTO_TEST_CASE(value__parse__no_tokens_scalar__throws)
{
    std::any out{};
    BOOST_REQUIRE_THROW(make<uint32_t>()->parse(out, {}, "name"), validation_exception);
}

BOOST_AUTO_TEST_CASE(value__parse__no_tokens_implicit__implicit)
{
    std::any out{};
    const auto instance = make<uint32_t>();
    instance->implicit_value(42);
    instance->parse(out, {}, "name");
    BOOST_REQUIRE_EQUAL(std::any_cast<uint32_t>(out), 42u);
}

BOOST_AUTO_TEST_SUITE_END()
