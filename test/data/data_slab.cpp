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

BC_AUTO_TEST_SUITE(data_slab_tests)

// construct, empty

BC_AUTO_TEST_CASE(data_slab__construct__default__empty)
{
    data_slab slab{};
    const data_array<0> empty_array{};
    const data_slab empty_slab{};

    // properties (with value reads past end, zero padded)
    BC_REQUIRE(slab.empty());
    BC_REQUIRE_EQUAL(slab.size(), 0u);
    BC_REQUIRE_EQUAL(slab.data(), nullptr);
    BC_REQUIRE_EQUAL(slab.begin(), nullptr);
    BC_REQUIRE_EQUAL(slab.end(), nullptr);
    BC_REQUIRE_EQUAL(slab.front(), 0x00);
    BC_REQUIRE_EQUAL(slab.back(), 0x00);

    // methods
    ////BC_REQUIRE_EQUAL(slab.encoded(), "");
    BC_REQUIRE_EQUAL(slab.to_string(), "");
    BC_REQUIRE_EQUAL(slab.to_chunk(), data_chunk{});
    BC_REQUIRE_EQUAL(slab.to_array<0>(), data_array<0>{});

    // operator[] (value read past end, zero padded)
    BC_REQUIRE_EQUAL(slab[0], 0x00);

    // cast operators
    ////BC_REQUIRE(static_cast<data_chunk>(slab).empty());
    ////BC_REQUIRE_EQUAL(static_cast<data_array<32>>(slab), null_hash);

    // operator==/operator!=
    BC_REQUIRE(slab == slab);
    BC_REQUIRE(!(slab != slab));
    BC_REQUIRE(slab == empty_slab);
    BC_REQUIRE(!(slab != empty_slab));
    BC_REQUIRE(slab == empty_array);
    BC_REQUIRE(!(slab != empty_array));
}

BC_AUTO_TEST_CASE(data_slab__construct__array_empty__empty)
{
    const data_array<0> slab{};
    BC_REQUIRE(slab.empty());
}

BC_AUTO_TEST_CASE(data_slab__construct__pointers_empty__empty)
{
    std::string value{ "foobar" };
    BC_REQUIRE(data_slab(&value[0], &value[0]).empty());
}

BC_AUTO_TEST_CASE(data_slab__construct__pointers_reversed__empty)
{
    std::string value{ "foobar" };
    BC_REQUIRE(data_slab(&value[5], &value[0]).empty());
}

BC_AUTO_TEST_CASE(data_slab__construct__iterators_empty__empty)
{
    data_chunk value{};
    const data_slab slab(value.begin(), value.end());
    BC_REQUIRE(slab.empty());
}

BC_AUTO_TEST_CASE(data_slab__construct__iterators_reversed__empty)
{
    data_chunk value{ 'f', 'o', 'o', 'b', 'a', 'r' };
    const data_slab slab(value.end(), value.begin());
    BC_REQUIRE(slab.empty());
}

// test vector (careful to not mutate in tests).
const auto size = 6u;
std::string string{ "foobar" };
data_chunk data{ 'f', 'o', 'o', 'b', 'a', 'r' };
data_array<size> byte{ { 'f', 'o', 'o', 'b', 'a', 'r' } };
std_array<char, size> char_array{ { 'f', 'o', 'o', 'b', 'a', 'r' } };
////auto encoded = encode_base16(string);

// copy construct

BC_AUTO_TEST_CASE(data_slab__construct__copy__expected)
{
    const data_slab slab(byte);
    const data_slab copy(slab);
    BC_REQUIRE_EQUAL(copy.size(), size);

    // size/data
    BC_REQUIRE(!copy.empty());
    BC_REQUIRE_EQUAL(copy.size(), size);
    BC_REQUIRE_EQUAL(*copy.data(), 'f');
    BC_REQUIRE_EQUAL(copy.back(), 'r');
    BC_REQUIRE_EQUAL(copy[sub1(size)], 'r');
}

// construct, not empty

BC_AUTO_TEST_CASE(data_slab__construct__text__expected)
{
    // negative vector
    std::string negative_string{ "baadf00d" };
    data_chunk negative_data{ 'b', 'a', 'a', 'd', 'f', '0', '0', 'd' };
    data_array<8> negative_byte{ { 'b', 'a', 'a', 'd', 'f', '0', '0', 'd' } };

    // construct(string reference)
    const data_slab slab(string);

    // size/data
    BC_REQUIRE(!slab.empty());
    BC_REQUIRE_EQUAL(slab.size(), size);
    BC_REQUIRE_EQUAL(*slab.data(), 'f');

    // begin
    BC_REQUIRE_EQUAL(slab.front(), 'f');
    BC_REQUIRE_EQUAL(slab[0], 'f');
    BC_REQUIRE_EQUAL(*slab.begin(), 'f');
    ////BC_REQUIRE_EQUAL(static_cast<data_chunk>(slab).front(), 'f');
    ////BC_REQUIRE_EQUAL(static_cast<data_array<size>>(slab).front(), 'f');
    
    // end
    BC_REQUIRE_EQUAL(slab.back(), 'r');
    BC_REQUIRE_EQUAL(slab[sub1(size)], 'r');
    BC_REQUIRE_EQUAL(*std::prev(slab.end()), 'r');
    ////BC_REQUIRE_EQUAL(static_cast<data_chunk>(slab).back(), 'r');
    BC_REQUIRE_EQUAL(static_cast<data_slice>(slab).back(), 'r');
    ////BC_REQUIRE_EQUAL(static_cast<data_array<size>>(slab).back(), 'r');

    // methods
    ////BC_REQUIRE_EQUAL(slab.encoded(), encoded);
    BC_REQUIRE_EQUAL(slab.to_string(), string);
    BC_REQUIRE_EQUAL(slab.to_chunk(), data);
    BC_REQUIRE_EQUAL(slab.to_slice(), data);
    BC_REQUIRE_EQUAL(slab.to_array<size>(), byte);

    // operator==/operator!=

    // positive left
    BC_REQUIRE(slab == string);
    BC_REQUIRE(!(slab != string));
    BC_REQUIRE(slab == data);
    BC_REQUIRE(!(slab != data));
    BC_REQUIRE(slab == byte);
    BC_REQUIRE(!(slab != byte));
    BC_REQUIRE_EQUAL(slab, string);
    BC_REQUIRE_EQUAL(slab, data);
    BC_REQUIRE_EQUAL(slab, byte);

    // negative left
    BC_REQUIRE(slab != negative_string);
    BC_REQUIRE(!(slab == negative_string));
    BC_REQUIRE(slab != negative_data);
    BC_REQUIRE(!(slab == negative_data));
    BC_REQUIRE(slab != negative_byte);
    BC_REQUIRE(!(slab == negative_byte));
    BC_REQUIRE_NE(slab, negative_string);
    BC_REQUIRE_NE(slab, negative_data);
    BC_REQUIRE_NE(slab, negative_byte);

    // positive right
    BC_REQUIRE(string == slab);
    BC_REQUIRE(!(string != slab));
    BC_REQUIRE(data == slab);
    BC_REQUIRE(!(data != slab));
    BC_REQUIRE(byte == slab);
    BC_REQUIRE(!(byte != slab));
    BC_REQUIRE_EQUAL(string, slab);
    BC_REQUIRE_EQUAL(data, slab);
    BC_REQUIRE_EQUAL(byte, slab);

    // negative right
    BC_REQUIRE(negative_string != slab);
    BC_REQUIRE(!(negative_string == slab));
    BC_REQUIRE(negative_data != slab);
    BC_REQUIRE(!(negative_data == slab));
    BC_REQUIRE(negative_byte != slab);
    BC_REQUIRE(!(negative_byte == slab));
    BC_REQUIRE_NE(negative_string, slab);
    BC_REQUIRE_NE(negative_data, slab);
    BC_REQUIRE_NE(negative_byte, slab);
}

BC_AUTO_TEST_CASE(data_slab__construct__array__expected)
{
    // construct(data_array)
    const data_slab slab0(byte);
    BC_REQUIRE(!slab0.empty());
    BC_REQUIRE_EQUAL(slab0.size(), size);
    ////BC_REQUIRE_EQUAL(slab0.encoded(), encoded);

    // construct(data_array)
    const data_slab slab1{ byte };
    BC_REQUIRE(!slab1.empty());
    BC_REQUIRE_EQUAL(slab1.size(), size);
    ////BC_REQUIRE_EQUAL(slab1.encoded(), encoded);

    // Array construction is non-ambiguous.
    // Unnecessary copy construction.
    // construct(data_array)
    const data_slab slab3({ byte });
    BC_REQUIRE(!slab3.empty());
    BC_REQUIRE_EQUAL(slab3.size(), size);
    ////BC_REQUIRE_EQUAL(slab3.encoded(), encoded);

    // construct(char_array)
    const data_slab slab4(char_array);
    BC_REQUIRE(!slab4.empty());
    BC_REQUIRE_EQUAL(slab4.size(), size);
    ////BC_REQUIRE_EQUAL(slab4.encoded(), encoded);
}

BC_AUTO_TEST_CASE(data_slab__construct__pointers__expected)
{
    // construct(pointers)
    const data_slab slab1(byte.data(), std::next(byte.data(), size));
    BC_REQUIRE(!slab1.empty());
    BC_REQUIRE_EQUAL(slab1.size(), size);
    ////BC_REQUIRE_EQUAL(slab1.encoded(), encoded);

    // construct(pointers)
    const data_slab slab2 = { byte.data(), std::next(byte.data(), size) };
    BC_REQUIRE(!slab2.empty());
    BC_REQUIRE_EQUAL(slab2.size(), size);
    ////BC_REQUIRE_EQUAL(slab2.encoded(), encoded);
}

BC_AUTO_TEST_CASE(data_slab__construct__iterators__expected)
{
    // construct(iterators)
    const data_slab slab1(string.begin(), string.end());
    BC_REQUIRE(!slab1.empty());
    BC_REQUIRE_EQUAL(slab1.size(), size);
    ////BC_REQUIRE_EQUAL(slab1.encoded(), encoded);

    // construct(iterators)
    const data_slab slab2 = { string.begin(), string.end() };
    BC_REQUIRE(!slab2.empty());
    BC_REQUIRE_EQUAL(slab2.size(), size);
    ////BC_REQUIRE_EQUAL(slab2.encoded(), encoded);
}

// resize

BC_AUTO_TEST_CASE(data_slab__resize__empty__false)
{
    data_array<0> empty{ {} };
    data_slab slab(empty);
    BC_REQUIRE(!slab.resize(0));
    BC_REQUIRE(!slab.resize(42));
}

BC_AUTO_TEST_CASE(data_slab__resize__downsize__true_expected)
{
    data_array<6> data_value{ { 'f', 'o', 'o', 'b', 'a', 'r' } };
    data_slab slab(data_value);
    BC_REQUIRE(slab.resize(3));
    BC_REQUIRE_EQUAL(slab.to_string(), "foo");
}

BC_AUTO_TEST_CASE(data_slab__resize__upsize__false_expected)
{
    data_array<6> data_value{ { 'f', 'o', 'o', 'b', 'a', 'r' } };
    data_slab slab(data_value);
    BC_REQUIRE(!slab.resize(6));
    BC_REQUIRE(!slab.resize(7));
    BC_REQUIRE(slab.resize(3));
    BC_REQUIRE(!slab.resize(3));
    BC_REQUIRE(!slab.resize(6));
    BC_REQUIRE_EQUAL(slab.to_string(), "foo");
}

BC_AUTO_TEST_SUITE_END()
