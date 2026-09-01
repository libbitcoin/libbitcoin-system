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
#include "../test.hpp"

BOOST_AUTO_TEST_SUITE(annex_tests)

using namespace system::chain;

class annex_accessor
  : public annex
{
public:
    using annex::annex;
    using annex::from_stack;
};

// is_annex_pattern (at least two stack elements, last prefixed 0x50)

BOOST_AUTO_TEST_CASE(annex__is_annex_pattern__empty_stack__false)
{
    const chunk_cptrs stack{};
    BOOST_REQUIRE(!annex::is_annex_pattern(stack));
}

BOOST_AUTO_TEST_CASE(annex__is_annex_pattern__single_element__false)
{
    const chunk_cptrs stack{ to_shared(base16_chunk("500102")) };
    BOOST_REQUIRE(!annex::is_annex_pattern(stack));
}

BOOST_AUTO_TEST_CASE(annex__is_annex_pattern__two_elements_prefixed__true)
{
    const chunk_cptrs stack
    {
        to_shared(base16_chunk("0102")),
        to_shared(base16_chunk("500102"))
    };

    BOOST_REQUIRE(annex::is_annex_pattern(stack));
}

BOOST_AUTO_TEST_CASE(annex__is_annex_pattern__two_elements_unprefixed__false)
{
    const chunk_cptrs stack
    {
        to_shared(base16_chunk("0102")),
        to_shared(base16_chunk("510102"))
    };

    BOOST_REQUIRE(!annex::is_annex_pattern(stack));
}

// from_stack

BOOST_AUTO_TEST_CASE(annex__from_stack__annex_pattern__last_element)
{
    const auto last = to_shared(base16_chunk("500102"));
    const chunk_cptrs stack{ to_shared(base16_chunk("0102")), last };
    BOOST_REQUIRE_EQUAL(*annex_accessor::from_stack(stack), *last);
}

// construct/size/bool

BOOST_AUTO_TEST_CASE(annex__constructor__default__false)
{
    const annex instance{};
    BOOST_REQUIRE(!instance);
}

BOOST_AUTO_TEST_CASE(annex__constructor__stack__expected)
{
    const chunk_cptrs stack
    {
        to_shared(base16_chunk("0102")),
        to_shared(base16_chunk("500102"))
    };

    const annex instance{ stack };
    BOOST_REQUIRE(instance);
    BOOST_REQUIRE_EQUAL(instance.size(), 3u);
}

// hash
// Expected values computed by an independent implementation, the annex is
// hashed inclusive of its 0x50 prefix byte [bip341].

BOOST_AUTO_TEST_CASE(annex__hash__prefixed__expected)
{
    const annex instance{ to_shared(base16_chunk("500102")) };
    BOOST_REQUIRE_EQUAL(instance.hash(true), base16_array("d9b98f9023fc72b2c7191a75e1a1c4e6a8c0eff81051f173582cc21da52285e0"));
}

BOOST_AUTO_TEST_CASE(annex__hash__unprefixed__expected)
{
    const annex instance{ to_shared(base16_chunk("500102")) };
    BOOST_REQUIRE_EQUAL(instance.hash(false), base16_array("d978efba3e41ea9d2c9073fc50b3ed18048e85bbd32ad19594f3ef2ea7a5c58b"));
}

BOOST_AUTO_TEST_SUITE_END()
