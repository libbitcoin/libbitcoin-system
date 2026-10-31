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
#ifndef LIBBITCOIN_SYSTEM_UNIT_TEST_CHECKS_IPP
#define LIBBITCOIN_SYSTEM_UNIT_TEST_CHECKS_IPP

#include <cmath>
#include <iomanip>

namespace libbitcoin {
namespace system {
namespace unit_test {

BC_PUSH_WARNING(NO_THROW_IN_NOEXCEPT)

// Value printing.
// ----------------------------------------------------------------------------

namespace printing {

// Hides enclosing operator<< declarations from unqualified lookup, so that a
// value prints only by an operator<< associated with its own type.
struct barrier
{
};

void operator<<(barrier, barrier) = delete;

template <typename Type>
void write(std::ostream& stream, const Type& value) NOEXCEPT
{
    stream << value;
}

} // namespace printing

template <typename Type>
constexpr bool is_cstring = is_same_type<std::decay_t<Type>, char*> ||
    is_same_type<std::decay_t<Type>, const char*>;

template <typename Type>
constexpr bool is_wide_cstring = is_same_type<std::decay_t<Type>, wchar_t*> ||
    is_same_type<std::decay_t<Type>, const wchar_t*>;

template <typename Type>
constexpr bool is_character = is_same_type<Type, char> ||
    is_same_type<Type, signed char> || is_same_type<Type, unsigned char>;

template <typename Type>
inline void print(std::ostream& stream, const Type& value) NOEXCEPT
{
    if constexpr (is_same_type<Type, bool>)
    {
        stream << (value ? "true" : "false");
    }
    else if constexpr (is_same_type<Type, std::nullptr_t>)
    {
        stream << "nullptr";
    }
    else if constexpr (is_character<Type>)
    {
        if (value >= ' ' && value <= '~')
            stream << '\'' << value << '\'';
        else
            stream << +value;
    }
    else if constexpr (std::is_floating_point_v<Type>)
    {
        const auto precision = stream.precision(
            std::numeric_limits<Type>::max_digits10);
        stream << value;
        stream.precision(precision);
    }
    else if constexpr (is_cstring<Type>)
    {
        if (is_null(value))
            stream << "null string";
        else
            stream << value;
    }
    else if constexpr (is_wide_cstring<Type>)
    {
        const void* address = value;
        stream << address;
    }
    else
    {
        printing::write(stream, value);
    }
}

template <typename Type>
inline std::string printed(const Type& value) NOEXCEPT
{
    std::ostringstream stream{};
    print(stream, value);
    return stream.str();
}

template <typename Writer>
inline std::string format(const Writer& writer) NOEXCEPT
{
    std::ostringstream stream{};
    writer(stream);
    return stream.str();
}

// Comparisons follow the usual arithmetic conversions of the operands.
// ----------------------------------------------------------------------------

#if defined(HAVE_GNUC) || defined(HAVE_CLANG)
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wsign-compare"
#endif
BC_PUSH_WARNING(SIGNED_UNSIGNED_COMPARE)
BC_PUSH_WARNING(SIGNED_UNSIGNED_EQUALITY)

template <typename Left, typename Right>
inline bool equal::test(const Left& left, const Right& right) NOEXCEPT
{
    if constexpr (is_cstring<Left> && is_cstring<Right>)
    {
        if (is_null(left) || is_null(right))
            return is_null(left) && is_null(right);

        return std::string_view{ left } == std::string_view{ right };
    }
    else if constexpr (is_wide_cstring<Left> && is_wide_cstring<Right>)
    {
        if (is_null(left) || is_null(right))
            return is_null(left) && is_null(right);

        return std::wstring_view{ left } == std::wstring_view{ right };
    }
    else
    {
        return left == right;
    }
}

template <typename Left, typename Right>
inline bool not_equal::test(const Left& left, const Right& right) NOEXCEPT
{
    return !equal::test(left, right);
}

template <typename Left, typename Right>
inline bool less::test(const Left& left, const Right& right) NOEXCEPT
{
    return left < right;
}

template <typename Left, typename Right>
inline bool less_equal::test(const Left& left, const Right& right) NOEXCEPT
{
    return left <= right;
}

template <typename Left, typename Right>
inline bool greater::test(const Left& left, const Right& right) NOEXCEPT
{
    return left > right;
}

template <typename Left, typename Right>
inline bool greater_equal::test(const Left& left,
    const Right& right) NOEXCEPT
{
    return left >= right;
}

BC_POP_WARNING()
BC_POP_WARNING()
#if defined(HAVE_GNUC) || defined(HAVE_CLANG)
    #pragma GCC diagnostic pop
#endif

// Assertions.
// ----------------------------------------------------------------------------

inline std::string describe(level severity, const char* expression) NOEXCEPT
{
    switch (severity)
    {
        case level::warn:
            return std::string{ "condition " } + expression +
                " is not satisfied";
        case level::check:
            return std::string{ "check " } + expression + " has failed";
        case level::require:
        default:
            return std::string{ "critical check " } + expression +
                " has failed";
    }
}

inline void fail(level severity, const char* file, size_t line,
    const std::string& text) THROWS
{
    auto& state = session::instance();
    state.failed(severity, file, line, text);
    if (severity == level::require)
    {
        state.abort();
        throw aborted{};
    }
}

inline void check(level severity, bool predicate, const char* file,
    size_t line, const char* expression) THROWS
{
    if (predicate)
        session::instance().passed(severity, file, line, expression);
    else
        fail(severity, file, line, describe(severity, expression));
}

template <typename Writer>
inline void check_message(level severity, bool predicate, const char* file,
    size_t line, const char* expression, const Writer& writer) THROWS
{
    if (predicate)
        session::instance().passed(severity, file, line, expression);
    else
        fail(severity, file, line, format(writer));
}

template <typename Comparison, typename Left, typename Right>
inline void compare(level severity, const char* file, size_t line,
    const char* expression, const Left& left, const Right& right) THROWS
{
    if (Comparison::test(left, right))
        session::instance().passed(severity, file, line, expression);
    else
        fail(severity, file, line, describe(severity, expression) + " [" +
            printed(left) + Comparison::opposite + printed(right) + "]");
}

template <typename LeftIterator, typename RightIterator>
inline void compare_collections(level severity, const char* file,
    size_t line, const char* expression, LeftIterator left_begin,
    LeftIterator left_end, RightIterator right_begin,
    RightIterator right_end) THROWS
{
    std::string mismatches{};
    size_t position{};
    for (; left_begin != left_end && right_begin != right_end;
        ++left_begin, ++right_begin, ++position)
    {
        if (!equal::test(*left_begin, *right_begin))
            mismatches += "\nMismatch at position " +
                std::to_string(position) + ": " + printed(*left_begin) +
                " != " + printed(*right_begin);
    }

    if (left_begin != left_end || right_begin != right_end)
    {
        auto left_size = position;
        auto right_size = position;
        for (; left_begin != left_end; ++left_begin) ++left_size;
        for (; right_begin != right_end; ++right_begin) ++right_size;
        mismatches += "\nCollections size mismatch: " +
            std::to_string(left_size) + " != " + std::to_string(right_size);
    }

    if (mismatches.empty())
        session::instance().passed(severity, file, line, expression);
    else
        fail(severity, file, line, describe(severity, expression) + "." +
            mismatches);
}

template <typename Left, typename Right, typename Tolerance>
inline void compare_close(level severity, const char* file, size_t line,
    const char* expression, const Left& left, const Right& right,
    const Tolerance& percent) THROWS
{
    using real = std::common_type_t<Left, Right>;
    const auto difference = std::abs(left - right);
    const auto relative = [difference](real base) NOEXCEPT -> real
    {
        if (is_zero(base))
            return is_zero(difference) ? real{} :
                std::numeric_limits<real>::max();

        return difference / std::abs(base);
    };

    const auto tolerance = percent / real{ 100 };
    const auto worst = std::max(relative(left), relative(right));
    if (worst <= tolerance)
        session::instance().passed(severity, file, line, expression);
    else
        fail(severity, file, line, describe(severity, expression) + " [" +
            printed(left) + " != " + printed(right) +
            "]. Relative difference exceeds tolerance [" + printed(worst) +
            " > " + printed(tolerance) + "]");
}

// context
// ----------------------------------------------------------------------------

inline context::context(const std::string& text) NOEXCEPT
{
    session::instance().push_context(text);
}

inline context::~context() NOEXCEPT
{
    session::instance().pop_context();
}

BC_POP_WARNING()

} // namespace unit_test
} // namespace system
} // namespace libbitcoin

#endif
