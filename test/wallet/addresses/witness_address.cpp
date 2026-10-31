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
#include "../../test.hpp"

#include <sstream>

BC_AUTO_TEST_SUITE(witness_address_tests)

using namespace chain;
using namespace wallet;

using result = witness_address::parse_result;
using type = witness_address::program_type;

std::string out1;
uint8_t out2;
data_chunk out3;

// construct

BC_AUTO_TEST_CASE(witness_address__default_construct__always__invalid)
{
    BC_REQUIRE(!witness_address());
}

BC_AUTO_TEST_CASE(witness_address__move_construct__null_hash__valid)
{
    auto address = witness_address(null_hash);
    BC_REQUIRE(witness_address(std::move(address)));
}

BC_AUTO_TEST_CASE(witness_address__copy_construct__null_hash__valid)
{
    const auto address = witness_address(null_hash);
    BC_REQUIRE(witness_address(address));
}

BC_AUTO_TEST_CASE(witness_address__string_construct__empty__invalid)
{
    BC_REQUIRE(!witness_address(""));
}

BC_AUTO_TEST_CASE(witness_address__string_construct__invalid_version0__invalid)
{
    BC_REQUIRE(!witness_address("split1checkupstagehandshakeupstreamerranterredcaperred2y9e3w"));
}

BC_AUTO_TEST_CASE(witness_address__string_construct__invalid_version0_lax__invalid)
{
    BC_REQUIRE(!witness_address("split1checkupstagehandshakeupstreamerranterredcaperred2y9e3w", false));
}

BC_AUTO_TEST_CASE(witness_address__string_construct__valid_version0_strict__valid)
{
    BC_REQUIRE(witness_address("tb1qrp33g0q5c5txsp9arysrx4k6zdkfs4nce4xj0gdcccefvpysxf3q0sl5k7"));
}

BC_AUTO_TEST_CASE(witness_address__parameter_construct__minimal_program_abc_17__invalid)
{
    BC_REQUIRE(!witness_address(data_chunk(2, 0x00), 17, "abc"));
}

BC_AUTO_TEST_CASE(witness_address__parameter_construct__minimal_program_abc_16__valid)
{
    BC_REQUIRE(witness_address(data_chunk(2, 0x00), 16, "abc"));
}

BC_AUTO_TEST_CASE(witness_address__parameter_construct__maximal_program_abc_16__valid)
{
    BC_REQUIRE(witness_address(data_chunk(40, 0x00), 16, "abc"));
}

BC_AUTO_TEST_CASE(witness_address__parameter_construct__oversized_program_abc_16__invalid)
{
    BC_REQUIRE(!witness_address(data_chunk(40 + 1, 0x00), 16, "abc"));
}

BC_AUTO_TEST_CASE(witness_address__parameter_construct__minimal_program_abc_2__valid)
{
    BC_REQUIRE(witness_address(data_chunk(2, 0x00), 2, "abc"));
}

BC_AUTO_TEST_CASE(witness_address__parameter_construct__minimal_program_abc_1__invalid)
{
    BC_REQUIRE(!witness_address(data_chunk(2, 0x00), 1, "abc"));
}

BC_AUTO_TEST_CASE(witness_address__parameter_construct__minimal_program_abc_0__invalid)
{
    BC_REQUIRE(!witness_address(data_chunk(2, 0x00), 0, "abc"));
}

BC_AUTO_TEST_CASE(witness_address__parameter_construct__null_hash_abc_1__valid)
{
    BC_REQUIRE(witness_address(null_hash, 1, "abc"));
}

BC_AUTO_TEST_CASE(witness_address__parameter_construct__null_short_hash_abc_0__valid)
{
    BC_REQUIRE(witness_address(null_short_hash, 0, "abc"));
}

BC_AUTO_TEST_CASE(witness_address__parameter_construct__null_short_hash_invalid_prefix_0__invalid)
{
    BC_REQUIRE(!witness_address(null_short_hash, 0, "\x80"));
}

BC_AUTO_TEST_CASE(witness_address__short_hash_construct__null_short_hash__valid)
{
    BC_REQUIRE(witness_address(null_short_hash));
}

BC_AUTO_TEST_CASE(witness_address__parameter_construct__null_hash_abc_0__valid)
{
    BC_REQUIRE(witness_address(null_hash, 0, "abc"));
}

BC_AUTO_TEST_CASE(witness_address__parameter_construct__null_hash_invalid_prefix_0__invalid)
{
    BC_REQUIRE(!witness_address(null_hash, 0, "\x80"));
}

BC_AUTO_TEST_CASE(witness_address__parameter_construct__null_hash__valid)
{
    BC_REQUIRE(witness_address(null_hash));
}

BC_AUTO_TEST_CASE(witness_address__ec_private_construct__uninitialized__invalid)
{
    BC_REQUIRE(!witness_address(ec_private()));
}

BC_AUTO_TEST_CASE(witness_address__ec_private_construct__initialized__valid)
{
    BC_REQUIRE(witness_address(ec_private("L1WepftUBemj6H4XQovkiW1ARVjxMqaw4oj2kmkYqdG1xTnBcHfC")));
}

BC_AUTO_TEST_CASE(witness_address__ec_public_construct__uninitialized__invalid)
{
    BC_REQUIRE(!witness_address(ec_public()));
}

BC_AUTO_TEST_CASE(witness_address__ec_public_construct__initialized__valid)
{
    BC_REQUIRE(witness_address(ec_public(ec_private("L1WepftUBemj6H4XQovkiW1ARVjxMqaw4oj2kmkYqdG1xTnBcHfC"))));
}

BC_AUTO_TEST_CASE(witness_address__hash_digest_construct__null_hash__valid)
{
    BC_REQUIRE(witness_address(null_hash));
}

BC_AUTO_TEST_CASE(witness_address__script_construct__invalid__invalid)
{
    const chain::script script(data_chunk{ 0x01 }, true);
    BC_REQUIRE(!witness_address(script));
}

BC_AUTO_TEST_CASE(witness_address__script_construct__initialized__valid)
{
    BC_REQUIRE(witness_address(script(script::to_pay_null_data_pattern(null_hash))));
}

// parse_prefix

BC_AUTO_TEST_CASE(witness_address__parse_prefix__not_ascii__prefix_not_ascii)
{
    BC_REQUIRE(witness_address::parse_prefix("\x80") == result::prefix_not_ascii);
    BC_REQUIRE(witness_address::parse_prefix("\xff") == result::prefix_not_ascii);
}

BC_AUTO_TEST_CASE(witness_address__parse_prefix__upper_case__prefix_not_lower_case)
{
    BC_REQUIRE(witness_address::parse_prefix("A") == result::prefix_not_lower_case);
    BC_REQUIRE(witness_address::parse_prefix("aBc") == result::prefix_not_lower_case);
}

BC_AUTO_TEST_CASE(witness_address__parse_prefix__empty__prefix_too_short)
{
    BC_REQUIRE(witness_address::parse_prefix("") == result::prefix_too_short);
    BC_REQUIRE(witness_address::parse_prefix("\x00") == result::prefix_too_short);
}

BC_AUTO_TEST_CASE(witness_address__parse_prefix__upper_case__prefix_too_long)
{
    const std::string prefix((90 - 1 - 1 - 4 - 6) + 1, 'a');
    BC_REQUIRE(witness_address::parse_prefix(prefix) == result::prefix_too_long);
}

BC_AUTO_TEST_CASE(witness_address__parse_prefix__invalid_ascii__prefix_invalid_character)
{
    // Only ascii characters will be captured here [0x00-0x20, 0x7f].
    BC_REQUIRE(witness_address::parse_prefix({ 0x00 }) == result::prefix_invalid_character);
    BC_REQUIRE(witness_address::parse_prefix("\x20") == result::prefix_invalid_character);
    BC_REQUIRE(witness_address::parse_prefix("\x7f") == result::prefix_invalid_character);
    BC_REQUIRE(witness_address::parse_prefix("z\x7fz") == result::prefix_invalid_character);
}

BC_AUTO_TEST_CASE(witness_address__parse_prefix__valid_characters_and_lengths__valid)
{
    const std::string prefix((90 - 1 - 1 - 4 - 6), 'a');
    BC_REQUIRE(witness_address::parse_prefix(prefix) == result::valid);
    BC_REQUIRE(witness_address::parse_prefix("\x21") == result::valid);
    BC_REQUIRE(witness_address::parse_prefix("\x7e") == result::valid);
    BC_REQUIRE(witness_address::parse_prefix("xyz") == result::valid);
}

BC_AUTO_TEST_CASE(witness_address__parse_prefix__known__valid)
{
    BC_REQUIRE(witness_address::parse_prefix("bc") == result::valid);
    BC_REQUIRE(witness_address::parse_prefix("tb") == result::valid);
    BC_REQUIRE(witness_address::parse_prefix("bc", true) == result::valid);
    BC_REQUIRE(witness_address::parse_prefix("tb", true) == result::valid);
}

BC_AUTO_TEST_CASE(witness_address__parse_prefix__unknown_strict__prefix_unknown)
{
    BC_REQUIRE(witness_address::parse_prefix("split") == result::valid);
    BC_REQUIRE(witness_address::parse_prefix("split", true) == result::prefix_unknown);
}

// parse_address

BC_AUTO_TEST_CASE(witness_address__parse_address__not_ascii__address_not_ascii)
{
    BC_REQUIRE(witness_address::parse_address(out1, out2, out3, "\x80") == result::address_not_ascii);
    BC_REQUIRE(witness_address::parse_address(out1, out2, out3, "\xff") == result::address_not_ascii);
}

BC_AUTO_TEST_CASE(witness_address__parse_address__mixed_case__address_mixed_case)
{
    BC_REQUIRE(witness_address::parse_address(out1, out2, out3, "aBc") == result::address_mixed_case);
}

BC_AUTO_TEST_CASE(witness_address__parse_address__too_long__address_too_long)
{
    const std::string address(90 + 1, 'a');
    BC_REQUIRE(witness_address::parse_address(out1, out2, out3, address) == result::address_too_long);
}

BC_AUTO_TEST_CASE(witness_address__parse_address__no_separator__prefix_missing)
{
    BC_REQUIRE(witness_address::parse_address(out1, out2, out3, "abc") == result::prefix_missing);
    BC_REQUIRE(witness_address::parse_address(out1, out2, out3, "ABC") == result::prefix_missing);
}

BC_AUTO_TEST_CASE(witness_address__parse_address__separator__prefix_errors)
{
    const std::string prefix((90 - 1 - 1 - 4 - 6) + 1, 'a');
    BC_REQUIRE(witness_address::parse_address(out1, out2, out3, "1") == result::prefix_too_short);
    BC_REQUIRE(witness_address::parse_address(out1, out2, out3, prefix + "1") == result::prefix_too_long);
    BC_REQUIRE(witness_address::parse_address(out1, out2, out3, "\x7f" "1") == result::prefix_invalid_character);
}

BC_AUTO_TEST_CASE(witness_address__parse_address__short_payload__payload_too_short)
{
    const std::string payload((1 + 4 + 6) - 1, 'i');
    BC_REQUIRE(witness_address::parse_address(out1, out2, out3, "a1" + payload) == result::payload_too_short);
}

BC_AUTO_TEST_CASE(witness_address__parse_address__long_payload__payload_too_long)
{
    const std::string payload((1 + 64 + 6) + 1, 'a');
    BC_REQUIRE(witness_address::parse_address(out1, out2, out3, "a1" + payload) == result::payload_too_long);
}

BC_AUTO_TEST_CASE(witness_address__parse_address__invalid_payload__payload_not_base32)
{
    const std::string payload(1 + 4 + 6, 'i');
    BC_REQUIRE(witness_address::parse_address(out1, out2, out3, "a1" + payload) == result::payload_not_base32);
}

BC_AUTO_TEST_CASE(witness_address__parse_address__invalid_checksum__checksum_invalid)
{
    const std::string prefix{ "bc" };
    auto address = prefix + "1" + encode_base32b(bech32_build_checked(0, data_chunk(2, 0x00), prefix));
    *std::prev(address.end()) = 'x';
    BC_REQUIRE(witness_address::parse_address(out1, out2, out3, address) == result::checksum_invalid);
}

BC_AUTO_TEST_CASE(witness_address__parse_address__version17__version_invalid)
{
    const size_t version = 17;
    const data_chunk program(2, 0x00);
    const std::string prefix{ "bc" };
    const auto address = prefix + "1" + encode_base32b(bech32_build_checked(version, program, prefix));
    BC_REQUIRE_EQUAL(address, "bc13qqqqjggfzq");
    BC_REQUIRE(witness_address::parse_address(out1, out2, out3, address) == result::version_invalid);
}

BC_AUTO_TEST_CASE(witness_address__parse_address__version16_minimal_program_strict__program_invalid)
{
    const size_t version = 16;
    const data_chunk program(2, 0x00);
    const std::string prefix{ "bc" };
    const auto address = prefix + "1" + encode_base32b(bech32_build_checked(version, program, prefix));
    BC_REQUIRE_EQUAL(address, "bc1sqqqqkfw08p");
    BC_REQUIRE(witness_address::parse_address(out1, out2, out3, address, true) == result::program_invalid);
    BC_REQUIRE(witness_address::parse_address(out1, out2, out3, address, false) == result::valid);
    BC_REQUIRE_EQUAL(out1, prefix);
    BC_REQUIRE_EQUAL(out2, version);
    BC_REQUIRE_EQUAL(out3, program);
}

BC_AUTO_TEST_CASE(witness_address__parse_address__version0_empty_program__payload_too_short)
{
    const std::string prefix{ "bc" };
    const auto address = prefix + "1" + encode_base32b(bech32_build_checked(0, {}, prefix));
    BC_REQUIRE_EQUAL(address, "bc1q9zpgru");
    BC_REQUIRE(witness_address::parse_address(out1, out2, out3, address) == result::payload_too_short);
}

BC_AUTO_TEST_CASE(witness_address__parse_address__version0_minimal_program__program_invalid)
{
    const std::string prefix{ "bc" };
    const auto address = prefix + "1" + encode_base32b(bech32_build_checked(0, data_chunk(2, 0x00), prefix));
    BC_REQUIRE_EQUAL(address, "bc1qqqqq399cqn");
    BC_REQUIRE(witness_address::parse_address(out1, out2, out3, address) == result::program_invalid);
}

BC_AUTO_TEST_CASE(witness_address__parse_address__version0_null_short_hash_program__valid)
{
    const size_t version = 0;
    const data_chunk program = to_chunk(null_short_hash);
    const std::string prefix{ "bc" };
    const auto address = prefix + "1" + encode_base32b(bech32_build_checked(version, program, prefix));
    BC_REQUIRE_EQUAL(address, "bc1qqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqq9e75rs");
    BC_REQUIRE(witness_address::parse_address(out1, out2, out3, address) == result::valid);
    BC_REQUIRE_EQUAL(out1, prefix);
    BC_REQUIRE_EQUAL(out2, version);
    BC_REQUIRE_EQUAL(out3, program);
}

BC_AUTO_TEST_CASE(witness_address__parse_address__version0_null_hash_program__valid)
{
    const size_t version = 0;
    const data_chunk program = to_chunk(null_hash);
    const std::string prefix{ "bc" };
    const auto address = prefix + "1" + encode_base32b(bech32_build_checked(version, program, prefix));
    BC_REQUIRE_EQUAL(address, "bc1qqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqthqst8");
    BC_REQUIRE(witness_address::parse_address(out1, out2, out3, address) == result::valid);
    BC_REQUIRE_EQUAL(out1, prefix);
    BC_REQUIRE_EQUAL(out2, version);
    BC_REQUIRE_EQUAL(out3, program);
}

// parse_program

BC_AUTO_TEST_CASE(witness_address__parse_program__version0_short_hash_size__version0_p2kh)
{
    BC_REQUIRE(witness_address::parse_program(0, null_short_hash) == type::version0_p2kh);
}

BC_AUTO_TEST_CASE(witness_address__parse_program__version0_hash_size__version0_p2sh)
{
    BC_REQUIRE(witness_address::parse_program(0, null_hash) == type::version0_p2sh);
}

BC_AUTO_TEST_CASE(witness_address__parse_program__version16_short__invalid)
{
    BC_REQUIRE(witness_address::parse_program(16, data_chunk(1, 0x00)) == type::invalid);
}

BC_AUTO_TEST_CASE(witness_address__parse_program__version16_long__invalid)
{
    BC_REQUIRE(witness_address::parse_program(16, data_chunk(40 + 1, 0x00)) == type::invalid);
}

BC_AUTO_TEST_CASE(witness_address__parse_program__version_unknown_valid__unknown)
{
    BC_REQUIRE(witness_address::parse_program(2, null_hash) == type::unknown);
    BC_REQUIRE(witness_address::parse_program(16, null_hash) == type::unknown);
}

BC_AUTO_TEST_CASE(witness_address__parse_program__version_unknown_valid_strict__invalid)
{
    BC_REQUIRE(witness_address::parse_program(2, null_hash, true) == type::invalid);
    BC_REQUIRE(witness_address::parse_program(16, null_hash, true) == type::invalid);
}

BC_AUTO_TEST_CASE(witness_address__parse_program__version_invalid__invalid)
{
    BC_REQUIRE(witness_address::parse_program(17, null_hash) == type::invalid);
    BC_REQUIRE(witness_address::parse_program(255, null_hash) == type::invalid);
}

// cast bool

BC_AUTO_TEST_CASE(witness_address__bool__invalid__false)
{
    const witness_address instance{};
    BC_REQUIRE(!instance);
}

BC_AUTO_TEST_CASE(witness_address__bool__valid__true)
{
    const witness_address instance{ null_hash };
    BC_REQUIRE(instance);
}

// encoded

BC_AUTO_TEST_CASE(witness_address__encoded__empty__empty)
{
    const witness_address instance{};
    BC_REQUIRE(instance.encoded().empty());
}

BC_AUTO_TEST_CASE(witness_address__encoded__copied__expected)
{
    const auto address = "bc1sqqqqkfw08p";
    const witness_address instance{ address };
    BC_REQUIRE_EQUAL(witness_address(instance).encoded(), address);
}

BC_AUTO_TEST_CASE(witness_address__encoded__moved__expected)
{
    auto address = "bc1sqqqqkfw08p";
    const witness_address instance{ std::move(address) };
    BC_REQUIRE_EQUAL(witness_address(instance).encoded(), address);
}

BC_AUTO_TEST_CASE(witness_address__encoded__invalid_address__empty)
{
    const witness_address instance{ "split1bogus" };
    BC_REQUIRE(instance.encoded().empty());
}

BC_AUTO_TEST_CASE(witness_address__encoded__address__expected)
{
    const auto address = "bc1sqqqqkfw08p";
    const witness_address instance{ address };
    BC_REQUIRE_EQUAL(instance.encoded(), address);
}

BC_AUTO_TEST_CASE(witness_address__encoded__key_hash__expected)
{
    const witness_address instance{ null_hash };
    BC_REQUIRE_EQUAL(instance.encoded(), "bc1qqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqthqst8");
}

BC_AUTO_TEST_CASE(witness_address__encoded__key_hash_prefix__expected)
{
    const witness_address instance{ null_hash, "split" };
    BC_REQUIRE_EQUAL(instance.encoded(), "split1qqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqs8xlkk");
}

BC_AUTO_TEST_CASE(witness_address__encoded__invalid_ec_private__empty)
{
    const witness_address instance{ ec_private{} };
    BC_REQUIRE(instance.encoded().empty());
}

BC_AUTO_TEST_CASE(witness_address__encoded__ec_private__expected)
{
    const auto wif = "L1WepftUBemj6H4XQovkiW1ARVjxMqaw4oj2kmkYqdG1xTnBcHfC";
    const witness_address instance{ ec_private(wif) };
    BC_REQUIRE_EQUAL(instance.encoded(), "bc1qkn8up7xm8564fea4rzrn6t6pcv6mk8atnna7pw");
}

BC_AUTO_TEST_CASE(witness_address__encoded__ec_private_prefix__expected)
{
    const auto wif = "L1WepftUBemj6H4XQovkiW1ARVjxMqaw4oj2kmkYqdG1xTnBcHfC";
    const witness_address instance{ ec_private(wif), "split" };
    BC_REQUIRE_EQUAL(instance.encoded(), "split1qkn8up7xm8564fea4rzrn6t6pcv6mk8atkw7e5r");
}

BC_AUTO_TEST_CASE(witness_address__encoded__invalid_ec_public__empty)
{
    const witness_address instance{ ec_public{} };
    BC_REQUIRE(instance.encoded().empty());
}

BC_AUTO_TEST_CASE(witness_address__encoded__ec_public__expected)
{
    const auto wif = "L1WepftUBemj6H4XQovkiW1ARVjxMqaw4oj2kmkYqdG1xTnBcHfC";
    const witness_address instance{ ec_private(wif).to_public() };
    BC_REQUIRE_EQUAL(instance.encoded(), "bc1qkn8up7xm8564fea4rzrn6t6pcv6mk8atnna7pw");
}

BC_AUTO_TEST_CASE(witness_address__encoded__ec_public_prefix__expected)
{
    const auto wif = "L1WepftUBemj6H4XQovkiW1ARVjxMqaw4oj2kmkYqdG1xTnBcHfC";
    const witness_address instance{ ec_private(wif).to_public(), "split" };
    BC_REQUIRE_EQUAL(instance.encoded(), "split1qkn8up7xm8564fea4rzrn6t6pcv6mk8atkw7e5r");
}

BC_AUTO_TEST_CASE(witness_address__encoded__script_hash__expected)
{
    const auto script = chain::script(script::to_pay_null_data_pattern(null_hash));
    const auto script_hash = sha256_hash(script.to_data(false));
    const witness_address instance{ script_hash };
    BC_REQUIRE_EQUAL(instance.encoded(), "bc1qpxlkrchnvcz7exzzshh542797nzqx6g4vk5n4sn3m9sf64xlmarsp3t6g2");
}

BC_AUTO_TEST_CASE(witness_address__encoded__script_hash_prefix__expected)
{
    const auto script = chain::script(script::to_pay_null_data_pattern(null_hash));
    const auto script_hash = sha256_hash(script.to_data(false));
    const witness_address instance{ script_hash, "split" };
    BC_REQUIRE_EQUAL(instance.encoded(), "split1qpxlkrchnvcz7exzzshh542797nzqx6g4vk5n4sn3m9sf64xlmars6pd44m");
}

BC_AUTO_TEST_CASE(witness_address__encoded__invalid_script__empty)
{
    const chain::script script(data_chunk{ 0x01 }, true);
    BC_REQUIRE(!script.is_valid());

    const witness_address instance{ script };
    BC_REQUIRE(instance.encoded().empty());
}

BC_AUTO_TEST_CASE(witness_address__encoded__script__expected)
{
    const auto script = chain::script(script::to_pay_null_data_pattern(null_hash));
    const witness_address instance{ script };
    BC_REQUIRE_EQUAL(instance.encoded(), "bc1qpxlkrchnvcz7exzzshh542797nzqx6g4vk5n4sn3m9sf64xlmarsp3t6g2");
}

BC_AUTO_TEST_CASE(witness_address__encoded__script_prefix__expected)
{
    const auto script = chain::script(script::to_pay_null_data_pattern(null_hash));
    const witness_address instance{ script, "split" };
    BC_REQUIRE_EQUAL(instance.encoded(), "split1qpxlkrchnvcz7exzzshh542797nzqx6g4vk5n4sn3m9sf64xlmars6pd44m");
}

// prefix

BC_AUTO_TEST_CASE(witness_address__prefix__invalid__empty)
{
    const witness_address instance{};
    BC_REQUIRE(instance.prefix().empty());
}

BC_AUTO_TEST_CASE(witness_address__prefix__default__bc)
{
    const witness_address instance{ null_hash };
    BC_REQUIRE_EQUAL(instance.prefix(), "bc");
}

BC_AUTO_TEST_CASE(witness_address__prefix__explicit__expected)
{
    const witness_address instance{ null_hash, "split" };
    BC_REQUIRE_EQUAL(instance.prefix(), "split");
}

// version

BC_AUTO_TEST_CASE(witness_address__version__invalid__invalid)
{
    const witness_address instance{};
    BC_REQUIRE_EQUAL(instance.version(), max_uint8);
}

BC_AUTO_TEST_CASE(witness_address__version__17__invalid)
{
    const witness_address instance{ "bc13qqqqjggfzq" };
    BC_REQUIRE_EQUAL(instance.version(), max_uint8);
}

BC_AUTO_TEST_CASE(witness_address__version__16__16)
{
    const witness_address instance{ "bc1sqqqqkfw08p" };
    BC_REQUIRE_EQUAL(instance.version(), 16);
}

BC_AUTO_TEST_CASE(witness_address__version__16_strict__invalid)
{
    const witness_address instance{ "bc1sqqqqkfw08p", true };
    BC_REQUIRE_EQUAL(instance.version(), max_uint8);
}

BC_AUTO_TEST_CASE(witness_address__version__0_empty__invalid)
{
    const witness_address instance{ "bc1q9zpgru" };
    BC_REQUIRE_EQUAL(instance.version(), max_uint8);
}

BC_AUTO_TEST_CASE(witness_address__version__0__0)
{
    const witness_address instance{ "bc1qqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqq9e75rs" };
    BC_REQUIRE_EQUAL(instance.version(), 0);
}

// program

BC_AUTO_TEST_CASE(witness_address__program__invalid__empty)
{
    const witness_address instance{};
    BC_REQUIRE(instance.program().empty());
}

BC_AUTO_TEST_CASE(witness_address__program__null_short_hash__expected)
{
    const witness_address instance{ "bc1qqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqq9e75rs" };
    BC_REQUIRE_EQUAL(instance.program(), to_chunk(null_short_hash));
}

BC_AUTO_TEST_CASE(witness_address__program__null_hash_address__expected)
{
    const witness_address instance{ "bc1qqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqthqst8" };
    BC_REQUIRE_EQUAL(instance.program(), to_chunk(null_hash));
}

BC_AUTO_TEST_CASE(witness_address__program__script__expected)
{
    const auto script = chain::script(script::to_pay_null_data_pattern(null_hash));
    const auto script_hash = sha256_hash(script.to_data(false));
    const witness_address instance{ script };
    BC_REQUIRE_EQUAL(instance.program(), to_chunk(script_hash));
}

// identifier

BC_AUTO_TEST_CASE(witness_address__identifier__invalid__invalid)
{
    const witness_address instance{};
    BC_REQUIRE(instance.identifier() == type::invalid);
}

BC_AUTO_TEST_CASE(witness_address__identifier__version17_address__invalid)
{
    const witness_address instance{ "bc13qqqqjggfzq" };
    BC_REQUIRE(instance.identifier() == type::invalid);
}

BC_AUTO_TEST_CASE(witness_address__identifier__version16_address__unknown)
{
    const witness_address instance{ "bc1sqqqqkfw08p" };
    BC_REQUIRE(instance.identifier() == type::unknown);
}

BC_AUTO_TEST_CASE(witness_address__identifier__version0_minimal__invalid)
{
    const witness_address instance{ "bc1qqqqq399cqn" };
    BC_REQUIRE(instance.identifier() == type::invalid);
}

BC_AUTO_TEST_CASE(witness_address__identifier__key_hash__version0_p2kh)
{
    const witness_address instance{ null_short_hash };
    BC_REQUIRE(instance.identifier() == type::version0_p2kh);
}

BC_AUTO_TEST_CASE(witness_address__identifier__script_hash__version0_p2sh)
{
    const auto script = chain::script(script::to_pay_null_data_pattern(null_hash));
    const witness_address instance{ sha256_hash(script.to_data(false)) };
    BC_REQUIRE(instance.identifier() == type::version0_p2sh);
}

BC_AUTO_TEST_CASE(witness_address__identifier__script__version0_p2sh)
{
    const auto script = chain::script(script::to_pay_null_data_pattern(null_hash));
    const witness_address instance{ script };
    BC_REQUIRE(instance.identifier() == type::version0_p2sh);
}

// script

BC_AUTO_TEST_CASE(witness_address__script__version16_address__valid)
{
    const witness_address instance{ "bc1sqqqqkfw08p" };
    BC_REQUIRE(chain::script::is_pay_witness_pattern(instance.script().ops()));
}

BC_AUTO_TEST_CASE(witness_address__script__key_hash__pay_witness_key_hash)
{
    const witness_address instance{ null_short_hash };
    BC_REQUIRE(chain::script::is_pay_witness_key_hash_pattern(instance.script().ops()));
}

BC_AUTO_TEST_CASE(witness_address__script__script_hash__pay_witness_script_hash)
{
    const witness_address instance{ null_hash };
    BC_REQUIRE(chain::script::is_pay_witness_script_hash_pattern(instance.script().ops()));
}

BC_AUTO_TEST_CASE(witness_address__script__null_data_script__pay_witness_script_hash)
{
    const auto script = chain::script(script::to_pay_null_data_pattern(null_hash));
    const witness_address instance{ script };
    BC_REQUIRE(chain::script::is_pay_witness_script_hash_pattern(instance.script().ops()));
}

// assignment

BC_AUTO_TEST_CASE(witness_address__assignment__copied__expected)
{
    const auto address = "bc1qqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqq9e75rs";
    const witness_address instance1{ address };
    witness_address instance2;
    instance2 = instance1;
    BC_REQUIRE_EQUAL(instance2.encoded(), address);
}

BC_AUTO_TEST_CASE(witness_address__assignment__moved__expected)
{
    const auto address = "bc1qqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqq9e75rs";
    witness_address instance1{ address };
    witness_address instance2;
    instance2 = std::move(instance1);
    BC_REQUIRE_EQUAL(instance2.encoded(), address);
}

// operator<

BC_AUTO_TEST_CASE(witness_address__less_than_operator__self__false)
{
    const witness_address instance{ "bc1qqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqq9e75rs" };
    BC_REQUIRE(!(instance < instance));
}

BC_AUTO_TEST_CASE(witness_address__less_than_operator__equal__false)
{
    const witness_address instance1{ "bc1qqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqq9e75rs" };
    const witness_address instance2{ "bc1qqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqq9e75rs" };
    BC_REQUIRE(!(instance1 < instance2));
}

BC_AUTO_TEST_CASE(witness_address__less_than_operator__greater__false)
{
    const witness_address instance1{ "bc1qkn8up7xm8564fea4rzrn6t6pcv6mk8atnna7pw" };
    const witness_address instance2{ "bc1qqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqq9e75rs" };
    BC_REQUIRE(!(instance2 < instance1));
}

BC_AUTO_TEST_CASE(witness_address__less_than_operator__lesser__true)
{
    const witness_address instance1{ "bc1qkn8up7xm8564fea4rzrn6t6pcv6mk8atnna7pw" };
    const witness_address instance2{ "bc1qqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqq9e75rs" };
    BC_REQUIRE(instance1 < instance2);
}

// operator==

BC_AUTO_TEST_CASE(witness_address__equality__self__true)
{
    const witness_address instance{ "bc1qqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqq9e75rs" };
    BC_REQUIRE(instance == instance);
}

BC_AUTO_TEST_CASE(witness_address__equality__equal__true)
{
    const witness_address instance1{ "bc1qqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqq9e75rs" };
    const witness_address instance2{ "bc1qqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqq9e75rs" };
    BC_REQUIRE(instance1 == instance2);
}

BC_AUTO_TEST_CASE(witness_address__equality__distinct__false)
{
    const witness_address instance1{ "bc1qkn8up7xm8564fea4rzrn6t6pcv6mk8atnna7pw" };
    const witness_address instance2{ "bc1qqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqq9e75rs" };
    BC_REQUIRE(!(instance1 == instance2));
}

// operator!=

BC_AUTO_TEST_CASE(witness_address__inequality__self__false)
{
    const witness_address instance{ "bc1qqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqq9e75rs" };
    BC_REQUIRE(!(instance != instance));
}

BC_AUTO_TEST_CASE(witness_address__inequality__equal__false)
{
    const witness_address instance1{ "bc1qqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqq9e75rs" };
    const witness_address instance2{ "bc1qqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqq9e75rs" };
    BC_REQUIRE(!(instance1 != instance2));
}

BC_AUTO_TEST_CASE(witness_address__inequality__distinct__true)
{
    const witness_address instance1{ "bc1qkn8up7xm8564fea4rzrn6t6pcv6mk8atnna7pw" };
    const witness_address instance2{ "bc1qqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqq9e75rs" };
    BC_REQUIRE(instance1 != instance2);
}

// operator>>

BC_AUTO_TEST_CASE(witness_address__istream__invalid__throws_istream_exception)
{
    std::stringstream in{};
    witness_address instance{};
    BC_REQUIRE_THROW(in >> instance, istream_exception);
}

BC_AUTO_TEST_CASE(witness_address__istream__valid__expected)
{
    const auto address = "bc1qqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqq9e75rs";
    std::stringstream in{ address };
    witness_address instance{};
    in >> instance;
    BC_REQUIRE_EQUAL(instance.encoded(), address);
}

// operator<<

BC_AUTO_TEST_CASE(witness_address__ostream__invalid__empty)
{
    std::stringstream out;
    witness_address instance{};
    out << instance;
    BC_REQUIRE(out.str().empty());
}

BC_AUTO_TEST_CASE(witness_address__ostream__valid__expected)
{
    std::stringstream out;
    const auto address = "bc1qqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqq9e75rs";
    const witness_address instance{ address };
    out << instance;
    BC_REQUIRE_EQUAL(out.str(), address);
}

// BIP173 vectors
// See checksum and base_32b for related tests.

// These examples of valid "bech32" require an unversioned (and empty) payload, which
// requires exposure of bech32 internals and has no use case, so is not supported.
// The payload length requires five more characters to be a valid address.
// Note that the casing and character evaluations intended for testing here do in fact pass.
BC_AUTO_TEST_CASE(witness_address__bip173__unversioned_empty_bech32_vectors__payload_too_short)
{
    BC_REQUIRE(witness_address::parse_address(out1, out2, out3, "A12UEL5L") == result::payload_too_short);
    BC_REQUIRE(witness_address::parse_address(out1, out2, out3, "a12uel5l") == result::payload_too_short);
    BC_REQUIRE(witness_address::parse_address(out1, out2, out3, "?1ezyfcl") == result::payload_too_short);
}

// This example of valid "bech32" requires an unversioned (and empty) payload, which
// requires exposure of bech32 internals and has no use case, so is not supported.
// The payload length requires five more characters to be a valid address.
// Consequently the prefix length is restricted to one less byte, satisfying the overall length constraint.
BC_AUTO_TEST_CASE(witness_address__bip173__unversioned_empty_bech32_vectors__prefix_too_long)
{
    BC_REQUIRE(witness_address::parse_address(out1, out2, out3, "an83characterlonghumanreadablepartthatcontainsthenumber1andtheexcludedcharactersbio1tt5tgs") == result::prefix_too_long);
}

// This example of valid "bech32" can be interepreted as versioned or otherwise.
// The payload is greater than 71 characters, failing after successful parse of the maximum length tested here.
BC_AUTO_TEST_CASE(witness_address__bip173__unversioned_bech32_vectors__payload_too_long)
{
    BC_REQUIRE(witness_address::parse_address(out1, out2, out3, "11qqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqc8247j") == result::payload_too_long);
}

// This example of valid "bech32" can be interepreted as versioned or otherwise.
BC_AUTO_TEST_CASE(witness_address__bip173__unversioned_bech32_vectors__checksum_invalid)
{
    // Under bech32 this returns valid, but the non-zero version causes a checksum failure under bech32m.
    ////BC_REQUIRE(witness_address::parse_address(out1, out2, out3, "split1checkupstagehandshakeupstreamerranterredcaperred2y9e3w") == result::valid);
    BC_REQUIRE(witness_address::parse_address(out1, out2, out3, "split1checkupstagehandshakeupstreamerranterredcaperred2y9e3w") == result::checksum_invalid);
}

// HRP character out of range.
BC_AUTO_TEST_CASE(witness_address__bip173__invalid_bech32_prefix_character__invalid_expected)
{
    BC_REQUIRE(witness_address::parse_prefix("\x20") == result::prefix_invalid_character);
    BC_REQUIRE(witness_address::parse_prefix("\x7f") == result::prefix_invalid_character);
    BC_REQUIRE(witness_address::parse_prefix("\x80") == result::prefix_not_ascii);

    BC_REQUIRE(witness_address::parse_address(out1, out2, out3, "\x20" "1nwldj5") == result::prefix_invalid_character);
    BC_REQUIRE(witness_address::parse_address(out1, out2, out3, "\x7f" "1axkwrx") == result::prefix_invalid_character);
    BC_REQUIRE(witness_address::parse_address(out1, out2, out3, "\x80" "1eym55h") == result::address_not_ascii);
}
// Overall max length exceeded.
BC_AUTO_TEST_CASE(witness_address__bip173__invalid_bech32_overall_length__address_too_long)
{
    BC_REQUIRE(witness_address::parse_address(out1, out2, out3, "an84characterslonghumanreadablepartthatcontainsthenumber1andtheexcludedcharactersbio1569pvx") == result::address_too_long);
}

// No separator character.
BC_AUTO_TEST_CASE(witness_address__bip173__no_separator__prefix_missing)
{
    BC_REQUIRE(witness_address::parse_address(out1, out2, out3, "pzry9x0s0muk") == result::prefix_missing);
}

// Empty HRP.
BC_AUTO_TEST_CASE(witness_address__bip173__empty_prefix1__prefix_too_short)
{
    BC_REQUIRE(witness_address::parse_address(out1, out2, out3, "1pzry9x0s0muk") == result::prefix_too_short);
}

// Invalid data character.
BC_AUTO_TEST_CASE(witness_address__bip173__invalid_base32__payload_not_base32)
{
    // This example is too short to be a valid address test (requires 13 characters minimum).
    BC_REQUIRE(witness_address::parse_address(out1, out2, out3, "x1b4n0q5v") == result::payload_too_short);
    BC_REQUIRE(witness_address::parse_address(out1, out2, out3, "x1b4n00000q5v") == result::payload_not_base32);
}

// Too short checksum.
BC_AUTO_TEST_CASE(witness_address__bip173__short_checksum__payload_too_short)
{
    BC_REQUIRE(witness_address::parse_address(out1, out2, out3, "li1dgmt3") == result::payload_too_short);
}

// 0xFF: Invalid character in checksum.
BC_AUTO_TEST_CASE(witness_address__bip173__invalid_checksum__payload_not_base32)
{
    BC_REQUIRE(witness_address::parse_address(out1, out2, out3, "de1lg7wt" "\xff") == result::address_not_ascii);
}

// Checksum calculated with uppercase form of HRP.
// This example of valid "bech32" requires an unversioned (and empty) payload, which
// requires exposure of bech32 internals and has no use case, so is not supported.
// The payload length is limited by one more character in an implementation that requires a version.
BC_AUTO_TEST_CASE(witness_address__bip173__invalid_checksum__payload_too_short)
{
    BC_REQUIRE(witness_address::parse_address(out1, out2, out3, "A1G7SGD8") == result::payload_too_short);
}

// Empty HRP.
BC_AUTO_TEST_CASE(witness_address__bip173__empty_prefix2__prefix_too_short)
{
    BC_REQUIRE(witness_address::parse_address(out1, out2, out3, "10a06t8") == result::prefix_too_short);
}

// Empty HRP.
BC_AUTO_TEST_CASE(witness_address__bip173__empty_prefix3__prefix_too_short)
{
    BC_REQUIRE(witness_address::parse_address(out1, out2, out3, "1qzzfhee") == result::prefix_too_short);
}

// BIP173: The following list gives valid segwit addresses and the scriptPubKey that they translate to in hex.

BC_AUTO_TEST_CASE(witness_address__bip173__valid_address1__expected)
{
    const witness_address address("BC1QW508D6QEJXTDG4Y5R3ZARVARY0C5XW7KV8F3T4");
    BC_REQUIRE_EQUAL(address.version(), 0u);
    BC_REQUIRE(address.identifier() == type::version0_p2kh);
    BC_REQUIRE_EQUAL(address.program(), base16_chunk("751e76e8199196d454941c45d1b3a323f1433bd6"));
    BC_REQUIRE_EQUAL(address.script().to_string(chain::all_rules), "zero [751e76e8199196d454941c45d1b3a323f1433bd6]");
}

BC_AUTO_TEST_CASE(witness_address__bip173__valid_address2__expected)
{
    const witness_address address("tb1qrp33g0q5c5txsp9arysrx4k6zdkfs4nce4xj0gdcccefvpysxf3q0sl5k7");
    BC_REQUIRE_EQUAL(address.version(), 0u);
    BC_REQUIRE(address.identifier() == type::version0_p2sh);
    BC_REQUIRE_EQUAL(address.program(), base16_chunk("1863143c14c5166804bd19203356da136c985678cd4d27a1b8c6329604903262"));
    BC_REQUIRE_EQUAL(address.script().to_string(chain::all_rules), "zero [1863143c14c5166804bd19203356da136c985678cd4d27a1b8c6329604903262]");
}

// This test became invalid under taproot, given that the program size is not 32 bytes, so the version was increased from 1 to 2.
BC_AUTO_TEST_CASE(witness_address__bip173__valid_address3__expected)
{
    // This returns valid under bech32 and checksum_invalid under bech32m.
    ////const witness_address address("bc1pw508d6qejxtdg4y5r3zarvary0c5xw7kw508d6qejxtdg4y5r3zarvary0c5xw7k7grplx");
    const auto program = base16_array("751e76e8199196d454941c45d1b3a323f1433bd6751e76e8199196d454941c45d1b3a323f1433bd6");
    const witness_address address(program, 2, "bc");
    ////BC_REQUIRE_EQUAL(address.encoded(), "bc1pw508d6qejxtdg4y5r3zarvary0c5xw7kw508d6qejxtdg4y5r3zarvary0c5xw7kt5nd6y");
    BC_REQUIRE_EQUAL(address.encoded(), "bc1zw508d6qejxtdg4y5r3zarvary0c5xw7kw508d6qejxtdg4y5r3zarvary0c5xw7k6smc24");
    BC_REQUIRE_EQUAL(address.version(), 2u);
    BC_REQUIRE(address.identifier() == type::unknown);
    BC_REQUIRE_EQUAL(address.program(), to_chunk(program));
    BC_REQUIRE_EQUAL(address.script().to_string(chain::all_rules), "2 [751e76e8199196d454941c45d1b3a323f1433bd6751e76e8199196d454941c45d1b3a323f1433bd6]");
}

BC_AUTO_TEST_CASE(witness_address__bip173__valid_address4__expected)
{
    // This returns valid under bech32 and checksum_invalid under bech32m.
    ////const witness_address address("BC1SW50QA3JX3S");
    const auto program = base16_array("751e");
    const witness_address address(program, 16, "bc");
    BC_REQUIRE_EQUAL(ascii_to_upper(address.encoded()), "BC1SW50QGDZ25J");
    BC_REQUIRE_EQUAL(address.version(), 16u);
    BC_REQUIRE(address.identifier() == type::unknown);
    BC_REQUIRE_EQUAL(address.program(), to_chunk(program));
    BC_REQUIRE_EQUAL(address.script().to_string(chain::all_rules), "16 [751e]");
}

BC_AUTO_TEST_CASE(witness_address__bip173__valid_address5__expected)
{
    // This returns valid under bech32 and checksum_invalid under bech32m.
    ////const witness_address address("bc1zw508d6qejxtdg4y5r3zarvaryvg6kdaj");
    const auto program = base16_array("751e76e8199196d454941c45d1b3a323");
    const witness_address address(program, 2, "bc");
    BC_REQUIRE_EQUAL(address.encoded(), "bc1zw508d6qejxtdg4y5r3zarvaryvaxxpcs");
    BC_REQUIRE_EQUAL(address.version(), 2u);
    BC_REQUIRE(address.identifier() == type::unknown);
    BC_REQUIRE_EQUAL(address.program(), to_chunk(program));
    BC_REQUIRE_EQUAL(address.script().to_string(chain::all_rules), "2 [751e76e8199196d454941c45d1b3a323]");
}

BC_AUTO_TEST_CASE(witness_address__bip173__valid_address6__expected)
{
    // Bech32m/Bech32 same (version 0).
    const witness_address address("tb1qqqqqp399et2xygdj5xreqhjjvcmzhxw4aywxecjdzew6hylgvsesrxh6hy");
    BC_REQUIRE_EQUAL(address.version(), 0u);
    BC_REQUIRE(address.identifier() == type::version0_p2sh);
    BC_REQUIRE_EQUAL(address.program(), base16_chunk("000000c4a5cad46221b2a187905e5266362b99d5e91c6ce24d165dab93e86433"));
    BC_REQUIRE_EQUAL(address.script().to_string(chain::all_rules), "zero [000000c4a5cad46221b2a187905e5266362b99d5e91c6ce24d165dab93e86433]");
}

// BIP173: The following list gives invalid segwit addresses and the reason for their invalidity.

// Invalid human-readable part.
BC_AUTO_TEST_CASE(witness_address__bip173__invalid_address1__prefix_unknown)
{
    // Strict parsing changes from valid (prefix_unknown) to prefix_unknown.
    BC_REQUIRE(witness_address::parse_address(out1, out2, out3, "tc1qw508d6qejxtdg4y5r3zarvary0c5xw7kg3g4ty") == result::valid);
    BC_REQUIRE(witness_address::parse_address(out1, out2, out3, "tc1qw508d6qejxtdg4y5r3zarvary0c5xw7kg3g4ty", true) == result::prefix_unknown);
}

// Invalid checksum.
BC_AUTO_TEST_CASE(witness_address__bip173__invalid_address2__checksum_invalid)
{
    BC_REQUIRE(witness_address::parse_address(out1, out2, out3, "bc1qw508d6qejxtdg4y5r3zarvary0c5xw7kv8f3t5") == result::checksum_invalid);
}

// Invalid witness version.
BC_AUTO_TEST_CASE(witness_address__bip173__invalid_address3__checksum_invalid)
{
    // This returns version_invalid under bech32 and checksum_invalid under bech32m.
    ////BC_REQUIRE(witness_address::parse_address(out1, out2, out3, "BC13W508D6QEJXTDG4Y5R3ZARVARY0C5XW7KN40WF2") == result::version_invalid);
    BC_REQUIRE(witness_address::parse_address(out1, out2, out3, "BC13W508D6QEJXTDG4Y5R3ZARVARY0C5XW7KN40WF2") == result::checksum_invalid);
}

// Invalid program length.
BC_AUTO_TEST_CASE(witness_address__bip173__invalid_address4__payload_too_short)
{
    BC_REQUIRE(witness_address::parse_address(out1, out2, out3, "bc1rw5uspcuh") == result::payload_too_short);
}

// Invalid program length.
BC_AUTO_TEST_CASE(witness_address__bip173__invalid_address5__payload_too_long)
{
    BC_REQUIRE(witness_address::parse_address(out1, out2, out3, "bc10w508d6qejxtdg4y5r3zarvary0c5xw7kw508d6qejxtdg4y5r3zarvary0c5xw7kw5rljs90") == result::payload_too_long);
}

// Invalid program length for witness version 0.
BC_AUTO_TEST_CASE(witness_address__bip173__invalid_address6__program_invalid)
{
    // This is a zero version, so lax parsing has no impact.
    BC_REQUIRE(witness_address::parse_address(out1, out2, out3, "BC1QR508D6QEJXTDG4Y5R3ZARVARYV98GJ9P") == result::program_invalid);
    BC_REQUIRE(witness_address::parse_address(out1, out2, out3, "BC1QR508D6QEJXTDG4Y5R3ZARVARYV98GJ9P", true) == result::program_invalid);
}

// Mixed case.
BC_AUTO_TEST_CASE(witness_address__bip173__invalid_address7__address_mixed_case)
{
    BC_REQUIRE(witness_address::parse_address(out1, out2, out3, "tb1qrp33g0q5c5txsp9arysrx4k6zdkfs4nce4xj0gdcccefvpysxf3q0sL5k7") == result::address_mixed_case);
}

// Zero padding of more than 4 bits.
BC_AUTO_TEST_CASE(witness_address__bip173__invalid_address8__checksum_invalid)
{
    // This returns valid/program_invalid under bech32 and checksum_invalid under bech32m.
    ////BC_REQUIRE(witness_address::parse_address(out1, out2, out3, "bc1zw508d6qejxtdg4y5r3zarvaryvqyzf3du") == result::valid);
    ////BC_REQUIRE(witness_address::parse_address(out1, out2, out3, "bc1zw508d6qejxtdg4y5r3zarvaryvqyzf3du", true) == result::program_invalid);
    BC_REQUIRE(witness_address::parse_address(out1, out2, out3, "bc1zw508d6qejxtdg4y5r3zarvaryvqyzf3du") == result::checksum_invalid);
}

// Non-zero padding in 8-to-5 conversion.
BC_AUTO_TEST_CASE(witness_address__bip173__invalid_address9__program_invalid)
{
    // Non-zero padding changes the encoded representation, and would be ignored if not explicitly checked for.
    BC_REQUIRE(witness_address::parse_address(out1, out2, out3, "tb1qrp33g0q5c5txsp9arysrx4k6zdkfs4nce4xj0gdcccefvpysxf3pjxtptv") == result::program_invalid);
}

// Empty data section.
BC_AUTO_TEST_CASE(witness_address__bip173__invalid_address10__payload_too_short)
{
    BC_REQUIRE(witness_address::parse_address(out1, out2, out3, "bc1gmk9yu") == result::payload_too_short);
}


BC_AUTO_TEST_CASE(witness_address__script__invalid__empty)
{
    BC_REQUIRE(witness_address{}.script().ops().empty());
}

BC_AUTO_TEST_SUITE_END()
