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

BOOST_AUTO_TEST_SUITE(secp256k1_algorithm_scalar_tests)

class accessor
  : public secp256k1::algorithm
{
public:
    using scalar_t = algorithm::scalar_t;
    using bytes_t = algorithm::bytes_t;
    template <size_t Count>
    using digits_t = algorithm::digits_t<Count>;
    using algorithm::order;
    using algorithm::half_order;
    using algorithm::lambda;
    using algorithm::add;
    using algorithm::negate;
    using algorithm::multiply;
    using algorithm::inverse;
    using algorithm::split;
    using algorithm::recode;
    using algorithm::is_zero;
    using algorithm::is_high;
    using algorithm::from_bytes;
    using algorithm::to_bytes;
};

using scalar = accessor::scalar_t;
using bytes = accessor::bytes_t;

// vectors
// ----------------------------------------------------------------------------

constexpr auto zero_value = base16_array("0000000000000000000000000000000000000000000000000000000000000000");
constexpr auto one_value = base16_array("0000000000000000000000000000000000000000000000000000000000000001");
constexpr auto order_value = base16_array("fffffffffffffffffffffffffffffffebaaedce6af48a03bbfd25e8cd0364141");
constexpr auto order_minus_one = base16_array("fffffffffffffffffffffffffffffffebaaedce6af48a03bbfd25e8cd0364140");
constexpr auto order_minus_two = base16_array("fffffffffffffffffffffffffffffffebaaedce6af48a03bbfd25e8cd036413f");
constexpr auto all_value = base16_array("ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff");
constexpr auto all_reduced = base16_array("000000000000000000000000000000014551231950b75fc4402da1732fc9bebe");
constexpr auto first = base16_array("79be667ef9dcbbac55a06295ce870b07029bfcdb2dce28d959f2815b16f81798");
constexpr auto second = base16_array("483ada7726a3c4655da4fbfc0e1108a8fd17b448a68554199c47d08ffb10d4b8");
constexpr auto sample = base16_array("123456789abcdef0fedcba9876543210112233445566778899aabbccddeeff00");
constexpr auto lambda_value = base16_array("5363ad4cc05c30e0a5261c028812645a122e22ea20816678df02967c1b23bd72");

constexpr auto first_plus_second = base16_array("c1f940f620808011b3455e91dc9813afffb3b123d4537cf2f63a51eb1208ec50");
constexpr auto first_times_second = base16_array("805714a252d0c0b58910907e85b5b801fff610a36bdf46847a4bf5d9ae2d10ed");
constexpr auto sample_squared = base16_array("9a4a5698ace6ddd6c4076262d1fda179dc8f9e951d3247798be1fad71e2c5082");
constexpr auto first_negated = base16_array("8641998106234453aa5f9d6a3178f4f7b812e00b817a776265dfdd31b93e29a9");
constexpr auto first_inverse = base16_array("1dd887b3eaf153260a95e8b9fd31f60ac115d26ccbe1f572c0b8d7a6dec520fe");
constexpr auto second_inverse = base16_array("cadc1521998f02828b4f5959bb4518dd8c4cb2baebc2f4a993e1ae3fc8728dff");
constexpr auto sample_inverse = base16_array("c181574534e1e07a7709d64d63897410eef00b22fd7ba34f3342caf8b365a168");

constexpr auto first_k1 = base16_array("fffffffffffffffffffffffffffffffe7cac0edf6c9067a0af824ee954ef57da");
constexpr auto first_k2 = base16_array("0000000000000000000000000000000046faedc0a492faeda3e7027217e1ab30");
constexpr auto second_k1 = base16_array("fffffffffffffffffffffffffffffffe2f59d27cd6d7a0a83ce0d11ecfd4983b");
constexpr auto second_k2 = base16_array("fffffffffffffffffffffffffffffffeb4f9e8ec23324d0f1fc32acffa32dee1");
constexpr auto sample_k1 = base16_array("fffffffffffffffffffffffffffffffeb721d8348f2c98f40a61874fbae09120");
constexpr auto sample_k2 = base16_array("fffffffffffffffffffffffffffffffe7351959911dedb58cf77d469a9ae44bb");

constexpr scalar small_one{ 1, 0, 0, 0 };
constexpr scalar small_max{ max_uint64, max_uint64, 1, 0 };
constexpr scalar small_sample{ 0x0fedcba987654321, 0x0123456789abcdef, 0, 0 };

// helpers
// ----------------------------------------------------------------------------

constexpr scalar decode(const bytes& value) NOEXCEPT
{
    scalar out{};
    accessor::from_bytes(out, value);
    return out;
}

constexpr bytes encode(const scalar& value) NOEXCEPT
{
    bytes out{};
    accessor::to_bytes(out, value);
    return out;
}

constexpr bool is_decodable(const bytes& value) NOEXCEPT
{
    scalar out{};
    return accessor::from_bytes(out, value);
}

constexpr scalar scalar_add(const scalar& a, const scalar& b) NOEXCEPT
{
    scalar out{};
    accessor::add(out, a, b);
    return out;
}

constexpr scalar scalar_negate(const scalar& a) NOEXCEPT
{
    scalar out{};
    accessor::negate(out, a);
    return out;
}

constexpr scalar scalar_multiply(const scalar& a, const scalar& b) NOEXCEPT
{
    scalar out{};
    accessor::multiply(out, a, b);
    return out;
}

constexpr scalar scalar_inverse(const scalar& a) NOEXCEPT
{
    scalar out{};
    accessor::inverse(out, a);
    return out;
}

constexpr scalar split_first(const scalar& k) NOEXCEPT
{
    scalar k1{}, k2{};
    accessor::split(k1, k2, k);
    return k1;
}

constexpr scalar split_second(const scalar& k) NOEXCEPT
{
    scalar k1{}, k2{};
    accessor::split(k1, k2, k);
    return k2;
}

constexpr bool is_joined(const scalar& k) NOEXCEPT
{
    return scalar_add(split_first(k), scalar_multiply(split_second(k), accessor::lambda)) == k;
}

constexpr bool is_small(const scalar& a) NOEXCEPT
{
    const auto magnitude = accessor::is_high(a) ? scalar_negate(a) : a;
    return is_zero(magnitude[2]) && is_zero(magnitude[3]);
}

template <size_t Bits, size_t Count>
constexpr accessor::digits_t<Count> recoded(const scalar& value) NOEXCEPT
{
    accessor::digits_t<Count> out{};
    accessor::recode<Bits, Count>(out, value);
    return out;
}

// encoding
// ----------------------------------------------------------------------------

static_assert(encode(decode(zero_value)) == zero_value);
static_assert(encode(decode(one_value)) == one_value);
static_assert(encode(decode(order_minus_one)) == order_minus_one);
static_assert(encode(decode(first)) == first);
static_assert(encode(decode(sample)) == sample);
static_assert(encode(decode(order_value)) == zero_value);
static_assert(encode(decode(all_value)) == all_reduced);
static_assert(is_decodable(order_minus_one));
static_assert(!is_decodable(order_value));
static_assert(!is_decodable(all_value));
static_assert(decode(order_minus_one) == scalar{ 0xbfd25e8cd0364140, 0xbaaedce6af48a03b, 0xfffffffffffffffe, 0xffffffffffffffff });

BOOST_AUTO_TEST_CASE(secp256k1_algorithm_scalar__encoding__round_trip__expected)
{
    BOOST_CHECK_EQUAL(encode(decode(first)), first);
    BOOST_CHECK_EQUAL(encode(decode(all_value)), all_reduced);
    BOOST_CHECK(!is_decodable(order_value));
    BOOST_CHECK(is_decodable(order_minus_one));
}

// add/negate
// ----------------------------------------------------------------------------

static_assert(encode(scalar_add(decode(first), decode(second))) == first_plus_second);
static_assert(encode(scalar_add(decode(order_minus_one), decode(one_value))) == zero_value);
static_assert(encode(scalar_add(decode(order_minus_one), decode(order_minus_one))) == order_minus_two);
static_assert(encode(scalar_negate(decode(first))) == first_negated);
static_assert(encode(scalar_negate(decode(zero_value))) == zero_value);
static_assert(encode(scalar_negate(decode(one_value))) == order_minus_one);
static_assert(encode(scalar_add(decode(sample), scalar_negate(decode(sample)))) == zero_value);

BOOST_AUTO_TEST_CASE(secp256k1_algorithm_scalar__add_negate__values__expected)
{
    BOOST_CHECK_EQUAL(encode(scalar_add(decode(first), decode(second))), first_plus_second);
    BOOST_CHECK_EQUAL(encode(scalar_add(decode(order_minus_one), decode(order_minus_one))), order_minus_two);
    BOOST_CHECK_EQUAL(encode(scalar_negate(decode(first))), first_negated);
    BOOST_CHECK_EQUAL(encode(scalar_add(decode(sample), scalar_negate(decode(sample)))), zero_value);
}

// multiply
// ----------------------------------------------------------------------------

static_assert(encode(scalar_multiply(decode(first), decode(second))) == first_times_second);
static_assert(encode(scalar_multiply(decode(sample), decode(sample))) == sample_squared);
static_assert(encode(scalar_multiply(decode(order_minus_one), decode(order_minus_one))) == one_value);
static_assert(encode(scalar_multiply(decode(first), decode(one_value))) == first);
static_assert(encode(scalar_multiply(decode(first), decode(zero_value))) == zero_value);

BOOST_AUTO_TEST_CASE(secp256k1_algorithm_scalar__multiply__values__expected)
{
    BOOST_CHECK_EQUAL(encode(scalar_multiply(decode(first), decode(second))), first_times_second);
    BOOST_CHECK_EQUAL(encode(scalar_multiply(decode(sample), decode(sample))), sample_squared);
    BOOST_CHECK_EQUAL(encode(scalar_multiply(decode(order_minus_one), decode(order_minus_one))), one_value);
}

// inverse
// ----------------------------------------------------------------------------

static_assert(encode(scalar_inverse(decode(first))) == first_inverse);
static_assert(encode(scalar_inverse(decode(zero_value))) == zero_value);

BOOST_AUTO_TEST_CASE(secp256k1_algorithm_scalar__inverse__values__expected)
{
    BOOST_CHECK_EQUAL(encode(scalar_inverse(decode(first))), first_inverse);
    BOOST_CHECK_EQUAL(encode(scalar_inverse(decode(second))), second_inverse);
    BOOST_CHECK_EQUAL(encode(scalar_inverse(decode(sample))), sample_inverse);
    BOOST_CHECK_EQUAL(encode(scalar_inverse(decode(one_value))), one_value);
    BOOST_CHECK_EQUAL(encode(scalar_inverse(decode(order_minus_one))), order_minus_one);
    BOOST_CHECK_EQUAL(encode(scalar_multiply(decode(sample), scalar_inverse(decode(sample)))), one_value);
}

// split
// ----------------------------------------------------------------------------

static_assert(encode(split_first(decode(first))) == first_k1);
static_assert(encode(split_second(decode(first))) == first_k2);
static_assert(encode(split_first(decode(one_value))) == one_value);
static_assert(encode(split_second(decode(one_value))) == zero_value);
static_assert(encode(split_first(decode(lambda_value))) == zero_value);
static_assert(encode(split_second(decode(lambda_value))) == one_value);
static_assert(is_joined(decode(first)));
static_assert(is_small(split_first(decode(first))) && is_small(split_second(decode(first))));

BOOST_AUTO_TEST_CASE(secp256k1_algorithm_scalar__split__values__expected)
{
    BOOST_CHECK_EQUAL(encode(split_first(decode(first))), first_k1);
    BOOST_CHECK_EQUAL(encode(split_second(decode(first))), first_k2);
    BOOST_CHECK_EQUAL(encode(split_first(decode(second))), second_k1);
    BOOST_CHECK_EQUAL(encode(split_second(decode(second))), second_k2);
    BOOST_CHECK_EQUAL(encode(split_first(decode(sample))), sample_k1);
    BOOST_CHECK_EQUAL(encode(split_second(decode(sample))), sample_k2);
    BOOST_CHECK_EQUAL(encode(split_first(decode(order_minus_one))), order_minus_one);
    BOOST_CHECK_EQUAL(encode(split_second(decode(order_minus_one))), zero_value);
}

BOOST_AUTO_TEST_CASE(secp256k1_algorithm_scalar__split__values__joined_and_small)
{
    BOOST_CHECK(is_joined(decode(first)));
    BOOST_CHECK(is_joined(decode(second)));
    BOOST_CHECK(is_joined(decode(sample)));
    BOOST_CHECK(is_joined(decode(order_minus_one)));
    BOOST_CHECK(is_small(split_first(decode(second))) && is_small(split_second(decode(second))));
    BOOST_CHECK(is_small(split_first(decode(sample))) && is_small(split_second(decode(sample))));
}

// recode
// ----------------------------------------------------------------------------

constexpr accessor::digits_t<33> one_digits{ -15, -15, -15, -15, -15, -15, -15, -15, -15, -15, -15, -15, -15, -15, -15, -15, -15, -15, -15, -15, -15, -15, -15, -15, -15, -15, -15, -15, -15, -15, -15, -15, 1 };
constexpr accessor::digits_t<33> max_digits{ 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 1 };
constexpr accessor::digits_t<33> sample_digits4{ -15, 3, -13, 5, -11, 7, -9, 9, -7, 11, -5, 13, -3, 15, -1, 1, -1, 15, -3, 13, -5, 11, -7, 9, -9, 7, -11, 5, -13, 3, -15, -15, 1 };
constexpr accessor::digits_t<26> sample_digits5{ 1, -7, -15, -21, 23, -29, 7, 21, -21, 15, 27, -1, 17, 23, 23, 25, 11, -19, 3, -17, -9, 3, -19, 5, -31, 1 };

static_assert(recoded<4, 33>(small_one) == one_digits);
static_assert(recoded<4, 33>(small_max) == max_digits);
static_assert(recoded<4, 33>(small_sample) == sample_digits4);
static_assert(recoded<5, 26>(small_sample) == sample_digits5);

BOOST_AUTO_TEST_CASE(secp256k1_algorithm_scalar__recode__values__expected)
{
    BOOST_CHECK((recoded<4, 33>(small_sample) == sample_digits4));
    BOOST_CHECK((recoded<5, 26>(small_sample) == sample_digits5));
}

// predicates
// ----------------------------------------------------------------------------

static_assert(accessor::is_zero(decode(zero_value)));
static_assert(!accessor::is_zero(decode(one_value)));
static_assert(!accessor::is_high(accessor::half_order));
static_assert(accessor::is_high(scalar_add(accessor::half_order, decode(one_value))));
static_assert(accessor::is_high(decode(order_minus_one)));
static_assert(!accessor::is_high(decode(one_value)));

BOOST_AUTO_TEST_SUITE_END()
