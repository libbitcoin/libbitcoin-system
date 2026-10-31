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
#include <tuple>

BC_AUTO_TEST_SUITE(checked_tests)

using namespace bc::system::wallet;

typedef checked<0, 0, 0> zero_checked;
typedef checked<1, mini_hash_size, 2> mini_checked;
typedef checked<3, short_hash_size, 4> short_checked;
typedef checked<5, long_hash_size, 8> long_checked;

////const data_array<0> zero_prefix{};
const data_array<1> mini_prefix{ 1 };
const data_array<3> short_prefix{ 1, 2, 3 };
const data_array<5> long_prefix{ 1, 2, 3, 4, 5 };

////const data_array<0> test_zero_hash = base16_hash("");
const mini_hash test_mini_hash = base16_hash("000102030405");
const short_hash test_short_hash = base16_hash("0908070605040302010009080706050403020100");
const long_hash test_long_hash = base16_hash("09080706050403020100090807060504030201000908070605040302010009080706050403020100090807060504030201000908070605040302010003020100");

///const auto zero_valid_value = insert_checksum<zero_checked::value_size, 0>({ zero_prefix,  test_zero_hash });

auto mini_valid_value() NOEXCEPT
{
    static const auto value = insert_checksum<mini_checked::value_size, 2>({ mini_prefix,  test_mini_hash });
    return value;
}

auto short_valid_value() NOEXCEPT
{
    static const auto value = insert_checksum<short_checked::value_size, 4>({ short_prefix, test_short_hash });
    return value;
}

auto long_valid_value() NOEXCEPT
{
    static const auto value = insert_checksum<long_checked::value_size, 8>({ long_prefix,  test_long_hash });
    return value;
}

BC_AUTO_TEST_CASE(checked__default_construct__zero__valid)
{
    // Empty checksum and empty value, so no state and always valid.
    BC_REQUIRE(zero_checked());
}

BC_AUTO_TEST_CASE(checked__default_construct__non_empty_always__invalid)
{
    // Zeroed value and checksum implies invalid checksum.
    BC_REQUIRE(!mini_checked());
    BC_REQUIRE(!short_checked());
    BC_REQUIRE(!long_checked());
}

BC_AUTO_TEST_CASE(checked__values_construct__zero__valid)
{
    // Empty checksum and empty value, so no state and always valid.
    const zero_checked zero_instance({}, {});
    BC_REQUIRE(zero_instance);

    const mini_checked mini_instance({ 0 }, null_mini_hash);
    BC_REQUIRE(mini_instance);

    const short_checked short_instance({ 0, 0, 0 }, null_short_hash);
    BC_REQUIRE(short_instance);

    const long_checked long_instance({ 0, 0, 0, 0, 0 }, null_long_hash);
    BC_REQUIRE(long_instance);
}

BC_AUTO_TEST_CASE(checked__value_copy_construct__mini__valid)
{
    const mini_checked instance(mini_valid_value());
    BC_REQUIRE(instance);
}

BC_AUTO_TEST_CASE(checked__copy_construct__long__valid)
{
    const long_checked instance(long_valid_value());
    const long_checked copy(instance);
    BC_REQUIRE(copy);
}

BC_AUTO_TEST_CASE(checked__value_move_construct__short__valid)
{
    short_checked instance(short_valid_value());
    const short_checked copy(std::move(instance));
    BC_REQUIRE(copy);
}

BC_AUTO_TEST_CASE(checked__move_construct__long__valid)
{
    long_checked instance(long_valid_value());
    const long_checked copy(std::move(instance));
    BC_REQUIRE(copy);
}

BC_AUTO_TEST_CASE(checked__copy_assign__short__valid)
{
    const short_checked instance(short_valid_value());
    short_checked copy;
    copy = instance;
    BC_REQUIRE(copy);
}

BC_AUTO_TEST_CASE(checked__move_assign__short__valid)
{
    short_checked instance(short_valid_value());
    short_checked copy;
    copy = std::move(instance);
    BC_REQUIRE(copy);
}

BC_AUTO_TEST_CASE(checked__properies__zero__expected)
{
    const zero_checked zero_instance({}, {});
    BC_REQUIRE(zero_instance);
    BC_REQUIRE(zero_instance.prefix() == zero_checked::prefix_type{});
    BC_REQUIRE(zero_instance.payload() == zero_checked::payload_type{});
    BC_REQUIRE_EQUAL(encode_base16(zero_instance.checksum()), "");
}

BC_AUTO_TEST_CASE(checked__properies__mini__expected)
{
    const mini_checked mini_instance({ 42 }, test_mini_hash);
    BC_REQUIRE(mini_instance);
    const auto expected_prefix = mini_checked::prefix_type{ 42 };
    BC_REQUIRE(mini_instance.prefix() == expected_prefix);
    BC_REQUIRE(mini_instance.payload() == test_mini_hash);
    BC_REQUIRE_EQUAL(encode_base16(mini_instance.checksum()), "6a7b");
}

BC_AUTO_TEST_CASE(checked__properies__short__expected)
{
    const short_checked short_instance({ 42, 24, 0 }, test_short_hash);
    BC_REQUIRE(short_instance);
    const auto expected_prefix = short_checked::prefix_type{ 42, 24 };
    BC_REQUIRE(short_instance.prefix() == expected_prefix);
    BC_REQUIRE(short_instance.payload() == test_short_hash);
    BC_REQUIRE_EQUAL(encode_base16(short_instance.checksum()), "77a9ccbb");
}

BC_AUTO_TEST_CASE(checked__properies__long__expected)
{
    const long_checked long_instance({ 1, 24, 42, 2, 0 }, test_long_hash);
    BC_REQUIRE(long_instance);
    const auto expected_prefix = long_checked::prefix_type{ 1, 24, 42, 2, 0 };
    BC_REQUIRE(long_instance.prefix() == expected_prefix);
    BC_REQUIRE(long_instance.payload() == test_long_hash);
    BC_REQUIRE_EQUAL(encode_base16(long_instance.checksum()), "2aed29c75b84cdaf");
}

BC_AUTO_TEST_CASE(checked__equality__same__true)
{
    const long_checked long_instance({ 1, 24, 42, 2, 0 }, test_long_hash);
    BC_REQUIRE(long_instance);
    const auto expected_prefix = long_checked::prefix_type{ 1, 24, 42, 2, 0 };
    BC_REQUIRE(long_instance.prefix() == expected_prefix);
    BC_REQUIRE(long_instance.payload() == test_long_hash);
    BC_REQUIRE_EQUAL(encode_base16(long_instance.checksum()), "2aed29c75b84cdaf");
}


BC_AUTO_TEST_CASE(checked__assign_copy__always__expected)
{
    const short_checked instance1(short_prefix, test_short_hash);
    short_checked instance2{};
    instance2 = instance1;
    BC_REQUIRE(instance2 == instance1);
}

BC_AUTO_TEST_CASE(checked__assign_move__always__expected)
{
    short_checked instance1(short_prefix, test_short_hash);
    const short_checked expected(instance1);
    short_checked instance2{};
    instance2 = std::move(instance1);
    BC_REQUIRE(instance2 == expected);
}

BC_AUTO_TEST_SUITE_END()
