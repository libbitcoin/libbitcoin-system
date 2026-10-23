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
#include <bitcoin/system/serial/json/serializer.hpp>

#include <charconv>
#include <cmath>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/math/math.hpp>

namespace libbitcoin {
namespace system {
namespace json {

BC_PUSH_WARNING(NO_ARRAY_INDEXING)
BC_PUSH_WARNING(NO_POINTER_ARITHMETIC)

constexpr auto hex_digits = "0123456789abcdef";

// Escape character for each byte, 'u' for \u00xx and zero for none.
constexpr auto escapes = []() NOEXCEPT
{
    std::array<char, 256> table{};
    for (size_t byte{}; byte < 0x20; ++byte)
        table[byte] = 'u';

    table['\b'] = 'b';
    table['\t'] = 't';
    table['\n'] = 'n';
    table['\f'] = 'f';
    table['\r'] = 'r';
    table['"'] = '"';
    table['\\'] = '\\';
    return table;
}();

static size_t format_double(char* data, double number) NOEXCEPT
{
    if (std::isnan(number))
    {
        std::memcpy(data, "null", 4);
        return 4;
    }

    if (std::isinf(number))
    {
        const auto text = std::signbit(number) ? "-1e99999" : "1e99999";
        const auto size = std::strlen(text);
        std::memcpy(data, text, size);
        return size;
    }

    // Shortest round-trip scientific (d[.ddd]e[+-]XX) as d[.ddd]E[-]X.
    char buffer[32]{};
    const auto end = std::next(&buffer[0], sizeof(buffer));
    const auto result = std::to_chars(&buffer[0], end, number,
        std::chars_format::scientific);

    const auto exponent = std::find(&buffer[0], result.ptr, 'e');
    auto out = std::copy(&buffer[0], exponent, data);
    *out++ = 'E';

    auto it = std::next(exponent);
    if (*it == '-')
        *out++ = '-';

    ++it;
    while (std::next(it) != result.ptr && *it == '0')
        ++it;

    out = std::copy(it, result.ptr, out);
    return possible_narrow_and_sign_cast<size_t>(std::distance(data, out));
}

serializer::serializer() NOEXCEPT
{
}

serializer::~serializer() NOEXCEPT
{
}

void serializer::reset(const value* model) NOEXCEPT
{
    stack_.clear();
    model_ = model;
    text_ = {};
    texting_ = false;
    offset_ = zero;
    pending_size_ = zero;
    pending_offset_ = zero;
    started_ = false;
    done_ = is_null(model);
    is_key_ = false;
}

bool serializer::done() const NOEXCEPT
{
    return done_ || (started_ && stack_.empty() && !texting_ &&
        pending_offset_ == pending_size_);
}

std::string_view serializer::read(char* data, size_t size) THROWS
{
    auto out = data;
    const auto end = std::next(data, size);
    while (out != end)
    {
        if (pending_offset_ != pending_size_)
            out = write_pending(out, end);
        else if (texting_)
            out = write_text(out, end);
        else if (done())
            break;
        else
            next();
    }

    return { data, possible_narrow_and_sign_cast<size_t>(
        std::distance(data, out)) };
}

// private
// ----------------------------------------------------------------------------

size_t serializer::format(char* data, const value& element) NOEXCEPT
{
    const auto end = std::next(data, sizeof(pending_));
    switch (element.kind())
    {
        case kind::int64:
        {
            const auto result = std::to_chars(data, end, element.get_int64());
            return possible_narrow_and_sign_cast<size_t>(
                std::distance(data, result.ptr));
        }
        case kind::uint64:
        {
            const auto result = std::to_chars(data, end, element.get_uint64());
            return possible_narrow_and_sign_cast<size_t>(
                std::distance(data, result.ptr));
        }
        case kind::double_:
        default:
        {
            return format_double(data, element.get_double());
        }
    }
}

void serializer::pend(std::string_view text) NOEXCEPT
{
    BC_ASSERT(text.size() <= sizeof(pending_));
    std::memcpy(&pending_[0], text.data(), text.size());
    pending_size_ = text.size();
    pending_offset_ = zero;
}

void serializer::begin(const value& element) THROWS
{
    switch (element.kind())
    {
        case kind::null:
        {
            pend("null");
            return;
        }
        case kind::bool_:
        {
            pend(element.get_bool() ? "true" : "false");
            return;
        }
        case kind::int64:
        case kind::uint64:
        case kind::double_:
        {
            pending_size_ = format(&pending_[0], element);
            pending_offset_ = zero;
            return;
        }
        case kind::string:
        {
            begin_text(element.get_string(), false);
            return;
        }
        case kind::array:
        {
            if (element.get_array().empty())
            {
                pend("[]");
                return;
            }

            pend("[");
            stack_.push_back({ &element, zero, false, false });
            return;
        }
        case kind::object:
        default:
        {
            if (element.get_object().empty())
            {
                pend("{}");
                return;
            }

            pend("{");
            stack_.push_back({ &element, zero, false, false });
            return;
        }
    }
}

void serializer::begin_text(std::string_view text, bool is_key) NOEXCEPT
{
    pend("\"");
    text_ = text;
    texting_ = true;
    offset_ = zero;
    is_key_ = is_key;
}

// Emit the next token of the tree walk (comma, key, element or close).
void serializer::next() THROWS
{
    if (!started_)
    {
        started_ = true;
        begin(*model_);
        return;
    }

    auto& top = stack_.back();
    if (const auto array = top.container->if_array())
    {
        if (top.index == array->size())
        {
            pend("]");
            stack_.pop_back();
            return;
        }

        if (top.comma)
        {
            top.comma = false;
            pend(",");
            return;
        }

        const auto& element = (*array)[top.index++];
        top.comma = true;
        begin(element);
        return;
    }

    const auto& object = top.container->get_object();
    if (top.index == object.size())
    {
        pend("}");
        stack_.pop_back();
        return;
    }

    if (top.comma)
    {
        top.comma = false;
        pend(",");
        return;
    }

    // A member is visited twice, first for its key and then its value.
    const auto& member = *std::next(object.begin(), top.index);
    if (!top.keyed)
    {
        top.keyed = true;
        begin_text(member.key(), true);
        return;
    }

    top.keyed = false;
    ++top.index;
    top.comma = true;
    begin(member.value());
}

char* serializer::write_pending(char* out, const char* end) NOEXCEPT
{
    const auto space = possible_narrow_and_sign_cast<size_t>(
        std::distance<const char*>(out, end));
    const auto size = std::min(pending_size_ - pending_offset_, space);
    std::memcpy(out, &pending_[pending_offset_], size);
    pending_offset_ += size;
    return std::next(out, size);
}

char* serializer::write_text(char* out, const char* end) NOEXCEPT
{
    const auto text = text_;
    while (out != end && offset_ < text.size())
    {
        auto run = offset_;
        while (run < text.size() && is_zero(escapes[to_unsigned(text[run])]))
            ++run;

        if (run > offset_)
        {
            const auto space = possible_narrow_and_sign_cast<size_t>(
                std::distance<const char*>(out, end));
            const auto size = std::min(run - offset_, space);
            std::memcpy(out, &text[offset_], size);
            offset_ += size;
            out = std::next(out, size);
            continue;
        }

        const auto byte = to_unsigned(text[offset_++]);
        const auto escape = escapes[byte];
        pending_[0] = '\\';
        pending_[1] = escape;
        pending_size_ = two;
        pending_offset_ = zero;

        if (escape == 'u')
        {
            pending_[2] = '0';
            pending_[3] = '0';
            pending_[4] = hex_digits[byte >> 4];
            pending_[5] = hex_digits[byte & 0x0f];
            pending_size_ = 6;
        }

        return out;
    }

    if (offset_ == text.size())
    {
        texting_ = false;
        pend(is_key_ ? "\":" : "\"");
    }

    return out;
}

BC_POP_WARNING()
BC_POP_WARNING()

// serialize
// ----------------------------------------------------------------------------

string serialize(const value& model) THROWS
{
    constexpr size_t chunk = 4096;
    char buffer[chunk]{};
    serializer writer{};
    writer.reset(&model);

    string out{};
    while (!writer.done())
        out.append(writer.read(&buffer[0], chunk));

    return out;
}

std::ostream& operator<<(std::ostream& stream, const value& model) THROWS
{
    stream << serialize(model);
    return stream;
}

} // namespace json
} // namespace system
} // namespace libbitcoin
