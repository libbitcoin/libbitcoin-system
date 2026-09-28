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

BOOST_AUTO_TEST_SUITE(der_tests)

using namespace x509::der;
using reader = x509::der::reader;
using writer = x509::der::writer;

static data_chunk chunk(const_byte_span bytes)
{
    return { bytes.begin(), bytes.end() };
}

// Element of the tag with the text as content (short form).
static data_chunk element(uint8_t tag, const std::string& text)
{
    return splice(data_chunk{ tag, narrow_cast<uint8_t>(text.size()) }, to_chunk(text));
}

// reader

BOOST_AUTO_TEST_CASE(der__reader__empty__complete)
{
    const reader source{ const_byte_span{} };
    BOOST_REQUIRE(source);
    BOOST_REQUIRE(source.is_complete());
    BOOST_REQUIRE_EQUAL(source.peek(), 0x00);
}

BOOST_AUTO_TEST_CASE(der__reader_read__expected_tag__content)
{
    const auto data = base16_chunk("0403010203");
    reader source{ data };
    BOOST_REQUIRE_EQUAL(source.peek(), octet_string_tag);

    const auto content = source.read(octet_string_tag);
    BOOST_REQUIRE_EQUAL(chunk(content), base16_chunk("010203"));
    BOOST_REQUIRE(source.is_complete());
}

BOOST_AUTO_TEST_CASE(der__reader_read__unexpected_tag__invalid)
{
    const auto data = base16_chunk("0403010203");
    reader source{ data };
    BOOST_REQUIRE(source.read(integer_tag).empty());
    BOOST_REQUIRE(!source);
    BOOST_REQUIRE(!source.is_complete());
    BOOST_REQUIRE_EQUAL(source.peek(), 0x00);
    BOOST_REQUIRE(source.read(octet_string_tag).empty());
}

BOOST_AUTO_TEST_CASE(der__reader_read__truncated_header__invalid)
{
    const auto data = base16_chunk("04");
    reader source{ data };
    BOOST_REQUIRE(source.read(octet_string_tag).empty());
    BOOST_REQUIRE(!source);
}

BOOST_AUTO_TEST_CASE(der__reader_read__truncated_content__invalid)
{
    const auto data = base16_chunk("04030102");
    reader source{ data };
    BOOST_REQUIRE(source.read(octet_string_tag).empty());
    BOOST_REQUIRE(!source);
}

BOOST_AUTO_TEST_CASE(der__reader_read__high_tag_number__invalid)
{
    const auto data = base16_chunk("1f0100");
    reader source{ data };
    BOOST_REQUIRE(source.read(0x1f).empty());
    BOOST_REQUIRE(!source);
}

BOOST_AUTO_TEST_CASE(der__reader_read__long_form__content)
{
    const data_chunk content(128, 0x42);
    const auto data = splice(base16_chunk("048180"), content);
    reader source{ data };
    BOOST_REQUIRE_EQUAL(chunk(source.read(octet_string_tag)), content);
    BOOST_REQUIRE(source.is_complete());
}

BOOST_AUTO_TEST_CASE(der__reader_read__two_byte_long_form__content)
{
    const data_chunk content(300, 0x42);
    const auto data = splice(base16_chunk("0482012c"), content);
    reader source{ data };
    BOOST_REQUIRE_EQUAL(source.read(octet_string_tag).size(), 300u);
    BOOST_REQUIRE(source.is_complete());
}

BOOST_AUTO_TEST_CASE(der__reader_read__indefinite_length__invalid)
{
    const auto data = base16_chunk("30800000");
    reader source{ data };
    BOOST_REQUIRE(source.read(sequence_tag).empty());
    BOOST_REQUIRE(!source);
}

BOOST_AUTO_TEST_CASE(der__reader_read__non_minimal_long_form__invalid)
{
    const auto data = base16_chunk("04817f");
    reader source{ data };
    BOOST_REQUIRE(source.read(octet_string_tag).empty());
    BOOST_REQUIRE(!source);
}

BOOST_AUTO_TEST_CASE(der__reader_read__leading_zero_length__invalid)
{
    const auto data = base16_chunk("04820080");
    reader source{ data };
    BOOST_REQUIRE(source.read(octet_string_tag).empty());
    BOOST_REQUIRE(!source);
}

BOOST_AUTO_TEST_CASE(der__reader_read__five_length_bytes__invalid)
{
    const auto data = base16_chunk("04850000000001");
    reader source{ data };
    BOOST_REQUIRE(source.read(octet_string_tag).empty());
    BOOST_REQUIRE(!source);
}

BOOST_AUTO_TEST_CASE(der__reader_read__truncated_length_bytes__invalid)
{
    const auto data = base16_chunk("048201");
    reader source{ data };
    BOOST_REQUIRE(source.read(octet_string_tag).empty());
    BOOST_REQUIRE(!source);
}

BOOST_AUTO_TEST_CASE(der__reader_read__long_form_beyond_data__invalid)
{
    const auto data = base16_chunk("04820100ff");
    reader source{ data };
    BOOST_REQUIRE(source.read(octet_string_tag).empty());
    BOOST_REQUIRE(!source);
}

BOOST_AUTO_TEST_CASE(der__reader_read_encoding__expected_tag__whole_element)
{
    const auto data = base16_chunk("0403010203050000");
    reader source{ data };
    BOOST_REQUIRE_EQUAL(chunk(source.read_encoding(octet_string_tag)), base16_chunk("0403010203"));
    BOOST_REQUIRE_EQUAL(source.peek(), null_tag);
}

BOOST_AUTO_TEST_CASE(der__reader_read_nested__sequence__content_reader)
{
    const auto data = base16_chunk("3006020101020102");
    reader source{ data };
    auto nested = source.read_nested(sequence_tag);
    BOOST_REQUIRE(source.is_complete());
    BOOST_REQUIRE_EQUAL(nested.read_unsigned(), 1u);
    BOOST_REQUIRE_EQUAL(nested.read_unsigned(), 2u);
    BOOST_REQUIRE(nested.is_complete());
}

BOOST_AUTO_TEST_CASE(der__reader_read_nested__unexpected_tag__both_invalid)
{
    const auto data = base16_chunk("3100");
    reader source{ data };
    const auto nested = source.read_nested(sequence_tag);
    BOOST_REQUIRE(!source);
    BOOST_REQUIRE(!nested);
}

BOOST_AUTO_TEST_CASE(der__reader_skip__any_tag__next_element)
{
    const auto data = base16_chunk("a0030201010500");
    reader source{ data };
    source.skip();
    BOOST_REQUIRE_EQUAL(source.peek(), null_tag);
    source.read_null();
    BOOST_REQUIRE(source.is_complete());
}

BOOST_AUTO_TEST_CASE(der__reader_skip__malformed__invalid)
{
    const auto data = base16_chunk("04");
    reader source{ data };
    source.skip();
    BOOST_REQUIRE(!source);
}

// boolean

BOOST_AUTO_TEST_CASE(der__reader_read_boolean__true__true)
{
    const auto data = base16_chunk("0101ff");
    reader source{ data };
    BOOST_REQUIRE(source.read_boolean());
    BOOST_REQUIRE(source.is_complete());
}

BOOST_AUTO_TEST_CASE(der__reader_read_boolean__false__false)
{
    const auto data = base16_chunk("010100");
    reader source{ data };
    BOOST_REQUIRE(!source.read_boolean());
    BOOST_REQUIRE(source.is_complete());
}

BOOST_AUTO_TEST_CASE(der__reader_read_boolean__non_canonical__invalid)
{
    const auto data = base16_chunk("010101");
    reader source{ data };
    BOOST_REQUIRE(!source.read_boolean());
    BOOST_REQUIRE(!source);
}

BOOST_AUTO_TEST_CASE(der__reader_read_boolean__two_bytes__invalid)
{
    const auto data = base16_chunk("0102ffff");
    reader source{ data };
    BOOST_REQUIRE(!source.read_boolean());
    BOOST_REQUIRE(!source);
}

// integer

BOOST_AUTO_TEST_CASE(der__reader_read_integer__zero__content)
{
    const auto data = base16_chunk("020100");
    reader source{ data };
    BOOST_REQUIRE_EQUAL(chunk(source.read_integer()), base16_chunk("00"));
    BOOST_REQUIRE(source.is_complete());
}

BOOST_AUTO_TEST_CASE(der__reader_read_integer__sign_padded__content)
{
    const auto data = base16_chunk("02020080");
    reader source{ data };
    BOOST_REQUIRE_EQUAL(chunk(source.read_integer()), base16_chunk("0080"));
    BOOST_REQUIRE(source.is_complete());
}

BOOST_AUTO_TEST_CASE(der__reader_read_integer__negative__content)
{
    const auto data = base16_chunk("0202ff7f");
    reader source{ data };
    BOOST_REQUIRE_EQUAL(chunk(source.read_integer()), base16_chunk("ff7f"));
    BOOST_REQUIRE(source.is_complete());
}

BOOST_AUTO_TEST_CASE(der__reader_read_integer__redundant_zero__invalid)
{
    const auto data = base16_chunk("0202007f");
    reader source{ data };
    BOOST_REQUIRE(source.read_integer().empty());
    BOOST_REQUIRE(!source);
}

BOOST_AUTO_TEST_CASE(der__reader_read_integer__redundant_ones__invalid)
{
    const auto data = base16_chunk("0202ff80");
    reader source{ data };
    BOOST_REQUIRE(source.read_integer().empty());
    BOOST_REQUIRE(!source);
}

BOOST_AUTO_TEST_CASE(der__reader_read_integer__empty__invalid)
{
    const auto data = base16_chunk("0200");
    reader source{ data };
    BOOST_REQUIRE(source.read_integer().empty());
    BOOST_REQUIRE(!source);
}

BOOST_AUTO_TEST_CASE(der__reader_read_unsigned__two_bytes__value)
{
    const auto data = base16_chunk("02020100");
    reader source{ data };
    BOOST_REQUIRE_EQUAL(source.read_unsigned(), 256u);
    BOOST_REQUIRE(source.is_complete());
}

BOOST_AUTO_TEST_CASE(der__reader_read_unsigned__maximum__value)
{
    const auto data = base16_chunk("020900ffffffffffffffff");
    reader source{ data };
    BOOST_REQUIRE_EQUAL(source.read_unsigned(), max_uint64);
    BOOST_REQUIRE(source.is_complete());
}

BOOST_AUTO_TEST_CASE(der__reader_read_unsigned__overflow__invalid)
{
    const auto data = base16_chunk("0209010000000000000000");
    reader source{ data };
    BOOST_REQUIRE_EQUAL(source.read_unsigned(), 0u);
    BOOST_REQUIRE(!source);
}

BOOST_AUTO_TEST_CASE(der__reader_read_unsigned__negative__invalid)
{
    const auto data = base16_chunk("0201ff");
    reader source{ data };
    BOOST_REQUIRE_EQUAL(source.read_unsigned(), 0u);
    BOOST_REQUIRE(!source);
}

// object identifier

BOOST_AUTO_TEST_CASE(der__reader_read_oid__valid__content)
{
    const auto data = base16_chunk("06072a8648ce3d0201");
    reader source{ data };
    BOOST_REQUIRE_EQUAL(chunk(source.read_oid()), to_chunk(x509::oid::ec_public_key));
    BOOST_REQUIRE(source.is_complete());
}

BOOST_AUTO_TEST_CASE(der__reader_read_oid__empty__invalid)
{
    const auto data = base16_chunk("0600");
    reader source{ data };
    BOOST_REQUIRE(source.read_oid().empty());
    BOOST_REQUIRE(!source);
}

BOOST_AUTO_TEST_CASE(der__reader_read_oid__unterminated__invalid)
{
    const auto data = base16_chunk("06022a86");
    reader source{ data };
    BOOST_REQUIRE(source.read_oid().empty());
    BOOST_REQUIRE(!source);
}

BOOST_AUTO_TEST_CASE(der__reader_read_oid__non_minimal_subidentifier__invalid)
{
    const auto data = base16_chunk("06032a8001");
    reader source{ data };
    BOOST_REQUIRE(source.read_oid().empty());
    BOOST_REQUIRE(!source);
}

// bit string

BOOST_AUTO_TEST_CASE(der__reader_read_bit_string__no_unused__bits)
{
    const auto data = base16_chunk("030300abcd");
    reader source{ data };
    BOOST_REQUIRE_EQUAL(chunk(source.read_bit_string()), base16_chunk("abcd"));
    BOOST_REQUIRE(source.is_complete());
}

BOOST_AUTO_TEST_CASE(der__reader_read_bit_string__unused__invalid)
{
    const auto data = base16_chunk("03020780");
    reader source{ data };
    BOOST_REQUIRE(source.read_bit_string().empty());
    BOOST_REQUIRE(!source);
}

BOOST_AUTO_TEST_CASE(der__reader_read_bit_string__zero_padding__bits_and_unused)
{
    const auto data = base16_chunk("03020580");
    reader source{ data };
    uint8_t unused{};
    BOOST_REQUIRE_EQUAL(chunk(source.read_bit_string(unused)), base16_chunk("80"));
    BOOST_REQUIRE_EQUAL(unused, 5u);
    BOOST_REQUIRE(source.is_complete());
}

BOOST_AUTO_TEST_CASE(der__reader_read_bit_string__set_padding__invalid)
{
    const auto data = base16_chunk("03020581");
    reader source{ data };
    uint8_t unused{};
    BOOST_REQUIRE(source.read_bit_string(unused).empty());
    BOOST_REQUIRE(!source);
}

BOOST_AUTO_TEST_CASE(der__reader_read_bit_string__eight_unused__invalid)
{
    const auto data = base16_chunk("03020800");
    reader source{ data };
    uint8_t unused{};
    BOOST_REQUIRE(source.read_bit_string(unused).empty());
    BOOST_REQUIRE(!source);
}

BOOST_AUTO_TEST_CASE(der__reader_read_bit_string__empty_content__invalid)
{
    const auto data = base16_chunk("0300");
    reader source{ data };
    BOOST_REQUIRE(source.read_bit_string().empty());
    BOOST_REQUIRE(!source);
}

BOOST_AUTO_TEST_CASE(der__reader_read_bit_string__no_bits__empty)
{
    const auto data = base16_chunk("030100");
    reader source{ data };
    BOOST_REQUIRE(source.read_bit_string().empty());
    BOOST_REQUIRE(source.is_complete());
}

BOOST_AUTO_TEST_CASE(der__reader_read_bit_string__no_bits_unused__invalid)
{
    const auto data = base16_chunk("030101");
    reader source{ data };
    uint8_t unused{};
    BOOST_REQUIRE(source.read_bit_string(unused).empty());
    BOOST_REQUIRE(!source);
}

// octet string, null

BOOST_AUTO_TEST_CASE(der__reader_read_octet_string__valid__content)
{
    const auto data = base16_chunk("0402abcd");
    reader source{ data };
    BOOST_REQUIRE_EQUAL(chunk(source.read_octet_string()), base16_chunk("abcd"));
    BOOST_REQUIRE(source.is_complete());
}

BOOST_AUTO_TEST_CASE(der__reader_read_null__empty__complete)
{
    const auto data = base16_chunk("0500");
    reader source{ data };
    source.read_null();
    BOOST_REQUIRE(source.is_complete());
}

BOOST_AUTO_TEST_CASE(der__reader_read_null__content__invalid)
{
    const auto data = base16_chunk("050100");
    reader source{ data };
    source.read_null();
    BOOST_REQUIRE(!source);
}

// time

BOOST_AUTO_TEST_CASE(der__reader_read_time__utc__seconds)
{
    const auto data = element(utc_time_tag, "250101000000Z");
    reader source{ data };
    BOOST_REQUIRE_EQUAL(source.read_time(), 1735689600u);
    BOOST_REQUIRE(source.is_complete());
}

BOOST_AUTO_TEST_CASE(der__reader_read_time__utc_before_pivot__twenty_first_century)
{
    const auto data = element(utc_time_tag, "491231235959Z");
    reader source{ data };
    BOOST_REQUIRE_EQUAL(source.read_time(), 2524607999u);
}

BOOST_AUTO_TEST_CASE(der__reader_read_time__utc_before_epoch__invalid)
{
    const auto data = element(utc_time_tag, "691231235959Z");
    reader source{ data };
    BOOST_REQUIRE_EQUAL(source.read_time(), 0u);
    BOOST_REQUIRE(!source);
}

BOOST_AUTO_TEST_CASE(der__reader_read_time__utc_epoch__zero)
{
    const auto data = element(utc_time_tag, "700101000000Z");
    reader source{ data };
    BOOST_REQUIRE_EQUAL(source.read_time(), 0u);
    BOOST_REQUIRE(source.is_complete());
}

BOOST_AUTO_TEST_CASE(der__reader_read_time__generalized__seconds)
{
    const auto data = element(generalized_time_tag, "20350101000000Z");
    reader source{ data };
    BOOST_REQUIRE_EQUAL(source.read_time(), 2051222400u);
    BOOST_REQUIRE(source.is_complete());
}

BOOST_AUTO_TEST_CASE(der__reader_read_time__generalized_no_expiry__seconds)
{
    const auto data = element(generalized_time_tag, "99991231235959Z");
    reader source{ data };
    BOOST_REQUIRE_EQUAL(source.read_time(), 253402300799u);
}

BOOST_AUTO_TEST_CASE(der__reader_read_time__leap_day__seconds)
{
    const auto data = element(generalized_time_tag, "20240229000000Z");
    reader source{ data };
    BOOST_REQUIRE_EQUAL(source.read_time(), 1709164800u);
}

BOOST_AUTO_TEST_CASE(der__reader_read_time__quadricentennial_leap_day__seconds)
{
    const auto data = element(generalized_time_tag, "20000229000000Z");
    reader source{ data };
    BOOST_REQUIRE_EQUAL(source.read_time(), 951782400u);
}

BOOST_AUTO_TEST_CASE(der__reader_read_time__non_leap_day__invalid)
{
    const auto data = element(generalized_time_tag, "20250229000000Z");
    reader source{ data };
    BOOST_REQUIRE_EQUAL(source.read_time(), 0u);
    BOOST_REQUIRE(!source);
}

BOOST_AUTO_TEST_CASE(der__reader_read_time__centennial_leap_day__invalid)
{
    const auto data = element(generalized_time_tag, "21000229000000Z");
    reader source{ data };
    BOOST_REQUIRE_EQUAL(source.read_time(), 0u);
    BOOST_REQUIRE(!source);
}

BOOST_AUTO_TEST_CASE(der__reader_read_time__thirty_first_short_month__invalid)
{
    const auto data = element(generalized_time_tag, "20250431000000Z");
    reader source{ data };
    BOOST_REQUIRE_EQUAL(source.read_time(), 0u);
    BOOST_REQUIRE(!source);
}

BOOST_AUTO_TEST_CASE(der__reader_read_time__month_thirteen__invalid)
{
    const auto data = element(generalized_time_tag, "20251301000000Z");
    reader source{ data };
    BOOST_REQUIRE_EQUAL(source.read_time(), 0u);
    BOOST_REQUIRE(!source);
}

BOOST_AUTO_TEST_CASE(der__reader_read_time__day_zero__invalid)
{
    const auto data = element(generalized_time_tag, "20250100000000Z");
    reader source{ data };
    BOOST_REQUIRE_EQUAL(source.read_time(), 0u);
    BOOST_REQUIRE(!source);
}

BOOST_AUTO_TEST_CASE(der__reader_read_time__hour_twenty_four__invalid)
{
    const auto data = element(generalized_time_tag, "20250101240000Z");
    reader source{ data };
    BOOST_REQUIRE_EQUAL(source.read_time(), 0u);
    BOOST_REQUIRE(!source);
}

BOOST_AUTO_TEST_CASE(der__reader_read_time__minute_sixty__invalid)
{
    const auto data = element(generalized_time_tag, "20250101006000Z");
    reader source{ data };
    BOOST_REQUIRE_EQUAL(source.read_time(), 0u);
    BOOST_REQUIRE(!source);
}

BOOST_AUTO_TEST_CASE(der__reader_read_time__second_sixty__invalid)
{
    const auto data = element(generalized_time_tag, "20250101000060Z");
    reader source{ data };
    BOOST_REQUIRE_EQUAL(source.read_time(), 0u);
    BOOST_REQUIRE(!source);
}

BOOST_AUTO_TEST_CASE(der__reader_read_time__fraction__invalid)
{
    const auto data = element(generalized_time_tag, "20250101000000.5Z");
    reader source{ data };
    BOOST_REQUIRE_EQUAL(source.read_time(), 0u);
    BOOST_REQUIRE(!source);
}

BOOST_AUTO_TEST_CASE(der__reader_read_time__non_digit__invalid)
{
    const auto data = element(utc_time_tag, "25010100000xZ");
    reader source{ data };
    BOOST_REQUIRE_EQUAL(source.read_time(), 0u);
    BOOST_REQUIRE(!source);
}

BOOST_AUTO_TEST_CASE(der__reader_read_time__not_zulu__invalid)
{
    const auto data = element(utc_time_tag, "2501010000000");
    reader source{ data };
    BOOST_REQUIRE_EQUAL(source.read_time(), 0u);
    BOOST_REQUIRE(!source);
}

BOOST_AUTO_TEST_CASE(der__reader_read_time__not_time__invalid)
{
    const auto data = base16_chunk("020100");
    reader source{ data };
    BOOST_REQUIRE_EQUAL(source.read_time(), 0u);
    BOOST_REQUIRE(!source);
}

// writer

BOOST_AUTO_TEST_CASE(der__writer_write__short_form__element)
{
    writer sink{};
    sink.write(octet_string_tag, base16_chunk("010203"));
    BOOST_REQUIRE_EQUAL(sink.data(), base16_chunk("0403010203"));
}

BOOST_AUTO_TEST_CASE(der__writer_write__one_byte_long_form__element)
{
    const data_chunk content(200, 0x42);
    writer sink{};
    sink.write(octet_string_tag, content);
    BOOST_REQUIRE_EQUAL(sink.data(), splice(base16_chunk("0481c8"), content));
}

BOOST_AUTO_TEST_CASE(der__writer_write__two_byte_long_form__element)
{
    const data_chunk content(300, 0x42);
    writer sink{};
    sink.write(octet_string_tag, content);
    BOOST_REQUIRE_EQUAL(sink.data(), splice(base16_chunk("0482012c"), content));
}

BOOST_AUTO_TEST_CASE(der__writer_write_nested__sequence__element)
{
    writer content{};
    content.write_null();

    writer sink{};
    sink.write_nested(sequence_tag, content);
    BOOST_REQUIRE_EQUAL(sink.data(), base16_chunk("30020500"));
}

BOOST_AUTO_TEST_CASE(der__writer_write_boolean__both__elements)
{
    writer sink{};
    sink.write_boolean(true);
    sink.write_boolean(false);
    BOOST_REQUIRE_EQUAL(sink.data(), base16_chunk("0101ff010100"));
}

BOOST_AUTO_TEST_CASE(der__writer_write_integer__empty__zero)
{
    writer sink{};
    sink.write_integer({});
    BOOST_REQUIRE_EQUAL(sink.data(), base16_chunk("020100"));
}

BOOST_AUTO_TEST_CASE(der__writer_write_integer__leading_zeros__minimal)
{
    writer sink{};
    sink.write_integer(base16_chunk("00000102"));
    BOOST_REQUIRE_EQUAL(sink.data(), base16_chunk("02020102"));
}

BOOST_AUTO_TEST_CASE(der__writer_write_integer__high_bit__sign_padded)
{
    writer sink{};
    sink.write_integer(base16_chunk("80"));
    BOOST_REQUIRE_EQUAL(sink.data(), base16_chunk("02020080"));
}

BOOST_AUTO_TEST_CASE(der__writer_write_unsigned__values__minimal)
{
    writer sink{};
    sink.write_unsigned(0);
    sink.write_unsigned(256);
    sink.write_unsigned(max_uint64);
    BOOST_REQUIRE_EQUAL(sink.data(), base16_chunk("020100" "02020100" "020900ffffffffffffffff"));
}

BOOST_AUTO_TEST_CASE(der__writer_write_oid__ec_public_key__element)
{
    writer sink{};
    sink.write_oid(x509::oid::ec_public_key);
    BOOST_REQUIRE_EQUAL(sink.data(), base16_chunk("06072a8648ce3d0201"));
}

BOOST_AUTO_TEST_CASE(der__writer_write_bit_string__default_and_unused__elements)
{
    writer sink{};
    sink.write_bit_string(base16_chunk("abcd"));
    sink.write_bit_string(base16_chunk("80"), 7);
    BOOST_REQUIRE_EQUAL(sink.data(), base16_chunk("030300abcd" "03020780"));
}

BOOST_AUTO_TEST_CASE(der__writer_write_octet_string_null_string__elements)
{
    writer sink{};
    sink.write_octet_string(base16_chunk("abcd"));
    sink.write_null();
    sink.write_string(utf8_string_tag, "bs");
    BOOST_REQUIRE_EQUAL(sink.data(), base16_chunk("0402abcd" "0500" "0c026273"));
}

BOOST_AUTO_TEST_CASE(der__writer_write_time__before_2050__utc)
{
    writer sink{};
    sink.write_time(2524607999);
    BOOST_REQUIRE_EQUAL(sink.data(), element(utc_time_tag, "491231235959Z"));
}

BOOST_AUTO_TEST_CASE(der__writer_write_time__from_2050__generalized)
{
    writer sink{};
    sink.write_time(2524608000);
    BOOST_REQUIRE_EQUAL(sink.data(), element(generalized_time_tag, "20500101000000Z"));
}

BOOST_AUTO_TEST_CASE(der__writer_write_time__leap_day__round_trip)
{
    writer sink{};
    sink.write_time(1709164800);
    BOOST_REQUIRE_EQUAL(sink.data(), element(utc_time_tag, "240229000000Z"));

    reader source{ sink.data() };
    BOOST_REQUIRE_EQUAL(source.read_time(), 1709164800u);
}

BOOST_AUTO_TEST_CASE(der__writer_write_time__no_expiry__round_trip)
{
    writer sink{};
    sink.write_time(253402300799);
    BOOST_REQUIRE_EQUAL(sink.data(), element(generalized_time_tag, "99991231235959Z"));

    reader source{ sink.data() };
    BOOST_REQUIRE_EQUAL(source.read_time(), 253402300799u);
}

// context tags

BOOST_AUTO_TEST_CASE(der__explicit_implicit_tag__numbers__expected)
{
    static_assert(explicit_tag(0) == 0xa0);
    static_assert(explicit_tag(3) == 0xa3);
    static_assert(implicit_tag(2) == 0x82);
    static_assert(implicit_tag(7) == 0x87);
    BOOST_REQUIRE(true);
}

BOOST_AUTO_TEST_SUITE_END()
