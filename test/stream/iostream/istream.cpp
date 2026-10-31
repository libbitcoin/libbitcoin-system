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
#include "../../test.hpp"

BC_AUTO_TEST_SUITE(istream_tests)

using seekdir = typename istream<>::seekdir;
using pos_type = typename istream<>::pos_type;
using iostate = typename istream<>::iostate;

// tellg

BC_AUTO_TEST_CASE(istream__tellg__empty__zero_goodbit)
{
    data_chunk empty{};
    istream<> stream{ empty };
    BC_REQUIRE(is_zero(stream.tellg()));
    BC_REQUIRE(stream.rdstate() == istream<>::goodbit);
}

BC_AUTO_TEST_CASE(istream__tellg__initial__zero_goodbit)
{
    auto chunk = base16_chunk("00010203040506070809");
    const istream<> stream{ chunk };
    BC_REQUIRE(is_zero(stream.tellg()));
    BC_REQUIRE(stream.rdstate() == istream<>::goodbit);
}

// istream<>::beg

BC_AUTO_TEST_CASE(istream__seekg__zero_from_begin__tellg_zero_goodbit)
{
    auto chunk = base16_chunk("00010203040506070809");
    istream<> stream{ chunk };
    BC_REQUIRE_EQUAL(stream.seekg(3, istream<>::beg).tellg(), 3);
    BC_REQUIRE(is_zero(stream.seekg(0, istream<>::beg).tellg()));
    BC_REQUIRE(stream.rdstate() == istream<>::goodbit);
}

BC_AUTO_TEST_CASE(istream__seekg__three_from_begin__tellg_three_goodbit)
{
    auto chunk = base16_chunk("00010203040506070809");
    istream<> stream{ chunk };
    BC_REQUIRE_EQUAL(stream.seekg(3, istream<>::beg).tellg(), 3);
    BC_REQUIRE_EQUAL(stream.seekg(3, istream<>::beg).tellg(), 3);
    BC_REQUIRE(stream.rdstate() == istream<>::goodbit);
}

BC_AUTO_TEST_CASE(istream__seekg__size_from_begin__tellg_size_goodbit)
{
    auto chunk = base16_chunk("00010203040506070809");
    istream<> stream{ chunk };
    BC_REQUIRE_EQUAL(stream.seekg(3, istream<>::beg).tellg(), 3);
    BC_REQUIRE_EQUAL(stream.seekg(chunk.size(), istream<>::beg).tellg(), to_signed(chunk.size()));
    BC_REQUIRE(stream.rdstate() == istream<>::goodbit);
}

BC_AUTO_TEST_CASE(istream__seekg__overflow_from_begin__unchanged_badbit)
{
    auto chunk = base16_chunk("00010203040506070809");
    istream<> stream{ chunk };
    BC_REQUIRE(is_zero(stream.seekg(add1(chunk.size()), istream<>::beg).tellg()));
    BC_REQUIRE(stream.rdstate() == istream<>::badbit);
}

// istream<>::cur

BC_AUTO_TEST_CASE(istream__seekg__zero_from_three_current__tellg_three_goodbit)
{
    auto chunk = base16_chunk("00010203040506070809");
    istream<> stream{ chunk };
    BC_REQUIRE_EQUAL(stream.seekg(3, istream<>::beg).tellg(), 3);
    BC_REQUIRE_EQUAL(stream.seekg(0, istream<>::cur).tellg(), 3);
    BC_REQUIRE(stream.rdstate() == istream<>::goodbit);
}

BC_AUTO_TEST_CASE(istream__seekg__three_from_three_current__tellg_six_goodbit)
{
    auto chunk = base16_chunk("00010203040506070809");
    istream<> stream{ chunk };
    BC_REQUIRE_EQUAL(stream.seekg(3, istream<>::beg).tellg(), 3);
    BC_REQUIRE_EQUAL(stream.seekg(3, istream<>::cur).tellg(), 6);
    BC_REQUIRE(stream.rdstate() == istream<>::goodbit);
}

BC_AUTO_TEST_CASE(istream__seekg__size_from_zero_current__tellg_size_goodbit)
{
    auto chunk = base16_chunk("00010203040506070809");
    istream<> stream{ chunk };
    BC_REQUIRE_EQUAL(stream.seekg(chunk.size(), istream<>::cur).tellg(), to_signed(chunk.size()));
    BC_REQUIRE(stream.rdstate() == istream<>::goodbit);
}

BC_AUTO_TEST_CASE(istream__seekg__overflow_from_three_current__unchanged_badbit)
{
    auto chunk = base16_chunk("00010203040506070809");
    istream<> stream{ chunk };
    BC_REQUIRE_EQUAL(stream.seekg(3, istream<>::beg).tellg(), 3);
    BC_REQUIRE(stream.seekg(chunk.size(), istream<>::cur).tellg() == 3);
    BC_REQUIRE(stream.rdstate() == istream<>::badbit);
}

// istream<>::end

BC_AUTO_TEST_CASE(istream__seekg__zero_from_end__tellg_size_goodbit)
{
    auto chunk = base16_chunk("00010203040506070809");
    istream<> stream{ chunk };
    BC_REQUIRE_EQUAL(stream.seekg(3, istream<>::beg).tellg(), 3);
    BC_REQUIRE_EQUAL(stream.seekg(0, istream<>::end).tellg(), to_signed(chunk.size()));
    BC_REQUIRE(stream.rdstate() == istream<>::goodbit);
}

BC_AUTO_TEST_CASE(istream__seekg__negative_three_from_end__tellg_size_less_three_goodbit)
{
    auto chunk = base16_chunk("00010203040506070809");
    istream<> stream{ chunk };
    BC_REQUIRE_EQUAL(stream.seekg(3, istream<>::beg).tellg(), 3);
    BC_REQUIRE_EQUAL(stream.seekg(-3, istream<>::end).tellg(), to_signed(chunk.size()) - 3);
    BC_REQUIRE(stream.rdstate() == istream<>::goodbit);
}

BC_AUTO_TEST_CASE(istream__seekg__negative_size_from_end__tellg_zero_goodbit)
{
    auto chunk = base16_chunk("00010203040506070809");
    istream<> stream{ chunk };
    BC_REQUIRE(is_zero(stream.seekg(-to_signed(chunk.size()), istream<>::end).tellg()));
    BC_REQUIRE(stream.rdstate() == istream<>::goodbit);
}

BC_AUTO_TEST_CASE(istream__seekg__underflow_from_end__unchanged_badbit)
{
    auto chunk = base16_chunk("00010203040506070809");
    istream<> stream{ chunk };
    BC_REQUIRE(is_zero(stream.seekg(add1(chunk.size()), istream<>::end).tellg()));
    BC_REQUIRE(stream.rdstate() == istream<>::badbit);
}

// peek

BC_AUTO_TEST_CASE(istream__peek__empty__eof_badbit)
{
    constexpr auto eof = std::char_traits<char>::eof();
    data_chunk empty{};
    istream<> stream{ empty };
    BC_REQUIRE_EQUAL(stream.peek(), eof);
    BC_REQUIRE(stream.rdstate() == istream<>::badbit);
}

BC_AUTO_TEST_CASE(istream__peek__chunk__no_advance_expected_goodbit)
{
    auto chunk = base16_chunk("00010203040506070809");
    istream<> stream{ chunk };
    BC_REQUIRE_EQUAL(stream.peek(), 0x00);
    BC_REQUIRE(is_zero(stream.tellg()));
    BC_REQUIRE_EQUAL(stream.peek(), 0x00);
    BC_REQUIRE(is_zero(stream.tellg()));
    BC_REQUIRE(stream.rdstate() == istream<>::goodbit);
}

BC_AUTO_TEST_CASE(istream__peek__end__eof_badbit)
{
    constexpr auto eof = std::char_traits<char>::eof();
    auto chunk = base16_chunk("00010203040506070809");
    istream<> stream{ chunk };
    BC_REQUIRE_EQUAL(stream.seekg(0, istream<>::end).tellg(), to_signed(chunk.size()));
    BC_REQUIRE_EQUAL(stream.peek(), eof);
    BC_REQUIRE(stream.rdstate() == istream<>::badbit);
}

BC_AUTO_TEST_CASE(istream__peek__chunk__advance_expected_goodbit)
{
    auto chunk = base16_chunk("00010203040506070809");
    istream<> stream{ chunk };
    BC_REQUIRE_EQUAL(stream.peek(), 0x00);
    BC_REQUIRE_EQUAL(stream.seekg(1, istream<>::cur).tellg(), 1);
    BC_REQUIRE_EQUAL(stream.peek(), 0x01);
    BC_REQUIRE_EQUAL(stream.seekg(1, istream<>::cur).tellg(), 2);
    BC_REQUIRE_EQUAL(stream.peek(), 0x02);
    BC_REQUIRE_EQUAL(stream.seekg(1, istream<>::cur).tellg(), 3);
    BC_REQUIRE_EQUAL(stream.peek(), 0x03);
    BC_REQUIRE_EQUAL(stream.seekg(1, istream<>::cur).tellg(), 4);
    BC_REQUIRE_EQUAL(stream.peek(), 0x04);
    BC_REQUIRE_EQUAL(stream.seekg(1, istream<>::cur).tellg(), 5);
    BC_REQUIRE_EQUAL(stream.peek(), 0x05);
    BC_REQUIRE_EQUAL(stream.seekg(1, istream<>::cur).tellg(), 6);
    BC_REQUIRE_EQUAL(stream.peek(), 0x06);
    BC_REQUIRE_EQUAL(stream.seekg(1, istream<>::cur).tellg(), 7);
    BC_REQUIRE_EQUAL(stream.peek(), 0x07);
    BC_REQUIRE_EQUAL(stream.seekg(1, istream<>::cur).tellg(), 8);
    BC_REQUIRE_EQUAL(stream.peek(), 0x08);
    BC_REQUIRE_EQUAL(stream.seekg(1, istream<>::cur).tellg(), 9);
    BC_REQUIRE_EQUAL(stream.peek(), 0x09);
    BC_REQUIRE(stream.rdstate() == istream<>::goodbit);
}

// read

BC_AUTO_TEST_CASE(istream__read__none_empty__goodbit)
{
    data_chunk buffer{};
    data_chunk empty{};
    istream<> stream{ empty };
    stream.read(system::pointer_cast<char>(buffer.data()), buffer.size());
    BC_REQUIRE(stream.rdstate() == istream<>::goodbit);
}

BC_AUTO_TEST_CASE(istream__read__underflow_empty__badbit)
{
    auto buffer = base16_chunk("00000000000000000000");
    data_chunk empty{};
    istream<> stream{ empty };
    stream.read(system::pointer_cast<char>(buffer.data()), buffer.size());
    BC_REQUIRE(stream.rdstate() == istream<>::badbit);
}

BC_AUTO_TEST_CASE(istream__read__underflow_nonempty__badbit)
{
    auto buffer = base16_chunk("00000000000000000000000000");
    auto chunk = base16_chunk("00010203040506070809");
    istream<> stream{ chunk };
    stream.read(system::pointer_cast<char>(buffer.data()), add1(chunk.size()));
    BC_REQUIRE(stream.rdstate() == istream<>::badbit);
}

BC_AUTO_TEST_CASE(istream__read__full_buffer__goodbit)
{
    auto buffer = base16_chunk("00000000000000000000000000");
    auto chunk = base16_chunk("00010203040506070809");
    BC_REQUIRE_GE(buffer.size(), chunk.size());
    BC_REQUIRE_NE(buffer, chunk);

    istream<> stream{ chunk };
    stream.read(system::pointer_cast<char>(buffer.data()), chunk.size());
    BC_REQUIRE(stream.rdstate() == istream<>::goodbit);
    buffer.resize(chunk.size());
    BC_REQUIRE_EQUAL(buffer, chunk);
}

// reader

BC_AUTO_TEST_CASE(istream__reader__read_8_bytes_big_endian__expected_goodbit)
{
    auto chunk = base16_chunk("010203040506070809");
    istream<> stream{ chunk };
    byte_reader<istream<>> reader{ stream };
    BC_REQUIRE_EQUAL(reader.read_8_bytes_big_endian(), 0x0102030405060708_u64);
    BC_REQUIRE(stream.rdstate() == istream<>::goodbit);
}

BC_AUTO_TEST_SUITE_END()
