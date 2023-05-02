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

BOOST_AUTO_TEST_SUITE(ostream_tests)

using ostream_chunk = ostream<data_chunk>;
using pos_type = typename ostream_chunk::pos_type;
using iostate = typename ostream_chunk::iostate;
const auto chunk = base16_chunk("00010203040506070809");

// setstate/clear

BOOST_AUTO_TEST_CASE(ostream__setstate__goodbit__goodbit)
{
    auto chunk = base16_chunk("");
    ostream_chunk stream{ chunk };
    stream.setstate(iostate::goodbit);
    BOOST_REQUIRE(stream.rdstate() == iostate::goodbit);
}

BOOST_AUTO_TEST_CASE(ostream__setstate__eofbit__eofbit)
{
    auto chunk = base16_chunk("00000000000000000000000000");
    ostream_chunk stream{ chunk };
    stream.setstate(iostate::eofbit);
    BOOST_REQUIRE(stream.rdstate() == iostate::eofbit);

    stream.clear();
    BOOST_REQUIRE(stream.rdstate() == iostate::goodbit);
}

BOOST_AUTO_TEST_CASE(ostream__setstate__failbit__failbit)
{
    auto chunk = base16_chunk("");
    ostream_chunk stream{ chunk };
    stream.setstate(iostate::failbit);
    BOOST_REQUIRE(stream.rdstate() == iostate::failbit);

    stream.clear();
    BOOST_REQUIRE(stream.rdstate() == iostate::goodbit);
}

BOOST_AUTO_TEST_CASE(ostream__setstate__badbit__badbit)
{
    auto chunk = base16_chunk("");
    ostream_chunk stream{ chunk };
    stream.setstate(iostate::badbit);
    BOOST_REQUIRE(stream.rdstate() == iostate::badbit);

    stream.clear();
    BOOST_REQUIRE(stream.rdstate() == iostate::goodbit);
}

BOOST_AUTO_TEST_CASE(ostream__setstate__badbit_failbit__badbit_failbit)
{
    constexpr auto badfail = static_cast<iostate>(
        static_cast<std::underlying_type_t<iostate>>(iostate::failbit) |
        static_cast<std::underlying_type_t<iostate>>(iostate::badbit));

    auto chunk = base16_chunk("00000000000000000000000000");
    ostream_chunk stream{ chunk };
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

// tellp

BOOST_AUTO_TEST_CASE(ostream__tellp__empty__zero_goodbit)
{
    auto chunk = base16_chunk("");
    ostream_chunk stream{ chunk };
    BOOST_REQUIRE(is_zero(stream.tellp()));
    BOOST_REQUIRE(stream.rdstate() == iostate::goodbit);
}

BOOST_AUTO_TEST_CASE(ostream__tellp__initial__zero_goodbit)
{
    auto chunk = base16_chunk("");
    ostream_chunk stream{ chunk };
    BOOST_REQUIRE(is_zero(stream.tellp()));
    BOOST_REQUIRE(stream.rdstate() == iostate::goodbit);
}

// write

BOOST_AUTO_TEST_CASE(ostream__write__none_empty__goodbit)
{
    const auto buffer = base16_chunk("");
    auto chunk = base16_chunk("");
    ostream_chunk stream{ chunk };
    stream.write(system::pointer_cast<const char>(buffer.data()), buffer.size());
    BOOST_REQUIRE(stream.rdstate() == iostate::goodbit);
}

BOOST_AUTO_TEST_CASE(ostream__write__overflow_empty__badbit)
{
    const auto buffer = base16_chunk("00010203040506070809");
    auto chunk = base16_chunk("");
    ostream_chunk stream{ chunk };
    stream.write(system::pointer_cast<const char>(buffer.data()), buffer.size());
    BOOST_REQUIRE(stream.rdstate() == iostate::badbit);
}

BOOST_AUTO_TEST_CASE(ostream__write__overflow_nonempty__badbit)
{
    const auto buffer = base16_chunk("00010203040506070809");
    auto chunk = base16_chunk("000000000000");
    ostream_chunk stream{ chunk };
    stream.write(system::pointer_cast<const char>(buffer.data()), buffer.size());
    BOOST_REQUIRE(stream.rdstate() == iostate::badbit);
}

BOOST_AUTO_TEST_CASE(ostream__write__full_buffer__goodbit)
{
    const auto buffer = base16_chunk("00010203040506070809");
    auto chunk = base16_chunk("00000000000000000000000000");
    BOOST_REQUIRE_GE(chunk.size(), buffer.size());
    BOOST_REQUIRE_NE(buffer, chunk);

    ostream_chunk stream{ chunk };
    stream.write(system::pointer_cast<const char>(buffer.data()), buffer.size());
    BOOST_REQUIRE(stream.rdstate() == iostate::goodbit);
    chunk.resize(buffer.size());
    BOOST_REQUIRE_EQUAL(buffer, chunk);
}

BOOST_AUTO_TEST_SUITE_END()
