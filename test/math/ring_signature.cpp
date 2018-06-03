/**
 * Copyright (c) 2011-2017 libbitcoin developers (see AUTHORS)
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
#include <random>
#include <boost/test/unit_test.hpp>
#include <bitcoin/bitcoin.hpp>
#include "ring_signature.hpp"

using namespace bc;

BOOST_AUTO_TEST_SUITE(ring_signature_tests)

BOOST_AUTO_TEST_CASE(ring_signature__basic_test)
{
    for (const auto& test: ring_signature_test_vectors)
    {
        ring_signature signature;
        signature.proofs = test.s;

        BOOST_REQUIRE(sign(
            signature, test.secrets, test.public_rings, test.message, test.k));

        // Signing produces correct e value
        BOOST_REQUIRE(std::equal(
            signature.challenge.begin(), signature.challenge.end(),
            test.e.begin()));

        // Signing produces correct s values
        BOOST_REQUIRE(signature.proofs.size() == test.s.size());
        for (size_t i = 0; i < signature.proofs.size(); ++i)
        {
            const auto& signature_s = signature.proofs[i];
            const auto& test_s = test.s[i];
            BOOST_REQUIRE(signature_s.size() == test_s.size());
            for (size_t j = 0; j < signature_s.size(); ++j)
            {
                const auto& s0 = signature_s[j];
                const auto& s1 = test_s[j];
                // Both s values are equal.
                BOOST_REQUIRE(std::equal(s0.begin(), s0.end(), s1.begin()));
            }
        }

        BOOST_REQUIRE(verify(test.public_rings, test.message, signature));
    }
}

BOOST_AUTO_TEST_CASE(ring_signature__negate)
{
    for (const auto& test: ring_signature_negative_test_vectors)
    {
        ring_signature signature;
        signature.challenge = test.e;
        signature.proofs = test.s;

        BOOST_REQUIRE(!verify(test.public_rings, test.message, signature));
    }
}

BOOST_AUTO_TEST_CASE(ring_signature__faulty)
{
    for (const auto& test: ring_signature_faulty_test_vectors)
    {
        ring_signature signature;
        signature.challenge = test.e;
        signature.proofs = test.s;

        BOOST_REQUIRE(!verify(test.public_rings, test.message, signature));
    }
}

BOOST_AUTO_TEST_SUITE_END()

