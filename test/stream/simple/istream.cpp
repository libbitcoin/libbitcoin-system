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

BOOST_AUTO_TEST_SUITE(istream_tests)

using istream_chunk = istream<data_chunk>;
using seekdir = typename istream_chunk::seekdir;
using pos_type = typename istream_chunk::pos_type;
using iostate = typename istream_chunk::iostate;
const auto chunk = base16_chunk("00010203040506070809");

// setstate/clear

BOOST_AUTO_TEST_CASE(istream__setstate__goodbit__goodbit)
{
    istream_chunk stream{ {} };
    stream.setstate(iostate::goodbit);
    BOOST_REQUIRE(stream.rdstate() == iostate::goodbit);
}

BOOST_AUTO_TEST_CASE(istream__setstate__eofbit__eofbit)
{
    istream_chunk stream{ chunk };
    stream.setstate(iostate::eofbit);
    BOOST_REQUIRE(stream.rdstate() == iostate::eofbit);

    stream.clear();
    BOOST_REQUIRE(stream.rdstate() == iostate::goodbit);
}

BOOST_AUTO_TEST_CASE(istream__setstate__failbit__failbit)
{
    istream_chunk stream{ {} };
    stream.setstate(iostate::failbit);
    BOOST_REQUIRE(stream.rdstate() == iostate::failbit);

    stream.clear();
    BOOST_REQUIRE(stream.rdstate() == iostate::goodbit);
}

BOOST_AUTO_TEST_CASE(istream__setstate__badbit__badbit)
{
    istream_chunk stream{ {} };
    stream.setstate(iostate::badbit);
    BOOST_REQUIRE(stream.rdstate() == iostate::badbit);

    stream.clear();
    BOOST_REQUIRE(stream.rdstate() == iostate::goodbit);
}

BOOST_AUTO_TEST_CASE(istream__setstate__badbit_failbit__badbit_failbit)
{
    constexpr auto badfail = static_cast<iostate>(
        static_cast<std::underlying_type_t<iostate>>(iostate::failbit) |
        static_cast<std::underlying_type_t<iostate>>(iostate::badbit));

    istream_chunk stream{ chunk };
    stream.setstate(iostate::badbit);
    stream.setstate(iostate::failbit);
    BOOST_REQUIRE(stream.rdstate() == badfail);

    stream.clear(iostate::badbit);
    BOOST_REQUIRE(stream.rdstate() == iostate::badbit);

    stream.clear(iostate::failbit);
    BOOST_REQUIRE(stream.rdstate() == iostate::failbit);

    stream.clear(iostate::goodbit);
    BOOST_REQUIRE(stream.rdstate() == iostate::goodbit);
}

// tellg

BOOST_AUTO_TEST_CASE(istream__tellg__empty__zero_goodbit)
{
    const istream_chunk stream{ {} };
    BOOST_REQUIRE(is_zero(stream.tellg()));
    BOOST_REQUIRE(stream.rdstate() == iostate::goodbit);
}

BOOST_AUTO_TEST_CASE(istream__tellg__initial__zero_goodbit)
{
    const istream_chunk stream{ chunk };
    BOOST_REQUIRE(is_zero(stream.tellg()));
    BOOST_REQUIRE(stream.rdstate() == iostate::goodbit);
}

// seekdir::beg

BOOST_AUTO_TEST_CASE(istream__seekg__zero_from_begin__tellg_zero_goodbit)
{
    istream_chunk stream{ chunk };
    BOOST_REQUIRE_EQUAL(stream.seekg(3, seekdir::beg).tellg(), 3);
    BOOST_REQUIRE(is_zero(stream.seekg(0, seekdir::beg).tellg()));
    BOOST_REQUIRE(stream.rdstate() == iostate::goodbit);
}

BOOST_AUTO_TEST_CASE(istream__seekg__three_from_begin__tellg_three_goodbit)
{
    istream_chunk stream{ chunk };
    BOOST_REQUIRE_EQUAL(stream.seekg(3, seekdir::beg).tellg(), 3);
    BOOST_REQUIRE_EQUAL(stream.seekg(3, seekdir::beg).tellg(), 3);
    BOOST_REQUIRE(stream.rdstate() == iostate::goodbit);
}

BOOST_AUTO_TEST_CASE(istream__seekg__size_from_begin__tellg_size_goodbit)
{
    istream_chunk stream{ chunk };
    BOOST_REQUIRE_EQUAL(stream.seekg(3, seekdir::beg).tellg(), 3);
    BOOST_REQUIRE_EQUAL(stream.seekg(chunk.size(), seekdir::beg).tellg(), to_signed(chunk.size()));
    BOOST_REQUIRE(stream.rdstate() == iostate::goodbit);
}

BOOST_AUTO_TEST_CASE(istream__seekg__overflow_from_begin__unchanged_badbit)
{
    istream_chunk stream{ chunk };
    BOOST_REQUIRE(is_zero(stream.seekg(add1(chunk.size()), seekdir::beg).tellg()));
    BOOST_REQUIRE(stream.rdstate() == iostate::badbit);
}

// seekdir::cur

BOOST_AUTO_TEST_CASE(istream__seekg__zero_from_three_current__tellg_three_goodbit)
{
    istream_chunk stream{ chunk };
    BOOST_REQUIRE_EQUAL(stream.seekg(3, seekdir::beg).tellg(), 3);
    BOOST_REQUIRE_EQUAL(stream.seekg(0, seekdir::cur).tellg(), 3);
    BOOST_REQUIRE(stream.rdstate() == iostate::goodbit);
}

BOOST_AUTO_TEST_CASE(istream__seekg__three_from_three_current__tellg_six_goodbit)
{
    istream_chunk stream{ chunk };
    BOOST_REQUIRE_EQUAL(stream.seekg(3, seekdir::beg).tellg(), 3);
    BOOST_REQUIRE_EQUAL(stream.seekg(3, seekdir::cur).tellg(), 6);
    BOOST_REQUIRE(stream.rdstate() == iostate::goodbit);
}

BOOST_AUTO_TEST_CASE(istream__seekg__size_from_zero_current__tellg_size_goodbit)
{
    istream_chunk stream{ chunk };
    BOOST_REQUIRE_EQUAL(stream.seekg(chunk.size(), seekdir::cur).tellg(), to_signed(chunk.size()));
    BOOST_REQUIRE(stream.rdstate() == iostate::goodbit);
}

BOOST_AUTO_TEST_CASE(istream__seekg__overflow_from_three_current__unchanged_badbit)
{
    istream_chunk stream{ chunk };
    BOOST_REQUIRE_EQUAL(stream.seekg(3, seekdir::beg).tellg(), 3);
    BOOST_REQUIRE(stream.seekg(chunk.size(), seekdir::cur).tellg() == 3);
    BOOST_REQUIRE(stream.rdstate() == iostate::badbit);
}

// seekdir::end

BOOST_AUTO_TEST_CASE(istream__seekg__zero_from_end__tellg_size_goodbit)
{
    istream_chunk stream{ chunk };
    BOOST_REQUIRE_EQUAL(stream.seekg(3, seekdir::beg).tellg(), 3);
    BOOST_REQUIRE_EQUAL(stream.seekg(0, seekdir::end).tellg(), to_signed(chunk.size()));
    BOOST_REQUIRE(stream.rdstate() == iostate::goodbit);
}

BOOST_AUTO_TEST_CASE(istream__seekg__negative_three_from_end__tellg_size_less_three_goodbit)
{
    istream_chunk stream{ chunk };
    BOOST_REQUIRE_EQUAL(stream.seekg(3, seekdir::beg).tellg(), 3);
    BOOST_REQUIRE_EQUAL(stream.seekg(-3, seekdir::end).tellg(), to_signed(chunk.size()) - 3);
    BOOST_REQUIRE(stream.rdstate() == iostate::goodbit);
}

BOOST_AUTO_TEST_CASE(istream__seekg__negative_size_from_end__tellg_zero_goodbit)
{
    istream_chunk stream{ chunk };
    BOOST_REQUIRE(is_zero(stream.seekg(-to_signed(chunk.size()), seekdir::end).tellg()));
    BOOST_REQUIRE(stream.rdstate() == iostate::goodbit);
}

BOOST_AUTO_TEST_CASE(istream__seekg__underflow_from_end__unchanged_badbit)
{
    istream_chunk stream{ chunk };
    BOOST_REQUIRE(is_zero(stream.seekg(add1(chunk.size()), seekdir::end).tellg()));
    BOOST_REQUIRE(stream.rdstate() == iostate::badbit);
}

// peek

BOOST_AUTO_TEST_CASE(istream__peek__empty__eof_badbit)
{
    constexpr auto eof = std::char_traits<char>::eof();

    istream_chunk stream{ {} };
    BOOST_REQUIRE_EQUAL(stream.peek(), eof);
    BOOST_REQUIRE(stream.rdstate() == iostate::badbit);
}

BOOST_AUTO_TEST_CASE(istream__peek__chunk__no_advance_expected_goodbit)
{
    istream_chunk stream{ chunk };
    BOOST_REQUIRE_EQUAL(stream.peek(), 0x00);
    BOOST_REQUIRE(is_zero(stream.tellg()));
    BOOST_REQUIRE_EQUAL(stream.peek(), 0x00);
    BOOST_REQUIRE(is_zero(stream.tellg()));
    BOOST_REQUIRE(stream.rdstate() == iostate::goodbit);
}

BOOST_AUTO_TEST_CASE(istream__peek__end__eof_badbit)
{
    constexpr auto eof = std::char_traits<char>::eof();

    istream_chunk stream{ chunk };
    BOOST_REQUIRE_EQUAL(stream.seekg(0, seekdir::end).tellg(), to_signed(chunk.size()));
    BOOST_REQUIRE_EQUAL(stream.peek(), eof);
    BOOST_REQUIRE(stream.rdstate() == iostate::badbit);
}

BOOST_AUTO_TEST_CASE(istream__peek__chunk__advance_expected_goodbit)
{
    istream_chunk stream{ chunk };
    BOOST_REQUIRE_EQUAL(stream.peek(), 0x00);
    BOOST_REQUIRE_EQUAL(stream.seekg(1, seekdir::cur).tellg(), 1);
    BOOST_REQUIRE_EQUAL(stream.peek(), 0x01);
    BOOST_REQUIRE_EQUAL(stream.seekg(1, seekdir::cur).tellg(), 2);
    BOOST_REQUIRE_EQUAL(stream.peek(), 0x02);
    BOOST_REQUIRE_EQUAL(stream.seekg(1, seekdir::cur).tellg(), 3);
    BOOST_REQUIRE_EQUAL(stream.peek(), 0x03);
    BOOST_REQUIRE_EQUAL(stream.seekg(1, seekdir::cur).tellg(), 4);
    BOOST_REQUIRE_EQUAL(stream.peek(), 0x04);
    BOOST_REQUIRE_EQUAL(stream.seekg(1, seekdir::cur).tellg(), 5);
    BOOST_REQUIRE_EQUAL(stream.peek(), 0x05);
    BOOST_REQUIRE_EQUAL(stream.seekg(1, seekdir::cur).tellg(), 6);
    BOOST_REQUIRE_EQUAL(stream.peek(), 0x06);
    BOOST_REQUIRE_EQUAL(stream.seekg(1, seekdir::cur).tellg(), 7);
    BOOST_REQUIRE_EQUAL(stream.peek(), 0x07);
    BOOST_REQUIRE_EQUAL(stream.seekg(1, seekdir::cur).tellg(), 8);
    BOOST_REQUIRE_EQUAL(stream.peek(), 0x08);
    BOOST_REQUIRE_EQUAL(stream.seekg(1, seekdir::cur).tellg(), 9);
    BOOST_REQUIRE_EQUAL(stream.peek(), 0x09);
    BOOST_REQUIRE(stream.rdstate() == iostate::goodbit);
}

// read

BOOST_AUTO_TEST_CASE(istream__read__none_empty__goodbit)
{
    auto buffer = base16_chunk("");

    istream_chunk stream{ {} };
    stream.read(system::pointer_cast<char>(buffer.data()), buffer.size());
    BOOST_REQUIRE(stream.rdstate() == iostate::goodbit);
}

BOOST_AUTO_TEST_CASE(istream__read__underflow_empty__badbit)
{
    auto buffer = base16_chunk("00000000000000000000");

    istream_chunk stream{ {} };
    stream.read(system::pointer_cast<char>(buffer.data()), buffer.size());
    BOOST_REQUIRE(stream.rdstate() == iostate::badbit);
}

BOOST_AUTO_TEST_CASE(istream__read__underflow_nonempty__badbit)
{
    auto buffer = base16_chunk("00000000000000000000000000");

    istream_chunk stream{ chunk };
    stream.read(system::pointer_cast<char>(buffer.data()), add1(chunk.size()));
    BOOST_REQUIRE(stream.rdstate() == iostate::badbit);
}

BOOST_AUTO_TEST_CASE(istream__read__full_buffer__goodbit)
{
    auto buffer = base16_chunk("00000000000000000000000000");
    BOOST_REQUIRE_GE(buffer.size(), chunk.size());
    BOOST_REQUIRE_NE(buffer, chunk);

    istream_chunk stream{ chunk };
    stream.read(system::pointer_cast<char>(buffer.data()), chunk.size());
    BOOST_REQUIRE(stream.rdstate() == iostate::goodbit);
    buffer.resize(chunk.size());
    BOOST_REQUIRE_EQUAL(buffer, chunk);
}

BOOST_AUTO_TEST_SUITE_END()
