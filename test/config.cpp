/*
 * Copyright (c) 2011-2015 libbitcoin developers (see AUTHORS)
 *
 * This file is part of libbitcoin.
 *
 * libbitcoin is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License with
 * additional permissions to the one published by the Free Software
 * Foundation, either version 3 of the License, or (at your option)
 * any later version. For more information see LICENSE.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#include <utility>
#include <vector>
#include <boost/test/test_tools.hpp>
#include <boost/test/unit_test_suite.hpp>
#include <bitcoin/bitcoin.hpp>

using namespace bc::config;

BOOST_AUTO_TEST_SUITE(config)

// TODO: add tests for these config utils.
//find_pair_position(const std::vector<Pair>& list, const Key& key)
//if_else(bool antecedent, const Consequent consequent, const Alternate alternative)
//insert_sorted(std::vector<Type>& list, Type const& element, Predicate predicate)
//home_directory()
//join(const std::vector<std::string>& words, const std::string& delimiter)
//read_stream(std::istream& stream)
//split(const std::string& sentence, const std::string& delimiter)
//trim(std::string& value)

//BOOST_AUTO_TEST_CASE(config__utilities__test)
//{
//    BOOST_REQUIRE(true);
//}

BOOST_AUTO_TEST_SUITE_END() // config