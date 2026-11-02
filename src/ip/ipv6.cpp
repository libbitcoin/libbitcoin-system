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
#include <bitcoin/system/ip/ipv6.hpp>

#include <charconv>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/endian/endian.hpp>
#include <bitcoin/system/ip/ipv4.hpp>
#include <bitcoin/system/math/math.hpp>

// based on:
// rfc-editor.org/rfc/rfc4291.txt
// rfc-editor.org/rfc/rfc5952.txt

namespace libbitcoin {
namespace system {
namespace ip {

constexpr auto words = ipv6::size / sizeof(uint16_t);
constexpr auto word_digits = sizeof(uint16_t) * 2u;
constexpr auto prefix_size = array_count<decltype(mapped_prefix)>;
constexpr std::string_view elision{ "::" };

BC_PUSH_WARNING(NO_ARRAY_INDEXING)
BC_PUSH_WARNING(NO_THROW_IN_NOEXCEPT)

// Append colon-separated hex words, the last of which may be dotted IPv4.
static bool parse_words(data_chunk& out, const std::string_view& text,
    bool suffix) NOEXCEPT
{
    if (text.empty())
        return true;

    size_t start{};
    while (true)
    {
        const auto colon = text.find(':', start);
        const auto last = (colon == std::string_view::npos);
        const auto piece = text.substr(start, last ? colon : colon - start);

        if (last && suffix && piece.find('.') != std::string_view::npos)
        {
            ipv4 embedded{};
            if (!ipv4::from_string(embedded, piece))
                return false;

            const auto& bytes = embedded.to_bytes();
            out.insert(out.end(), bytes.begin(), bytes.end());
            return true;
        }

        if (piece.empty() || piece.size() > word_digits)
            return false;

        uint16_t word{};
        const auto end = std::next(piece.data(), piece.size());
        const auto [next, ec] = std::from_chars(piece.data(), end, word, 16);
        if (ec != std::errc{} || next != end)
            return false;

        const auto bytes = to_big_endian(word);
        out.insert(out.end(), bytes.begin(), bytes.end());

        if (last || out.size() > ipv6::size)
            return last;

        start = add1(colon);
    }
}

// Lowercase hex without leading zeros (rfc5952 4.1, 4.3).
static std::string to_hex(uint16_t word) NOEXCEPT
{
    std::string out(word_digits, '\0');
    const auto end = std::next(out.data(), out.size());
    const auto result = std::to_chars(out.data(), end, word, 16);
    out.resize(std::distance(out.data(), result.ptr));
    return out;
}

// Statics.
// ----------------------------------------------------------------------------

bool ipv6::from_string(ipv6& out, const std::string_view& text) NOEXCEPT
{
    const auto split = text.find(elision);
    const auto elided = (split != std::string_view::npos);
    const auto head = elided ? text.substr(zero, split) : text;
    const auto tail = elided ? text.substr(split + elision.size()) :
        std::string_view{};

    if (elided && tail.find(elision) != std::string_view::npos)
        return false;

    data_chunk front{};
    data_chunk back{};
    if (!parse_words(front, head, !elided) || !parse_words(back, tail, elided))
        return false;

    // An elision replaces at least one word (rfc4291 2.2).
    const auto total = front.size() + back.size();
    if (elided ? total >= size : total != size)
        return false;

    bytes_type bytes{};
    std::copy(front.begin(), front.end(), bytes.begin());
    std::copy(back.begin(), back.end(), std::prev(bytes.end(), back.size()));
    out = { bytes };
    return true;
}

ipv6 ipv6::any() NOEXCEPT
{
    return {};
}

ipv6 ipv6::loopback() NOEXCEPT
{
    return { { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 } };
}

ipv6 ipv6::mapped(const ipv4& ip) NOEXCEPT
{
    return { splice(mapped_prefix, ip.to_bytes()) };
}

// Constructors.
// ----------------------------------------------------------------------------

ipv6::ipv6() NOEXCEPT
  : bytes_{}
{
}

ipv6::ipv6(const bytes_type& bytes) NOEXCEPT
  : bytes_{ bytes }
{
}

// Properties.
// ----------------------------------------------------------------------------

const ipv6::bytes_type& ipv6::to_bytes() const NOEXCEPT
{
    return bytes_;
}

// The longest run of two or more zero words is elided, the first of equal
// runs (rfc5952 4.2). IPv4-mapped and IPv4-compatible addresses write the low
// four bytes in dotted decimal (rfc4291 2.2, rfc5952 5).
std::string ipv6::to_string() const NOEXCEPT
{
    std_array<uint16_t, words> word{};
    for (size_t index = 0; index < words; ++index)
    {
        const auto high = bytes_[index * sizeof(uint16_t)];
        const auto low = bytes_[add1(index * sizeof(uint16_t))];
        word[index] = bit_or<uint16_t>(shift_left<uint16_t>(high, byte_bits),
            low);
    }

    const auto zeros = std::all_of(bytes_.begin(),
        std::next(bytes_.begin(), prefix_size), [](uint8_t byte) NOEXCEPT
        {
            return is_zero(byte);
        });

    const auto embedded = word[prefix_size / sizeof(uint16_t)];
    const auto mixed = is_v4_mapped() || (zeros && is_nonzero(embedded));
    const auto count = mixed ? prefix_size / sizeof(uint16_t) : words;

    size_t best{ count };
    size_t best_size{ one };
    for (size_t index = 0; index < count;)
    {
        auto end = index;
        while (end < count && is_zero(word[end]))
            ++end;

        if (end - index > best_size)
        {
            best = index;
            best_size = end - index;
        }

        index = (end == index) ? add1(index) : end;
    }

    std::string out{};
    for (size_t index = 0; index < count;)
    {
        if (index == best)
        {
            out.append(elision);
            index += best_size;
            continue;
        }

        if (!out.empty() && out.back() != ':')
            out.push_back(':');

        out.append(to_hex(word[index]));
        ++index;
    }

    if (mixed)
    {
        if (!out.empty() && out.back() != ':')
            out.push_back(':');

        out.append(to_v4().to_string());
    }

    return out;
}

ipv4 ipv6::to_v4() const NOEXCEPT
{
    ipv4::bytes_type bytes{};
    std::copy(std::prev(bytes_.end(), ipv4::size), bytes_.end(), bytes.begin());
    return { bytes };
}

bool ipv6::is_loopback() const NOEXCEPT
{
    return *this == loopback();
}

bool ipv6::is_unspecified() const NOEXCEPT
{
    return bytes_ == bytes_type{};
}

bool ipv6::is_v4_mapped() const NOEXCEPT
{
    return std::equal(mapped_prefix.begin(), mapped_prefix.end(),
        bytes_.begin());
}

BC_POP_WARNING()
BC_POP_WARNING()

// Operators.
// ----------------------------------------------------------------------------

std::ostream& operator<<(std::ostream& stream, const ipv6& ip) NOEXCEPT
{
    BC_PUSH_WARNING(NO_THROW_IN_NOEXCEPT)
    stream << ip.to_string();
    BC_POP_WARNING()
    return stream;
}

} // namespace ip
} // namespace system
} // namespace libbitcoin
