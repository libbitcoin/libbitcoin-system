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
#include <bitcoin/system/x509/pem.hpp>

#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/radix/radix.hpp>

// rfc7468 (strict, without encapsulated headers)

namespace libbitcoin {
namespace system {
namespace x509 {

constexpr std::string_view begin_prefix{ "-----BEGIN " };
constexpr std::string_view end_prefix{ "-----END " };
constexpr std::string_view suffix{ "-----" };
constexpr std::string_view whitespace{ " \t\r" };
constexpr size_t line_width = 64;
constexpr char header_separator = ':';

// The label of a boundary line with the prefix, empty if not a boundary.
static std::string_view boundary(std::string_view line,
    std::string_view prefix) NOEXCEPT
{
    const auto minimum = prefix.size() + suffix.size();
    if ((line.size() <= minimum) || !line.starts_with(prefix) ||
        !line.ends_with(suffix))
        return {};

    return line.substr(prefix.size(), line.size() - minimum);
}

std::string encode_pem(const std::string& label,
    const_byte_span data) NOEXCEPT
{
    const data_slice bytes(data.begin(), data.end());
    const auto text = encode_base64(bytes);

    std::string out{ begin_prefix };
    out.append(label).append(suffix).push_back('\n');
    for (size_t start{}; start < text.size(); start += line_width)
        out.append(text.substr(start, line_width)).push_back('\n');

    out.append(end_prefix).append(label).append(suffix).push_back('\n');
    return out;
}

bool decode_pem(pems& out, const std::string& text) NOEXCEPT
{
    pems blocks{};
    std::string label{};
    std::string body{};
    auto inside = false;
    std::string_view rest{ text };

    while (!rest.empty())
    {
        const auto end = rest.find('\n');
        auto line = rest.substr(zero, end);
        rest = (end == std::string_view::npos) ? std::string_view{} :
            rest.substr(add1(end));

        const auto last = line.find_last_not_of(whitespace);
        line = (last == std::string_view::npos) ? std::string_view{} :
            line.substr(zero, add1(last));

        if (!inside)
        {
            const auto begin = boundary(line, begin_prefix);
            if (!begin.empty())
            {
                label = begin;
                body.clear();
                inside = true;
            }

            continue;
        }

        const auto finish = boundary(line, end_prefix);
        if (!finish.empty())
        {
            data_chunk data{};
            if ((finish != label) || !decode_base64(data, body) ||
                data.empty())
                return false;

            blocks.push_back({ label, std::move(data) });
            inside = false;
            continue;
        }

        if (line.find(header_separator) != std::string_view::npos)
            return false;

        body.append(line);
    }

    if (inside)
        return false;

    out = std::move(blocks);
    return true;
}

} // namespace x509
} // namespace system
} // namespace libbitcoin
