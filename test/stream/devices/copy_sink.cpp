/**
 * Copyright (c) 2011-2022 libbitcoin developers (see AUTHORS)
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

// output_sequence

// CLang boost does not provide expected output_sequence for empty copy_sink.
// However it does for empty copy_source.
// 
// Platform: linux
// Compiler: Clang version 14.0.0 
// STL     : GNU libstdc++ version 20220324
// Boost   : 1.78.0 [debug and ndebug]
// test/stream/devices/copy_sink.cpp(32): fatal error:
// stream_tests/copy_sink__output_sequence__empty__empty:
// critical check first == sequence.first has failed.
//
// Additional detail on above Clang error from XCode:
//
// Platform: Mac OS
// Compiler: Clang version 13.0.0 (clang-1300.0.29.30)
// STL     : libc++ version 12000
// Boost   : 1.78.0 [debug and ndebug]
// unknown location:0: fatal error:
// stream_tests/copy_sink__output_sequence__empty__empty:
// memory access violation at address: 0x00000000: no mapping at fault address
#if defined(HAVE_MSC)
BOOST_AUTO_TEST_CASE(copy_sink__output_sequence__empty__empty)
{
    // sink.data() is nullptr, and should be reflected in sequence.first
    // and sequence.second, as sink.size() is zero.
    const data_chunk chunk{};
    BOOST_REQUIRE(chunk.data() == nullptr);

    data_chunk sink{};
    copy_sink<data_slab> instance(sink);
    const auto sequence = instance.output_sequence();
    BOOST_REQUIRE(sequence.first == nullptr);
    BOOST_REQUIRE(sequence.second == nullptr);
    BOOST_REQUIRE_EQUAL(std::distance(sequence.first, sequence.second), ptrdiff_t{ 0 });
}
#endif

BOOST_AUTO_TEST_CASE(copy_sink__output_sequence__not_empty__expected)
{
    constexpr auto size = 42u;
    data_chunk sink(size, 0x00);
    copy_sink<data_slab> instance(sink);
    const auto sequence = instance.output_sequence();
    BOOST_REQUIRE(reinterpret_cast<uint8_t*>(sequence.first) == &sink[0]);
    BOOST_REQUIRE(reinterpret_cast<uint8_t*>(std::prev(sequence.second)) == &sink[sub1(size)]);
    BOOST_REQUIRE_EQUAL(std::distance(sequence.first, sequence.second), ptrdiff_t{ size });
}

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
