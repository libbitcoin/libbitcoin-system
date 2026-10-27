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
#include <bitcoin/system/serial/json/parser.hpp>

#include <charconv>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/math/math.hpp>
#include <bitcoin/system/serial/deserialize.hpp>

namespace libbitcoin {
namespace system {
namespace json {

BC_PUSH_WARNING(NO_ARRAY_INDEXING)
BC_PUSH_WARNING(NO_POINTER_ARITHMETIC)

constexpr uint32_t high_surrogate_begin = 0xd800;
constexpr uint32_t low_surrogate_begin = 0xdc00;
constexpr uint32_t low_surrogate_end = 0xdfff;
constexpr uint32_t surrogate_offset = 0x10000;
constexpr uint8_t continuation_lower = 0x80;
constexpr uint8_t continuation_upper = 0xbf;
constexpr uint8_t control_end = 0x20;

static bool is_decimal(char character) NOEXCEPT
{
    return character >= '0' && character <= '9';
}

// The magnitude of an out of range number is below the double domain.
static bool is_underflow(std::string_view token) NOEXCEPT
{
    constexpr int32_t saturation = 1'000'000;
    auto it = token.cbegin();
    if (*it == '-')
        ++it;

    int32_t magnitude{};
    if (*it == '0')
    {
        ++it;
        if (it != token.cend() && *it == '.')
            for (++it; it != token.cend() && *it == '0'; ++it)
                magnitude = std::max(magnitude - 1, -saturation);
    }
    else
    {
        for (; it != token.cend() && is_decimal(*it); ++it)
            magnitude = std::min(magnitude + 1, saturation);
    }

    while (it != token.cend() && *it != 'e' && *it != 'E')
        ++it;

    if (it == token.cend())
        return magnitude < 0;

    ++it;
    const auto negative = (*it == '-');
    if (*it == '-' || *it == '+')
        ++it;

    int32_t exponent{};
    for (; it != token.cend(); ++it)
        exponent = std::min(exponent * 10 + (*it - '0'), saturation);

    return (negative ? magnitude - exponent : magnitude + exponent) < 0;
}

// Return the hex value of the character, or -1.
static int32_t to_hex(char character) NOEXCEPT
{
    if (character >= '0' && character <= '9')
        return character - '0';
    if (character >= 'a' && character <= 'f')
        return character - 'a' + 10;
    if (character >= 'A' && character <= 'F')
        return character - 'A' + 10;

    return -1;
}

parser::parser(const parse_options& options) NOEXCEPT
  : options_{ options }
{
}

parser::~parser() NOEXCEPT
{
}

void parser::reset() NOEXCEPT
{
    stack_.clear();
    result_.emplace_null();
    token_.clear();
    ec_.clear();
    state_ = state::value;
    numeric_ = numeric::sign;
    literal_ = {};
    matched_ = zero;
    is_key_ = false;
    integral_ = false;
    hex_count_ = 0;
    hex_ = 0;
    high_ = 0;
    utf8_remain_ = 0;
    utf8_lower_ = 0;
    utf8_upper_ = 0;
}

bool parser::done() const NOEXCEPT
{
    return state_ == state::done;
}

value parser::release() NOEXCEPT
{
    if (!done())
        return {};

    auto out = std::move(result_);
    reset();
    return out;
}

void parser::finish(code& ec) THROWS
{
    if (ec_)
    {
        ec = ec_;
        return;
    }

    if (state_ == state::number && stack_.empty() && is_terminal(numeric_))
    {
        ec = end_number();
        if (ec)
        {
            ec_ = ec;
            return;
        }
    }

    ec = done() ? code{} : fail(error::json_incomplete);
}

size_t parser::write(const char* data, size_t size, code& ec) THROWS
{
    const auto consumed = write_some(data, size, ec);
    if (!ec && consumed < size)
        ec = fail(error::json_extra_data);

    return consumed;
}

size_t parser::write_some(const char* data, size_t size, code& ec) THROWS
{
    if (ec_)
    {
        ec = ec_;
        return zero;
    }

    ec.clear();
    size_t index{};
    while (index < size)
    {
        const auto character = data[index];
        const auto byte = to_unsigned(character);

        switch (state_)
        {
            case state::done:
            {
                if (!is_whitespace(character))
                    return index;

                ++index;
                continue;
            }
            case state::value:
            case state::element:
            {
                if (is_whitespace(character))
                {
                    ++index;
                    continue;
                }

                if (character == ']' && state_ == state::element)
                    ec = close();
                else if (character == '{')
                    ec = open(true);
                else if (character == '[')
                    ec = open(false);
                else if (character == '"')
                {
                    is_key_ = false;
                    state_ = state::text;
                }
                else if (character == '-' || is_decimal(character))
                    ec = start_number(character);
                else
                    ec = start_literal(character);

                break;
            }
            case state::member:
            case state::key:
            {
                if (is_whitespace(character))
                {
                    ++index;
                    continue;
                }

                if (character == '}' && state_ == state::member)
                    ec = close();
                else if (character == '"')
                {
                    is_key_ = true;
                    state_ = state::text;
                }
                else
                    ec = fail(error::json_syntax);

                break;
            }
            case state::colon:
            {
                if (is_whitespace(character))
                {
                    ++index;
                    continue;
                }

                if (character == ':')
                    state_ = state::value;
                else
                    ec = fail(error::json_syntax);

                break;
            }
            case state::after:
            {
                if (is_whitespace(character))
                {
                    ++index;
                    continue;
                }

                const auto is_object = stack_.back().container.is_object();
                if (character == ',')
                    state_ = is_object ? state::key : state::value;
                else if (character == (is_object ? '}' : ']'))
                    ec = close();
                else
                    ec = fail(error::json_syntax);

                break;
            }
            case state::text:
            {
                auto end = index;
                for (; end < size; ++end)
                {
                    const auto next = data[end];
                    const auto unsigned_next = to_unsigned(next);
                    if (next == '"' || next == '\\' ||
                        unsigned_next < control_end ||
                        unsigned_next >= continuation_lower)
                        break;
                }

                if (end > index)
                {
                    ec = append(&data[index], end - index);
                    if (ec)
                        break;

                    index = end;
                    continue;
                }

                if (character == '"')
                {
                    ec = end_text();
                }
                else if (character == '\\')
                {
                    state_ = state::escape;
                }
                else if (byte < control_end)
                {
                    ec = fail(error::json_syntax);
                }
                else
                {
                    if (byte >= 0xc2 && byte <= 0xdf)
                    {
                        utf8_remain_ = 1;
                        utf8_lower_ = continuation_lower;
                        utf8_upper_ = continuation_upper;
                    }
                    else if (byte == 0xe0)
                    {
                        utf8_remain_ = 2;
                        utf8_lower_ = 0xa0;
                        utf8_upper_ = continuation_upper;
                    }
                    else if (byte == 0xed)
                    {
                        utf8_remain_ = 2;
                        utf8_lower_ = continuation_lower;
                        utf8_upper_ = 0x9f;
                    }
                    else if (byte >= 0xe1 && byte <= 0xef)
                    {
                        utf8_remain_ = 2;
                        utf8_lower_ = continuation_lower;
                        utf8_upper_ = continuation_upper;
                    }
                    else if (byte == 0xf0)
                    {
                        utf8_remain_ = 3;
                        utf8_lower_ = 0x90;
                        utf8_upper_ = continuation_upper;
                    }
                    else if (byte >= 0xf1 && byte <= 0xf3)
                    {
                        utf8_remain_ = 3;
                        utf8_lower_ = continuation_lower;
                        utf8_upper_ = continuation_upper;
                    }
                    else if (byte == 0xf4)
                    {
                        utf8_remain_ = 3;
                        utf8_lower_ = continuation_lower;
                        utf8_upper_ = 0x8f;
                    }
                    else
                    {
                        ec = fail(error::json_invalid_utf8);
                        break;
                    }

                    ec = append(&data[index], one);
                    if (!ec)
                        state_ = state::utf8;
                }

                break;
            }
            case state::utf8:
            {
                if (byte < utf8_lower_ || byte > utf8_upper_)
                {
                    ec = fail(error::json_invalid_utf8);
                    break;
                }

                ec = append(&data[index], one);
                if (ec)
                    break;

                utf8_lower_ = continuation_lower;
                utf8_upper_ = continuation_upper;
                if (is_zero(--utf8_remain_))
                    state_ = state::text;

                break;
            }
            case state::escape:
            {
                char escaped{};
                switch (character)
                {
                    case '"': escaped = '"'; break;
                    case '\\': escaped = '\\'; break;
                    case '/': escaped = '/'; break;
                    case 'b': escaped = '\b'; break;
                    case 'f': escaped = '\f'; break;
                    case 'n': escaped = '\n'; break;
                    case 'r': escaped = '\r'; break;
                    case 't': escaped = '\t'; break;
                    case 'u':
                    {
                        hex_count_ = 0;
                        hex_ = 0;
                        state_ = state::unicode;
                        break;
                    }
                    default:
                    {
                        ec = fail(error::json_invalid_escape);
                        break;
                    }
                }

                if (ec || state_ == state::unicode)
                    break;

                ec = append(&escaped, one);
                if (!ec)
                    state_ = state::text;

                break;
            }
            case state::unicode:
            case state::surrogate:
            {
                const auto digit = to_hex(character);
                if (is_negative(digit))
                {
                    ec = fail(error::json_expected_hex_digit);
                    break;
                }

                hex_ = (hex_ << 4) | to_unsigned(digit);
                if (++hex_count_ < 4)
                    break;

                if (state_ == state::surrogate)
                {
                    if (hex_ < low_surrogate_begin || hex_ > low_surrogate_end)
                    {
                        ec = fail(error::json_illegal_trailing_surrogate);
                        break;
                    }

                    const auto point = surrogate_offset +
                        ((high_ - high_surrogate_begin) << 10) +
                        (hex_ - low_surrogate_begin);

                    ec = append_code_point(point);
                    if (!ec)
                        state_ = state::text;
                }
                else if (hex_ >= high_surrogate_begin &&
                    hex_ < low_surrogate_begin)
                {
                    high_ = hex_;
                    state_ = state::surrogate_slash;
                }
                else if (hex_ >= low_surrogate_begin &&
                    hex_ <= low_surrogate_end)
                {
                    ec = fail(error::json_illegal_leading_surrogate);
                }
                else
                {
                    ec = append_code_point(hex_);
                    if (!ec)
                        state_ = state::text;
                }

                break;
            }
            case state::surrogate_slash:
            {
                if (character == '\\')
                    state_ = state::surrogate_u;
                else
                    ec = fail(error::json_expected_utf16_escape);

                break;
            }
            case state::surrogate_u:
            {
                if (character == 'u')
                {
                    hex_count_ = 0;
                    hex_ = 0;
                    state_ = state::surrogate;
                }
                else
                {
                    ec = fail(error::json_expected_utf16_escape);
                }

                break;
            }
            case state::number:
            {
                ec = step_number(character);
                if (ec)
                    break;

                // The terminating character is not consumed.
                if (state_ != state::number)
                    continue;

                if (token_.size() >= options_.max_number)
                {
                    ec = fail(error::json_number_too_large);
                    break;
                }

                token_.push_back(character);
                break;
            }
            case state::literal:
            {
                if (character != literal_[matched_])
                {
                    ec = fail(error::json_syntax);
                    break;
                }

                if (++matched_ < literal_.size())
                    break;

                if (literal_.front() == 't')
                    ec = push(json::value(true));
                else if (literal_.front() == 'f')
                    ec = push(json::value(false));
                else
                    ec = push(json::value(nullptr));

                break;
            }
        }

        if (ec)
            return index;

        ++index;
    }

    return index;
}

// private
// ----------------------------------------------------------------------------

bool parser::is_whitespace(char character) NOEXCEPT
{
    return character == ' '
        || character == '\t'
        || character == '\r'
        || character == '\n';
}

bool parser::is_terminal(numeric step) NOEXCEPT
{
    return step == numeric::zero
        || step == numeric::whole
        || step == numeric::fraction
        || step == numeric::exponent_digits;
}

code parser::fail(error::json_error_t ec) NOEXCEPT
{
    ec_ = ec;
    return ec_;
}

code parser::push(json::value&& element) THROWS
{
    if (stack_.empty())
    {
        result_ = std::move(element);
        state_ = state::done;
        return {};
    }

    auto& top = stack_.back();
    if (const auto object = top.container.if_object())
    {
        if (object->size() >= options_.max_object)
            return fail(error::json_object_too_large);

        object->insert_or_assign(top.key, std::move(element));
    }
    else
    {
        auto& array = top.container.get_array();
        if (array.size() >= options_.max_array)
            return fail(error::json_array_too_large);

        array.push_back(std::move(element));
    }

    state_ = state::after;
    return {};
}

code parser::open(bool is_object) THROWS
{
    if (stack_.size() >= options_.max_depth)
        return fail(error::json_too_deep);

    if (is_object)
    {
        stack_.push_back({ json::value(json::object{}), {} });
        state_ = state::member;
    }
    else
    {
        stack_.push_back({ json::value(json::array{}), {} });
        state_ = state::element;
    }

    return {};
}

code parser::close() THROWS
{
    auto container = std::move(stack_.back().container);
    stack_.pop_back();
    return push(std::move(container));
}

code parser::append(const char* data, size_t size) THROWS
{
    const auto limit = is_key_ ? options_.max_key : options_.max_string;
    if (size > limit || token_.size() > limit - size)
        return fail(is_key_ ? error::json_key_too_large :
            error::json_string_too_large);

    token_.append(data, size);
    return {};
}

code parser::append_code_point(uint32_t point) THROWS
{
    char bytes[4]{};
    size_t size{};
    if (point < 0x80)
    {
        bytes[0] = possible_sign_narrow_cast<char>(point);
        size = 1;
    }
    else if (point < 0x800)
    {
        bytes[0] = possible_sign_narrow_cast<char>(0xc0 | (point >> 6));
        bytes[1] = possible_sign_narrow_cast<char>(0x80 | (point & 0x3f));
        size = 2;
    }
    else if (point < surrogate_offset)
    {
        bytes[0] = possible_sign_narrow_cast<char>(0xe0 | (point >> 12));
        bytes[1] = possible_sign_narrow_cast<char>(0x80 | ((point >> 6) & 0x3f));
        bytes[2] = possible_sign_narrow_cast<char>(0x80 | (point & 0x3f));
        size = 3;
    }
    else
    {
        bytes[0] = possible_sign_narrow_cast<char>(0xf0 | (point >> 18));
        bytes[1] = possible_sign_narrow_cast<char>(0x80 | ((point >> 12) & 0x3f));
        bytes[2] = possible_sign_narrow_cast<char>(0x80 | ((point >> 6) & 0x3f));
        bytes[3] = possible_sign_narrow_cast<char>(0x80 | (point & 0x3f));
        size = 4;
    }

    return append(&bytes[0], size);
}

code parser::end_text() THROWS
{
    if (is_key_)
    {
        stack_.back().key = std::move(token_);
        token_.clear();
        state_ = state::colon;
        return {};
    }

    auto text = std::move(token_);
    token_.clear();
    return push(json::value(std::move(text)));
}

code parser::start_number(char character) THROWS
{
    token_.clear();
    token_.push_back(character);
    integral_ = true;
    numeric_ = character == '-' ? numeric::sign :
        (character == '0' ? numeric::zero : numeric::whole);
    state_ = state::number;
    return {};
}

// Advance the number grammar, or end the number (state change) on a
// character that cannot continue it.
code parser::step_number(char character) THROWS
{
    const auto digit = is_decimal(character);
    const auto exponent = (character == 'e' || character == 'E');

    switch (numeric_)
    {
        case numeric::sign:
        {
            if (!digit)
                return fail(error::json_syntax);

            numeric_ = character == '0' ? numeric::zero : numeric::whole;
            return {};
        }
        case numeric::zero:
        {
            if (digit)
                return fail(error::json_syntax);

            if (character == '.')
            {
                integral_ = false;
                numeric_ = numeric::dot;
                return {};
            }

            if (exponent)
            {
                integral_ = false;
                numeric_ = numeric::exponent;
                return {};
            }

            break;
        }
        case numeric::whole:
        {
            if (digit)
                return {};

            if (character == '.')
            {
                integral_ = false;
                numeric_ = numeric::dot;
                return {};
            }

            if (exponent)
            {
                integral_ = false;
                numeric_ = numeric::exponent;
                return {};
            }

            break;
        }
        case numeric::dot:
        {
            if (!digit)
                return fail(error::json_syntax);

            numeric_ = numeric::fraction;
            return {};
        }
        case numeric::fraction:
        {
            if (digit)
                return {};

            if (exponent)
            {
                numeric_ = numeric::exponent;
                return {};
            }

            break;
        }
        case numeric::exponent:
        {
            if (character == '+' || character == '-')
            {
                numeric_ = numeric::exponent_sign;
                return {};
            }

            if (!digit)
                return fail(error::json_syntax);

            numeric_ = numeric::exponent_digits;
            return {};
        }
        case numeric::exponent_sign:
        {
            if (!digit)
                return fail(error::json_syntax);

            numeric_ = numeric::exponent_digits;
            return {};
        }
        case numeric::exponent_digits:
        {
            if (digit)
                return {};

            break;
        }
    }

    return end_number();
}

code parser::end_number() THROWS
{
    json::value number{};
    const auto ec = to_number(number);
    token_.clear();
    return ec ? ec : push(std::move(number));
}

code parser::to_number(json::value& out) NOEXCEPT
{
    const auto first = token_.data();
    const auto last = std::next(first, token_.size());

    if (integral_)
    {
        if (token_.front() == '-')
        {
            int64_t signed_{};
            const auto result = std::from_chars(first, last, signed_);
            if (result.ec == std::errc{})
            {
                out = signed_;
                return {};
            }
        }
        else
        {
            uint64_t unsigned_{};
            const auto result = std::from_chars(first, last, unsigned_);
            if (result.ec == std::errc{})
            {
                if (unsigned_ <= to_unsigned(max_int64))
                    out = to_signed(unsigned_);
                else
                    out = unsigned_;

                return {};
            }
        }
    }

    double number{};
    const auto result = from_chars(first, last, number);
    if (result.ec == std::errc::result_out_of_range)
    {
        if (!is_underflow(token_))
            return fail(error::json_exponent_overflow);

        number = token_.front() == '-' ? -0.0 : 0.0;
    }
    else if (result.ec != std::errc{} || result.ptr != last)
    {
        return fail(error::json_syntax);
    }

    out = number;
    return {};
}

code parser::start_literal(char character) NOEXCEPT
{
    if (character == 't')
        literal_ = "true";
    else if (character == 'f')
        literal_ = "false";
    else if (character == 'n')
        literal_ = "null";
    else
        return fail(error::json_syntax);

    matched_ = one;
    state_ = state::literal;
    return {};
}

// parse
// ----------------------------------------------------------------------------

value parse(std::string_view text, code& ec,
    const parse_options& options) THROWS
{
    parser reader{ options };
    reader.write(text.data(), text.size(), ec);
    if (!ec)
        reader.finish(ec);

    return ec ? value{} : reader.release();
}

value parse(std::string_view text, const parse_options& options) THROWS
{
    code ec{};
    auto out = parse(text, ec, options);
    if (ec)
        throw_error(ec);

    return out;
}

BC_POP_WARNING()
BC_POP_WARNING()

} // namespace json
} // namespace system
} // namespace libbitcoin
