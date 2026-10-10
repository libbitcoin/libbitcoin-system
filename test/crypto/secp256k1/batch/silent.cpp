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
#include "../../../test.hpp"

BC_PUSH_WARNING(NO_USE_OF_SPAN)

BOOST_AUTO_TEST_SUITE(secp256k1_batch_silent_tests)

using namespace system::silent;

constexpr ec_secret scan_secret = base16_array("0f694e068028a717f8af6b9411f9a133dd3565258714cc226594b34db90c1f2c");
constexpr ec_secret spend_secret = base16_array("9d6ad855ce3417ef84e836892e5a56392bfba05fa5d97ccea30e266f540e08b3");
constexpr ec_compressed unlabeled_point = base16_array("024ac253c216532e961988e2a8ce266a447c894c781e52ef6cee902361db960004");
constexpr ec_compressed labeled_point = base16_array("0314bec14463d6c0181083d607fecfba67bb83f95915f6f247975ec566d5642ee8");

// compute
// ----------------------------------------------------------------------------

struct computed
{
    bool success{};
    std_vector<ec_compressed> out{};
    data_chunk valid{};
};

static batch::row_t to_row(const ec_compressed& sum,
    const ec_secret& hash) NOEXCEPT
{
    return { {}, sum, hash };
}

static ec_compressed product(const ec_compressed& sum,
    const ec_secret& hash) NOEXCEPT
{
    auto out = sum;
    return ec_multiply(out, hash) ? out : ec_compressed{};
}

static computed compute(const std_vector<batch::row_t>& rows) NOEXCEPT
{
    computed out{};
    const stopper cancel{};
    out.success = batch::compute(out.out, out.valid, cancel, { rows });
    return out;
}

static std_vector<batch::row_t> alternating(size_t count) NOEXCEPT
{
    std_vector<batch::row_t> rows(count);
    for (size_t row{}; row < count; ++row)
        rows[row] = is_even(row) ?
            to_row(unlabeled_point, spend_secret) :
            to_row(labeled_point, scan_secret);

    return rows;
}

static bool is_alternating(const computed& result) NOEXCEPT
{
    const auto even = product(unlabeled_point, spend_secret);
    const auto odd = product(labeled_point, scan_secret);
    for (size_t row{}; row < result.out.size(); ++row)
        if (is_zero(result.valid[row]) ||
            result.out[row] != (is_even(row) ? even : odd))
            return false;

    return true;
}

BOOST_AUTO_TEST_CASE(secp256k1_batch_silent__compute__empty__empty)
{
    const auto result = compute({});
    BOOST_REQUIRE(result.success);
    BOOST_REQUIRE(result.out.empty());
    BOOST_REQUIRE(result.valid.empty());
}

BOOST_AUTO_TEST_CASE(secp256k1_batch_silent__compute__runs__expected)
{
    const auto first = to_row(unlabeled_point, spend_secret);
    const auto second = to_row(labeled_point, scan_secret);
    const auto result = compute({ first, first, second, first });
    const auto expected_first = product(unlabeled_point, spend_secret);
    const auto expected_second = product(labeled_point, scan_secret);
    BOOST_REQUIRE(result.success);
    BOOST_REQUIRE_EQUAL(result.valid, (data_chunk{ 1, 1, 1, 1 }));
    BOOST_REQUIRE_EQUAL(result.out.at(0), expected_first);
    BOOST_REQUIRE_EQUAL(result.out.at(1), expected_first);
    BOOST_REQUIRE_EQUAL(result.out.at(2), expected_second);
    BOOST_REQUIRE_EQUAL(result.out.at(3), expected_first);
}

BOOST_AUTO_TEST_CASE(secp256k1_batch_silent__compute__zero_hash__invalid)
{
    const auto result = compute({ to_row(unlabeled_point, ec_secret{}), to_row(labeled_point, scan_secret) });
    BOOST_REQUIRE(result.success);
    BOOST_REQUIRE_EQUAL(result.valid, (data_chunk{ 0, 1 }));
    BOOST_REQUIRE_EQUAL(result.out.at(1), product(labeled_point, scan_secret));
}

BOOST_AUTO_TEST_CASE(secp256k1_batch_silent__compute__invalid_sum__invalid)
{
    const auto result = compute({ to_row(null_ec_compressed, spend_secret), to_row(labeled_point, scan_secret) });
    BOOST_REQUIRE(result.success);
    BOOST_REQUIRE_EQUAL(result.valid, (data_chunk{ 0, 1 }));
    BOOST_REQUIRE_EQUAL(result.out.at(1), product(labeled_point, scan_secret));
}

BOOST_AUTO_TEST_CASE(secp256k1_batch_silent__compute__chunks__expected)
{
    const auto result = compute(alternating(3000));
    BOOST_REQUIRE(result.success);
    BOOST_REQUIRE(is_alternating(result));
}

BOOST_AUTO_TEST_CASE(secp256k1_batch_silent__compute__device_rows__expected)
{
    const auto result = compute(alternating(70000));
    BOOST_REQUIRE(result.success);
    BOOST_REQUIRE(is_alternating(result));
}

BOOST_AUTO_TEST_SUITE_END()

BC_POP_WARNING()
