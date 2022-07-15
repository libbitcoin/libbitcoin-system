/////**
//// * Copyright (c) 2011-2022 libbitcoin developers (see AUTHORS)
//// *
//// * This file is part of libbitcoin.
//// *
//// * This program is free software: you can redistribute it and/or modify
//// * it under the terms of the GNU Affero General Public License as published by
//// * the Free Software Foundation, either version 3 of the License, or
//// * (at your option) any later version.
//// *
//// * This program is distributed in the hope that it will be useful,
//// * but WITHOUT ANY WARRANTY; without even the implied warranty of
//// * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//// * GNU Affero General Public License for more details.
//// *
//// * You should have received a copy of the GNU Affero General Public License
//// * along with this program.  If not, see <http://www.gnu.org/licenses/>.
//// */
////#include "../test.hpp"
////
////BOOST_AUTO_TEST_SUITE(sha256_context_tests)
////
////struct context_accessor
////  : sha256::context
////{
////    constexpr void do_set_size(size_t size) NOEXCEPT
////    {
////        context::set_size(possible_narrow_cast<uint32_t>(size));
////    }
////
////    constexpr size_t do_get_size() const NOEXCEPT
////    {
////        return context::get_size();
////    }
////};
////
////BOOST_AUTO_TEST_CASE(context__construct__always__initial)
////{
////    // context.buffer is not initialized as counter indicates zero logical size.
////    using namespace sha256;
////    const context context{};
////    BOOST_REQUIRE_EQUAL(context.counter[0], 0_u32);
////    BOOST_REQUIRE_EQUAL(context.counter[1], 0_u32);
////    BOOST_REQUIRE_EQUAL(context.state, initial);
////}
////
////BOOST_AUTO_TEST_CASE(context__reset__always__sets_initial)
////{
////    // context.buffer is not initialized as counter indicates zero logical size.
////    using namespace sha256;
////    context context{};
////    context.counter = { 24_u32, 42_u32 };
////    context.state = { 1_u8, 2_u8, 3_u8, 4_u8, 5_u8, 6_u8, 7_u8, 8_u8 };
////    context.reset();
////    BOOST_REQUIRE_EQUAL(context.counter[0], 0_u32);
////    BOOST_REQUIRE_EQUAL(context.counter[1], 0_u32);
////    BOOST_REQUIRE_EQUAL(context.state, initial);
////}
////
////BOOST_AUTO_TEST_CASE(context__sizes__initial__expected)
////{
////    using namespace sha256;
////    const context_accessor context{};
////    BOOST_REQUIRE_EQUAL(context.do_get_size(), zero);
////    BOOST_REQUIRE_EQUAL(context.gap_size(), block_size);
////    BOOST_REQUIRE_EQUAL(context.pad_size(), block_size - context::counter_size);
////}
////
////BOOST_AUTO_TEST_CASE(context__sizes__block_underflow__expected)
////{
////    using namespace sha256;
////    constexpr auto size = 42_size;
////    context_accessor context{};
////    context.do_set_size(size);
////    BOOST_REQUIRE_EQUAL(context.do_get_size(), size);
////    BOOST_REQUIRE_EQUAL(context.gap_size(), block_size - size);
////    BOOST_REQUIRE_EQUAL(context.pad_size(), block_size - size - context::counter_size);
////}
////
////BOOST_AUTO_TEST_CASE(context__sizes__block_exact__expected)
////{
////    using namespace sha256;
////    constexpr auto size = block_size;
////    context_accessor context{};
////    context.do_set_size(size);
////    BOOST_REQUIRE_EQUAL(context.do_get_size(), zero);
////    BOOST_REQUIRE_EQUAL(context.gap_size(), block_size);
////    BOOST_REQUIRE_EQUAL(context.pad_size(), block_size - context::counter_size);
////}
////
////BOOST_AUTO_TEST_CASE(context__sizes__block_overflow__expected)
////{
////    using namespace sha256;
////    constexpr auto overflow = 42_size;
////    constexpr auto size = block_size + overflow;
////    context_accessor context{};
////    context.do_set_size(size);
////    BOOST_REQUIRE_EQUAL(context.do_get_size(), overflow);
////    BOOST_REQUIRE_EQUAL(context.gap_size(), block_size - overflow);
////    BOOST_REQUIRE_EQUAL(context.pad_size(), block_size - overflow - context::counter_size);
////}
////
////BOOST_AUTO_TEST_CASE(context__sizes__max_uint32__expected)
////{
////    // This overflows bits by 8x.
////    using namespace sha256;
////    constexpr auto size = max_uint32;
////    constexpr auto expected_buffer = max_uint32 % block_size;
////    context_accessor context{};
////    context.do_set_size(size);
////    BOOST_REQUIRE_EQUAL(context.do_get_size(), expected_buffer);
////    BOOST_REQUIRE_EQUAL(context.gap_size(), block_size - expected_buffer);
////    BOOST_REQUIRE_EQUAL(context.pad_size(), two * block_size - expected_buffer - context::counter_size);
////}
////
////BOOST_AUTO_TEST_CASE(context__sizes__max_uint32_overflow__expected)
////{
////    // This overflows bits by 8x plus (42 * 8).
////    using namespace sha256;
////    constexpr auto size1 = max_uint32;
////    constexpr auto size2 = 42_size;
////    constexpr auto expected_buffer = ((max_uint32 % block_size) + size2) % block_size;
////    context_accessor context{};
////    context.do_set_size(size1);
////    context.do_set_size(size2);
////    BOOST_REQUIRE_EQUAL(context.do_get_size(), expected_buffer);
////    BOOST_REQUIRE_EQUAL(context.gap_size(), block_size - expected_buffer);
////    BOOST_REQUIRE_EQUAL(context.pad_size(), block_size - expected_buffer - context::counter_size);
////}
////
////BOOST_AUTO_TEST_CASE(context__add_data__fill_from_empty__filled_buffer)
////{
////    using namespace sha256;
////    data_array<block_size> buffer{};
////    buffer.fill(42_u8);
////    context_accessor context{};
////    context.add_data(block_size, buffer.data());
////    BOOST_REQUIRE_EQUAL(context.buffer, buffer);
////    BOOST_REQUIRE_EQUAL(context.do_get_size(), zero);
////}
////
////BOOST_AUTO_TEST_CASE(context__add_data__fill_from_fill_less_one__filled_last_byte_only)
////{
////    using namespace sha256;
////    constexpr auto expected = 42_u8;
////    data_array<block_size> buffer{};
////    buffer.fill(expected);
////    context_accessor context{};
////    context.buffer.fill(0_u8);
////    context.do_set_size(sub1(block_size));
////    context.add_data(block_size, buffer.data());
////    BOOST_REQUIRE_EQUAL(context.buffer[zero], 0_u8);
////    BOOST_REQUIRE_EQUAL(context.buffer[sub1(sub1(block_size))], 0_u8);
////    BOOST_REQUIRE_EQUAL(context.buffer[sub1(block_size)], expected);
////    BOOST_REQUIRE_EQUAL(context.do_get_size(), sub1(block_size));
////}
////
////BOOST_AUTO_TEST_CASE(context__set_data__fill_from_empty__filled_buffer_zero_size)
////{
////    using namespace sha256;
////    data_array<block_size> buffer{};
////    buffer.fill(42_u8);
////    context_accessor context{};
////    context.set_data(block_size, buffer.data());
////    BOOST_REQUIRE_EQUAL(context.buffer, buffer);
////    BOOST_REQUIRE_EQUAL(context.do_get_size(), zero);
////}
////
////#if !!defined(NDEBUG)
////
////// This will cause an assertion in debug builds.
////// This is an invalid set but is guarded against write past buffer end.
////// The modulo size is unchanged because the added size is exactly one block.
////BOOST_AUTO_TEST_CASE(context__set_data__one_block_from_fill_less_one__filled_last_byte_only_size_unchanged)
////{
////    using namespace sha256;
////    constexpr auto expected = 42_u8;
////    constexpr auto fill_size = block_size;
////    data_array<fill_size> fill{};
////    fill.fill(expected);
////    context_accessor context{};
////    context.buffer.fill(0_u8);
////    context.do_set_size(sub1(block_size));
////    context.set_data(fill_size, fill.data());
////    BOOST_REQUIRE_EQUAL(context.buffer[zero], 0_u8);
////    BOOST_REQUIRE_EQUAL(context.buffer[sub1(sub1(block_size))], 0_u8);
////    BOOST_REQUIRE_EQUAL(context.buffer[sub1(block_size)], expected);
////    BOOST_REQUIRE_EQUAL(context.do_get_size(), sub1(block_size));
////}
////
////// This will cause an assertion in debug builds.
////// This is also an invalid write, but not at a full block, so size changes.
////BOOST_AUTO_TEST_CASE(context__set_data__half_block_from_fill_less_one__filled_last_byte_only_size_halved)
////{
////    using namespace sha256;
////    constexpr auto expected = 42_u8;
////    constexpr auto fill_size = to_half(block_size);
////    data_array<fill_size> fill{};
////    fill.fill(expected);
////    context_accessor context{};
////    context.buffer.fill(0_u8);
////    context.do_set_size(sub1(block_size));
////    context.set_data(fill_size, fill.data());
////    BOOST_REQUIRE_EQUAL(context.buffer[zero], 0_u8);
////    BOOST_REQUIRE_EQUAL(context.buffer[sub1(sub1(block_size))], 0_u8);
////    BOOST_REQUIRE_EQUAL(context.buffer[sub1(block_size)], expected);
////    BOOST_REQUIRE_EQUAL(context.do_get_size(), (sub1(block_size) + fill_size) % block_size);
////}
////
////#endif // !NDEBUG
////
////BOOST_AUTO_TEST_CASE(context__set_data__half_block_from_half_full__filled_size_zero)
////{
////    using namespace sha256;
////    constexpr auto expected = 42_u8;
////    constexpr auto fill_size = to_half(block_size);
////    data_array<fill_size> fill{};
////    fill.fill(expected);
////    context_accessor context{};
////    context.buffer.fill(0_u8);
////    context.do_set_size(to_half(block_size));
////    context.set_data(fill_size, fill.data());
////    BOOST_REQUIRE(std::equal(std::next(context.buffer.begin(), fill_size), context.buffer.end(), fill.begin(), fill.end()));
////    BOOST_REQUIRE_EQUAL(context.do_get_size(), zero);
////}
////
////BOOST_AUTO_TEST_CASE(context__serialize_counter__initial__zeros)
////{
////    using namespace sha256;
////    constexpr data_array<context::counter_size> expected{ 0_u8 };
////    const context context{};
////    BOOST_REQUIRE_EQUAL(context.serialize_counter(), expected);
////}
////
////BOOST_AUTO_TEST_CASE(context__serialize_counter__values__expected)
////{
////    using namespace sha256;
////    constexpr context::count counter
////    {
////        native_to_little_end(0x01234567_u32),
////        native_to_little_end(0x89abcdef_u32)
////    };
////
////    constexpr data_array<context::counter_size> expected
////    {
////        0x01_u8, 0x23_u8, 0x45_u8, 0x67_u8, 0x89_u8, 0xab_u8, 0xcd_u8, 0xef_u8
////    };
////
////    context context{};
////    context.counter = counter;
////    BOOST_REQUIRE_EQUAL(context.serialize_counter(), expected);
////}
////
////BOOST_AUTO_TEST_SUITE_END()
