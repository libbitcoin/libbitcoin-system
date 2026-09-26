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

BOOST_AUTO_TEST_SUITE(secp256k1_algorithm_field_tests)

class accessor
  : public secp256k1::algorithm
{
public:
    template <typename Word>
    using field_t = algorithm::field_t<Word>;
    using bytes_t = algorithm::bytes_t;
    using algorithm::limb_mask;
    using algorithm::top_mask;
    using algorithm::prime;
    using algorithm::add;
    using algorithm::negate;
    using algorithm::carry;
    using algorithm::normalize;
    using algorithm::multiply;
    using algorithm::square;
    using algorithm::inverse;
    using algorithm::square_root;
    using algorithm::is_zero;
    using algorithm::is_odd;
    using algorithm::equal;
    using algorithm::from_bytes;
    using algorithm::to_bytes;
};

using field = accessor::field_t<uint64_t>;
using bytes = accessor::bytes_t;

// vectors
// ----------------------------------------------------------------------------

constexpr auto zero_value = base16_array("0000000000000000000000000000000000000000000000000000000000000000");
constexpr auto one_value = base16_array("0000000000000000000000000000000000000000000000000000000000000001");
constexpr auto seven_value = base16_array("0000000000000000000000000000000000000000000000000000000000000007");
constexpr auto half_value = base16_array("8000000000000000000000000000000000000000000000000000000000000000");
constexpr auto prime_value = base16_array("fffffffffffffffffffffffffffffffffffffffffffffffffffffffefffffc2f");
constexpr auto prime_minus_one = base16_array("fffffffffffffffffffffffffffffffffffffffffffffffffffffffefffffc2e");
constexpr auto all_value = base16_array("ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff");
constexpr auto sample = base16_array("123456789abcdef0fedcba9876543210112233445566778899aabbccddeeff00");
constexpr auto gx = base16_array("79be667ef9dcbbac55a06295ce870b07029bfcdb2dce28d959f2815b16f81798");
constexpr auto gy = base16_array("483ada7726a3c4655da4fbfc0e1108a8fd17b448a68554199c47d08ffb10d4b8");

constexpr auto gx_times_gy = base16_array("fd3dc529c6eb60fb9d166034cf3c1a5a72324aa9dfd3428a56d7e1ce0179fd9b");
constexpr auto gx_plus_gy = base16_array("c1f940f620808011b3455e91dc9813afffb3b123d4537cf2f63a51eb1208ec50");
constexpr auto gx_negated = base16_array("8641998106234453aa5f9d6a3178f4f8fd640324d231d726a60d7ea3e907e497");
constexpr auto gx_squared = base16_array("8550e7d238fcf3086ba9adcf0fb52a9de3652194d06cb5bb38d50229b854fc49");
constexpr auto gy_squared = base16_array("4866d6a5ab41ab2c6bcc57ccd3735da5f16f80a548e5e20a44e4e9b8118c26f2");
constexpr auto sample_times_half = base16_array("0a3d709bc4d5c82a8f5c28f5d43aa14adc760fa942dc760fb25d076f3b6d1d0f");
constexpr auto sample_squared = base16_array("e5fac473f7a6ab795dfd35e77c6717c6fc0017821c9e0698335021bdd8c498f2");
constexpr auto gx_inverse = base16_array("237afdf1d2938d86870aaeb8ad77626a67b8e794abfb076be61d003687ca9ef6");
constexpr auto gy_inverse = base16_array("6fc6340c9dae9a629bcf20238be148d582aac046a7b87a681f7d5dda2ecf511d");
constexpr auto sample_inverse = base16_array("75e927d5286dbac0cc9c7ab7b2938cde7deb9bcfd864f2f7416b9ebe7ddb9304");
constexpr auto gx_root = base16_array("cb6dfbd6cdf31164bbeb3052460c1fa3f827f01d6e7fb5f69580cfb96560c16a");
constexpr auto sample_root = base16_array("e4a6ec4a58703dc5c8d9b12b0c9bde24175e882fb7a4b5c2573e5daac49d4115");
constexpr auto all_reduced = base16_array("00000000000000000000000000000000000000000000000000000001000003d0");
constexpr auto power_reduced = base16_array("00000000000000000000000000000000000000000000000000000001000003d1");
constexpr auto weak_reduced = base16_array("00000000000000000000000000000000000000000000000000000002000007a1");
constexpr auto weak_squared = base16_array("00000000000000000000000000000000000000000000000400001e84003a3341");
constexpr auto weak_times_gx = base16_array("b9456608d83482e4d99e2eea99731a3143a055cb25aaa4de5479db597a042996");
constexpr auto loose_reduced = base16_array("0000000003ff00000000003ff00000000003ff00000000003ff0400000f43fff");

constexpr auto mask = accessor::limb_mask;
constexpr auto top = accessor::top_mask;
constexpr auto weak_top = bit_or<uint64_t>(shift_left(top), one);
constexpr uint64_t loose = 0x3fffffffffffffff;
constexpr field prime_limbs{ accessor::prime };
constexpr field prime_plus_one{ add1(accessor::prime[0]), mask, mask, mask, top };
constexpr field all_limbs{ mask, mask, mask, mask, top };
constexpr field power_limbs{ 0, 0, 0, 0, add1(top) };
constexpr field weak_limbs{ mask, mask, mask, mask, weak_top };
constexpr field loose_limbs{ loose, loose, loose, loose, loose };

// helpers
// ----------------------------------------------------------------------------

constexpr field decode(const bytes& value) NOEXCEPT
{
    field out{};
    accessor::from_bytes(out, value);
    return out;
}

constexpr bytes encode(field value) NOEXCEPT
{
    accessor::normalize(value);
    bytes out{};
    accessor::to_bytes(out, value);
    return out;
}

constexpr bool is_weak(const field& value) NOEXCEPT
{
    return value[0] <= mask && value[1] <= mask && value[2] <= mask &&
        value[3] <= mask && value[4] <= weak_top;
}

constexpr bool is_decodable(const bytes& value) NOEXCEPT
{
    field out{};
    return accessor::from_bytes(out, value);
}

constexpr field field_add(const field& a, const field& b) NOEXCEPT
{
    field out{};
    accessor::add(out, a, b);
    return out;
}

constexpr field field_negate(const field& a) NOEXCEPT
{
    field out{};
    accessor::negate(out, a);
    return out;
}

constexpr field field_carry(field value) NOEXCEPT
{
    accessor::carry(value);
    return value;
}

constexpr field field_multiply(const field& a, const field& b) NOEXCEPT
{
    field out{};
    accessor::multiply(out, a, b);
    return out;
}

constexpr field field_square(const field& a) NOEXCEPT
{
    field out{};
    accessor::square(out, a);
    return out;
}

constexpr field field_square_twice(const field& a) NOEXCEPT
{
    field out{};
    accessor::square<2>(out, a);
    return out;
}

constexpr field field_inverse(const field& a) NOEXCEPT
{
    field out{};
    accessor::inverse(out, a);
    return out;
}

constexpr field field_root(const field& a) NOEXCEPT
{
    field out{};
    accessor::square_root(out, a);
    return out;
}

constexpr bool field_is_square(const field& a) NOEXCEPT
{
    field out{};
    return f::any(accessor::square_root(out, a));
}

// encoding
// ----------------------------------------------------------------------------

static_assert(encode(decode(zero_value)) == zero_value);
static_assert(encode(decode(one_value)) == one_value);
static_assert(encode(decode(prime_minus_one)) == prime_minus_one);
static_assert(encode(decode(gx)) == gx);
static_assert(encode(decode(gy)) == gy);
static_assert(encode(decode(sample)) == sample);
static_assert(is_decodable(prime_minus_one));
static_assert(!is_decodable(prime_value));
static_assert(!is_decodable(all_value));
static_assert(decode(prime_value) == prime_limbs);
static_assert(decode(all_value) == all_limbs);

BOOST_AUTO_TEST_CASE(secp256k1_algorithm_field__encoding__round_trip__expected)
{
    BOOST_CHECK_EQUAL(encode(decode(gx)), gx);
    BOOST_CHECK_EQUAL(encode(decode(prime_minus_one)), prime_minus_one);
    BOOST_CHECK(!is_decodable(prime_value));
}

// carry/normalize
// ----------------------------------------------------------------------------

static_assert(is_weak(field_carry(loose_limbs)));
static_assert(is_weak(field_carry(all_limbs)));
static_assert(is_weak(field_carry(power_limbs)));
static_assert(encode(field_carry(loose_limbs)) == loose_reduced);
static_assert(encode(prime_limbs) == zero_value);
static_assert(encode(prime_plus_one) == one_value);
static_assert(encode(all_limbs) == all_reduced);
static_assert(encode(power_limbs) == power_reduced);
static_assert(encode(weak_limbs) == weak_reduced);
static_assert(encode(loose_limbs) == loose_reduced);

BOOST_AUTO_TEST_CASE(secp256k1_algorithm_field__normalize__edges__expected)
{
    BOOST_CHECK(is_weak(field_carry(loose_limbs)));
    BOOST_CHECK_EQUAL(encode(prime_limbs), zero_value);
    BOOST_CHECK_EQUAL(encode(prime_plus_one), one_value);
    BOOST_CHECK_EQUAL(encode(all_limbs), all_reduced);
    BOOST_CHECK_EQUAL(encode(power_limbs), power_reduced);
    BOOST_CHECK_EQUAL(encode(weak_limbs), weak_reduced);
    BOOST_CHECK_EQUAL(encode(loose_limbs), loose_reduced);
}

// add/negate
// ----------------------------------------------------------------------------

static_assert(encode(field_add(decode(gx), decode(gy))) == gx_plus_gy);
static_assert(encode(field_add(decode(prime_minus_one), decode(one_value))) == zero_value);
static_assert(encode(field_negate(decode(gx))) == gx_negated);
static_assert(encode(field_negate(decode(zero_value))) == zero_value);
static_assert(encode(field_add(decode(gx), field_negate(decode(gx)))) == zero_value);
static_assert(encode(field_add(weak_limbs, field_negate(weak_limbs))) == zero_value);

BOOST_AUTO_TEST_CASE(secp256k1_algorithm_field__add_negate__values__expected)
{
    BOOST_CHECK_EQUAL(encode(field_add(decode(gx), decode(gy))), gx_plus_gy);
    BOOST_CHECK_EQUAL(encode(field_negate(decode(gx))), gx_negated);
    BOOST_CHECK_EQUAL(encode(field_add(weak_limbs, field_negate(weak_limbs))), zero_value);
}

// multiply/square
// ----------------------------------------------------------------------------

static_assert(encode(field_multiply(decode(gx), decode(gy))) == gx_times_gy);
static_assert(encode(field_multiply(decode(sample), decode(half_value))) == sample_times_half);
static_assert(encode(field_multiply(decode(prime_minus_one), decode(prime_minus_one))) == one_value);
static_assert(encode(field_multiply(decode(gx), decode(zero_value))) == zero_value);
static_assert(encode(field_multiply(decode(gx), decode(one_value))) == gx);
static_assert(encode(field_multiply(weak_limbs, weak_limbs)) == weak_squared);
static_assert(encode(field_multiply(weak_limbs, decode(gx))) == weak_times_gx);
static_assert(encode(field_square(decode(gx))) == gx_squared);
static_assert(encode(field_square(decode(gy))) == gy_squared);
static_assert(encode(field_square(decode(sample))) == sample_squared);
static_assert(encode(field_square(weak_limbs)) == weak_squared);
static_assert(encode(field_square_twice(decode(gx))) == encode(field_square(field_square(decode(gx)))));
static_assert(is_weak(field_multiply(weak_limbs, weak_limbs)));
static_assert(is_weak(field_square(weak_limbs)));

BOOST_AUTO_TEST_CASE(secp256k1_algorithm_field__multiply__values__expected)
{
    BOOST_CHECK_EQUAL(encode(field_multiply(decode(gx), decode(gy))), gx_times_gy);
    BOOST_CHECK_EQUAL(encode(field_multiply(decode(sample), decode(half_value))), sample_times_half);
    BOOST_CHECK_EQUAL(encode(field_multiply(decode(prime_minus_one), decode(prime_minus_one))), one_value);
    BOOST_CHECK_EQUAL(encode(field_multiply(weak_limbs, weak_limbs)), weak_squared);
    BOOST_CHECK_EQUAL(encode(field_multiply(weak_limbs, decode(gx))), weak_times_gx);
    BOOST_CHECK(is_weak(field_multiply(weak_limbs, weak_limbs)));
}

BOOST_AUTO_TEST_CASE(secp256k1_algorithm_field__square__values__expected)
{
    BOOST_CHECK_EQUAL(encode(field_square(decode(gx))), gx_squared);
    BOOST_CHECK_EQUAL(encode(field_square(decode(gy))), gy_squared);
    BOOST_CHECK_EQUAL(encode(field_square(decode(sample))), sample_squared);
    BOOST_CHECK_EQUAL(encode(field_square(weak_limbs)), weak_squared);
    BOOST_CHECK(is_weak(field_square(weak_limbs)));
}

BOOST_AUTO_TEST_CASE(secp256k1_algorithm_field__curve__generator__on_curve)
{
    const auto y2 = field_square(decode(gy));
    const auto x3 = field_multiply(field_square(decode(gx)), decode(gx));
    BOOST_CHECK_EQUAL(encode(y2), encode(field_add(x3, decode(seven_value))));
}

// inverse
// ----------------------------------------------------------------------------

static_assert(encode(field_inverse(decode(gx))) == gx_inverse);
static_assert(encode(field_inverse(decode(zero_value))) == zero_value);

BOOST_AUTO_TEST_CASE(secp256k1_algorithm_field__inverse__values__expected)
{
    BOOST_CHECK_EQUAL(encode(field_inverse(decode(gx))), gx_inverse);
    BOOST_CHECK_EQUAL(encode(field_inverse(decode(gy))), gy_inverse);
    BOOST_CHECK_EQUAL(encode(field_inverse(decode(sample))), sample_inverse);
    BOOST_CHECK_EQUAL(encode(field_inverse(decode(one_value))), one_value);
    BOOST_CHECK_EQUAL(encode(field_inverse(decode(zero_value))), zero_value);
    BOOST_CHECK_EQUAL(encode(field_multiply(decode(sample), field_inverse(decode(sample)))), one_value);
    BOOST_CHECK_EQUAL(encode(field_multiply(weak_limbs, field_inverse(weak_limbs))), one_value);
}

// square_root
// ----------------------------------------------------------------------------

static_assert(encode(field_root(decode(gy_squared))) == gy);
static_assert(field_is_square(decode(gy_squared)));
static_assert(!field_is_square(decode(seven_value)));

BOOST_AUTO_TEST_CASE(secp256k1_algorithm_field__square_root__squares__expected)
{
    BOOST_CHECK_EQUAL(encode(field_root(decode(gy_squared))), gy);
    BOOST_CHECK_EQUAL(encode(field_root(decode(gx))), gx_root);
    BOOST_CHECK_EQUAL(encode(field_root(decode(sample))), sample_root);
    BOOST_CHECK_EQUAL(encode(field_root(decode(zero_value))), zero_value);
    BOOST_CHECK(field_is_square(decode(gy_squared)));
    BOOST_CHECK(field_is_square(decode(gx)));
    BOOST_CHECK(field_is_square(decode(sample)));
    BOOST_CHECK(field_is_square(decode(zero_value)));
}

BOOST_AUTO_TEST_CASE(secp256k1_algorithm_field__square_root__non_squares__false)
{
    BOOST_CHECK(!field_is_square(decode(seven_value)));
    BOOST_CHECK(!field_is_square(decode(prime_minus_one)));
}

// predicates
// ----------------------------------------------------------------------------

static_assert(f::any(accessor::is_zero(decode(zero_value))));
static_assert(!f::any(accessor::is_zero(decode(one_value))));
static_assert(f::any(accessor::is_odd(decode(one_value))));
static_assert(!f::any(accessor::is_odd(decode(zero_value))));
static_assert(!f::any(accessor::is_odd(decode(gy))));
static_assert(f::any(accessor::equal(decode(gx), decode(gx))));
static_assert(!f::any(accessor::equal(decode(gx), decode(gy))));

// lanes
// ----------------------------------------------------------------------------

template <typename xWord>
using xfield = accessor::field_t<xWord>;

template <typename xWord>
constexpr auto lanes = capacity<xWord, uint64_t>;

using fields = std_array<field, 8>;
using masks = std_array<uint64_t, 8>;

constexpr fields left
{
    weak_limbs, decode(gx), decode(prime_minus_one), decode(zero_value),
    decode(gy), decode(sample), decode(seven_value), decode(one_value)
};

constexpr fields right
{
    weak_limbs, decode(gy), decode(prime_minus_one), decode(sample),
    decode(half_value), decode(one_value), decode(gy_squared), decode(gx)
};

constexpr fields loosed
{
    loose_limbs, all_limbs, power_limbs, prime_limbs,
    prime_plus_one, weak_limbs, decode(gx), decode(zero_value)
};

constexpr fields normals
{
    decode(gx), decode(zero_value), decode(prime_minus_one), decode(one_value),
    decode(gy), decode(gx), decode(seven_value), decode(sample)
};

constexpr fields generators
{
    decode(gx), decode(gx), decode(gx), decode(gx),
    decode(gx), decode(gx), decode(gx), decode(gx)
};

constexpr field normal(field value) NOEXCEPT
{
    accessor::normalize(value);
    return value;
}

constexpr field field_subtract(const field& a, const field& b) NOEXCEPT
{
    return field_add(a, field_negate(b));
}

constexpr uint64_t field_square_mask(const field& a) NOEXCEPT
{
    field out{};
    return accessor::square_root(out, a);
}

constexpr uint64_t field_is_zero(const field& a) NOEXCEPT
{
    return accessor::is_zero(a);
}

constexpr uint64_t field_is_odd(const field& a) NOEXCEPT
{
    return accessor::is_odd(a);
}

constexpr uint64_t field_equal(const field& a, const field& b) NOEXCEPT
{
    return accessor::equal(a, b);
}

constexpr fields expect(auto function, const fields& a) NOEXCEPT
{
    return
    {
        normal(function(a[0])), normal(function(a[1])),
        normal(function(a[2])), normal(function(a[3])),
        normal(function(a[4])), normal(function(a[5])),
        normal(function(a[6])), normal(function(a[7]))
    };
}

constexpr fields expect(auto function, const fields& a, const fields& b) NOEXCEPT
{
    return
    {
        normal(function(a[0], b[0])), normal(function(a[1], b[1])),
        normal(function(a[2], b[2])), normal(function(a[3], b[3])),
        normal(function(a[4], b[4])), normal(function(a[5], b[5])),
        normal(function(a[6], b[6])), normal(function(a[7], b[7]))
    };
}

constexpr masks expect_masks(auto function, const fields& a) NOEXCEPT
{
    return
    {
        function(a[0]), function(a[1]), function(a[2]), function(a[3]),
        function(a[4]), function(a[5]), function(a[6]), function(a[7])
    };
}

constexpr masks expect_masks(auto function, const fields& a,
    const fields& b) NOEXCEPT
{
    return
    {
        function(a[0], b[0]), function(a[1], b[1]),
        function(a[2], b[2]), function(a[3], b[3]),
        function(a[4], b[4]), function(a[5], b[5]),
        function(a[6], b[6]), function(a[7], b[7])
    };
}

template <typename xWord>
static xfield<xWord> pack(const fields& in) NOEXCEPT
{
    if constexpr (lanes<xWord> == 8)
    {
        return
        {
            f::set<xWord>(in[0][0], in[1][0], in[2][0], in[3][0],
                in[4][0], in[5][0], in[6][0], in[7][0]),
            f::set<xWord>(in[0][1], in[1][1], in[2][1], in[3][1],
                in[4][1], in[5][1], in[6][1], in[7][1]),
            f::set<xWord>(in[0][2], in[1][2], in[2][2], in[3][2],
                in[4][2], in[5][2], in[6][2], in[7][2]),
            f::set<xWord>(in[0][3], in[1][3], in[2][3], in[3][3],
                in[4][3], in[5][3], in[6][3], in[7][3]),
            f::set<xWord>(in[0][4], in[1][4], in[2][4], in[3][4],
                in[4][4], in[5][4], in[6][4], in[7][4])
        };
    }
    else if constexpr (lanes<xWord> == 4)
    {
        return
        {
            f::set<xWord>(in[0][0], in[1][0], in[2][0], in[3][0]),
            f::set<xWord>(in[0][1], in[1][1], in[2][1], in[3][1]),
            f::set<xWord>(in[0][2], in[1][2], in[2][2], in[3][2]),
            f::set<xWord>(in[0][3], in[1][3], in[2][3], in[3][3]),
            f::set<xWord>(in[0][4], in[1][4], in[2][4], in[3][4])
        };
    }
    else
    {
        return
        {
            f::set<xWord>(in[0][0], in[1][0]),
            f::set<xWord>(in[0][1], in[1][1]),
            f::set<xWord>(in[0][2], in[1][2]),
            f::set<xWord>(in[0][3], in[1][3]),
            f::set<xWord>(in[0][4], in[1][4])
        };
    }
}

template <size_t Lane, typename xWord>
static field unpack(const xfield<xWord>& in) NOEXCEPT
{
    return
    {
        f::get<uint64_t, Lane>(in[0]),
        f::get<uint64_t, Lane>(in[1]),
        f::get<uint64_t, Lane>(in[2]),
        f::get<uint64_t, Lane>(in[3]),
        f::get<uint64_t, Lane>(in[4])
    };
}

template <typename xWord>
static void check_fields(const xfield<xWord>& out, const fields& expected)
{
    BOOST_CHECK_EQUAL(unpack<0>(out), expected[0]);
    BOOST_CHECK_EQUAL(unpack<1>(out), expected[1]);

    if constexpr (lanes<xWord> >= 4)
    {
        BOOST_CHECK_EQUAL(unpack<2>(out), expected[2]);
        BOOST_CHECK_EQUAL(unpack<3>(out), expected[3]);
    }

    if constexpr (lanes<xWord> >= 8)
    {
        BOOST_CHECK_EQUAL(unpack<4>(out), expected[4]);
        BOOST_CHECK_EQUAL(unpack<5>(out), expected[5]);
        BOOST_CHECK_EQUAL(unpack<6>(out), expected[6]);
        BOOST_CHECK_EQUAL(unpack<7>(out), expected[7]);
    }
}

template <typename xWord>
static void check_masks(xWord out, const masks& expected)
{
    BOOST_CHECK_EQUAL((f::get<uint64_t, 0>(out)), expected[0]);
    BOOST_CHECK_EQUAL((f::get<uint64_t, 1>(out)), expected[1]);

    if constexpr (lanes<xWord> >= 4)
    {
        BOOST_CHECK_EQUAL((f::get<uint64_t, 2>(out)), expected[2]);
        BOOST_CHECK_EQUAL((f::get<uint64_t, 3>(out)), expected[3]);
    }

    if constexpr (lanes<xWord> >= 8)
    {
        BOOST_CHECK_EQUAL((f::get<uint64_t, 4>(out)), expected[4]);
        BOOST_CHECK_EQUAL((f::get<uint64_t, 5>(out)), expected[5]);
        BOOST_CHECK_EQUAL((f::get<uint64_t, 6>(out)), expected[6]);
        BOOST_CHECK_EQUAL((f::get<uint64_t, 7>(out)), expected[7]);
    }
}

template <typename xWord>
static void check_add_negate()
{
    if constexpr (have<xWord>)
    {
        xfield<xWord> negated{}, out{};
        accessor::negate(negated, pack<xWord>(right));
        accessor::add(out, pack<xWord>(left), negated);
        accessor::normalize(out);
        check_fields(out, expect(field_subtract, left, right));
    }
}

template <typename xWord>
static void check_multiply()
{
    if constexpr (have<xWord>)
    {
        xfield<xWord> out{};
        accessor::multiply(out, pack<xWord>(left), pack<xWord>(right));
        accessor::normalize(out);
        check_fields(out, expect(field_multiply, left, right));
    }
}

template <typename xWord>
static void check_square()
{
    if constexpr (have<xWord>)
    {
        xfield<xWord> out{};
        accessor::square(out, pack<xWord>(left));
        accessor::normalize(out);
        check_fields(out, expect(field_square, left));
    }
}

template <typename xWord>
static void check_inverse()
{
    if constexpr (have<xWord>)
    {
        xfield<xWord> out{};
        accessor::inverse(out, pack<xWord>(left));
        accessor::normalize(out);
        check_fields(out, expect(field_inverse, left));
    }
}

template <typename xWord>
static void check_square_root()
{
    if constexpr (have<xWord>)
    {
        xfield<xWord> out{};
        const auto squares = accessor::square_root(out, pack<xWord>(right));
        accessor::normalize(out);
        check_fields(out, expect(field_root, right));
        check_masks(squares, expect_masks(field_square_mask, right));
    }
}

template <typename xWord>
static void check_normalize()
{
    if constexpr (have<xWord>)
    {
        auto out = pack<xWord>(loosed);
        accessor::normalize(out);
        check_fields(out, expect(normal, loosed));
    }
}

template <typename xWord>
static void check_predicates()
{
    if constexpr (have<xWord>)
    {
        const auto in = pack<xWord>(normals);
        check_masks(accessor::is_zero(in), expect_masks(field_is_zero, normals));
        check_masks(accessor::is_odd(in), expect_masks(field_is_odd, normals));
        check_masks(accessor::equal(in, pack<xWord>(generators)),
            expect_masks(field_equal, normals, generators));
    }
}

BOOST_AUTO_TEST_CASE(secp256k1_algorithm_field__add_negate__lanes__match_integral)
{
    check_add_negate<xint128_t>();
    check_add_negate<xint256_t>();
    check_add_negate<xint512_t>();
}

BOOST_AUTO_TEST_CASE(secp256k1_algorithm_field__multiply__lanes__match_integral)
{
    check_multiply<xint128_t>();
    check_multiply<xint256_t>();
    check_multiply<xint512_t>();
}

BOOST_AUTO_TEST_CASE(secp256k1_algorithm_field__square__lanes__match_integral)
{
    check_square<xint128_t>();
    check_square<xint256_t>();
    check_square<xint512_t>();
}

BOOST_AUTO_TEST_CASE(secp256k1_algorithm_field__inverse__lanes__match_integral)
{
    check_inverse<xint128_t>();
    check_inverse<xint256_t>();
    check_inverse<xint512_t>();
}

BOOST_AUTO_TEST_CASE(secp256k1_algorithm_field__square_root__lanes__match_integral)
{
    check_square_root<xint128_t>();
    check_square_root<xint256_t>();
    check_square_root<xint512_t>();
}

BOOST_AUTO_TEST_CASE(secp256k1_algorithm_field__normalize__lanes__match_integral)
{
    check_normalize<xint128_t>();
    check_normalize<xint256_t>();
    check_normalize<xint512_t>();
}

BOOST_AUTO_TEST_CASE(secp256k1_algorithm_field__predicates__lanes__match_integral)
{
    check_predicates<xint128_t>();
    check_predicates<xint256_t>();
    check_predicates<xint512_t>();
}

BOOST_AUTO_TEST_SUITE_END()

