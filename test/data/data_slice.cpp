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

BC_AUTO_TEST_SUITE(data_slice_tests)

// construct, empty

BC_AUTO_TEST_CASE(data_slice__construct__default__empty)
{
    // Empty data_slice.data() is nullptr, like std_array and std_vector.
    static_assert(is_null(data_array<0>{}.data()));
    BC_REQUIRE(is_null(data_chunk{}.data()));

    // requires constexpr.
    ////static_assert(is_null(data_chunk{}.data()));

    data_slice slice{};

    // properties (with value reads past end, zero padded)
    BC_REQUIRE(slice.empty());
    BC_REQUIRE_EQUAL(slice.size(), 0u);
    BC_REQUIRE_EQUAL(slice.data(), nullptr);
    BC_REQUIRE_EQUAL(slice.begin(), nullptr);
    BC_REQUIRE_EQUAL(slice.end(), nullptr);
    BC_REQUIRE_EQUAL(slice.front(), 0x00u);
    BC_REQUIRE_EQUAL(slice.back(), 0x00u);

    // methods
    ////BC_REQUIRE_EQUAL(slice.encoded(), "");
    BC_REQUIRE_EQUAL(slice.to_string(), "");
    BC_REQUIRE_EQUAL(slice.to_chunk(), data_chunk{});
    BC_REQUIRE_EQUAL(slice.to_array<0>(), data_array<0>{});

    // operator[] (value read past end, zero padded)
    BC_REQUIRE_EQUAL(slice[0], 0x00u);

    ////// cast operators
    ////BC_REQUIRE(static_cast<data_chunk>(slice).empty());
    ////BC_REQUIRE_EQUAL(static_cast<data_array<32>>(slice), null_hash);

    // operator==/operator!=
    BC_REQUIRE(slice == slice);
    BC_REQUIRE(!(slice != slice));
    BC_REQUIRE(slice == std::string{});
    BC_REQUIRE(!(slice != std::string{}));
    BC_REQUIRE(slice == data_slice{});
    BC_REQUIRE(!(slice != data_slice{}));
    BC_REQUIRE(slice == data_array<0>{});
    BC_REQUIRE(!(slice != data_array<0>{}));
    BC_REQUIRE(slice == "");
    BC_REQUIRE(!(slice != ""));
}

BC_AUTO_TEST_CASE(data_slice__construct__text_empty__empty)
{
    BC_REQUIRE(data_slice("").empty());
}

BC_AUTO_TEST_CASE(data_slice__construct__array_empty__empty)
{
    BC_REQUIRE(data_slice(data_array<0>{}).empty());
}

BC_AUTO_TEST_CASE(data_slice__construct__string_empty__empty)
{
    BC_REQUIRE(data_slice(std::string{}).empty());
}

BC_AUTO_TEST_CASE(data_slice__construct__vector_empty__empty)
{
    BC_REQUIRE(data_slice(data_chunk{}).empty());
}

BC_AUTO_TEST_CASE(data_slice__construct__pointers_empty__empty)
{
    const std::string value{ "foobar" };
    BC_REQUIRE(data_slice(&value[0], &value[0]).empty());
}

BC_AUTO_TEST_CASE(data_slice__construct__pointers_reversed__empty)
{
    const std::string value{ "foobar" };
    BC_REQUIRE(data_slice(&value[5], &value[0]).empty());
}

BC_AUTO_TEST_CASE(data_slice__construct__iterators_empty__empty)
{
    const data_chunk value{};
    BC_REQUIRE(data_slice(value.cbegin(), value.cend()).empty());
}

BC_AUTO_TEST_CASE(data_slice__construct__iterators_reversed__empty)
{
    const data_chunk value{ 'f', 'o', 'o', 'b', 'a', 'r' };
    BC_REQUIRE(data_slice(value.cend(), value.cbegin()).empty());
}

BC_AUTO_TEST_CASE(data_slice__to_array__past_end__zero_padded)
{
    const auto size = 10u;
    const data_array<size> expected{ { 'f', 'o', 'o', 'b', 'a', 'r', 0, 0, 0, 0 } };
    const data_slice slice("foobar");
    BC_REQUIRE_EQUAL(slice.size(), 6u);
    BC_REQUIRE_EQUAL(slice.to_array<size>(), expected);
}

// test vector
const auto size = 6u;
const std::string string{ "foobar" };
const data_chunk data{ 'f', 'o', 'o', 'b', 'a', 'r' };
const data_array<size> byte{ { 'f', 'o', 'o', 'b', 'a', 'r' } };
const std_vector<char> char_vector{ 'f', 'o', 'o', 'b', 'a', 'r' };
const std_array<char, size> char_array{ { 'f', 'o', 'o', 'b', 'a', 'r' } };
////const auto encoded = encode_base16(string);


// copy construct/assigment

BC_AUTO_TEST_CASE(data_slice__construct__copy__expected)
{
    const data_slice slice(string);
    const data_slice copy(slice);
    BC_REQUIRE_EQUAL(slice.size(), size);
    BC_REQUIRE_EQUAL(copy.size(), size);

    // size/data
    BC_REQUIRE(!slice.empty());
    BC_REQUIRE_EQUAL(slice.size(), size);
    BC_REQUIRE_EQUAL(*slice.data(), 'f');
    BC_REQUIRE_EQUAL(slice.back(), 'r');
    BC_REQUIRE_EQUAL(slice[sub1(size)], 'r');

    BC_REQUIRE(!copy.empty());
    BC_REQUIRE_EQUAL(copy.size(), size);
    BC_REQUIRE_EQUAL(*copy.data(), 'f');
    BC_REQUIRE_EQUAL(copy.back(), 'r');
    BC_REQUIRE_EQUAL(copy[sub1(size)], 'r');
}

BC_AUTO_TEST_CASE(data_slice__operator__assignment__expected)
{
    const data_slice slice(string);
    const auto copy = slice;
    BC_REQUIRE_EQUAL(slice.size(), size);
    BC_REQUIRE_EQUAL(copy.size(), size);

    // size/data
    BC_REQUIRE(!slice.empty());
    BC_REQUIRE_EQUAL(slice.size(), size);
    BC_REQUIRE_EQUAL(*slice.data(), 'f');
    BC_REQUIRE_EQUAL(slice.back(), 'r');
    BC_REQUIRE_EQUAL(slice[sub1(size)], 'r');

    BC_REQUIRE(!copy.empty());
    BC_REQUIRE_EQUAL(copy.size(), size);
    BC_REQUIRE_EQUAL(*copy.data(), 'f');
    BC_REQUIRE_EQUAL(copy.back(), 'r');
    BC_REQUIRE_EQUAL(copy[sub1(size)], 'r');
}

// construct, not empty

BC_AUTO_TEST_CASE(data_slice__construct__text__expected)
{
    // negative vector
    const std::string negative_string{ "baadf00d" };
    const data_chunk negative_data{ 'b', 'a', 'a', 'd', 'f', '0', '0', 'd' };
    const data_array<8> negative_byte{ { 'b', 'a', 'a', 'd', 'f', '0', '0', 'd' } };

    // construct(literal)
    const data_slice slice("foobar");

    // size/data
    BC_REQUIRE(!slice.empty());
    BC_REQUIRE_EQUAL(slice.size(), size);
    BC_REQUIRE_EQUAL(*slice.data(), 'f');

    // begin
    BC_REQUIRE_EQUAL(slice.front(), 'f');
    BC_REQUIRE_EQUAL(slice[0], 'f');
    BC_REQUIRE_EQUAL(*slice.begin(), 'f');
    ////BC_REQUIRE_EQUAL(static_cast<data_chunk>(slice).front(), 'f');
    ////BC_REQUIRE_EQUAL(static_cast<data_array<size>>(slice).front(), 'f');
    
    // end
    BC_REQUIRE_EQUAL(slice.back(), 'r');
    BC_REQUIRE_EQUAL(slice[sub1(size)], 'r');
    BC_REQUIRE_EQUAL(*std::prev(slice.end()), 'r');
    ////BC_REQUIRE_EQUAL(static_cast<data_chunk>(slice).back(), 'r');
    ////BC_REQUIRE_EQUAL(static_cast<data_array<size>>(slice).back(), 'r');

    // methods
    ////BC_REQUIRE_EQUAL(slice.encoded(), encoded);
    BC_REQUIRE_EQUAL(slice.to_string(), string);
    BC_REQUIRE_EQUAL(slice.to_chunk(), data);
    BC_REQUIRE_EQUAL(slice.to_array<size>(), byte);

    // operator==/operator!=

    // positive left
    BC_REQUIRE(slice == string);
    BC_REQUIRE(!(slice != string));
    BC_REQUIRE(slice == data);
    BC_REQUIRE(!(slice != data));
    BC_REQUIRE(slice == byte);
    BC_REQUIRE(!(slice != byte));
    BC_REQUIRE(slice == "foobar");
    BC_REQUIRE(!(slice != "foobar"));
    BC_REQUIRE_EQUAL(slice, string);
    BC_REQUIRE_EQUAL(slice, data);
    BC_REQUIRE_EQUAL(slice, byte);
    BC_REQUIRE_EQUAL(slice, "foobar");

    // negative left
    BC_REQUIRE(slice != negative_string);
    BC_REQUIRE(!(slice == negative_string));
    BC_REQUIRE(slice != negative_data);
    BC_REQUIRE(!(slice == negative_data));
    BC_REQUIRE(slice != negative_byte);
    BC_REQUIRE(!(slice == negative_byte));
    BC_REQUIRE(slice != "baadf00d");
    BC_REQUIRE(!(slice == "baadf00d"));
    BC_REQUIRE_NE(slice, negative_string);
    BC_REQUIRE_NE(slice, negative_data);
    BC_REQUIRE_NE(slice, negative_byte);
    BC_REQUIRE_NE(slice, "baadf00d");

    // positive right
    BC_REQUIRE(string == slice);
    BC_REQUIRE(!(string != slice));
    BC_REQUIRE(data == slice);
    BC_REQUIRE(!(data != slice));
    BC_REQUIRE(byte == slice);
    BC_REQUIRE(!(byte != slice));
    BC_REQUIRE("foobar" == slice);
    BC_REQUIRE(!("foobar" != slice));
    BC_REQUIRE_EQUAL(string, slice);
    BC_REQUIRE_EQUAL(data, slice);
    BC_REQUIRE_EQUAL(byte, slice);
    BC_REQUIRE_EQUAL(data_slice("foobar"), slice);

    // negative right
    BC_REQUIRE(negative_string != slice);
    BC_REQUIRE(!(negative_string == slice));
    BC_REQUIRE(negative_data != slice);
    BC_REQUIRE(!(negative_data == slice));
    BC_REQUIRE(negative_byte != slice);
    BC_REQUIRE(!(negative_byte == slice));
    BC_REQUIRE("baadf00d" != slice);
    BC_REQUIRE(!("baadf00d" == slice));
    BC_REQUIRE_NE(negative_string, slice);
    BC_REQUIRE_NE(negative_data, slice);
    BC_REQUIRE_NE(negative_byte, slice);
    BC_REQUIRE_NE(data_slice("baadf00d"), slice);

    // construct(literal [not initializer - text])
    const data_slice slice1{ "foobar" };
    BC_REQUIRE(!slice1.empty());
    BC_REQUIRE_EQUAL(slice1.size(), size);
    ////BC_REQUIRE_EQUAL(slice1.encoded(), encoded);

    // Invalid, use:
    // data_slice slice("foobar");
    ////// construct(literal [not initializer - text])
    ////const data_slice slice2({ "foobar" });
    ////BC_REQUIRE(!slice2.empty());
    ////BC_REQUIRE_EQUAL(slice2.size(), size);
    ////BC_REQUIRE_EQUAL(slice2.encoded(), encoded);

    // construct(literal [not initializer - text])
    const data_slice slice3 = { "foobar" };
    BC_REQUIRE(!slice3.empty());
    BC_REQUIRE_EQUAL(slice3.size(), size);
    ////BC_REQUIRE_EQUAL(slice3.encoded(), encoded);
}

BC_AUTO_TEST_CASE(data_slice__construct__array__expected)
{
    // construct(data_array)
    const data_slice slice0(byte);
    BC_REQUIRE(!slice0.empty());
    BC_REQUIRE_EQUAL(slice0.size(), size);
    ////BC_REQUIRE_EQUAL(slice0.encoded(), encoded);

    // construct(data_array)
    const data_slice slice1{ byte };
    BC_REQUIRE(!slice1.empty());
    BC_REQUIRE_EQUAL(slice1.size(), size);
    ////BC_REQUIRE_EQUAL(slice1.encoded(), encoded);

    // Deleted function, use:
    // data_slice slice(byte);
    ////// construct(data_array)
    ////const data_slice slice2 = byte;
    ////BC_REQUIRE(!slice2.empty());
    ////BC_REQUIRE_EQUAL(slice2.size(), size);
    ////BC_REQUIRE_EQUAL(slice2.encoded(), encoded);

    // Array construction is non-ambiguous.
    // Unnecessary copy construction.
    // construct(data_array)
    const data_slice slice3({ byte });
    BC_REQUIRE(!slice3.empty());
    BC_REQUIRE_EQUAL(slice3.size(), size);
    ////BC_REQUIRE_EQUAL(slice3.encoded(), encoded);

    // construct(char_array)
    const data_slice slice4(char_array);
    BC_REQUIRE(!slice4.empty());
    BC_REQUIRE_EQUAL(slice4.size(), size);
    ////BC_REQUIRE_EQUAL(slice4.encoded(), encoded);
}

BC_AUTO_TEST_CASE(data_slice__construct__initializer__expected)
{
    // Invalid construction
    ////const data_slice slice0('f', 'o', 'o', 'b', 'a', 'r');
    ////BC_WARN(!slice0.empty());
    ////BC_WARN_EQUAL(slice0.size(), size);
    ////BC_WARN_EQUAL(slice0.encoded(), encoded);

    // construct(initializer - data)
    const data_slice slice1{ 'f', 'o', 'o', 'b', 'a', 'r' };
    BC_WARN(!slice1.empty());
    BC_WARN_EQUAL(slice1.size(), size);
    ////BC_WARN_EQUAL(slice1.encoded(), encoded);

    // construct(initializer - data)
    const data_slice slice2 = { 'f', 'o', 'o', 'b', 'a', 'r' };
    BC_WARN(!slice2.empty());
    BC_WARN_EQUAL(slice2.size(), size);
    ////BC_WARN_EQUAL(slice2.encoded(), encoded);

    // Invalid construction
    ////const data_slice slice3(0, 1, 2, 3, 4, 5, 6, 7, 8, 9);
    ////BC_REQUIRE(!slice3.empty());
    ////BC_REQUIRE_EQUAL(slice3.size(), 10);
    ////BC_REQUIRE_EQUAL(slice3.encoded(), "00010203040506070809");

    // construct(initializer - data)
    const data_slice slice4{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
    BC_WARN(!slice4.empty());
    BC_WARN_EQUAL(slice4.size(), 10u);
    ////BC_WARN_EQUAL(slice4.encoded(), "00010203040506070809");

    // construct(initializer - data)
    const data_slice slice5 = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
    BC_WARN(!slice5.empty());
    BC_WARN_EQUAL(slice5.size(), 10u);
    ////BC_WARN_EQUAL(slice5.encoded(), "00010203040506070809");

    // construct(initializer - data)
    // The non-char terminator causes the data initializer to be picked up.
    const data_slice slice6({ 'f', 'o', 'o', 'b', 'a', 'r', 0 });
    BC_WARN(!slice6.empty());
    BC_WARN_EQUAL(slice6.size(), add1(size));
    ////BC_WARN_EQUAL(slice6.encoded(), encoded + "00");

    // construct(initializer - text)
    // All characters is picked up by the char array initializer.
    // But unlike literal char arrays, it is not automatically null terminated.
    // Because data_slice sees this as a literal, it removes the last character.
    // So this initializer requires explicit null termination.
    const data_slice slice7({ 'f', 'o', 'o', 'b', 'a', 'r', '\0' });
    BC_WARN(!slice7.empty());

    // TODO: This is failing on clang/gcc.
    ////BC_WARN_EQUAL(slice7.size(), size);
    ////BC_WARN_EQUAL(slice7.encoded(), encoded);
}

BC_AUTO_TEST_CASE(data_slice__construct__string__expected)
{
    // construct(string)
    const data_slice slice0(string);
    BC_REQUIRE(!slice0.empty());
    BC_REQUIRE_EQUAL(slice0.size(), size);
    ////BC_REQUIRE_EQUAL(slice0.encoded(), encoded);

    // construct(string)
    const data_slice slice1{ string };
    BC_REQUIRE(!slice1.empty());
    BC_REQUIRE_EQUAL(slice1.size(), size);
    ////BC_REQUIRE_EQUAL(slice1.encoded(), encoded);

    // Deleted function, use:
    // data_slice slice(string);
    ////// construct(string)
    ////const data_slice slice2 = string;
    ////BC_REQUIRE(!slice2.empty());
    ////BC_REQUIRE_EQUAL(slice2.size(), size);
    ////BC_REQUIRE_EQUAL(slice2.encoded(), encoded);

    // construct(string)
    const data_slice slice3 = { string };
    BC_REQUIRE(!slice3.empty());
    BC_REQUIRE_EQUAL(slice3.size(), size);
    ////BC_REQUIRE_EQUAL(slice3.encoded(), encoded);

    // Ambiguous, use:
    // data_slice slice(string);
    ////// Unnecessary copy construction.
    ////// construct(string)
    ////const data_slice slice4({ string });
    ////BC_REQUIRE(!slice4.empty());
    ////BC_REQUIRE_EQUAL(slice4.size(), size);
    ////BC_REQUIRE_EQUAL(slice4.encoded(), encoded);
}

BC_AUTO_TEST_CASE(data_slice__construct__vector__expected)
{
    // construct(data_chunk)
    const data_slice slice1(data);
    BC_REQUIRE(!slice1.empty());
    BC_REQUIRE_EQUAL(slice1.size(), size);
    ////BC_REQUIRE_EQUAL(slice1.encoded(), encoded);

    // Deleted function, use:
    // data_slice slice(data);
    ////// construct(data_chunk)
    ////const data_slice slice2 = data;
    ////BC_REQUIRE(!slice2.empty());
    ////BC_REQUIRE_EQUAL(slice2.size(), size);
    ////BC_REQUIRE_EQUAL(slice2.encoded(), encoded);

    // construct(data_chunk)
    const data_slice slice3 = { data };
    BC_REQUIRE(!slice3.empty());
    BC_REQUIRE_EQUAL(slice3.size(), size);
    ////BC_REQUIRE_EQUAL(slice3.encoded(), encoded);

    // Ambiguous, use:
    // data_slice slice(data);
    ////// Unnecessary copy construction.
    ////// construct(data_chunk)
    ////const data_slice slice4({ data });
    ////BC_REQUIRE(!slice4.empty());
    ////BC_REQUIRE_EQUAL(slice4.size(), size);
    ////BC_REQUIRE_EQUAL(slice4.encoded(), encoded);

    // construct(char_vector)
    const data_slice slice5(char_vector);
    BC_REQUIRE(!slice5.empty());
    BC_REQUIRE_EQUAL(slice5.size(), size);
    ////BC_REQUIRE_EQUAL(slice5.encoded(), encoded);
}

BC_AUTO_TEST_CASE(data_slice__construct__pointers__expected)
{
    // construct(pointers)
    const data_slice slice1(byte.data(), std::next(byte.data(), size));
    BC_REQUIRE(!slice1.empty());
    BC_REQUIRE_EQUAL(slice1.size(), size);
    ////BC_REQUIRE_EQUAL(slice1.encoded(), encoded);

    // construct(pointers)
    const data_slice slice2 = { byte.data(), std::next(byte.data(), size) };
    BC_REQUIRE(!slice2.empty());
    BC_REQUIRE_EQUAL(slice2.size(), size);
    ////BC_REQUIRE_EQUAL(slice2.encoded(), encoded);

    // Ambiguous, use:
    // data_slice slice(byte.data(), std::next(byte.data(), size));
    // Unnecessary copy construction.
    ////// construct(pointers)
    ////const data_slice slice3({ byte.data(), std::next(byte.data(), size) });
    ////BC_REQUIRE(!slice3.empty());
    ////BC_REQUIRE_EQUAL(slice3.size(), size);
    ////BC_REQUIRE_EQUAL(slice3.encoded(), encoded);
}

BC_AUTO_TEST_CASE(data_slice__construct__iterators__expected)
{
    // construct(iterators)
    const data_slice slice1(string.cbegin(), string.cend());
    BC_REQUIRE(!slice1.empty());
    BC_REQUIRE_EQUAL(slice1.size(), size);
    ////BC_REQUIRE_EQUAL(slice1.encoded(), encoded);

    // construct(iterators)
    const data_slice slice2 = { string.cbegin(), string.cend() };
    BC_REQUIRE(!slice2.empty());
    BC_REQUIRE_EQUAL(slice2.size(), size);
    ////BC_REQUIRE_EQUAL(slice2.encoded(), encoded);

    // Ambiguous, use:
    // data_slice slice(string.substr(,));
    // Unnecessary copy construction.
    ////// construct(iterators)
    ////const data_slice slice3({ string.begin(), string.end() });
    ////BC_REQUIRE(!slice3.empty());
    ////BC_REQUIRE_EQUAL(slice3.size(), size);
    ////BC_REQUIRE_EQUAL(slice3.encoded(), encoded);
}

// resize

BC_AUTO_TEST_CASE(data_slice__resize__empty__false)
{
    data_slice slice("");
    BC_REQUIRE(!slice.resize(0));
    BC_REQUIRE(!slice.resize(42));
}

BC_AUTO_TEST_CASE(data_slice__resize__downsize__true_expected)
{
    data_slice slice("foobar");
    BC_REQUIRE(slice.resize(3));
    BC_REQUIRE_EQUAL(slice.to_string(), "foo");
}

BC_AUTO_TEST_CASE(data_slice__resize__upsize__false_expected)
{
    data_slice slice("foobar");
    BC_REQUIRE(!slice.resize(6));
    BC_REQUIRE(!slice.resize(7));
    BC_REQUIRE(slice.resize(3));
    BC_REQUIRE(!slice.resize(3));
    BC_REQUIRE(!slice.resize(6));
    BC_REQUIRE_EQUAL(slice.to_string(), "foo");
}

BC_AUTO_TEST_SUITE_END()
