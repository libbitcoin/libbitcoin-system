/**
 * Copyright (c) 2011-2021 libbitcoin developers (see AUTHORS)
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
#include "electrum_v1.hpp"

BOOST_AUTO_TEST_SUITE(dictionaries_tests)

using namespace bc::system::wallet;

#ifdef WITH_ICU

// abnormal (requires ICU)

// These dictionaries are in normal form.
// So there is no need to nfkd normalize these for wordlist-based seedings.
// This also removes the ICU dependency for these language.
BOOST_AUTO_TEST_CASE(dictionaries_electrum_v1__normal__normal_words__true)
{
    BOOST_REQUIRE(!abnormal(electrum_v1::en));
    BOOST_REQUIRE(!abnormal(electrum_v1::pt));
}

#endif

BOOST_AUTO_TEST_SUITE_END()
