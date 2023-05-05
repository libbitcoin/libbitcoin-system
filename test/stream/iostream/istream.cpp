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

using iostream_chunk = iostream<data_chunk>;
using seekdir = typename iostream_chunk::seekdir;
using pos_type = typename iostream_chunk::pos_type;
using iostate = typename iostream_chunk::iostate;

// tellg

BOOST_AUTO_TEST_CASE(iostream__tellg__empty__zero_goodbit)
{
    data_chunk empty{};
    iostream_chunk stream{ empty };
    BOOST_REQUIRE(is_zero(stream.tellg()));
    BOOST_REQUIRE(stream.rdstate() == iostream_chunk::goodbit);
}

BOOST_AUTO_TEST_CASE(iostream__tellg__initial__zero_goodbit)
{
    auto chunk = base16_chunk("00010203040506070809");
    const iostream_chunk stream{ chunk };
    BOOST_REQUIRE(is_zero(stream.tellg()));
    BOOST_REQUIRE(stream.rdstate() == iostream_chunk::goodbit);
}

// iostream_chunk::beg

BOOST_AUTO_TEST_CASE(iostream__seekg__zero_from_begin__tellg_zero_goodbit)
{
    auto chunk = base16_chunk("00010203040506070809");
    iostream_chunk stream{ chunk };
    BOOST_REQUIRE_EQUAL(stream.seekg(3, iostream_chunk::beg).tellg(), 3);
    BOOST_REQUIRE(is_zero(stream.seekg(0, iostream_chunk::beg).tellg()));
    BOOST_REQUIRE(stream.rdstate() == iostream_chunk::goodbit);
}

BOOST_AUTO_TEST_CASE(iostream__seekg__three_from_begin__tellg_three_goodbit)
{
    auto chunk = base16_chunk("00010203040506070809");
    iostream_chunk stream{ chunk };
    BOOST_REQUIRE_EQUAL(stream.seekg(3, iostream_chunk::beg).tellg(), 3);
    BOOST_REQUIRE_EQUAL(stream.seekg(3, iostream_chunk::beg).tellg(), 3);
    BOOST_REQUIRE(stream.rdstate() == iostream_chunk::goodbit);
}

BOOST_AUTO_TEST_CASE(iostream__seekg__size_from_begin__tellg_size_goodbit)
{
    auto chunk = base16_chunk("00010203040506070809");
    iostream_chunk stream{ chunk };
    BOOST_REQUIRE_EQUAL(stream.seekg(3, iostream_chunk::beg).tellg(), 3);
    BOOST_REQUIRE_EQUAL(stream.seekg(chunk.size(), iostream_chunk::beg).tellg(), to_signed(chunk.size()));
    BOOST_REQUIRE(stream.rdstate() == iostream_chunk::goodbit);
}

BOOST_AUTO_TEST_CASE(iostream__seekg__overflow_from_begin__unchanged_badbit)
{
    auto chunk = base16_chunk("00010203040506070809");
    iostream_chunk stream{ chunk };
    BOOST_REQUIRE(is_zero(stream.seekg(add1(chunk.size()), iostream_chunk::beg).tellg()));
    BOOST_REQUIRE(stream.rdstate() == iostream_chunk::badbit);
}

// iostream_chunk::cur

BOOST_AUTO_TEST_CASE(iostream__seekg__zero_from_three_current__tellg_three_goodbit)
{
    auto chunk = base16_chunk("00010203040506070809");
    iostream_chunk stream{ chunk };
    BOOST_REQUIRE_EQUAL(stream.seekg(3, iostream_chunk::beg).tellg(), 3);
    BOOST_REQUIRE_EQUAL(stream.seekg(0, iostream_chunk::cur).tellg(), 3);
    BOOST_REQUIRE(stream.rdstate() == iostream_chunk::goodbit);
}

BOOST_AUTO_TEST_CASE(iostream__seekg__three_from_three_current__tellg_six_goodbit)
{
    auto chunk = base16_chunk("00010203040506070809");
    iostream_chunk stream{ chunk };
    BOOST_REQUIRE_EQUAL(stream.seekg(3, iostream_chunk::beg).tellg(), 3);
    BOOST_REQUIRE_EQUAL(stream.seekg(3, iostream_chunk::cur).tellg(), 6);
    BOOST_REQUIRE(stream.rdstate() == iostream_chunk::goodbit);
}

BOOST_AUTO_TEST_CASE(iostream__seekg__size_from_zero_current__tellg_size_goodbit)
{
    auto chunk = base16_chunk("00010203040506070809");
    iostream_chunk stream{ chunk };
    BOOST_REQUIRE_EQUAL(stream.seekg(chunk.size(), iostream_chunk::cur).tellg(), to_signed(chunk.size()));
    BOOST_REQUIRE(stream.rdstate() == iostream_chunk::goodbit);
}

BOOST_AUTO_TEST_CASE(iostream__seekg__overflow_from_three_current__unchanged_badbit)
{
    auto chunk = base16_chunk("00010203040506070809");
    iostream_chunk stream{ chunk };
    BOOST_REQUIRE_EQUAL(stream.seekg(3, iostream_chunk::beg).tellg(), 3);
    BOOST_REQUIRE(stream.seekg(chunk.size(), iostream_chunk::cur).tellg() == 3);
    BOOST_REQUIRE(stream.rdstate() == iostream_chunk::badbit);
}

// iostream_chunk::end

BOOST_AUTO_TEST_CASE(iostream__seekg__zero_from_end__tellg_size_goodbit)
{
    auto chunk = base16_chunk("00010203040506070809");
    iostream_chunk stream{ chunk };
    BOOST_REQUIRE_EQUAL(stream.seekg(3, iostream_chunk::beg).tellg(), 3);
    BOOST_REQUIRE_EQUAL(stream.seekg(0, iostream_chunk::end).tellg(), to_signed(chunk.size()));
    BOOST_REQUIRE(stream.rdstate() == iostream_chunk::goodbit);
}

BOOST_AUTO_TEST_CASE(iostream__seekg__negative_three_from_end__tellg_size_less_three_goodbit)
{
    auto chunk = base16_chunk("00010203040506070809");
    iostream_chunk stream{ chunk };
    BOOST_REQUIRE_EQUAL(stream.seekg(3, iostream_chunk::beg).tellg(), 3);
    BOOST_REQUIRE_EQUAL(stream.seekg(-3, iostream_chunk::end).tellg(), to_signed(chunk.size()) - 3);
    BOOST_REQUIRE(stream.rdstate() == iostream_chunk::goodbit);
}

BOOST_AUTO_TEST_CASE(iostream__seekg__negative_size_from_end__tellg_zero_goodbit)
{
    auto chunk = base16_chunk("00010203040506070809");
    iostream_chunk stream{ chunk };
    BOOST_REQUIRE(is_zero(stream.seekg(-to_signed(chunk.size()), iostream_chunk::end).tellg()));
    BOOST_REQUIRE(stream.rdstate() == iostream_chunk::goodbit);
}

BOOST_AUTO_TEST_CASE(iostream__seekg__underflow_from_end__unchanged_badbit)
{
    auto chunk = base16_chunk("00010203040506070809");
    iostream_chunk stream{ chunk };
    BOOST_REQUIRE(is_zero(stream.seekg(add1(chunk.size()), iostream_chunk::end).tellg()));
    BOOST_REQUIRE(stream.rdstate() == iostream_chunk::badbit);
}

// peek

BOOST_AUTO_TEST_CASE(iostream__peek__empty__eof_badbit)
{
    constexpr auto eof = std::char_traits<char>::eof();
    data_chunk empty{};
    iostream_chunk stream{ empty };
    BOOST_REQUIRE_EQUAL(stream.peek(), eof);
    BOOST_REQUIRE(stream.rdstate() == iostream_chunk::badbit);
}

BOOST_AUTO_TEST_CASE(iostream__peek__chunk__no_advance_expected_goodbit)
{
    auto chunk = base16_chunk("00010203040506070809");
    iostream_chunk stream{ chunk };
    BOOST_REQUIRE_EQUAL(stream.peek(), 0x00);
    BOOST_REQUIRE(is_zero(stream.tellg()));
    BOOST_REQUIRE_EQUAL(stream.peek(), 0x00);
    BOOST_REQUIRE(is_zero(stream.tellg()));
    BOOST_REQUIRE(stream.rdstate() == iostream_chunk::goodbit);
}

BOOST_AUTO_TEST_CASE(iostream__peek__end__eof_badbit)
{
    constexpr auto eof = std::char_traits<char>::eof();
    auto chunk = base16_chunk("00010203040506070809");
    iostream_chunk stream{ chunk };
    BOOST_REQUIRE_EQUAL(stream.seekg(0, iostream_chunk::end).tellg(), to_signed(chunk.size()));
    BOOST_REQUIRE_EQUAL(stream.peek(), eof);
    BOOST_REQUIRE(stream.rdstate() == iostream_chunk::badbit);
}

BOOST_AUTO_TEST_CASE(iostream__peek__chunk__advance_expected_goodbit)
{
    auto chunk = base16_chunk("00010203040506070809");
    iostream_chunk stream{ chunk };
    BOOST_REQUIRE_EQUAL(stream.peek(), 0x00);
    BOOST_REQUIRE_EQUAL(stream.seekg(1, iostream_chunk::cur).tellg(), 1);
    BOOST_REQUIRE_EQUAL(stream.peek(), 0x01);
    BOOST_REQUIRE_EQUAL(stream.seekg(1, iostream_chunk::cur).tellg(), 2);
    BOOST_REQUIRE_EQUAL(stream.peek(), 0x02);
    BOOST_REQUIRE_EQUAL(stream.seekg(1, iostream_chunk::cur).tellg(), 3);
    BOOST_REQUIRE_EQUAL(stream.peek(), 0x03);
    BOOST_REQUIRE_EQUAL(stream.seekg(1, iostream_chunk::cur).tellg(), 4);
    BOOST_REQUIRE_EQUAL(stream.peek(), 0x04);
    BOOST_REQUIRE_EQUAL(stream.seekg(1, iostream_chunk::cur).tellg(), 5);
    BOOST_REQUIRE_EQUAL(stream.peek(), 0x05);
    BOOST_REQUIRE_EQUAL(stream.seekg(1, iostream_chunk::cur).tellg(), 6);
    BOOST_REQUIRE_EQUAL(stream.peek(), 0x06);
    BOOST_REQUIRE_EQUAL(stream.seekg(1, iostream_chunk::cur).tellg(), 7);
    BOOST_REQUIRE_EQUAL(stream.peek(), 0x07);
    BOOST_REQUIRE_EQUAL(stream.seekg(1, iostream_chunk::cur).tellg(), 8);
    BOOST_REQUIRE_EQUAL(stream.peek(), 0x08);
    BOOST_REQUIRE_EQUAL(stream.seekg(1, iostream_chunk::cur).tellg(), 9);
    BOOST_REQUIRE_EQUAL(stream.peek(), 0x09);
    BOOST_REQUIRE(stream.rdstate() == iostream_chunk::goodbit);
}

// read

BOOST_AUTO_TEST_CASE(iostream__read__none_empty__goodbit)
{
    data_chunk buffer{};
    data_chunk empty{};
    iostream_chunk stream{ empty };
    stream.read(system::pointer_cast<char>(buffer.data()), buffer.size());
    BOOST_REQUIRE(stream.rdstate() == iostream_chunk::goodbit);
}

BOOST_AUTO_TEST_CASE(iostream__read__underflow_empty__badbit)
{
    auto buffer = base16_chunk("00000000000000000000");
    data_chunk empty{};
    iostream_chunk stream{ empty };
    stream.read(system::pointer_cast<char>(buffer.data()), buffer.size());
    BOOST_REQUIRE(stream.rdstate() == iostream_chunk::badbit);
}

BOOST_AUTO_TEST_CASE(iostream__read__underflow_nonempty__badbit)
{
    auto buffer = base16_chunk("00000000000000000000000000");
    auto chunk = base16_chunk("00010203040506070809");
    iostream_chunk stream{ chunk };
    stream.read(system::pointer_cast<char>(buffer.data()), add1(chunk.size()));
    BOOST_REQUIRE(stream.rdstate() == iostream_chunk::badbit);
}

BOOST_AUTO_TEST_CASE(iostream__read__full_buffer__goodbit)
{
    auto buffer = base16_chunk("00000000000000000000000000");
    auto chunk = base16_chunk("00010203040506070809");
    BOOST_REQUIRE_GE(buffer.size(), chunk.size());
    BOOST_REQUIRE_NE(buffer, chunk);

    iostream_chunk stream{ chunk };
    stream.read(system::pointer_cast<char>(buffer.data()), chunk.size());
    BOOST_REQUIRE(stream.rdstate() == iostream_chunk::goodbit);
    buffer.resize(chunk.size());
    BOOST_REQUIRE_EQUAL(buffer, chunk);
}

// reader

BOOST_AUTO_TEST_CASE(iostream__reader__read_8_bytes_big_endian__exected_goodbit)
{
    auto chunk = base16_chunk("010203040506070809");
    iostream_chunk stream{ chunk };
    byte_reader<iostream_chunk> reader{ stream };
    BOOST_REQUIRE_EQUAL(reader.read_8_bytes_big_endian(), 0x0102030405060708_u64);
    BOOST_REQUIRE(stream.rdstate() == iostream_chunk::goodbit);
}

BOOST_AUTO_TEST_SUITE_END()
