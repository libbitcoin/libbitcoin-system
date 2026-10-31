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
#ifndef LIBBITCOIN_SYSTEM_UNIT_TEST_CHECKS_HPP
#define LIBBITCOIN_SYSTEM_UNIT_TEST_CHECKS_HPP

#include <sstream>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/unit_test/session.hpp>

namespace libbitcoin {
namespace system {
namespace unit_test {

// Value printing for failure text.
// ----------------------------------------------------------------------------

/// Write a value to the stream, found by argument dependent lookup only.
template <typename Type>
void print(std::ostream& stream, const Type& value) NOEXCEPT;

/// The printed value.
template <typename Type>
std::string printed(const Type& value) NOEXCEPT;

/// The text written by the writer.
template <typename Writer>
std::string format(const Writer& writer) NOEXCEPT;

// Comparisons, with the opposite operator for failure text.
// ----------------------------------------------------------------------------

struct equal
{
    static constexpr auto opposite = " != ";
    template <typename Left, typename Right>
    static bool test(const Left& left, const Right& right) NOEXCEPT;
};

struct not_equal
{
    static constexpr auto opposite = " == ";
    template <typename Left, typename Right>
    static bool test(const Left& left, const Right& right) NOEXCEPT;
};

struct less
{
    static constexpr auto opposite = " >= ";
    template <typename Left, typename Right>
    static bool test(const Left& left, const Right& right) NOEXCEPT;
};

struct less_equal
{
    static constexpr auto opposite = " > ";
    template <typename Left, typename Right>
    static bool test(const Left& left, const Right& right) NOEXCEPT;
};

struct greater
{
    static constexpr auto opposite = " <= ";
    template <typename Left, typename Right>
    static bool test(const Left& left, const Right& right) NOEXCEPT;
};

struct greater_equal
{
    static constexpr auto opposite = " < ";
    template <typename Left, typename Right>
    static bool test(const Left& left, const Right& right) NOEXCEPT;
};

// Assertions (a failed require throws aborted).
// ----------------------------------------------------------------------------

/// The failure text of an assertion of the expression.
std::string describe(level severity, const char* expression) NOEXCEPT;

/// Record a failed assertion (throws aborted for require).
void fail(level severity, const char* file, size_t line,
    const std::string& text) THROWS;

/// Assert the predicate.
void check(level severity, bool predicate, const char* file, size_t line,
    const char* expression) THROWS;

/// Assert the predicate, with failure text from the writer.
template <typename Writer>
void check_message(level severity, bool predicate, const char* file,
    size_t line, const char* expression, const Writer& writer) THROWS;

/// Assert the comparison of left and right.
template <typename Comparison, typename Left, typename Right>
void compare(level severity, const char* file, size_t line,
    const char* expression, const Left& left, const Right& right) THROWS;

/// Assert that the ranges are equal in size and element by element.
template <typename LeftIterator, typename RightIterator>
void compare_collections(level severity, const char* file, size_t line,
    const char* expression, LeftIterator left_begin, LeftIterator left_end,
    RightIterator right_begin, RightIterator right_end) THROWS;

/// Assert that left and right differ by no more than percent of each.
template <typename Left, typename Right, typename Tolerance>
void compare_close(level severity, const char* file, size_t line,
    const char* expression, const Left& left, const Right& right,
    const Tolerance& percent) THROWS;

/// Failure context text, in scope for the life of the object.
class context
{
public:
    DELETE_COPY_MOVE(context);

    context(const std::string& text) NOEXCEPT;
    ~context() NOEXCEPT;
};

} // namespace unit_test
} // namespace system
} // namespace libbitcoin

#include <bitcoin/system/impl/unit_test/checks.ipp>

#endif
