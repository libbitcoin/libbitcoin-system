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
#include <bitcoin/system/x509/der.hpp>

#include <algorithm>
#include <string>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/endian/endian.hpp>
#include <bitcoin/system/math/math.hpp>
#include <bitcoin/system/unicode/unicode.hpp>

// itu-t x.690 (distinguished encoding rules), rfc5280 (time)

namespace libbitcoin {
namespace system {
namespace x509 {
namespace der {

BC_PUSH_WARNING(NO_ARRAY_INDEXING)
BC_PUSH_WARNING(NO_DYNAMIC_ARRAY_INDEXING)

constexpr size_t long_form = 0x80;
constexpr size_t maximum_length_bytes = sizeof(uint32_t);
constexpr uint8_t high_tag_number = 0x1f;
constexpr uint8_t boolean_true = 0xff;
constexpr uint8_t boolean_false = 0x00;
constexpr uint8_t continuation = 0x80;

constexpr uint64_t epoch_year = 1970;
constexpr uint64_t utc_limit_year = 2050;
constexpr uint64_t utc_century = 1900;
constexpr uint64_t utc_pivot = 50;
constexpr uint64_t seconds_per_minute = 60;
constexpr uint64_t seconds_per_hour = 3600;
constexpr uint64_t seconds_per_day = 86400;
constexpr size_t utc_year_digits = 2;
constexpr size_t generalized_year_digits = 4;
constexpr size_t time_digits = 10;
constexpr char zulu = 'Z';
constexpr uint64_t digit_zero = '0';

// Gregorian calendar (from 1970).
// ----------------------------------------------------------------------------
// [Hinnant] chrono-compatible low-level date algorithms.

constexpr bool is_leap_year(uint64_t year) NOEXCEPT
{
    const auto quadrennial = is_zero(year % 4_u64);
    const auto centennial = is_zero(year % 100_u64);
    const auto quadricentennial = is_zero(year % 400_u64);
    return (quadrennial && !centennial) || quadricentennial;
}

constexpr uint64_t days_in_month(uint64_t year, uint64_t month) NOEXCEPT
{
    if (month == 2_u64)
        return 28_u64 + to_int<uint64_t>(is_leap_year(year));

    const auto odd = bit_and(month + shift_right(month, 3), 1_u64);
    return 30_u64 + odd;
}

constexpr uint64_t to_days(uint64_t year, uint64_t month,
    uint64_t day) NOEXCEPT
{
    const auto shifted = year - to_int<uint64_t>(month <= 2_u64);
    const auto era = shifted / 400_u64;
    const auto year_of_era = shifted - era * 400_u64;
    const auto march = month > 2_u64 ? month - 3_u64 : month + 9_u64;
    const auto day_of_year = (153_u64 * march + 2_u64) / 5_u64 + sub1(day);
    const auto leaps = year_of_era / 4_u64 - year_of_era / 100_u64;
    const auto day_of_era = year_of_era * 365_u64 + leaps + day_of_year;
    return era * 146097_u64 + day_of_era - 719468_u64;
}

constexpr void to_date(uint64_t& year, uint64_t& month, uint64_t& day,
    uint64_t days) NOEXCEPT
{
    const auto shifted = days + 719468_u64;
    const auto era = shifted / 146097_u64;
    const auto day_of_era = shifted - era * 146097_u64;
    const auto quadrennia = day_of_era / 1460_u64;
    const auto centuries = day_of_era / 36524_u64;
    const auto eras = day_of_era / 146096_u64;
    const auto skips = quadrennia - centuries + eras;
    const auto year_of_era = (day_of_era - skips) / 365_u64;
    const auto leaps = year_of_era / 4_u64 - year_of_era / 100_u64;
    const auto day_of_year = day_of_era - (365_u64 * year_of_era + leaps);
    const auto march = (5_u64 * day_of_year + 2_u64) / 153_u64;
    day = add1(day_of_year - (153_u64 * march + 2_u64) / 5_u64);
    month = march < 10_u64 ? march + 3_u64 : march - 9_u64;
    year = year_of_era + era * 400_u64 + to_int<uint64_t>(month <= 2_u64);
}

// reader
// ----------------------------------------------------------------------------

reader::reader(const_byte_span data) NOEXCEPT
  : data_(data), valid_(true)
{
}

reader::operator bool() const NOEXCEPT
{
    return valid_;
}

bool reader::is_complete() const NOEXCEPT
{
    return valid_ && data_.empty();
}

uint8_t reader::peek() const NOEXCEPT
{
    return valid_ && !data_.empty() ? data_.front() : uint8_t{};
}

const_byte_span reader::read(uint8_t tag) NOEXCEPT
{
    size_t header{};
    const auto encoding = read_encoding(tag, header);
    return encoding.subspan(std::min(header, encoding.size()));
}

const_byte_span reader::read_encoding(uint8_t tag) NOEXCEPT
{
    size_t header{};
    return read_encoding(tag, header);
}

reader reader::read_nested(uint8_t tag) NOEXCEPT
{
    reader nested{ read(tag) };
    if (!valid_)
        nested.invalidate();

    return nested;
}

void reader::skip() NOEXCEPT
{
    uint8_t tag{};
    size_t header{};
    data_ = data_.subspan(next(tag, header).size());
}

bool reader::read_boolean() NOEXCEPT
{
    const auto content = read(boolean_tag);
    if (content.size() != one)
    {
        invalidate();
        return false;
    }

    const auto value = content.front();
    if ((value != boolean_true) && (value != boolean_false))
    {
        invalidate();
        return false;
    }

    return value == boolean_true;
}

// Minimal two's complement (x.690 8.3.2).
const_byte_span reader::read_integer() NOEXCEPT
{
    const auto content = read(integer_tag);
    if (content.empty())
        return invalidate();

    if (content.size() > one)
    {
        const auto first = content[0];
        const auto second = get_left(content[1]);
        const auto positive = is_zero(first) && !second;
        const auto negative = (first == max_uint8) && second;
        if (positive || negative)
            return invalidate();
    }

    return content;
}

uint64_t reader::read_unsigned() NOEXCEPT
{
    auto content = read_integer();
    if (content.empty() || get_left(content.front()))
    {
        invalidate();
        return {};
    }

    if (is_zero(content.front()) && (content.size() > one))
        content = content.subspan(one);

    if (content.size() > sizeof(uint64_t))
    {
        invalidate();
        return {};
    }

    uint64_t value{};
    for (const auto byte: content)
    {
        const auto shifted = shift_left(value, byte_bits);
        value = bit_or(shifted, wide_cast<uint64_t>(byte));
    }

    return value;
}

// Subidentifiers are minimal base 128 (x.690 8.19.2).
const_byte_span reader::read_oid() NOEXCEPT
{
    const auto content = read(oid_tag);
    if (content.empty() || get_left(content.back()))
        return invalidate();

    auto start = true;
    for (const auto byte: content)
    {
        if (start && (byte == continuation))
            return invalidate();

        start = !get_left(byte);
    }

    return content;
}

const_byte_span reader::read_bit_string() NOEXCEPT
{
    uint8_t unused{};
    const auto bits = read_bit_string(unused);
    if (!is_zero(unused))
        return invalidate();

    return bits;
}

// Unused bits are zero (x.690 11.2.1).
const_byte_span reader::read_bit_string(uint8_t& unused) NOEXCEPT
{
    const auto content = read(bit_string_tag);
    if (content.empty())
        return invalidate();

    unused = content.front();
    const auto bits = content.subspan(one);
    if (unused >= byte_bits)
        return invalidate();

    if (bits.empty())
    {
        if (!is_zero(unused))
            return invalidate();

        return bits;
    }

    const auto padding = unmask_right<uint8_t>(unused);
    if (!is_zero(bit_and(bits.back(), padding)))
        return invalidate();

    return bits;
}

const_byte_span reader::read_octet_string() NOEXCEPT
{
    return read(octet_string_tag);
}

void reader::read_null() NOEXCEPT
{
    if (!read(null_tag).empty())
        invalidate();
}

// Zulu with seconds and no fraction (x.690 11.7, 11.8, rfc5280 4.1.2.5).
uint64_t reader::read_time() NOEXCEPT
{
    const auto utc = (peek() == utc_time_tag);
    const auto text = read(utc ? utc_time_tag : generalized_time_tag);
    const auto year_digits = utc ? utc_year_digits : generalized_year_digits;
    const auto digits_size = year_digits + time_digits;
    if (text.size() != add1(digits_size))
    {
        invalidate();
        return {};
    }

    const auto digits = text.first(digits_size);
    const auto is_digit = [](uint8_t byte) NOEXCEPT
    {
        return is_ascii_number(byte);
    };

    const auto numeric = std::all_of(digits.begin(), digits.end(), is_digit);
    if (!numeric || (text.back() != zulu))
    {
        invalidate();
        return {};
    }

    const auto number = [&](size_t offset) NOEXCEPT
    {
        const auto count = is_zero(offset) ? year_digits : two;
        uint64_t value{};
        for (const auto byte: digits.subspan(offset, count))
            value = value * 10_u64 + wide_cast<uint64_t>(byte) - digit_zero;

        return value;
    };

    auto year = number(zero);
    if (utc)
        year += (year < utc_pivot) ? utc_century + 100_u64 : utc_century;

    const auto month = number(year_digits);
    const auto day = number(year_digits + 2_size);
    const auto hour = number(year_digits + 4_size);
    const auto minute = number(year_digits + 6_size);
    const auto second = number(year_digits + 8_size);

    const auto valid_month = (month >= 1_u64) && (month <= 12_u64);
    const auto valid_day = valid_month && (day >= 1_u64) &&
        (day <= days_in_month(year, month));
    const auto valid_clock = (hour < 24_u64) && (minute < 60_u64) &&
        (second < 60_u64);
    if ((year < epoch_year) || !valid_day || !valid_clock)
    {
        invalidate();
        return {};
    }

    const auto days = to_days(year, month, day);
    const auto clock = hour * seconds_per_hour + minute * seconds_per_minute;
    return days * seconds_per_day + clock + second;
}

// protected
// ----------------------------------------------------------------------------

// Low tag number form, definite and minimal length (x.690 8.1, 10.1).
const_byte_span reader::next(uint8_t& tag, size_t& header) NOEXCEPT
{
    if (!valid_ || (data_.size() < two))
        return invalidate();

    tag = data_[0];
    if (bit_and(tag, high_tag_number) == high_tag_number)
        return invalidate();

    const auto first = wide_cast<size_t>(data_[1]);
    auto length = first;
    header = two;

    if (first >= long_form)
    {
        const auto bytes = first - long_form;
        const auto truncated = data_.size() < (header + bytes);
        if (is_zero(bytes) || (bytes > maximum_length_bytes) || truncated ||
            is_zero(data_[header]))
            return invalidate();

        length = zero;
        for (const auto byte: data_.subspan(header, bytes))
        {
            const auto shifted = shift_left(length, byte_bits);
            length = bit_or(shifted, wide_cast<size_t>(byte));
        }

        if (length < long_form)
            return invalidate();

        header += bytes;
    }

    if (length > (data_.size() - header))
        return invalidate();

    return data_.first(header + length);
}

const_byte_span reader::read_encoding(uint8_t tag, size_t& header) NOEXCEPT
{
    uint8_t actual{};
    const auto encoding = next(actual, header);
    if (!valid_)
        return {};

    if (actual != tag)
        return invalidate();

    data_ = data_.subspan(encoding.size());
    return encoding;
}

const_byte_span reader::invalidate() NOEXCEPT
{
    valid_ = false;
    data_ = {};
    return {};
}

// writer
// ----------------------------------------------------------------------------

const data_chunk& writer::data() const NOEXCEPT
{
    return data_;
}

void writer::write(uint8_t tag, const_byte_span content) NOEXCEPT
{
    const auto size = content.size();
    data_.push_back(tag);

    if (size < long_form)
    {
        data_.push_back(narrow_cast<uint8_t>(size));
    }
    else
    {
        const auto bytes = ceilinged_divide(bit_width(size), byte_bits);
        data_.push_back(narrow_cast<uint8_t>(long_form + bytes));
        for (auto byte = bytes; !is_zero(byte); --byte)
        {
            const auto shifted = shift_right(size, sub1(byte) * byte_bits);
            data_.push_back(narrow_cast<uint8_t>(shifted));
        }
    }

    data_.insert(data_.end(), content.begin(), content.end());
}

void writer::write_nested(uint8_t tag, const writer& content) NOEXCEPT
{
    write(tag, content.data());
}

void writer::write_boolean(bool value) NOEXCEPT
{
    const data_array<one> content{ value ? boolean_true : boolean_false };
    write(boolean_tag, content);
}

// Unsigned big-endian magnitude, minimal with a sign byte if required.
void writer::write_integer(const_byte_span magnitude) NOEXCEPT
{
    auto digits = magnitude;
    while ((digits.size() > one) && is_zero(digits.front()))
        digits = digits.subspan(one);

    data_chunk content{};
    if (digits.empty() || get_left(digits.front()))
        content.push_back(0x00);

    content.insert(content.end(), digits.begin(), digits.end());
    write(integer_tag, content);
}

void writer::write_unsigned(uint64_t value) NOEXCEPT
{
    write_integer(to_big_endian(value));
}

void writer::write_oid(const_byte_span oid) NOEXCEPT
{
    write(oid_tag, oid);
}

void writer::write_bit_string(const_byte_span bits, uint8_t unused) NOEXCEPT
{
    data_chunk content{ unused };
    content.insert(content.end(), bits.begin(), bits.end());
    write(bit_string_tag, content);
}

void writer::write_octet_string(const_byte_span value) NOEXCEPT
{
    write(octet_string_tag, value);
}

void writer::write_null() NOEXCEPT
{
    write(null_tag, {});
}

void writer::write_string(uint8_t tag, const std::string& text) NOEXCEPT
{
    write(tag, to_chunk(text));
}

void writer::write_time(uint64_t seconds) NOEXCEPT
{
    uint64_t year{}, month{}, day{};
    const auto clock = seconds % seconds_per_day;
    to_date(year, month, day, seconds / seconds_per_day);

    const auto utc = (year < utc_limit_year);
    const auto year_digits = utc ? utc_year_digits : generalized_year_digits;
    const auto shown = utc ? year % 100_u64 : year;

    data_chunk text{};
    const auto append = [&](uint64_t value, size_t count) NOEXCEPT
    {
        for (auto digit = count; !is_zero(digit); --digit)
        {
            auto divisor = 1_u64;
            for (auto power = one; power < digit; ++power)
                divisor *= 10_u64;

            const auto number = (value / divisor) % 10_u64;
            text.push_back(narrow_cast<uint8_t>(digit_zero + number));
        }
    };

    append(shown, year_digits);
    append(month, two);
    append(day, two);
    append(clock / seconds_per_hour, two);
    append((clock % seconds_per_hour) / seconds_per_minute, two);
    append(clock % seconds_per_minute, two);
    text.push_back(zulu);
    write(utc ? utc_time_tag : generalized_time_tag, text);
}

BC_POP_WARNING()
BC_POP_WARNING()

} // namespace der
} // namespace x509
} // namespace system
} // namespace libbitcoin
