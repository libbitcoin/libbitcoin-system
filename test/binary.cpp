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

static void require_equal(binary_type::size_type expected_size,
    data_chunk& expected_blocks, binary_type& instance)
{
//    BOOST_CHECK_EQUAL_COLLECTIONS(
//        instance.blocks().begin(), instance.blocks().end(),
//        expected_blocks.begin(), expected_blocks.end());

    BOOST_REQUIRE(instance.size() == expected_size);
    BOOST_REQUIRE(instance.blocks().size() == expected_blocks.size());

    for (binary_type::size_type i = 0; i < expected_blocks.size(); i++)
    {
        uint8_t value = instance.blocks()[i];
        uint8_t expected = expected_blocks[i];

        if (value != expected)
        {
            // printf("item: %02u\n", i);
            std::cout << "item: " << i << std::endl;
            printf("value: 0x%02x\n", value);
            printf("expected:0x%02x\n", expected);
        }

        BOOST_REQUIRE(value == expected);
    }
}

BOOST_AUTO_TEST_SUITE(binary_type_tests)

//
// shift_left tests
//
BOOST_AUTO_TEST_CASE(shift_left_by_zero)
{
    binary_type::size_type distance = 0;

    data_chunk initial_blocks = { 0xAA, 0xAA, 0xAA };
    binary_type::size_type initial_size = 16;

    data_chunk expected_blocks = { 0xAA, 0xAA };
    binary_type::size_type expected_size = 16;

    bc::binary_type instance(initial_size, initial_blocks);

    instance.shift_left(distance);

    require_equal(expected_size, expected_blocks, instance);
}

BOOST_AUTO_TEST_CASE(shift_left_alternate_initial)
{
    binary_type::size_type distance = 16;

    data_chunk initial_blocks = { 0xAB, 0xCD, 0xEF };
    binary_type::size_type initial_size = 24;

    data_chunk expected_blocks = { 0xEF };
    binary_type::size_type expected_size = 8;

    bc::binary_type instance(initial_size, initial_blocks);

    instance.shift_left(distance);

    require_equal(expected_size, expected_blocks, instance);
}

BOOST_AUTO_TEST_CASE(shift_left_by_less_than_byte)
{
    binary_type::size_type distance = 5;

    data_chunk initial_blocks = { 0xAA, 0xAA, 0xAA };
    binary_type::size_type initial_size = 24;

    data_chunk expected_blocks = { 0x55, 0x55, 0x40};
    binary_type::size_type expected_size = 19;

    bc::binary_type instance(initial_size, initial_blocks);

    instance.shift_left(distance);

    require_equal(expected_size, expected_blocks, instance);
}

BOOST_AUTO_TEST_CASE(shift_left_by_less_than_byte_to_byte_align)
{
    binary_type::size_type distance = 5;

    data_chunk initial_blocks = { 0xAA, 0xAA, 0xAA };
    binary_type::size_type initial_size = 21;

    data_chunk expected_blocks = { 0x55, 0x55 };
    binary_type::size_type expected_size = 16;

    bc::binary_type instance(initial_size, initial_blocks);

    instance.shift_left(distance);

    require_equal(expected_size, expected_blocks, instance);
}

BOOST_AUTO_TEST_CASE(shift_left_by_byte_single)
{
    binary_type::size_type distance = 8;

    data_chunk initial_blocks = { 0xAA, 0xAA, 0xAA };
    binary_type::size_type initial_size = 24;

    data_chunk expected_blocks = { 0xAA, 0xAA };
    binary_type::size_type expected_size = 16;

    bc::binary_type instance(initial_size, initial_blocks);

    instance.shift_left(distance);

    require_equal(expected_size, expected_blocks, instance);
}

BOOST_AUTO_TEST_CASE(shift_left_by_greater_than_byte)
{
    binary_type::size_type distance = 13;

    data_chunk initial_blocks = { 0xAA, 0xAA, 0xAA };
    binary_type::size_type initial_size = 24;

    data_chunk expected_blocks = { 0x55, 0x40 };
    binary_type::size_type expected_size = 11;

    bc::binary_type instance(initial_size, initial_blocks);

    instance.shift_left(distance);

    require_equal(expected_size, expected_blocks, instance);
}

BOOST_AUTO_TEST_CASE(shift_left_by_greater_than_byte_not_initially_aligned)
{
    binary_type::size_type distance = 13;

    data_chunk initial_blocks = { 0xAA, 0xAA, 0xAA };
    binary_type::size_type initial_size = 18;

    data_chunk expected_blocks = { 0x50 };
    binary_type::size_type expected_size = 5;

    bc::binary_type instance(initial_size, initial_blocks);

    instance.shift_left(distance);

    require_equal(expected_size, expected_blocks, instance);
}

BOOST_AUTO_TEST_CASE(shift_left_by_byte_multiple)
{
    binary_type::size_type distance = 16;

    data_chunk initial_blocks = { 0xAA, 0xAA, 0xAA };
    binary_type::size_type initial_size = 24;

    data_chunk expected_blocks = { 0xAA };
    binary_type::size_type expected_size = 8;

    bc::binary_type instance(initial_size, initial_blocks);

    instance.shift_left(distance);

    require_equal(expected_size, expected_blocks, instance);
}

BOOST_AUTO_TEST_CASE(shift_left_by_size)
{
    binary_type::size_type distance = 24;

    data_chunk initial_blocks = { 0xAA, 0xAA, 0xAA };
    binary_type::size_type initial_size = 24;

    data_chunk expected_blocks = { };
    binary_type::size_type expected_size = 0;

    bc::binary_type instance(initial_size, initial_blocks);

    instance.shift_left(distance);

    require_equal(expected_size, expected_blocks, instance);
}

BOOST_AUTO_TEST_CASE(shift_left_by_greater_than_size)
{
    binary_type::size_type distance = 30;

    data_chunk initial_blocks = { 0xAA, 0xAA, 0xAA };
    binary_type::size_type initial_size = 24;

    data_chunk expected_blocks = { };
    binary_type::size_type expected_size = 0;

    bc::binary_type instance(initial_size, initial_blocks);

    instance.shift_left(distance);

    require_equal(expected_size, expected_blocks, instance);
}

//
// shift_right tests
//
BOOST_AUTO_TEST_CASE(shift_right_by_zero)
{
    data_chunk initial_blocks = { 0xAA, 0xAA, 0xAA };
    binary_type::size_type initial_size = 16;

    data_chunk expected_blocks = { 0xAA, 0xAA };
    binary_type::size_type expected_size = 16;

    bc::binary_type instance(initial_size, initial_blocks);

    instance.shift_right(0);

    BOOST_REQUIRE(instance.size() == expected_size);
    BOOST_REQUIRE(instance.blocks().size() == expected_blocks.size());

    require_equal(expected_size, expected_blocks, instance);
}

BOOST_AUTO_TEST_CASE(shift_right_by_less_than_byte)
{
    binary_type::size_type distance = 5;

    data_chunk initial_blocks = { 0xAA, 0xAA, 0xAA };
    binary_type::size_type initial_size = 24;

    data_chunk expected_blocks = { 0x05, 0x55, 0x55, 0x50};
    binary_type::size_type expected_size = 29;

    bc::binary_type instance(initial_size, initial_blocks);

    instance.shift_right(distance);

    require_equal(expected_size, expected_blocks, instance);
}

BOOST_AUTO_TEST_CASE(shift_right_by_less_than_byte_to_byte_align)
{
    binary_type::size_type distance = 3;

    data_chunk initial_blocks = { 0xAA, 0xAA, 0xAA };
    binary_type::size_type initial_size = 21;

    data_chunk expected_blocks = { 0x15, 0x55, 0x55 };
    binary_type::size_type expected_size = 24;

    bc::binary_type instance(initial_size, initial_blocks);

    instance.shift_right(distance);

    require_equal(expected_size, expected_blocks, instance);
}

BOOST_AUTO_TEST_CASE(shift_right_by_byte_single)
{
    binary_type::size_type distance = 8;

    data_chunk initial_blocks = { 0xAA, 0xAA, 0xAA };
    binary_type::size_type initial_size = 24;

    data_chunk expected_blocks = { 0x00, 0xAA, 0xAA, 0xAA };
    binary_type::size_type expected_size = 32;

    bc::binary_type instance(initial_size, initial_blocks);

    instance.shift_right(distance);

    require_equal(expected_size, expected_blocks, instance);
}

BOOST_AUTO_TEST_CASE(shift_right_by_greater_than_byte)
{
    binary_type::size_type distance = 13;

    data_chunk initial_blocks = { 0xAA, 0xAA, 0xAA };
    binary_type::size_type initial_size = 24;

    data_chunk expected_blocks = { 0x00, 0x05, 0x55, 0x55, 0x50 };
    binary_type::size_type expected_size = 37;

    bc::binary_type instance(initial_size, initial_blocks);

    instance.shift_right(distance);

    require_equal(expected_size, expected_blocks, instance);
}

BOOST_AUTO_TEST_CASE(shift_right_by_greater_than_byte_not_initially_aligned)
{
    binary_type::size_type distance = 13;

    data_chunk initial_blocks = { 0xAA, 0xAA, 0xAA };
    binary_type::size_type initial_size = 18;

    data_chunk expected_blocks = { 0x00, 0x05, 0x55, 0x54 };
    binary_type::size_type expected_size = 31;

    bc::binary_type instance(initial_size, initial_blocks);

    instance.shift_right(distance);

    require_equal(expected_size, expected_blocks, instance);
}

BOOST_AUTO_TEST_CASE(shift_right_by_byte_multiple)
{
    binary_type::size_type distance = 16;

    data_chunk initial_blocks = { 0xAA, 0xAA, 0xAA };
    binary_type::size_type initial_size = 24;

    data_chunk expected_blocks = { 0x00, 0x00, 0xAA, 0xAA, 0xAA };
    binary_type::size_type expected_size = 40;

    bc::binary_type instance(initial_size, initial_blocks);

    instance.shift_right(distance);

    require_equal(expected_size, expected_blocks, instance);
}

BOOST_AUTO_TEST_CASE(shift_right_by_size)
{
    binary_type::size_type distance = 24;

    data_chunk initial_blocks = { 0xAA, 0xAA, 0xAA };
    binary_type::size_type initial_size = 24;

    data_chunk expected_blocks = { 0x00, 0x00, 0x00, 0xAA, 0xAA, 0xAA };
    binary_type::size_type expected_size = 48;

    bc::binary_type instance(initial_size, initial_blocks);

    instance.shift_right(distance);

    require_equal(expected_size, expected_blocks, instance);
}

BOOST_AUTO_TEST_CASE(shift_right_by_greater_than_size)
{
    binary_type::size_type distance = 30;

    data_chunk initial_blocks = { 0xAA, 0xAA, 0xAA };
    binary_type::size_type initial_size = 24;

    data_chunk expected_blocks = { 0x00, 0x00, 0x00, 0x02, 0xAA, 0xAA, 0xA8 };
    binary_type::size_type expected_size = 54;

    bc::binary_type instance(initial_size, initial_blocks);

    instance.shift_right(distance);

    require_equal(expected_size, expected_blocks, instance);
}

//
// append tests
//
BOOST_AUTO_TEST_CASE(append_to_zero_length)
{
    data_chunk initial_blocks = { 0x00 };
    binary_type::size_type initial_size = 0;

    data_chunk appended_blocks = { 0xAA, 0xBB, 0xCC };
    binary_type::size_type appended_size = 24;
    bc::binary_type augment(appended_size, appended_blocks);

    data_chunk expected_blocks = { 0xAA, 0xBB, 0xCC };
    binary_type::size_type expected_size = 24;

    bc::binary_type instance(initial_size, initial_blocks);

    instance.append(augment);

    require_equal(expected_size, expected_blocks, instance);
}

BOOST_AUTO_TEST_CASE(append_zero_length_to_content)
{
    data_chunk initial_blocks = { 0xAA, 0xBB, 0xCC };
    binary_type::size_type initial_size = 24;

    data_chunk appended_blocks = { };
    binary_type::size_type appended_size = 0;
    bc::binary_type augment(appended_size, appended_blocks);

    data_chunk expected_blocks = { 0xAA, 0xBB, 0xCC };
    binary_type::size_type expected_size = 24;

    bc::binary_type instance(initial_size, initial_blocks);

    instance.append(augment);

    require_equal(expected_size, expected_blocks, instance);
}

BOOST_AUTO_TEST_CASE(append_byte_aligned_instances)
{
    data_chunk initial_blocks = { 0xAA, 0xBB, 0xCC };
    binary_type::size_type initial_size = 24;

    data_chunk appended_blocks = { 0xDD, 0xEE, 0xFF };
    binary_type::size_type appended_size = 24;
    bc::binary_type augment(appended_size, appended_blocks);

    data_chunk expected_blocks = { 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF };
    binary_type::size_type expected_size = 48;

    bc::binary_type instance(initial_size, initial_blocks);

    instance.append(augment);

    require_equal(expected_size, expected_blocks, instance);
}

BOOST_AUTO_TEST_CASE(append_byte_nonaligned_instances)
{
    data_chunk initial_blocks = { 0xAA, 0xBB, 0xCC };
    binary_type::size_type initial_size = 20;

    data_chunk appended_blocks = { 0xDD, 0xEE };
    binary_type::size_type appended_size = 13;
    bc::binary_type augment(appended_size, appended_blocks);

    data_chunk expected_blocks = { 0xAA, 0xBB, 0xCD, 0xDE, 0x80 };
    binary_type::size_type expected_size = 33;

    bc::binary_type instance(initial_size, initial_blocks);

    instance.append(augment);

    require_equal(expected_size, expected_blocks, instance);
}

//
// prepend tests
//
BOOST_AUTO_TEST_CASE(prepend_to_zero_length)
{
    data_chunk initial_blocks = { 0x00 };
    binary_type::size_type initial_size = 0;

    data_chunk appended_blocks = { 0xAA, 0xBB, 0xCC };
    binary_type::size_type appended_size = 24;
    bc::binary_type augment(appended_size, appended_blocks);

    data_chunk expected_blocks = { 0xAA, 0xBB, 0xCC };
    binary_type::size_type expected_size = 24;

    bc::binary_type instance(initial_size, initial_blocks);

    instance.prepend(augment);

    require_equal(expected_size, expected_blocks, instance);
}

BOOST_AUTO_TEST_CASE(prepend_zero_length_to_content)
{
    data_chunk initial_blocks = { 0xAA, 0xBB, 0xCC };
    binary_type::size_type initial_size = 24;

    data_chunk appended_blocks = { };
    binary_type::size_type appended_size = 0;
    bc::binary_type augment(appended_size, appended_blocks);

    data_chunk expected_blocks = { 0xAA, 0xBB, 0xCC };
    binary_type::size_type expected_size = 24;

    bc::binary_type instance(initial_size, initial_blocks);

    instance.prepend(augment);

    require_equal(expected_size, expected_blocks, instance);
}

BOOST_AUTO_TEST_CASE(prepend_byte_aligned_instances)
{
    data_chunk initial_blocks = { 0xAA, 0xBB, 0xCC };
    binary_type::size_type initial_size = 24;

    data_chunk appended_blocks = { 0xDD, 0xEE, 0xFF };
    binary_type::size_type appended_size = 24;
    bc::binary_type augment(appended_size, appended_blocks);

    data_chunk expected_blocks = { 0xDD, 0xEE, 0xFF, 0xAA, 0xBB, 0xCC };
    binary_type::size_type expected_size = 48;

    bc::binary_type instance(initial_size, initial_blocks);

    instance.prepend(augment);

    require_equal(expected_size, expected_blocks, instance);
}

BOOST_AUTO_TEST_CASE(prepend_byte_nonaligned_instances)
{
    data_chunk initial_blocks = { 0xAA, 0xBB, 0xCC };
    binary_type::size_type initial_size = 20;

    data_chunk appended_blocks = { 0xDD, 0xEE };
    binary_type::size_type appended_size = 13;
    bc::binary_type augment(appended_size, appended_blocks);

    data_chunk expected_blocks = { 0xDD, 0xED, 0x55, 0xDE, 0x00 };
    binary_type::size_type expected_size = 33;

    bc::binary_type instance(initial_size, initial_blocks);

    instance.prepend(augment);

    require_equal(expected_size, expected_blocks, instance);
}

//
// get_substring tests
//
BOOST_AUTO_TEST_CASE(get_substring_start_after_end)
{
    data_chunk initial_blocks = { 0xAA, 0xBB, 0xCC };
    binary_type::size_type initial_size = 20;

    binary_type::size_type start = 21;
    binary_type::size_type length = 10;

    data_chunk expected_blocks = {};
    binary_type::size_type expected_size = 0;

    bc::binary_type instance(initial_size, initial_blocks);

    binary_type result = instance.get_substring(start, length);

    require_equal(expected_size, expected_blocks, result);
}

BOOST_AUTO_TEST_CASE(get_substring_length_zero)
{
    data_chunk initial_blocks = { 0xAA, 0xBB, 0xCC };
    binary_type::size_type initial_size = 20;

    binary_type::size_type start = 5;
    binary_type::size_type length = 0;

    data_chunk expected_blocks = {};
    binary_type::size_type expected_size = 0;

    bc::binary_type instance(initial_size, initial_blocks);

    binary_type result = instance.get_substring(start, length);

    require_equal(expected_size, expected_blocks, result);
}

BOOST_AUTO_TEST_CASE(get_substring_byte_aligned_start)
{
    data_chunk initial_blocks = { 0xAA, 0xBB, 0xCC };
    binary_type::size_type initial_size = 20;

    binary_type::size_type start = 8;
    binary_type::size_type length = 10;

    data_chunk expected_blocks = { 0xBB, 0xC0 };
    binary_type::size_type expected_size = 10;

    bc::binary_type instance(initial_size, initial_blocks);

    binary_type result = instance.get_substring(start, length);

    require_equal(expected_size, expected_blocks, result);
}

BOOST_AUTO_TEST_CASE(get_substring_byte_nonaligned_start)
{
    data_chunk initial_blocks = { 0xAA, 0xBB, 0xCC };
    binary_type::size_type initial_size = 20;

    binary_type::size_type start = 10;
    binary_type::size_type length = 10;

    data_chunk expected_blocks = { 0xEF, 0x00 };
    binary_type::size_type expected_size = 10;

    bc::binary_type instance(initial_size, initial_blocks);

    binary_type result = instance.get_substring(start, length);

    require_equal(expected_size, expected_blocks, result);
}

BOOST_AUTO_TEST_CASE(get_substring_request_exceeds_string)
{
    data_chunk initial_blocks = { 0xAA, 0xBB, 0xCC };
    binary_type::size_type initial_size = 20;

    binary_type::size_type start = 10;
    binary_type::size_type length = 15;

    data_chunk expected_blocks = { 0xEF, 0x00 };
    binary_type::size_type expected_size = 10;

    bc::binary_type instance(initial_size, initial_blocks);

    binary_type result = instance.get_substring(start, length);

    require_equal(expected_size, expected_blocks, result);
}

BOOST_AUTO_TEST_SUITE_END()
