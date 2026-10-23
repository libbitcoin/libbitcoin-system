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
#ifndef LIBBITCOIN_SYSTEM_SERIAL_JSON_PARSER_HPP
#define LIBBITCOIN_SYSTEM_SERIAL_JSON_PARSER_HPP

#include <bitcoin/system/define.hpp>
#include <bitcoin/system/error/error.hpp>
#include <bitcoin/system/serial/json/value.hpp>

namespace libbitcoin {
namespace system {
namespace json {

/// Parse limits.
struct parse_options
{
    size_t max_depth{ 32 };
    size_t max_number{ 1024 };
    size_t max_string{ max_size_t };
    size_t max_key{ max_size_t };
    size_t max_array{ max_size_t };
    size_t max_object{ max_size_t };
};

/// Incremental RFC 8259 parser of a single json text (UTF-8 validated).
/// write_some consumes through the end of the text and any whitespace that
/// follows it, leaving subsequent characters unconsumed. A top-level number
/// completes only upon a following character or finish.
class BC_API parser
{
public:
    DELETE_COPY_MOVE(parser);

    parser(const parse_options& options={}) NOEXCEPT;
    ~parser() NOEXCEPT;

    /// Discard any parse state.
    void reset() NOEXCEPT;

    /// Parse buffer, return count consumed (allocation may throw).
    size_t write_some(const char* data, size_t size, code& ec) THROWS;

    /// As write_some, but json_extra_data if not all consumed.
    size_t write(const char* data, size_t size, code& ec) THROWS;

    /// Complete the text (top-level number), json_incomplete if not done.
    void finish(code& ec) THROWS;

    /// The text is complete.
    bool done() const NOEXCEPT;

    /// Move out the parsed value and reset (null if not done).
    value release() NOEXCEPT;

private:
    enum class state : uint8_t
    {
        value,
        element,
        member,
        key,
        colon,
        after,
        text,
        escape,
        unicode,
        surrogate_slash,
        surrogate_u,
        surrogate,
        utf8,
        number,
        literal,
        done
    };

    enum class numeric : uint8_t
    {
        sign,
        zero,
        whole,
        dot,
        fraction,
        exponent,
        exponent_sign,
        exponent_digits
    };

    struct frame
    {
        json::value container;
        string key;
    };

    static bool is_whitespace(char character) NOEXCEPT;
    static bool is_terminal(numeric step) NOEXCEPT;

    code fail(error::json_error_t ec) NOEXCEPT;
    code push(json::value&& element) THROWS;
    code open(bool is_object) THROWS;
    code close() THROWS;
    code append(const char* data, size_t size) THROWS;
    code append_code_point(uint32_t point) THROWS;
    code end_text() THROWS;
    code end_number() THROWS;
    code to_number(json::value& out) NOEXCEPT;
    code start_number(char character) THROWS;
    code step_number(char character) THROWS;
    code start_literal(char character) NOEXCEPT;

    const parse_options options_;
    std::vector<frame> stack_{};
    json::value result_{};
    string token_{};
    code ec_{};
    state state_{ state::value };
    numeric numeric_{ numeric::sign };
    std::string_view literal_{};
    size_t matched_{};
    bool is_key_{};
    bool integral_{};
    uint8_t hex_count_{};
    uint32_t hex_{};
    uint32_t high_{};
    uint8_t utf8_remain_{};
    uint8_t utf8_lower_{};
    uint8_t utf8_upper_{};
};

/// Parse a complete json text.
BC_API value parse(std::string_view text, code& ec,
    const parse_options& options={}) THROWS;

/// Parse a complete json text, throws json::exception.
BC_API value parse(std::string_view text,
    const parse_options& options={}) THROWS;

} // namespace json
} // namespace system
} // namespace libbitcoin

#endif
