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

BOOST_AUTO_TEST_SUITE(schnorr_signatures_tests)

using namespace system::chain;

const hash_digest sighash_bad = base16_array
(
    "4242424242424242424242424242424242424242424242424242424242424242"
);
const hash_digest schnorr_sighash = base16_array
(
    "0000000000000000000000000000000000000000000000000000000000000000"
);
const ec_xonly schnorr_key = base16_array
(
    "f9308a019258c31049344f85f89d5229b531c845836f99b08601f113bce036f9"
);
const ec_signature schnorr_sig = base16_array
(
    "e907831f80848d1069a5371b402410364bdf1c5f8307b0084c55f1ce2dca8215"
    "25f66a4a85ea8b71e482a74f382d2ce5ebeee8fdb2172f477df4900d310536c0"
);

BOOST_AUTO_TEST_CASE(schnorr_signatures__default__empty)
{
    const schnorr_signatures accumulator{};
    BOOST_REQUIRE(accumulator.empty());
    BOOST_REQUIRE(accumulator.rows().empty());
    BOOST_REQUIRE_EQUAL(accumulator.thresholds(), zero);
    BOOST_REQUIRE(accumulator.verify());
}

BOOST_AUTO_TEST_CASE(schnorr_signatures__append__expected_rows)
{
    schnorr_signatures accumulator{};
    accumulator.append(schnorr_sighash, schnorr_key, schnorr_sig);
    accumulator.append(sighash_bad, schnorr_key, schnorr_sig);
    BOOST_REQUIRE(!accumulator.empty());
    BOOST_REQUIRE_EQUAL(accumulator.rows().size(), 2u);
    BOOST_REQUIRE_EQUAL(accumulator.rows().front().digest, schnorr_sighash);
    BOOST_REQUIRE_EQUAL(accumulator.rows().front().point, schnorr_key);
    BOOST_REQUIRE_EQUAL(accumulator.rows().front().signature, schnorr_sig);
    BOOST_REQUIRE_EQUAL(accumulator.rows().back().digest, sighash_bad);
}

BOOST_AUTO_TEST_CASE(schnorr_signatures__count_threshold__expected)
{
    schnorr_signatures accumulator{};
    accumulator.count_threshold(2);
    accumulator.count_threshold(3);
    BOOST_REQUIRE_EQUAL(accumulator.thresholds(), 5u);
}

BOOST_AUTO_TEST_CASE(schnorr_signatures__verify_valid__true)
{
    schnorr_signatures accumulator{};
    accumulator.append(schnorr_sighash, schnorr_key, schnorr_sig);
    BOOST_REQUIRE(accumulator.verify());
}

BOOST_AUTO_TEST_CASE(schnorr_signatures__verify_invalid__false)
{
    schnorr_signatures accumulator{};
    accumulator.append(schnorr_sighash, schnorr_key, schnorr_sig);
    accumulator.append(sighash_bad, schnorr_key, schnorr_sig);
    BOOST_REQUIRE(!accumulator.verify());
}

BOOST_AUTO_TEST_CASE(schnorr_signatures__clear__empty)
{
    schnorr_signatures accumulator{};
    accumulator.append(schnorr_sighash, schnorr_key, schnorr_sig);
    accumulator.count_threshold(1);
    accumulator.clear();
    BOOST_REQUIRE(accumulator.empty());
    BOOST_REQUIRE_EQUAL(accumulator.thresholds(), zero);
}

BOOST_AUTO_TEST_CASE(schnorr_signatures__purge__empty)
{
    schnorr_signatures accumulator{};
    accumulator.append(schnorr_sighash, schnorr_key, schnorr_sig);
    accumulator.purge();
    BOOST_REQUIRE(accumulator.empty());
}

BOOST_AUTO_TEST_SUITE_END()
