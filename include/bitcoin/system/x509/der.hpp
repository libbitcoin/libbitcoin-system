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
#ifndef LIBBITCOIN_SYSTEM_X509_DER_HPP
#define LIBBITCOIN_SYSTEM_X509_DER_HPP

#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>

namespace libbitcoin {
namespace system {
namespace x509 {
namespace der {

/// Universal tags (single byte, low tag number form).
constexpr uint8_t boolean_tag = 0x01;
constexpr uint8_t integer_tag = 0x02;
constexpr uint8_t bit_string_tag = 0x03;
constexpr uint8_t octet_string_tag = 0x04;
constexpr uint8_t null_tag = 0x05;
constexpr uint8_t oid_tag = 0x06;
constexpr uint8_t utf8_string_tag = 0x0c;
constexpr uint8_t printable_string_tag = 0x13;
constexpr uint8_t ia5_string_tag = 0x16;
constexpr uint8_t utc_time_tag = 0x17;
constexpr uint8_t generalized_time_tag = 0x18;
constexpr uint8_t sequence_tag = 0x30;
constexpr uint8_t set_tag = 0x31;

/// Context specific tags: explicit (constructed) and implicit (primitive).
constexpr uint8_t explicit_tag(uint8_t number) NOEXCEPT
{
    return bit_or<uint8_t>(0xa0, number);
}

constexpr uint8_t implicit_tag(uint8_t number) NOEXCEPT
{
    return bit_or<uint8_t>(0x80, number);
}

/// Strict (distinguished) encoding reader over a byte span.
/// A failed read invalidates the reader, and all later reads return empty.
class BC_API reader
{
public:
    reader(const_byte_span data) NOEXCEPT;

    /// True if no read has failed.
    operator bool() const NOEXCEPT;

    /// True if no read has failed and all data has been read.
    bool is_complete() const NOEXCEPT;

    /// The tag of the next element, zero if none.
    uint8_t peek() const NOEXCEPT;

    /// Content of the next element, which must have the tag.
    const_byte_span read(uint8_t tag) NOEXCEPT;

    /// Encoding (tag, length and content) of the next element with the tag.
    const_byte_span read_encoding(uint8_t tag) NOEXCEPT;

    /// Reader over the content of the next element with the tag.
    reader read_nested(uint8_t tag) NOEXCEPT;

    /// Skip the next element, of any tag.
    void skip() NOEXCEPT;

    /// Typed universal elements.
    bool read_boolean() NOEXCEPT;
    const_byte_span read_integer() NOEXCEPT;
    uint64_t read_unsigned() NOEXCEPT;
    const_byte_span read_oid() NOEXCEPT;
    const_byte_span read_bit_string() NOEXCEPT;
    const_byte_span read_bit_string(uint8_t& unused) NOEXCEPT;
    const_byte_span read_octet_string() NOEXCEPT;
    void read_null() NOEXCEPT;

    /// UTCTime or GeneralizedTime (Zulu, with seconds) as unix seconds.
    uint64_t read_time() NOEXCEPT;

protected:
    const_byte_span next(uint8_t& tag, size_t& header) NOEXCEPT;
    const_byte_span read_encoding(uint8_t tag, size_t& header) NOEXCEPT;
    const_byte_span invalidate() NOEXCEPT;

private:
    const_byte_span data_;
    bool valid_;
};

/// Distinguished encoding writer.
class BC_API writer
{
public:
    /// The encoded elements.
    const data_chunk& data() const NOEXCEPT;

    /// Element of the tag with the content.
    void write(uint8_t tag, const_byte_span content) NOEXCEPT;

    /// Element of the tag with the elements of the writer as content.
    void write_nested(uint8_t tag, const writer& content) NOEXCEPT;

    /// Typed universal elements.
    void write_boolean(bool value) NOEXCEPT;
    void write_integer(const_byte_span magnitude) NOEXCEPT;
    void write_unsigned(uint64_t value) NOEXCEPT;
    void write_oid(const_byte_span oid) NOEXCEPT;
    void write_bit_string(const_byte_span bits, uint8_t unused=0) NOEXCEPT;
    void write_octet_string(const_byte_span value) NOEXCEPT;
    void write_null() NOEXCEPT;
    void write_string(uint8_t tag, const std::string& text) NOEXCEPT;

    /// UTCTime before 2050, otherwise GeneralizedTime (rfc5280).
    void write_time(uint64_t seconds) NOEXCEPT;

private:
    data_chunk data_{};
};

} // namespace der
} // namespace x509
} // namespace system
} // namespace libbitcoin

#endif
