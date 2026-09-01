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

BOOST_AUTO_TEST_SUITE(stripper_tests)

using namespace system::chain;

BOOST_AUTO_TEST_CASE(stripper__constructor__opcode__expected)
{
    const stripper instance{ opcode::codeseparator };
    BOOST_REQUIRE(instance.code() == opcode::codeseparator);
}

BOOST_AUTO_TEST_CASE(stripper__constructor__push_data__nominal_opcode)
{
    const auto data = base16_chunk("0102");
    const chunk_xptr ptr{ data };
    const stripper instance{ ptr };
    BOOST_REQUIRE(instance.code() == opcode::push_size_2);
    BOOST_REQUIRE_EQUAL(instance.data(), data);
    BOOST_REQUIRE(instance.data_ptr() == ptr);
}

BOOST_AUTO_TEST_CASE(stripper__operation_equality__same_value_distinct_data__true)
{
    const auto data = base16_chunk("0102");
    const chunk_xptr ptr{ data };
    const stripper instance{ ptr };
    const operation op{ base16_chunk("0102"), false };
    BOOST_REQUIRE(op == instance);
}

BOOST_AUTO_TEST_CASE(stripper__operation_equality__different_data__false)
{
    const auto data = base16_chunk("0102");
    const chunk_xptr ptr{ data };
    const stripper instance{ ptr };
    const operation op{ base16_chunk("0103"), false };
    BOOST_REQUIRE(!(op == instance));
}

BOOST_AUTO_TEST_SUITE_END()
