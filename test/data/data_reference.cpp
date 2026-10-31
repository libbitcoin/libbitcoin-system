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

BC_AUTO_TEST_SUITE(data_reference_tests)

// test vector
const auto size = 6u;
const std::string string{ "foobar" };
const data_chunk data{ 'f', 'o', 'o', 'b', 'a', 'r' };
const data_array<size> byte{ { 'f', 'o', 'o', 'b', 'a', 'r' } };
const std_vector<char> char_vector{ 'f', 'o', 'o', 'b', 'a', 'r' };
const std_array<char, size> char_array{ { 'f', 'o', 'o', 'b', 'a', 'r' } };
////const auto encoded = encode_base16(string);

// construct, empty

BC_AUTO_TEST_CASE(data_reference__construct__default__empty)
{
    const data_reference reference{};
    const data_reference reference2(string);

    // properties (with value reads past end, zero padded)
    BC_REQUIRE(reference.empty());
    BC_REQUIRE_EQUAL(reference.size(), 0u);
    BC_REQUIRE_EQUAL(reference.data(), nullptr);
    BC_REQUIRE_EQUAL(reference.begin(), nullptr);
    BC_REQUIRE_EQUAL(reference.end(), nullptr);
    BC_REQUIRE_EQUAL(reference.front(), 0x00);
    BC_REQUIRE_EQUAL(reference.back(), 0x00);

    // methods
    ////BC_REQUIRE_EQUAL(reference.encoded(), "");
    BC_REQUIRE_EQUAL(reference.to_string(), "");
    BC_REQUIRE_EQUAL(reference.to_chunk(), data_chunk{});
    BC_REQUIRE_EQUAL(reference.to_array<0>(), data_array<0>{});

    // operator[] (value read past end, zero padded)
    BC_REQUIRE_EQUAL(reference[0], 0x00);

    ////// cast operators
    ////BC_REQUIRE(static_cast<data_chunk>(reference).empty());
    ////BC_REQUIRE_EQUAL(static_cast<data_array<32>>(reference), null_hash);

    // operator==/operator!=
    BC_REQUIRE(reference == reference);
    BC_REQUIRE(!(reference != reference));
    BC_REQUIRE(!(reference == reference2));
    BC_REQUIRE(reference != reference2);
}

BC_AUTO_TEST_CASE(data_reference__construct__pointers_empty__empty)
{
    const std::string value{ "foobar" };
    BC_REQUIRE(data_reference(&value[0], &value[0]).empty());
}

BC_AUTO_TEST_CASE(data_reference__construct__pointers_reversed__empty)
{
    const std::string value{ "foobar" };
    BC_REQUIRE(data_reference(&value[5], &value[0]).empty());
}

BC_AUTO_TEST_CASE(data_reference__construct__iterators_empty__empty)
{
    const data_chunk value{};
    BC_REQUIRE(data_reference(value.cbegin(), value.cend()).empty());
}

BC_AUTO_TEST_CASE(data_reference__construct__iterators_reversed__empty)
{
    const data_chunk value{ 'f', 'o', 'o', 'b', 'a', 'r' };
    BC_REQUIRE(data_reference(value.cend(), value.cbegin()).empty());
}

// copy construct

BC_AUTO_TEST_CASE(data_reference__construct__copy__expected)
{
    const data_reference reference(string);
    const data_reference copy(reference);
    BC_REQUIRE_EQUAL(reference.size(), size);

    // size/data
    BC_REQUIRE(!copy.empty());
    BC_REQUIRE_EQUAL(copy.size(), size);
    BC_REQUIRE_EQUAL(*copy.data(), 'f');
    BC_REQUIRE_EQUAL(copy.back(), 'r');
    BC_REQUIRE_EQUAL(copy[sub1(size)], 'r');
}

// construct, not empty

BC_AUTO_TEST_CASE(data_reference__construct__string__expected)
{
    // negative vector
    const std::string negative_string{ "baadf00d" };
    const data_chunk negative_data{ 'b', 'a', 'a', 'd', 'f', '0', '0', 'd' };
    const data_array<8> negative_byte{ { 'b', 'a', 'a', 'd', 'f', '0', '0', 'd' } };

    // construct(literal)
    const data_reference reference(string);

    // size/data
    BC_REQUIRE(!reference.empty());
    BC_REQUIRE_EQUAL(reference.size(), size);
    BC_REQUIRE_EQUAL(*reference.data(), 'f');

    // begin
    BC_REQUIRE_EQUAL(reference.front(), 'f');
    BC_REQUIRE_EQUAL(reference[0], 'f');
    BC_REQUIRE_EQUAL(*reference.begin(), 'f');
    ////BC_REQUIRE_EQUAL(static_cast<data_chunk>(reference).front(), 'f');
    ////BC_REQUIRE_EQUAL(static_cast<data_array<size>>(reference).front(), 'f');
    
    // end
    BC_REQUIRE_EQUAL(reference.back(), 'r');
    BC_REQUIRE_EQUAL(reference[sub1(size)], 'r');
    BC_REQUIRE_EQUAL(*std::prev(reference.end()), 'r');
    ////BC_REQUIRE_EQUAL(static_cast<data_chunk>(reference).back(), 'r');
    ////BC_REQUIRE_EQUAL(static_cast<data_array<size>>(reference).back(), 'r');

    // methods
    ////BC_REQUIRE_EQUAL(reference.encoded(), encoded);
    BC_REQUIRE_EQUAL(reference.to_string(), string);
    BC_REQUIRE_EQUAL(reference.to_chunk(), data);
    BC_REQUIRE_EQUAL(reference.to_array<size>(), byte);

    // operator==/operator!=

    // positive left
    BC_REQUIRE(reference == string);
    BC_REQUIRE(!(reference != string));
    BC_REQUIRE(reference == data);
    BC_REQUIRE(!(reference != data));
    BC_REQUIRE(reference == byte);
    BC_REQUIRE(!(reference != byte));
    BC_REQUIRE_EQUAL(reference, string);
    BC_REQUIRE_EQUAL(reference, data);
    BC_REQUIRE_EQUAL(reference, byte);

    // negative left
    BC_REQUIRE(reference != negative_string);
    BC_REQUIRE(!(reference == negative_string));
    BC_REQUIRE(reference != negative_data);
    BC_REQUIRE(!(reference == negative_data));
    BC_REQUIRE(reference != negative_byte);
    BC_REQUIRE(!(reference == negative_byte));
    BC_REQUIRE_NE(reference, negative_string);
    BC_REQUIRE_NE(reference, negative_data);
    BC_REQUIRE_NE(reference, negative_byte);

    // positive right
    BC_REQUIRE(string == reference);
    BC_REQUIRE(!(string != reference));
    BC_REQUIRE(data == reference);
    BC_REQUIRE(!(data != reference));
    BC_REQUIRE(byte == reference);
    BC_REQUIRE(!(byte != reference));
    BC_REQUIRE_EQUAL(string, reference);
    BC_REQUIRE_EQUAL(data, reference);
    BC_REQUIRE_EQUAL(byte, reference);

    // negative right
    BC_REQUIRE(negative_string != reference);
    BC_REQUIRE(!(negative_string == reference));
    BC_REQUIRE(negative_data != reference);
    BC_REQUIRE(!(negative_data == reference));
    BC_REQUIRE(negative_byte != reference);
    BC_REQUIRE(!(negative_byte == reference));
    BC_REQUIRE_NE(negative_string, reference);
    BC_REQUIRE_NE(negative_data, reference);
    BC_REQUIRE_NE(negative_byte, reference);
}

// base16 removed from data_slice (circular in constexpr).

BC_AUTO_TEST_CASE(data_reference__construct__array__expected)
{
    // construct(data_array)
    const data_reference reference0(byte);
    BC_REQUIRE(!reference0.empty());
    BC_REQUIRE_EQUAL(reference0.size(), size);
    ////BC_REQUIRE_EQUAL(reference0.encoded(), encoded);

    // construct(data_array)
    const data_reference reference1{ byte };
    BC_REQUIRE(!reference1.empty());
    BC_REQUIRE_EQUAL(reference1.size(), size);
    ////BC_REQUIRE_EQUAL(reference1.encoded(), encoded);

    // construct(char_array)
    const data_reference reference4(char_array);
    BC_REQUIRE(!reference4.empty());
    BC_REQUIRE_EQUAL(reference4.size(), size);
    ////BC_REQUIRE_EQUAL(reference4.encoded(), encoded);
}

BC_AUTO_TEST_CASE(data_reference__construct__vector__expected)
{
    // construct(data_chunk)
    const data_reference reference1(data);
    BC_REQUIRE(!reference1.empty());
    BC_REQUIRE_EQUAL(reference1.size(), size);
    ////BC_REQUIRE_EQUAL(reference1.encoded(), encoded);

    // construct(data_chunk)
    const data_reference reference3 = { data };
    BC_REQUIRE(!reference3.empty());
    BC_REQUIRE_EQUAL(reference3.size(), size);
    ////BC_REQUIRE_EQUAL(reference3.encoded(), encoded);

    // construct(char_vector)
    const data_reference reference5(char_vector);
    BC_REQUIRE(!reference5.empty());
    BC_REQUIRE_EQUAL(reference5.size(), size);
    ////BC_REQUIRE_EQUAL(reference5.encoded(), encoded);
}

BC_AUTO_TEST_CASE(data_reference__construct__pointers__expected)
{
    // construct(pointers)
    const data_reference reference1(byte.data(), std::next(byte.data(), size));
    BC_REQUIRE(!reference1.empty());
    BC_REQUIRE_EQUAL(reference1.size(), size);
    ////BC_REQUIRE_EQUAL(reference1.encoded(), encoded);

    // construct(pointers)
    const data_reference reference2 = { byte.data(), std::next(byte.data(), size) };
    BC_REQUIRE(!reference2.empty());
    BC_REQUIRE_EQUAL(reference2.size(), size);
    ////BC_REQUIRE_EQUAL(reference2.encoded(), encoded);
}

BC_AUTO_TEST_CASE(data_reference__construct__iterators__expected)
{
    // construct(iterators)
    const data_reference reference1(string.cbegin(), string.cend());
    BC_REQUIRE(!reference1.empty());
    BC_REQUIRE_EQUAL(reference1.size(), size);
    ////BC_REQUIRE_EQUAL(reference1.encoded(), encoded);

    // construct(iterators)
    const data_reference reference2 = { string.cbegin(), string.cend() };
    BC_REQUIRE(!reference2.empty());
    BC_REQUIRE_EQUAL(reference2.size(), size);
    ////BC_REQUIRE_EQUAL(reference2.encoded(), encoded);
}

// resize

BC_AUTO_TEST_CASE(data_reference__resize__empty__false)
{
    const std::string empty;
    data_reference reference(empty);
    BC_REQUIRE(!reference.resize(0));
    BC_REQUIRE(!reference.resize(42));
}

BC_AUTO_TEST_CASE(data_reference__resize__downsize__true_expected)
{
    data_reference reference(string);
    BC_REQUIRE(reference.resize(3));
    BC_REQUIRE_EQUAL(reference.to_string(), "foo");
}

BC_AUTO_TEST_CASE(data_reference__resize__upsize__false_expected)
{
    data_reference reference(string);
    BC_REQUIRE(!reference.resize(6));
    BC_REQUIRE(!reference.resize(7));
    BC_REQUIRE(reference.resize(3));
    BC_REQUIRE(!reference.resize(3));
    BC_REQUIRE(!reference.resize(6));
    BC_REQUIRE_EQUAL(reference.to_string(), "foo");
}

BC_AUTO_TEST_SUITE_END()
