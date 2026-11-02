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

BC_AUTO_TEST_SUITE(base_64_tests)

#define BASE64_MURRAY "TXVycmF5IFJvdGhiYXJk"
#define BASE64_DATA_MURRAY \
{ \
    0x4d, 0x75, 0x72, 0x72, 0x61, 0x79, 0x20, \
    0x52, 0x6f, 0x74, 0x68, 0x62, 0x61, 0x72, 0x64 \
}

#define BASE64_BOOK "TWFuLCBFY29ub215IGFuZCBTdGF0ZQ=="
#define BASE64_DATA_BOOK \
{ \
    0x4d, 0x61, 0x6e, 0x2c, 0x20, 0x45, 0x63, \
    0x6f, 0x6e, 0x6f, 0x6d, 0x79, 0x20, 0x61, \
    0x6e, 0x64, 0x20, 0x53, 0x74, 0x61, 0x74, 0x65 \
}

BC_AUTO_TEST_CASE(encode_base64_empty_test)
{
    data_chunk decoded;
    BC_REQUIRE_EQUAL(encode_base64(decoded), "");
}

BC_AUTO_TEST_CASE(decode_base64_empty_test)
{
    data_chunk result;
    BC_REQUIRE(decode_base64(result, ""));
    BC_REQUIRE(result == data_chunk());
}

BC_AUTO_TEST_CASE(encode_base64_test)
{
    data_chunk decoded(BASE64_DATA_MURRAY);
    BC_REQUIRE_EQUAL(encode_base64(decoded), BASE64_MURRAY);
}

BC_AUTO_TEST_CASE(decode_base64_valid_test)
{
    data_chunk result;
    BC_REQUIRE(decode_base64(result, BASE64_MURRAY));
    BC_REQUIRE(result == data_chunk(BASE64_DATA_MURRAY));
}

BC_AUTO_TEST_CASE(encode_base64_padded_test)
{
    data_chunk decoded(BASE64_DATA_BOOK);
    BC_REQUIRE_EQUAL(encode_base64(decoded), BASE64_BOOK);
}

BC_AUTO_TEST_CASE(decode_base64_padded_valid_test)
{
    data_chunk result;
    BC_REQUIRE(decode_base64(result, BASE64_BOOK));
    BC_REQUIRE(result == data_chunk(BASE64_DATA_BOOK));
}

BC_AUTO_TEST_CASE(decode_base64_invalid_test)
{
    data_chunk result;
    BC_REQUIRE(!decode_base64(result, "!@#$%^&*()"));
}

BC_AUTO_TEST_SUITE_END()
