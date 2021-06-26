/**
 * Copyright (c) 2011-2021 libbitcoin developers (see AUTHORS)
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
#include <sstream>

BOOST_AUTO_TEST_SUITE(stream_tests)

// TODO: test all methods against std::stringstream
// TODO: test against copy_sink.

#define BIT_FLIPPER_IS_EXHAUSTED

#ifdef BIT_FLIPPER_IS_EXHAUSTED

BOOST_AUTO_TEST_CASE(bit_flipper1)
{
    ////const data_chunk hard;
    ////data_chunk soft;
    ////data_slab slab(soft);
    ////const data_array<1> bytes2{ { 0x00 } };
    ////data_array<1> bytes;
    ////const std::string text;
    ////std::string text2;
    ////std::istringstream in;

    ////read::bytes::move move7(text2);
    ////read::bytes::move move5(bytes);
    ////read::bytes::move move6(bytes);
    ////read::bytes::move move3(slab);
    ////read::bytes::move move4(soft);

    ////read::bytes::copy text1(text);
    ////read::bytes::copy text8(bytes2);
    ////read::bytes::copy text9("text");
    ////read::bytes::copy copy1(hard);
    ////read::bytes::move move1(soft);
    ////read::bytes::istream istream1(in);

    ////read::bits::copy text65("text");
    ////read::bits::copy copy2(hard);
    ////read::bits::move move2(soft);
    ////read::bits::istream istream2(in);

    ////copy_source<data_slice> text3("text");
    ////copy_source<data_slice> copy3(hard);
    ////copy_source<data_slice> copy4(hard);
    ////move_source<data_slab> move9(soft);
    ////move_source<data_slab> move8(text2);

    ////bit_reader<std::istream> istream3(in);
    ////byte_reader<std::istream> istream4(in);
}

BOOST_AUTO_TEST_CASE(bit_flipper2)
{
    ////data_chunk data;
    ////data_array<1> bytes;
    ////std::string text;
    ////std::ostringstream out;
    ////std::stringstream inout;

    ////write::bytes::slab copy1(bytes);
    ////write::bytes::slab copy5(text);
    ////write::bytes::slab copy6(data);

    ////write::bytes::push push1(data);
    ////write::bytes::text text1(text);
    ////write::bytes::ostream ostream1(out);
    ////write::bytes::iostream iostream1(inout);

    ////write::bits::slab copy2(data);
    ////write::bits::push push2(data);
    ////write::bits::text text2(text);
    ////write::bits::ostream ostream2(out);
    ////write::bits::iostream iostream2(inout);

    ////// Data copied to buffer.
    ////copy_sink<data_slab> copy4(data);
    ////copy_sink<data_slab> copy55(text);

    ////copy_sink<data_slab> copy3(data);
    ////push_sink<data_chunk> push3(data);
    ////push_sink<std::string> text3(text);

    ////bit_writer<std::ostream> ostream3(out);
    ////byte_writer<std::ostream> ostream4(out);

    ////bit_flipper<std::iostream> iostream3(inout);
    ////byte_flipper<std::iostream> iostream4(inout);
}

#endif // BIT_FLIPPER_IS_EXHAUSTED

BOOST_AUTO_TEST_SUITE_END()
