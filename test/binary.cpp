/*
 * Copyright (c) 2011-2013 libbitcoin developers (see AUTHORS)
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
#include <boost/test/unit_test.hpp>
#include <bitcoin/bitcoin.hpp>

using namespace bc;

static void require_equal(binary_type& expected, binary_type& instance)
{
//    for (binary_type::size_type i = 0; i < expected.blocks().size(); i++)
//    {
//        uint8_t value = instance.blocks()[i];
//        uint8_t expected_value = expected.blocks()[i];
//
//        if (value != expected_value)
//        {
//            // printf("item: %02u\n", i);
//            std::cout << "item: " << i << std::endl;
//            printf("value: 0x%02x\n", value);
//            printf("expected:0x%02x\n", expected_value);
//        }
//
//        BOOST_REQUIRE(value == expected_value);
//    }
    BOOST_REQUIRE(instance == expected);
}

BOOST_AUTO_TEST_SUITE(binary_type_tests)

//
// shift_left tests
//
BOOST_AUTO_TEST_CASE(shift_left_by_zero)
{
    binary_type::size_type distance = 0;
    binary_type instance(16, data_chunk { 0xAA, 0xAA, 0xAA });

    binary_type expected(16, data_chunk { 0xAA, 0xAA });

    instance.shift_left(distance);

    require_equal(expected, instance);
}

BOOST_AUTO_TEST_CASE(shift_left_alternate_initial)
{
    binary_type::size_type distance = 16;
    binary_type instance(24, data_chunk { 0xAB, 0xCD, 0xEF });

    binary_type expected(8, data_chunk { 0xEF });

    instance.shift_left(distance);

    require_equal(expected, instance);
}

BOOST_AUTO_TEST_CASE(shift_left_by_less_than_byte)
{
    binary_type::size_type distance = 5;
    binary_type instance(24, data_chunk { 0xAA, 0xAA, 0xAA });

    binary_type expected(19, data_chunk { 0x55, 0x55, 0x40});

    instance.shift_left(distance);

    require_equal(expected, instance);
}

BOOST_AUTO_TEST_CASE(shift_left_by_less_than_byte_to_byte_align)
{
    binary_type::size_type distance = 5;
    binary_type instance(21, data_chunk { 0xAA, 0xAA, 0xAA });

    binary_type expected(16, data_chunk { 0x55, 0x55 });

    instance.shift_left(distance);

    require_equal(expected, instance);
}

BOOST_AUTO_TEST_CASE(shift_left_by_byte_single)
{
    binary_type::size_type distance = 8;
    binary_type instance(24, data_chunk { 0xAA, 0xAA, 0xAA });

    binary_type expected(16, data_chunk { 0xAA, 0xAA });

    instance.shift_left(distance);

    require_equal(expected, instance);
}

BOOST_AUTO_TEST_CASE(shift_left_by_greater_than_byte)
{
    binary_type::size_type distance = 13;
    binary_type instance(24, data_chunk { 0xAA, 0xAA, 0xAA });

    binary_type expected(11, data_chunk { 0x55, 0x40 });

    instance.shift_left(distance);

    require_equal(expected, instance);
}

BOOST_AUTO_TEST_CASE(shift_left_by_greater_than_byte_not_initially_aligned)
{
    binary_type::size_type distance = 13;
    binary_type instance(18, data_chunk { 0xAA, 0xAA, 0xAA });

    binary_type expected(5, data_chunk { 0x50 });

    instance.shift_left(distance);

    require_equal(expected, instance);
}

BOOST_AUTO_TEST_CASE(shift_left_by_byte_multiple)
{
    binary_type::size_type distance = 16;
    binary_type instance(24, data_chunk { 0xAA, 0xAA, 0xAA });

    binary_type expected(8, data_chunk { 0xAA });

    instance.shift_left(distance);

    require_equal(expected, instance);
}

BOOST_AUTO_TEST_CASE(shift_left_by_size)
{
    binary_type::size_type distance = 24;
    binary_type instance(24, data_chunk { 0xAA, 0xAA, 0xAA });

    binary_type expected(0, data_chunk { });

    instance.shift_left(distance);

    require_equal(expected, instance);
}

BOOST_AUTO_TEST_CASE(shift_left_by_greater_than_size)
{
    binary_type::size_type distance = 30;
    binary_type instance(24, data_chunk { 0xAA, 0xAA, 0xAA });

    binary_type expected(0, data_chunk { });

    instance.shift_left(distance);

    require_equal(expected, instance);
}

//
// shift_right tests
//
BOOST_AUTO_TEST_CASE(shift_right_by_zero)
{
    binary_type::size_type distance = 0;
    binary_type instance(16, data_chunk { 0xAA, 0xAA, 0xAA });

    binary_type expected(16, data_chunk { 0xAA, 0xAA });

    instance.shift_right(distance);

    require_equal(expected, instance);
}

BOOST_AUTO_TEST_CASE(shift_right_by_less_than_byte)
{
    binary_type::size_type distance = 5;
    binary_type instance(24, data_chunk { 0xAA, 0xAA, 0xAA });

    binary_type expected(29, data_chunk { 0x05, 0x55, 0x55, 0x50});

    instance.shift_right(distance);

    require_equal(expected, instance);
}

BOOST_AUTO_TEST_CASE(shift_right_by_less_than_byte_to_byte_align)
{
    binary_type::size_type distance = 3;
    binary_type instance(21, data_chunk { 0xAA, 0xAA, 0xAA });

    binary_type expected(24, data_chunk { 0x15, 0x55, 0x55 });

    instance.shift_right(distance);

    require_equal(expected, instance);
}

BOOST_AUTO_TEST_CASE(shift_right_by_byte_single)
{
    binary_type::size_type distance = 8;
    binary_type instance(24, data_chunk { 0xAA, 0xAA, 0xAA });

    binary_type expected(32, data_chunk { 0x00, 0xAA, 0xAA, 0xAA });

    instance.shift_right(distance);

    require_equal(expected, instance);
}

BOOST_AUTO_TEST_CASE(shift_right_by_greater_than_byte)
{
    binary_type::size_type distance = 13;
    binary_type instance(24, data_chunk { 0xAA, 0xAA, 0xAA });

    binary_type expected(37, data_chunk { 0x00, 0x05, 0x55, 0x55, 0x50 });

    instance.shift_right(distance);

    require_equal(expected, instance);
}

BOOST_AUTO_TEST_CASE(shift_right_by_greater_than_byte_not_initially_aligned)
{
    binary_type::size_type distance = 13;
    binary_type instance(18, data_chunk { 0xAA, 0xAA, 0xAA });

    binary_type expected(31, data_chunk { 0x00, 0x05, 0x55, 0x54 });

    instance.shift_right(distance);

    require_equal(expected, instance);
}

BOOST_AUTO_TEST_CASE(shift_right_by_byte_multiple)
{
    binary_type::size_type distance = 16;
    binary_type instance(24, data_chunk { 0xAA, 0xAA, 0xAA });

    binary_type expected(40, data_chunk { 0x00, 0x00, 0xAA, 0xAA, 0xAA });

    instance.shift_right(distance);

    require_equal(expected, instance);
}

BOOST_AUTO_TEST_CASE(shift_right_by_size)
{
    binary_type::size_type distance = 24;
    binary_type instance(24, data_chunk { 0xAA, 0xAA, 0xAA });

    binary_type expected(48, data_chunk { 0x00, 0x00, 0x00, 0xAA, 0xAA, 0xAA });

    instance.shift_right(distance);

    require_equal(expected, instance);
}

BOOST_AUTO_TEST_CASE(shift_right_by_greater_than_size)
{
    binary_type::size_type distance = 30;
    binary_type instance(24, data_chunk { 0xAA, 0xAA, 0xAA });

    binary_type expected(54, data_chunk { 0x00, 0x00, 0x00, 0x02, 0xAA, 0xAA, 0xA8 });

    instance.shift_right(distance);

    require_equal(expected, instance);
}

//
// append tests
//
BOOST_AUTO_TEST_CASE(append_to_zero_length)
{
    binary_type instance(0, data_chunk { 0x00 });
    binary_type augment(24, data_chunk { 0xAA, 0xBB, 0xCC });

    binary_type expected(24, data_chunk { 0xAA, 0xBB, 0xCC });

    instance.append(augment);

    require_equal(expected, instance);
}

BOOST_AUTO_TEST_CASE(append_zero_length_to_content)
{
    binary_type instance(24, data_chunk { 0xAA, 0xBB, 0xCC });
    binary_type augment(0, data_chunk { });

    binary_type expected(24, data_chunk { 0xAA, 0xBB, 0xCC });

    instance.append(augment);

    require_equal(expected, instance);
}

BOOST_AUTO_TEST_CASE(append_byte_aligned_instances)
{
    binary_type instance(24, data_chunk { 0xAA, 0xBB, 0xCC });
    binary_type augment(24, data_chunk { 0xDD, 0xEE, 0xFF });

    binary_type expected(48, data_chunk { 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF });

    instance.append(augment);

    require_equal(expected, instance);
}

BOOST_AUTO_TEST_CASE(append_byte_nonaligned_instances)
{
    binary_type instance(20, data_chunk { 0xAA, 0xBB, 0xCC });
    binary_type augment(13, data_chunk { 0xDD, 0xEE });

    binary_type expected(33, data_chunk { 0xAA, 0xBB, 0xCD, 0xDE, 0x80 });

    instance.append(augment);

    require_equal(expected, instance);
}

//
// prepend tests
//
BOOST_AUTO_TEST_CASE(prepend_to_zero_length)
{
    binary_type instance(0, data_chunk { 0x00 });
    binary_type augment(24, data_chunk { 0xAA, 0xBB, 0xCC });

    binary_type expected(24, data_chunk { 0xAA, 0xBB, 0xCC });

    instance.prepend(augment);

    require_equal(expected, instance);
}

BOOST_AUTO_TEST_CASE(prepend_zero_length_to_content)
{
    binary_type instance(24, data_chunk { 0xAA, 0xBB, 0xCC });
    binary_type augment(0, data_chunk { });

    binary_type expected(24, data_chunk { 0xAA, 0xBB, 0xCC });

    instance.prepend(augment);

    require_equal(expected, instance);
}

BOOST_AUTO_TEST_CASE(prepend_byte_aligned_instances)
{
    binary_type instance(24, data_chunk { 0xAA, 0xBB, 0xCC });
    binary_type augment(24, data_chunk { 0xDD, 0xEE, 0xFF });

    binary_type expected(48, data_chunk { 0xDD, 0xEE, 0xFF, 0xAA, 0xBB, 0xCC });

    instance.prepend(augment);

    require_equal(expected, instance);
}

BOOST_AUTO_TEST_CASE(prepend_byte_nonaligned_instances)
{
    binary_type instance(20, data_chunk { 0xAA, 0xBB, 0xCC });
    binary_type augment(13, data_chunk { 0xDD, 0xEE });

    binary_type expected(33, data_chunk { 0xDD, 0xED, 0x55, 0xDE, 0x00 });

    instance.prepend(augment);

    require_equal(expected, instance);
}

//
// get_substring tests
//
BOOST_AUTO_TEST_CASE(get_substring_start_after_end)
{
    binary_type instance(20, data_chunk { 0xAA, 0xBB, 0xCC });
    binary_type::size_type start = 21;
    binary_type::size_type length = 10;

    binary_type expected(0, data_chunk {});

    binary_type result = instance.get_substring(start, length);

    require_equal(expected, result);
}

BOOST_AUTO_TEST_CASE(get_substring_length_zero)
{
    binary_type instance(20, data_chunk { 0xAA, 0xBB, 0xCC });
    binary_type::size_type start = 5;
    binary_type::size_type length = 0;

    binary_type expected(0, data_chunk {});

    binary_type result = instance.get_substring(start, length);

    require_equal(expected, result);
}

BOOST_AUTO_TEST_CASE(get_substring_byte_aligned_start)
{
    binary_type instance(20, data_chunk { 0xAA, 0xBB, 0xCC });
    binary_type::size_type start = 8;
    binary_type::size_type length = 10;

    binary_type expected(10, data_chunk { 0xBB, 0xC0 });

    binary_type result = instance.get_substring(start, length);

    require_equal(expected, result);
}

BOOST_AUTO_TEST_CASE(get_substring_byte_nonaligned_start)
{
    binary_type instance(20, data_chunk { 0xAA, 0xBB, 0xCC });
    binary_type::size_type start = 10;
    binary_type::size_type length = 10;

    binary_type expected(10, data_chunk { 0xEF, 0x00 });

    binary_type result = instance.get_substring(start, length);

    require_equal(expected, result);
}

BOOST_AUTO_TEST_CASE(get_substring_request_exceeds_string)
{
    binary_type instance(20, data_chunk { 0xAA, 0xBB, 0xCC });
    binary_type::size_type start = 10;
    binary_type::size_type length = 15;

    binary_type expected(10, data_chunk { 0xEF, 0x00 });

    binary_type result = instance.get_substring(start, length);

    require_equal(expected, result);
}

BOOST_AUTO_TEST_CASE(get_substring_implicit_length)
{
    binary_type instance(20, data_chunk { 0xAA, 0xBB, 0xCC });
    binary_type::size_type start = 10;

    binary_type expected(10, data_chunk { 0xEF, 0x00 });

    binary_type result = instance.get_substring(start);

    require_equal(expected, result);
}

BOOST_AUTO_TEST_SUITE_END()
