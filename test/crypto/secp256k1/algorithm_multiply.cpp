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

BOOST_AUTO_TEST_SUITE(secp256k1_algorithm_multiply_tests)

class accessor
  : public secp256k1::algorithm
{
public:
    template <typename Word>
    using field_t = algorithm::field_t<Word>;
    template <typename Word>
    using affine_t = algorithm::affine_t<Word>;
    template <typename Word>
    using jacobian_t = algorithm::jacobian_t<Word>;
    template <typename Word>
    using scalars_t = algorithm::scalars_t<Word>;
    using scalar_t = algorithm::scalar_t;
    using term_t = algorithm::term_t;
    using bytes_t = algorithm::bytes_t;
    using algorithm::multiply;
    using algorithm::multiply_complete;
    using algorithm::linear;
    using algorithm::secret_multiply;
    using algorithm::lookup_comb;
    using algorithm::add_comb;
    using algorithm::comb_bits;
    using algorithm::naf_t;
    using algorithm::naf;
    using algorithm::to_affine;
    using algorithm::from_bytes;
    using algorithm::to_bytes;
    using algorithm::generator_slices;
    using algorithm::slice_size;
    using algorithm::table_words;
    using algorithm::block_size;
    using algorithm::locate;
    using algorithm::beta;
    using algorithm::normalize;
};

using field = accessor::field_t<uint64_t>;
using affine = accessor::affine_t<uint64_t>;
using jacobian = accessor::jacobian_t<uint64_t>;
using scalar = accessor::scalar_t;
using bytes = accessor::bytes_t;

// vectors
// ----------------------------------------------------------------------------

constexpr auto zero_value = base16_array("0000000000000000000000000000000000000000000000000000000000000000");
constexpr auto one_value = base16_array("0000000000000000000000000000000000000000000000000000000000000001");
constexpr auto order_minus_one = base16_array("fffffffffffffffffffffffffffffffebaaedce6af48a03bbfd25e8cd0364140");
constexpr auto order_minus_two = base16_array("fffffffffffffffffffffffffffffffebaaedce6af48a03bbfd25e8cd036413f");
constexpr auto sample = base16_array("123456789abcdef0fedcba9876543210112233445566778899aabbccddeeff00");
constexpr auto first = base16_array("79be667ef9dcbbac55a06295ce870b07029bfcdb2dce28d959f2815b16f81798");
constexpr auto second = base16_array("483ada7726a3c4655da4fbfc0e1108a8fd17b448a68554199c47d08ffb10d4b8");

constexpr auto gx = base16_array("79be667ef9dcbbac55a06295ce870b07029bfcdb2dce28d959f2815b16f81798");
constexpr auto gy = base16_array("483ada7726a3c4655da4fbfc0e1108a8fd17b448a68554199c47d08ffb10d4b8");
constexpr auto gy_negated = base16_array("b7c52588d95c3b9aa25b0403f1eef75702e84bb7597aabe663b82f6f04ef2777");
constexpr auto gx_beta = base16_array("bcace2e99da01887ab0102b696902325872844067f15e98da7bba04400b88fcb");
constexpr auto g2x = base16_array("c6047f9441ed7d6d3045406e95c07cd85c778e4b8cef3ca7abac09b95c709ee5");
constexpr auto g2y = base16_array("1ae168fea63dc339a3c58419466ceaeef7f632653266d0e1236431a950cfe52a");
constexpr auto g3x = base16_array("f9308a019258c31049344f85f89d5229b531c845836f99b08601f113bce036f9");
constexpr auto g3y = base16_array("388f7b0f632de8140fe337e62a37f3566500a99934c2231b6cb9fd7584b8e672");
constexpr auto g7x = base16_array("5cbdf0646e5db4eaa398f365f2ea7a0e3d419b7e0330e39ce92bddedcac4f9bc");
constexpr auto g7y = base16_array("6aebca40ba255960a3178d6d861a54dba813d0b813fde7b5a5082628087264da");
constexpr auto g127x = base16_array("841d6063a586fa475a724604da03bc5b92a2e0d2e0a36acfe4c73a5514742881");
constexpr auto g127y = base16_array("073867f59c0659e81904f9a1c7543698e62562d6744c169ce7a36de01a8d6154");
constexpr auto g1023x = base16_array("c7a363246aeb7c8c991b2aa710abdf5cfff2991230b3a69fbe2dd4817c7c3e0a");
constexpr auto g1023y = base16_array("1298fdd70e448d2d799863026b4b2d4902b32148d6d1e5f815f5b0c005c9e21f");
constexpr auto px = base16_array("74e8586b1604b6409cb198eef8a40ef97294fcfb38f770e1c7b111163f57c99b");
constexpr auto py = base16_array("88a13c2536d83144543c105147aeb62795c99d6d17577ffdbe4fba047946c4b1");

// sample * G + first * P, and (n - 1) * G + (n - 2) * P.
constexpr auto sum1x = base16_array("790161baa4ad68e25fda5ce43e0dd602434b9d4cd9dde402ba8744a4afc17431");
constexpr auto sum1y = base16_array("f3d8926a99955c5823f45393de1a4a001d7bab1979ffa4f5c8ec650f1b8af96f");
constexpr auto sum2x = base16_array("4cfbbe145caa965ec1ea94384d162f78e9c32a851ce9dff664d634310d270db7");
constexpr auto sum2y = base16_array("7e43fceb6c4239a973f348dd61c628a7ab9abca26a0f36e1ebca8c9563fd6b19");

// helpers
// ----------------------------------------------------------------------------

constexpr field decode(const bytes& value) NOEXCEPT
{
    field out{};
    accessor::from_bytes(out, value);
    return out;
}

constexpr bytes encode(const field& value) NOEXCEPT
{
    bytes out{};
    accessor::to_bytes(out, value);
    return out;
}

constexpr scalar number(const bytes& value) NOEXCEPT
{
    scalar out{};
    accessor::from_bytes(out, value);
    return out;
}

static affine entry(size_t index, bool mapped) NOEXCEPT
{
    const auto slice = accessor::generator_slices[index / accessor::slice_size];
    const auto offset = accessor::locate(index % accessor::slice_size) + (mapped ? accessor::table_words : 0u);
    affine out{};
    BC_PUSH_WARNING(NO_POINTER_ARITHMETIC)
    for (size_t limb{}; limb < out.x.size(); ++limb)
    {
        out.x[limb] = slice[offset + limb * accessor::block_size];
        out.y[limb] = slice[offset + (out.x.size() + limb) * accessor::block_size];
    }
    BC_POP_WARNING()
    return out;
}

// Generator table entry is (2 * index + 1)G, as computed by libsecp256k1.
static bool is_multiple(size_t index) NOEXCEPT
{
    const auto multiple = add1(two * index);
    ec_secret secret{};
    secret[30] = narrow_cast<uint8_t>(multiple >> byte_bits);
    secret[31] = narrow_cast<uint8_t>(multiple);

    ec_compressed key{};
    const auto point = entry(index, false);
    const auto x = encode(point.x);
    return secret_to_public(key, secret) &&
        std::equal(x.begin(), x.end(), std::next(key.begin())) &&
        (key.front() == ec_odd_sign) == get_right(point.y[0]);
}

// Entry of the endomorphism table is that of the generator table with x * beta.
static bool is_mapped(size_t index) NOEXCEPT
{
    const auto point = entry(index, false);
    const auto mapped = entry(index, true);
    field x{};
    accessor::multiply(x, point.x, accessor::beta);
    accessor::normalize(x);
    return x == mapped.x && point.y == mapped.y;
}

constexpr bool is_point(const affine& a, const bytes& x, const bytes& y) NOEXCEPT
{
    return encode(a.x) == x && encode(a.y) == y;
}

static bool is_point(const jacobian& a, const bytes& x, const bytes& y) NOEXCEPT
{
    affine out{};
    accessor::to_affine(out, a);
    return is_point(out, x, y);
}

static jacobian product(const scalar& g, const affine& a, const scalar& k) NOEXCEPT
{
    jacobian out{};
    accessor::multiply(out, accessor::scalars_t<uint64_t>{ g }, a,
        accessor::scalars_t<uint64_t>{ k });
    return out;
}

static uint64_t faults(const scalar& g, const affine& a, const scalar& k) NOEXCEPT
{
    jacobian out{};
    return accessor::multiply(out, accessor::scalars_t<uint64_t>{ g }, a,
        accessor::scalars_t<uint64_t>{ k });
}

static jacobian complete(const scalar& g, const affine& a, const scalar& k) NOEXCEPT
{
    jacobian out{};
    accessor::multiply_complete(out, g, a, k);
    return out;
}

constexpr affine g1{ decode(gx), decode(gy) };
constexpr affine g2{ decode(g2x), decode(g2y) };
constexpr affine g3{ decode(g3x), decode(g3y) };
constexpr affine p1{ decode(px), decode(py) };

// tables
// ----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(secp256k1_algorithm_multiply__tables__odd_multiples__expected)
{
    BOOST_CHECK(is_point(entry(0, false), gx, gy));
    BOOST_CHECK(is_point(entry(1, false), g3x, g3y));
    BOOST_CHECK(is_point(entry(3, false), g7x, g7y));
    BOOST_CHECK(is_point(entry(63, false), g127x, g127y));
    BOOST_CHECK(is_point(entry(511, false), g1023x, g1023y));
    BOOST_CHECK(is_point(entry(0, true), gx_beta, gy));
}

BOOST_AUTO_TEST_CASE(secp256k1_algorithm_multiply__tables__computed_multiples__expected)
{
    BOOST_CHECK(is_multiple(0));
    BOOST_CHECK(is_multiple(511));
    BOOST_CHECK(is_multiple(512));
    BOOST_CHECK(is_multiple(4095));
    BOOST_CHECK(is_multiple(7680));
    BOOST_CHECK(is_multiple(8191));
    BOOST_CHECK(is_mapped(0));
    BOOST_CHECK(is_mapped(511));
    BOOST_CHECK(is_mapped(512));
    BOOST_CHECK(is_mapped(8191));
}

// multiply
// ----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(secp256k1_algorithm_multiply__multiply__values__expected)
{
    BOOST_CHECK(is_point(product(number(sample), p1, number(first)), sum1x, sum1y));
    BOOST_CHECK(is_point(product(number(order_minus_one), p1, number(order_minus_two)), sum2x, sum2y));
    BOOST_CHECK(is_point(product(number(one_value), p1, number(zero_value)), gx, gy));
    BOOST_CHECK(is_point(product(number(zero_value), g1, number(one_value)), gx, gy));
    BOOST_CHECK_EQUAL(faults(number(sample), p1, number(first)), 0_u64);
    BOOST_CHECK_EQUAL(faults(number(order_minus_one), p1, number(order_minus_two)), 0_u64);
}

BOOST_AUTO_TEST_CASE(secp256k1_algorithm_multiply__multiply_complete__values__expected)
{
    BOOST_CHECK(is_point(complete(number(sample), p1, number(first)), sum1x, sum1y));
    BOOST_CHECK(is_point(complete(number(order_minus_one), p1, number(order_minus_two)), sum2x, sum2y));
    BOOST_CHECK(is_point(complete(number(one_value), p1, number(zero_value)), gx, gy));
    BOOST_CHECK(is_point(complete(number(one_value), g1, number(one_value)), g2x, g2y));
    BOOST_CHECK(f::any(complete(number(zero_value), p1, number(zero_value)).infinity));
    BOOST_CHECK(f::any(complete(number(one_value), g1, number(order_minus_one)).infinity));
}

BOOST_AUTO_TEST_CASE(secp256k1_algorithm_multiply__multiply__zero__infinity)
{
    BOOST_CHECK_EQUAL(faults(number(zero_value), p1, number(zero_value)), 0_u64);
    BOOST_CHECK(f::any(product(number(zero_value), p1, number(zero_value)).infinity));
}

BOOST_AUTO_TEST_CASE(secp256k1_algorithm_multiply__multiply__exceptional__faults)
{
    BOOST_CHECK(f::any(faults(number(one_value), g1, number(one_value))));
    BOOST_CHECK(f::any(faults(number(one_value), g1, number(order_minus_one))));
}

// linear
// ----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(secp256k1_algorithm_multiply__linear__point__matches_unit_scalar)
{
    affine sum{}, expected{};
    BOOST_REQUIRE(accessor::linear(sum, number(sample), p1));
    BOOST_REQUIRE(accessor::linear(expected, number(sample), p1, number(one_value)));
    BOOST_CHECK(sum.x == expected.x && sum.y == expected.y);
}

BOOST_AUTO_TEST_CASE(secp256k1_algorithm_multiply__linear__point_zero__point)
{
    affine sum{};
    BOOST_REQUIRE(accessor::linear(sum, number(zero_value), p1));
    BOOST_CHECK(is_point(sum, px, py));
}

BOOST_AUTO_TEST_CASE(secp256k1_algorithm_multiply__linear__point_generator__doubled)
{
    affine sum{};
    BOOST_REQUIRE(accessor::linear(sum, number(one_value), g1));
    BOOST_CHECK(is_point(sum, g2x, g2y));
}

BOOST_AUTO_TEST_CASE(secp256k1_algorithm_multiply__linear__point_negated__infinity)
{
    affine sum{};
    BOOST_CHECK(!accessor::linear(sum, number(order_minus_one), g1));
}

// comb
// ----------------------------------------------------------------------------

static bool is_same(const affine& left, const affine& right) NOEXCEPT
{
    return encode(left.x) == encode(right.x) &&
        encode(left.y) == encode(right.y);
}

static affine multiple(const scalar& k) NOEXCEPT
{
    affine out{};
    accessor::to_affine(out, complete(k, g1, {}));
    return out;
}

static bool is_comb(size_t window, size_t entry) NOEXCEPT
{
    scalar k{};
    const auto bit = window * accessor::comb_bits;
    k[bit / 64] = uint64_t{ add1(entry) } << (bit % 64);

    affine positive{}, negative{};
    accessor::lookup_comb(positive, window, entry, false);
    accessor::lookup_comb(negative, window, entry, true);
    const auto expected = multiple(k);
    return is_same(positive, expected) &&
        encode(negative.x) == encode(expected.x) &&
        encode(negative.y) != encode(expected.y);
}

static bool is_comb_product(const scalar& k) NOEXCEPT
{
    uint64_t faults{};
    jacobian sum{};
    sum.infinity = max_uint64;
    accessor::add_comb(sum, k, faults);
    if (is_nonzero(faults))
        return false;

    const auto expected = complete(k, g1, {});
    if (f::any(sum.infinity) || f::any(expected.infinity))
        return f::any(sum.infinity) && f::any(expected.infinity);

    affine out{};
    accessor::to_affine(out, sum);
    return is_same(out, multiple(k));
}

static bool is_secret_product(const scalar& k, const scalar& m) NOEXCEPT
{
    affine out{};
    accessor::secret_multiply(out, k, m);
    return is_same(out, multiple(k));
}

BOOST_AUTO_TEST_CASE(secp256k1_algorithm_multiply__comb__entries__expected)
{
    BOOST_CHECK(is_comb(0, 0));
    BOOST_CHECK(is_comb(0, 1));
    BOOST_CHECK(is_comb(0, 31));
    BOOST_CHECK(is_comb(1, 0));
    BOOST_CHECK(is_comb(14, 31));
    BOOST_CHECK(is_comb(15, 0));
    BOOST_CHECK(is_comb(29, 17));
    BOOST_CHECK(is_comb(30, 5));
    BOOST_CHECK(is_comb(42, 0));
    BOOST_CHECK(is_comb(42, 7));
}

BOOST_AUTO_TEST_CASE(secp256k1_algorithm_multiply__add_comb__values__expected)
{
    constexpr scalar carried{ max_uint64, max_uint64, max_uint64, 0x0fffffffffffffff };
    BOOST_CHECK(is_comb_product(number(zero_value)));
    BOOST_CHECK(is_comb_product(number(one_value)));
    BOOST_CHECK(is_comb_product(number(sample)));
    BOOST_CHECK(is_comb_product(number(first)));
    BOOST_CHECK(is_comb_product(number(order_minus_one)));
    BOOST_CHECK(is_comb_product(number(order_minus_two)));
    BOOST_CHECK(is_comb_product(carried));
}

BOOST_AUTO_TEST_CASE(secp256k1_algorithm_multiply__secret_multiply__generator__expected)
{
    BOOST_CHECK(is_secret_product(number(sample), number(first)));
    BOOST_CHECK(is_secret_product(number(one_value), number(order_minus_one)));
    BOOST_CHECK(is_secret_product(number(order_minus_one), number(one_value)));
    BOOST_CHECK(is_secret_product(number(sample), number(sample)));
}

// naf
// ----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(secp256k1_algorithm_multiply__naf__zero__empty)
{
    accessor::naf_t digits{};
    BOOST_CHECK_EQUAL(accessor::naf<5>(digits, scalar{}), 0u);
    BOOST_CHECK_EQUAL(std::count(digits.begin(), digits.end(), 0), 130);
}

BOOST_AUTO_TEST_CASE(secp256k1_algorithm_multiply__naf__ones__carried)
{
    accessor::naf_t digits{};
    BOOST_CHECK_EQUAL(accessor::naf<5>(digits, scalar{ max_uint64, max_uint64, 0, 0 }), 129u);
    BOOST_CHECK_EQUAL(digits[0], -1);
    BOOST_CHECK_EQUAL(digits[128], 1);
    BOOST_CHECK_EQUAL(std::count(digits.begin(), digits.end(), 0), 128);
}

BOOST_AUTO_TEST_CASE(secp256k1_algorithm_multiply__naf__window__expected)
{
    accessor::naf_t digits{};
    BOOST_CHECK_EQUAL(accessor::naf<10>(digits, scalar{ 0x1003ff, 0, 0, 0 }), 21u);
    BOOST_CHECK_EQUAL(digits[0], 1023);
    BOOST_CHECK_EQUAL(digits[20], 1);
    BOOST_CHECK_EQUAL(std::count(digits.begin(), digits.end(), 0), 128);
}

// multiscalar
// ----------------------------------------------------------------------------

using terms = std_vector<accessor::term_t>;

constexpr scalar small1{ 0x123456789abcdef0, 0x0fedcba987654321, 0, 0 };
constexpr scalar small2{ 0xfedcba9876543210, 0xffffffffffffffff, 3, 0 };
constexpr affine g1_negated{ decode(gx), decode(gy_negated) };

static jacobian sum(const terms& in) NOEXCEPT
{
    jacobian out{};
    accessor::multiply(out, in);
    return out;
}

static bool is_equal(const jacobian& a, const jacobian& b) NOEXCEPT
{
    affine left{}, right{};
    accessor::to_affine(left, a);
    accessor::to_affine(right, b);
    return encode(left.x) == encode(right.x) &&
        encode(left.y) == encode(right.y);
}

BOOST_AUTO_TEST_CASE(secp256k1_algorithm_multiply__multiply__terms__expected)
{
    BOOST_CHECK(is_point(sum({ { g1, scalar{ 1 } } }), gx, gy));
    BOOST_CHECK(is_point(sum({ { g1, scalar{ 1 } }, { g1, scalar{ 1 } } }), g2x, g2y));
    BOOST_CHECK(is_point(sum({ { g1, scalar{ 3 } }, { g2, scalar{ 2 } } }), g7x, g7y));
    BOOST_CHECK(is_equal(sum({ { g1, small1 }, { p1, small2 } }), complete(small1, p1, small2)));
    BOOST_CHECK(f::any(sum({ { g1, scalar{ 1 } }, { g1_negated, scalar{ 1 } } }).infinity));
    BOOST_CHECK(f::any(sum({}).infinity));
}

// lanes
// ----------------------------------------------------------------------------

template <typename xWord>
using xfield = accessor::field_t<xWord>;
template <typename xWord>
using xaffine = accessor::affine_t<xWord>;
template <typename xWord>
using xjacobian = accessor::jacobian_t<xWord>;

template <typename xWord>
constexpr auto lanes = capacity<xWord, uint64_t>;

using fields = std_array<field, 8>;
using affines = std_array<affine, 8>;
using scalars = std_array<scalar, 8>;
using masks = std_array<uint64_t, 8>;

[[maybe_unused]] static const scalars left_scalars
{
    number(sample), number(order_minus_one), number(one_value),
    number(zero_value), number(first), number(zero_value), number(second),
    number(one_value)
};

[[maybe_unused]] static const affines points
{
    p1, p1, p1, g1, g2, p1, g3, g1
};

[[maybe_unused]] static const scalars right_scalars
{
    number(first), number(order_minus_two), number(zero_value),
    number(one_value), number(second), number(zero_value), number(sample),
    number(order_minus_one)
};

template <typename xWord>
static xWord pack(const masks& in) NOEXCEPT
{
    if constexpr (lanes<xWord> == 8)
        return f::set<xWord>(in[0], in[1], in[2], in[3], in[4], in[5], in[6], in[7]);
    else if constexpr (lanes<xWord> == 4)
        return f::set<xWord>(in[0], in[1], in[2], in[3]);
    else
        return f::set<xWord>(in[0], in[1]);
}

template <typename xWord, size_t Limb>
static xWord pack_limb(const fields& in) NOEXCEPT
{
    const masks limbs
    {
        in[0][Limb], in[1][Limb], in[2][Limb], in[3][Limb],
        in[4][Limb], in[5][Limb], in[6][Limb], in[7][Limb]
    };

    return pack<xWord>(limbs);
}

template <typename xWord>
static xfield<xWord> pack(const fields& in) NOEXCEPT
{
    return
    {
        pack_limb<xWord, 0>(in),
        pack_limb<xWord, 1>(in),
        pack_limb<xWord, 2>(in),
        pack_limb<xWord, 3>(in),
        pack_limb<xWord, 4>(in)
    };
}

template <typename xWord>
static xaffine<xWord> pack(const affines& in) NOEXCEPT
{
    const fields x
    {
        in[0].x, in[1].x, in[2].x, in[3].x, in[4].x, in[5].x, in[6].x, in[7].x
    };

    const fields y
    {
        in[0].y, in[1].y, in[2].y, in[3].y, in[4].y, in[5].y, in[6].y, in[7].y
    };

    return { pack<xWord>(x), pack<xWord>(y) };
}

template <typename xWord>
static accessor::scalars_t<xWord> pack(const scalars& in) NOEXCEPT
{
    if constexpr (lanes<xWord> == 8)
        return { in[0], in[1], in[2], in[3], in[4], in[5], in[6], in[7] };
    else if constexpr (lanes<xWord> == 4)
        return { in[0], in[1], in[2], in[3] };
    else
        return { in[0], in[1] };
}

template <size_t Lane, typename xWord>
static jacobian unpack(const xjacobian<xWord>& in) NOEXCEPT
{
    return
    {
        {
            f::get<uint64_t, Lane>(in.x[0]),
            f::get<uint64_t, Lane>(in.x[1]),
            f::get<uint64_t, Lane>(in.x[2]),
            f::get<uint64_t, Lane>(in.x[3]),
            f::get<uint64_t, Lane>(in.x[4])
        },
        {
            f::get<uint64_t, Lane>(in.y[0]),
            f::get<uint64_t, Lane>(in.y[1]),
            f::get<uint64_t, Lane>(in.y[2]),
            f::get<uint64_t, Lane>(in.y[3]),
            f::get<uint64_t, Lane>(in.y[4])
        },
        {
            f::get<uint64_t, Lane>(in.z[0]),
            f::get<uint64_t, Lane>(in.z[1]),
            f::get<uint64_t, Lane>(in.z[2]),
            f::get<uint64_t, Lane>(in.z[3]),
            f::get<uint64_t, Lane>(in.z[4])
        },
        f::get<uint64_t, Lane>(in.infinity)
    };
}

template <size_t Lane, typename xWord>
static void check_lane(const xjacobian<xWord>& out, xWord lane_faults)
{
    if (is_nonzero(f::get<uint64_t, Lane>(lane_faults)))
        return;

    const auto sum = unpack<Lane>(out);
    const auto expected = complete(left_scalars[Lane], points[Lane],
        right_scalars[Lane]);
    BOOST_CHECK_EQUAL(f::any(sum.infinity), f::any(expected.infinity));

    if (!f::any(expected.infinity))
    {
        affine actual{}, reference{};
        accessor::to_affine(actual, sum);
        accessor::to_affine(reference, expected);
        BOOST_CHECK_EQUAL(encode(actual.x), encode(reference.x));
        BOOST_CHECK_EQUAL(encode(actual.y), encode(reference.y));
    }
}

template <typename xWord>
static void check_multiply()
{
    if constexpr (have<xWord>)
    {
        xjacobian<xWord> out{};
        const auto lane_faults = accessor::multiply(out,
            pack<xWord>(left_scalars), pack<xWord>(points),
            pack<xWord>(right_scalars));

        BOOST_CHECK_EQUAL((f::get<uint64_t, 0>(lane_faults)), 0_u64);
        BOOST_CHECK_EQUAL((f::get<uint64_t, 1>(lane_faults)), 0_u64);
        check_lane<0>(out, lane_faults);
        check_lane<1>(out, lane_faults);

        if constexpr (lanes<xWord> >= 4)
        {
            check_lane<2>(out, lane_faults);
            check_lane<3>(out, lane_faults);
        }

        if constexpr (lanes<xWord> >= 8)
        {
            check_lane<4>(out, lane_faults);
            check_lane<5>(out, lane_faults);
            check_lane<6>(out, lane_faults);
            check_lane<7>(out, lane_faults);
        }
    }
}

BOOST_AUTO_TEST_CASE(secp256k1_algorithm_multiply__multiply__lanes__match_integral)
{
    check_multiply<xint128_t>();
    check_multiply<xint256_t>();
    check_multiply<xint512_t>();
}

BOOST_AUTO_TEST_SUITE_END()
