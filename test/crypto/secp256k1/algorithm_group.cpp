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

BOOST_AUTO_TEST_SUITE(secp256k1_algorithm_group_tests)

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
    using bytes_t = algorithm::bytes_t;
    using algorithm::generator;
    using algorithm::double_;
    using algorithm::add;
    using algorithm::add_complete;
    using algorithm::negate;
    using algorithm::endomorphism;
    using algorithm::to_jacobian;
    using algorithm::to_affine;
    using algorithm::lift;
    using algorithm::is_on_curve;
    using algorithm::from_bytes;
    using algorithm::to_bytes;
    using algorithm::normalize;
};

using field = accessor::field_t<uint64_t>;
using affine = accessor::affine_t<uint64_t>;
using jacobian = accessor::jacobian_t<uint64_t>;
using bytes = accessor::bytes_t;

// vectors
// ----------------------------------------------------------------------------

constexpr auto zero_value = base16_array("0000000000000000000000000000000000000000000000000000000000000000");
constexpr auto gx = base16_array("79be667ef9dcbbac55a06295ce870b07029bfcdb2dce28d959f2815b16f81798");
constexpr auto gy = base16_array("483ada7726a3c4655da4fbfc0e1108a8fd17b448a68554199c47d08ffb10d4b8");
constexpr auto gy_negated = base16_array("b7c52588d95c3b9aa25b0403f1eef75702e84bb7597aabe663b82f6f04ef2777");
constexpr auto gx_beta = base16_array("bcace2e99da01887ab0102b696902325872844067f15e98da7bba04400b88fcb");
constexpr auto g2x = base16_array("c6047f9441ed7d6d3045406e95c07cd85c778e4b8cef3ca7abac09b95c709ee5");
constexpr auto g2y = base16_array("1ae168fea63dc339a3c58419466ceaeef7f632653266d0e1236431a950cfe52a");
constexpr auto g2y_negated = base16_array("e51e970159c23cc65c3a7be6b99315110809cd9acd992f1edc9bce55af301705");
constexpr auto g3x = base16_array("f9308a019258c31049344f85f89d5229b531c845836f99b08601f113bce036f9");
constexpr auto g3y = base16_array("388f7b0f632de8140fe337e62a37f3566500a99934c2231b6cb9fd7584b8e672");
constexpr auto g4x = base16_array("e493dbf1c10d80f3581e4904930b1404cc6c13900ee0758474fa94abe8c4cd13");
constexpr auto g4y = base16_array("51ed993ea0d455b75642e2098ea51448d967ae33bfbdfe40cfe97bdc47739922");
constexpr auto g7x = base16_array("5cbdf0646e5db4eaa398f365f2ea7a0e3d419b7e0330e39ce92bddedcac4f9bc");
constexpr auto g7y = base16_array("6aebca40ba255960a3178d6d861a54dba813d0b813fde7b5a5082628087264da");
constexpr auto px = base16_array("74e8586b1604b6409cb198eef8a40ef97294fcfb38f770e1c7b111163f57c99b");
constexpr auto py = base16_array("88a13c2536d83144543c105147aeb62795c99d6d17577ffdbe4fba047946c4b1");
constexpr auto pgx = base16_array("b27dd07a360099a5f14cd68d4a1c864783e589868d548aadea98158d221ff482");
constexpr auto pgy = base16_array("59f8254f27f5871fa426941f6a4ef202370d29914790f899e3b7240187b8e367");
constexpr auto p2x = base16_array("166770ee2e79e63b2033d97983f17b1b63b76406bba0b10086f4708ad06d25bb");
constexpr auto p2y = base16_array("53166aa1f25035f59e59ccf1bfdaf51787419aa6d1188ef2ed6001fea37d7940");
constexpr auto pg2x = base16_array("0ef691a1f5fdfd995835da38f80cab73f13dd99f5df94375c48a7cdc6e4f4f38");
constexpr auto pg2y = base16_array("1c928fe5a36200a890ab9aa59db67d6cee4b3d90f9e4ae2afd6e72a258b2fddd");

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

constexpr affine point(const bytes& x, const bytes& y) NOEXCEPT
{
    return { decode(x), decode(y) };
}

constexpr jacobian projective(const affine& a) NOEXCEPT
{
    jacobian out{};
    accessor::to_jacobian(out, a);
    return out;
}

constexpr affine flat(const jacobian& a) NOEXCEPT
{
    affine out{};
    accessor::to_affine(out, a);
    return out;
}

constexpr jacobian twice(const jacobian& a) NOEXCEPT
{
    jacobian out{};
    accessor::double_(out, a);
    return out;
}

constexpr jacobian sum(const jacobian& a, const affine& b) NOEXCEPT
{
    jacobian out{};
    accessor::add(out, a, b);
    return out;
}

constexpr jacobian sum(const jacobian& a, const jacobian& b) NOEXCEPT
{
    jacobian out{};
    accessor::add(out, a, b);
    return out;
}

constexpr uint64_t uncomputed(const jacobian& a, const affine& b) NOEXCEPT
{
    jacobian out{};
    return accessor::add(out, a, b);
}

constexpr uint64_t uncomputed(const jacobian& a, const jacobian& b) NOEXCEPT
{
    jacobian out{};
    return accessor::add(out, a, b);
}

constexpr jacobian complete(const jacobian& a, const affine& b) NOEXCEPT
{
    jacobian out{};
    accessor::add_complete(out, a, b);
    return out;
}

constexpr jacobian complete(const jacobian& a, const jacobian& b) NOEXCEPT
{
    jacobian out{};
    accessor::add_complete(out, a, b);
    return out;
}

constexpr affine negated(const affine& a) NOEXCEPT
{
    affine out{};
    accessor::negate(out, a);
    return out;
}

constexpr jacobian negated(const jacobian& a) NOEXCEPT
{
    jacobian out{};
    accessor::negate(out, a);
    return out;
}

constexpr affine mapped(const affine& a) NOEXCEPT
{
    affine out{};
    accessor::endomorphism(out, a);
    return out;
}

constexpr affine lifted(const bytes& x, bool odd) NOEXCEPT
{
    affine out{};
    accessor::lift(out, decode(x), odd ? max_uint64 : 0_u64);
    return out;
}

constexpr bool is_liftable(const bytes& x) NOEXCEPT
{
    affine out{};
    return f::any(accessor::lift(out, decode(x), 0_u64));
}

constexpr bool is_point(const affine& a, const bytes& x, const bytes& y) NOEXCEPT
{
    affine normal{ a };
    accessor::normalize(normal.x);
    accessor::normalize(normal.y);
    return encode(normal.x) == x && encode(normal.y) == y;
}

constexpr affine g1 = point(gx, gy);
constexpr affine g2 = point(g2x, g2y);
constexpr affine g3 = point(g3x, g3y);
constexpr affine g4 = point(g4x, g4y);
constexpr affine p1 = point(px, py);
constexpr jacobian infinity{ {}, {}, {}, max_uint64 };

static const jacobian g1j = projective(g1);
static const jacobian g2j = twice(g1j);
static const jacobian g3j = sum(g2j, g1);
static const jacobian g4j = twice(g2j);
static const jacobian p1j = projective(p1);

// constants
// ----------------------------------------------------------------------------

static_assert(accessor::generator.x == g1.x);
static_assert(accessor::generator.y == g1.y);
static_assert(f::any(accessor::is_on_curve(accessor::generator)));
static_assert(f::any(accessor::is_on_curve(p1)));
static_assert(!f::any(accessor::is_on_curve(affine{ g1.x, g2.y })));
static_assert(is_point(mapped(g1), gx_beta, gy));

// double/add
// ----------------------------------------------------------------------------

static_assert(is_point(flat(twice(projective(g1))), g2x, g2y));
static_assert(is_point(flat(sum(twice(projective(g1)), g1)), g3x, g3y));
static_assert(is_point(flat(sum(twice(twice(projective(g1))), sum(twice(projective(g1)), g1))), g7x, g7y));

BOOST_AUTO_TEST_CASE(secp256k1_algorithm_group__double__points__expected)
{
    BOOST_CHECK(is_point(flat(twice(projective(g1))), g2x, g2y));
    BOOST_CHECK(is_point(flat(twice(twice(projective(g1)))), g4x, g4y));
    BOOST_CHECK(is_point(flat(twice(projective(p1))), p2x, p2y));
}

BOOST_AUTO_TEST_CASE(secp256k1_algorithm_group__add__mixed__expected)
{
    BOOST_CHECK(is_point(flat(sum(g2j, g1)), g3x, g3y));
    BOOST_CHECK(is_point(flat(sum(twice(g2j), g3)), g7x, g7y));
    BOOST_CHECK(is_point(flat(sum(projective(p1), g1)), pgx, pgy));
    BOOST_CHECK(is_point(flat(sum(projective(p1), g2)), pg2x, pg2y));
    BOOST_CHECK(!f::any(uncomputed(g2j, g1)));
}

BOOST_AUTO_TEST_CASE(secp256k1_algorithm_group__add__jacobian__expected)
{
    BOOST_CHECK(is_point(flat(sum(g4j, g3j)), g7x, g7y));
    BOOST_CHECK(is_point(flat(sum(g3j, projective(g4))), g7x, g7y));
    BOOST_CHECK(is_point(flat(sum(g2j, projective(g1))), g3x, g3y));
    BOOST_CHECK(!f::any(uncomputed(g4j, g3j)));
}

// exceptions
// ----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(secp256k1_algorithm_group__add__exceptional__uncomputed)
{
    BOOST_CHECK(f::any(uncomputed(g1j, g1)));
    BOOST_CHECK(f::any(uncomputed(g1j, negated(g1))));
    BOOST_CHECK(f::any(uncomputed(infinity, g1)));
    BOOST_CHECK(f::any(uncomputed(g2j, twice(g1j))));
    BOOST_CHECK(f::any(uncomputed(g2j, negated(g2j))));
    BOOST_CHECK(f::any(uncomputed(infinity, g2j)));
    BOOST_CHECK(f::any(uncomputed(g2j, infinity)));
}

BOOST_AUTO_TEST_CASE(secp256k1_algorithm_group__add_complete__exceptional__expected)
{
    BOOST_CHECK(is_point(flat(complete(g1j, g1)), g2x, g2y));
    BOOST_CHECK(f::any(complete(g1j, negated(g1)).infinity));
    BOOST_CHECK(is_point(flat(complete(infinity, g1)), gx, gy));
    BOOST_CHECK(is_point(flat(complete(g2j, twice(g1j))), g4x, g4y));
    BOOST_CHECK(f::any(complete(g2j, negated(g2j)).infinity));
    BOOST_CHECK(is_point(flat(complete(infinity, g2j)), g2x, g2y));
    BOOST_CHECK(is_point(flat(complete(g2j, infinity)), g2x, g2y));
    BOOST_CHECK(is_point(flat(complete(g2j, g1)), g3x, g3y));
    BOOST_CHECK(f::any(complete(infinity, infinity).infinity));
}

// negate/endomorphism
// ----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(secp256k1_algorithm_group__negate__generator__expected)
{
    BOOST_CHECK(is_point(negated(g1), gx, gy_negated));
    BOOST_CHECK(is_point(flat(negated(projective(g2))), g2x, g2y_negated));
    BOOST_CHECK(is_point(mapped(g1), gx_beta, gy));
}

// conversion
// ----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(secp256k1_algorithm_group__to_affine__batch__expected)
{
    const std_array<jacobian, 4> points{ g2j, g3j, g4j, sum(g4j, g3j) };
    std_array<affine, 4> out{};
    accessor::to_affine(out, points);
    BOOST_CHECK(is_point(out[0], g2x, g2y));
    BOOST_CHECK(is_point(out[1], g3x, g3y));
    BOOST_CHECK(is_point(out[2], g4x, g4y));
    BOOST_CHECK(is_point(out[3], g7x, g7y));
}

static_assert(is_point(lifted(g2x, false), g2x, g2y));
static_assert(is_point(lifted(g2x, true), g2x, g2y_negated));
static_assert(!is_liftable(zero_value));

BOOST_AUTO_TEST_CASE(secp256k1_algorithm_group__lift__parity__expected)
{
    BOOST_CHECK(is_point(lifted(g2x, false), g2x, g2y));
    BOOST_CHECK(is_point(lifted(g2x, true), g2x, g2y_negated));
    BOOST_CHECK(is_point(lifted(gx, false), gx, gy));
    BOOST_CHECK(is_point(lifted(px, true), px, py));
    BOOST_CHECK(is_liftable(gx));
    BOOST_CHECK(!is_liftable(zero_value));
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

using affines = std_array<affine, 8>;
using jacobians = std_array<jacobian, 8>;
using masks = std_array<uint64_t, 8>;

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
static xWord pack_limb(const std_array<field, 8>& in) NOEXCEPT
{
    return pack<xWord>(masks{ in[0][Limb], in[1][Limb], in[2][Limb], in[3][Limb], in[4][Limb], in[5][Limb], in[6][Limb], in[7][Limb] });
}

template <typename xWord>
static xfield<xWord> pack(const std_array<field, 8>& in) NOEXCEPT
{
    return { pack_limb<xWord, 0>(in), pack_limb<xWord, 1>(in), pack_limb<xWord, 2>(in), pack_limb<xWord, 3>(in), pack_limb<xWord, 4>(in) };
}

template <typename xWord>
static xaffine<xWord> pack(const affines& in) NOEXCEPT
{
    const std_array<field, 8> x{ in[0].x, in[1].x, in[2].x, in[3].x, in[4].x, in[5].x, in[6].x, in[7].x };
    const std_array<field, 8> y{ in[0].y, in[1].y, in[2].y, in[3].y, in[4].y, in[5].y, in[6].y, in[7].y };
    return { pack<xWord>(x), pack<xWord>(y) };
}

template <typename xWord>
static xjacobian<xWord> pack(const jacobians& in) NOEXCEPT
{
    const std_array<field, 8> x{ in[0].x, in[1].x, in[2].x, in[3].x, in[4].x, in[5].x, in[6].x, in[7].x };
    const std_array<field, 8> y{ in[0].y, in[1].y, in[2].y, in[3].y, in[4].y, in[5].y, in[6].y, in[7].y };
    const std_array<field, 8> z{ in[0].z, in[1].z, in[2].z, in[3].z, in[4].z, in[5].z, in[6].z, in[7].z };
    const masks infinities{ in[0].infinity, in[1].infinity, in[2].infinity, in[3].infinity, in[4].infinity, in[5].infinity, in[6].infinity, in[7].infinity };
    return { pack<xWord>(x), pack<xWord>(y), pack<xWord>(z), pack<xWord>(infinities) };
}

template <size_t Lane, typename xWord>
static field unpack(const xfield<xWord>& in) NOEXCEPT
{
    return { f::get<uint64_t, Lane>(in[0]), f::get<uint64_t, Lane>(in[1]), f::get<uint64_t, Lane>(in[2]), f::get<uint64_t, Lane>(in[3]), f::get<uint64_t, Lane>(in[4]) };
}

template <size_t Lane, typename xWord>
static jacobian unpack(const xjacobian<xWord>& in) NOEXCEPT
{
    return { unpack<Lane>(in.x), unpack<Lane>(in.y), unpack<Lane>(in.z), f::get<uint64_t, Lane>(in.infinity) };
}

template <size_t Lane, typename xWord>
static affine unpack(const xaffine<xWord>& in) NOEXCEPT
{
    return { unpack<Lane>(in.x), unpack<Lane>(in.y) };
}

template <size_t Lane, typename xWord>
static void check_lane(const xjacobian<xWord>& out, xWord uncomputed, const jacobians& expected, const masks& expected_uncomputed)
{
    BOOST_CHECK_EQUAL((f::get<uint64_t, Lane>(uncomputed)), expected_uncomputed[Lane]);
    if (!f::any(expected_uncomputed[Lane]) && !f::any(expected[Lane].infinity))
    {
        BOOST_CHECK_EQUAL(flat(unpack<Lane>(out)).x, flat(expected[Lane]).x);
        BOOST_CHECK_EQUAL(flat(unpack<Lane>(out)).y, flat(expected[Lane]).y);
    }
}

template <typename xWord>
static void check_lanes(const xjacobian<xWord>& out, xWord uncomputed, const jacobians& expected, const masks& expected_uncomputed)
{
    check_lane<0>(out, uncomputed, expected, expected_uncomputed);
    check_lane<1>(out, uncomputed, expected, expected_uncomputed);

    if constexpr (lanes<xWord> >= 4)
    {
        check_lane<2>(out, uncomputed, expected, expected_uncomputed);
        check_lane<3>(out, uncomputed, expected, expected_uncomputed);
    }

    if constexpr (lanes<xWord> >= 8)
    {
        check_lane<4>(out, uncomputed, expected, expected_uncomputed);
        check_lane<5>(out, uncomputed, expected, expected_uncomputed);
        check_lane<6>(out, uncomputed, expected, expected_uncomputed);
        check_lane<7>(out, uncomputed, expected, expected_uncomputed);
    }
}


static const jacobians mixed_left{ g2j, g1j, infinity, g1j, p1j, p1j, g4j, g3j };
static const affines mixed_right{ g1, g1, g1, negated(g1), g1, g2, g3, g1 };
static const jacobians jacobian_left{ g2j, g2j, infinity, g2j, g4j, p1j, g1j, g3j };
static const jacobians jacobian_right{ projective(g1), g2j, g1j, negated(g2j), g3j, g2j, infinity, g4j };
static const jacobians doubled_in{ g1j, infinity, g2j, p1j, g3j, g4j, negated(g1j), sum(g4j, g3j) };

template <typename xWord>
static void check_mixed()
{
    if constexpr (have<xWord>)
    {
        xjacobian<xWord> out{};
        const auto uncomputed_lanes = accessor::add(out, pack<xWord>(mixed_left), pack<xWord>(mixed_right));
        const jacobians expected{ sum(mixed_left[0], mixed_right[0]), sum(mixed_left[1], mixed_right[1]), sum(mixed_left[2], mixed_right[2]), sum(mixed_left[3], mixed_right[3]), sum(mixed_left[4], mixed_right[4]), sum(mixed_left[5], mixed_right[5]), sum(mixed_left[6], mixed_right[6]), sum(mixed_left[7], mixed_right[7]) };
        const masks expected_uncomputed{ uncomputed(mixed_left[0], mixed_right[0]), uncomputed(mixed_left[1], mixed_right[1]), uncomputed(mixed_left[2], mixed_right[2]), uncomputed(mixed_left[3], mixed_right[3]), uncomputed(mixed_left[4], mixed_right[4]), uncomputed(mixed_left[5], mixed_right[5]), uncomputed(mixed_left[6], mixed_right[6]), uncomputed(mixed_left[7], mixed_right[7]) };
        check_lanes(out, uncomputed_lanes, expected, expected_uncomputed);
    }
}

template <typename xWord>
static void check_jacobian()
{
    if constexpr (have<xWord>)
    {
        xjacobian<xWord> out{};
        const auto uncomputed_lanes = accessor::add(out, pack<xWord>(jacobian_left), pack<xWord>(jacobian_right));
        const jacobians expected{ sum(jacobian_left[0], jacobian_right[0]), sum(jacobian_left[1], jacobian_right[1]), sum(jacobian_left[2], jacobian_right[2]), sum(jacobian_left[3], jacobian_right[3]), sum(jacobian_left[4], jacobian_right[4]), sum(jacobian_left[5], jacobian_right[5]), sum(jacobian_left[6], jacobian_right[6]), sum(jacobian_left[7], jacobian_right[7]) };
        const masks expected_uncomputed{ uncomputed(jacobian_left[0], jacobian_right[0]), uncomputed(jacobian_left[1], jacobian_right[1]), uncomputed(jacobian_left[2], jacobian_right[2]), uncomputed(jacobian_left[3], jacobian_right[3]), uncomputed(jacobian_left[4], jacobian_right[4]), uncomputed(jacobian_left[5], jacobian_right[5]), uncomputed(jacobian_left[6], jacobian_right[6]), uncomputed(jacobian_left[7], jacobian_right[7]) };
        check_lanes(out, uncomputed_lanes, expected, expected_uncomputed);
    }
}

template <typename xWord>
static void check_double()
{
    if constexpr (have<xWord>)
    {
        xjacobian<xWord> out{};
        accessor::double_(out, pack<xWord>(doubled_in));
        const jacobians expected{ twice(doubled_in[0]), twice(doubled_in[1]), twice(doubled_in[2]), twice(doubled_in[3]), twice(doubled_in[4]), twice(doubled_in[5]), twice(doubled_in[6]), twice(doubled_in[7]) };
        const masks infinities{ doubled_in[0].infinity, doubled_in[1].infinity, doubled_in[2].infinity, doubled_in[3].infinity, doubled_in[4].infinity, doubled_in[5].infinity, doubled_in[6].infinity, doubled_in[7].infinity };
        check_lanes(out, out.infinity, expected, infinities);
    }
}

template <typename xWord>
static void check_lift()
{
    if constexpr (have<xWord>)
    {
        const std_array<field, 8> xs{ decode(g2x), decode(g2x), decode(zero_value), decode(gx), decode(px), decode(g3x), decode(g7x), decode(g4x) };
        const masks odds{ 0, max_uint64, 0, 0, max_uint64, 0, 0, 0 };
        xaffine<xWord> out{};
        const auto valid = accessor::lift(out, pack<xWord>(xs), pack<xWord>(odds));
        BOOST_CHECK_EQUAL((f::get<uint64_t, 0>(valid)), max_uint64);
        BOOST_CHECK_EQUAL((f::get<uint64_t, 1>(valid)), max_uint64);
        BOOST_CHECK(is_point(unpack<0>(out), g2x, g2y));
        BOOST_CHECK(is_point(unpack<1>(out), g2x, g2y_negated));

        if constexpr (lanes<xWord> >= 4)
        {
            BOOST_CHECK_EQUAL((f::get<uint64_t, 2>(valid)), 0_u64);
            BOOST_CHECK_EQUAL((f::get<uint64_t, 3>(valid)), max_uint64);
            BOOST_CHECK(is_point(unpack<3>(out), gx, gy));
        }

        if constexpr (lanes<xWord> >= 8)
        {
            BOOST_CHECK(is_point(unpack<4>(out), px, py));
            BOOST_CHECK(is_point(unpack<5>(out), g3x, g3y));
            BOOST_CHECK(is_point(unpack<6>(out), g7x, g7y));
            BOOST_CHECK(is_point(unpack<7>(out), g4x, g4y));
        }
    }
}

template <typename xWord>
static void check_to_affine()
{
    if constexpr (have<xWord>)
    {
        const std_array<xjacobian<xWord>, 2> in{ pack<xWord>(mixed_left), pack<xWord>(doubled_in) };
        std_array<xaffine<xWord>, 2> out{};
        accessor::to_affine(out, in);
        BOOST_CHECK_EQUAL(unpack<0>(out[0]).x, flat(mixed_left[0]).x);
        BOOST_CHECK_EQUAL(unpack<0>(out[1]).y, flat(doubled_in[0]).y);

        if constexpr (lanes<xWord> >= 4)
        {
            BOOST_CHECK_EQUAL(unpack<3>(out[0]).y, flat(mixed_left[3]).y);
            BOOST_CHECK_EQUAL(unpack<3>(out[1]).x, flat(doubled_in[3]).x);
        }
    }
}

BOOST_AUTO_TEST_CASE(secp256k1_algorithm_group__add_mixed__lanes__match_integral)
{
    check_mixed<xint128_t>();
    check_mixed<xint256_t>();
    check_mixed<xint512_t>();
}

BOOST_AUTO_TEST_CASE(secp256k1_algorithm_group__add_jacobian__lanes__match_integral)
{
    check_jacobian<xint128_t>();
    check_jacobian<xint256_t>();
    check_jacobian<xint512_t>();
}

BOOST_AUTO_TEST_CASE(secp256k1_algorithm_group__double__lanes__match_integral)
{
    check_double<xint128_t>();
    check_double<xint256_t>();
    check_double<xint512_t>();
}

BOOST_AUTO_TEST_CASE(secp256k1_algorithm_group__lift__lanes__expected)
{
    check_lift<xint128_t>();
    check_lift<xint256_t>();
    check_lift<xint512_t>();
}

BOOST_AUTO_TEST_CASE(secp256k1_algorithm_group__to_affine__lanes__match_integral)
{
    check_to_affine<xint128_t>();
    check_to_affine<xint256_t>();
    check_to_affine<xint512_t>();
}

BOOST_AUTO_TEST_SUITE_END()
