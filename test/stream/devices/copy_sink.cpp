/**
 * Copyright (c) 2011-2023 libbitcoin developers (see AUTHORS)
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

BOOST_AUTO_TEST_SUITE(stream_tests)

// Sequence tests fail on clang, possibly std::pair wraps members.
// Now failing on MSC as well.
////#if defined (HAVE_MSC)
////
////BOOST_AUTO_TEST_CASE(copy_sink__output_sequence__not_empty__expected)
////{
////    constexpr auto size = 42u;
////    data_chunk sink(size);
////    copy_sink<data_slab> instance(sink);
////    const auto sequence = instance.output_sequence();
////    using char_type = typename device<data_chunk>::char_type;
////    BOOST_REQUIRE(sequence.first == reinterpret_cast<char_type*>(&(*sink.begin())));
////    BOOST_REQUIRE(sequence.second == std::next(sequence.first, size));
////}
////
////#endif // HAVE_MSC

// write() is not required for direct devices.
#if defined(DISABLED)

// write

BOOST_AUTO_TEST_CASE(copy_sink__write__nullptr__false)
{
    data_chunk sink;
    copy_sink<data_slab> instance(sink);
    BOOST_REQUIRE_EQUAL(instance.write(nullptr, 0), -1);
}

BOOST_AUTO_TEST_CASE(copy_sink__write__empty__true)
{
    const std::string from;
    data_chunk sink{ 0x00 };
    copy_sink<data_slab> instance(sink);
    BOOST_REQUIRE_EQUAL(instance.write(from.data(), 0), 0);
}

BOOST_AUTO_TEST_CASE(copy_sink__write__negative__false)
{
    const std::string from{ "a" };
    data_chunk sink{ 0x00 };
    copy_sink<data_slab> instance(sink);
    BOOST_REQUIRE_EQUAL(instance.write(from.data(), -1), -1);
    BOOST_REQUIRE_EQUAL(sink[0], 0x00);
}

BOOST_AUTO_TEST_CASE(copy_sink__write__past_end__expected)
{
    const std::string from{ "a" };
    data_chunk sink{ 0x00 };
    copy_sink<data_slab> instance(sink);
    BOOST_REQUIRE_EQUAL(instance.write(from.data(), 2), 1);
    BOOST_REQUIRE_EQUAL(sink[0], from[0]);
}

BOOST_AUTO_TEST_CASE(copy_sink__write__zero__zero)
{
    const std::string from{ "a" };
    data_chunk sink{ 0x00 };
    copy_sink<data_slab> instance(sink);
    BOOST_REQUIRE_EQUAL(instance.write(from.data(), 0), 0);
    BOOST_REQUIRE_EQUAL(sink[0], 0x00);
}

BOOST_AUTO_TEST_CASE(copy_sink__write__one__expected)
{
    const std::string from{ "a" };
    data_chunk sink{ 0x00 };
    copy_sink<data_slab> instance(sink);
    BOOST_REQUIRE_EQUAL(instance.write(from.data(), 1), 1);
    BOOST_REQUIRE_EQUAL(sink[0], from[0]);
}

BOOST_AUTO_TEST_CASE(copy_sink__write__multiple__correct_tracking)
{
    const std::string from{ "abcdef" };
    data_chunk sink(6, 0x00);
    copy_sink<data_slab> instance(sink);
    BOOST_REQUIRE_EQUAL(instance.write(&from[0], 1), 1);
    BOOST_REQUIRE_EQUAL(sink[0], from[0]);
    BOOST_REQUIRE_EQUAL(instance.write(&from[1], 2), 2);
    BOOST_REQUIRE_EQUAL(sink[1], from[1]);
    BOOST_REQUIRE_EQUAL(sink[2], from[2]);
    BOOST_REQUIRE_EQUAL(instance.write(&from[3], 3), 3);
    BOOST_REQUIRE_EQUAL(sink[3], from[3]);
    BOOST_REQUIRE_EQUAL(sink[4], from[4]);
    BOOST_REQUIRE_EQUAL(sink[5], from[5]);
    BOOST_REQUIRE_EQUAL(instance.write(&from[6], 42), 0);
}

#endif // DISABLED

BOOST_AUTO_TEST_SUITE_END()
