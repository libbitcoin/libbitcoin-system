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
using pos_type = typename iostream_chunk::pos_type;
using iostate = typename iostream_chunk::iostate;
const auto chunk = base16_chunk("00010203040506070809");

// tellp

BOOST_AUTO_TEST_CASE(iostream__tellp__empty__zero_goodbit)
{
    data_chunk empty{};
    iostream_chunk stream{ empty };
    BOOST_REQUIRE(is_zero(stream.tellp()));
    BOOST_REQUIRE(stream.rdstate() == iostream_chunk::goodbit);
}

BOOST_AUTO_TEST_CASE(iostream__tellp__initial__zero_goodbit)
{
    data_chunk empty{};
    iostream_chunk stream{ empty };
    BOOST_REQUIRE(is_zero(stream.tellp()));
    BOOST_REQUIRE(stream.rdstate() == iostream_chunk::goodbit);
}

// write

BOOST_AUTO_TEST_CASE(iostream__write__none_empty__goodbit)
{
    const data_chunk buffer{};
    data_chunk empty{};
    iostream_chunk stream{ empty };
    stream.write(system::pointer_cast<const char>(buffer.data()), buffer.size());
    BOOST_REQUIRE(stream.rdstate() == iostream_chunk::goodbit);
}

BOOST_AUTO_TEST_CASE(iostream__write__overflow_empty__badbit)
{
    const auto buffer = base16_chunk("00010203040506070809");
    data_chunk empty{};
    iostream_chunk stream{ empty };
    stream.write(system::pointer_cast<const char>(buffer.data()), buffer.size());
    BOOST_REQUIRE(stream.rdstate() == iostream_chunk::badbit);
}

BOOST_AUTO_TEST_CASE(iostream__write__overflow_nonempty__badbit)
{
    const auto buffer = base16_chunk("00010203040506070809");
    auto chunk = base16_chunk("000000000000");
    iostream_chunk stream{ chunk };
    stream.write(system::pointer_cast<const char>(buffer.data()), buffer.size());
    BOOST_REQUIRE(stream.rdstate() == iostream_chunk::badbit);
}

BOOST_AUTO_TEST_CASE(iostream__write__full_buffer__goodbit)
{
    const auto buffer = base16_chunk("00010203040506070809");
    auto chunk = base16_chunk("00000000000000000000000000");
    BOOST_REQUIRE_GE(chunk.size(), buffer.size());
    BOOST_REQUIRE_NE(buffer, chunk);

    iostream_chunk stream{ chunk };
    stream.write(system::pointer_cast<const char>(buffer.data()), buffer.size());
    BOOST_REQUIRE(stream.rdstate() == iostream_chunk::goodbit);
    chunk.resize(buffer.size());
    BOOST_REQUIRE_EQUAL(buffer, chunk);
}

// writer

BOOST_AUTO_TEST_CASE(iostream__writer__write_8_bytes_big_endian__exected_goodbit)
{
    const auto expected = base16_chunk("010203040506070800");
    auto chunk = base16_chunk("000000000000000000");
    iostream_chunk stream{ chunk };
    byte_writer<iostream_chunk> writer{ stream };
    writer.write_8_bytes_big_endian(0x0102030405060708_u64);
    BOOST_REQUIRE(stream.rdstate() == iostream_chunk::goodbit);
    BOOST_REQUIRE_EQUAL(chunk, expected);
}

BOOST_AUTO_TEST_SUITE_END()
