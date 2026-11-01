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
#ifndef LIBBITCOIN_SYSTEM_CONFIG_TOKEN_HPP
#define LIBBITCOIN_SYSTEM_CONFIG_TOKEN_HPP

#include <bitcoin/system/define.hpp>

namespace libbitcoin {
namespace system {
namespace config {

/// Serialization helper for an enumeration as one of a list of words.
/// Words::words is an array of string_view in enumeration value order, where
/// an empty word is a value that is serialized as empty and is not parsed.
template <typename Enum, typename Words>
class token
{
public:
    static_assert(std::is_enum_v<Enum>);

    using type = Enum;
    static constexpr auto& words = Words::words;

    /// The word of the value, empty if none.
    static constexpr std::string_view to_word(type value) NOEXCEPT;

    /// The value of the word, false if not a parsed word.
    static constexpr bool from_word(type& out, std::string_view word) NOEXCEPT;

    /// The parsed words, delimited by '|'.
    static std::string names() NOEXCEPT;

    DEFAULT_COPY_MOVE_DESTRUCT(token);

    token() NOEXCEPT;
    token(type value) NOEXCEPT;

    /// Deserialize from a word (throws on invalid word).
    token(const std::string& value) THROWS;

    operator type() const NOEXCEPT;

    /// Deserialize from input stream (throws on invalid word).
    friend std::istream& operator>>(std::istream& input,
        token& argument) THROWS
    {
        std::string word{};
        input >> word;

        if (!from_word(argument.value_, word))
            throw istream_exception(word);

        return input;
    }

    /// Serialize to output stream.
    friend std::ostream& operator<<(std::ostream& output,
        const token& argument) NOEXCEPT
    {
        BC_PUSH_WARNING(NO_THROW_IN_NOEXCEPT)
        output << to_word(argument.value_);
        BC_POP_WARNING()
        return output;
    }

private:
    type value_;
};

} // namespace config
} // namespace system
} // namespace libbitcoin

#define TEMPLATE template <typename Enum, typename Words>
#define CLASS token<Enum, Words>

#include <bitcoin/system/impl/config/token.ipp>

#undef CLASS
#undef TEMPLATE

#endif
