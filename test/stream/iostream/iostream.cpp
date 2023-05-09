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

BOOST_AUTO_TEST_SUITE(iostream_tests)

using seekdir = typename iostream<>::seekdir;
using pos_type = typename iostream<>::pos_type;
using iostate = typename iostream<>::iostate;

// setstate/rdstate/clear

BOOST_AUTO_TEST_CASE(iostream__setstate__goodbit__goodbit)
{
    data_chunk empty{};
    iostream<> stream{ empty };
    stream.setstate(iostream<>::goodbit);
    BOOST_REQUIRE(stream.rdstate() == iostream<>::goodbit);
}

BOOST_AUTO_TEST_CASE(iostream__setstate__eofbit__eofbit)
{
    auto chunk = base16_chunk("00010203040506070809");
    iostream<> stream{ chunk };
    stream.setstate(iostream<>::eofbit);
    BOOST_REQUIRE(stream.rdstate() == iostream<>::eofbit);

    stream.clear();
    BOOST_REQUIRE(stream.rdstate() == iostream<>::goodbit);
}

BOOST_AUTO_TEST_CASE(iostream__setstate__failbit__failbit)
{
    data_chunk empty{};
    iostream<> stream{ empty };
    stream.setstate(iostream<>::failbit);
    BOOST_REQUIRE(stream.rdstate() == iostream<>::failbit);

    stream.clear();
    BOOST_REQUIRE(stream.rdstate() == iostream<>::goodbit);
}

BOOST_AUTO_TEST_CASE(iostream__setstate__badbit__badbit)
{
    data_chunk empty{};
    iostream<> stream{ empty };
    stream.setstate(iostream<>::badbit);
    BOOST_REQUIRE(stream.rdstate() == iostream<>::badbit);

    stream.clear();
    BOOST_REQUIRE(stream.rdstate() == iostream<>::goodbit);
}

BOOST_AUTO_TEST_CASE(iostream__setstate__badbit_failbit__badbit_failbit)
{
    constexpr auto badfail = iostream<>::failbit | iostream<>::badbit;

    auto chunk = base16_chunk("00010203040506070809");
    iostream<> stream{ chunk };
    stream.setstate(iostream<>::badbit);
    stream.setstate(iostream<>::failbit);
    BOOST_REQUIRE(stream.rdstate() == badfail);

    stream.clear(iostream<>::badbit);
    BOOST_REQUIRE(stream.rdstate() == iostream<>::badbit);

    stream.clear(iostream<>::failbit);
    BOOST_REQUIRE(stream.rdstate() == iostream<>::failbit);

    stream.clear(iostream<>::goodbit);
    BOOST_REQUIRE(stream.rdstate() == iostream<>::goodbit);
}

// istream
// ----------------------------------------------------------------------------

// tellg

BOOST_AUTO_TEST_CASE(iostream__tellg__empty__zero_goodbit)
{
    data_chunk empty{};
    iostream<> stream{ empty };
    BOOST_REQUIRE(is_zero(stream.tellg()));
    BOOST_REQUIRE(stream.rdstate() == iostream<>::goodbit);
}

BOOST_AUTO_TEST_CASE(iostream__tellg__initial__zero_goodbit)
{
    auto chunk = base16_chunk("00010203040506070809");
    const iostream<> stream{ chunk };
    BOOST_REQUIRE(is_zero(stream.tellg()));
    BOOST_REQUIRE(stream.rdstate() == iostream<>::goodbit);
}

// iostream<>::beg

BOOST_AUTO_TEST_CASE(iostream__seekg__zero_from_begin__tellg_zero_goodbit)
{
    auto chunk = base16_chunk("00010203040506070809");
    iostream<> stream{ chunk };
    BOOST_REQUIRE_EQUAL(stream.seekg(3, iostream<>::beg).tellg(), 3);
    BOOST_REQUIRE(is_zero(stream.seekg(0, iostream<>::beg).tellg()));
    BOOST_REQUIRE(stream.rdstate() == iostream<>::goodbit);
}

BOOST_AUTO_TEST_CASE(iostream__seekg__three_from_begin__tellg_three_goodbit)
{
    auto chunk = base16_chunk("00010203040506070809");
    iostream<> stream{ chunk };
    BOOST_REQUIRE_EQUAL(stream.seekg(3, iostream<>::beg).tellg(), 3);
    BOOST_REQUIRE_EQUAL(stream.seekg(3, iostream<>::beg).tellg(), 3);
    BOOST_REQUIRE(stream.rdstate() == iostream<>::goodbit);
}

BOOST_AUTO_TEST_CASE(iostream__seekg__size_from_begin__tellg_size_goodbit)
{
    auto chunk = base16_chunk("00010203040506070809");
    iostream<> stream{ chunk };
    BOOST_REQUIRE_EQUAL(stream.seekg(3, iostream<>::beg).tellg(), 3);
    BOOST_REQUIRE_EQUAL(stream.seekg(chunk.size(), iostream<>::beg).tellg(), to_signed(chunk.size()));
    BOOST_REQUIRE(stream.rdstate() == iostream<>::goodbit);
}

BOOST_AUTO_TEST_CASE(iostream__seekg__overflow_from_begin__unchanged_badbit)
{
    auto chunk = base16_chunk("00010203040506070809");
    iostream<> stream{ chunk };
    BOOST_REQUIRE(is_zero(stream.seekg(add1(chunk.size()), iostream<>::beg).tellg()));
    BOOST_REQUIRE(stream.rdstate() == iostream<>::badbit);
}

// iostream<>::cur

BOOST_AUTO_TEST_CASE(iostream__seekg__zero_from_three_current__tellg_three_goodbit)
{
    auto chunk = base16_chunk("00010203040506070809");
    iostream<> stream{ chunk };
    BOOST_REQUIRE_EQUAL(stream.seekg(3, iostream<>::beg).tellg(), 3);
    BOOST_REQUIRE_EQUAL(stream.seekg(0, iostream<>::cur).tellg(), 3);
    BOOST_REQUIRE(stream.rdstate() == iostream<>::goodbit);
}

BOOST_AUTO_TEST_CASE(iostream__seekg__three_from_three_current__tellg_six_goodbit)
{
    auto chunk = base16_chunk("00010203040506070809");
    iostream<> stream{ chunk };
    BOOST_REQUIRE_EQUAL(stream.seekg(3, iostream<>::beg).tellg(), 3);
    BOOST_REQUIRE_EQUAL(stream.seekg(3, iostream<>::cur).tellg(), 6);
    BOOST_REQUIRE(stream.rdstate() == iostream<>::goodbit);
}

BOOST_AUTO_TEST_CASE(iostream__seekg__size_from_zero_current__tellg_size_goodbit)
{
    auto chunk = base16_chunk("00010203040506070809");
    iostream<> stream{ chunk };
    BOOST_REQUIRE_EQUAL(stream.seekg(chunk.size(), iostream<>::cur).tellg(), to_signed(chunk.size()));
    BOOST_REQUIRE(stream.rdstate() == iostream<>::goodbit);
}

BOOST_AUTO_TEST_CASE(iostream__seekg__overflow_from_three_current__unchanged_badbit)
{
    auto chunk = base16_chunk("00010203040506070809");
    iostream<> stream{ chunk };
    BOOST_REQUIRE_EQUAL(stream.seekg(3, iostream<>::beg).tellg(), 3);
    BOOST_REQUIRE(stream.seekg(chunk.size(), iostream<>::cur).tellg() == 3);
    BOOST_REQUIRE(stream.rdstate() == iostream<>::badbit);
}

// iostream<>::end

BOOST_AUTO_TEST_CASE(iostream__seekg__zero_from_end__tellg_size_goodbit)
{
    auto chunk = base16_chunk("00010203040506070809");
    iostream<> stream{ chunk };
    BOOST_REQUIRE_EQUAL(stream.seekg(3, iostream<>::beg).tellg(), 3);
    BOOST_REQUIRE_EQUAL(stream.seekg(0, iostream<>::end).tellg(), to_signed(chunk.size()));
    BOOST_REQUIRE(stream.rdstate() == iostream<>::goodbit);
}

BOOST_AUTO_TEST_CASE(iostream__seekg__negative_three_from_end__tellg_size_less_three_goodbit)
{
    auto chunk = base16_chunk("00010203040506070809");
    iostream<> stream{ chunk };
    BOOST_REQUIRE_EQUAL(stream.seekg(3, iostream<>::beg).tellg(), 3);
    BOOST_REQUIRE_EQUAL(stream.seekg(-3, iostream<>::end).tellg(), to_signed(chunk.size()) - 3);
    BOOST_REQUIRE(stream.rdstate() == iostream<>::goodbit);
}

BOOST_AUTO_TEST_CASE(iostream__seekg__negative_size_from_end__tellg_zero_goodbit)
{
    auto chunk = base16_chunk("00010203040506070809");
    iostream<> stream{ chunk };
    BOOST_REQUIRE(is_zero(stream.seekg(-to_signed(chunk.size()), iostream<>::end).tellg()));
    BOOST_REQUIRE(stream.rdstate() == iostream<>::goodbit);
}

BOOST_AUTO_TEST_CASE(iostream__seekg__underflow_from_end__unchanged_badbit)
{
    auto chunk = base16_chunk("00010203040506070809");
    iostream<> stream{ chunk };
    BOOST_REQUIRE(is_zero(stream.seekg(add1(chunk.size()), iostream<>::end).tellg()));
    BOOST_REQUIRE(stream.rdstate() == iostream<>::badbit);
}

// peek

BOOST_AUTO_TEST_CASE(iostream__peek__empty__eof_badbit)
{
    constexpr auto eof = std::char_traits<char>::eof();
    data_chunk empty{};
    iostream<> stream{ empty };
    BOOST_REQUIRE_EQUAL(stream.peek(), eof);
    BOOST_REQUIRE(stream.rdstate() == iostream<>::badbit);
}

BOOST_AUTO_TEST_CASE(iostream__peek__chunk__no_advance_expected_goodbit)
{
    auto chunk = base16_chunk("00010203040506070809");
    iostream<> stream{ chunk };
    BOOST_REQUIRE_EQUAL(stream.peek(), 0x00);
    BOOST_REQUIRE(is_zero(stream.tellg()));
    BOOST_REQUIRE_EQUAL(stream.peek(), 0x00);
    BOOST_REQUIRE(is_zero(stream.tellg()));
    BOOST_REQUIRE(stream.rdstate() == iostream<>::goodbit);
}

BOOST_AUTO_TEST_CASE(iostream__peek__end__eof_badbit)
{
    constexpr auto eof = std::char_traits<char>::eof();
    auto chunk = base16_chunk("00010203040506070809");
    iostream<> stream{ chunk };
    BOOST_REQUIRE_EQUAL(stream.seekg(0, iostream<>::end).tellg(), to_signed(chunk.size()));
    BOOST_REQUIRE_EQUAL(stream.peek(), eof);
    BOOST_REQUIRE(stream.rdstate() == iostream<>::badbit);
}

BOOST_AUTO_TEST_CASE(iostream__peek__chunk__advance_expected_goodbit)
{
    auto chunk = base16_chunk("00010203040506070809");
    iostream<> stream{ chunk };
    BOOST_REQUIRE_EQUAL(stream.peek(), 0x00);
    BOOST_REQUIRE_EQUAL(stream.seekg(1, iostream<>::cur).tellg(), 1);
    BOOST_REQUIRE_EQUAL(stream.peek(), 0x01);
    BOOST_REQUIRE_EQUAL(stream.seekg(1, iostream<>::cur).tellg(), 2);
    BOOST_REQUIRE_EQUAL(stream.peek(), 0x02);
    BOOST_REQUIRE_EQUAL(stream.seekg(1, iostream<>::cur).tellg(), 3);
    BOOST_REQUIRE_EQUAL(stream.peek(), 0x03);
    BOOST_REQUIRE_EQUAL(stream.seekg(1, iostream<>::cur).tellg(), 4);
    BOOST_REQUIRE_EQUAL(stream.peek(), 0x04);
    BOOST_REQUIRE_EQUAL(stream.seekg(1, iostream<>::cur).tellg(), 5);
    BOOST_REQUIRE_EQUAL(stream.peek(), 0x05);
    BOOST_REQUIRE_EQUAL(stream.seekg(1, iostream<>::cur).tellg(), 6);
    BOOST_REQUIRE_EQUAL(stream.peek(), 0x06);
    BOOST_REQUIRE_EQUAL(stream.seekg(1, iostream<>::cur).tellg(), 7);
    BOOST_REQUIRE_EQUAL(stream.peek(), 0x07);
    BOOST_REQUIRE_EQUAL(stream.seekg(1, iostream<>::cur).tellg(), 8);
    BOOST_REQUIRE_EQUAL(stream.peek(), 0x08);
    BOOST_REQUIRE_EQUAL(stream.seekg(1, iostream<>::cur).tellg(), 9);
    BOOST_REQUIRE_EQUAL(stream.peek(), 0x09);
    BOOST_REQUIRE(stream.rdstate() == iostream<>::goodbit);
}

// read

BOOST_AUTO_TEST_CASE(iostream__read__none_empty__goodbit)
{
    data_chunk buffer{};
    data_chunk empty{};
    iostream<> stream{ empty };
    stream.read(system::pointer_cast<char>(buffer.data()), buffer.size());
    BOOST_REQUIRE(stream.rdstate() == iostream<>::goodbit);
}

BOOST_AUTO_TEST_CASE(iostream__read__underflow_empty__badbit)
{
    auto buffer = base16_chunk("00000000000000000000");
    data_chunk empty{};
    iostream<> stream{ empty };
    stream.read(system::pointer_cast<char>(buffer.data()), buffer.size());
    BOOST_REQUIRE(stream.rdstate() == iostream<>::badbit);
}

BOOST_AUTO_TEST_CASE(iostream__read__underflow_nonempty__badbit)
{
    auto buffer = base16_chunk("00000000000000000000000000");
    auto chunk = base16_chunk("00010203040506070809");
    iostream<> stream{ chunk };
    stream.read(system::pointer_cast<char>(buffer.data()), add1(chunk.size()));
    BOOST_REQUIRE(stream.rdstate() == iostream<>::badbit);
}

BOOST_AUTO_TEST_CASE(iostream__read__full_buffer__goodbit)
{
    auto buffer = base16_chunk("00000000000000000000000000");
    auto chunk = base16_chunk("00010203040506070809");
    BOOST_REQUIRE_GE(buffer.size(), chunk.size());
    BOOST_REQUIRE_NE(buffer, chunk);

    iostream<> stream{ chunk };
    stream.read(system::pointer_cast<char>(buffer.data()), chunk.size());
    BOOST_REQUIRE(stream.rdstate() == iostream<>::goodbit);
    buffer.resize(chunk.size());
    BOOST_REQUIRE_EQUAL(buffer, chunk);
}

// reader

BOOST_AUTO_TEST_CASE(iostream__reader__read_8_bytes_big_endian__exected_goodbit)
{
    auto chunk = base16_chunk("010203040506070809");
    iostream<> stream{ chunk };
    byte_reader<iostream<>> reader{ stream };
    BOOST_REQUIRE_EQUAL(reader.read_8_bytes_big_endian(), 0x0102030405060708_u64);
    BOOST_REQUIRE(stream.rdstate() == iostream<>::goodbit);
}

// ostream
// ----------------------------------------------------------------------------

const auto chunk = base16_chunk("00010203040506070809");

// tellp

BOOST_AUTO_TEST_CASE(iostream__tellp__empty__zero_goodbit)
{
    data_chunk empty{};
    iostream<> stream{ empty };
    BOOST_REQUIRE(is_zero(stream.tellp()));
    BOOST_REQUIRE(stream.rdstate() == iostream<>::goodbit);
}

BOOST_AUTO_TEST_CASE(iostream__tellp__initial__zero_goodbit)
{
    data_chunk empty{};
    iostream<> stream{ empty };
    BOOST_REQUIRE(is_zero(stream.tellp()));
    BOOST_REQUIRE(stream.rdstate() == iostream<>::goodbit);
}

// write

BOOST_AUTO_TEST_CASE(iostream__write__none_empty__goodbit)
{
    const data_chunk buffer{};
    data_chunk empty{};
    iostream<> stream{ empty };
    stream.write(system::pointer_cast<const char>(buffer.data()), buffer.size());
    BOOST_REQUIRE(stream.rdstate() == iostream<>::goodbit);
}

BOOST_AUTO_TEST_CASE(iostream__write__overflow_empty__badbit)
{
    const auto buffer = base16_chunk("00010203040506070809");
    data_chunk empty{};
    iostream<> stream{ empty };
    stream.write(system::pointer_cast<const char>(buffer.data()), buffer.size());
    BOOST_REQUIRE(stream.rdstate() == iostream<>::badbit);
}

BOOST_AUTO_TEST_CASE(iostream__write__overflow_nonempty__badbit)
{
    const auto buffer = base16_chunk("00010203040506070809");
    auto chunk = base16_chunk("000000000000");
    iostream<> stream{ chunk };
    stream.write(system::pointer_cast<const char>(buffer.data()), buffer.size());
    BOOST_REQUIRE(stream.rdstate() == iostream<>::badbit);
}

BOOST_AUTO_TEST_CASE(iostream__write__full_buffer__goodbit)
{
    const auto buffer = base16_chunk("00010203040506070809");
    auto chunk = base16_chunk("00000000000000000000000000");
    BOOST_REQUIRE_GE(chunk.size(), buffer.size());
    BOOST_REQUIRE_NE(buffer, chunk);

    iostream<> stream{ chunk };
    stream.write(system::pointer_cast<const char>(buffer.data()), buffer.size());
    BOOST_REQUIRE(stream.rdstate() == iostream<>::goodbit);
    chunk.resize(buffer.size());
    BOOST_REQUIRE_EQUAL(buffer, chunk);
}

// writer

BOOST_AUTO_TEST_CASE(iostream__writer__write_8_bytes_big_endian__exected_goodbit)
{
    const auto expected = base16_chunk("010203040506070800");
    auto chunk = base16_chunk("000000000000000000");
    iostream<> stream{ chunk };
    byte_writer<iostream<>> writer{ stream };
    writer.write_8_bytes_big_endian(0x0102030405060708_u64);
    BOOST_REQUIRE(stream.rdstate() == iostream<>::goodbit);
    BOOST_REQUIRE_EQUAL(chunk, expected);
}

BOOST_AUTO_TEST_SUITE_END()
