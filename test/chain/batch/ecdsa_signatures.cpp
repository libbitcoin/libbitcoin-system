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

BOOST_AUTO_TEST_SUITE(ecdsa_signatures_tests)

using namespace system::chain;

const hash_digest sighash_bad = base16_array
(
    "4242424242424242424242424242424242424242424242424242424242424242"
);
const hash_digest ecdsa_sighash = base16_array
(
    "504d68beac187dd0b259ddd6ed6d5d6348150b9b23ee6dfdb43e87f74dd3c547"
);
const ec_compressed ecdsa_key = base16_array
(
    "039cfcfe4a5d0efad27382e5d2b478eb398a8b691a66e01c878b600b5042b33166"
);
const ec_signature ecdsa_sig = base16_array
(
    "b434c7c720d63d71e1136d740df7ff636770ce59cb0389ae8cd24c0d4441f143"
    "01f4e1dbc36f32b4683faeecc8e4b2c6810da69e98fd783f1aad105636c3da08"
);

BOOST_AUTO_TEST_CASE(ecdsa_signatures__default__empty)
{
    const ecdsa_signatures accumulator{};
    BOOST_REQUIRE(accumulator.empty());
    BOOST_REQUIRE_EQUAL(accumulator.groups(), zero);
    BOOST_REQUIRE_EQUAL(accumulator.rows(), zero);
    BOOST_REQUIRE_EQUAL(accumulator.singles(), zero);
    BOOST_REQUIRE_EQUAL(accumulator.multisig_keys(), zero);
    BOOST_REQUIRE(accumulator.verify());
}

BOOST_AUTO_TEST_CASE(ecdsa_signatures__append_single__expected_counts)
{
    ecdsa_signatures accumulator{};
    BOOST_REQUIRE(accumulator.append(ecdsa_sighash, ecdsa_key, ecdsa_sig));
    BOOST_REQUIRE(!accumulator.empty());
    BOOST_REQUIRE_EQUAL(accumulator.groups(), 1u);
    BOOST_REQUIRE_EQUAL(accumulator.rows(), 1u);
    BOOST_REQUIRE_EQUAL(accumulator.singles(), 1u);
    BOOST_REQUIRE_EQUAL(accumulator.multisig_keys(), zero);
}

BOOST_AUTO_TEST_CASE(ecdsa_signatures__append_multisig__expected_counts)
{
    // 2 of 3 multisig: banded rows = m * (n - m + 1) = 4.
    const std::array<ec_compressed, 3> keys{ ecdsa_key, ecdsa_key, ecdsa_key };
    const std::array<ec_signature, 2> sigs{ ecdsa_sig, ecdsa_sig };

    ecdsa_signatures accumulator{};
    BOOST_REQUIRE(accumulator.append(ecdsa_sighash, keys, sigs));
    BOOST_REQUIRE_EQUAL(accumulator.groups(), 1u);
    BOOST_REQUIRE_EQUAL(accumulator.rows(), 4u);
    BOOST_REQUIRE_EQUAL(accumulator.singles(), zero);
    BOOST_REQUIRE_EQUAL(accumulator.multisig_keys(), 3u);
}

BOOST_AUTO_TEST_CASE(ecdsa_signatures__append_multisig_1of1__counted_as_multisig)
{
    // Classification follows the capture path, not the group shape.
    const std::array<ec_compressed, 1> keys{ ecdsa_key };
    const std::array<ec_signature, 1> sigs{ ecdsa_sig };

    ecdsa_signatures accumulator{};
    BOOST_REQUIRE(accumulator.append(ecdsa_sighash, keys, sigs));
    BOOST_REQUIRE_EQUAL(accumulator.groups(), 1u);
    BOOST_REQUIRE_EQUAL(accumulator.rows(), 1u);
    BOOST_REQUIRE_EQUAL(accumulator.singles(), zero);
    BOOST_REQUIRE_EQUAL(accumulator.multisig_keys(), 1u);
}

BOOST_AUTO_TEST_CASE(ecdsa_signatures__for_each__expected_records)
{
    const std::array<ec_compressed, 2> keys{ ecdsa_key, ecdsa_key };
    const std::array<ec_signature, 2> sigs{ ecdsa_sig, ecdsa_sig };

    ecdsa_signatures accumulator{};
    BOOST_REQUIRE(accumulator.append(ecdsa_sighash, ecdsa_key, ecdsa_sig));
    BOOST_REQUIRE(accumulator.append(sighash_bad, keys, sigs));

    size_t group{};
    accumulator.for_each([&](const hash_digest& digest,
        std::span<const ec_compressed> each_keys,
        std::span<const ec_signature> each_sigs) NOEXCEPT
    {
        BOOST_CHECK_EQUAL(digest, is_zero(group) ? ecdsa_sighash : sighash_bad);
        BOOST_CHECK_EQUAL(each_keys.size(), is_zero(group) ? one : two);
        BOOST_CHECK_EQUAL(each_sigs.size(), is_zero(group) ? one : two);
        BOOST_CHECK_EQUAL(each_keys.front(), ecdsa_key);
        BOOST_CHECK_EQUAL(each_sigs.front(), ecdsa_sig);
        ++group;
    });

    BOOST_REQUIRE_EQUAL(group, accumulator.groups());
}

BOOST_AUTO_TEST_CASE(ecdsa_signatures__verify_valid__true)
{
    ecdsa_signatures accumulator{};
    BOOST_REQUIRE(accumulator.append(ecdsa_sighash, ecdsa_key, ecdsa_sig));
    BOOST_REQUIRE(accumulator.verify());
}

BOOST_AUTO_TEST_CASE(ecdsa_signatures__verify_invalid__false)
{
    ecdsa_signatures accumulator{};
    BOOST_REQUIRE(accumulator.append(ecdsa_sighash, ecdsa_key, ecdsa_sig));
    BOOST_REQUIRE(accumulator.append(sighash_bad, ecdsa_key, ecdsa_sig));
    BOOST_REQUIRE(!accumulator.verify());
}

BOOST_AUTO_TEST_CASE(ecdsa_signatures__clear__empty)
{
    ecdsa_signatures accumulator{};
    BOOST_REQUIRE(accumulator.append(ecdsa_sighash, ecdsa_key, ecdsa_sig));
    accumulator.clear();
    BOOST_REQUIRE(accumulator.empty());
    BOOST_REQUIRE_EQUAL(accumulator.rows(), zero);
    BOOST_REQUIRE_EQUAL(accumulator.singles(), zero);
}

BOOST_AUTO_TEST_CASE(ecdsa_signatures__purge__empty)
{
    ecdsa_signatures accumulator{};
    BOOST_REQUIRE(accumulator.append(ecdsa_sighash, ecdsa_key, ecdsa_sig));
    accumulator.purge();
    BOOST_REQUIRE(accumulator.empty());
}

BOOST_AUTO_TEST_SUITE_END()
